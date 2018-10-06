#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
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

    QString getConfigFilePath();
    void setConfigFilePath(QString path);

    void printInfo();
    void readLocalSettings();
    void writeLocalSettings();

    double getSpeedThreshold() const;
    void setSpeedThreshold(double speedThreadold);

    int getFrameRate() const;
    void setFrameRate(int frameRate);

private:
    QString _portNameData;
    qint32 _baudRateData;

    QString _portNameUart;
    qint32 _baudRateUart;

    QString _configFilePath;

    double _speedThreshold;
    int _frameRate;
};

#endif // SETTINGS_H
