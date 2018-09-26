#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QKeyEvent>
#include "uartthread.h"
#include "settings.h"
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);

public slots:
    void onSpeedChanged();
    void showAboutMessage();

private:
    Ui::MainWindow *ui;
    Settings *_settings;
    UartThread *_uartReadThread;
};

#endif // MAINWINDOW_H
