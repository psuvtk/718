#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "areasettingdialog.h"
#include "qcustomplot.h"
#include "mmwaveconfig.h"
#include "serialsettingdialog.h"

#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
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
                           "OSET",
                           "海空电子<br>@author: Kristoffer<br>@email: psuvtk@gmail.com");
    });

    connect(ui->actionSerialSetting, &QAction::triggered,[=](){
        SerialSettingDialog dlg(this, portNameUART, portNameAUX);
        if (dlg.exec()) {
            qDebug() << "[+] 配置端口: " << this->portNameUART;
            qDebug() << "[+] 数据端口: " << this->portNameAUX;
            radar->setPort(portNameUART, portNameAUX);
        }
    });

    connect(ui->actionAreaSetting, &QAction::triggered, [&](){
        AreaSettingDialog dlg(this);
        if (dlg.exec()) {
            this->reDraw();
            qDebug() << "area setting dld return TRUE!";
        }
    });

    connect(ui->actionRadarConfig, &QAction::triggered, [=](){
        this->pathToConfig = QFileDialog::getOpenFileName(this);

        qDebug() << "[+] 更改雷达配置文件路径: " << this->pathToConfig;
    });

    connect(ui->checkRedBlueBox, &QAction::toggled, [&](){
        rbBoxEnabled = ui->checkRedBlueBox->isChecked();
        this->_drawBackground();
    });

    // 如果有多于两个串口， 则选取前两个，否则什么也不做，
    auto ports = QSerialPortInfo::availablePorts();
    if (ports.length() >= 2) {
#if defined(__linux__)
    portNameUART = "/dev/" + ports[0].portName();
    portNameAUX = "/dev/" + ports[1].portName();
#elif defined(_WIN32)
    portNameUART = ports[0].portName();
    portNameAUX = ports[0].portName();
#endif
    }
    qDebug() << "初始化...";
    qDebug() << "自动选择串口...";
    qDebug() << "配置端口: " << portNameUART;
    qDebug() << "数据端口:" << portNameAUX;

    this->radar = new MmWaveRadar(portNameUART, portNameAUX);

    this->_drawBackground();

    //设置画图
//    ui->plotPointCloud->addGraph();
//    this->_graphPointCloud = ui->plotPointCloud->graph(0);
//    _graphPointCloud->setLineStyle(QCPGraph::lsNone);
//    _graphPointCloud->setPen(QPen(Qt::darkGray));
//    _graphPointCloud->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));

//    ui->plotPeopleCount->addGraph();
//    this->_graphTarget = ui->plotPeopleCount->graph(0);
//    _graphTarget->setLineStyle(QCPGraph::lsNone);
//    _graphTarget->setBrush(Qt::DiagCrossPattern);
//    _graphTarget->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 32));

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

    setWindowTitle("基于毫米波雷达人员计数演示");
    setFixedSize(1440, 900);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete radar;
    delete _graphPointCloud;
    delete _graphTarget;
}



void MainWindow::_drawBackground()
{
    // 固定背景
    double angle = this->visionAngle / 2.0f;
    double h = 4.0 /3 *(1-std::cos(angle))/std::sin(angle);
    double remAngle = pi/2 - angle;
    double x, y, a, b;

    // 清空背景重新绘制
    ui->plotPointCloud->clearItems();
    ui->plotPeopleCount->clearItems();

    QCPItemLine *line0 = new QCPItemLine(ui->plotPointCloud);
    QCPItemLine *line1 = new QCPItemLine(ui->plotPeopleCount);
    line0->setPen(QPen(Qt::darkBlue));
    line0->start->setCoords(0, 0);
    line1->setPen(QPen(Qt::darkBlue));
    line1->start->setCoords(0, 0);
    a = -r*std::sin(angle);
    b = r*std::cos(angle);
    x = a*cos(rotateAngle) - b*sin(rotateAngle);
    y = b*cos(rotateAngle)+a*sin(rotateAngle);
    line0->end->setCoords(x, y);
    line1->end->setCoords(x, y);

    QCPItemLine *line2 = new QCPItemLine(ui->plotPointCloud);
    QCPItemLine *line3 = new QCPItemLine(ui->plotPeopleCount);
    line2->setPen(QPen(Qt::darkBlue));
    line2->start->setCoords(0, 0);
    line3->setPen(QPen(Qt::darkBlue));
    line3->start->setCoords(0, 0);
    a = r*std::sin(angle);
    b = r*std::cos(angle);
    x = a*cos(rotateAngle) - b*sin(rotateAngle);
    y = b*cos(rotateAngle) + a*sin(rotateAngle);
    line2->end->setCoords(x, y);
    line3->end->setCoords(x, y);

    QCPItemCurve *curve = new QCPItemCurve(ui->plotPointCloud);
    QCPItemCurve *curve1 = new QCPItemCurve(ui->plotPeopleCount);
    curve->setPen(QPen(Qt::darkBlue));
    curve1->setPen(QPen(Qt::darkBlue));
    // 贝叶斯曲线模拟圆弧
    a = -r*std::sin(angle);
    b = r*std::cos(angle);
    x = a*cos(rotateAngle) - b*sin(rotateAngle);
    y = b*cos(rotateAngle)+a*sin(rotateAngle);
    curve->start->setCoords(x, y);
    curve1->start->setCoords(x, y);

    a = r*std::sin(angle);
    b = r*std::cos(angle);
    x = a*cos(rotateAngle) - b*sin(rotateAngle);
    y = b*cos(rotateAngle)+a*sin(rotateAngle);
    curve->end->setCoords(x, y);
    curve1->end->setCoords(x, y);

    a = r*((cos(visionAngle)+h*sin(visionAngle))*cos(remAngle) - sin(remAngle)*(sin(visionAngle)-h*cos(visionAngle)));
    b = r*(sin(visionAngle)*cos(remAngle)+cos(visionAngle)*sin(remAngle)-h*cos(visionAngle)*cos(remAngle)+h*sin(visionAngle)*sin(remAngle));
    x = a*cos(rotateAngle) - b*sin(rotateAngle);
    y = b*cos(rotateAngle)+a*sin(rotateAngle);
    curve->startDir->setCoords(x, y);
    curve1->startDir->setCoords(x, y);

    a = r* (cos(remAngle) - h* sin(remAngle));
    b = r * (h*cos(remAngle) + sin(remAngle));
    x = a*cos(rotateAngle) - b*sin(rotateAngle);
    y = b*cos(rotateAngle)+a*sin(rotateAngle);
    curve->endDir->setCoords(x, y);
    curve1->endDir->setCoords(x, y);
    if (rbBoxEnabled) {
        //红蓝框  左红右蓝
        QCPItemRect *rectRed0 = new QCPItemRect(ui->plotPointCloud);
        QCPItemRect *rectRed1 = new QCPItemRect(ui->plotPeopleCount);
        QCPItemRect *rectBlue0 = new QCPItemRect(ui->plotPointCloud);
        QCPItemRect *rectBlue1 = new QCPItemRect(ui->plotPeopleCount);
        rectRed0->setPen(QPen(Qt::red, 3));
        rectRed1->setPen(QPen(Qt::red, 3));
        rectBlue0->setPen(QPen(Qt::blue, 3));
        rectBlue1->setPen(QPen(Qt::blue, 3));

        a = _redTopLeftX;
        b = _redTopLeftY;
        x = a*cos(rotateAngle) - b*sin(rotateAngle);
        y = b*cos(rotateAngle)+a*sin(rotateAngle);
        rectRed0->setPen(QPen(Qt::red, 3));
        rectRed0->topLeft->setCoords(x, y);
        rectRed1->topLeft->setCoords(x, y);

        a = _redBottomRightX;
        b = _redBottomRightY;
        x = a*cos(rotateAngle) - b*sin(rotateAngle);
        y = b*cos(rotateAngle) + a*sin(rotateAngle);
        rectRed0->bottomRight->setCoords(x, y);
        rectRed1->bottomRight->setCoords(x, y);        

        a = _blueTopLeftX;
        b = _blueTopLeftY;
        x = a*cos(rotateAngle) - b*sin(rotateAngle);
        y = b*cos(rotateAngle) + a*sin(rotateAngle);
        rectBlue0->topLeft->setCoords(x, y);
        rectBlue1->topLeft->setCoords(x, y);
        a = _blueBottomRightX;
        b = _blueBottomRightY;
        x = a*cos(rotateAngle) - b*sin(rotateAngle);
        y = b*cos(rotateAngle) + a*sin(rotateAngle);
        rectBlue0->bottomRight->setCoords(x, y);
        rectBlue1->bottomRight->setCoords(x, y);
    }


    double dx = 0.5, dy = 0.5;
    ui->plotPointCloud->xAxis->setRange(-areaLeft-dx, areaRight+dx);
    ui->plotPeopleCount->xAxis->setRange(-areaLeft-dx, areaRight+dx);
    ui->plotPeopleCount->yAxis->setRange(areaBack, areaFront);
    ui->plotPointCloud->yAxis->setRange(areaBack, areaFront);

    ui->plotPointCloud->replot();
    ui->plotPeopleCount->replot();
}


void MainWindow::reDraw()
{
    this->_drawBackground();
    qDebug() << "Not yet Implement: reDraw!";
}

void MainWindow::queryArea(double *left, double *right, double *front, double *back)
{
    *left = this->areaLeft;
    *right = this->areaRight;
    *front = this->areaFront;
    *back = this->areaBack;
}

void MainWindow::queryAngle(double *va, double *ra)
{
    *va = this->visionAngle/pi*180;
    *ra = this->rotateAngle/pi*180;
}

void MainWindow::setArea(double left, double right, double front, double back)
{
    this->areaBack = back;
    this->areaFront = front;
    this->areaLeft = left;
    this->areaRight = right;
}

void MainWindow::setAngle(double va, double ra)
{
    this->visionAngle = va / 180.0 * pi;
    qDebug() << "Pi: " << pi;
    qDebug() << "this->visonAngle" << this->visionAngle;
    this->rotateAngle = ra / 180.0 * pi;
    qDebug() << "this->rotateAngle" << this->rotateAngle;
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
    if(!radar->config(pathToConfig, radarRetStr)) {
        qDebug() << "错误: 雷达返回"<< radarRetStr;
        if(radarRetStr.contains("Error") || radarRetStr.length() == 0 || !radar->config(pathToConfig)){
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
    quint16 numTLVs;


    if (!p->radar->isAuxOpen()) {
        p->radar->openSerialAux();
    }

    if (!p->radar->isRunning()){
        p->radar->sensorStart();
    }

    const QByteArray SYNC = RadarPacket::getSync();
    qDebug() << "等待雷达数据";


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

        _graphsPointTrace[tid%MAX_GRAPH_NUM]->setData(traceX, traceY);
        _graphsTargetTrack[j%MAX_GRAPH_NUM]->setData(QVector<double>(1, x[j]), QVector<double>(1, y[j]));

        if ( x[j] > _redTopLeftX && x[j] < _redBottomRightX
                    && y[j] > _redBottomRightY && y[j] < _redTopLeftY) {
            r += 1;
        }
        if ( x[j] > _blueTopLeftX && x[j] < _blueBottomRightX
                    && y[j] > _blueBottomRightY && y[j] < _redTopLeftY) {
            b += 1;
        }

        ui->plotPointCloud->replot();
        ui->plotPeopleCount->replot();
    }
    ui->lcdRed->display(QString::number(r));
    ui->lcdBlue->display(QString::number(b));
}

void MainWindow::_parseTartgetIdx(const char *tlv)
{

}


void MainWindow::on_pushButton_clicked()
{
    QMessageBox::critical(this,""," THanks!");
}
