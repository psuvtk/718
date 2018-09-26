#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "dialogpreference.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _speed(0)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("718");
    QCoreApplication::setApplicationName("SrrLED");

    connect(ui->actionPreference, &QAction::triggered, [=](){
        Settings settings(this);
        DialogPreference dlg(this);
        dlg.exec();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSpeedChanged() {

}
