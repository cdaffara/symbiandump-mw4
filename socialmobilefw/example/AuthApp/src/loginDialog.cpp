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

#include <qdebug.h>

#include "authAppConstants.h"
#include "sessionSP.h"
#include "errorCodes.h"
#include "loginDialog.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// global

static const QString kLoginURL = "http://www.facebook.com/login.php";

///////////////////////////////////////////////////////////////////////////////////////////////////
void FBLoginDialog::connectToGetSession(const QString& aToken)
{
	qDebug()<<"Inside FBLoginDialog::connectToGetSession()";
    iGetSessionRequest = FBRequest::requestWithSession(iSession);

    connect(iGetSessionRequest, SIGNAL(requestDidLoad(QVariant)), this, SLOT(requestDidLoad(QVariant)));
    connect(iGetSessionRequest, SIGNAL(requestFailedWithNetworkError(QNetworkReply::NetworkError)), this, SLOT(requestFailedWithNetworkError(QNetworkReply::NetworkError)));
    connect(iGetSessionRequest, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));

    Dictionary params;
    params["auth_token"] = aToken;

    if (iSession->apiSecret().length())
    {
        params["generate_session_secret"]="1";
    }

    if (iSession->getSessionProxy().length())
    {
        iGetSessionRequest->post(iSession->getSessionProxy(),params);
    }
    else
    {
        iGetSessionRequest->call("facebook.auth.getSession", params);
    }
}

void FBLoginDialog::loadLoginPage()
{
	qDebug()<<"Inside FBLoginDialog::loadLoginPage()";
    Dictionary getParams, postParams;
    getParams["api_key"] = iSession->apiKey();
    getParams["connect_display"] = "popup";
    getParams["v"]="1.0";
    getParams["next"] = KSuccessurl;
    getParams["cancel_url"] = "http://www.facebook.com/connect/login_failure.html";
    getParams["fbconnect"] = "1";
    getParams["return_session"]="1";   // Uncomment this to get session in the url
    getParams["session_key_only"]="1"; // Uncomment this to get session in the url
    getParams["ext_perm"/*"req_perms"*/] = "read_stream,publish_stream,offline_access,user_photos,friends_photos,user_status,friends_status,user_location,friends_location,user_activities,friends_activities,user_relationships,friends_relationships,read_requests,read_friendlists";
   // req_perms=read_stream,publish_stream,offline_access

    loadURL(kLoginURL,QNetworkAccessManager::GetOperation, getParams, postParams);

}
///////////////////////////////////////////////////////////////////////////////////////////////////

FBLoginDialog::FBLoginDialog() : FBDialog () {}

FBLoginDialog::FBLoginDialog(FBSession* aSession) : FBDialog(aSession) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FBDialog

void FBLoginDialog::load()
{
	qDebug()<<"Inside FBLoginDialog::load()";
    loadLoginPage();
}

void FBLoginDialog::dialogWillDisappear()
{
	qDebug()<<"Inside FBLoginDialog::dialogWillDisappear()";
  iGetSessionRequest->cancel();

  if (!iSession->isConnected())
  {
      iSession->cancelLogin();
  }
}

void FBLoginDialog::GetSessionKey(const QUrl& aUrl)
{
	qDebug()<<"Inside FBLoginDialog::GetSessionKey()";
    const QString authToken = "auth_token=";
    const QString url ( aUrl.toString() );

    int start = url.indexOf(authToken);
    if (start != -1)
    {
        QString token;
        int end = url.indexOf("&", start);
        int delta = start + authToken.size() + 1;
        if (end != -1)
        {
            token = url.mid(delta, end - delta );
        }
        else
        {
            token = url.right(url.size() - delta + 1);
        }

        connectToGetSession(token);
    }
}
void FBLoginDialog::FetchKeyFromUrl(const QUrl& aUrl)
{
    const QString url ( aUrl.toString() );
    QStringList varList = url.split("{");
    QString newvar = varList[1];
    varList.clear();
    varList = newvar.split(",");
    QString session_key = varList[0];
    QString uid = varList[1];
    QString expires = varList[2];
    QString secret = varList[3];
    varList.clear();
    varList = session_key.split(":");
    session_key = varList[1];
    session_key = session_key.mid(1,session_key.length()-2);
    varList.clear();
    varList = uid.split(":");
    uid = varList[1];
	varList.clear();
	varList = expires.split(":");
	expires = varList[1];
	varList.clear();
	varList = secret.split(":");
	secret = varList[1];
	secret = secret.mid(1,secret.length()-2);
	varList.clear();
    qDebug()<<"session_key"<<session_key<<"\n";
    qDebug()<<"uid"<<uid<<"\n";
    qDebug()<<"expires"<<expires<<"\n";
    qDebug()<<"secret"<<secret<<"\n";
    bool ok = false;
    uint expiry = expires.toUInt();//.toUInt(&conversionError);
    QDateTime expiration; expiration.setTime_t( expiry );
    iSession->beginSession(session_key, secret,expiration);
    iSession->resume();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// slots for signals from FBRequest

void FBLoginDialog::requestDidLoad(const QVariant& aResult)
{
	qDebug()<<"Inside FBLoginDialog::requestDidLoad";
    bool conversionError = false;
    QVariantHash object = aResult.toHash();
    QString sessionKey = object.value("session_key").toString();
    QString sessionSecret = object.value("secret").toString();

    QVariant ex = object.value("expires");
    uint expires = object.value("expires").toUInt(&conversionError);
    QDateTime expiration; expiration.setTime_t( expires );

    iSession->beginSession(sessionKey, sessionSecret,expiration);
    iSession->resume();

    dismissWithSuccess(true, true);
}

void FBLoginDialog::requestFailedWithFacebookError (const FBError& aCode )
{
	qDebug()<<"Inside FBLoginDialog::requestFailedWithFacebookError()";
    dismissWithError(aCode, true);
}

void FBLoginDialog::requestFailedWithNetworkError( QNetworkReply::NetworkError aCode )
{
	qDebug()<<"Inside FBLoginDialog::requestFailedWithNetworkError()";
    dismissWithError(aCode, true);
}
