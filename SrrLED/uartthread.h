#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>

#include <QByteArray>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

#include "settings.h"
#include "srrpacket.h"

class CommThread : public QThread
{
    Q_OBJECT

public:
    enum DeviceState {
        CLOSE,
        OPEN,
        PAUSE
    };

    enum FramePerSecond {
        FPS33 = 1,
        FPS16,
        FPS11,
        FPS8,
        FPS6
    };

public:
    CommThread() = delete;
    explicit CommThread(QSerialPort *_device);

    void run();
    DeviceState deviceState() const;

signals:
    void frameChanged(SrrPacket *);
    void deviceOpenSuccess();
    void deviceOpenFailed();
    void connectionLost();

public slots:
    void handleDispDone();
    void handleDeviceOpen();

    void handleFrameRateChanged(FramePerSecond fm);
    void handleDeviceStateChanged(const DeviceState &deviceState);

private slots:
    void handleReadReady();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *_device = nullptr;
    QByteArray bufRecv;
    QByteArray bufFrame;
    FramePerSecond _frameRate = FPS33;
    DeviceState _deviceState = CLOSE;
    QTime _tic;

    bool _isDispDone = true;
};

#endif // UARTTHREAD_H
