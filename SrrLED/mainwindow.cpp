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
    qRegisterMetaType<CommThread::FramePerSecond>("CommThread::FramePerSecond");

    _settings = new Settings(this);
    _portUart = new QSerialPort;
    _portData = new QSerialPort;
    _commThread = new CommThread(_portData);

    // 初始化 绘制线程
    _plotWorker = new PlotWorker(ui->canvasRange, ui->canvasDoppler);

    connect(_commThread, &CommThread::frameChanged, this, &MainWindow::handleFrameChanged);
    connect(_commThread, &CommThread::deviceOpenSuccess, this, &MainWindow::handleDeviceOpenSuccess);
    connect(_commThread, &CommThread::deviceOpenFailed, this, &MainWindow::handleDeviceOpenFailed);
    connect(_commThread, &CommThread::connectionLost, this, &MainWindow::handleConnectionLost);
    connect(this, &MainWindow::dispDone, _commThread, &CommThread::handleDispDone, Qt::ConnectionType::QueuedConnection);
    connect(this, &MainWindow::deviceStateChanged, _commThread, &CommThread::handleDeviceStateChanged, Qt::ConnectionType::QueuedConnection);
    connect(this, &MainWindow::deviceOpen, _commThread, &CommThread::handleDeviceOpen, Qt::ConnectionType::QueuedConnection);
    connect(this, &MainWindow::frameRateChanged, _commThread, &CommThread::handleFrameRateChanged, Qt::ConnectionType::QueuedConnection);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeOut);
    timer->setInterval(2500);
    timer->start();

    initGui();
    populateLocalSetting();
    tryFindSerialPort();
    displaySubframeParams();

    _plotWorker->drawBackground();
    _commThread->start();

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
    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);
    actionSettings->setEnabled(true);
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
    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(ui->canvasRange, 0, 0,18,18);
    mainLayout->addWidget(ui->canvasDoppler, 0, 18,7,14);
    mainLayout->addWidget(ui->groupBox_3, 8, 19, 5, 6);
    mainLayout->addWidget(ui->groupBox_4, 8, 25, 5, 6);
    mainLayout->addWidget(ui->gbSpeed, 14,19,3,6);
    mainLayout->addWidget(ui->gbDisplay, 14,25,3,6);
    centralWidget()->setLayout(mainLayout);

    // 超速标签设置
    ui->labelExceedSpeed->setStyleSheet("color:green;");

    menuBar()->hide();
    setWindowTitle(tr("SrrLED"));
    setWindowIcon(QIcon(":/icons/oset.png"));

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

    QAction *actionAbout = new QAction(QIcon(":/icons/toy.ico"),
                                       tr("about"),
                                       this);
    ui->mainToolBar->addAction(actionAbout);
    connect(actionAbout, &QAction::triggered, this, [=](){

                QMessageBox::about(this,
                           "About SrrLED",
                           tr("<h1>SrrLED</h1>"
                              "<h3>Reseach Center of Electronics & Infomation Equipment for Ocean& Aerospace<br/></h3>"
                              "Software Version: v0.04<br/>"
                              "Qt Version: v5.11.2<br/>"
                              "<h5>Developed By Kristoffer</h5>"));
    });

    // 参数显示控件初始化
    QString detailTmpl = "<style type=\"text/css\">"
                         ".tg  {border-collapse:collapse;border-spacing:0;border-color:#999;}"
                         ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#444;background-color:#F7FDFA;}"
                         ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#fff;background-color:#26ADE4;}"
                         ".tg .tg-c3ow{border-color:inherit;text-align:center;vertical-align:top}"
                         ".tg .tg-lqy6{text-align:right;vertical-align:top}"
                         ".tg .tg-0pky{border-color:inherit;text-align:left;vertical-align:top}"
                         ".tg .tg-dvpl{border-color:inherit;text-align:right;vertical-align:top}"
                         ".tg .tg-0lax{text-align:left;vertical-align:top}"
                         "</style>"
                        "<table class=\"tg\">"
                        "<tr><th class=\"tg-c3ow\" colspan=\"2\">Subframe #%1</th></tr>"
                        "<tr><td class=\"tg-0pky\">Version</td><td class=\"tg-dvpl\"></td></tr>"
                        "<tr><td class=\"tg-0pky\">Platform</td><td class=\"tg-dvpl\"></td></tr>"
                        "<tr><td class=\"tg-0pky\">TotalPacketLen</td><td class=\"tg-dvpl\"></td></tr>"
                        "<tr> <td class=\"tg-0pky\">FrameNumber</td><td class=\"tg-dvpl\"></td></tr>"
                        "<tr><td class=\"tg-0pky\">TimeCpuCycles</td><td class=\"tg-dvpl\"></td></tr>"
                        "<tr><td class=\"tg-0lax\">NumDetectedObjs</td><td class=\"tg-lqy6\"></td></tr>"
                        "<tr><td class=\"tg-0lax\">NumTLVs</td><td class=\"tg-lqy6\"></td></tr>";
    ui->textBrowserFrame1Detail->setHtml(detailTmpl.arg("SRR"));
    ui->textBrowserFrame2Detail->setHtml(detailTmpl.arg("USRR"));
}

void MainWindow::populateLocalSetting()
{
    if (_settings->getFullscreenOnStartup()) {
        showFullScreen();
    } else {
        showMaximized();
    }

    emit frameRateChanged(CommThread::FramePerSecond(_settings->getFrameRate()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        if (_isFullScreen) {
            _isFullScreen = false;
            showMaximized();
        } else {
            _isFullScreen = true;
            showFullScreen();
        }
        return;
    }

    if (event->key() == Qt::Key_Space) {
        if (actionConnect->isEnabled())
            onActionConnect();
    }
}

void MainWindow::displaySubframeParams() {
    ui->textBrowserFrame1Params->setHtml("<style type=\"text/css\">"
                                       ".tg  {border-collapse:collapse;border-spacing:0;border-color:#999;}"
                                       ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#444;background-color:#F7FDFA;}"
                                       ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#fff;background-color:#26ADE4;}"
                                       ".tg .tg-c3ow{border-color:inherit;text-align:center;vertical-align:top}"
                                       ".tg .tg-0pky{border-color:inherit;text-align:left;vertical-align:top}"
                                       ".tg .tg-dvpl{border-color:inherit;text-align:right;vertical-align:top}"
                                       "</style>"
                                       "<table class=\"tg\"><tr><th class=\"tg-c3ow\" colspan=\"2\">Subframe #1</th></tr>"
                                       "<tr><td class=\"tg-0pky\">Start Frequency(GHz)</td><td class=\"tg-dvpl\">76</td></tr>"
                                       "<tr><td class=\"tg-0pky\">BandWidth(GHz)</td><td class=\"tg-dvpl\">0.4096</td></tr>"
                                       "<tr><td class=\"tg-0pky\">Range Resolution(m)</td><td class=\"tg-dvpl\">0.3662</td></tr>"
                                       "<tr><td class=\"tg-0pky\">Velosity Resolution(m/s)</td><td class=\"tg-dvpl\">0.5227</td></tr>"
                                       "<tr><td class=\"tg-0pky\">Number of Tx(MIMO)</td><td class=\"tg-dvpl\">1</td></tr></table>");
    ui->textBrowserFrame2Params->setHtml("<style type=\"text/css\">"
                                         ".tg  {border-collapse:collapse;border-spacing:0;border-color:#999;}"
                                         ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#444;background-color:#F7FDFA;}"
                                         ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#fff;background-color:#26ADE4;}"
                                         ".tg .tg-c3ow{border-color:inherit;text-align:center;vertical-align:top}"
                                         ".tg .tg-0pky{border-color:inherit;text-align:left;vertical-align:top}"
                                         ".tg .tg-dvpl{border-color:inherit;text-align:right;vertical-align:top}"
                                         "</style>"
                                         "<table class=\"tg\"><tr><th class=\"tg-c3ow\" colspan=\"2\">Subframe #2</th></tr>"
                                         "<tr><td class=\"tg-0pky\">Start Frequency(GHz)</td><td class=\"tg-dvpl\">76</td></tr>"
                                         "<tr><td class=\"tg-0pky\">BandWidth(GHz)</td><td class=\"tg-dvpl\">3.4561</td></tr>"
                                         "<tr><td class=\"tg-0pky\">Range Resolution(m)</td><td class=\"tg-dvpl\">0.0434</td></tr>"
                                         "<tr><td class=\"tg-0pky\">Velosity Resolution(m/s)</td><td class=\"tg-dvpl\">0.3229</td></tr>"
                                         "<tr><td class=\"tg-0pky\">Number of Tx(MIMO)</td><td class=\"tg-dvpl\">2</td></tr></table>");
}

void MainWindow::dispPacketDetail(SrrPacket *pSrrPacket)
{
    QString detailTmpl = "<style type=\"text/css\">"
                         ".tg  {border-collapse:collapse;border-spacing:0;border-color:#999;}"
                         ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#444;background-color:#F7FDFA;}"
                         ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#fff;background-color:#26ADE4;}"
                         ".tg .tg-c3ow{border-color:inherit;text-align:center;vertical-align:top}"
                         ".tg .tg-lqy6{text-align:right;vertical-align:top}"
                         ".tg .tg-0pky{border-color:inherit;text-align:left;vertical-align:top}"
                         ".tg .tg-dvpl{border-color:inherit;text-align:right;vertical-align:top}"
                         ".tg .tg-0lax{text-align:left;vertical-align:top}"
                         "</style>"
                        "<table class=\"tg\">"
                        "<tr><th class=\"tg-c3ow\" colspan=\"2\">Subframe #%9</th></tr>"
                        "<tr><td class=\"tg-0pky\">Version</td><td class=\"tg-dvpl\">0x%1</td></tr>"
                        "<tr><td class=\"tg-0pky\">Platform</td><td class=\"tg-dvpl\">0x%2</td></tr>"
                        "<tr><td class=\"tg-0pky\">TotalPacketLen</td><td class=\"tg-dvpl\">%3</td></tr>"
                        "<tr> <td class=\"tg-0pky\">FrameNumber</td><td class=\"tg-dvpl\">%4</td></tr>"
                        "<tr><td class=\"tg-0pky\">TimeCpuCycles</td><td class=\"tg-dvpl\">%5</td></tr>"
                        "<tr><td class=\"tg-0lax\">NumDetectedObjs</td><td class=\"tg-lqy6\">%6</td></tr>"
                        "<tr><td class=\"tg-0lax\">NumTLVs</td><td class=\"tg-lqy6\">%7</td></tr>"
//                        "<tr><td class=\"tg-0lax\">Subframe Number</td><td class=\"tg-lqy6\">%8</td></tr>"
                        "</table>";
    QString detail = detailTmpl.arg(pSrrPacket->getVersion(), 8, 16, QChar('0'))
            .arg(pSrrPacket->getPlatform(), 8, 16, QChar('0'))
            .arg(pSrrPacket->getTotalPacketLen())
            .arg(pSrrPacket->getFrameNumber())
            .arg(pSrrPacket->getTimeCpuCycles())
            .arg(pSrrPacket->getNumDetectedObj())
            .arg(pSrrPacket->getNumTLVs())
//            .arg(pSrrPacket->getSubframeNumber())
            ;

    if (pSrrPacket->getSubframeNumber() == 0) {
        //srr
        ui->textBrowserFrame1Detail->setHtml(detail.arg("SRR"));
    } else {
        // usrr
        ui->textBrowserFrame2Detail->setHtml(detail.arg("USRR"));
    }
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
