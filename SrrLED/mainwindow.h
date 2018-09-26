#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onSpeedChanged();

private:
    Ui::MainWindow *ui;
    QMutex _mutex;
    qint64 _speed;
};

#endif // MAINWINDOW_H
