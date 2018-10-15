#ifndef COMMANDLINEINTERFACE_H
#define COMMANDLINEINTERFACE_H

#include <QObject>
#include <QFile>
#include <QThread>
#include <QSerialPort>
#include <QDebug>

class CommandLineInterface : public QObject
{
    Q_OBJECT

public:
    explicit CommandLineInterface(QString portName, QObject *parent = nullptr);
    ~CommandLineInterface();

    bool open();
    void close();

    bool sendConfigFile(const QString &path);
    bool sendCmd(const QStringList &cmds);
    bool sendCmd(const QString &cmd);

private slots:
    void handleReadyRead();


private:
    QSerialPort *_cli;
    QByteArray _bufRecv;
};

#endif // COMMANDLINEINTERFACE_H
