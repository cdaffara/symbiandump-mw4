#include "FriendView.h"
#include "ScreenSize.h"
#include "qpushbutton.h"
#include "ToolBar.h"
#include <qmessagebox.h>

//Forward declaration
class ScreenSize;
class CustomListWidget;
class GridView;

FriendView::FriendView(QString aHeadName):
        iHeadName(aHeadName)
{

}
FriendView::~FriendView()
{

}
void FriendView::SetupUI()
{
    //iMainwindow = Mainwindow;
    QPushButton* HomeButton = new QPushButton(this);
    HomeButton->setText(iHeadName);
    HomeButton->setGeometry(0,0,ScreenSize::GetScreenRect().width(),KHeadNameHeight);
    HomeButton->setStyleSheet("background-color: rgb(0,0,0);"
                             "border-width: 2px;"
                             "font: bold 16px;"
                             "min-width: 10em;"
                             "padding: 4px;"
							 "color: white;"
                            );

    HomeButton->setDisabled(true);

    TabWidget = new QTabWidget(this);

    iPostsListWidget = new CustomListWidget();
    TabWidget->addTab(iPostsListWidget,"Posts");

    AlbumView = new GridView();
    QStringList Iconpathlist;
    Iconpathlist.append("E:/Images/SMF/SMF.JPG");
    Iconpathlist.append("E:/Images/SMF/SMF.JPG");
    Iconpathlist.append("E:/Images/SMF/SMF.JPG");
    Iconpathlist.append("E:/Images/SMF/SMF.JPG");
    Iconpathlist.append("E:/Images/SMF/SMF.JPG");
    Iconpathlist.append("E:/Images/SMF/SMF.JPG");

    TabWidget->addTab(AlbumView->CreateGridView(Iconpathlist),"Album");

    TabWidget->setGeometry(0,KHeadNameHeight + KWidgetGapFactor,ScreenSize::GetScreenRect().width(),ScreenSize::GetScreenRect().height() - (ToolbarIconHeight + KWidgetGapFactor));

    connect(TabWidget,SIGNAL(currentChanged(int)),this,SLOT(Navigated2OtherTab(int)));
    //Drawing Tool bar
    ToolBar* ToolBarwidget = new ToolBar;
    QStringList actionList;
    actionList.append("Post");
    actionList.append("Reply");
    actionList.append("Add Service");
    ToolBarwidget->GetToolBar(this,actionList);
    showFullScreen();
}

void FriendView::Navigated2OtherTab(int tabIndex)
{
     if(tabIndex == 0)
        iPostsListWidget->AddListItem("E:/Images/SMF/SMF.JPG","Rajat","Hello !");

}
