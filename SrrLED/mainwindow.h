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
#include "plotworker.h"

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
    void dispPacketDetail(SrrPacket *);
    bool sensorStart();
    void testPlot();

signals:
    void dispDone();

public slots:
    void onFrameChanged(SrrPacket *);

    void showAboutMessage();

    void onActionConnect();
    void onActionDisconnect();
    void onActionSettings();

private slots:
    void on_cbNearView_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    Settings *_settings;
    CommThread *_commThread;
    PlotWorker *_plotWorker;

    QSerialPort *_portUart;
    QSerialPort *_portData;
    DeviceState _deviceState;

    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionSettings;
};

#endif // MAINWINDOW_H
