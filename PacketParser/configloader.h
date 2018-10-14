#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include <QSerialPort>

class ConfigLoader : public QObject
{
    Q_OBJECT
public:
    explicit ConfigLoader(QString portName, QObject *parent = nullptr);

    bool load(QString path);

signals:

private slots:
    void handleReadyRead();

private:
    QSerialPort *_cli;
    QStringList _cmds;
    QByteArray _curCommand;
};

#endif // CONFIGLOADER_H
