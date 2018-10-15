#ifndef COMMTHREAD_H
#define COMMTHREAD_H

#include <QObject>
#include <QThread>
#include <QSerialPort>

class CommThread : public QThread
{
    Q_OBJECT

public:
   CommThread(const QString &portName);


signals:
    void deviceOpenSuccess();
    void deviceOpenFailed();
    void connectionLost();

public slots:
    void handleDeviceOpen();

private slots:
    void handleReadReady();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *_loggingHandle;
    QByteArray _bufRecv;
};

#endif // COMMTHREAD_H
