#ifndef GRIDVIEW_H
#define GRIDVIEW_H

///Its not in use at this poin

#include <qwidget.h>
#include "WidgetConstants.h"
#include<qpushbutton.h>

class GridView : public QWidget
{
Q_OBJECT

public:
    GridView();
    void CreateGridView(QStringList aIconPath, QStringList names);
    ~GridView();

};


#endif // GRIDVIEW_H
