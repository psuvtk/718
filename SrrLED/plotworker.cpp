#include "plotworker.h"

PlotWorker::PlotWorker(QCustomPlot *canvasRange, QCustomPlot *canvasDoppler)
{
    _canvasRange = canvasRange;
    _canvasDoppler = canvasDoppler;
    drawBackground();
}

void PlotWorker::drawBackground()
{
    beginReplot();
    endReplot();
}

void PlotWorker::drawDetObj(vector<DetObj_t> &objs, int subframeNum)
{
    if (objs.size() == 0) return;

    if (subframeNum == 0) {
        _xSrr.clear();
        _ySrr.clear();
        _rangeSrr.clear();
        _dopplerSrr.clear();

        for (auto obj: objs) {
            _xSrr << obj.x;
            _ySrr << obj.y;
            _rangeSrr << obj.range;
            _dopplerSrr << obj.doppler;
        }
    } else {
        _xUsrr.clear();
        _yUsrr.clear();
        _rangeUsrr.clear();
        _dopplerUsrr.clear();

        for (auto obj: objs) {
            _xUsrr << obj.x;
            _yUsrr << obj.y;
            _rangeUsrr << obj.range;
            _dopplerUsrr << obj.doppler;
        }
    }

    int numRangeGraphs;
    int numDopplerGraphs;
    if (_enableSrrDetObj) {
        _canvasRange->addGraph();
        numRangeGraphs = _canvasRange->graphCount();
        _canvasRange->graph(numRangeGraphs-1)->setPen(QPen(Qt::darkYellow, 3));
        _canvasRange->graph(numRangeGraphs-1)->setLineStyle(QCPGraph::lsNone);
        _canvasRange->graph(numRangeGraphs-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
        _canvasRange->graph(numRangeGraphs-1)->setData(_xSrr, _ySrr);

        _canvasDoppler->addGraph();
        numDopplerGraphs = _canvasDoppler->graphCount();
        _canvasDoppler->graph(numDopplerGraphs-1)->setPen(QPen(Qt::darkYellow, 3));
        _canvasDoppler->graph(numDopplerGraphs-1)->setLineStyle(QCPGraph::lsNone);
        _canvasDoppler->graph(numDopplerGraphs-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
        _canvasDoppler->graph(numDopplerGraphs-1)->setData(_rangeSrr, _dopplerSrr);
    }


    if (_enableUsrrDetObj) {
        _canvasRange->addGraph();
        numRangeGraphs = _canvasRange->graphCount();
        _canvasRange->graph(numRangeGraphs-1)->setPen(QPen(Qt::darkCyan, 3));
        _canvasRange->graph(numRangeGraphs-1)->setLineStyle(QCPGraph::lsNone);
        _canvasRange->graph(numRangeGraphs-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
        _canvasRange->graph(numRangeGraphs-1)->setData(_xUsrr, _yUsrr);

        _canvasDoppler->addGraph();
        numDopplerGraphs = _canvasDoppler->graphCount();
        _canvasDoppler->graph(numDopplerGraphs-1)->setPen(QPen(Qt::darkCyan, 3));
        _canvasDoppler->graph(numDopplerGraphs-1)->setLineStyle(QCPGraph::lsNone);
        _canvasDoppler->graph(numDopplerGraphs-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
        _canvasDoppler->graph(numDopplerGraphs-1)->setData(_rangeSrr, _dopplerSrr);
    }

}

void PlotWorker::drawClusters(vector<Cluster_t> &clusters)
{
    if (!_enableClusters) return;
    if (clusters.size() == 0) return;

    for (auto cluster: clusters) {
        QCPItemRect *rect= new QCPItemRect(_canvasRange);
        rect->setPen(QPen(Qt::cyan , 3));
        rect->topLeft->setCoords(cluster.xCenter-cluster.xSize,
                                 cluster.yCenter+cluster.ySize);
        rect->bottomRight->setCoords(cluster.xCenter+cluster.xSize,
                                     cluster.yCenter-cluster.ySize);
    }
}

void PlotWorker::drawTrackers(vector<Tracker_t> &trackers)
{
    if (!_enableTracker) return;
    if (trackers.size() == 0) return;

    QVector<double> x, y;
    QVector<double> range, doppler;
    for (auto tracker: trackers) {
        x << tracker.x;
        y << tracker.y;
        range << tracker.range;
        doppler << tracker.doppler;
    }

    int numGraph;
    _canvasRange->addGraph();
    numGraph = _canvasRange->graphCount();
    _canvasRange->graph(numGraph-1)->setPen(QPen(Qt::green, 2));
    _canvasRange->graph(numGraph-1)->setLineStyle(QCPGraph::lsNone);
    _canvasRange->graph(numGraph-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 20));
    _canvasRange->graph(numGraph-1)->setData(x, y);

    _canvasDoppler->addGraph();
    numGraph = _canvasDoppler->graphCount();
    _canvasDoppler->graph(numGraph-1)->setPen(QPen(Qt::green, 2));
    _canvasDoppler->graph(numGraph-1)->setLineStyle(QCPGraph::lsNone);
    _canvasDoppler->graph(numGraph-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 20));
    _canvasDoppler->graph(numGraph-1)->setData(range, doppler);

    for (auto tracker: trackers) {
        QCPItemRect *rect= new QCPItemRect(_canvasRange);
        rect->setPen(QPen(Qt::green, 2));
        rect->topLeft->setCoords(tracker.x - tracker.xSize,
                                 tracker.y + tracker.ySize);
        rect->bottomRight->setCoords(tracker.x + tracker.xSize,
                                     tracker.y - tracker.ySize);
    }
}

void PlotWorker::drawParkingAssitBins(vector<ParkingAssistBin_t> &objs)
{
    if (!_enableParkingAssitBins) return;
    if (objs.size() == 0) return;

    // Not implement;
    qDebug() << "NOT ImpleMent PlotWorker::drawParkingAssitBins(vector<ParkingAssistBin_t> &objs)";
}

void PlotWorker::drawTarget(Tracker_t t)
{
    int numGraph;
    _canvasRange->addGraph();
    numGraph = _canvasRange->graphCount();
    _canvasRange->graph(numGraph-1)->setPen(QPen(Qt::red, 2));
    _canvasRange->graph(numGraph-1)->setLineStyle(QCPGraph::lsNone);
    _canvasRange->graph(numGraph-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 30));
    _canvasRange->graph(numGraph-1)->setData(QVector<double>{t.x}, QVector<double>{t.y});

    _canvasDoppler->addGraph();
    numGraph = _canvasDoppler->graphCount();
    _canvasDoppler->graph(numGraph-1)->setPen(QPen(Qt::red, 2));
    _canvasDoppler->graph(numGraph-1)->setLineStyle(QCPGraph::lsNone);
    _canvasDoppler->graph(numGraph-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 30));
    _canvasDoppler->graph(numGraph-1)->setData(QVector<double>{t.range}, QVector<double>{t.doppler});
}

void PlotWorker::beginReplot()
{
    if (_canvasRange->graphCount() != 0) _canvasRange->clearGraphs();
    if (_canvasRange->itemCount() != 0) _canvasRange->clearItems();
    if (_canvasDoppler->graphCount() != 0) _canvasDoppler->clearGraphs();
    if (_canvasDoppler->itemCount() != 0) _canvasDoppler->clearItems();
    __drawBackground();
}

void PlotWorker::endReplot()
{
    _canvasRange->setBackground(QBrush(Qt::darkBlue));
    _canvasRange->xAxis->setTickLabelColor(Qt::white);
    _canvasRange->xAxis->setBasePen(QPen(Qt::white));
    _canvasRange->xAxis->setTickPen(QPen(Qt::white));
    _canvasRange->xAxis->setSubTickPen(QPen(Qt::white));
    _canvasRange->yAxis->setTickLabelColor(Qt::white);
    _canvasRange->yAxis->setBasePen(QPen(Qt::white));
    _canvasRange->yAxis->setTickPen(QPen(Qt::white));
    _canvasRange->yAxis->setSubTickPen(QPen(Qt::white));
    _canvasRange->xAxis->grid()->setVisible(false);
    _canvasRange->yAxis->grid()->setVisible(false);

    _canvasDoppler->setBackground(QBrush(Qt::darkBlue));
    _canvasDoppler->xAxis->setTickLabelColor(Qt::white);
    _canvasDoppler->xAxis->setBasePen(QPen(Qt::white));
    _canvasDoppler->xAxis->setTickPen(QPen(Qt::white));
    _canvasDoppler->xAxis->setSubTickPen(QPen(Qt::white));
    _canvasDoppler->yAxis->setTickLabelColor(Qt::white);
    _canvasDoppler->yAxis->setBasePen(QPen(Qt::white));
    _canvasDoppler->yAxis->setTickPen(QPen(Qt::white));
    _canvasDoppler->yAxis->setSubTickPen(QPen(Qt::white));

    if (_enableNearView) {
        _canvasRange->xAxis->setRange(-8, 8);
        _canvasRange->yAxis->setRange(0, 16);
        _canvasDoppler->xAxis->setRange(0, 16);
        _canvasDoppler->yAxis->setRange(-4, 4);
    } else {
        _canvasRange->xAxis->setRange(-40, 40);
        _canvasRange->yAxis->setRange(0, 80);
        _canvasDoppler->xAxis->setRange(0, 80);
        _canvasDoppler->yAxis->setRange(-20, 20);
    }
    _canvasRange->xAxis->setLabel("Distance along lateral axis(meters)");
    _canvasRange->yAxis->setLabel("Distance along logitudinal axis(meters)");
    _canvasRange->xAxis->setLabelColor(Qt::darkGray);
    _canvasRange->yAxis->setLabelColor(Qt::darkGray);

    _canvasDoppler->xAxis->setLabel("Range (meters)");
    _canvasDoppler->yAxis->setLabel("Doppler (m/s)");
    _canvasDoppler->xAxis->setLabelColor(Qt::darkGray);
    _canvasDoppler->yAxis->setLabelColor(Qt::darkGray);

//    _canvasRange->legend->setVisible(true);
//    _canvasRange->legend->setFont(QFont("Helvetica", 9));
//    _canvasRange->legend->setRowSpacing(3);

    _canvasRange->replot();
    _canvasDoppler->replot();
}

void PlotWorker::__drawBackground()
{
    const double PI = 3.1415926535;

    for (int i = 30; i <= 150; i += 15) {
        QCPItemStraightLine *item = new QCPItemStraightLine(_canvasRange);
        item->setPen(QPen(Qt::gray));
        item->point1->setCoords(0, 0);
        item->point2->setCoords(cos(PI*i/180.0), sin(PI*i/180.0));
    }

    QVector<double> rs;
    rs << 15.0 << 30.0 << 45.0 << 60.0 << 75.0;
    for (auto r: rs) {
        QCPItemCurve *item = new QCPItemCurve(_canvasRange);
        item->setPen(QPen(Qt::darkGray));

        // 三次贝塞尔曲线拟合圆弧
        double a = 4.0/3*std::tan(PI*30.0/180);
        double sx = r*cos(PI*30/180.0);
        double sy = r*sin(PI*30/180.0);
        double ex = r*cos(PI*150/180.0);
        double ey = r*sin(PI*150/180.0);

        item->start->setCoords(sx, sy);
        item->end->setCoords(ex, ey);
        item->startDir->setCoords(sx - a*sy,  sy + a*sx);
        item->endDir->setCoords(ex + a*ey,  ey - a*ex);
    }
}

void PlotWorker::setEnableParkingAssitBins(bool enableParkingAssitBins)
{
    _enableParkingAssitBins = enableParkingAssitBins;
}

void PlotWorker::setEnableTracker(bool enableTracker)
{
    _enableTracker = enableTracker;
}

void PlotWorker::setEnableClusters(bool enableClusters)
{
    _enableClusters = enableClusters;
}

void PlotWorker::setEnableUsrrDetObj(bool enableUsrrDetObj)
{
    _enableUsrrDetObj = enableUsrrDetObj;
}

void PlotWorker::setEnableSrrDetObj(bool enableSrrDetObj)
{
    _enableSrrDetObj = enableSrrDetObj;
}

void PlotWorker::setEnableNearView(bool value)
{
    _enableNearView = value;
}
