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
    enum DeviceState {
        OPENING,
        OPEN,
        CLOSING,
        CLOSED
    };
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void serialPortConfig();

    bool startSensor();
    bool stopSensor();

public slots:
    void onSpeedChanged();
    void showAboutMessage();


private:
    Ui::MainWindow *ui;
    Settings *_settings;
    UartThread *_uartReadThread;
    QSerialPort *_portUart;
    QSerialPort *_portData;
    DeviceState _deviceState;
};

#endif // MAINWINDOW_H
