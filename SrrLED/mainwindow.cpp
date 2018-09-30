#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogpreference.h"
#include <QDesktopWidget>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("718");
    QCoreApplication::setApplicationName("SrrLED");

    _settings = new Settings(this);
    _commThread = new UartThread;
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
    auto w = ui->tabWidget->width();
    auto h = ui->tabWidget->height() + statusBar()->height()
             + ui->mainToolBar->height();
    auto dw = QApplication::desktop()->width();
    auto dh = QApplication::desktop()->height();
    setGeometry((dw-w)/2, (dh-h)/2, w, h);
    qDebug() << dw << " " << dh;

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
}

MainWindow::~MainWindow()
{
    _commThread->terminate();
    _commThread->wait();
    if (_portData->isOpen()) _portData->close();

    delete _commThread;
    delete _portUart;
    delete _portData;
    delete ui;
    delete _settings;
}

void MainWindow::onSpeedChanged() {

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
