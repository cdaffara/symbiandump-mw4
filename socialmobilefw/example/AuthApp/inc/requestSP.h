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
* class to handle calls to rest Server API's
*/
#ifndef FBREQUEST_H
#define FBREQUEST_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QHttp>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "progressbar.h"
#include "authAppConstants.h"
#include "xmlParser.h"

// FORWARD DECLARATIONS
class FBSession;
class FBError;

///////////////////////////////////////////////////////////////////////////////////////////////////
class  FBRequest : public QObject
{
    Q_OBJECT

private:

    /* pointer to the session that owns this request */
    FBSession*  iSession;

    /**
     * The URL which will be contacted to execute the request.
     */
    QString iUrl;

    /**
     * The API method which will be called.
     */
    QString iMethod;

    /**
     * An object used by the user of the request to help identify the meaning of the request.
     */
    void*   iUserInfo;

    /**
     * The dictionary of parameters to pass to the method.
     *
     * These values in the dictionary will be converted to strings using the
     * standard Objective-C object-to-string conversion facilities.
     */
    Dictionary  iParams;

    /**
     * A data parameter.
     *
     * Used for methods such as photos.upload, video.upload, events.create, and
     * events.edit.
     */
    QByteArray  iDataParam;

    /**
      * true if iDataParam holds picture data
      */
    bool        iDataParamPicture;

    /**
     * The timestamp of when the request was sent to the server.
     */
    QDateTime   iTimestamp;
    QHttp       iConnection;
    QByteArray  iResponseText;

    QNetworkAccessManager iNetworkAccessManager;
    progressbar* pbar;

signals: /* the signals ommitted by FBRequest */

    /**
     * Called just before the request is sent to the server.
     */
    void requestLoading();

    /**
     * Called when the server responds and begins to send back data.
     */
    //todo: void requestDidReceiveResponse (FBRequest* aRequest, NSURLResponse* aResponse);

    /**
     * Called when an error prevents the request from completing successfully.
     */
    void requestFailedWithNetworkError( QNetworkReply::NetworkError code );
    void requestFailedWithFacebookError ( const FBError& aError );

    /**
     * Called when a request returns and its response has been parsed into an object.
     *
     * The resulting object may be a dictionary, an array, a string, or a number, depending
     * on thee format of the API response.
     */
    void requestDidLoad ( const QVariant& aResult);

    /**
     * Called when the request was cancelled.
     */
    void requestWasCancelled ();


private slots:
    void networkReplyError ( QNetworkReply::NetworkError code );
    void networkReplyFinished ();

public: /* class functions */
    /**
     * Creates a new API request for the global session.
     */
    static FBRequest* request();

    /**
     * Creates a new API request for a particular session.
     */
    static FBRequest* requestWithSession (FBSession* aSession);

public: /* instance functions */

    /**
     * Creates a new request paired to a session.
     */
    FBRequest (FBSession* aSession);

    /**
     * Calls a method on the server asynchronously.
     *
     * The delegate will be called for each stage of the loading process.
     */
    void call (const QString& aMethod, const Dictionary& aParams);

    /**
     * Calls a method on the server asynchronously, with a file upload component.
     *
     * The delegate will be called for each stage of the loading process.
     */
    void callWithDataParams (const QString& aMethod, const Dictionary& aParams, const QByteArray& aDataParam, bool aDataParamPicture);

    /**
     * Calls a URL on the server asynchronously.
     *
     * The delegate will be called for each stage of the loading process.
     */
    void post( const QString& aUrl, const Dictionary& aParams);

    /**
     * Stops an active request before the response has returned.
     */
    void cancel();

    /**
      * returns the time stamp of when the request was sent to the server
      */
    const QDateTime& timeStamp() const;

    void connect();

private:
    /**
      * Given a string returns its hex coded md5 hash
      */
    static QString md5(const QString&);

    /**
      * @return true if the current request method is a special method
      */
    bool isSpecialMethod() const;

    /**
      * @return QString a url to use for the given method
      */
    QString urlForMethod (const QString& aMethod) const;

    /**
      * @return the Get Url for the request
      */
    QString generateGetURL() const;

    QString generateCallId() const;
    QString generateSig();

    void generatePostBody(QByteArray& body);

    /**
      * handles the data received from the server
      * @param aResponseData is the data received from the server
      */
    void handleResponseData( const QByteArray& aResponseData );

    /**
      * @param aResponseData is the data received from the server
      * @param aError will get error codes if any error occurs ( this will change in the future )
      * @return a void* pointer, this will change
      */
    QVariant parseXMLResponse ( const QByteArray& aResponseData, FBError& aError);

};

#endif // FBREQUEST_H
