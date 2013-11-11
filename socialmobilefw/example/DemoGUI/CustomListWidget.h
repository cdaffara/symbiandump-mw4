#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <qlistwidget.h>
#include <smfcontact.h>
#include <smfpost.h>
#include <smfactions.h>
#include "qmainwindow.h"

class CustomListWidget : public QListWidget
{
public:
    CustomListWidget();
    //QWidget* CreateListwidget(QStrng aIconPath,QString aFirstLine,QString aSecondLine);
    QWidget* CreateListwidget(SmfContactList* friendsList);
    QWidget* CreateListwidget(SmfPostList *postsList);
    QWidget* CreateListwidget(SmfActivityEntryList *activityList);
    void AddListItem(QString aIconPath,QString aFirstLine,QString aSecondLine);
    void AddListItem(QMainWindow *mainWindow,QStringList aIconPath, QStringList iconName);
    ~CustomListWidget();

};


#endif // CUSTOMLISTWIDGET_H
