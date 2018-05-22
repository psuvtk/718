#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fovsettingdialog.h"
#include "serialsettingdialog.h"
#include "mmwaveconfig.h"
#include "qcustomplot.h"

#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include <cassert>
#include <cmath>
#include <thread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionAboutQt, &QAction::triggered, [](){
        QMessageBox::aboutQt(NULL, "About Qt");
    });
    connect(ui->actionAboutUs, &QAction::triggered, [=](){
        QMessageBox::about(this,
                           "关于我们",
                           "海空电子<br>@author: Kristoffer<br>@email: psuvtk@gmail.com");
    });

    connect(ui->actionSerialSetting, &QAction::triggered,[=](){
        SerialSettingDialog dlg(this, _portNameUART, _portNameAUX);
        if (dlg.exec()) {
            qDebug() << "[+] 配置端口: " << this->_portNameUART;
            qDebug() << "[+] 数据端口: " << this->_portNameAUX;
            radar->setPort(_portNameUART, _portNameAUX);
        }
    });

    connect(ui->actionAreaSetting, &QAction::triggered, [&](){
        AreaSettingDialog dlg(this);
        if (dlg.exec()) {
            this->_updateScene();
            qDebug() << "area setting dld return TRUE!";
        }
    });

    connect(ui->actionRadarConfig, &QAction::triggered, [=](){
        this->_pathToConfig = QFileDialog::getOpenFileName(this);
        qDebug() << "[+] 更改雷达配置文件路径: " << this->_pathToConfig;
    });

    connect(ui->checkRedBlueBox, &QAction::toggled, [&](){
        _rbBoxEnabled = ui->checkRedBlueBox->isChecked();
        this->_updateScene();
    });

    connect(ui->checkFlipUpDown, &QAction::toggled, [&](){
        if (ui->checkFlipUpDown->isChecked()) {
            ui->plotPeopleCount->yAxis->setRangeReversed(true);
            ui->plotPointCloud->yAxis->setRangeReversed(true);
        } else {
            ui->plotPeopleCount->yAxis->setRangeReversed(false);
            ui->plotPointCloud->yAxis->setRangeReversed(false);
        }
        this->_replot();
    });

    connect(ui->checkFlipLeftRight, &QAction::toggled, [&](){
        if (ui->checkFlipLeftRight->isChecked()) {
            ui->plotPeopleCount->xAxis->setRangeReversed(true);
            ui->plotPointCloud->xAxis->setRangeReversed(true);
        } else {
            ui->plotPeopleCount->xAxis->setRangeReversed(false);
            ui->plotPointCloud->xAxis->setRangeReversed(false);
        }
        this->_replot();
    });

    connect(ui->actionDefaultSetting, &QAction::triggered, [&](){
        this->_rbBoxEnabled = true;
        ui->checkRedBlueBox->setChecked(true);
        ui->checkFlipUpDown->setChecked(false);
        ui->checkFlipLeftRight->setChecked(false);

        this->_canvasLeft = 6.0f;
        this->_canvasRight = 6.0f;
        this->_canvasFront = 6.0f;
        this->_canvasBack = 0.0f;

        this->_spinAngle = 0.0f;
        this->_viewAngle = 2 * PI / 3;

        ui->plotPeopleCount->xAxis->setRangeReversed(false);
        ui->plotPointCloud->xAxis->setRangeReversed(false);
        ui->plotPeopleCount->yAxis->setRangeReversed(false);
        ui->plotPointCloud->yAxis->setRangeReversed(false);

        this->_updateScene();
    });



    // 如果有多于两个串口， 则选取前两个，否则什么也不做，
    auto ports = QSerialPortInfo::availablePorts();
    if (ports.length() >= 2) {
#if defined(__linux__)
    _portNameUART = "/dev/" + ports[0].portName();
    _portNameAUX = "/dev/" + ports[1].portName();
#elif defined(_WIN32)
    _portNameUART = ports[0].portName();
    _portNameAUX = ports[0].portName();
#endif
    }
    qDebug() << "初始化...";
    qDebug() << "自动选择串口...";
    qDebug() << "配置端口: " << _portNameUART;
    qDebug() << "数据端口:" << _portNameAUX;

    radar = new MmWaveRadar(_portNameUART, _portNameAUX);

    this->_drawBackground();
    this->_drawBlueBox();
    this->_drawRedBox();
    this->_replot();

    for (int i = 0; i < MAX_GRAPH_NUM; i++) {
        auto color = TargetColor[i % 10];

        ui->plotPeopleCount->addGraph();
        _graphsTargetTrack[i] = ui->plotPeopleCount->graph(i);
        _graphsTargetTrack[i]->setLineStyle(QCPGraph::lsNone);
        _graphsTargetTrack[i]->setPen(QPen(QColor(color), 3));
        _graphsTargetTrack[i]->setBrush(QBrush(color, Qt::DiagCrossPattern));
        _graphsTargetTrack[i]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 32));


        ui->plotPointCloud->addGraph();
        _graphsPointTrace[i] = ui->plotPointCloud->graph(i);
        _graphsPointTrace[i]->setLineStyle(QCPGraph::lsNone);
        _graphsPointTrace[i]->setPen(QColor(color));
        _graphsPointTrace[i]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    }

    ui->plotPointCloud->addGraph();
    this->_graphPointCloud = ui->plotPointCloud->graph(MAX_GRAPH_NUM);
    _graphPointCloud->setLineStyle(QCPGraph::lsNone);
    _graphPointCloud->setPen(QPen(Qt::darkGray));
    _graphPointCloud->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));


    QTimer *timer=new QTimer(this);
    connect(timer, &QTimer::timeout, [&](){
        for (int i =0; i< MAX_GRAPH_NUM; i++) {
           _graphsPointTrace[i]->setData(QVector<double>(0), QVector<double>(0));
           _graphsTargetTrack[i]->setData(QVector<double>(0), QVector<double>(0));
        }
    });//timeoutslot()为自定义槽
    timer->start(3000);


    ui->lcdBlue->setStyleSheet("color:blue;");
    ui->lcdRed->setStyleSheet("color:red;");


    setWindowTitle("基于毫米波雷达人员计数演示");
    setFixedSize(1440, 900);
//    setFixedSize(1920, 1080);
//    ui->plotPeopleCount->setGeometry(960,10, 960,480);
//    ui->plotPointCloud->setGeometry(10,500, 960, 480);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete radar;
    delete _graphPointCloud;

    for (int i = 0; i < MAX_GRAPH_NUM; ++i) {
        delete _graphsTargetTrack[i];
        delete _graphsPointTrace[i];
    }
}



void MainWindow::_drawBackground()
{
    // 固定背景
    double angle = this->_viewAngle / 2.0f;
    double h = 4.0 /3 *(1-std::cos(angle))/std::sin(angle);
    double remAngle = PI/2 - angle;
    double x, y, a, b;
    double _bgRadius = std::min(this->_canvasLeft, this->_canvasFront);

    static QCPItemLine *_bgLine00 = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *_bgLine10 = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *_bgLine01 = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *_bgLine11 = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemCurve *_bgArc0 = new QCPItemCurve(ui->plotPointCloud);
    static QCPItemCurve *_bgArc1 = new QCPItemCurve(ui->plotPeopleCount);


    assert(_bgLine00 != nullptr);
    assert(_bgLine01 != nullptr);
    assert(_bgLine10 != nullptr);
    assert(_bgLine11 != nullptr);
    assert(_bgArc0 != nullptr);
    assert(_bgArc1 != nullptr);

    _bgLine00->setPen(QPen(Qt::darkBlue));
    _bgLine00->start->setCoords(0, 0);
    _bgLine10->setPen(QPen(Qt::darkBlue));
    _bgLine10->start->setCoords(0, 0);
    a = -_bgRadius*std::sin(angle);
    b = _bgRadius*std::cos(angle);
    x = a*cos(_spinAngle) - b*sin(_spinAngle);
    y = b*cos(_spinAngle)+a*sin(_spinAngle);
    _bgLine00->end->setCoords(x, y);
    _bgLine10->end->setCoords(x, y);

    _bgLine01->setPen(QPen(Qt::darkBlue));
    _bgLine01->start->setCoords(0, 0);
    _bgLine11->setPen(QPen(Qt::darkBlue));
    _bgLine11->start->setCoords(0, 0);
    a = _bgRadius*std::sin(angle);
    b = _bgRadius*std::cos(angle);
    x = a*cos(_spinAngle) - b*sin(_spinAngle);
    y = b*cos(_spinAngle) + a*sin(_spinAngle);
    _bgLine01->end->setCoords(x, y);
    _bgLine11->end->setCoords(x, y);

    _bgArc0->setPen(QPen(Qt::darkBlue));
    _bgArc1->setPen(QPen(Qt::darkBlue));
    // 贝叶斯曲线模拟圆弧(没有直接绘制圆弧的API)
    a = -_bgRadius*std::sin(angle);
    b = _bgRadius*std::cos(angle);
    x = a*cos(_spinAngle) - b*sin(_spinAngle);
    y = b*cos(_spinAngle)+a*sin(_spinAngle);
    _bgArc0->start->setCoords(x, y);
    _bgArc1->start->setCoords(x, y);

    a = _bgRadius*std::sin(angle);
    b = _bgRadius*std::cos(angle);
    x = a*cos(_spinAngle) - b*sin(_spinAngle);
    y = b*cos(_spinAngle)+a*sin(_spinAngle);
    _bgArc0->end->setCoords(x, y);
    _bgArc1->end->setCoords(x, y);

    a = _bgRadius*((cos(_viewAngle)+h*sin(_viewAngle))*cos(remAngle) - sin(remAngle)*(sin(_viewAngle)-h*cos(_viewAngle)));
    b = _bgRadius*(sin(_viewAngle)*cos(remAngle)+cos(_viewAngle)*sin(remAngle)-h*cos(_viewAngle)*cos(remAngle)+h*sin(_viewAngle)*sin(remAngle));
    x = a*cos(_spinAngle) - b*sin(_spinAngle);
    y = b*cos(_spinAngle)+a*sin(_spinAngle);
    _bgArc0->startDir->setCoords(x, y);
    _bgArc1->startDir->setCoords(x, y);

    a = _bgRadius* (cos(remAngle) - h* sin(remAngle));
    b = _bgRadius * (h*cos(remAngle) + sin(remAngle));
    x = a*cos(_spinAngle) - b*sin(_spinAngle);
    y = b*cos(_spinAngle)+a*sin(_spinAngle);
    _bgArc0->endDir->setCoords(x, y);
    _bgArc1->endDir->setCoords(x, y);

    double dx = 0.5;
    ui->plotPointCloud->xAxis->setRange(-_canvasLeft-dx, _canvasRight+dx);
    ui->plotPeopleCount->xAxis->setRange(-_canvasLeft-dx, _canvasRight+dx);
    ui->plotPeopleCount->yAxis->setRange(_canvasBack, _canvasFront);
    ui->plotPointCloud->yAxis->setRange(_canvasBack, _canvasFront);


}

void MainWindow::_drawRedBox(QPen pen)
{
    double x, y;
    static QCPItemLine *lineTop = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineBottom = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineLeft = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineRight = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineTopPoint = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *lineBottomPoint = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *lineLeftPoint = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *lineRightPoint = new QCPItemLine(ui->plotPointCloud);


    if (_rbBoxEnabled) {
        lineTop->setPen(pen);
        lineTopPoint->setPen(pen);
        lineBottom->setPen(pen);
        lineBottomPoint->setPen(pen);
        lineLeft->setPen(pen);
        lineLeftPoint->setPen(pen);
        lineRight->setPen(pen);
        lineRightPoint->setPen(pen);
    } else {
        //隐藏
        lineTop->setPen(QPen(Qt::transparent));
        lineTopPoint->setPen(QPen(Qt::transparent));
        lineBottom->setPen(QPen(Qt::transparent));
        lineBottomPoint->setPen(QPen(Qt::transparent));
        lineLeft->setPen(QPen(Qt::transparent));
        lineLeftPoint->setPen(QPen(Qt::transparent));
        lineRight->setPen(QPen(Qt::transparent));
        lineRightPoint->setPen(QPen(Qt::transparent));
    }

    _rotateCoord( _redTopLeftX, _redTopLeftY, &x, &y);
    lineTop->start->setCoords(x, y);
    lineLeft->end->setCoords(x, y);
    lineTopPoint->start->setCoords(x, y);
    lineLeftPoint->end->setCoords(x, y);


    _rotateCoord( _redBottomRightX, _redTopLeftY, &x, &y);
    lineTop->end->setCoords(x, y);
    lineRight->start->setCoords(x, y);
    lineTopPoint->end->setCoords(x, y);
    lineRightPoint->start->setCoords(x, y);

    _rotateCoord(_redBottomRightX, _redBottomRightY, &x, &y);
    lineRight->end->setCoords(x, y);
    lineBottom->start->setCoords(x, y);
    lineRightPoint->end->setCoords(x, y);
    lineBottomPoint->start->setCoords(x, y);

    _rotateCoord(_redTopLeftX, _redBottomRightY,&x, &y);
    lineBottom->end->setCoords(x, y);
    lineLeft->start->setCoords(x, y);
    lineBottomPoint->end->setCoords(x, y);
    lineLeftPoint->start->setCoords(x, y);


}

void MainWindow::_rotateCoord(double a, double b, double *x, double *y)
{
    *x = a*cos(_spinAngle) - b*sin(_spinAngle);
    *y = b*cos(_spinAngle) + a*sin(_spinAngle);
}

bool MainWindow::_isInRedBox(double x, double y)
{
    double a, b;
    // rotate back
    a = x*cos(_spinAngle) + y*sin(_spinAngle);
    b = y*cos(_spinAngle) - x*sin(_spinAngle);

    return  a > _redTopLeftX
            && a < _redBottomRightX
            && b > _redBottomRightY
            && b < _redTopLeftY;

}

bool MainWindow::_isInBlueBox(double x, double y)
{
    double a, b;
    // rotate back
    a = x*cos(_spinAngle) + y*sin(_spinAngle);
    b = y*cos(_spinAngle) - x*sin(_spinAngle);

    return  a > _blueTopLeftX
            && a < _blueBottomRightX
            && b > _blueBottomRightY
            && b < _blueTopLeftY;
}

void MainWindow::_drawBlueBox(QPen pen)
{
    double x, y;
    static QCPItemLine *lineTop = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineBottom = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineLeft = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineRight = new QCPItemLine(ui->plotPeopleCount);
    static QCPItemLine *lineTopPoint = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *lineBottomPoint = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *lineLeftPoint = new QCPItemLine(ui->plotPointCloud);
    static QCPItemLine *lineRightPoint = new QCPItemLine(ui->plotPointCloud);


    if (_rbBoxEnabled) {
        lineTop->setPen(pen);
        lineTopPoint->setPen(pen);
        lineBottom->setPen(pen);
        lineBottomPoint->setPen(pen);
        lineLeft->setPen(pen);
        lineLeftPoint->setPen(pen);
        lineRight->setPen(pen);
        lineRightPoint->setPen(pen);
    } else {
        //隐藏
        lineTop->setPen(QPen(Qt::transparent));
        lineTopPoint->setPen(QPen(Qt::transparent));
        lineBottom->setPen(QPen(Qt::transparent));
        lineBottomPoint->setPen(QPen(Qt::transparent));
        lineLeft->setPen(QPen(Qt::transparent));
        lineLeftPoint->setPen(QPen(Qt::transparent));
        lineRight->setPen(QPen(Qt::transparent));
        lineRightPoint->setPen(QPen(Qt::transparent));
    }

    _rotateCoord( _blueTopLeftX, _blueTopLeftY, &x, &y);
    lineTop->start->setCoords(x, y);
    lineLeft->end->setCoords(x, y);
    lineTopPoint->start->setCoords(x, y);
    lineLeftPoint->end->setCoords(x, y);


    _rotateCoord( _blueBottomRightX, _blueTopLeftY, &x, &y);
    lineTop->end->setCoords(x, y);
    lineRight->start->setCoords(x, y);
    lineTopPoint->end->setCoords(x, y);
    lineRightPoint->start->setCoords(x, y);

    _rotateCoord(_blueBottomRightX, _blueBottomRightY, &x, &y);
    lineRight->end->setCoords(x, y);
    lineBottom->start->setCoords(x, y);
    lineRightPoint->end->setCoords(x, y);
    lineBottomPoint->start->setCoords(x, y);

    _rotateCoord(_blueTopLeftX, _blueBottomRightY,&x, &y);
    lineBottom->end->setCoords(x, y);
    lineLeft->start->setCoords(x, y);
    lineBottomPoint->end->setCoords(x, y);
    lineLeftPoint->start->setCoords(x, y);
}

void MainWindow::_replot()
{
    ui->plotPointCloud->replot();
    ui->plotPeopleCount->replot();
}




void MainWindow::queryArea(double *left, double *right, double *front, double *back)
{
    *left = this->_canvasLeft;
    *right = this->_canvasRight;
    *front = this->_canvasFront;
    *back = this->_canvasBack;
}

void MainWindow::queryAngle(double *va, double *ra)
{
    *va = this->_viewAngle/PI*180;
    *ra = this->_spinAngle/PI*180;
}

void MainWindow::queryRedBoxShape(double *a, double *b, double *c, double *d)
{
    *a = _redTopLeftX;
    *b = _redTopLeftY;
    *c = _redBottomRightX;
    *d = _redBottomRightY;
}

void MainWindow::queryBlueBoxShape(double *a, double *b, double *c, double *d)
{
    *a = _blueTopLeftX;
    *b = _blueTopLeftY;
    *c = _blueBottomRightX;
    *d = _blueBottomRightY;
}

void MainWindow::setArea(double left, double right, double front, double back)
{
    this->_canvasBack = back;
    this->_canvasFront = front;
    this->_canvasLeft = left;
    this->_canvasRight = right;
}

void MainWindow::setAngle(double va, double ra)
{
    this->_viewAngle = va / 180.0 * PI;
    qDebug() << "Pi: " << PI;
    qDebug() << "this->visonAngle" << this->_viewAngle;
    this->_spinAngle = ra / 180.0 * PI;
    qDebug() << "this->rotateAngle" << this->_spinAngle;
}

void MainWindow::setRedBoxShape(double a, double b, double c, double d)
{
    this->_redTopLeftX = a;
    this->_redTopLeftY = b;
    this->_redBottomRightX = c;
    this->_redBottomRightY = d;
}

void MainWindow::setBlueBoxShape(double a, double b, double c, double d)
{
    this->_blueTopLeftX = a;
    this->_blueTopLeftY = b;
    this->_blueBottomRightX = c;
    this->_blueBottomRightY = d;
}


void MainWindow::on_btnRadarOff_clicked()
{
    if (radar->sendCmd("sensorStop\n")) {
        qDebug() << "[+] Done: Stop Radar.";
    } else {
        QMessageBox::critical(this,
                              "Fatal",
                              "错误：不能停止雷达，请重试！");
        qDebug() << "Fatal: cant stop radar.";
    }

}


void MainWindow::on_btnSendConfig_clicked()
{
    if (radar->isRunning()) {
        qDebug() << "[+] 雷达运行中...";
        ui->btnSendConfig->setStyleSheet("background-color:green;color: white;");
        ui->btnStartDetect->setEnabled(true);
        ui->btnSendConfig->setEnabled(false);
    }


    // 针对第一次发送 会出现 不是 CLI Command
    QByteArray radarRetStr;
    if(!radar->config(_pathToConfig, radarRetStr)) {
        qDebug() << "错误: 雷达返回"<< radarRetStr;
        if(radarRetStr.contains("Error") || radarRetStr.length() == 0 || !radar->config(_pathToConfig)){
            qDebug() << "重新配置";
            QMessageBox::critical(this,
                                  "Fatal",
                                  "错误：配置毫米波雷达失败\n请检查串口配置是否正确！");
            ui->btnSendConfig->setStyleSheet("background-color:red;");
            return;
        }
    }

    qDebug() << "[+] 成功配置！";

    ui->btnSendConfig->setStyleSheet("background-color:green;color: white;");
    ui->btnSendConfig->setEnabled(false);
    ui->btnStartDetect->setEnabled(true);
    ui->btnRadarOff->setEnabled(true);
}


void MainWindow::on_btnStartDetect_clicked()
{
    std::thread t(MainWindow::listen_wrapper, this);
    t.detach();
}


void MainWindow::listen_wrapper(MainWindow *p)
{
    const char *frame;
    QByteArray bufRecv;
    QByteArray bufFrame;
    qint64 skipLength;

    if (!p->radar->isAuxOpen()) {
        p->radar->openSerialAux();
    }

    if (!p->radar->isRunning()){
        p->radar->sensorStart();
    }

    const QByteArray SYNC = RadarPacket::getSync();
//    qDebug() << "等待雷达数据";


labelRecover:
    try{
        while (p->radar->waitForReadyRead()) {
            bufRecv.append(p->radar->readAll());
    //        qDebug() << "-----bufRecv: " << bufRecv.toHex() << "\n";

            // 尝试读取一帧
            if (bufRecv.startsWith(SYNC)) {
                skipLength = bufRecv.indexOf(SYNC, 8);
                if (skipLength != -1) {
                    bufFrame.append(bufRecv.left(skipLength));
                } else {
                    continue;
                }
            } else {
                skipLength = bufRecv.indexOf(SYNC);
                // 一下两种情况可以规约成一种情况
                if (skipLength != -1) {
                    qDebug() << "未检测到同步码元";
                    continue;
                } else {
                    bufRecv.clear();
                    qDebug() << "未检测到同步码元" << "\n";
                    continue;
                }
            }

            //判断数据包是否有效
            frame = bufFrame.data();
            if (bufFrame.length() == RadarPacket::getPacketLength(frame)) {
                qDebug() << "有效数据包";
                qDebug() << "Frame:"<< bufFrame.toHex() << "\n";

                // do something
                p->_parseFrame(frame);

                bufRecv.remove(0, skipLength);
                bufFrame.clear();
            } else {
                bufRecv.remove(0, skipLength);
                bufFrame.clear();
                qDebug() << "无效数据包";
            }
        }
    } catch(...){
        bufRecv.clear();
        bufFrame.clear();
        qDebug() << "Recover from exception.";
        goto labelRecover;
    }
}



// 给周青采集点云
void MainWindow::collectPointCloud(const char *frame, int nTLVs)
{
    quint32 type, nTlvBody, lengthTLV;
    const char *tlv = RadarPacket::getFrameBody(frame);
    FILE *fp = fopen("./points.txt", "a");

    qDebug() << "\nTimestamp: "<< RadarPacket::getTimeStamp(frame);
    for (int i = 0; i < nTLVs; i++){
        type = RadarPacket::getTlvHeaderType(tlv);
        lengthTLV = RadarPacket::getTlvHeaderLength(tlv);
        nTlvBody = (lengthTLV - RadarPacket::sizeTlvHeader) / RadarPacket::sizePointStruct;
        qDebug() << "TLV Type: " << type;
        qDebug() << "nTlvBody: " << nTlvBody;
        QVector<double> x(nTlvBody), y(nTlvBody);
        if (type == 0x0006){
            qDebug() << "nPoints(Range - Azimuth): " << nTlvBody;
            tlv += RadarPacket::sizeTlvHeader;
            for (int j = 0; j < nTlvBody; j++, tlv+=RadarPacket::sizePointStruct) {
                float range = RadarPacket::getPointRange(tlv);
                float azimuth = RadarPacket::getPointAzimuth(tlv);

                x[j] = range * std::sin(azimuth);
                y[j] = range * std::abs(std::cos(azimuth));
                qDebug() << "range, azi: " << range << azimuth;
                qDebug() << "x, y: " << x[j]<< y[j];
            }

            _graphPointCloud->setData(x, y);
            ui->plotPointCloud->replot();
        } else {
            fclose(fp);
            return;
        }
    }
}

void MainWindow::_updateScene()
{
    this->_drawBackground();
    this->_drawBlueBox();
    this->_drawRedBox();
    this->_replot();
}


void MainWindow::_parseFrame(const char *frame)
{
    quint32 lengthTLV;
    RadarPacket::TLVTYPE type;

    const char *tlv = RadarPacket::getFrameBody(frame);
    quint32 nTLVs = RadarPacket::getNumTLVs(frame);

    for (int i = 0; i < nTLVs; i++, tlv += lengthTLV){
        type = RadarPacket::getTlvHeaderType(tlv);
        lengthTLV = RadarPacket::getTlvHeaderLength(tlv);


        switch (type) {
        case RadarPacket::TLVTYPE::POINT_CLOUD_2D:  qDebug() << "点云";
                                                    _parsePoint(tlv);
                                                    break;
        case RadarPacket::TLVTYPE::TARGET_LIST_2D:  qDebug() << "目标列表";
                                                    _parseTartget(tlv);
                                                    break;
        case RadarPacket::TLVTYPE::TARGET_INDEX:    qDebug() << "目标索引";
                                                    _parseTartgetIdx(tlv);
                                                    break;
        }
    }
}



void MainWindow::_parsePoint(const char *tlv)
{
    quint32 lengthTLV = RadarPacket::getTlvHeaderLength(tlv);
    quint32 nTlvBody = (lengthTLV - RadarPacket::sizeTlvHeader) / RadarPacket::sizePointStruct;
    QVector<double> x(nTlvBody), y(nTlvBody);

//    qDebug() << "nPoints(Range - Azimuth): " << nTlvBody;
    tlv += RadarPacket::sizeTlvHeader;
    for (int j = 0; j < nTlvBody; j++, tlv+=RadarPacket::sizePointStruct) {
        float range = RadarPacket::getPointRange(tlv);
        float azimuth = RadarPacket::getPointAzimuth(tlv);

        x[j] = range * std::sin(azimuth);
        y[j] = range * std::abs(std::cos(azimuth));
//        qDebug() << "range, azi: " << range << azimuth;
//        qDebug() << "x, y: " << x[j]<< y[j];
    }

    _graphPointCloud->setData(x, y);
    ui->plotPointCloud->replot();
}


void MainWindow::_parseTartget(const char *tlv)
{
    static QVector<double> traceX(128, 0), traceY(128, 0);     //轨迹

    quint32 lengthTLV = RadarPacket::getTlvHeaderLength(tlv);
    quint32 nTlvBody = (lengthTLV - RadarPacket::sizeTlvHeader) / RadarPacket::sizeTargetStruct;
    QVector<double> x(nTlvBody), y(nTlvBody);

    // 判断红蓝并判断人数、显示
    ui->lcdTotal->display(QString::number(nTlvBody));
    int r=0, b=0;

    tlv += RadarPacket::sizeTlvHeader;
    for (int j = 0; j < nTlvBody; j++, tlv+=RadarPacket::sizeTargetStruct) {
        x[j] = RadarPacket::getTargetPosX(tlv);
        y[j] = RadarPacket::getTargetPosY(tlv);
        quint32 tid = RadarPacket::getTargetTid(tlv);
        qDebug() << "x[j]:" << x[j];
        qDebug() << "y[j]:" << y[j];


        traceX.push_back(x[j]);
        traceY.push_back(y[j]);

        if (!traceX.isEmpty() && !traceY.isEmpty()) {
            traceX.pop_front();
            traceY.pop_front();
        }

        _graphsPointTrace[tid % MAX_GRAPH_NUM]->setData(traceX, traceY);
        _graphsTargetTrack[j % MAX_GRAPH_NUM]->setData(QVector<double>(1, x[j]), QVector<double>(1, y[j]));

        if (_isInRedBox(x[j], y[j])) {
            r += 1;
        }
        if (_isInBlueBox(x[j], y[j])) {
            b += 1;
        }

        if (r != 0) {
            this->_drawRedBox(QPen(Qt::red, 6));
        } else {
            // 恢复默认形状
            this->_drawRedBox();
        }
        if (b != 0) {
            this->_drawBlueBox(QPen(Qt::blue, 6));
        } else {
            this->_drawBlueBox();
        }

        this->_replot();
    }
    ui->lcdRed->display(QString::number(r));
    ui->lcdBlue->display(QString::number(b));
}

void MainWindow::_parseTartgetIdx(const char *tlv)
{

}
