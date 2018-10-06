#ifndef PLOTWORKER_H
#define PLOTWORKER_H

#include <QObject>
#include "qcustomplot.h"
#include "srrpacket.h"

class PlotWorker
{
public:
    PlotWorker() = delete;
    explicit PlotWorker(QCustomPlot *canvasRange, QCustomPlot *canvasDoppler);

    void drawBackground();
    void drawDetObj(vector<DetObj_t> &objs, int subframeNum);
    void drawClusters(vector<Cluster_t> &clusters);
    void drawTrackers(vector<Tracker_t> &trackers);
    void drawParkingAssitBins(vector<ParkingAssistBin_t> &objs);
    void drawTarget(Tracker_t);

    void beginReplot();
    void endReplot();

    void setEnableNearView(bool value);

    void setEnableSrrDetObj(bool enableSrrDetObj);

    void setEnableUsrrDetObj(bool enableUsrrDetObj);

    void setEnableClusters(bool enableClusters);

    void setEnableTracker(bool enableTracker);

    void setEnableParkingAssitBins(bool enableParkingAssitBins);

private:
    void __drawBackground();

    QCustomPlot *_canvasRange = nullptr;
    QCustomPlot *_canvasDoppler = nullptr;
    bool _enableNearView = false;
    bool _enableSrrDetObj = true;
    bool _enableUsrrDetObj = true;
    bool _enableClusters = true;
    bool _enableTracker = true;
    bool _enableParkingAssitBins = false;

    QVector<double> _xSrr, _ySrr;
    QVector<double> _rangeSrr, _dopplerSrr;
    QVector<double> _xUsrr, _yUsrr;
    QVector<double> _rangeUsrr, _dopplerUsrr;
};

#endif // PLOTWORKER_H
