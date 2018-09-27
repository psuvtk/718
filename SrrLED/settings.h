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

    QString getPortNameUartPort();
    qint32 getBaudRateUartPort();
    void setPortNameUartPort(QString pn);
    void setBaudRateUartPort(qint32 br);

    QString getPortNameDataPort();
    qint32 getBaudRateDataPort();
    void setPortNameDataPort(QString pn);
    void setBaudRateDataPort(qint32 br);


    void printInfo();
    void readLocalSettings();
    void writeLocalSettings();

private:
    QString _portNameData;
    qint32 _baudRateData;

    QString _portNameUart;
    qint32 _baudRateUart;

};

#endif // SETTINGS_H
