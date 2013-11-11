#ifndef TESTSCREEN_H
#define TESTSCREEN_H

#include <QtGui/QWidget>
#include "ui_SmfClientApp.h"
#include <smfcontactfetcher.h>
#include <smfpostprovider.h>
#include <QSplashScreen>
class TestScreen : public QWidget
{
    Q_OBJECT

public:
    TestScreen(QWidget *parent = 0);
    ~TestScreen();
	 //debugging
	 void writeLog(QString log) const;
public slots:
	void friendsButtonClicked();
	void postsButtonClicked();
	 void showGroups(SmfGroupList* grps, SmfError , SmfResultPage);
	 void showFriends(SmfContactList* frnds, SmfError , SmfResultPage);
	 void showPosts(SmfPostList* postlist, SmfError error, SmfResultPage resultPage);
private:
	 Ui::SmfClientApp ui;
    SmfProvider* m_provider;
    SmfContactFetcher* m_contactFetcher;
    SmfPostProvider* m_postProvider;
    QList<SmfProvider>* providerList;
    QPixmap pixmap;
    QSplashScreen splash;
};

#endif // TESTSCREEN_H
