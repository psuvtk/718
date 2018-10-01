#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
#include <QSerialPortInfo>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QTableWidgetItem>

#include "srrpacket.h"
#include "dialogpreference.h"
#include "uartthread.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum DeviceState {
        CLOSE,
        OPEN,
        PAUSE
    };
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void tryFindSerialPort();
    void displaySubframeParams();

    bool sensorStart();

signals:
    void dispDone();

public slots:
    void onSpeedChanged(double);
    void onFrameChanged(SrrPacket *);

    void showAboutMessage();

    void onActionConnect();
    void onActionDisconnect();
    void onActionSettings();

private:
    Ui::MainWindow *ui;
    Settings *_settings;
    UartThread *_commThread;

    QSerialPort *_portUart;
    QSerialPort *_portData;
    DeviceState _deviceState;

    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionSettings;
};

#endif // MAINWINDOW_H
