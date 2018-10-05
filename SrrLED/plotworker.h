#ifndef PLOTWORKER_H
#define PLOTWORKER_H

#include <QObject>
#include "qcustomplot.h"

class PlotWorker
{
public:
    PlotWorker() = delete;
    explicit PlotWorker(QCustomPlot *handler);

    void drawBackground();

    void drawDetObj(const QVector<double> &x, const QVector<double> &y, QPen pen = QPen(Qt::cyan, 2));
    void drawCluster(double xCenter, double yCenter, double xSize, double ySize, QPen pen = QPen(Qt::cyan , 3));
    void drawTracker(const QVector<double> &x, const QVector<double> &y, QPen pen = QPen(Qt::green, 2));

    void beginReplot();
    void endReplot();

    void setEnableNearView(bool value);

private:
    void __drawBackground();

    QCustomPlot *_handler = nullptr;
    bool _enableNearView = false;
};

#endif // PLOTWORKER_H
