#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include "settings.h"
#include "srrpacket.h"
#include <QDebug>
#include <QByteArray>

class CommThread : public QThread
{
    Q_OBJECT

public:
    CommThread();

    void run();
    void setHandle(QSerialPort *_device);

    void waitForDispDone();

    void setOverN(int overN);

public slots:
    void onDispDone();

signals:
    void frameChanged(SrrPacket *);

private:
    QSerialPort *_device = nullptr;
    bool _isDispDone = false;
    int _overN = 1;
};

#endif // UARTTHREAD_H
