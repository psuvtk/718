#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include "settings.h"
#include "srrpacket.h"
#include <QDebug>
#include <QByteArray>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>

class CommThread : public QThread
{
    Q_OBJECT
public:
    enum DeviceState {
        CLOSE,
        OPEN,
        PAUSE
    };
public:
    CommThread();

    void run();
    void setHandle(QSerialPort *_device);

    void waitForDispDone();

    void setOverN(uint overN);

    DeviceState deviceState() const;
    void setDeviceState(const DeviceState &deviceState);

    void setIsDispDone(bool isDispDone);

public slots:
    void onDispDone();
    void onDataReady();

signals:
    void frameChanged(SrrPacket *);

private:
    QSerialPort *_device = nullptr;
    DeviceState _deviceState = CLOSE;

    bool _isDispDone = true;
    uint _overN = 1;
};

#endif // UARTTHREAD_H
