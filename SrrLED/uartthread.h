#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include "settings.h"

class UartThread : public QThread
{
    Q_OBJECT

public:
    UartThread();

    void run();
    void setHandle(QSerialPort *_device);

signals:
    void speedChanged(qint64);

private:
    QSerialPort *_device;
};

#endif // UARTTHREAD_H
