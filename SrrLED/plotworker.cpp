#include "plotworker.h"

PlotWorker::PlotWorker(QCustomPlot *handler)
{
    _handler = handler;
    drawBackground();
}

void PlotWorker::drawBackground()
{
    beginReplot();
    endReplot();
}

void PlotWorker::drawSrrDetObj(vector<DetObj_t> &objs)
{
    if (!_enableSrrDetObj) return;
    if (objs.size() == 0) return;

    QVector<double> x, y;
    for (auto obj: objs) {
        x << obj.x;
        y << obj.y;
    }

    _handler->addGraph();
    int numGraphs = _handler->graphCount();
    _handler->graph(numGraphs-1)->setPen(QPen(Qt::darkYellow, 3));
    _handler->graph(numGraphs-1)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(numGraphs-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
    _handler->graph(numGraphs-1)->setData(x, y);
}

void PlotWorker::drawUsrrDetObjs(vector<DetObj_t> &objs)
{
    if (!_enableUsrrDetObj) return;
    if (objs.size() == 0) return;

    QVector<double> x, y;
    for (auto obj: objs) {
        x << obj.x;
        y << obj.y;
    }

    _handler->addGraph();
    int numGraphs = _handler->graphCount();
    _handler->graph(numGraphs-1)->setPen(QPen(Qt::darkCyan, 3));
    _handler->graph(numGraphs-1)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(numGraphs-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
    _handler->graph(numGraphs-1)->setData(x, y);
}

void PlotWorker::drawClusters(vector<Cluster_t> &clusters)
{
    if (!_enableClusters) return;
    if (clusters.size() == 0) return;

    for (auto cluster: clusters) {
        QCPItemRect *rect= new QCPItemRect(_handler);
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
    for (auto tracker: trackers) {
        x << tracker.x;
        y << tracker.y;

    }
    _handler->addGraph();
    int numGraph = _handler->graphCount();
    _handler->graph(numGraph-1)->setPen(QPen(Qt::green, 2));
    _handler->graph(numGraph-1)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(numGraph-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 20));
    _handler->graph(numGraph-1)->setData(x, y);

    for (auto tracker: trackers) {
    QCPItemRect *rect= new QCPItemRect(_handler);
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

void PlotWorker::beginReplot()
{
    if (_handler->graphCount() != 0) _handler->clearGraphs();
    if (_handler->itemCount() != 0) _handler->clearItems();
    __drawBackground();
}

void PlotWorker::endReplot()
{
    _handler->setBackground(QBrush(Qt::darkBlue));
    _handler->xAxis->setTickLabelColor(Qt::white);
    _handler->xAxis->setBasePen(QPen(Qt::white));
    _handler->xAxis->setTickPen(QPen(Qt::white));
    _handler->xAxis->setSubTickPen(QPen(Qt::white));
    _handler->yAxis->setTickLabelColor(Qt::white);
    _handler->yAxis->setBasePen(QPen(Qt::white));
    _handler->yAxis->setTickPen(QPen(Qt::white));
    _handler->yAxis->setSubTickPen(QPen(Qt::white));
    _handler->xAxis->grid()->setVisible(false);
    _handler->yAxis->grid()->setVisible(false);

    if (_enableNearView) {
        _handler->xAxis->setRange(-8, 8);
        _handler->yAxis->setRange(0, 16);
    } else {
        _handler->xAxis->setRange(-40, 40);
        _handler->yAxis->setRange(0, 80);
    }

    _handler->replot();
}

void PlotWorker::__drawBackground()
{
    const double PI = 3.1415926535;

    for (int i = 30; i <= 150; i+=15) {
        QCPItemStraightLine *item = new QCPItemStraightLine(_handler);
        item->setPen(QPen(Qt::gray));
        item->point1->setCoords(0, 0);
        item->point2->setCoords(cos(PI*i/180.0), sin(PI*i/180.0));
    }

    QVector<double> rs;
    rs << 15.0 << 30.0 << 45.0 << 60.0 << 75.0;
    for (auto r: rs) {
        QCPItemCurve *item = new QCPItemCurve(_handler);
        item->setPen(QPen(Qt::darkGray));

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
