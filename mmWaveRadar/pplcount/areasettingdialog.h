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

private:
    Ui::AreaSettingDialog *ui;
    double _left = 6.0f;
    double _right = 6.0f;
    double _back = 6.0f;
    double _front =6.0f;

    double _rotateAngle = 0.0f;
    double _visionAngle = 0.0f;
};

#endif // AREASETTINGDIALOG_H
