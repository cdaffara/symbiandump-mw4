#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QtGui/QWidget>
#include "ui_progressbar.h"

class progressbar : public QWidget
{
    Q_OBJECT

public:
    progressbar(QWidget *parent = 0);
    ~progressbar();

private:
    Ui::progressbarClass ui;
};

#endif // PROGRESSBAR_H
