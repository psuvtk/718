#include "mainwindow.h"
#include "ui_mainwindow.h"

extern bool isDispDone;
extern QMutex mutexDispDone;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("718");
    QCoreApplication::setApplicationName("SrrLED");
    qRegisterMetaType<CommThread::DeviceState>("CommThread::DeviceState");
    qRegisterMetaType<CommThread::FramePerSecond>("CommThread::FramePerMinute");

    initGui();
    _settings = new Settings(this);

    // 初始化 串口
    _portUart = new QSerialPort;
    _portData = new QSerialPort;

    // 初始化 通信线程
    _commThread = new CommThread(_portData);
    _commThread->start();

    // 初始化 绘制线程
    _plotWorker = new PlotWorker(ui->canvasRange, ui->canvasDoppler);
    _plotWorker->drawBackground();

    tryFindSerialPort();
    displaySubframeParams();

    connect(_commThread, &CommThread::frameChanged, this, &MainWindow::handleFrameChanged);
    connect(_commThread, &CommThread::deviceOpenSuccess, this, &MainWindow::handleDeviceOpenSuccess);
    connect(_commThread, &CommThread::deviceOpenFailed, this, &MainWindow::handleDeviceOpenFailed);
    connect(_commThread, &CommThread::connectionLost, this, &MainWindow::handleConnectionLost);
    connect(this, &MainWindow::dispDone, _commThread, &CommThread::handleDispDone, Qt::ConnectionType::QueuedConnection);
    connect(this, &MainWindow::deviceStateChanged, _commThread, &CommThread::handleDeviceStateChanged, Qt::ConnectionType::QueuedConnection);
    connect(this, &MainWindow::deviceOpen, _commThread, &CommThread::handleDeviceOpen, Qt::ConnectionType::QueuedConnection);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeOut);
    timer->setInterval(2500);
    timer->start();
}

MainWindow::~MainWindow()
{
    _commThread->terminate();
    _commThread->wait();
    if (_portUart->isOpen()) _portUart->close();
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
    if (_commThread->deviceState() == CommThread::PAUSE) {
        actionConnect->setEnabled(false);
        actionDisconnect->setEnabled(true);
        actionSettings->setEnabled(false);
        emit deviceStateChanged(CommThread::OPEN);
        statusBar()->showMessage("Device Status: Listening");
        return;
    }

    if (_commThread->deviceState() == CommThread::CLOSE) {
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

    emit deviceOpen();
}

void MainWindow::onActionDisconnect() {
    emit deviceStateChanged(CommThread::PAUSE);
    statusBar()->showMessage("Device Status: Paused");

    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);
    actionSettings->setEnabled(true);
//    _plotWorker->beginReplot();
//    _plotWorker->endReplot();
}

void MainWindow::onActionSettings() {
    DialogPreference dlg(_settings, this);
    if (dlg.exec()) {
        _settings->printInfo();
        emit frameRateChanged(CommThread::FramePerSecond(_settings->getFrameRate()));
    }
}

void MainWindow::onTimeOut()
{
    static double lastValue = 0;
    double cur = ui->lcdNumber->value();
    if ( (cur - lastValue) < 0.000001) {
        ui->lcdNumber->display(0);
    } else {
        lastValue = cur;
    }
}

void MainWindow::handleDeviceOpenSuccess()
{
    actionConnect->setEnabled(false);
    actionDisconnect->setEnabled(true);
    actionSettings->setEnabled(false);
    statusBar()->showMessage("Device Status: Listening");
}

void MainWindow::handleDeviceOpenFailed()
{
    QMessageBox::critical(this,
                          tr("Open Failed"),
                          tr("Failed to open DATA port\nPlease check settings!"));
}

void MainWindow::handleConnectionLost()
{
    QMessageBox::critical(this,
                          tr("Device Connection Lost"),
                          tr("Device Connection Lost!"));
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

void MainWindow::initGui()
{

    // Widgets位置尺寸设置
    auto dh = QApplication::desktop()->height() - menuBar()->height() - statusBar()->height();
    auto dw = QApplication::desktop()->width();

    ui->tabWidget->setGeometry(0, 0, dw, dh);
    ui->canvasRange->setGeometry(10, 10, dh-60, dh-60);
    ui->canvasDoppler->setGeometry(dh-30, 10, dw-dh+10, dh/2-20);
    ui->gbSpeed->setGeometry(dh-30, dh*5/8,
                             (dw-dh+20)*3/5-10, dh/4);


    ui->gbDisplay->setGeometry((dw-dh+20)*3/5+dh-30, dh*5/8,
                               (dw-dh+20)*2/5-10, dh/4);

    // 超速标签设置
    ui->labelExceedSpeed->setStyleSheet("color:green;");

    menuBar()->hide();
    setWindowTitle(tr("SrrLED"));
//    showFullScreen();
    showNormal();

    // 工具栏设置
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

    // 参数显示控件初始化
    ui->twPacketSubframe1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twPacketSubframe2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twParamSubframe1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twParamSubframe2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twPacketSubframe1->setColumnWidth(0, 280);
    ui->twPacketSubframe2->setColumnWidth(0, 280);
    ui->twParamSubframe1->setColumnWidth(0, 245);
    ui->twParamSubframe2->setColumnWidth(0, 245);
}

void MainWindow::displaySubframeParams() {
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

    QString version = QString("0x%1").arg(pSrrPacket->getVersion(), 8, 16, QChar('0'));
    QString platform = QString("0x%1").arg(pSrrPacket->getPlatform(), 8, 16, QChar('0'));
    QString totalPacketLen = QString::number(pSrrPacket->getTotalPacketLen());
    QString frameNumber = QString::number(pSrrPacket->getFrameNumber());
    QString timeCpuCycles = QString::number(pSrrPacket->getTimeCpuCycles());
    QString numDetObjs = QString::number(pSrrPacket->getNumDetectedObj());
    QString numTLVs = QString::number(pSrrPacket->getNumTLVs());
    QString subframeNumber = QString::number(pSrrPacket->getSubframeNumber());

    tw->setItem(0, 0, new QTableWidgetItem("0x0102 0x0304 0x0506 0x0708"));
    tw->setItem(0, 1, new QTableWidgetItem(version));
    tw->setItem(0, 2, new QTableWidgetItem(platform));
    tw->setItem(0, 3, new QTableWidgetItem(totalPacketLen));
    tw->setItem(0, 4, new QTableWidgetItem(frameNumber));
    tw->setItem(0, 5, new QTableWidgetItem(timeCpuCycles));
    tw->setItem(0, 6, new QTableWidgetItem(numDetObjs));
    tw->setItem(0, 7, new QTableWidgetItem(numTLVs));
    tw->setItem(0, 8, new QTableWidgetItem(subframeNumber));
}

const Tracker_t* MainWindow::extractSpeed(vector<Tracker_t> &trackers)
{
    auto lessThan = [](const Tracker_t a, const Tracker_t b){
        return a.range < b.range;
    };
    std::sort(trackers.begin(), trackers.end(), lessThan);

    for (auto it = trackers.begin(); it != trackers.end(); it++) {
        if (it->doppler < 0 && std::fabs(it->doppler) > _settings->getFilterThreshold()/3.6) {
            // m/s -> km/h
            ui->lcdNumber->display(QString::number(it->doppler * 3.6, 'g', 3));

            if (std::fabs(it->doppler) > _settings->getSpeedThreshold()/3.6) {
                ui->labelExceedSpeed->setStyleSheet("color:red;");
                ui->labelExceedSpeed->setText("Exceed");
            } else {
                ui->labelExceedSpeed->setStyleSheet("color:green;");
                ui->labelExceedSpeed->setText("Normal");
            }
            return &*it;
        }
    }

    ui->labelExceedSpeed->setStyleSheet("color:green;");
    ui->labelExceedSpeed->setText("Normal");
    return nullptr;
}

void MainWindow::handleFrameChanged(SrrPacket *pSrrPacket) {
    dispPacketDetail(pSrrPacket);
    const Tracker_t *it = nullptr;
    if (pSrrPacket->getFrameNumber()%2 == 1) {
        it = extractSpeed(pSrrPacket->getTackers());
    }

    _plotWorker->beginReplot();
    if (it != nullptr) _plotWorker->drawTarget(*it);
    _plotWorker->drawDetObj(pSrrPacket->getDetObjs(), pSrrPacket->getSubframeNumber());
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
    if (checked) {
        statusBar()->showMessage("Near View Enabled", 1000);
    } else {
        statusBar()->showMessage("Near View Disabled", 1000);
    }
}

void MainWindow::on_cbSrrdDetObj_toggled(bool checked)
{
    _plotWorker->setEnableSrrDetObj(checked);
    if (checked) {
        statusBar()->showMessage("Srr Enabled", 1000);
    } else {
        statusBar()->showMessage("Srr Disabled", 1000);
    }
}

void MainWindow::on_cbUsrrDetObj_toggled(bool checked)
{
    _plotWorker->setEnableUsrrDetObj(checked);
    if (checked) {
        statusBar()->showMessage("USRR Enabled", 1000);
    } else {
        statusBar()->showMessage("USRR Disabled", 1000);
    }
}

void MainWindow::on_cbCluster_toggled(bool checked)
{
    _plotWorker->setEnableClusters(checked);
    if (checked) {
        statusBar()->showMessage("Clusters View Enabled", 1000);
    } else {
        statusBar()->showMessage("Clusters View Disabled", 1000);
    }
}

void MainWindow::on_cbTrackers_toggled(bool checked)
{
    _plotWorker->setEnableTracker(checked);
    if (checked) {
        statusBar()->showMessage("Near View Enabled", 1000);
    } else {
        statusBar()->showMessage("Near View Disabled", 1000);
    }
}

void MainWindow::on_cbParkingAssisBins_toggled(bool checked)
{
    _plotWorker->setEnableParkingAssitBins(checked);
    if (checked) {
        statusBar()->showMessage("Parking Assit Bins Enabled", 1000);
    } else {
        statusBar()->showMessage("Parking Assit Bins Disabled", 1000);
    }
}
