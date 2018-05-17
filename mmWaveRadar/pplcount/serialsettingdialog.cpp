#include "serialsettingdialog.h"
#include "ui_serialsettingdialog.h"
#include <QSerialPortInfo>

SerialSettingDialog::SerialSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialSettingDialog)
{
    ui->setupUi(this);
    setWindowTitle("串口配置");
}

SerialSettingDialog::SerialSettingDialog(QWidget *parent, QString &uart, QString &aux)
    : SerialSettingDialog()
{
    connect(ui->buttonBox, &QDialogButtonBox::accepted, [&](){
        uart = ui->lineEditUart->text().trimmed();
        aux = ui->lineEditAux->text().trimmed();
    });

    ui->lineEditUart->setText(uart);
    ui->lineEditAux->setText(aux);
}

SerialSettingDialog::~SerialSettingDialog()
{
    delete ui;
}
