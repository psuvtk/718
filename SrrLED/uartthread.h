#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>

#include <QDebug>
#include <QByteArray>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>

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

    enum FramePerMinute {
        FPM33 = 1,
        FPM16,
        FPM11,
        FPM8,
        FPM6
    };
public:
    CommThread() = delete;
    explicit CommThread(QSerialPort *_device);

    void run();

    DeviceState deviceState() const;

private:


signals:
    void frameChanged(SrrPacket *);
    void deviceOpenSuccess();
    void deviceOpenFailed();

    void connectionLost();

public slots:
    void onDispDone();
    void onFrameRateChanged(FramePerMinute fm);
    void onDeviceStateChanged(const DeviceState &deviceState);

    void onDeviceOpen();
    void onSerialPortError(QSerialPort::SerialPortError error);

private slots:
    void onDataReady();


private:
    QSerialPort *_device = nullptr;

    FramePerMinute _frameRate = FPM33;
    DeviceState _deviceState = CLOSE;
    bool _isDispDone = true;
};

#endif // UARTTHREAD_H
