#include "smfglobal.h"
#include "smfclient.h"
#include "smfprovider.h"
#include "smfgallery.h"
#include "smfcontact.h"
#include "smfpostprovider.h"
#include "smfcontactfetcher.h"
#include "smfmusic.h"
#include "smfpicture.h"
#include "smftrackinfo.h"
#include "smfmusicprofile.h"
#include "smflyrics.h"
#include "smfactions.h"
#include "smfactivityfetcher.h"
#include <qalgorithms.h>
#include "qtcontacts.h"
#include <qdatastream.h>
#include <QSharedData>
#include <smfclientglobal.h>

using namespace QtMobility;

class MyAppView //: public QAbstractItemView
	{
public:
	void add(QImage pic);
	void add(SmfTrackInfo trackInfo);
	void add(QContact qc);
	void setPicture(QImage image);
	void setTitle(QString cap);	
	void setIcon(QImage image);
	void setDescription(QString desc);
	void setProvider(SmfProvider p);
	void setLyricsData(SmfLyrics l);
	};


class MyApplication :public QObject
	{
	Q_OBJECT
	
public slots:
	void displayGallery();
	void showPicsSlot(SmfPictureList* pics, QString err);
	void uploadPicture(QImage* picture, QList<SmfGallery> galleries);
	void uploaded(bool success);

	void displayFriends();
	void showlist(SmfContactList* friendsList);
	void postUpdate();
	void showPosts(SmfPostList* posts, QString err);

	void getMusic(SmfTrackInfo currTrack);
	void showTrackSearch(SmfTrackInfoList* songs);
	void showStore(SmfProviderList* stores);
	void updateCurrentPlaying(QList<SmfMusicSearch> musicServices, SmfTrackInfo currTrack);
	void displayLyrics(SmfTrackInfo currTrack);
	void showLyrics(SmfLyricsList* list);

	void getActivities();
	void showActivities(SmfActivityEntryList* entries);
private:
	MyAppView* m_view;
	SmfClient client;
	SmfGallery* m_smfgl;
	SmfContactFetcher* m_smfcf;
	SmfMusicService* m_smfms;
	QList<SmfContact> m_myfrndz;
	};
/** 1. Display a gallery on the screen for some remote service.
 * assume m_view is some gallery view object in the application.*/
void MyApplication::displayGallery()
	{
	// Some common interface for finding implementations.
	QList<SmfProvider>* galleries = client.GetServices("org.symbian.smf.client.gallery\0.2");

	// We will use the first one now
	SmfProvider smfp = galleries->value(0);
	SmfGallery* myGallery = new SmfGallery(&smfp);

	// Adjust our view to show where these pictures came from
	QImage imge = smfp.serviceIcon();
	QString desc = smfp.description();
	QString name = smfp.serviceName();
	m_view->setIcon(imge);
	m_view->setProvider(smfp);
	m_view->setDescription(desc);

	/**
	 * Asynchrnous request to fetch the pictures.
	 * The picturesAvailable() signal is emitted 
	 * with SmfPictureList once the pictures have arrived.
	 */
	myGallery->pictures();
	connect(myGallery,SIGNAL(picturesAvailable(SmfPictureList*, QString, SmfResultPage )),
			SLOT(showPicsSlot(SmfPictureList*, QString)));
	
	m_smfgl = myGallery;
	}

void MyApplication::showPicsSlot(SmfPictureList* pics, QString err)
	{
	//check err string if there is any error
	if(err.compare("Err")) return;
	//if no error
	foreach(SmfPicture pic, *pics) {
		m_view->add(pic.picture() ); // do something with the picture in this gallery
	}
	}

/** 2. Upload a picture captured by the user to some selection of galeries.*/
void MyApplication::uploadPicture(QImage* picture, QList<SmfGallery> galleries)
	{
	/**
	 * When uploading is finished we can check the success of the uploading
	 */	
	QObject::connect(m_smfcf,SIGNAL(uploadFinished(bool)),SLOT(uploaded(bool)));
	SmfPicture*  smfPic = new SmfPicture(*picture);
	// The list could be from a selection of galleries chosen by the user,
	// think multiple TweetDeck accounts?
	foreach(SmfGallery gallery, galleries)
		{
		gallery.upload(smfPic);
		}
	}

/**
 * Slot to catch the uploading finished event
 */
void MyApplication::uploaded(bool success)
	{
	if(!success)
		{
	//error occured while uploading
		}
	}

/**
 * 3. This is an example of displaying the friends profile image in a view from one or more
 * service provider. Note that this service can be provided by any kind of service provider,
 * e.g. last.fm music service where users maintain profiles and friends.
 */
void MyApplication::displayFriends()
	{
	// Some common interface for finding implementations.
	QList<SmfProvider>* contactFetcherList = client.GetServices("org.symbian.smf.cleint.contact.fetcher\0.2");
	SmfProvider smfp = contactFetcherList->value(0);
	SmfContactFetcher* smfcf = new SmfContactFetcher(&smfp);
	
	//Request friend list, the friendsListAvailable() signal
	//is emitted with SmfContactList once data is arrived.
	QObject::connect(smfcf,SIGNAL(friendsListAvailable(SmfContactList*, QString, SmfResultPage )),
			SLOT(showlist(SmfContactList*)));
	smfcf->friends();
	
	m_smfcf = smfcf;
	}

void MyApplication::showlist(SmfContactList* friendsList)
	{

	// Adjust our view to show where these pictures came from
	//display service name description and the logo
	m_view->setIcon( (m_smfcf->getProvider())->serviceIcon() );
	m_view->setDescription( (m_smfcf->getProvider())->description() );

	//now display the images
	foreach(SmfContact contact, *friendsList) {
		QVariant data = contact.value("Avatar"); 
		QImage pic = data.value<QImage>();
		QContact qc;
		contact.convert(qc);
		m_view->add(qc);
		m_myfrndz.append(contact);
	}
	}
/**
 * 4. This is an example of posting and reading user updates to social netowrking sites
 */
void MyApplication::postUpdate()
	{
	// Some common interface for finding implementations.
	QList<SmfProvider>* postServices = client.GetServices("org.symbian.smf.client.contact.posts\0.2");

	//let us use the first one
	QString servName = postServices->value(0).serviceName();
	if(!servName.compare("Facebook.com")) return;
	SmfProvider smfp = postServices->value(0);
	SmfPostProvider* myPostServer = new SmfPostProvider(&smfp);

	//Adjust our view to show where these posts came from (e.g. tweets from twitter)
	//display service name description and the logo
	m_view->setIcon((myPostServer->getProvider())->serviceIcon() );
	m_view->setProvider(myPostServer->getProvider());
	m_view->setDescription((myPostServer->getProvider())->description() );

	SmfPost reply("this is a text post", this);
	//post my udpate to be visible to all, connect to updatePostFinished()
	// signal of SmfPostProvider to track the success
	SmfContact frnd(m_myfrndz.value(0));
	myPostServer->postDirected(reply,frnd);

	//Asynchronously get all posts to me in my profle (e.g. twits from all friends)
	//connect to postsAvailable to show the post
	myPostServer->posts();

	QObject::connect(myPostServer,
			SIGNAL(postsAvailable(SmfPostList*, QString, SmfResultPage )),
			SLOT(showPosts(SmfPostList*, QString)));
	}
void MyApplication::showPosts(SmfPostList* posts, QString /*err*/)
	{
	//Show the first post  
	SmfPost post = posts->at(0);
	m_view->setDescription( post.toPlainText() );
	}
/**
 * 5. This is an example of getting song recommendations from a social netowrking sites
 */
void MyApplication::getMusic(SmfTrackInfo currTrack)
	{
	// Some common interface for finding implementations.
	QList<SmfProvider>* smfProList = client.GetServices("org.symbian.smf.client.music\0.2");
	SmfProvider smfp = smfProList->value(0);
	SmfMusicSearch* mServer = new SmfMusicSearch(&smfp);

	QObject::connect(mServer,SIGNAL(trackSearchAvailable(SmfTrackInfoList*, QString,SmfResultPage )),this,SLOT(showTrackSearch(SmfTrackInfoList*)));
	QObject::connect(mServer,SIGNAL(storeSearchAvailable(SmfProviderList*, QString,SmfResultPage )),this,SLOT(showStoreSearch(SmfProviderList*)));
	//search songs similar to currently playing,
	//connect to trackSearchAvailable signal to get the result
	mServer->recommendations(currTrack);
	//display to the user
	m_view->setIcon( mServer->getProvider()->serviceIcon() );
	m_view->setProvider( mServer->getProvider());
	m_view->setDescription( mServer->getProvider()->description() );



	}
void MyApplication::showTrackSearch(SmfTrackInfoList* songs)
	{
	foreach(SmfTrackInfo track, *songs){
		m_view->add(track);
	}
	QList<SmfProvider>* smfProList = client.GetServices("org.symbian.smf.client.music.search\0.2");
	SmfProvider smfp = smfProList->value(0);
	SmfMusicSearch* mServer = new SmfMusicSearch(&smfp);
	//allow user to select a track and get purchase links
	//connect to showStoreSearch signal to display the stores for that track
	mServer->stores(songs->value(0));
	}
void MyApplication::showStore(SmfProviderList* /*stores*/)
	{
	//show stores
	}
void MyApplication::updateCurrentPlaying(QList<SmfMusicSearch> musicServices, SmfTrackInfo currTrack)
	{
	//after purchasing and downloading is over, user plays the track
	//now post the current platying track to all service providers
	//postFinished() signal of SmfMusicSearch can be tracked to check the success of the posts
	foreach(SmfMusicSearch provider, musicServices) {
		provider.postCurrentPlaying(currTrack);
	}
	//postCurrentPlaying is also a slot funtion, may be application can use connect
	}

void MyApplication::displayLyrics(SmfTrackInfo currTrack)
	{

	// Some common interface for finding implementations.
	QList<SmfProvider>* smfProList = client.GetServices("org.symbian.smf.client.music.lyrics\0.2","lyricsfly.com");
	SmfProvider smfp = smfProList->value(0);
	SmfLyricsService* lyricsService = new SmfLyricsService(&smfp);
	QObject::connect(lyricsService,SIGNAL(lyricsAvailable(SmfLyricsList*, QString, SmfResultPage )),this,SLOT(showLyrics(SmfLyricsList*)));

	//Request to get the lyrics
	//lyricsAvailable() signal of SmfLyricsService is emitted when lyrics is available
	lyricsService->lyrics(currTrack);

	}
void MyApplication::showLyrics(SmfLyricsList* list)
	{
	//now display the latest edited lyrics
	//qSort(list->begin(),list->end(),caseInsensitiveLessThan);
	m_view->setLyricsData(list->at(0));
	}

void MyApplication::getActivities()
	{
	// Some common interface for finding implementations.
	QList<SmfProvider>* activityServices = client.GetServices("org.symbian.smf.client.activity.fetcher\0.2");

	//let us use the Facebook one
	QString servName = activityServices->value(0).serviceName();
	if(!servName.compare("Facebook.com")) return;
	SmfProvider smfp = activityServices->value(0);
	SmfActivityFetcher* myActivityServer = new SmfActivityFetcher(&smfp);

	//Adjust our view to show where these posts came from (e.g. streams from Facebook)
	//display service name description and the logo
	m_view->setIcon((myActivityServer->getProvider())->serviceIcon() );
	m_view->setProvider(myActivityServer->getProvider());
	m_view->setDescription((myActivityServer->getProvider())->description() );
	
	QObject::connect(myActivityServer,
			SIGNAL(resultsAvailable(SmfActivityEntryList*,QString,SmfResultPage)),
			this,SLOT(showActivities(SmfActivityEntryList*)));	

	//get a list of updates to my wall
	myActivityServer->selfActivities();
	}

void MyApplication::showActivities(SmfActivityEntryList* entries)
	{
	foreach(SmfActivityEntry entry, *entries) {
		SmfContact sc = entry.author();
		QString desc = (sc.value("Name")).toString();
		SmfPost details = entry.details();
		QList<SmfActivityObject> aol = entry.activities();
		SmfActivityObject sao = aol.value(0);
		if(SmfActivityMarkAsFavorite == entry.actionName() )
			{
			desc.append("  has marked  ");
			QVariant qv = sao.objData();
			SmfActivityObjectType otype = sao.type();
			if((SmfActivityObjTypePhoto == otype ) && (qv.canConvert<SmfPicture>()))
				{
				SmfPicture pic = qv.value<SmfPicture>();
				desc.append( pic.description() );
				}
			desc.append("  as Favorite at");
			QDateTime qdt = sao.time();
			desc.append(qdt.toString("h:m:s ap"));
			}
	}
	}
