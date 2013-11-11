/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0" 
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Contributors:
 * Manasij Roy, Nalina Hariharan
 */



#include "displaywidget.h"
#include <QApplication>
#include <QDesktopWidget>

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent)
{
//	ui.setupUi(this);
//	ui.verticalLayout->setGeometry(QApplication::desktop()->availableGeometry());
//	//Add item as and when they are implemented
//	ui.comboBox_intf->addItem("Contact Fetcher");
//	ui.comboBox_intf->addItem("Post Provider");
////	connect(ui.comboBox_intf,
////			SIGNAL(currentIndexChanged(int)),
////			this,
////			SLOT(interfaceSelected(int)));
//	connect(ui.pushButton_intf,SIGNAL(clicked()),this,SLOT(interfaceSelected()));
//	connect(ui.pushButton_SP,SIGNAL(clicked()),this,SLOT(serviceProviderSelected()));
//	connect(ui.pushButton_service,SIGNAL(clicked()),this,SLOT(serviceSelected()));
//	writeLog("Start");
	SmfClient client;
	QString intfName("org.symbian.smf.client.contact.fetcher\0.2");
	providerList= client.GetServices(intfName);
	writeLog("GetServices count=");
	QString c = QString::number(providerList->count());
	writeLog(c);	
	SmfProvider smfP(providerList->at(0));
	m_contactFetcher = new SmfContactFetcher(&smfP);
				//connect to appropriate slot
	connect(m_contactFetcher,
			SIGNAL(friendsListAvailable(SmfContactList*, SmfError , SmfResultPage)),
			this,
			SLOT(showFriends(SmfContactList*, SmfError , SmfResultPage)));
	
	writeLog("Before m_contactFetcher->friends=");
	//request for friends, excluding paging info
	m_contactFetcher->friends();
}
void DisplayWidget::interfaceSelected()
	{
	SmfClient client;
	QString intfName;
	switch(ui.comboBox_intf->currentIndex())
		{
		case 0:
			intfName = "org.symbian.smf.client.contact.fetcher";
			break;
		case 1:
			intfName = "org.symbian.smf.client.contact.posts";
			break;
		default:
			//should not reach here!!!!
			break;
		}
	writeLog("Before client.GetServices");
	providerList= client.GetServices(intfName);
	ui.comboBox__SP->clear();
	//add provider names to the combobox
	for(int i=0; i< providerList->count();i++)
		{
		SmfProvider provider = providerList->at(i);
		ui.comboBox__SP->addItem(provider.serviceName());
		}

	//logging for debugging purpose
	writeLog("GetServices count=");
	QString c = QString::number(providerList->count());
	writeLog(c);
	//serviceProviderSelected
//	connect(ui.comboBox__SP,
//			SIGNAL(currentIndexChanged(int)),
//			this,
//			SLOT(serviceProviderSelected(int)));
	}
void DisplayWidget::serviceProviderSelected()
	{
	
	switch(ui.comboBox_intf->currentIndex())
		{
		case 0:
			ui.comboBox_service->addItem("Get Friend List");
			//ui.comboBox_3->addItem("Get Group List");
			break;
		case 1:
			ui.comboBox_service->addItem("Get Own Posts");
			//ui.comboBox_3->addItem("Get Friend's Posts");
			break;
		}
//	connect(ui.comboBox_service,
//			SIGNAL(currentIndexChanged(int)),
//			this,
//			SLOT(serviceSelected(int)));
	}
void DisplayWidget::serviceSelected()
	{
	SmfProvider smfP(providerList->at(ui.comboBox_service->currentIndex()));
	
	writeLog("Selected SmfProvider=");
	writeLog(smfP.description());
	writeLog(smfP.serviceUrl().toString());
	writeLog(smfP.applicationUrl().toString());
	switch(ui.comboBox_intf->currentIndex())
		{
		case 1:
			m_postProvider = new SmfPostProvider(&smfP);
			//TODO:- it should be nested switch case as there are multiple APIs under one interface
			//connect to appropriate slot
			connect(m_postProvider,
					SIGNAL(postsAvailable(SmfPostList*, SmfError, SmfResultPage)),
					this,
					SLOT(showPosts(SmfPostList* , SmfError , SmfResultPage )));
			
			writeLog("Before m_postProvider->posts=");
			//request for self posts
			m_postProvider->posts();
			break;
		case 0:
			m_contactFetcher = new SmfContactFetcher(&smfP);
			//connect to appropriate slot
			connect(m_contactFetcher,
					SIGNAL(friendsListAvailable(SmfContactList*, SmfError , SmfResultPage)),
					this,
					SLOT(showFriends(SmfContactList*, SmfError , SmfResultPage)));
			
			writeLog("Before m_contactFetcher->friends=");
			//request for friends, excluding paging info
			m_contactFetcher->friends();
			break;
		}
	}
void DisplayWidget::showPosts(SmfPostList* postlist, SmfError error, SmfResultPage resultPage)
	{
	Q_UNUSED(resultPage)
	writeLog("TestScreen::showPosts");
	ui.listWidget->clear();
	ui.listWidget->setVerticalScrollBar(ui.verticalScrollBar_list);
	writeLog("TestScreen::showPosts count=");
	writeLog(QString::number(postlist->count()));
	writeLog("Error=");
	writeLog(QString::number(error));
	if(error)
		{
		QString smferrString("Smf Error code=");
		smferrString += QString::number(error);
		QMessageBox::information(this,"Error",smferrString,QMessageBox::Ok);
		}
	//display post description
	
	foreach(SmfPost post, *postlist)
			{ 
			QString desc = post.description();
			ui.listWidget->addItem(desc);
			}
	ui.listWidget->show();
	}
void DisplayWidget::showFriends(SmfContactList* frnds, SmfError err, SmfResultPage)
	{
	splash.finish(this);
	writeLog("TestScreen::showFriends count=");
	writeLog(QString::number(frnds->count()));
	writeLog("Error=");
	writeLog(QString::number(err));
	//display friends
	
	foreach(SmfContact frnd, *frnds)
			{
		//lets display only name
		QVariant nameVar = frnd.value("Name");
		QContactName name = nameVar.value<QContactName>();
		QString fname;
		QString lname;
		fname = name.firstName();
		lname = name.lastName();
		//qDebug()<<fname;
		//qDebug()<<lname;
		//ui.listWidget->addItem(fname);
			}
	//ui.listWidget->show();
	}
void DisplayWidget::writeLog(QString log) const
	{
#ifdef WRITE_LOG 
	QFile file("c:\\data\\SmfClientLogs.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
	         ;
    QTextStream out(&file);
    out << log << "\n";
    file.close();
#endif
	}
DisplayWidget::~DisplayWidget()
{

}
