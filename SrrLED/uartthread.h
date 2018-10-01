#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include "settings.h"
#include "srrpacket.h"
#include <QDebug>
#include <QByteArray>

class UartThread : public QThread
{
    Q_OBJECT

public:
    UartThread();

    void run();
    void setHandle(QSerialPort *_device);

    void waitForDispDone();
    double selectSpeed();

public slots:
    void onDispDone();

signals:
    void speedChanged(double);
    void frameChanged(SrrPacket *);

private:
    QSerialPort *_device = nullptr;
    bool _isDispDone = false;
};

#endif // UARTTHREAD_H
