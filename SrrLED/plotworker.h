#ifndef PLOTWORKER_H
#define PLOTWORKER_H

#include <QObject>
#include "qcustomplot.h"
#include "srrpacket.h"

class PlotWorker
{
public:
    PlotWorker() = delete;
    explicit PlotWorker(QCustomPlot *handler);

    void drawBackground();
    void drawSrrDetObj(vector<DetObj_t> &objs);
    void drawUsrrDetObjs(vector<DetObj_t> &objs);
    void drawClusters(vector<Cluster_t> &clusters);
    void drawTrackers(vector<Tracker_t> &trackers);
    void drawParkingAssitBins(vector<ParkingAssistBin_t> &objs);

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

    QCustomPlot *_handler = nullptr;
    bool _enableNearView = false;
    bool _enableSrrDetObj = true;
    bool _enableUsrrDetObj = true;
    bool _enableClusters = true;
    bool _enableTracker = true;
    bool _enableParkingAssitBins = false;
};

#endif // PLOTWORKER_H
