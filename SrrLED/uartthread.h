#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>

class UartThread : public QThread
{
    Q_OBJECT

public:
    UartThread();

    void run();

signals:
    void speedChanged(qint64);

private:
    QSerialPort _device;
};

#endif // UARTTHREAD_H
