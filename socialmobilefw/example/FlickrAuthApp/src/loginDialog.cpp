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
* Login Dialog class of Authentication Application
*/

#include "authAppConstants.h"
#include "sessionSP.h"
#include "errorCodes.h"
#include "loginDialog.h"
#include <qdebug.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// global

static const QString kLoginURL = "http://www.flickr.com/services/auth/"; 
//static const QString kLoginURL = "http://www.flickr.com/auth-72157623913597531";


///////////////////////////////////////////////////////////////////////////////////////////////////
void FBLoginDialog::connectToGetFullToken()
{
	if(iGetSessionRequest == NULL ){
		
		iGetSessionRequest = FBRequest::requestWithSession(iSession);
	
		connect(iGetSessionRequest, SIGNAL(requestDidLoad(QVariant)), this, SLOT(requestDidLoad(QVariant)));
		connect(iGetSessionRequest, SIGNAL(requestFailedWithNetworkError(QNetworkReply::NetworkError)), this, SLOT(requestFailedWithNetworkError(QNetworkReply::NetworkError)));
		connect(iGetSessionRequest, SIGNAL(requestFailedWithFlickrError(FBError)), this, SLOT(requestFailedWithFlickrError(FBError)));
		
	}
	
	 iGetSessionRequest->callforToken();
}
void FBLoginDialog::connecttoGetFrob()
{
	qDebug()<<"Inside FBLoginDialog::connecttoGetFrob";
    iGetSessionRequest = FBRequest::requestWithSession(iSession);

    connect(iGetSessionRequest, SIGNAL(requestDidLoad(QVariant)), this, SLOT(requestDidLoad(QVariant)));
    connect(iGetSessionRequest, SIGNAL(requestFailedWithNetworkError(QNetworkReply::NetworkError)), this, SLOT(requestFailedWithNetworkError(QNetworkReply::NetworkError)));
    connect(iGetSessionRequest, SIGNAL(requestFailedWithFlickrError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));

    Dictionary params;

    iGetSessionRequest->call("flickr.auth.getFrob", params);
    
}
void FBLoginDialog::loadLoginPage()
{
    Dictionary getParams, postParams;
    
    getParams["api_key"] = iSession->apiKey();
    getParams["perms"] = "delete";
    getParams["frob"] = iSession->frobKey();
    
    FBRequest* req = FBRequest::requestWithSession(iSession);
    QString sig = req->generateSig(getParams);
    
    getParams["api_sig"] = sig;
    
    QString fullURL;
    fullURL = kLoginURL /*+ "?" + "api_key=" + iSession->apiKey() + "&perms=delete&frob=" + iSession->frobKey() + "&api_sig=" + sig*/;
    
    loadURL(fullURL,QNetworkAccessManager::GetOperation, getParams, postParams);

}
///////////////////////////////////////////////////////////////////////////////////////////////////

FBLoginDialog::FBLoginDialog() : FBDialog () {}

FBLoginDialog::FBLoginDialog(FBSession* aSession) : FBDialog(aSession) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FBDialog

void FBLoginDialog::load()
{
    loadLoginPage();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// slots for signals from FBRequest

void FBLoginDialog::requestDidLoad(const QVariant& aResult)
{
	qDebug()<<"Inside FBLoginDialog::requestDidLoad";
    bool conversionError = false;
    QVariantHash object = aResult.toHash();
    
    QString frob = object.value("frob").toString();
    if(frob.length()){
    	iSession->SaveFrobKey(frob);
    	show();
    }
    qDebug()<<"frob = "<<frob;
    qDebug() << "Root Object key : " << object.keys().takeFirst() << endl;
    
    if(object.keys().takeFirst().compare("auth") == 0)
    {
    	
		QVariantHash childobject = object.value("auth").toHash();
		qDebug() << "Child Object data : " << childobject << endl;
		QString token = childobject.value("token").toString();
		qDebug() << "Read Token value : " << token << endl;
		if(token.length()){
			iSession->Savetoken(token);
		}
  	
    }
}

void FBLoginDialog::requestFailedWithFlickrError (const FBError& aCode )
{
    dismissWithError(aCode);
}

void FBLoginDialog::requestFailedWithNetworkError( QNetworkReply::NetworkError aCode )
{
    dismissWithError(aCode);
}
