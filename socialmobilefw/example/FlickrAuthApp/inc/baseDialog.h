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

#ifndef FB_DIALOG_H_
#define FB_DIALOH_H_

#include <QDialog>
#include <QWebView>
#include <qprogressbar.h>
#include <qboxlayout.h>
#include "authAppConstants.h"

// FORWARD DECLARATIONS
class FBSession;
class FBError;
class QNetworkReply;

//#define __WINSCW__

/**
  * The base Dialoggg
  */
class FBDialog : public QWidget
{
    Q_OBJECT

protected:

    /**
    * The session for which the login is taking place.
    */
    FBSession*	iSession;
    QWebView*	iWebView;
    QString	iLoadingUrl;
    bool        iIgnorePageLoadCompleteEvent;
    QProgressBar* progressbar;
    QVBoxLayout* layout;

signals:

    /**
     * Called when the dialog succeeds and is about to be dismissed.
     */
    void dialogDidSucceed ();

    /**
     * Called when the dialog is cancelled and is about to be dismissed.
     */
    void dialogDidCancel();

    /**
     * Called when dialog failed to load due to an error.
     */
    void dialogDidFailWithError ( const FBError& error );


	
private slots:
    void cancel();

    /* slots for signals from QWebView.page() */
    void linkClicked ( const QUrl & url );
    void loadStarted ();
    void loadProgress(int progress);
    void loadFinished ( bool ok );
    //void urlChanged ( const QUrl & url );
    //Network Error Slots
    void slotAuthenticationRequired( QNetworkReply* reply, QAuthenticator* authenticator );
    void slotsslErrors( QNetworkReply* reply, const QList<QSslError>& errors  );
    void slotproxyAuthenticationRequired( const QNetworkProxy& proxy, QAuthenticator* authenticator  );

public:
	
	void proxysettings();
    /**
    * Creates the view but does not display it.
    */
    FBDialog(FBSession* aSession);
    FBDialog();

    /**
    * Displays the view with an animation.
    *
    * The view will be added to the top of the current key window.
    */
    void show();
	
    /** Displays the first page of the dialog.
     *
     * Do not ever call this directly.  It is intended to be overriden by subclasses.
     */
    virtual void load ();
		
    /**
     * Displays a URL in the dialog.
     */
    void loadURL(const QString& aUrl, QNetworkAccessManager::Operation aMethod, const QHash<QString, QString>& aGetParams,
                 const QHash<QString, QString>&  aPostParams);


    void dismiss(bool aAnimated);

    /**
     * Hides the view and notifies delegates of success or cancellation.
     */
    void dismissWithSuccess (bool aSuccess, bool aAnimated);

    /**
    * Hides the view and notifies delegates of an error.
    */
    void dismissWithError (const FBError& aError);

    /**
    * Subclasses may override to perform actions just prior to showing the dialog.
    */
    virtual void dialogWillAppear();

    /**
     * Subclasses may override to perform actions just after the dialog is hidden.
     */
    virtual void dialogWillDisappear();

    /**
     * Subclasses should override to process data returned from the server in a 'fbconnect' url.
     *
     * Implementations must call dismissWithSuccess:YES at some point to hide the dialog.
     */
    virtual void dialogDidSucceed(const QUrl& aUrl);
	
    QString title() const;
    void setTitle ( const QString& aTitle );
    
    virtual void GetSessionKey(const QUrl& aUrl);
    
    virtual void connectToGetFullToken();

private:
    void createControls();
    QString generateURL( const QString& aUrl, const QHash<QString, QString>& aParams) const;
    QByteArray generatePostBody (const QHash<QString, QString>& aParams) const;

    void postDismissCleanup();

 };
#endif
