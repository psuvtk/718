#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include <QPlainTextEdit>

class DataDisplay : public QPlainTextEdit
{
    Q_OBJECT

public:
    DataDisplay(QWidget *parent=nullptr);
};

#endif // DATADISPLAY_H
