#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogpreference.h"
#include <QSerialPortInfo>

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


    connect(ui->actionPreference, &QAction::triggered, [=](){
        DialogPreference dlg(_settings, this);
        if (dlg.exec()) {
            _settings->printInfo();
        }
    });

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutMessage);
    connect(ui->actionAbout_Qt, &QAction::triggered, [=](){
        QApplication::aboutQt();
    });


    // beign Test


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
    if (event->key() != Qt::Key_Space) return;

//    _uartReadThread->run();
}

bool MainWindow::startSensor() {
    if (_settings->getPortNameUartPort() == "") return false;

    _portUart->setPortName(_settings->getPortNameUartPort());
    _portUart->setBaudRate(QSerialPort::Baud115200);
    _portUart->setDataBits(QSerialPort::Data8);
    _portUart->setParity(QSerialPort::NoParity);
    _portUart->setStopBits(QSerialPort::OneStop);
    _portUart->setFlowControl(QSerialPort::NoFlowControl);

    if (_portUart->open(QIODevice::ReadWrite | QIODevice::Text)) {
        // TODO： send command
    } else {
        return false;
    }

    _portUart->close();
    return true;
}

bool MainWindow::stopSensor() {
    _portUart->setPortName("");
    _portUart->setBaudRate(QSerialPort::Baud115200);
    _portUart->setDataBits(QSerialPort::Data8);
    _portUart->setParity(QSerialPort::NoParity);
    _portUart->setStopBits(QSerialPort::OneStop);
    _portUart->setFlowControl(QSerialPort::NoFlowControl);

    if (_portUart->open(QIODevice::ReadWrite | QIODevice::Text)) {
        // TODO： send command
    } else {
        return false;
    }

    _portUart->close();
    return true;
}


void MainWindow::serialPortConfig() {
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
}

