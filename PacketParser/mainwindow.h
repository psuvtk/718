#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <unordered_map>

#include <QDebug>

#include "dronepacket.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void readConfig();
    void tryFindSerialPort();
    bool sendConfig();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onReadyRead();
    void onUartReadyRead();

private slots:
    void onActionConnect();
    void onActionDisconnect();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionSettings;

    qint32 _baudRate = 57600;
    QString _cfgFileName = "default.cfg";

    QSerialPort *_portUart;
    QSerialPort *_portData;

    QStringList _cmds;
};

#endif // MAINWINDOW_H
