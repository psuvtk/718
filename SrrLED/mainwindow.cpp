#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogpreference.h"
#include <QDesktopWidget>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("718");
    QCoreApplication::setApplicationName("SrrLED");

    _settings = new Settings(this);
    _uartReadThread = new UartThread;
    _portUart = new QSerialPort;
    _portData = new QSerialPort;

    serialPortConfig();
    menuBar()->hide();
    auto w = ui->tabWidget->width();
    auto h = ui->tabWidget->height() + statusBar()->height()
             + ui->mainToolBar->height();
    auto dw = QApplication::desktop()->width();
    auto dh = QApplication::desktop()->height();
    setGeometry((dw-w)/2, (dh-h)/2, w, h);
    qDebug() << dw << " " <<dh;

    // 打开配置文件
//    QAction *actionOpenConfig = new QAction(QIcon(":/icons/folder.svg"),
//                                            tr("Open Config File"),
//                                            this);
//    ui->mainToolBar->addAction(actionOpenConfig);
//    connect(actionOpenConfig, &QAction::triggered, this, &MainWindow::onActionOpenConfig);
//    ui->mainToolBar->addSeparator();

    actionStartSensor = new QAction(QIcon(":/icons/connect.svg"),
                                    tr("connect"),
                                    this);
    ui->mainToolBar->addAction(actionStartSensor);
    connect(actionStartSensor, &QAction::triggered, this, &MainWindow::onActionSensorStart);

    actionStopSensor = new QAction(QIcon(":/icons/disconnect.svg"),
                                   tr("disconnect"),
                                   this);
    actionStopSensor->setEnabled(false);
    ui->mainToolBar->addAction(actionStopSensor);
    connect(actionStopSensor, &QAction::triggered, this, &MainWindow::onActionSensorStop);

    ui->mainToolBar->addSeparator();

    actionSettings = new QAction(QIcon(":/icons/settings.svg"),
                                 tr("option"),
                                 this);
    ui->mainToolBar->addAction(actionSettings);
    connect(actionSettings, &QAction::triggered, this, &MainWindow::onActionSettings);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete _settings;
}

void MainWindow::onSpeedChanged() {

}

void MainWindow::showAboutMessage() {
   qDebug() << "MainWindow::showAboutMessage()";
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QMainWindow::keyPressEvent(event);
}



void MainWindow::serialPortConfig() {
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

void MainWindow::onActionOpenConfig() {
    qDebug() << "TODO: impl open config";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Config File"),
                                                    QDir::homePath(),
                                                    "Config (*.cfg)");
    _settings->setConfigFilePath(fileName);
}

void MainWindow::onActionSensorStart() {
    if (_settings->getPortNameUartPort() == "") {
        qDebug() << "Empty UartPort Name";
        QMessageBox::critical(this,
                              tr("Open UART Failed"),
                              tr("Open UART Port Failed\nPlease Check Settings!"));
        return;
    }

    _portUart->setPortName(_settings->getPortNameUartPort());
    _portUart->setBaudRate(QSerialPort::Baud115200);
    _portUart->setDataBits(QSerialPort::Data8);
    _portUart->setParity(QSerialPort::NoParity);
    _portUart->setStopBits(QSerialPort::OneStop);
    _portUart->setFlowControl(QSerialPort::NoFlowControl);

    QStringList cmds;
    QByteArray readBuf;
    cmds.append("advFrameCfg");
    cmds.append("sensorStart");
    if (_portUart->open(QIODevice::ReadWrite | QIODevice::Text)) {
        for (QString cmd: cmds) {
            _portUart->write(cmd.toLatin1());
            _portUart->write("\n");
            _portUart->waitForReadyRead(300);
            QThread::msleep(10);
            readBuf = _portUart->readAll();
            if (QString(readBuf).contains("Done")) {
                qDebug() << "Send Command => "
                         << cmd;
            } else {
                qDebug() << "\nSend Command ["
                         << cmd << "] Faild!";
                QMessageBox::critical(this,
                                      tr("Send Command Failed"),
                                      tr("Not Recogized as a CLI Command"));
                _portUart->close();
                return;
            }
        }
        _portUart->close();
    } else {
        qDebug() << "Open UART Port Failed.";
        QMessageBox::critical(this,
                              tr("Open UART Failed"),
                              tr("Failed to open UART port"));
        return;
    }


    // 打开数据串口
    if (_settings->getPortNameDataPort() == "") {
        qDebug() << "Empty DataPort Name";
        QMessageBox::critical(this,
                              tr("Open DATA Failed"),
                              tr("Failed to open DATA port\nPlease check settings!"));
        return;
    }
    _portData->setPortName(_settings->getPortNameDataPort());
    _portData->setBaudRate(QSerialPort::Baud115200);
    _portData->setDataBits(QSerialPort::Data8);
    _portData->setParity(QSerialPort::NoParity);
    _portData->setStopBits(QSerialPort::OneStop);
    _portData->setFlowControl(QSerialPort::NoFlowControl);
    if (_portData->open(QIODevice::ReadOnly)) {
        actionStartSensor->setEnabled(false);
        actionStopSensor->setEnabled(true);
        actionSettings->setEnabled(false);
        return;
    } else {
        QMessageBox::critical(this,
                              tr("Open Failed"),
                              tr("Failed to open DATA port\nPlease check settings!"));
        return;
    }
}

void MainWindow::onActionSensorStop() {
    _portUart->setPortName(_settings->getPortNameUartPort());
    _portUart->setBaudRate(QSerialPort::Baud115200);
    _portUart->setDataBits(QSerialPort::Data8);
    _portUart->setParity(QSerialPort::NoParity);
    _portUart->setStopBits(QSerialPort::OneStop);
    _portUart->setFlowControl(QSerialPort::NoFlowControl);

    QStringList cmds;
    QByteArray readBuf;
    cmds.append("sensorStop");
    cmds.append("sensorStop");
    if (_portUart->open(QIODevice::ReadWrite | QIODevice::Text)) {
        for (QString cmd: cmds) {
            _portUart->write(cmd.toLatin1());
            _portUart->write("\n");
            _portUart->waitForReadyRead(300);
            QThread::msleep(10); // 期待全部数据
            readBuf = _portUart->readAll();
            if (QString(readBuf).contains("Done")) {
                qDebug() << "Send Command => "
                         << cmd;
            } else {
                qDebug() << "\nSend Command ["
                         << cmd << "] Faild!";
                QMessageBox::critical(this,
                                      tr("Close Failed"),
                                      tr("Not Recogized as a CLI Command"));
                _portUart->close();
                return;
            }
        }
        _portUart->close();
    } else {
        qDebug() << "Close Serial Port Failed.";
        QMessageBox::critical(this,
                              tr("Close Failed"),
                              tr("Failed to open serial port"));
        return;
    }

    actionStartSensor->setEnabled(true);
    actionStopSensor->setEnabled(false);
    actionSettings->setEnabled(true);
    _portData->close();
}

void MainWindow::onActionSettings() {
    DialogPreference dlg(_settings, this);
    if (dlg.exec()) {
        _settings->printInfo();
    }
}
