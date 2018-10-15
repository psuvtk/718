#ifndef COMMANDLINEINTERFACE_H
#define COMMANDLINEINTERFACE_H

#include <QObject>
#include <QFile>
#include <QThread>
#include <QSerialPort>
#include <QDebug>

class CommandLineInterface : public QThread
{
    Q_OBJECT
public:
    explicit CommandLineInterface(QString portName, QObject *parent = nullptr);
    ~CommandLineInterface();

    bool sendConfigFile(const QString &path);

    bool sendCmd(const QStringList &cmds);
    bool sendCmd(const QString &cmd);


    void setDelay(int delay);
    void run() {
        exec();
    }

public slots:
    void handleSendCmd(const QString &cmd);

private slots:
    void handleReadyRead();


private:
    QSerialPort *_cli;
    QByteArray _bufRecv;
    int _delay = 50;  // ms
};

#endif // COMMANDLINEINTERFACE_H
