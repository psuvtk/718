#include "areasettingdialog.h"
#include "ui_areasettingdialog.h"
#include "mainwindow.h"
#include <QDialogButtonBox>
#include <QDebug>

AreaSettingDialog::AreaSettingDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::AreaSettingDialog)
{
    ui->setupUi(this);

    ui->spinRight->setMaximum(8);
    ui->spinRight->setMinimum(0);
    ui->spinLeft->setMaximum(8);
    ui->spinLeft->setMinimum(0);
    ui->spinFront->setMaximum(8);
    ui->spinFront->setMinimum(0);
    ui->spinBack->setMaximum(8);
    ui->spinBack->setMinimum(0);

    ui->spinVisionAngle->setMaximum(180);
    ui->spinVisionAngle->setMinimum(0);
    ui->spinRotateAngle->setMaximum(30);
    ui->spinRotateAngle->setMinimum(-30);

    parent->queryArea(&_left, &_right, &_front, &_back);
    parent->queryAngle(&_visionAngle, &_rotateAngle);

    ui->spinRight->setValue(_right);
    ui->spinLeft->setValue(_left);
    ui->spinFront->setValue(_front);
    ui->spinBack->setValue(_back);
    ui->spinVisionAngle->setValue(_visionAngle);
    ui->spinRotateAngle->setValue(_rotateAngle);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, [&](){
        qDebug() << "connecting accept setAngle set Area";
        parent->setAngle(_visionAngle, _rotateAngle);
        parent->setArea(_left, _right, _front, _back);
    });
}

AreaSettingDialog::~AreaSettingDialog()
{
    delete ui;
}

void AreaSettingDialog::on_spinRotateAngle_valueChanged(double arg1)
{
    this->_rotateAngle = arg1;

}

void AreaSettingDialog::on_spinVisionAngle_valueChanged(double arg1)
{
    this->_visionAngle = arg1;
}

void AreaSettingDialog::on_spinFront_valueChanged(double arg1)
{
    this->_front = arg1;
}

void AreaSettingDialog::on_spinBack_valueChanged(double arg1)
{
    this->_back = arg1;
}

void AreaSettingDialog::on_spinRight_valueChanged(double arg1)
{
    this->_right = arg1;
}

void AreaSettingDialog::on_spinLeft_valueChanged(double arg1)
{
    this->_left = arg1;
}
