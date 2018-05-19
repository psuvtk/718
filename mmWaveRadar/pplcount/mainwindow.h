#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "mmwaveconfig.h"
#include "radarpacket.h"
#include "qcustomplot.h"
#include <cmath>


const double PI = std::acos(-1);
#define MAX_GRAPH_NUM 64
constexpr Qt::GlobalColor TargetColor[10] = {Qt::red, Qt::darkCyan, Qt::darkGreen,  Qt::darkMagenta, Qt::green,
                                                   Qt::darkRed, Qt::cyan, Qt::darkYellow, Qt::magenta, Qt::yellow};

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

    void queryArea(double *left, double *right, double *front, double *back);
    void queryAngle(double *va, double *ra);
    void setArea(double left, double right, double front, double back);
    void setAngle(double va, double ra);

    static void listen_wrapper(MainWindow *p);

    // 给周青收集点云数据
    void collectPointCloud(const char *body, int nTLVs);


    void _drawBackground();
    void _drawRedBox(QPen pen=QPen(Qt::darkRed, 2));
    void _drawBlueBox(QPen pen=QPen(Qt::darkBlue, 2));
    void _replot();
    void _updateScene();


    void _parseFrame(const char *frame);
    void _parseTartget(const char *tlv);
    void _parsePoint(const char *tlv);
    void _parseTartgetIdx(const char *tlv);

private slots:
    void on_btnSendConfig_clicked();
    void on_btnStartDetect_clicked();
    void on_btnRadarOff_clicked();


private:
    Ui::MainWindow *ui;
    MmWaveRadar *radar;

    // canvas setting
    double _canvasLeft = 6.0f;
    double _canvasRight = 6.0f;
    double _canvasFront = 6.0f;
    double _canvasBack = 0.0f;

    // 红蓝框
    bool _rbBoxEnabled = true;
    double _redTopLeftX = -2.0f;
    double _redTopLeftY = 3.5f;
    double _redBottomRightX = -0.5f;
    double _redBottomRightY = 1.5f;

    double _blueTopLeftX = 0.5f;
    double _blueTopLeftY = 3.5f;
    double _blueBottomRightX = 2.0f;
    double _blueBottomRightY = 1.5f;

    // 视野角
    double _viewAngle = 2 * PI / 3;
    // 旋转角
    double _spinAngle = 0.0f;

    QCPGraph *_graphPointCloud = nullptr;

#if defined(__linux__)
    QString _portNameUART = "/dev/ttyACM0";
    QString _portNameAUX = "/dev/ttyACM1";
#elif defined(_WIN32)
    QString _portNameUART = "COM3";
    QString _portNameAUX = "COM4";
#endif



    QString _pathToConfig= "/home/kristoffer/Workspace/Qt/lab0011-pplcount/lab0011_pplcount_gui/mmw_pplcount_demo_default.cfg";

    QCPItemRect *_rectRed0 = nullptr;
    QCPItemRect *_rectRed1 = nullptr;
    QCPItemRect *_rectBlue0 = nullptr;
    QCPItemRect *_rectBlue1 = nullptr;
    QCPItemLine *_bgLine00 = nullptr;
    QCPItemLine *_bgLine10 = nullptr;
    QCPItemLine *_bgLine01 = nullptr;
    QCPItemLine *_bgLine11 = nullptr;
    QCPItemCurve *_bgArc0 = nullptr;
    QCPItemCurve *_bgArc1 = nullptr;
    QCPGraph *_graphsPointTrace[MAX_GRAPH_NUM];
    QCPGraph *_graphsTargetTrack[MAX_GRAPH_NUM];

};

#endif // MAINWINDOW_H
