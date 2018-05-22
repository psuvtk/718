#include "fovsettingdialog.h"
#include "ui_fovsettingdialog.h"
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
    ui->spinRotateAngle->setMaximum(25);
    ui->spinRotateAngle->setMinimum(-25);

    parent->queryArea(&_left, &_right, &_front, &_back);
    parent->queryAngle(&_visionAngle, &_rotateAngle);
    parent->queryRedBoxShape(&_redTopLeftX, &_redTopLeftY, &_redBottomRightX, &_redBottomRightY);
    parent->queryBlueBoxShape(&_blueTopLeftX, &_blueTopLeftY, &_blueBottomRightX, &_blueBottomRightY);

    ui->spinRight->setValue(_right);
    ui->spinLeft->setValue(_left);
    ui->spinFront->setValue(_front);
    ui->spinBack->setValue(_back);
    ui->spinVisionAngle->setValue(_visionAngle);
    ui->spinRotateAngle->setValue(_rotateAngle);

    ui->spinRedTopLeftX->setValue(_redTopLeftX);
    ui->spinRedTopLeftY->setValue(_redTopLeftY);
    ui->spinRedBottomRightX->setValue(_redBottomRightX);
    ui->spinRedBottomRightY->setValue(_redBottomRightY);

    ui->spinBlueTopLeftX->setValue(_blueTopLeftX);
    ui->spinBlueTopLeftY->setValue(_blueTopLeftY);
    ui->spinBlueBottomRightX->setValue(_blueBottomRightX);
    ui->spinBlueBottomRightY->setValue(_blueBottomRightY);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, [&](){
        qDebug() << "connecting accept setAngle set Area";
        parent->setAngle(_visionAngle, _rotateAngle);
        parent->setArea(_left, _right, _front, _back);
        parent->setRedBoxShape(_redTopLeftX, _redTopLeftY, _redBottomRightX, _redBottomRightY);
        parent->setBlueBoxShape(_blueTopLeftX, _blueTopLeftY, _blueBottomRightX, _blueBottomRightY);
    });

    setWindowTitle("视场设置");
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



void AreaSettingDialog::on_spinRedTopLeftX_valueChanged(double arg1)
{
    this->_redTopLeftX = arg1;
}

void AreaSettingDialog::on_spinRedTopLeftY_valueChanged(double arg1)
{
    this->_redTopLeftY = arg1;
}

void AreaSettingDialog::on_spinRedBottomRightX_valueChanged(double arg1)
{
    this->_redBottomRightX = arg1;
}

void AreaSettingDialog::on_spinRedBottomRightY_valueChanged(double arg1)
{
    this->_redBottomRightY = arg1;
}

void AreaSettingDialog::on_spinBlueTopLeftX_valueChanged(double arg1)
{
    this->_blueTopLeftX = arg1;
}

void AreaSettingDialog::on_spinBlueTopLeftY_valueChanged(double arg1)
{
    this->_blueTopLeftY = arg1;
}

void AreaSettingDialog::on_spinBlueBottomRightX_valueChanged(double arg1)
{
    this->_blueBottomRightX = arg1;
}

void AreaSettingDialog::on_spinBlueBottomRightY_valueChanged(double arg1)
{
    this->_blueBottomRightY = arg1;
}
