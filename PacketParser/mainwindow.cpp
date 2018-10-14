#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    menuBar()->hide();
    setWindowTitle(tr("Packet Parser"));
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

    ui->mainToolBar->addSeparator();

    actionSettings = new QAction(QIcon(":/icons/settings.svg"),
                                 tr("option"),
                                 this);
    ui->mainToolBar->addAction(actionSettings);

    QAction *actionAbout = new QAction(QIcon(":/icons/toy.ico"),
                                       tr("about"),
                                       this);
    ui->mainToolBar->addAction(actionAbout);

    _portData = new QSerialPort;

    connect(_portData, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    connect(_portUart, &QSerialPort::readyRead, this, &MainWindow::onUartReadyRead);
    tryFindSerialPort();
    readConfig();



    ui->listWidget->addItem("advFrameCfg");
    ui->listWidget->addItem("sensorStart");

}

MainWindow::~MainWindow()
{
    if (_portData->isOpen()) _portData->close();
    if (_portUart->isOpen()) _portUart->close();
    delete _portData;
    delete _portUart;
    delete ui;
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    std::unordered_map<int, quint32> hmap;
    hmap.insert({0,38400});
    hmap.insert({1,57600});
    hmap.insert({2,76800});
    hmap.insert({3,115200});
    hmap.insert({4,921600});

    _baudRate = hmap[index];
    qDebug() << _baudRate;
}

void MainWindow::on_pushButton_clicked()
{
   QFileDialog dlg(this);
   dlg.setNameFilter("Config(*.cfg)");
   dlg.setDirectory(".");
   if (dlg.exec()) {
        _cfgFileName = dlg.selectedFiles()[0];
        statusBar()->showMessage(QString("Config File: %1").arg(_cfgFileName));
   }
   readConfig();
}

void MainWindow::onActionConnect()
{
    tryFindSerialPort();
    sendConfig();

    _portData->setBaudRate(_baudRate);
    _portData->setDataBits(QSerialPort::Data8);
    _portData->setParity(QSerialPort::NoParity);
    _portData->setStopBits(QSerialPort::OneStop);
    _portData->setFlowControl(QSerialPort::NoFlowControl);

    if (_portData->isOpen()) _portData->close();
    if (!_portData->open(QIODevice::ReadOnly)){
        qDebug() << "Uart Port Open Failed";
    }
}

void MainWindow::onActionDisconnect()
{

}

bool MainWindow::sendConfig()
{
    _portUart->setBaudRate(QSerialPort::Baud115200);
    _portUart->setDataBits(QSerialPort::Data8);
    _portUart->setParity(QSerialPort::NoParity);
    _portUart->setStopBits(QSerialPort::OneStop);
    _portUart->setFlowControl(QSerialPort::NoFlowControl);

    if (!_portUart->isOpen()) _portUart->open(QIODevice::ReadWrite);

    // 正式发送CLI命令


    QByteArray readBuf;
    quint32 nErr = 0;

//    for (QString cmd: _cmds) {
//retry:
//        readBuf.clear();
//        _portUart->write(cmd.toLatin1());
//        // 等待全部返回数据，10ms超时
//        while (_portUart->waitForReadyRead(5)) {
//        }
//        readBuf.append(_portUart->readAll());



//        qDebug() << "Send Command => " << cmd;
//        qDebug() << "Received: " << readBuf;

//        if (QString(readBuf).contains("Done\n\rmmwDemo:/>\nmmwDemo:/>")) {
//            readBuf.clear();
//            continue;
//        }

//        if (QString(readBuf).contains("Error")) {
//            qDebug() << "\nSend Command ["
//                     << cmd << "] Faild!";
//            _portUart->close();
//            return false;
//        }

//        if (nErr++ < 2 ) {
//            goto retry;
//        } else {
//            qDebug() << "\nSend Command ["
//                     << cmd << "] Faild!";
//            _portUart->close();
//            return false;
//        }
//    }
//    QString cmd("advFrameCfg\n");
//    _portUart->write(cmd.toLatin1());
//      _portUart->write("advFrameCfg\n", 12);
//      _portUart->flush();
//      QThread::msleep(50);
//      _portUart->write("sensorStart\n", 12);
//      _portUart->flush();
//      QThread::msleep(50);

    _cmds << "advFrameCfg\n";
    _cmds << "sensorStart\n";
    for (QString cmd: _cmds) {
        _portUart->write(cmd.toLocal8Bit());
        _portUart->flush();
        QThread::msleep(10);
    }

    return true;
}




void MainWindow::tryFindSerialPort()
{
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
                _portUart->setPortName(ports[i].portName());
            else
                _portData->setPortName(ports[i].portName());
        }
        ui->lineEditUart->setText(_portUart->portName());
        ui->lineEditData->setText(_portData->portName());
    }
}

void MainWindow::readConfig()
{
    QFile f(_cfgFileName);

    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file: " << _cfgFileName;
    }

    _cmds.clear();
    while (!f.atEnd()) {
        QString cmd(f.readLine());
        qDebug() << "Parse commad: " << cmd;
        _cmds << cmd;
    }
}

void MainWindow::onReadyRead()
{
    static QByteArray SYNC = QByteArray::fromHex("0201040306050807");
    qint32 skipLength = 0;

    static QByteArray bufRecv, bufFrame;
    static int nErr = 0;
    bufRecv.append(_portData->readAll());

    if (bufRecv.startsWith(SYNC)) {
        skipLength = bufRecv.indexOf(SYNC, 8);
        if (skipLength != -1) {
            bufFrame.append(bufRecv.left(skipLength));
            bufRecv.remove(0, skipLength);

            DronePacket packet(bufFrame.data(), bufFrame.length());
            qDebug() << packet.isValid();
            qDebug() << "++++++++++++++++++++++++";
            for (auto obj: packet.getDetObjs()) {
                qDebug() << "x,y:" << obj.x <<","
                         << obj.y;
            }
            qDebug() << "++++++++++++++++++++++++";

            bufFrame.clear();
        } else {
            // pass
        }
    } else {
        skipLength = bufRecv.indexOf(SYNC);
        if (skipLength == -1) {
            bufRecv.clear();
            return;
        }
        else
            bufRecv.remove(0, skipLength);
        qDebug() << "\n未检测到同步码元\n";
//        nErr++;
//        if (nErr >3) {
//            nErr=0;
//            bufRecv.clear();
//        }
    }

}

void MainWindow::onUartReadyRead()
{
    static int cnt = 0;
    qDebug() <<cnt++ <<": ";
    qDebug() <<  _portUart->readAll();
}
