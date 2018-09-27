#include "settings.h"
#include <QDebug>
Settings::Settings(QWidget *parent)
{
    readLocalSettings();
    printInfo();
}

Settings::~Settings()
{
    writeLocalSettings();
}


void Settings::setPortNameUartPort(QString pn) {
    _portNameUart = pn;
}

void Settings::setBaudRateUartPort(qint32 br) {
    _baudRateUart = br;
}

QString Settings::getPortNameUartPort() {
    return _portNameUart;
}

qint32 Settings::getBaudRateUartPort() {
    return _baudRateUart;
}

void Settings::setPortNameDataPort(QString pn) {
    _portNameData = pn;
}

void Settings::setBaudRateDataPort(qint32 br) {
    _baudRateData = br;
}


QString Settings::getPortNameDataPort() {
    return _portNameData;
}

qint32 Settings::getBaudRateDataPort() {
    return _baudRateData;
}

void Settings::setConfigFilePath(QString path) {
    _configFilePath = path;
}

QString Settings::getConfigFilePath() {
    return _configFilePath;
}






void Settings::printInfo() {

}

void Settings::readLocalSettings() {
    qDebug() << "Settings::readLocalSettings()";
    QSettings settings(this->parent());
    settings.beginGroup("serialport");
//    _portNameUart = settings.value("portNameUart").toString();
//    _baudRateUart = static_cast<qint32>(settings.value("baudRateUart").toInt());
//    _portNameData = settings.value("portNameData").toString();
//    _baudRateData = static_cast<qint32>(settings.value("baudRateData").toInt());
    settings.endGroup();
}

void Settings::writeLocalSettings() {
    qDebug() << "Settings::writeLocalSettings()";
    QSettings settings(this->parent());
    settings.beginGroup("serialport");
//    settings.setValue("portNameUart", _portNameUart);
//    settings.setValue("baudRateUart", _baudRateUart);
//    settings.setValue("portNameData", _portNameData);
//    settings.setValue("baudRateData", _baudRateData);
    settings.endGroup();
}
