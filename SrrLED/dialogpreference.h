#ifndef DIALOGPREFERENCE_H
#define DIALOGPREFERENCE_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "settings.h"

namespace Ui {
class DialogPreference;
}

class DialogPreference : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPreference(Settings *s, QWidget *parent = nullptr);
    ~DialogPreference();

private slots:
    void on_pbCancel_clicked();

    void on_pbSave_clicked();

private:
    void fillComboBoxes();

private:
    Ui::DialogPreference *ui;
    Settings *_settings;
};

#endif // DIALOGPREFERENCE_H
