#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <qmainwindow.h>
#include <WidgetConstants.h>
//#include <smfglobal.h>
#include <smfcontact.h>
#include <smfpost.h>
#include <smfpicturealbum.h>
#include <smfactions.h>
#include <smfpicture.h>
#include <smfcontactfetcher.h>
#include <smfpostprovider.h>
#include <smfgallery.h>
#include <smfactivityfetcher.h>
#include "ImageDownload.h"
#include "ToolBar.h"
#include <qmenu.h>

class CustomListWidget;
class GridView;

class HomeView : public QObject
{
	Q_OBJECT
public:
    HomeView(QString aHeadName);
    ~HomeView();
    void SetupUI(QMainWindow *Mainwindow);
    void populateFriendsWidget();
    void populatePostsWidget();
    void populateActivitiesWidget();
    void populatePhotosGridView();
    
private:
    void getFriends();
    void getPosts();
    void downloadImages(const SmfItemIdentifier &identifier, const QMap<QString, QUrl> urlMap);
    void getAlbums();
    void getActivities();
    void getPhotos();
    
private slots:
    void friendsAvailable(SmfContactList* friendsList, SmfError , SmfResultPage );
    void postsAvailable(SmfPostList* postsList, SmfError , SmfResultPage );
    void albumsAvailable(SmfPictureAlbumList* albums, SmfError error, SmfResultPage resultPage);
    void activitiesAvailable(SmfActivityEntryList* postsList, SmfError , SmfResultPage );
    void picturesAvailable(SmfPictureList* albums, SmfError error, SmfResultPage resultPage);
    void Navigated2OtherTab(int index);
    
private:
    QString iHeadName;
    ToolBar* ToolBarwidget;
    QMainWindow *m_mainWindow;
    QTabWidget *TabWidget;
    CustomListWidget *iFrndsListWidget;
    CustomListWidget *iPostsListWidget;
    CustomListWidget *iActivityListWidget;
    CustomListWidget *iAlbumWidget;
    GridView *AlbumView;
    ImageDownload m_downloader;
    
    SmfContactFetcher *m_contactFetcher;
    SmfPostProvider *m_postProvider;
    SmfGallery *m_gallery;
    SmfActivityFetcher *m_activityFetcher;
    SmfContactList *m_friendsList;
    SmfPostList *m_postsList;
    SmfPictureAlbumList *m_albumsList;
    SmfActivityEntryList *m_activitiesList;
    SmfPictureList *m_picList;
    
};

#endif // HOMEVIEW_H
