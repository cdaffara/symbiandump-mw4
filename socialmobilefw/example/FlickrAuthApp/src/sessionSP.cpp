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
* class to maintian session & all credential keys.
*/

#include "sessionSP.h"
#include "requestSP.h"
#include <qfile.h>
#include <QTimer>
#include <qdebug.h>

#include "keys.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// global

static const QString kAPIRestURL = "http://api.flickr.com/services/rest/";

static const int kMaxBurstRequests = 3;
static const int kBurstDuration = 2;

static FBSession* sharedSession = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Static class functions
FBSession* FBSession::session()
{
    return sharedSession;
}

void FBSession::setSession(FBSession* aSession)
{
    sharedSession = aSession;
}


FBSession* FBSession::sessionForApplication ( const QString& aAppKey, const QString& aAppSecret, const QString& aSessionProxy)
{
    FBSession* session = new FBSession ( aAppKey, aAppSecret, aSessionProxy );
    return session;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// instance public functions
FBSession::FBSession( const QString& aAppKey, const QString& aAppSecret, const QString& aGetSessionProxy ) :
        iApiKey (aAppKey),
        iApiSecret ( aAppSecret ),
        m_Client(new SmfCredMgrClient(this))	
{
    if (!sharedSession)
    {
        sharedSession = this;
    }
}

FBSession::~FBSession()
{
	if(m_Client){
		delete m_Client;
		m_Client = NULL;
	}
}

const QString& FBSession::apiURL() const
{
    return kAPIRestURL;
}
void FBSession::SaveFrobKey (const QString& frob )
{
	iFrobKey = frob;
    //save();
}
void FBSession::Savetoken (const QString& token )
{
	qDebug()<<"Inside FBSession::Savetoken";
	qDebug()<<"token argument = "<<token;
	iToken = token;
    save();
    emit sessionDidLogin(token);
}
bool FBSession::resume()
{
    QString CMRegToken = iSettings.value("CMFlickrRegToken", "NA" ).toString();
	QDateTime ExpiryTime = iSettings.value("FlckrExpiryTime","NA").toDateTime();
	
	qDebug() << "CMRegToken = :" << CMRegToken;
	qDebug() << "ExpiryTime = :" << ExpiryTime;
	
	SmfAuthParams Params;
	if(m_Client->AuthDataSet(CMRegToken,ExpiryTime,Params))
	{
		QByteArray accessToken = Params.value("finalToken");
       emit sessionDidLogin( accessToken );
       return true;
    }
    return false;
}
void FBSession::logout() {

        iToken.clear();
        iFrobKey.clear();

        unsave();

        emit sessionDidLogout();
}

void FBSession::send (FBRequest* aRequest) {
	qDebug()<<"Inside FBSession::send";
    performRequest (aRequest, true);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// instance private functions
void FBSession::save()
{
	qDebug()<<"Inside FBSession::save()";
	qDebug()<<"Auth token = "<<iToken;
   /* if (iFrobKey.length()) {
        iSettings.setValue("FrobKey", iFrobKey);
    } else {
        iSettings.remove("FrobKey");
    }
    if (iToken.length()) {
            iSettings.setValue("FullToken", iToken);
        } else {
            iSettings.remove("FullToken");
        }
    
    iSettings.sync();*/


    SmfAuthParams Params;
    Params.insert("ApiKey",kApiKey.toAscii());
    Params.insert("ApiSecret",kApiSecret.toAscii());
    Params.insert("AuthToken",iToken.toAscii());
    
    QList<QUrl> UrlList;
    UrlList.append(QUrl("http://api.flickr.com"));
    
    QStringList PluginList;
    PluginList.append(QString("flickrcontactfetcherplugin.qtplugin"));
    PluginList.append(QString("flickrgalleryplugin.qtplugin"));
    
    QString UID("0xE1D8C7D7");
    
    //Currently Hardcoded with current time bcoz CM is not handling expiry time as '0' value
    QDateTime ExpirationDate = QDateTime::currentDateTime();
    ExpirationDate.addYears(1);
    
    QString CMRegToken = m_Client->StoreAuthData(Params,ExpirationDate,UrlList,PluginList,UID,true);
    
    qDebug()<<"Reg token from CSM = "<<CMRegToken;
    qDebug()<<"Reg token size = "<<CMRegToken.count();
    qDebug()<<"Exp date from CSM = "<<ExpirationDate;
    
    iSettings.remove("CMFlickrRegToken");
    iSettings.remove("FlckrExpiryTime");
    
    if(CMRegToken.size())
   	{
    	iSettings.setValue("CMFlickrRegToken", CMRegToken);
    	iSettings.setValue("FlckrExpiryTime", ExpirationDate);
    }
}

void FBSession::unsave()
{

}
bool FBSession::performRequest(FBRequest* aRequest, bool aEnqueue) {
    // Stagger requests that happen in short bursts to prevent the server from rejecting
    // them for making too many requests in a short time
	qDebug()<<"Inside FBSession::performRequest";
        aRequest->connect();
        return true;
}
