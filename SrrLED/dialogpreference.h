#ifndef DIALOGPREFERENCE_H
#define DIALOGPREFERENCE_H

#include <QDialog>

namespace Ui {
class DialogPreference;
}

class DialogPreference : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPreference(QWidget *parent = 0);
    ~DialogPreference();

private slots:
    void on_pbCancel_clicked();

    void on_pbSave_clicked();

private:
    void fillDeviceComboBox();

private:
    Ui::DialogPreference *ui;
};

#endif // DIALOGPREFERENCE_H
