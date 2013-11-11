#include "CustomListWidget.h"
#include <qlistwidget.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qdebug.h>
#include "ScreenSize.h"
#include <qpushbutton.h>
#include "WidgetConstants.h"

CustomListWidget::CustomListWidget()
{
     setStyleSheet("selection-color: yellow;"
                      "selection-background-color: grey;");
}
CustomListWidget::~CustomListWidget()
{

}

void CustomListWidget::AddListItem(QString aIconPath,QString aFirstLine,QString aSecondLine)
{
    QWidget* customwidget = new QWidget;
    QHBoxLayout* HMainlayout = new QHBoxLayout(customwidget);
    QLabel *lab =new QLabel();
    lab->setPixmap(QPixmap(aIconPath));
    lab->setFixedSize(60,60);
    HMainlayout->addWidget(lab);

    //To add First & second row horizontal layouts
    QVBoxLayout* VTextlayout = new QVBoxLayout;

    QHBoxLayout* HFirstrowLayout = new QHBoxLayout;
    //Add FirstLine label
    QLabel *lab1 =new QLabel(aFirstLine);
    lab1->setStyleSheet("font: bold 20px;");
    HFirstrowLayout->addWidget(lab1);
    //Add Images to
    QLabel *Img1lab =new QLabel();
    Img1lab->setPixmap(QPixmap(aIconPath));
    Img1lab->setFixedSize(10,10);
    QLabel *Img2lab =new QLabel();
    Img2lab->setPixmap(QPixmap(aIconPath));
    Img2lab->setFixedSize(10,10);
    QLabel *Img3lab =new QLabel();
    Img3lab->setPixmap(QPixmap(aIconPath));
    Img3lab->setFixedSize(10,10);

    HFirstrowLayout->addWidget(Img1lab);
    HFirstrowLayout->addWidget(Img2lab);
    HFirstrowLayout->addWidget(Img3lab);

//        HFirstrowLayout->setAlignment(Img3lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img2lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img1lab,Qt::AlignRight);

    VTextlayout->addLayout(HFirstrowLayout);


     QHBoxLayout* HSecondrowLayout = new QHBoxLayout;
    QLabel *lab2 =new QLabel(aSecondLine);
    lab2->setStyleSheet("font: 16px;");
    HSecondrowLayout->addWidget(lab2);

    QLabel *Timestamplabel =new QLabel("00:00 JUL 12");
    Timestamplabel->setStyleSheet("font: 12px;");
    HSecondrowLayout->addWidget(Timestamplabel);

    HSecondrowLayout->setAlignment(Timestamplabel,Qt::AlignRight);

    VTextlayout->addLayout(HSecondrowLayout);


    HMainlayout->addLayout(VTextlayout);
    customwidget->setLayout(HMainlayout);
    QListWidgetItem *item=new QListWidgetItem;

    //customlist->adjustSize();
    //customlist->adjustSize();
    item->setSizeHint(QSize(60,60));

    addItem(item);
    setItemWidget(item,customwidget);
}

void CustomListWidget::AddListItem(QMainWindow *mainWindow,QStringList aIconPath,QStringList aNames)
{
	qDebug()<<"Inside GridView::CreateGridView()";
	qDebug()<<"aIconPath count = "<<aIconPath.count();
	qDebug()<<"names count = "<<aNames.count();

	int row;
	int col;
	//QWidget* widget = new QWidget();
	QGridLayout* GridLayout = new QGridLayout(this);
	if((6 == aIconPath.count()) || (5 == aIconPath.count()))
	{
		row = 3;
		col = 2;
	}
	else if((4 == aIconPath.count()) || (3 == aIconPath.count()))
	{
		row = col = 2;
	}
	else if((2 == aIconPath.count()) || (1 == aIconPath.count()))
	{
		row = 1;
		col = 2;
	}
	else if(0 == aIconPath.count())
	{
		row = col = 0;
	}

	int rowSpace = (ScreenSize::GetScreenRect().height() - (KHeadNameHeight+ToolbarIconHeight+2*KWidgetGapFactor))/row;
	int colSpace = (ScreenSize::GetScreenRect().width() - 2*KWidgetGapFactor )/col;

	int index = 0;
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
		{
		QPushButton *pushBtn = new QPushButton();
		qDebug()<<"name = "<<aNames[index];
		int gapFac = KHeadNameHeight+ToolbarIconHeight;
		pushBtn->setGeometry(i*colSpace,(j*rowSpace + KWidgetGapFactor),colSpace,rowSpace);
		pushBtn->setIconSize(QSize(colSpace-(3*KWidgetGapFactor),colSpace+(3*KWidgetGapFactor)));
		qDebug()<<"icon = "<<aIconPath[index];
		pushBtn->setIcon(QIcon(aIconPath[index]));
		qDebug()<<"After image display";
		GridLayout->addWidget(pushBtn,i,j);
		
		index++;
		if(index == aNames.count())
			{
			qDebug()<<"Count became same so exiting";
			break;
			}
		}
	if(index == aNames.count())
		{
		qDebug()<<"Count became same so exiting";
		break;
		}
	}
	//Its required please don't remove
	show();
}


QWidget* CustomListWidget::CreateListwidget(SmfContactList* friendsList)
	{
	qDebug()<<"Inside CustomListWidget::CreateListwidget() for friends";
	qDebug()<<"Friends count = "<<friendsList->count();
	
    QListWidget* customlist = new QListWidget();
    
    // Todo:- Display something when no friends are available
    foreach(SmfContact contact, *friendsList)
    {
    	QString name(contact.value("Name").value<QContactName>().firstName());
    	QString status(contact.value("Presence").value<QContactPresence>().customMessage());
    	if(!status.size())
    		status.append("Not available");
    	//QUrl url(contact.value("Avatar").value<QContactAvatar>().imageUrl());
    	QString url("C:\\data\\sample.bmp"); // ToDo:- should be the profile image path

        QWidget* widgetItem = new QWidget();
        QHBoxLayout* HMainlayout = new QHBoxLayout(widgetItem);
        QLabel *lab =new QLabel();
        lab->setPixmap(QPixmap(url));  
        lab->setFixedSize(60,60);
        HMainlayout->addWidget(lab);

        //To add First & second row horizontal layouts
        QVBoxLayout* VTextlayout = new QVBoxLayout;

        QHBoxLayout* HFirstrowLayout = new QHBoxLayout;
        
        //Add FirstLine label
        QLabel *lab1 = new QLabel(name);
        lab1->setStyleSheet("font: bold 16px;");
        HFirstrowLayout->addWidget(lab1);
        
        //Add Images to
        QLabel *Img1lab =new QLabel();
        Img1lab->setPixmap(QPixmap(url));
        Img1lab->setFixedSize(10,10);
        QLabel *Img2lab =new QLabel();
        Img2lab->setPixmap(QPixmap(url));
        Img2lab->setFixedSize(10,10);
        QLabel *Img3lab =new QLabel();
        Img3lab->setPixmap(QPixmap(url));
        Img3lab->setFixedSize(10,10);

        HFirstrowLayout->addWidget(Img1lab);
        HFirstrowLayout->addWidget(Img2lab);
        HFirstrowLayout->addWidget(Img3lab);

//        HFirstrowLayout->setAlignment(Img3lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img2lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img1lab,Qt::AlignRight);

        VTextlayout->addLayout(HFirstrowLayout);


         QHBoxLayout* HSecondrowLayout = new QHBoxLayout;
        QLabel *lab2 =new QLabel(status);
        lab2->setStyleSheet("font: 8px;");
        HSecondrowLayout->addWidget(lab2);

        QLabel *Timestamplabel =new QLabel("");
        Timestamplabel->setStyleSheet("font: 7px;");
        HSecondrowLayout->addWidget(Timestamplabel);

        HSecondrowLayout->setAlignment(Timestamplabel,Qt::AlignRight);

        VTextlayout->addLayout(HSecondrowLayout);


        HMainlayout->addLayout(VTextlayout);
        widgetItem->setLayout(HMainlayout);
        QListWidgetItem *item=new QListWidgetItem;

        //customlist->adjustSize();
        //customlist->adjustSize();
        item->setSizeHint(QSize(60,60));

        customlist->addItem(item);
        customlist->setItemWidget(item,widgetItem);
    }
    customlist->setStyleSheet("selection-color: yellow;"
                          "selection-background-color: grey;");
    return customlist;
	}


QWidget* CustomListWidget::CreateListwidget(SmfPostList *postsList)
	{
	qDebug()<<"Inside CustomListWidget::CreateListwidget() for posts";
	qDebug()<<"Posts count = "<<postsList->count();
	
    QListWidget* customlist = new QListWidget();
    
    // Todo:- Display something when no posts are available
    foreach(SmfPost post, *postsList)
    {
    	QString text(post.description());
    	QString ownerName(post.owner().value("Name").value<QContactName>().firstName());

    	//QUrl url(contact.value("Avatar").value<QContactAvatar>().imageUrl());
    	QString url("C:\\data\\sample.bmp"); // ToDo:- should be the profile image path

        QWidget* widgetItem = new QWidget();
        QHBoxLayout* HMainlayout = new QHBoxLayout(widgetItem);
        QLabel *lab =new QLabel();
        lab->setPixmap(QPixmap(url));  
        lab->setFixedSize(60,60);
        HMainlayout->addWidget(lab);

        //To add First & second row horizontal layouts
        QVBoxLayout* VTextlayout = new QVBoxLayout;

        QHBoxLayout* HFirstrowLayout = new QHBoxLayout;
        
        //Add FirstLine label
        QLabel *lab1 = new QLabel(ownerName);
        lab1->setStyleSheet("font: bold 16px;");
        HFirstrowLayout->addWidget(lab1);
        
        //Add Images to
        QLabel *Img1lab =new QLabel();
        Img1lab->setPixmap(QPixmap(url));
        Img1lab->setFixedSize(10,10);
        QLabel *Img2lab =new QLabel();
        Img2lab->setPixmap(QPixmap(url));
        Img2lab->setFixedSize(10,10);
        QLabel *Img3lab =new QLabel();
        Img3lab->setPixmap(QPixmap(url));
        Img3lab->setFixedSize(10,10);

        HFirstrowLayout->addWidget(Img1lab);
        HFirstrowLayout->addWidget(Img2lab);
        HFirstrowLayout->addWidget(Img3lab);

//        HFirstrowLayout->setAlignment(Img3lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img2lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img1lab,Qt::AlignRight);

        VTextlayout->addLayout(HFirstrowLayout);


         QHBoxLayout* HSecondrowLayout = new QHBoxLayout;
        QLabel *lab2 =new QLabel(text);
        lab2->setStyleSheet("font: 8px;");
        HSecondrowLayout->addWidget(lab2);

        QLabel *Timestamplabel =new QLabel("");
        Timestamplabel->setStyleSheet("font: 7px;");
        HSecondrowLayout->addWidget(Timestamplabel);

        HSecondrowLayout->setAlignment(Timestamplabel,Qt::AlignRight);

        VTextlayout->addLayout(HSecondrowLayout);


        HMainlayout->addLayout(VTextlayout);
        widgetItem->setLayout(HMainlayout);
        QListWidgetItem *item=new QListWidgetItem;

        //customlist->adjustSize();
        //customlist->adjustSize();
        item->setSizeHint(QSize(60,60));

        customlist->addItem(item);
        customlist->setItemWidget(item,widgetItem);
    }
    customlist->setStyleSheet("selection-color: yellow;"
                          "selection-background-color: grey;");
    return customlist;
	
	}
QWidget* CreateListwidget(SmfActivityEntryList *activityList)
			{
	
			}

/*QWidget* CustomListWidget::CreateListwidget(QString aIconPath,QString aFirstLine,QString aSecondLine)
{
	qDebug()<<"Inside CustomListWidget::CreateListwidget()";
	qDebug()<<"Icon path = "<<aIconPath;
	qDebug()<<"firstline = "<<aFirstLine;
	qDebug()<<"second line = "<<aSecondLine;
    QListWidget* customlist = new QListWidget();
    //for(int i =0;i<2;i++)
    //{
        QWidget* widgetItem = new QWidget();
        QHBoxLayout* HMainlayout = new QHBoxLayout(widgetItem);
        QLabel *lab =new QLabel();
        lab->setPixmap(QPixmap(aIconPath));
        lab->setFixedSize(60,60);
        HMainlayout->addWidget(lab);

        //To add First & second row horizontal layouts
        QVBoxLayout* VTextlayout = new QVBoxLayout;

        QHBoxLayout* HFirstrowLayout = new QHBoxLayout;
        //Add FirstLine label
        QLabel *lab1 =new QLabel(aFirstLine);
        lab1->setStyleSheet("font: bold 16px;");
        HFirstrowLayout->addWidget(lab1);
        //Add Images to
        QLabel *Img1lab =new QLabel();
        Img1lab->setPixmap(QPixmap(aIconPath));
        Img1lab->setFixedSize(10,10);
        QLabel *Img2lab =new QLabel();
        Img2lab->setPixmap(QPixmap(aIconPath));
        Img2lab->setFixedSize(10,10);
        QLabel *Img3lab =new QLabel();
        Img3lab->setPixmap(QPixmap(aIconPath));
        Img3lab->setFixedSize(10,10);

        HFirstrowLayout->addWidget(Img1lab);
        HFirstrowLayout->addWidget(Img2lab);
        HFirstrowLayout->addWidget(Img3lab);

//        HFirstrowLayout->setAlignment(Img3lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img2lab,Qt::AlignRight);
//        HFirstrowLayout->setAlignment(Img1lab,Qt::AlignRight);

        VTextlayout->addLayout(HFirstrowLayout);


         QHBoxLayout* HSecondrowLayout = new QHBoxLayout;
        QLabel *lab2 =new QLabel(aSecondLine);
        lab2->setStyleSheet("font: 8px;");
        HSecondrowLayout->addWidget(lab2);

        QLabel *Timestamplabel =new QLabel("00:00 JUN 29");
        Timestamplabel->setStyleSheet("font: 7px;");
        HSecondrowLayout->addWidget(Timestamplabel);

        HSecondrowLayout->setAlignment(Timestamplabel,Qt::AlignRight);

        VTextlayout->addLayout(HSecondrowLayout);


        HMainlayout->addLayout(VTextlayout);
        widgetItem->setLayout(HMainlayout);
        QListWidgetItem *item=new QListWidgetItem;

        //customlist->adjustSize();
        //customlist->adjustSize();
        item->setSizeHint(QSize(60,60));

        customlist->addItem(item);
        customlist->setItemWidget(item,widgetItem);
    //}
customlist->setStyleSheet("selection-color: yellow;"
                          "selection-background-color: grey;");
    return customlist;
}*/
