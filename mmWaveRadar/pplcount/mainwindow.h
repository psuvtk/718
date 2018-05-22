#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <cmath>

#include "mmwaveconfig.h"
#include "radarpacket.h"
#include "qcustomplot.h"


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

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // 给周青收集点云数据
    void collectPointCloud(const char *body, int nTLVs);

    void _drawBackground();
    void _drawRedBox(QPen pen=QPen(Qt::darkRed, 2));
    void _drawBlueBox(QPen pen=QPen(Qt::darkBlue, 2));
    void _replot();
    void _updateScene();

    // 解析数据包
    void _parseFrame(const char *frame);
    void _parseTartget(const char *tlv);
    void _parsePoint(const char *tlv);
    void _parseTartgetIdx(const char *tlv);
    // 多线程监听
    static void listen_wrapper(MainWindow *p);

    // utils
    void queryArea(double *left, double *right, double *front, double *back);
    void queryAngle(double *va, double *ra);
    void queryRedBoxShape(double *a, double *b, double *c, double *d);
    void queryBlueBoxShape(double *a, double *b, double *c, double *d);
    void setArea(double left, double right, double front, double back);
    void setAngle(double va, double ra);
    void setRedBoxShape(double a, double b, double c, double d);
    void setBlueBoxShape(double a, double b, double c, double d);

    void _rotateCoord(double a, double b, double *x, double *y);
    bool _isInRedBox(double x, double y);
    bool _isInBlueBox(double x, double y);
private slots:
    void on_btnSendConfig_clicked();
    void on_btnStartDetect_clicked();
    void on_btnRadarOff_clicked();

private:
    Ui::MainWindow *ui;
    MmWaveRadar *radar;

    // 视场设置
    double _canvasLeft = 6.0f;
    double _canvasRight = 6.0f;
    double _canvasFront = 6.0f;
    double _canvasBack = 0.0f;
    double _viewAngle = 2 * PI / 3;
    double _spinAngle = 0.0f;

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

    // 绘制目标的容器
    QCPGraph *_graphPointCloud = nullptr;
    QCPGraph *_graphsPointTrace[MAX_GRAPH_NUM];
    QCPGraph *_graphsTargetTrack[MAX_GRAPH_NUM];

    // 默认配置文件路径
    QString _pathToConfig= "./mmw_pplcount_demo_default.cfg";
    // 串口设置
#if defined(__linux__)
    QString _portNameUART = "/dev/ttyACM0";
    QString _portNameAUX = "/dev/ttyACM1";
#elif defined(_WIN32)
    QString _portNameUART = "COM3";
    QString _portNameAUX = "COM4";
#endif
};

#endif // MAINWINDOW_H
