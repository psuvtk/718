#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogpreference.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("718");
    QCoreApplication::setApplicationName("SrrLED");

    _settings = new Settings(this);
    _uartReadThread = new UartThread;

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
    if (!_uartReadThread->open(_settings)) {
        QMessageBox::critical(this, tr("Faild to Open Serail Port"),
                              tr("Open serial port failed"));
        return;
    }

    _uartReadThread->run();
}
