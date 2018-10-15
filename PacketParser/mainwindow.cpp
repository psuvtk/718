#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <commandlineinterface.h>
#include <QPlainTextEdit>

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
    tryFindSerialPort();

    _cli = new CommandLineInterface(_portNameUart);
//    connect(this, &MainWindow::sendCmd, _cli, &CommandLineInterface::handleSendCmd);
//    _cli->start();
}

MainWindow::~MainWindow()
{
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
}

void MainWindow::onActionConnect()
{

    QStringList cmds;
    cmds << "advFrameCfg\n" << "sensorStart\n";


    for (auto cmd: cmds) {
        _cli->sendCmd(cmd);
//        QThread::msleep(75);
    }

}

void MainWindow::onActionDisconnect()
{

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
                _portNameUart = ports[i].portName();
            else
                _portNameData = ports[i].portName();
        }
        ui->lineEditUart->setText(_portNameUart);
        ui->lineEditData->setText(_portNameData);
    }
}
