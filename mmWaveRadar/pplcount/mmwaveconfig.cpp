#include "mmwaveconfig.h"
#include <QObject>
#include <QFile>
#include <QDebug>
#include <iostream>
#include <thread>


MmWaveRadar::MmWaveRadar(const QString portUart, const QString portAux)
{
    this->serialUart = new QSerialPort(portUart);
    this->serialAux = new QSerialPort(portAux);
}

void MmWaveRadar::setPort(const QString portUart, const QString portAux)
{
    delete this->serialUart;
    delete this->serialAux;
    this->serialUart = new QSerialPort(portUart);
    this->serialAux = new QSerialPort(portAux);
}


/**
 * @brief   MmWaveConfig::config
 * @date    201805123
 * @return  配置成功返回 true， 否则返回 false
 * @details 配置文件每发送一行，等待接受返回值(10ms超时),如果返回值中包含"Done",
 *          则发送下一行，一旦发送的行数与接受的"Done"则放回 false
 */
bool MmWaveRadar::config(const QString pathToConfig)
{
    if (!serialUart->isOpen()) {
        this->openSerialUart();
    }

    QFile configFile(pathToConfig);

    if (! configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: can't open file!";
    } else {
        qDebug() << "[+] Done: open configuration file.";
    }

    QByteArray bufRecv;
    while(!configFile.atEnd()) {
        QByteArray line = configFile.readLine();
        serialUart->write(line);

        bufRecv.clear();
        while (serialUart->waitForReadyRead(10)) {
            bufRecv.append(serialUart->readAll());
        }

        // 发送成功
        if(bufRecv.contains("Done")) {
            qDebug() << "[+] Send: "<< line;
            bufRecv.clear();
        } else {
            qDebug() << "\nError:\n";
            qDebug() << "Send:"<< line;
            qDebug() << "Return:" << bufRecv << "\n";
            return false;
        }
    }
    return true;
}


bool MmWaveRadar::config(const QString pathToConfig, QByteArray &radarRetStr)
{
    if (!serialUart->isOpen()) {
        this->openSerialUart();
    }

    QFile configFile(pathToConfig);
    if (! configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: can't open file!";
    } else {
        qDebug() << "[+] Done: open configuration file.";
    }

    while(!configFile.atEnd()) {
        QByteArray line = configFile.readLine();
        serialUart->write(line);

        radarRetStr.clear();
        while (serialUart->waitForReadyRead(10)) {
            radarRetStr.append(serialUart->readAll());
        }

        // 发送成功
        if(radarRetStr.contains("Done")) {
            radarRetStr.clear();
            qDebug() << "[+] Send: "<< line;
        } else {
            qDebug() << "\nError:\n";
            qDebug() << "Send:"<< line;
            qDebug() << "Return:" << radarRetStr << "\n";
            return false;
        }
    }
    return true;
}



bool MmWaveRadar::sendCmd(QString cmd)
{
    if (!serialUart->isOpen()) {
        this->openSerialUart();
    }

    serialUart->write(cmd.toLatin1());
    QByteArray bufRecv;
    while (serialUart->waitForReadyRead(10)) {
        bufRecv.append(serialUart->readAll());
    }

    // 发送成功
    if (bufRecv.contains("Done")) {
        qDebug() << "[+] Send: "<< cmd;
        return true;
    } else {
        qDebug() << "Fatal: command send failed.\n"
                 << "Send: " << cmd << "\n"
                 << "Return: " << bufRecv << "\n";
        return false;
    }
}

bool MmWaveRadar::sendCmd(QString cmd, QByteArray &radarRetStr)
{
    if (!serialUart->isOpen()) {
        this->openSerialUart();
    }

    serialUart->write(cmd.toLatin1());

    while (serialUart->waitForReadyRead(10)) {
        radarRetStr.append(serialUart->readAll());
    }

    // 发送成功
    if (radarRetStr.contains("Done")) {
        qDebug() << "[+] Send: "<< cmd;
        return true;
    } else {
        qDebug() << "Fatal: command send failed.\n"
                 << "Send: " << cmd << "\n"
                 << "Return: " << radarRetStr << "\n";
        return false;
    }
}


bool MmWaveRadar::sensorStart()
{
    return sendCmd("sensorStart\n");
}

bool MmWaveRadar::isRunning()
{
    return serialAux->waitForReadyRead(5);
}

/**
 * @brief MmWaveConfig::openSerialUART
 * @return
 * @details 串口: uart
 *          波特率: 115200
 */
bool MmWaveRadar::openSerialUart(){
    if (serialUart->open(QIODevice::ReadWrite)) {
        serialUart->setBaudRate(QSerialPort::Baud115200);
        serialUart->setDataBits(QSerialPort::Data8);
        serialUart->setStopBits(QSerialPort::OneStop);
        serialUart->setParity(QSerialPort::NoParity);
        serialUart->setFlowControl(QSerialPort::NoFlowControl);
        qDebug() << "[+] open serial port "
                 << serialUart->portName()
                 << " success.";
        return true;
    } else {
        qDebug() << "Fatal: can't open seraial prot UART.";
        return false;
    }
}


bool MmWaveRadar::openSerialAux(){
    if (serialAux->open(QIODevice::ReadWrite)) {
        serialAux->setBaudRate(AuxBaudRate921600);
        serialAux->setDataBits(QSerialPort::Data8);
        serialAux->setStopBits(QSerialPort::OneStop);
        serialAux->setParity(QSerialPort::NoParity);
        serialAux->setFlowControl(QSerialPort::NoFlowControl);
        // no limit size
        serialAux->setReadBufferSize(0);
        qDebug() << "[+] open serial port "
                 << serialAux->portName()
                 << " success.";
        return true;
    } else {
        qDebug() << "Fatal: can't open seraial prot AUX.";
        return false;
    }
}


bool MmWaveRadar::waitForReadyRead()
{
    return serialAux->waitForReadyRead();
}

QByteArray MmWaveRadar::readAll()
{
    return serialAux->readAll();
}

QByteArray MmWaveRadar::read(qint64 nBytesRead)
{
    return serialAux->read(nBytesRead);
}


