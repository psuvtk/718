#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "mmwaveconfig.h"
#include "radarpacket.h"
#include "qcustomplot.h"
#include <cmath>


const double pi = std::acos(-1);
#define MAX_GRAPH_NUM 64
constexpr Qt::GlobalColor TargetColor[10] = {Qt::red, Qt::darkCyan, Qt::yellow, Qt::darkMagenta, Qt::green,
                                                   Qt::darkRed, Qt::cyan, Qt::darkYellow, Qt::magenta, Qt::darkGreen};



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QSerialPort::BaudRate BaudRate;
    typedef QSerialPort::StopBits StopBits;
    typedef QSerialPort::DataBits DataBits;
    typedef QSerialPort::Parity Parity;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void drawBackground();
    void reDraw();

    void queryArea(double *left, double *right, double *front, double *back);
    void queryAngle(double *va, double *ra);
    void setArea(double left, double right, double front, double back);
    void setAngle(double va, double ra);


    static void listen_wrapper(MainWindow *p);


    // 给周青收集点云数据
    void collectPointCloud(const char *body, int nTLVs);

    void _drawBackground();
    void _parseFrame(const char *frame);
    void _parseTartget(const char *tlv);
    void _parsePoint(const char *tlv);
    void _parseTartgetIdx(const char *tlv);

private slots:

    void on_btnSendConfig_clicked();

    void on_btnStartDetect_clicked();

    void on_btnRadarOff_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    MmWaveRadar *radar;

    QCPGraph *_graphsPointTrace[MAX_GRAPH_NUM];
    QCPGraph *_graphsTargetTrack[MAX_GRAPH_NUM];

    // canvas setting
    double areaLeft = 6.0f;
    double areaRight = 6.0f;
    double areaFront = 6.0f;
    double areaBack = 0.0f;

    // 扇形区域半径
    const double r = 6;

    // 红蓝框
    bool rbBoxEnabled = true;
    double _redTopLeftX = -2.0f;
    double _redTopLeftY = 3.5f;
    double _redBottomRightX = -0.5f;
    double _redBottomRightY = 1.5f;

    double _blueTopLeftX = 0.5f;
    double _blueTopLeftY = 3.5f;
    double _blueBottomRightX = 2.0f;
    double _blueBottomRightY = 1.5f;




    // serial setting;
    BaudRate baudRate = BaudRate::Baud115200;
    StopBits stopBits = StopBits::OneStop;
    DataBits dataBits = DataBits::Data8;
    Parity parity = Parity::NoParity;

    // 视野角
    double visionAngle = 2 * pi / 3;
    // 旋转角
    double rotateAngle = 0.0f;


    QCPGraph *_graphPointCloud = nullptr;
    QCPGraph *_graphTarget = nullptr;


#if defined(__linux__)
    QString portNameUART = "/dev/ttyACM0";
    QString portNameAUX = "/dev/ttyACM1";
#elif defined(_WIN32)
    QString portNameUART = "COM3";
    QString portNameAUX = "COM4";
#endif



    QString pathToConfig= "/home/kristoffer/Workspace/Qt/lab0011-pplcount/lab0011_pplcount_gui/mmw_pplcount_demo_default.cfg";

};

#endif // MAINWINDOW_H
