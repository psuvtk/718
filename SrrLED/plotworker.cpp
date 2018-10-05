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

void PlotWorker::drawDetObj(const QVector<double> &x, const QVector<double> &y, QPen pen)
{
    _handler->addGraph();
    qDebug() << "Graph Count: "<< _handler->graphCount();

    _handler->graph(0)->setPen(pen);
    _handler->graph(0)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7.5));
    _handler->graph(0)->setData(x, y);
}

void PlotWorker::drawCluster(double xCenter, double yCenter, double xSize, double ySize, QPen pen)
{
    QCPItemRect *rect= new QCPItemRect(_handler);
    rect->setPen(pen);
    rect->topLeft->setCoords(xCenter-xSize, yCenter+ySize);
    rect->bottomRight->setCoords(xCenter+xSize, yCenter-ySize);
}

void PlotWorker::drawTracker(const QVector<double> &x, const QVector<double> &y, QPen pen)
{
    _handler->addGraph();
    int numGraph = _handler->graphCount();
    _handler->graph(numGraph-1)->setPen(pen);
    _handler->graph(numGraph-1)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(numGraph-1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 20));
    _handler->graph(numGraph-1)->setData(x, y);
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

void PlotWorker::setEnableNearView(bool value)
{
    _enableNearView = value;
}
