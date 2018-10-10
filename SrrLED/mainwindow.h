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
#include <QTimer>
#include <QMutex>
#include <QMetaType>

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

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void tryFindSerialPort();
    void displaySubframeParams();
    void dispPacketDetail(SrrPacket *);
    const Tracker_t* extractSpeed(vector<Tracker_t> &trackers);
    bool sensorStart();

    void initGui();

signals:
    void dispDone();
    void deviceOpen();

    void deviceStateChanged(const CommThread::DeviceState &deviceState);
    void frameRateChanged(const CommThread::FramePerMinute &frameRate);

public slots:
    // commthread
    void onFrameChanged(SrrPacket *);
    void onDeviceOpenSuccess();
    void onDeviceOpenFailed();
    void onConnectionLost();

private slots:
    void showAboutMessage();
    void onActionConnect();
    void onActionDisconnect();
    void onActionSettings();
    void onTimeOut();




    // gui
    void on_cbNearView_toggled(bool checked);

    void on_cbSrrdDetObj_toggled(bool checked);

    void on_cbUsrrDetObj_toggled(bool checked);

    void on_cbCluster_toggled(bool checked);

    void on_cbTrackers_toggled(bool checked);

    void on_cbParkingAssisBins_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    Settings *_settings;
    CommThread *_commThread;
    PlotWorker *_plotWorker;

    QSerialPort *_portUart;
    QSerialPort *_portData;

    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionSettings;
    QMutex *_mutex;
};

#endif // MAINWINDOW_H
