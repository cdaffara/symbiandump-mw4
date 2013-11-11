/**
* Copyright (c) 2010 Sasken Communication Technologies Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "{License}"
* which accompanies  this distribution, and is available
* at the URL "{LicenseUrl}".
*
* Initial Contributors:
* Narasimhulu Kavadapu, Sasken Communication Technologies Ltd - Initial contribution
*
* Contributors:
*
* Description:
* Base class of All dialogs
*/

#define EMULATORTESTING // UnComment for Emulator build

#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebFrame>
#include <QFile>
#include <QDesktopServices>
#include <QResizeEvent>
#include <QDebug>
#include <qmessagebox.h>
#include <QNetworkAccessManager>
#include <QPropertyAnimation>
 #include <qnetworkproxy.h>
#include <qdesktopwidget.h>
#include <qapplication.h>
//#include <qboxlayout.h>
#include "baseDialog.h"
#include "sessionSP.h"
#include "errorCodes.h"

static const QString kDefaultTitle = "Connect to Facebook";
static const QString kStringBoundary = "3i2ndDfv2rTHiSisAbouNdArYfORhtTPEefj3q2f";

static QNetworkAccessManager namanager;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

FBDialog::FBDialog() : iSession(FBSession::session()), /*iWebView ( this  ),*/ iIgnorePageLoadCompleteEvent( false )
{
    createControls();
}

FBDialog::FBDialog(FBSession* aSession) : iSession ( aSession ), /*iWebView ( this  ) ,*/ iIgnorePageLoadCompleteEvent ( false )
{
    createControls();
}
void FBDialog::createControls()
{
	iWebView = new QWebView(this);
    iWebView->page()->setNetworkAccessManager(&namanager);

    iWebView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    
    layout = new QVBoxLayout(this);
    
    
    progressbar = new QProgressBar(this);
    
    layout->addWidget(iWebView);
    //layout->addWidget(progressbar);
    
    setLayout(layout);
    
    progressbar->setOrientation(Qt::Horizontal);
    

    connect( iWebView->page(), SIGNAL(linkClicked(const QUrl &)),
                    this, SLOT(linkClicked(const QUrl &)));

    connect ( iWebView->page(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    connect ( iWebView->page(), SIGNAL(loadStarted()), this, SLOT ( loadStarted()));
    
    connect ( iWebView->page(), SIGNAL(loadProgress(int)), this, SLOT ( loadProgress(int)));
    
    connect (iWebView->page()->networkAccessManager(),SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator*)),this,SLOT( slotAuthenticationRequired( QNetworkReply*, QAuthenticator*)));
    connect (iWebView->page()->networkAccessManager(),SIGNAL( sslErrors( QNetworkReply*,QList<QSslError>&)),this,SLOT( slotsslErrors( QNetworkReply*,QList<QSslError>&)) );
    connect (iWebView->page()->networkAccessManager(),SIGNAL( proxyAuthenticationRequired(QNetworkProxy&, QAuthenticator*)),this,SLOT( slotproxyAuthenticationRequired(QNetworkProxy&, QAuthenticator*)) );
    
}
QString FBDialog::generateURL( const QString& aUrl, const QHash<QString, QString>& aParams) const
{
	qDebug()<<"Inside FBDialog::generateURL()";
    QString url ( aUrl );

    QStringList pairs;
    QHashIterator<QString, QString> i(aParams);

    while (i.hasNext()) {
        i.next();

        QUrl url (i.value());
        QString pair = i.key() + "=" + url.toEncoded();
        pairs << pair.toUtf8();
    }

    if (pairs.count())
    {
        url = url + "?" + pairs.join("&");
    }

    return url;

}

QByteArray FBDialog::generatePostBody (const QHash<QString, QString>& aParams) const
{
	qDebug()<<"Inside FBDialog::generatePostBody()";
    QByteArray body;

    if (!aParams.count())
        return body;


    QString endLine = "\r\n--" + kStringBoundary + "\r\n", kStringBoundary;

    QString tmp = "--" + kStringBoundary + "\r\n";
    body.append(tmp.toUtf8());


    QHashIterator<QString, QString> i(aParams);
    while (i.hasNext()) {
        i.next();

        tmp = "Content-Disposition: form-data; name=\"" + i.key().toUtf8() + "\"\r\n\r\n" ;
        body.append(tmp.toUtf8());
        body.append(i.value().toUtf8());
        body.append(endLine.toUtf8());
    }

    return body;
}

void FBDialog::postDismissCleanup()
{
    //accept();
    // could also be reject()?
}

void FBDialog::dismiss (bool /*aAnimated*/) {
	qDebug()<<"Inside FBDialog::dismiss()";
    dialogWillDisappear();
    iLoadingUrl.clear();

    //todo: do some animations if aAnimated == true !
    postDismissCleanup();
}


void FBDialog::dismissWithSuccess( bool aSuccess, bool aAnimated)
{
	qDebug()<<"Inside FBDialog::dismissWithSuccess()";
  if (aSuccess) {
        emit dialogDidSucceed();
  } else {
        emit dialogDidCancel();
  }

  dismiss(aAnimated);
}

void FBDialog::dismissWithError (const FBError& aError, bool aAnimated)
{
	qDebug()<<"Inside FBDialog::dismissWithError()";
  emit dialogDidFailWithError( aError );
  dismiss(aAnimated);
}
void FBDialog::slotAuthenticationRequired( QNetworkReply* reply, QAuthenticator* authenticator )
	{
		QMessageBox msgbox;
		QString msg ("Error!Authentication Required");
		msgbox.setText(msg);
	}
void FBDialog::slotsslErrors( QNetworkReply* reply, const QList<QSslError>& errors  )
	{
		QMessageBox msgbox;
		QString msg ("Error!SSL Error");
		msgbox.setText(msg);
	}
void FBDialog::slotproxyAuthenticationRequired( const QNetworkProxy& proxy, QAuthenticator* authenticator  )
	{
		QMessageBox msgbox;
		QString msg ("Error!Proxy Authenticatio Required");
		msgbox.setText(msg);
	}
void FBDialog::cancel()
{}

void FBDialog::load() {
	qDebug()<<"Inside FBDialog::load()";
}

void FBDialog::show()
{
	qDebug()<<"Inside FBDialog::show()";
    load();
    showMaximized();
    dialogWillAppear();

}

void FBDialog::loadURL(const QString& aUrl, QNetworkAccessManager::Operation aMethod, const QHash<QString, QString>& aGetParams, const QHash<QString, QString>&  aPostParams)
{   
	qDebug()<<"Inside FBDialog::loadURL()";
    //proxysettings();
    iIgnorePageLoadCompleteEvent = false;

    QNetworkCookieJar* cookieJar = iWebView->page()->networkAccessManager()->cookieJar();
    QByteArray body;

    iLoadingUrl = generateURL(aUrl, aGetParams);

    // This "test cookie" is required by login.php, or it complains that you need to enable JavaScript
    QNetworkCookie testCookie ("test_cookie", "1");
    testCookie.setDomain ( "www.facebook.com" );
    testCookie.setPath ( "/" );

    QList<QNetworkCookie> cookieList;
    cookieList.append(testCookie);

    cookieJar->setCookiesFromUrl ( cookieList, QUrl(iLoadingUrl) );

    QUrl url (iLoadingUrl);
    QNetworkRequest request(url);

    if (aMethod == QNetworkAccessManager::PostOperation)
    {
        const QString contentType = "multipart/form-data; boundary=" + kStringBoundary;
        request.setHeader (QNetworkRequest::ContentTypeHeader, contentType);
        body = generatePostBody (aPostParams);
    }
    proxysettings();
    
    qDebug()<< "Check URL : " << iLoadingUrl;

    iWebView->load ( request, aMethod, body);
    
}
void FBDialog::proxysettings()
{
#ifdef EMULATORTESTING
	qDebug()<<"proxysettings";
	
	// Reading the keys, CSM Stubbed - START
	QFile file("c:\\data\\DoNotShare.txt");
	if (!file.open(QIODevice::ReadOnly))
		{
		qDebug()<<"File to read the windows username and password could not be opened, returning!!!";
		return;
		}
	
	QByteArray arr = file.readAll();
	QList<QByteArray> list = arr.split(' ');
	file.close();
	
	QString username(list[0]);
	QString password(list[1]);
	
    QString httpProxy = "10.1.0.214";//ipwproxy.sasken.com
    QString httpPort = "3128";

    QString httpUser =username;/* This could be taken thru an QDialog implmentation to remove the Hard coding */
    QString httpPass =password;/* This could be taken thru an QDialog implmentation to remove the Hard coding */

    /*==Classes used from Network Module==*/
    QNetworkProxy proxy;

    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(httpProxy);
    proxy.setPort(httpPort.toInt());
    proxy.setUser(httpUser);
    proxy.setPassword(httpPass);

    QNetworkProxy::setApplicationProxy(proxy);
#endif
}
void FBDialog::dialogWillAppear() {}

void FBDialog::dialogWillDisappear() {}

void FBDialog::dialogDidSucceed (const QUrl& /*aUrl*/) {
  dismissWithSuccess(true,true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FBDialog::linkClicked ( const QUrl & url )
 {

        qDebug() << "Loading the url: " <<  url;
        
        proxysettings();
			
        iWebView->load(url);
}

void FBDialog::loadStarted()
{
    qDebug() << "Load started: " << iWebView->url();
    layout->addWidget(progressbar);
    progressbar->setVisible(true);
}
void FBDialog::loadProgress(int progress)
{
	progressbar->setValue(progress);
}
void FBDialog::GetSessionKey(const QUrl& aUrl)
{

}
void FBDialog::FetchKeyFromUrl(const QUrl& aUrl)
{

}
void FBDialog::loadFinished ( bool ok )
{
    qDebug() << "Load " << (ok ? "" : "un") << "successfull for: " << iWebView->url();
    progressbar->setVisible(false);
    layout->removeWidget(progressbar);
    if (ok)
    {
		QString PAth = iWebView->url().toString();
		PAth = PAth.mid(0,PAth.indexOf("?"));
		
		qDebug() << "Path is : " << PAth;
		
		if(KSuccessurl.compare(PAth) == 0){
		
			QByteArray URL = iWebView->url().encodedQuery();
			qDebug() << "Encoded Query is : " << URL;
			
			if(URL.contains("auth_token"))
				{
					qDebug() << "URL contains auth token";
					iWebView->setHidden(true);
					QMessageBox msgbox;
					QString msg ("Logged in Success!!!Complete the Authorization?");
					msgbox.setText(msg);
					if(msgbox.exec() == QMessageBox::Ok){
						GetSessionKey(iWebView->url());
					}else{
						
					}
				}
			else if(URL.contains("session_key") && URL.contains("uid"))
				{
					FetchKeyFromUrl(iWebView->url());
				}
			else
				{
					qDebug() << "URL doesnt have auth_token field";
					iWebView->setHidden(true);
					QMessageBox msgbox;
					QString msg ("Permissions Success!!!continue login?");
					msgbox.setText(msg);
					if(msgbox.exec() == QMessageBox::Ok){
						iWebView->setHidden(false);
						load();
					}else{
						
					}
				}
		}
        /*else if (URL.contains("read_stream") || URL.contains("publish_stream") || URL.contains("offline_access"))
        	{
				iWebView->setHidden(true);
				QMessageBox msgbox;
				QString msg ("Permissions Success!!!continue login?");
				msgbox.setText(msg);
				if(msgbox.exec() == QMessageBox::Ok){
					iWebView->setHidden(false);
					load();
				}else{
					
				}
        	}*/
    }
    else
    {
        if (iIgnorePageLoadCompleteEvent)
            return;

        FBError err;
        dismissWithError(err, true);
    }
}
