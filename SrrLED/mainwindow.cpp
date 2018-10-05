#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("718");
    QCoreApplication::setApplicationName("SrrLED");

    _settings = new Settings(this);
    // 初始化 通信线程
    _commThread = new CommThread;
    connect(_commThread, &CommThread::frameChanged, this, &MainWindow::onFrameChanged);
    connect(this, &MainWindow::dispDone, _commThread, &CommThread::onDispDone);

    // 初始化 绘制线程
    _plotWorker = new PlotWorker(ui->customPlot);
    _plotWorker->drawBackground();

    // 初始化 串口
    _portUart = new QSerialPort;
    _portData = new QSerialPort;
    _deviceState = CLOSE;
    _commThread->setHandle(_portData);
    _commThread->start();

    tryFindSerialPort();
    displaySubframeParams();



    // GUI 设置
    menuBar()->hide();
    setWindowTitle(tr("SrrLED"));
//    auto dw = QApplication::desktop()->width();
//    auto dh = QApplication::desktop()->height();
//    setGeometry(0, 0, dw, dh);
//    qDebug() << dw << " " << dh;

    ui->twPacketSubframe1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twPacketSubframe2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twParamSubframe1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twParamSubframe2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twPacketSubframe1->setColumnWidth(0, 280);
    ui->twPacketSubframe2->setColumnWidth(0, 280);
    ui->twParamSubframe1->setColumnWidth(0, 245);
    ui->twParamSubframe2->setColumnWidth(0, 245);
    // 打开配置文件
    //    QAction *actionOpenConfig = new QAction(QIcon(":/icons/folder.svg"),
    //                                            tr("Open Config File"),
    //                                            this);
    //    ui->mainToolBar->addAction(actionOpenConfig);
    //    connect(actionOpenConfig, &QAction::triggered, this, &MainWindow::onActionOpenConfig);
    //    ui->mainToolBar->addSeparator();

    actionConnect = new QAction(QIcon(":/icons/connect.svg"),
                                    tr("connect"),
                                    this);
    ui->mainToolBar->addAction(actionConnect);
    connect(actionConnect, &QAction::triggered, this, &MainWindow::onActionConnect);

    actionDisconnect = new QAction(QIcon(":/icons/disconnect.svg"),
                                   tr("disconnect"),
                                   this);
    actionDisconnect->setEnabled(false);
    ui->mainToolBar->addAction(actionDisconnect);
    connect(actionDisconnect, &QAction::triggered, this, &MainWindow::onActionDisconnect);

    ui->mainToolBar->addSeparator();

    actionSettings = new QAction(QIcon(":/icons/settings.svg"),
                                 tr("option"),
                                 this);
    ui->mainToolBar->addAction(actionSettings);
    connect(actionSettings, &QAction::triggered, this, &MainWindow::onActionSettings);


//    testPlot();
}

MainWindow::~MainWindow()
{
    _commThread->terminate();
    _commThread->wait();
    if (_portData->isOpen()) _portData->close();

    delete _commThread;
    delete _plotWorker;
    delete _portUart;
    delete _portData;
    delete ui;
    delete _settings;
}


void MainWindow::showAboutMessage() {
   qDebug() << "MainWindow::showAboutMessage()";
}




void MainWindow::tryFindSerialPort() {
#ifdef Q_OS_LINUX
    _settings->setPortNameUartPort("/dev/ttyACM0");
    _settings->setPortNameDataPort("/dev/ttyACM1");

    _settings->setBaudRateUartPort(115200);
    _settings->setBaudRateDataPort(921600);
#else
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (auto it = ports.begin(); it != ports.end();) {
        if ((*it).description().contains("XDS")) ++it;
        else it = ports.erase(it);
    }
    for (auto port: ports) {
        qDebug() << port.description();
    }

    if (ports.length() >= 2) {
        // 取前两个串口
        for (int i = 0; i < 2; i++) {
            if (ports[i].description().contains("UART"))
                _settings->setPortNameUartPort(ports[i].portName());
            else
                _settings->setPortNameDataPort(ports[i].portName());
        }
    } else {
        _settings->setPortNameUartPort("");
        _settings->setPortNameDataPort("");
    }

    _settings->setBaudRateUartPort(115200);
    _settings->setBaudRateDataPort(921600);
#endif
}



void MainWindow::onActionConnect() {
    if (_deviceState == CLOSE) {

        if (sensorStart()) {
            qDebug() << "Success to start sensor.";
        } else {
            qDebug() << "Failed to start sensor.";
            QMessageBox::critical(this,
                                  tr("Sensor start failed"),
                                  tr("Failed to start sensor\nCheck your settings and retry!"));
            return;
        }

        // 打开数据串口
        if (_settings->getPortNameDataPort() == "") {
            qDebug() << "Empty DataPort Name";
            tryFindSerialPort();
            if (_settings->getPortNameDataPort() == "") {
                QMessageBox::critical(this,
                                      tr("Open DATA Failed"),
                                      tr("Failed to open DATA port\nPlease check settings!"));
                return;
            }
        }

        _portData->setPortName(_settings->getPortNameDataPort());
        _portData->setBaudRate(921600);
        _portData->setDataBits(QSerialPort::Data8);
        _portData->setParity(QSerialPort::NoParity);
        _portData->setStopBits(QSerialPort::OneStop);
        _portData->setFlowControl(QSerialPort::NoFlowControl);

    }

    if (_portData->open(QIODevice::ReadOnly)) {
        actionConnect->setEnabled(false);
        actionDisconnect->setEnabled(true);
        actionSettings->setEnabled(false);
        return;
    } else {
        QMessageBox::critical(this,
                              tr("Open Failed"),
                              tr("Failed to open DATA port\nPlease check settings!"));
        return;
    }
}

void MainWindow::onActionDisconnect() {
    if (_portData->isOpen()) {
        _portData->close();
        _deviceState = PAUSE;
    }

    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);
    actionSettings->setEnabled(true);
}

void MainWindow::onActionSettings() {
    DialogPreference dlg(_settings, this);
    if (dlg.exec()) {
        _settings->printInfo();
    }
}

bool MainWindow::sensorStart() {
    if (_settings->getPortNameUartPort() == "") {
        tryFindSerialPort();
        if (_settings->getPortNameUartPort() == "") {
            qDebug() << "Empty UartPort Name";
            return false;
        }
    }

    _portUart->setPortName(_settings->getPortNameUartPort());
    _portUart->setBaudRate(QSerialPort::Baud115200);
    _portUart->setDataBits(QSerialPort::Data8);
    _portUart->setParity(QSerialPort::NoParity);
    _portUart->setStopBits(QSerialPort::OneStop);
    _portUart->setFlowControl(QSerialPort::NoFlowControl);

    if (!_portUart->open(QIODevice::ReadWrite))
        return false;

    // 正式发送CLI命令
    QStringList cmds;
    QByteArray readBuf;
    quint32 nErr = 0;

    cmds.append("advFrameCfg\n");
    cmds.append("sensorStart\n");
    for (QString cmd: cmds) {
retry:
        readBuf.clear();
        _portUart->write(cmd.toLatin1());
        // 等待全部返回数据，10ms超时
        while (_portUart->waitForReadyRead(10)) {
            QThread::msleep(20);
            readBuf.append(_portUart->readAll());
        }


        qDebug() << "Send Command => " << cmd;
        qDebug() << "Received: " << readBuf;

        if (QString(readBuf).contains("Done")) {
            nErr = 0;
            continue;
        }

        if (QString(readBuf).contains("Error")) {
            qDebug() << "\nSend Command ["
                     << cmd << "] Faild!";
            _portUart->close();
            return false;
        }

        if (nErr++ < 2 ) {
            QThread::msleep(10);
            readBuf.clear();
            goto retry;
        } else {
            qDebug() << "\nSend Command ["
                     << cmd << "] Faild!";
            _portUart->close();
            return false;
        }
    }

    _portUart->close();
    return true;
}

void MainWindow::testPlot()
{
    _plotWorker->beginReplot();




    // Cluster 框
    vector<Cluster_t> clusters{Cluster_t{ 8, 14,1.5,1.5}};
//    clusters << Cluster_t{ 8, 14,1.5,1.5};
    _plotWorker->drawClusters(clusters);

    _plotWorker->endReplot();
}


void MainWindow::displaySubframeParams() {
    qDebug() << "Not impl displaySubframeParams";
    ui->twParamSubframe1->setColumnWidth(0, 190);
    ui->twParamSubframe2->setColumnWidth(0, 190);
    ui->twParamSubframe1->setItem(0, 0, new QTableWidgetItem("76"));
    ui->twParamSubframe1->setItem(0, 1, new QTableWidgetItem("0.4096"));
    ui->twParamSubframe1->setItem(0, 2, new QTableWidgetItem("0.3662"));
    ui->twParamSubframe1->setItem(0, 3, new QTableWidgetItem("0.5227"));
    ui->twParamSubframe1->setItem(0, 4, new QTableWidgetItem("1"));

    ui->twParamSubframe2->setItem(0, 0, new QTableWidgetItem("77.0"));
    ui->twParamSubframe2->setItem(0, 1, new QTableWidgetItem("3.4561"));
    ui->twParamSubframe2->setItem(0, 2, new QTableWidgetItem("0.434"));
    ui->twParamSubframe2->setItem(0, 3, new QTableWidgetItem("0.3229"));
    ui->twParamSubframe2->setItem(0, 4, new QTableWidgetItem("2"));

}

void MainWindow::dispPacketDetail(SrrPacket *pSrrPacket)
{
    QTableWidget *tw;
    if (pSrrPacket->getSubframeNumber() == 0) {
        tw = ui->twPacketSubframe1;
    } else {
        tw = ui->twPacketSubframe2;
    }

    QString version = QString("0x%1").arg(pSrrPacket->getVersion(),8, 16,QChar('0'));
    QString platform = QString("0x%1").arg(pSrrPacket->getPlatform(), 8, 16, QChar('0'));
    QString totalPacketLen = QString::number(pSrrPacket->getTotalPacketLen());
    QString frameNumber = QString::number(pSrrPacket->getFrameNumber());
    QString timeCpuCycles = QString::number(pSrrPacket->getTimeCpuCycles());
    QString numDetObjs = QString::number(pSrrPacket->getNumDetectedObj());
    QString numTLVs = QString::number(pSrrPacket->getNumTLVs());
    QString subframeNumber = QString::number(pSrrPacket->getSubframeNumber());
    QString detObjs = QString::number(pSrrPacket->getDetObjs().size());
    QString clusters = QString::number(pSrrPacket->getClusters().size());
    QString trackers = QString::number(pSrrPacket->getTackers().size());
    QString parkingAssitBins = QString::number(pSrrPacket->getParkingAssistBins().size());

    tw->setItem(0, 0, new QTableWidgetItem("0x0102 0x0304 0x0506 0x0708"));
    tw->setItem(0, 1, new QTableWidgetItem(version));
    tw->setItem(0, 2, new QTableWidgetItem(platform));
    tw->setItem(0, 3, new QTableWidgetItem(totalPacketLen));
    tw->setItem(0, 4, new QTableWidgetItem(frameNumber));
    tw->setItem(0, 5, new QTableWidgetItem(timeCpuCycles));
    tw->setItem(0, 6, new QTableWidgetItem(numDetObjs));
    tw->setItem(0, 7, new QTableWidgetItem(numTLVs));
    tw->setItem(0, 8, new QTableWidgetItem(subframeNumber));
    tw->setItem(0, 9, new QTableWidgetItem(""));
    tw->setItem(0, 10, new QTableWidgetItem(detObjs));
    tw->setItem(0, 11, new QTableWidgetItem(clusters));
    tw->setItem(0, 12, new QTableWidgetItem(trackers));
    tw->setItem(0, 13, new QTableWidgetItem(parkingAssitBins));
}

void MainWindow::dispSpeed(vector<Tracker_t> &trackers)
{
    qSort(trackers.begin(), trackers.end(), [](const Tracker_t a, const Tracker_t b){
        return (a.x * a.x + a.y * a.y) < (b.x * b.x + b.y * b.y);
    });

    for (auto t: trackers) {
        if (t.vy > 0) {
            double speed = t.vx * t.vx + t.vy * t.vy;
            ui->lcdNumber->display(QString::number(speed));
            return;
        }
    }
    ui->lcdNumber->display("0");
}

void MainWindow::onFrameChanged(SrrPacket *pSrrPacket) {
    dispPacketDetail(pSrrPacket);
    dispSpeed(pSrrPacket->getTackers());

    _plotWorker->beginReplot();
    if (pSrrPacket->getSubframeNumber() == 0) { // SRR
        _plotWorker->drawSrrDetObj(pSrrPacket->getDetObjs());
    } else { // USRR
        _plotWorker->drawUsrrDetObjs(pSrrPacket->getDetObjs());
    }
    _plotWorker->drawClusters(pSrrPacket->getClusters());
    _plotWorker->drawTrackers(pSrrPacket->getTackers());
    _plotWorker->drawParkingAssitBins(pSrrPacket->getParkingAssistBins());
    _plotWorker->endReplot();

    emit dispDone();
}


void MainWindow::on_cbNearView_toggled(bool checked)
{
    _plotWorker->setEnableNearView(checked);
    _plotWorker->drawBackground();
}

void MainWindow::on_cbSrrdDetObj_toggled(bool checked)
{
    _plotWorker->setEnableSrrDetObj(checked);
}

void MainWindow::on_cbUsrrDetObj_toggled(bool checked)
{
    _plotWorker->setEnableUsrrDetObj(checked);
}

void MainWindow::on_cbCluster_toggled(bool checked)
{
    _plotWorker->setEnableClusters(checked);
}

void MainWindow::on_cbTrackers_toggled(bool checked)
{
    _plotWorker->setEnableTracker(checked);
}

void MainWindow::on_cbParkingAssisBins_toggled(bool checked)
{
    _plotWorker->setEnableParkingAssitBins(checked);
}
