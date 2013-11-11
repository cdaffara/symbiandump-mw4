#ifndef POSTTESTUI_H
#define POSTTESTUI_H

#include <QtGui/QWidget>
#include "ui_posttestui.h"

class PostTestUi : public QWidget
{
    Q_OBJECT

public:
    PostTestUi(QWidget *parent = 0);
    ~PostTestUi();

private:
    Ui::PostTestUiClass ui;
};

#endif // POSTTESTUI_H
