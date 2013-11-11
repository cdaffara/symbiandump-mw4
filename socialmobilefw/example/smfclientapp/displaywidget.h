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

#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QtGui/QWidget>
#include "ui_displaywidget.h"
#include "smfclient.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <smfcontactfetcher.h>
#include <smfpostprovider.h>
#include <QSplashScreen>
class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    DisplayWidget(QWidget *parent = 0);
    ~DisplayWidget();
	 //debugging
	 void writeLog(QString log) const;
public slots:
	//void friendsButtonClicked();
	//void postsButtonClicked();
	// void showGroups(SmfGroupList* grps, SmfError , SmfResultPage);
	 void showFriends(SmfContactList* frnds, SmfError , SmfResultPage);
	 void showPosts(SmfPostList* postlist, SmfError error, SmfResultPage resultPage);
	 //void getData(int index);
	 void interfaceSelected();
	 void serviceProviderSelected();
	 void serviceSelected();
private:
   SmfProvider* m_provider;
   SmfContactFetcher* m_contactFetcher;
   SmfPostProvider* m_postProvider;
   QList<SmfProvider>* providerList;
   QPixmap pixmap;
   QSplashScreen splash;
    Ui::DisplayWidgetClass ui;
};

#endif // DISPLAYWIDGET_H
