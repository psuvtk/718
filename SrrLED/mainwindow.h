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
#include <cmath>
#include "srrpacket.h"
#include "dialogpreference.h"
#include "uartthread.h"
#include "settings.h"
#include "qcustomplot.h"


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

    void plotUpdate();
signals:
    void dispDone();

public slots:
    void onFrameChanged(SrrPacket *);

    void showAboutMessage();

    void onActionConnect();
    void onActionDisconnect();
    void onActionSettings();

private:
    Ui::MainWindow *ui;
    Settings *_settings;
    UartThread *_commThread;
    QCustomPlot *_plotHandle;

    QSerialPort *_portUart;
    QSerialPort *_portData;
    DeviceState _deviceState;

    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionSettings;
};

#endif // MAINWINDOW_H
