#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSerialPort>

namespace Ui {
class Settings;
}

class Settings : public QObject
{
    Q_OBJECT


public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    void setPortName(QString pn);
    void setBaudRate(QSerialPort::BaudRate br);
    void setDataBits(QSerialPort::DataBits db);
    void setParity(QSerialPort::Parity p);
    void setStopBits(QSerialPort::StopBits sb);
    void setFlowControl(QSerialPort::FlowControl fc);

private:
    QString _portName;
    QSerialPort::BaudRate _baudRate;
    QSerialPort::DataBits _dataBits;
    QSerialPort::Parity _parity;
    QSerialPort::StopBits _stopBits;
    QSerialPort::FlowControl _flowControl;
};

#endif // SETTINGS_H
