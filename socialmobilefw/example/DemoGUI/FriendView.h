#ifndef FRIENDVIEW_H
#define FRIENDVIEW_H

#include <qmainwindow.h>
#include <WidgetConstants.h>
#include "customListwidget.h"
#include "GridView.h"
#include <qtablewidget.h>


class FriendView : public QMainWindow
{
    Q_OBJECT
public:
    FriendView(QString aHeadName);
    void SetupUI();
    ~FriendView();
private:
    QString iHeadName;
private slots:
    void Navigated2OtherTab(int tabIndex);
private:
    QMainWindow *iMainwindow;
    QTabWidget* TabWidget;
    GridView* AlbumView;
    CustomListWidget* iPostsListWidget;
};

#endif // FRIENDVIEW_H
