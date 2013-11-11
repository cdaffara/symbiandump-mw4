#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <qobject.h>
#include <qmainwindow.h>
#include <qboxlayout.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qsignalmapper.h>
#include "WidgetConstants.h"
#include <qmenu.h>

//FaceBook AuthApp UID
const TUint32 KFBUid3 = { 0xEFE2FD23 }; 
const TUint32 KFlickUid3 = { 0xE1D8C7D7 };

class ToolBar : public QObject
{
    Q_OBJECT
public:
    ToolBar();
    void GetToolBar(QMainWindow *Mainwindow,QStringList aListOfActions);
    void launchFB();
    void launchFlickr();
    ~ToolBar();
public slots:
    //void post();
    //void reply();
    void exitApp();
    void homeView();
    void msg();
    void call();
    void authApp_Fb();
    void authApp_Flkr();
private:
     QMainWindow *iMainWindow;
     QSignalMapper *signal;
public:
	QMenu* menu;
	//QAction *newAction1;
	//QAction *newAction2;
};

#endif // TOOLBAR_H
