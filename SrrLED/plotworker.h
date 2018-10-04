#ifndef PLOTWORKER_H
#define PLOTWORKER_H

#include <QObject>
#include "qcustomplot.h"

class PlotWorker
{
public:
    PlotWorker() = delete;
    explicit PlotWorker(QCustomPlot *handler);

    void drawRect(double xCenter, double yCenter, double xSize, double ySize);
    void drawDetObj(const QVector<double> &x, const QVector<double> &y);
    void drawCluster();
    void drawTracker(const QVector<double> &x, const QVector<double> &y);

    void beginReplot();
    void endReplot();

private:
    void __drawRect();
    void __drawPoint();
    void __drawLine();
    void __drawBackground();


    QCustomPlot *_handler;
};

#endif // PLOTWORKER_H
