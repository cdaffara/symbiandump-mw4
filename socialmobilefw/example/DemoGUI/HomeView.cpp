#include "HomeView.h"
#include "ScreenSize.h"
#include "qpushbutton.h"
#include "customListwidget.h"
#include "GridView.h"
#include <qtablewidget.h>

#include <QMapIterator>
#include <smfclient.h>
#include <smfprovider.h>
#include <QMessageBox>
#include <QDebug>
#include "ImageDownload.h"

static int tab = 0;

HomeView::HomeView(QString aHeadName):
        iHeadName(aHeadName)
	{
	//m_providerList = NULL;
	m_mainWindow = NULL;
	TabWidget = NULL;
	iFrndsListWidget = NULL;
	iPostsListWidget = NULL;
	iActivityListWidget = NULL;
	AlbumView = NULL;
	
	m_contactFetcher = NULL;
	m_postProvider = NULL;
	m_gallery = NULL;
	m_activityFetcher = NULL;
	m_friendsList = NULL;
	m_postsList = NULL;
	m_albumsList = NULL;
	m_activitiesList = NULL;
	m_picList = NULL;
	}

HomeView::~HomeView()
	{
	if(TabWidget)
		{
		delete TabWidget;
		TabWidget = NULL;
		}
	if(iFrndsListWidget)
		{
		delete iFrndsListWidget;
		iFrndsListWidget = NULL;
		}
	if(iPostsListWidget)
		{
		delete iPostsListWidget;
		iPostsListWidget = NULL;
		}
	if(iActivityListWidget)
		{
		delete iActivityListWidget;
		iActivityListWidget = NULL;
		}
	if(AlbumView)
		{
		delete AlbumView;
		AlbumView = NULL;
		}
	
	if(m_contactFetcher)
		{
		delete m_contactFetcher;
		m_contactFetcher =NULL;
		}
	if(m_postProvider)
		{
		delete m_postProvider;
		m_postProvider =NULL;
		}
	if(m_gallery)
		{
		delete m_gallery;
		m_gallery =NULL;
		}
	if(m_activityFetcher)
		{
		delete m_activityFetcher;
		m_activityFetcher =NULL;
		}
	if(m_friendsList)
		{
		delete m_friendsList;
		m_friendsList =NULL;
		}
	if(m_postsList)
		{
		delete m_postsList;
		m_postsList =NULL;
		}
	if(m_albumsList)
		{
		delete m_albumsList;
		m_albumsList =NULL;
		}
	if(m_activitiesList)
		{
		delete m_activitiesList;
		m_activitiesList =NULL;
		}
	if(m_picList)
		{
		delete m_picList;
		m_picList =NULL;
		}
	}


void HomeView::SetupUI(QMainWindow *Mainwindow)
	{
	qDebug()<<"Inside HomeView::SetupUI()";
	
	m_mainWindow = Mainwindow;
	
    QPushButton* HomeButton = new QPushButton(m_mainWindow);
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
    qDebug()<<"Home button created and setup";
    
    TabWidget = new QTabWidget(m_mainWindow);
    connect(TabWidget,SIGNAL(currentChanged(int)),this,SLOT(Navigated2OtherTab(int)));
    qDebug()<<"Tab widget created";
    
    iFrndsListWidget = new CustomListWidget();
    TabWidget->addTab(iFrndsListWidget,"Friends");
    qDebug()<<"Friends Tab created";
    
    iPostsListWidget = new CustomListWidget();
    TabWidget->addTab(iPostsListWidget,"Posts");
    qDebug()<<"Posts Tab created";

    //AlbumView = new GridView();
    /*QWidget *widget = new QWidget();
    TabWidget->addTab(widget,"Album");*/
    iAlbumWidget = new CustomListWidget();
    TabWidget->addTab(iAlbumWidget,"Album");
    
    qDebug()<<"Albums Tab created";
    
/*
    QStringList Iconpathlist;
    Iconpathlist.append("C:\\data\\AlbumDefault.JPG");
    Iconpathlist.append("C:\\data\\AlbumDefault.JPG");
    Iconpathlist.append("C:\\data\\AlbumDefault.JPG");
    Iconpathlist.append("C:\\data\\AlbumDefault.JPG");

    
    TabWidget->addTab(AlbumView->CreateGridView(Iconpathlist),"Album");
    qDebug()<<"Albums Tab created";
*/

    iActivityListWidget = new CustomListWidget();
    TabWidget->addTab(iActivityListWidget,"Activity");
    qDebug()<<"Activity Tab created";

    TabWidget->setGeometry(0,KHeadNameHeight + KWidgetGapFactor,ScreenSize::GetScreenRect().width(),ScreenSize::GetScreenRect().height() - 100);

    //Drawing Tool bar
    ToolBarwidget = new ToolBar;
    QStringList actionList;
    actionList.append("Add Services");
    //actionList.append("Open");
    //actionList.append("Reply");
    ToolBarwidget->GetToolBar(m_mainWindow,actionList);
    qDebug()<<"Tool bar created";
    }


void HomeView::getFriends()
	{
	qDebug()<<"Inside HomeView::getFriends()";
	
	// Get the list of providers
	SmfClient client;
	QString intfName("org.symbian.smf.plugin.contact.fetcher");
	
	SmfProviderList *providerList = client.GetServices(intfName);
	qDebug()<<"client.GetServices returned a list with count = "<<providerList->count();
	
	// flag to check if required plugin is there
	bool pluginFound = false;
	foreach(SmfProvider provider, *providerList)
		{
		if("Facebook" == provider.serviceName())
			{
			qDebug()<<"Plugin for Facebook found";
			pluginFound = true;
			m_contactFetcher = new SmfContactFetcher(&provider);
			bool ret = connect(m_contactFetcher, SIGNAL(friendsListAvailable(SmfContactList*, SmfError , SmfResultPage)),
					this, SLOT(friendsAvailable(SmfContactList*, SmfError , SmfResultPage)));
			
			qDebug()<<"Connected ?"<<ret;
			m_contactFetcher->friends(1,5);
			}
		}
	
	if(!pluginFound)
		{
		qDebug()<<"Plugin for Facebook not found!!!";
		QString smferrString("No Facebook plugin found!!!");
		QMessageBox::information(m_mainWindow,"Error",smferrString,QMessageBox::Ok);
		}
	}


void HomeView::friendsAvailable(SmfContactList* friendsList, SmfError error, SmfResultPage resultPage)
	{
	qDebug()<<"Inside HomeView::friendsAvailable()";
	
	m_friendsList = friendsList;
	if(error)
		{
		SmfClient client;
		QString errStr = client.errorString(error);
		qDebug()<<"Error found, code = "<<error;
		qDebug()<<"Error string is = "<<errStr;
		
		QMessageBox::information(m_mainWindow,"Error",errStr,QMessageBox::Ok);
		return;
		}
	
	//display friends description
	int count = 0;
	qDebug()<<"Number of friends retrieved = "<<friendsList->count();
	if(friendsList->count() == 0)
		{
		QString smferrString("No Friends");
		QMessageBox::information(m_mainWindow,"No Friends",smferrString,QMessageBox::Ok);
		return;
		}
		
	QMap<QString, QUrl> urlMap;
	foreach(SmfContact contact, *friendsList)
		{
		QString name(contact.value("Name").value<QContactName>().firstName());
		QString status(contact.value("Presence").value<QContactPresence>().customMessage());
		if(!status.size())
			status.append("Not available");
		QUrl url(contact.value("Avatar").value<QContactAvatar>().imageUrl());

		qDebug()<<"Friends name = "<<contact.value("Name").value<QContactName>().firstName();
		qDebug()<<"Friends status msg desc = "<<contact.value("Presence").value<QContactPresence>().customMessage();
		qDebug()<<"Friends profile image URL = "<<contact.value("Avatar").value<QContactAvatar>().imageUrl();
		urlMap.insert(name, url);
		}

	// Download Images
	downloadImages(SmfFriendsFetch, urlMap);
		
	}

void HomeView::populateFriendsWidget()
	{
	foreach(SmfContact contact, *m_friendsList)
		{
		QString name(contact.value("Name").value<QContactName>().firstName());
		QString status(contact.value("Presence").value<QContactPresence>().customMessage());
		if(!status.size())
			status.append("Not available");
		QString url("C:\\data\\");
		url.append(name);
		url.append(".jpg");
		
		// Add this contact to the list widget
		iFrndsListWidget->AddListItem(url,name,status);
		}
	}

void HomeView::downloadImages(const SmfItemIdentifier &identifier, const QMap<QString, QUrl> urlMap)
	{
	qDebug()<<"Inside HomeView::downloadImages()";
	QMapIterator<QString, QUrl> iter(urlMap);
	
	while(iter.hasNext())
		{
		iter.next();
		m_downloader.downloadImage(this, iter.key(), iter.value(), identifier);
		downloading = true;
		}
	}

void HomeView::Navigated2OtherTab(int tabIndex)
	{
	qDebug()<<"Inside HomeView::Navigated2OtherTab() for tab index = "<<tabIndex;
	
	// Display friends
    if(tabIndex == 0)
    	{
		qDebug()<<"Downloading images?? "<<downloading;
        if(!iFrndsListWidget->count())
        	{
			if(!downloading)	
				{
				getFriends();
				
				/*if (10 == tab)
					{
					ToolBarwidget->newAction1->setVisible(false);
					ToolBarwidget->newAction2->setVisible(false);
					}*/
				}
        	}
    	}
    
    // Display albums
    else if(tabIndex == 1)
    	{
		if(!iPostsListWidget->count())
			{
			if(!downloading)
				{
				getPosts();
				/*ToolBarwidget->menu->addAction(ToolBarwidget->newAction1);
				ToolBarwidget->menu->addAction(ToolBarwidget->newAction2);
				tab = 10;
				qDebug()<<"Action is visible ?"<<(ToolBarwidget->newAction1->isVisible());
				if (10 == tab || !(ToolBarwidget->newAction1->isVisible()))
					{
					ToolBarwidget->newAction1->setVisible(true);
					ToolBarwidget->newAction2->setVisible(true);
					}*/
				}
			}
    	}
    
    // Display albums
    else if(tabIndex == 2)
    	{
		if(!m_picList)
			{
			if(!downloading)	
				{
				getPhotos();
				/*if (ToolBarwidget->newAction1->isVisible())
					{
					ToolBarwidget->newAction1->setVisible(false);
					ToolBarwidget->newAction2->setVisible(false);
					}*/
				}
			}
			
    	}
    	
    // Display activities
    else if(tabIndex == 3)
    	{
		if(!iActivityListWidget->count())	
			{
			getActivities();
			/*if (ToolBarwidget->newAction1->isVisible())
				{
				ToolBarwidget->newAction1->setVisible(false);
				ToolBarwidget->newAction2->setVisible(false);
				}*/
			}
    	}
	}

void HomeView::getPosts()
	{
	qDebug()<<"Inside HomeView::getPosts()";
		
	SmfClient client;
	QString intfName("org.symbian.smf.client.contact.posts");
	SmfProviderList *providerList = client.GetServices(intfName);
	bool pluginFound = false;
	qDebug()<<"client.GetServices returned a list with count = "<<providerList->count();
	foreach(SmfProvider provider, *providerList)
		{
		if("Facebook" == provider.serviceName())
			{
			qDebug()<<"Plugin for facebook found";
			pluginFound = true;
			m_postProvider = new SmfPostProvider(&provider);
			bool ret = connect(m_postProvider, SIGNAL(postsAvailable(SmfPostList*, SmfError , SmfResultPage)),
					this, SLOT(postsAvailable(SmfPostList*, SmfError , SmfResultPage)));
			
			qDebug()<<"Connected ?"<<ret;
			m_postProvider->posts();
			}
		}

	if(!pluginFound)
		{
		qDebug()<<"Plugin for facebook not found!!!";
		QString smferrString("No Facebook plugin found!!!");
		QMessageBox::information(m_mainWindow,"Error",smferrString,QMessageBox::Ok);
		}
	}

void HomeView::postsAvailable(SmfPostList* postsList, SmfError error, SmfResultPage page)
	{
	Q_UNUSED(page);
			
	qDebug()<<"Inside HomeView::postsAvailable()";
	
	m_postsList = postsList;
	if(error)
		{
		SmfClient client;
		QString errStr = client.errorString(error);
		qDebug()<<"Error found, code = "<<error;
		qDebug()<<"Error string is = "<<errStr;
		
		QMessageBox::information(m_mainWindow,"Error",errStr,QMessageBox::Ok);
		return;
		}
	
	//display posts details
	int count = 0;
	qDebug()<<"Number of posts retrieved = "<<postsList->count();
	if(postsList->count() == 0)
		{
		QString smferrString("No Posts");
		QMessageBox::information(m_mainWindow,"No Posts",smferrString,QMessageBox::Ok);
		return;
		}
	QMap<QString, QUrl> urlMap;
	foreach(SmfPost post, *postsList)
		{
		QString name(post.owner().value("Name").value<QContactName>().firstName());
		QUrl url(post.owner().value("Avatar").value<QContactAvatar>().imageUrl());

		qDebug()<<"Post text = "<<post.description();
		qDebug()<<"owner = "<<name;
		qDebug()<<"owner's profile image url = "<<url.toString();
		
		urlMap.insert(name, url);
		}
	
	// Download Images
	downloadImages(SmfPostsFetch, urlMap);
	}


void HomeView::populatePostsWidget()
	{
	// Add individual items to the widget
	foreach(SmfPost post, *m_postsList)
		{
		QString name(post.owner().value("Name").value<QContactName>().firstName());
		QString text(post.description());

		QString url("C:\\data\\");
		url.append(name);
		url.append(".jpg");
		
		// Add this contact to the list widget
		iPostsListWidget->AddListItem(url,name,text);
		}
	}

void HomeView::getAlbums()
	{
	qDebug()<<"Inside HomeView::getAlbums()";
		
	SmfClient client;
	QString intfName("org.symbian.smf.client.gallery");
	SmfProviderList *providerList = client.GetServices(intfName);
	bool pluginFound = false;
	qDebug()<<"client.GetServices returned a list with count = "<<providerList->count();
	foreach(SmfProvider provider, *providerList)
		{
		if("Flickr" == provider.serviceName())
			{
			qDebug()<<"Plugin for flickr found";
			pluginFound = true;
			m_gallery = new SmfGallery(&provider);
			bool ret = connect(m_gallery, SIGNAL(albumsAvailable(SmfPictureAlbumList*, SmfError , SmfResultPage)),
					this, SLOT(albumsAvailable(SmfPictureAlbumList*, SmfError , SmfResultPage)));
			
			qDebug()<<"Connected ?"<<ret;
			QStringList names;
			
			SmfContact user; // current user
			
			m_gallery->albums(names, &user);
			}
		}
	if(!pluginFound)
		{
		qDebug()<<"Plugin for flickr not found!!!";
		QString smferrString("No flickr plugin found!!!");
		QMessageBox::information(m_mainWindow,"Error",smferrString,QMessageBox::Ok);
		}
	}

void HomeView::albumsAvailable(SmfPictureAlbumList* albums, SmfError error, SmfResultPage resultPage)
	{
	Q_UNUSED(resultPage);
			
	qDebug()<<"Inside HomeView::albumsAvailable()";
	
	m_albumsList = albums;
	if(error)
		{
		SmfClient client;
		QString errStr = client.errorString(error);
		qDebug()<<"Error found, code = "<<error;
		qDebug()<<"Error string is = "<<errStr;
		
		QMessageBox::information(m_mainWindow,"Error",errStr,QMessageBox::Ok);
		return;
		}
	
	//display album details
	int count = 0;
	qDebug()<<"Number of albums retrieved = "<<albums->count();
	if(albums->count() == 0)
		{
		QString smferrString("No Albums");
		QMessageBox::information(m_mainWindow,"No Albums",smferrString,QMessageBox::Ok);
		return;
		}
	
	QStringList albumPicList;
	QStringList albumNameList;
	foreach(SmfPictureAlbum album, *m_albumsList)
		{
		
		qDebug()<<"Album name = "<<album.title();
		qDebug()<<"Album description = "<<album.description();
		qDebug()<<"Album pictureCount = "<<album.pictureCount();
		qDebug()<<"Album id = "<<album.id();
		albumNameList.append(album.title());
		albumPicList.append("C:\\data\\sample.bmp");
		}
	
	// Create the grip view
	AlbumView->CreateGridView(albumPicList, albumNameList);
	}


void HomeView::getActivities()
	{
	qDebug()<<"Inside HomeView::getActivities()";
	
	// Get the list of providers
	SmfClient client;
	QString intfName("org.symbian.smf.client.activity.fetcher");
	
	SmfProviderList *providerList = client.GetServices(intfName);
	qDebug()<<"client.GetServices returned a list with count = "<<providerList->count();
	
	// flag to check if required plugin is there
	bool pluginFound = false;
	int index = 0;
	foreach(SmfProvider provider, *providerList)
		{
		if("Facebook" == provider.serviceName())
			{
			qDebug()<<"Plugin for Facebook found";
			pluginFound = true;
			m_activityFetcher = new SmfActivityFetcher(&provider);
			bool ret = connect(m_activityFetcher, SIGNAL(resultsAvailable(SmfActivityEntryList*, SmfError , SmfResultPage)),
					this, SLOT(activitiesAvailable(SmfActivityEntryList*, SmfError , SmfResultPage)));
			
			qDebug()<<"Connected ?"<<ret;
			m_activityFetcher->selfActivities();
			}
		index++;
		}
	
	if(!pluginFound)
		{
		qDebug()<<"Plugin for Facebook not found!!!";
		QString smferrString("No Facebook plugin found!!!");
		QMessageBox::information(m_mainWindow,"Error",smferrString,QMessageBox::Ok);
		}
	}


void HomeView::activitiesAvailable(SmfActivityEntryList* activitiesList, SmfError error, SmfResultPage resultPage)
	{
	qDebug()<<"Inside HomeView::activitiesAvailable()";
	
	m_activitiesList = activitiesList;
	qDebug()<<"Number of activities retrieved = "<<activitiesList->count();
	if(error)
		{
		SmfClient client;
		QString errStr = client.errorString(error);
		qDebug()<<"Error found, code = "<<error;
		qDebug()<<"Error string is = "<<errStr;
		
		QMessageBox::information(m_mainWindow,"Error",errStr,QMessageBox::Ok);
		return;
		}
	
	//display activity description
	int count = 0;
	qDebug()<<"Number of activities retrieved = "<<activitiesList->count();
	
	if(activitiesList->count() == 0)
		{
		QString smferrString("No Recent Activities");
		QMessageBox::information(m_mainWindow,"Error",smferrString,QMessageBox::Ok);
		return;
		}
	
	foreach(SmfActivityEntry activity, *activitiesList)
		{
		qDebug()<<"Activity author name = "<<activity.author().value("Name").value<QContactName>().firstName();
		qDebug()<<"Activity title = "<<activity.title().title();
		qDebug()<<"Activity details = "<<activity.details().description();
		}

	populateActivitiesWidget();
		
	}

void HomeView::populateActivitiesWidget()
	{
	qDebug()<<"Inside HomeView::populateActivityWidget()";
	QString defImagePath("C:\\data\\genericfriendicon.svg");
	foreach(SmfActivityEntry activity, *m_activitiesList)
		{
		QString name(activity.author().value("Name").value<QContactName>().firstName());
		QString title(activity.title().title());

		
		// Add this contact to the list widget
		iActivityListWidget->AddListItem(defImagePath, name, title);
		}
	}



void HomeView::getPhotos()
	{
	qDebug()<<"Inside HomeView::getPhotos()";
	
	// Get the list of providers
	SmfClient client;
	QString intfName("org.symbian.smf.client.gallery");
	
	SmfProviderList *providerList = client.GetServices(intfName);
	qDebug()<<"client.GetServices returned a list with count = "<<providerList->count();
	
	// flag to check if required plugin is there
	bool pluginFound = false;
	int index = 0;
	foreach(SmfProvider provider, *providerList)
		{
		if("Flickr" == provider.serviceName())
			{
			qDebug()<<"Plugin for flickr found";
			pluginFound = true;
			if(!m_gallery)
				m_gallery = new SmfGallery(&provider);
			bool ret = connect(m_gallery, SIGNAL(picturesAvailable(SmfPictureList*, SmfError , SmfResultPage)),
					this, SLOT(picturesAvailable(SmfPictureList*, SmfError , SmfResultPage)));
			
			qDebug()<<"Connected ?"<<ret;
			
#ifndef IFNOALBUMIDOFTHELOGGEDINUSERISAVAILABLE
			SmfPictureAlbumList list; // pass empty list to fetch photos which are not part of any set, when albumID is not available
#else		// If the album id is known use the below piece of code
			SmfPictureAlbumList list;
			SmfPictureAlbum album;
			album.setId("72157623348359220");
			list.append(album);
#endif
					
			m_gallery->pictures(list);
			}
		index++;
		}
	
	if(!pluginFound)
		{
		qDebug()<<"Plugin for flickr not found!!!";
		QString smferrString("No Flickr plugin found!!!");
		QMessageBox::information(m_mainWindow,"Error",smferrString,QMessageBox::Ok);
		}
	}


void HomeView::picturesAvailable(SmfPictureList* picList, SmfError error, SmfResultPage resultPage)
	{
	qDebug()<<"Inside HomeView::picturesAvailable()";
	
	m_picList = picList;
	if(error)
		{
		SmfClient client;
		QString errStr = client.errorString(error);
		qDebug()<<"Error found, code = "<<error;
		qDebug()<<"Error string is = "<<errStr;
		
		QMessageBox::information(m_mainWindow,"Error",errStr,QMessageBox::Ok);
		return;
		}
	
	//display pic description
	int count = 0;
	qDebug()<<"Number of pic retrieved = "<<picList->count();
	
	QMap<QString, QUrl> urlMap;
	foreach(SmfPicture pic, *picList)
		{
		qDebug()<<"Photo title = "<<pic.title();
		qDebug()<<"Photo posted date = "<<pic.postedDate();
		qDebug()<<"Photo url = "<<pic.url();
		qDebug()<<"Photo id = "<<pic.id();
		
		QString name(pic.title());
		QUrl url(pic.url());

		urlMap.insert(name, url);
		}

	// Download Images
	downloadImages(SmfPhotosFetch, urlMap);
	}


void HomeView::populatePhotosGridView()
	{
	qDebug()<<"Inside HomeView::populatePhotosGridView()";
	
	QStringList albumPicList;
	QStringList albumNameList;
	
	foreach(SmfPicture pic, *m_picList)
		{
		QString name(pic.title());
		
		qDebug()<<"Name  :"<<name;
		QString url("C:\\data\\");
		url.append(name);
		url.append(".jpg");
		
		
		albumNameList.append(name);
		albumPicList.append(url);
		}
	
	// Create the grip view
	iAlbumWidget->AddListItem(m_mainWindow,albumPicList,albumNameList);
//	AlbumView->CreateGridView(albumPicList,albumNameList);
	
	}
