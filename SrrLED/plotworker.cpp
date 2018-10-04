#include "plotworker.h"

PlotWorker::PlotWorker(QCustomPlot *handler)
{
    _handler = handler;
    __drawBackground();

}

void PlotWorker::drawRect(double xCenter, double yCenter, double xSize, double ySize)
{
    QCPItemRect *rect= new QCPItemRect(_handler);
    rect->setPen(QPen(Qt::cyan));
    rect->topLeft->setCoords(xCenter-xSize, yCenter+ySize);
    rect->bottomRight->setCoords(xCenter+xSize, yCenter-ySize);
}

void PlotWorker::drawDetObj(const QVector<double> &x, const QVector<double> &y)
{
    _handler->addGraph();
    _handler->graph(0)->setPen(QPen(Qt::red, 2));
    _handler->graph(0)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    _handler->graph(0)->setData(x, y);
}

void PlotWorker::drawTracker(const QVector<double> &x, const QVector<double> &y)
{
    _handler->addGraph();
    _handler->graph(1)->setPen(QPen(Qt::darkGreen, 2));
    _handler->graph(1)->setLineStyle(QCPGraph::lsNone);
    _handler->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 25));
    _handler->graph(1)->setData(x, y);
}


void PlotWorker::beginReplot()
{
    _handler->clearGraphs();
    _handler->clearItems();
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
    _handler->xAxis->setRange(-40, 40);
    _handler->yAxis->setRange(0, 80);
    _handler->xAxis->grid()->setVisible(false);
    _handler->yAxis->grid()->setVisible(false);

    _handler->replot();
    _handler->replot();
}

void PlotWorker::__drawBackground()
{
    const double PI = 3.1415926535;

    for (int i = 30; i <= 150; i+=15) {
        QCPItemStraightLine *item = new QCPItemStraightLine(_handler);
        item->setPen(QPen(Qt::white));
        item->point1->setCoords(0, 0);
        item->point2->setCoords(cos(PI*i/180.0), sin(PI*i/180.0));
    }

    QVector<double> rs;
    rs << 15.0 << 30.0 << 45.0 << 60.0 << 75.0;
    for (auto r: rs) {
        QCPItemCurve *item = new QCPItemCurve(_handler);
        item->setPen(QPen(Qt::white));

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
