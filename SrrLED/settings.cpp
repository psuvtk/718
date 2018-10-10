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
    if (settings.contains("frameRate")) {
        _frameRate = settings.value("frameRate").toInt();
    } else {
        _frameRate = 1;
    }
    if (settings.contains("speedThreshold")) {
        _speedThreshold = settings.value("speedThreshold").toDouble();
    } else {
        _speedThreshold = 18.0;
    }

    if (settings.contains("filterThreshold")) {
        _filterThreshold = settings.value("filterThreshold").toDouble();
    } else {
        _filterThreshold = 0.36;
    }
}

void Settings::writeLocalSettings() {
    qDebug() << "Settings::writeLocalSettings()";
    QSettings settings(this->parent());
    settings.setValue("frameRate", _frameRate);
    settings.setValue("speedThreshold", _speedThreshold);
    settings.setValue("filterThreshold", _filterThreshold);
}

double Settings::getSpeedThreshold() const
{
    return _speedThreshold;
}

void Settings::setSpeedThreshold(double speedThreadold)
{
    _speedThreshold = speedThreadold;
}

int Settings::getFrameRate() const
{
    return _frameRate;
}

void Settings::setFrameRate(int frameRate)
{
    _frameRate = frameRate;
}

double Settings::getFilterThreshold() const
{
    return _filterThreshold;
}

void Settings::setFilterThreshold(double filterThreshold)
{
    _filterThreshold = filterThreshold;
}

bool Settings::getFullscreenOnStartup() const
{
    return _fullscreenOnStartup;
}

void Settings::setFullscreenOnStartup(bool fullscreenOnStartup)
{
    _fullscreenOnStartup = fullscreenOnStartup;
}
