#include "testscreen.h"
#include "smfclient.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

TestScreen::TestScreen(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(friendsButtonClicked()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(postsButtonClicked()));
	writeLog("Start");
	SmfClient client;
	
	//Getting list of provider who implement this interface
//	QString name("org.symbian.smf.client.contact.fetcher");
	//TODO:- PM should use commented interface name instead
//	QString name("org.symbian.smf.client.contact.posts");
	QString name("posts");
	writeLog("Before client.GetServices");
	providerList= client.GetServices(name);
	//add provider names to the combobox
	for(int i=0; i< providerList->count();i++)
		{
		SmfProvider provider = providerList->at(i);
		ui.comboBox->addItem(provider.serviceName());
		}

	//logging for debugging purpose
	writeLog("GetServices count=");
	QString c = QString::number(providerList->count());
	writeLog(c);
	SmfProvider smfP(providerList->at(0));
	
	writeLog("0th SmfProvider=");
	writeLog(smfP.description());
		writeLog(smfP.serviceUrl().toString());
		writeLog(smfP.applicationUrl().toString());
		m_postProvider = new SmfPostProvider(&smfP);
	//connect to appropriate slot
	connect(m_postProvider,
			SIGNAL(postsAvailable(SmfPostList*, SmfError, SmfResultPage)),
			this,
			SLOT(showPosts(SmfPostList* , SmfError , SmfResultPage )));
	
	writeLog("Before m_postProvider->posts=");
	//request for self posts
	m_postProvider->posts();
}
void TestScreen::friendsButtonClicked()
	{
	ui.listWidget->clear();
//	pixmap.load("c:\\data\\smfpix.bmp");
//    splash.setPixmap(pixmap);
//    splash.show();
	//get the current index of combobox to get the SmfProvider
	int providerIndex = ui.comboBox->currentIndex();
	//now create contact fetcher with 0th
	SmfProvider smfP(providerList->at(providerIndex));
	
	writeLog("0th SmfProvider=");
	writeLog(smfP.description());
		writeLog(smfP.serviceUrl().toString());
		writeLog(smfP.applicationUrl().toString());
		
	m_contactFetcher = new SmfContactFetcher(&smfP);
	//connect to appropriate slot
	connect(m_contactFetcher,SIGNAL(friendsListAvailable(SmfContactList*, SmfError , SmfResultPage)),
			this,SLOT(showFriends(SmfContactList*, SmfError , SmfResultPage)));
	/*connect(m_contactFetcher,SIGNAL(groupListAvailable(SmfGroupList*, SmfError , SmfResultPage)),
				this,SLOT(showFriends(SmfGroupList*, SmfError , SmfResultPage)));*/
	
	writeLog("Before m_contactFetcher->friends=");
	//request for friends, excluding paging info
	m_contactFetcher->friends();
	//m_contactFetcher->groups();
	}
void TestScreen::postsButtonClicked()
	{
	//get the current index of combobox to get the SmfProvider
	int providerIndex = ui.comboBox->currentIndex();
	//now create contact fetcher with 0th
	SmfProvider smfP(providerList->at(providerIndex));
	
	writeLog("0th SmfProvider=");
	writeLog(smfP.description());
		writeLog(smfP.serviceUrl().toString());
		writeLog(smfP.applicationUrl().toString());
		
	m_postProvider = new SmfPostProvider(&smfP);
//	//connect to appropriate slot
	connect(m_postProvider,
			SIGNAL(postsAvailable(SmfPostList*, SmfError, SmfResultPage)),
			this,
			SLOT(showPosts(SmfPostList*, SmfError, SmfResultPage)));
	
	writeLog("Before m_contactFetcher->posts=");
	//request for self posts
	m_postProvider->posts();
	}
void TestScreen::writeLog(QString log) const
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
void TestScreen::showGroups(SmfGroupList* grps, SmfError err, SmfResultPage)
	{
	writeLog("TestScreen::showFriends count=");
	writeLog(QString::number(grps->count()));
	writeLog("Error=");
	writeLog(QString::number(err));
	//display friends
	QMessageBox::information(this,QString("Name"),grps->at(0).name(),QMessageBox::Ok);
	QMessageBox::information(this,QString("Name"),grps->at(1).name(),QMessageBox::Ok);
//	SmfContact frnd1 = frnds->at(0);
//	SmfContact frnd2 = frnds->at(1);
//	QString name = frnd1.value("Name").toString();
//	QMessageBox::information(this,QString("Name"),name,QMessageBox::Ok);
//	QString name2 = frnd2.value("Name").toString();
//	QMessageBox::information(this,QString("Name"),name2,QMessageBox::Ok);
//	foreach(SmfContact frnd, *frnds)
//			{
//		//lets display only street
////		QContactNickname name = frnd.value("Nickname").value<QContactNickname>();
////		QString nick = name.value("Nickname");
//		QString name = frnd.value("Name").toString();
//		QMessageBox::information(this,QString("Name"),name,QMessageBox::Ok);
//			}
	}
void TestScreen::showFriends(SmfContactList* frnds, SmfError err, SmfResultPage)
	{
	splash.finish(this);
	writeLog("TestScreen::showFriends count=");
	writeLog(QString::number(frnds->count()));
	writeLog("Error=");
	writeLog(QString::number(err));
	//display friends
	
	foreach(SmfContact frnd, *frnds)
			{
		//lets display only street
		QVariant nameVar = frnd.value("Name");
		QContactName name = nameVar.value<QContactName>();
//		QString nick = name.value("Nickname");
		QString fname;
		QString lname;
		fname = name.firstName();
		lname = name.lastName();
		
		ui.listWidget->addItem(fname);
//		QMessageBox::information(this,QString("First Name"),fname,QMessageBox::Ok);
//		QMessageBox::information(this,QString("Last Name"),lname,QMessageBox::Ok);
			}
	ui.listWidget->show();
	}
void TestScreen::showPosts(SmfPostList* postlist, SmfError error, SmfResultPage resultPage)
	{
	Q_UNUSED(resultPage)
	writeLog("TestScreen::showPosts");
	ui.listWidget->clear();
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
TestScreen::~TestScreen()
{

}
