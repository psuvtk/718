#ifndef AREASETTINGDIALOG_H
#define AREASETTINGDIALOG_H

#include <QDialog>
#include "mainwindow.h"


namespace Ui {
class AreaSettingDialog;
}

class AreaSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AreaSettingDialog(MainWindow *parent = 0);
    ~AreaSettingDialog();

private slots:
    void on_spinRotateAngle_valueChanged(double arg1);

    void on_spinVisionAngle_valueChanged(double arg1);

    void on_spinFront_valueChanged(double arg1);

    void on_spinBack_valueChanged(double arg1);

    void on_spinRight_valueChanged(double arg1);

    void on_spinLeft_valueChanged(double arg1);

    void on_spinRedTopLeftX_valueChanged(double arg1);

    void on_spinRedTopLeftY_valueChanged(double arg1);

    void on_spinRedBottomRightX_valueChanged(double arg1);

    void on_spinRedBottomRightY_valueChanged(double arg1);

    void on_spinBlueTopLeftX_valueChanged(double arg1);

    void on_spinBlueTopLeftY_valueChanged(double arg1);

    void on_spinBlueBottomRightX_valueChanged(double arg1);

    void on_spinBlueBottomRightY_valueChanged(double arg1);

private:
    Ui::AreaSettingDialog *ui;
    double _left;
    double _right;
    double _back;
    double _front;

    double _rotateAngle;
    double _visionAngle;

    double _redTopLeftX;
    double _redTopLeftY;
    double _redBottomRightX;
    double _redBottomRightY;
    double _blueTopLeftX;
    double _blueTopLeftY;
    double _blueBottomRightX;
    double _blueBottomRightY;
};

#endif // AREASETTINGDIALOG_H
