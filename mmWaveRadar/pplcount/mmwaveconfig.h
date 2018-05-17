#ifndef MMWAVECONFIG_H
#define MMWAVECONFIG_H

#include <QObject>
#include <QSerialPort>

class MmWaveRadar : public QObject
{
    Q_OBJECT

public:
    MmWaveRadar() = delete;
    MmWaveRadar(const QString portUart, const QString portAux);

    void setPort(const QString portUart, const QString portAux);


    bool config(const QString pathToConfig);
    bool config(const QString pathToConfig, QByteArray &radarRetStr);
    bool sendCmd(QString cmd);
    bool sendCmd(QString cmd, QByteArray &radarRetStr);

    bool sensorStart();
    bool isRunning();

    bool openSerialUart();
    bool openSerialAux();

    bool isAuxOpen() { return serialAux->isOpen();}



    bool waitForReadyRead();
    QByteArray readAll();
    QByteArray read(qint64 nBytesRead);


private slots:

private:
    QSerialPort *serialUart = nullptr;
    QSerialPort *serialAux = nullptr;
    bool flagRunning = false;


    // 数据端口波特率
    const qint32 AuxBaudRate921600 = 921600;
};

#endif // MMWAVECONFIG_H
