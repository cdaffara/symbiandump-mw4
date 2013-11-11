#include "ToolBar.h"
#include "ScreenSize.h"
#include <qinputdialog.h>
#include <qplaintextedit.h>
#include <qdir.h>   //Checl lateer
#include <qmessagebox.h>
#include <qdebug.h>
#include <qtooltip.h>
//#include <apacmdln.h>
//#include <apgcli.h>


//MACRO
#define KNullUid TUid::Null()

//Forward declaration
class ScreenSize;

ToolBar::ToolBar()
{

}
ToolBar::~ToolBar()
{

}
void ToolBar::GetToolBar(QMainWindow *Mainwindow,QStringList aListOfActions)
{
    //iMainWindow = new QMainWindow;
    iMainWindow = Mainwindow;

    QHBoxLayout* layout = new QHBoxLayout;
    QToolBar* tool = new QToolBar(Mainwindow);
    QToolButton* button1 = new QToolButton(Mainwindow);
    QToolButton* button2 = new QToolButton(Mainwindow);
    QToolButton* button3 = new QToolButton(Mainwindow);
    QToolButton* button4 = new QToolButton(Mainwindow);
    
    tool->setGeometry(0,ScreenSize::GetScreenRect().height() - (ToolbarIconHeight + KWidgetGapFactor),(ScreenSize::GetScreenRect().width()-(KWidgetGapFactor)),ToolbarIconHeight);
    
    //ToDo :- Ankit to check on button resizing
    const int IconWidth = ScreenSize::GetScreenRect().width()/7;
    button1->setGeometry(0,ScreenSize::GetScreenRect().height() - (ToolbarIconHeight + KWidgetGapFactor),IconWidth,ToolbarIconHeight);

    button1->setFixedWidth(IconWidth);
    button2->setFixedWidth(IconWidth);
    button3->setFixedWidth(IconWidth);
    button4->setFixedWidth(IconWidth);


    QIcon icon1("E:/Images/SMF/home.svg");
	QIcon icon2("E:/Images/SMF/all_friends.svg"); //next.PNG
	QIcon icon3("E:/Images/SMF/mail-message-new.svg");
	QIcon icon4("E:/Images/SMF/Phone.svg");
	QIcon icon5("E:/Images/SMF/system-log-out.svg");

    button1->setIcon(icon1);
    button2->setIcon(icon3);
    button3->setIcon(icon4);
    button4->setIcon(icon5);
    
    button1->setToolTip("Home");
    
    menu = new QMenu(Mainwindow);
    icon2.actualSize(QSize(IconWidth-10,ToolbarIconHeight));
    //menu->setFixedWidth(IconWidth+10);
    menu->setIcon(icon2);

    QMenu *addServ = new QMenu(Mainwindow);
    addServ->setTitle(aListOfActions.at(0));

    //For Facebook and Flickr SP
    QAction *addServ_Facebk = new QAction("FaceBook",Mainwindow);
    QAction *addServ_Flickr = new QAction("Flickr",Mainwindow);


    addServ->addAction(addServ_Facebk);
    addServ->addAction(addServ_Flickr);

   // }
    //Create Actions
    //newAction1 = new QAction(aListOfActions.at(1),Mainwindow);
    //newAction2 = new QAction(aListOfActions.at(2),Mainwindow);

    //need to be modified
    //menu->addAction(newAction1);
    //menu->addAction(newAction2);
    
    //to add services to the menu button
    menu->addMenu(addServ);
    
    //TODO: button to be disabled
    //button2->setEnabled(false);
    //button3->setEnabled(false);
    
    tool->addWidget(button1);
    tool->addAction(menu->menuAction());
    tool->addWidget(button2);
    tool->addWidget(button3);
    tool->addWidget(button4);

    layout->addWidget(tool);
    
    //connect actions
   connect(button1,SIGNAL(clicked()),this,SLOT(homeView()));
   connect(button2,SIGNAL(clicked()),this,SLOT(msg()));
   connect(button3,SIGNAL(clicked()),this,SLOT(call()));
   connect(button4,SIGNAL(clicked()),this,SLOT(exitApp()));
   //connect(newAction1,SIGNAL(triggered()),this,SLOT(post()));
   //connect(newAction2,SIGNAL(triggered()),this,SLOT(reply()));
   
   //connect for authentication
   connect(addServ_Facebk,SIGNAL(triggered()),this,SLOT(authApp_Fb()));
   connect(addServ_Flickr,SIGNAL(triggered()),this,SLOT(authApp_Flkr()));
   
}

//changing to home view
void ToolBar::homeView()
{
    //debug
    qDebug()<<"Inside View_Change()";
}



//slot for post comment
/*void ToolBar::post()
{
    //debug purpose
    qDebug()<<"Inside My Post";
    bool ok;
    QInputDialog inptDialg;
    inptDialg.setStyleSheet("background: black;");//rgb(255,255,255);");
    QString text = inptDialg.getText(iMainWindow, tr("QInputDialog::getText()"),tr("Post"),
                                         QLineEdit::Normal,QString::null, &ok,Qt::Window);


    if ( ok && !text.isEmpty() ) {
    // user entered something and pressed OK
    } else {
    // user entered nothing or pressed Cancel
    }


}*/

//slot for reply comment
/*void ToolBar::reply()
{
    //debug purpose
    qDebug()<<"Inside Reply";

    bool ok;
    QString text = QInputDialog::getText(iMainWindow, tr("QInputDialog::getText()"),tr("Reply To Post"),
                                         QLineEdit::Normal,QString::null, &ok,Qt::Window);


    if ( ok && !text.isEmpty() ) {
    // user entered something and pressed OK
    } else {
    // user entered nothing or pressed Cancel
    }


}*/
//calling 
//calling 
void ToolBar::call()
	{
		QMessageBox msgbox;
		QString msg ("Calling not configured!");
		msgbox.setText(msg);
		msgbox.exec();
	}



//msging
void ToolBar::msg()
	{
		QMessageBox msgbox;
		QString msg ("Messaging not configured!");
		msgbox.setText(msg);
		msgbox.exec();
	}
//exiting application
void ToolBar::exitApp()
{
    //qDebug()<<"Inside Exit_App()";
    QApplication::exit();
}


//Auth_App for FaceBook
void ToolBar::authApp_Fb()
{
    //debug
    qDebug()<<"Inside Auth app of faceBook";
    launchFB();
}

//Auth App for Flickr
void ToolBar::authApp_Flkr()
{
    //debug
    qDebug()<<"Inside Auth App of Flickr";
    launchFlickr();
}


//Facebook AuthApp Launch
void ToolBar::launchFB()
	{
	qDebug()<<"Inside Launch FB AuthApp";
	QMessageBox msgbox;
	QString msg ("Please Login Using Facebook Auth App!");
	msgbox.setText(msg);
	msgbox.exec();
	/*TInt result;
	_LIT(KExecutableName,"release\\winscw\\udeb\\AuthApp.exe");
	
	//we can ignore using UID
	//const TProcessId FBUid(KFBUid3);
	RProcess fbProc;
	result = fbProc.Create(KExecutableName,KNullDesC);
	if(result != KErrNone)
		{
		qDebug()<<"FB Can't be opened";
		return;
		}
	qDebug()<<"FB AuthApp Launched";
	fbProc.Resume();
	fbProc.Close();*/
	return;
	
	}

void ToolBar::launchFlickr()
	{
	qDebug()<<"Inside Launch FB AuthApp";
	QMessageBox msgbox;
	QString msg ("Please Login Using Flickr Auth App!");
	msgbox.setText(msg);
	msgbox.exec();
	/*TInt result;
	_LIT(KExecutableName,"release\\winscw\\udeb\\Flickr.exe");
	
	//we can ignore using UID
	//const TProcessId FBUid(KFBUid3);
	RProcess flickrProc;
	result = flickrProc.Create(KExecutableName,KNullDesC);
	if(result != KErrNone)
		{
		qDebug()<<"Flickr Can't be opened";
		return;
		}
	qDebug()<<"Flickr AuthApp Launched";
	flickrProc.Resume();
	flickrProc.Close();*/
	return;
	}
