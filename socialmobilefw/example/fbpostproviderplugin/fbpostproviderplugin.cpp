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
 * Nalina Hariharan
 * 
 * Description:
 * The Plugin that manages post operations on the logged in user's facebook account
 *
 */

// Include files
#include <QtPlugin>
#include <QCryptographicHash>
#include <QTextFormat>
#include <QFile>
#include <QVariant>
#include <QListIterator>
#include <QDebug>
#include <QSettings>
#include <smfpluginutil.h>
#ifdef SMF_XMLPARSING
#include <QXmlStreamReader>
#endif

#include "fbpostproviderplugin.h"

static int chance = 0;
QString uid;
#ifdef SMF_XMLPARSING
SmfPost mypost;
SmfContact contact;
QString currentId;
#endif
QMap<QString, QString> idNameMap;
QMap<QString, QString> idPicMap;


// Todo:- Macro added for limiting items fetched to recent 5
// Remove after demo
#define SETLIMITOFFIVEFORSMFDEMO 1

/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString FBPostProviderPlugin::generateSignature(const QString aBaseString)
	{
	qDebug()<<"Inside FBPostProviderPlugin::generateSignature()";
	
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}


/**
 * Method to interpret the key sets obtained from credential manager 
 * @param aApiKey [out] The api key
 * @param aApiSecret [out] The api secret
 * @param aSessionKey [out] The session key
 * @param aSessionSecret [out] The session secret
 */
void FBPostProviderPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aSessionKey, 
		QString &aSessionSecret )
	{
	qDebug()<<"Inside FBPostProviderPlugin::fetchKeys()";

	qDebug()<<"Reg Token = "<<m_provider->m_smfRegToken;
	qDebug()<<"Expiry Date as int = "<<m_provider->m_validity.toTime_t();
	
	SmfAuthParams keys;
	SmfPluginUtil util;
	util.getAuthKeys(keys, m_provider->m_smfRegToken, 
			m_provider->m_validity, m_provider->m_pluginId);
	
	qDebug()<<"Number of key-value pairs = "<<keys.count();
	
    QByteArray keyName;
    keyName.append("ApiKey");
	aApiKey.append(keys.value(keyName));
	
    keyName.clear();
    keyName.append("ApiSecret");
	aApiSecret.append(keys.value(keyName));
	
	keyName.clear();
    keyName.append("SessionKey");
	aSessionKey.append(keys.value(keyName));
	
	keyName.clear();
    keyName.append("SessionSecret");
	aSessionSecret.append(keys.value(keyName));
	
	qDebug()<<"Api Key = "<<aApiKey;
	qDebug()<<"Api Secret = "<<aApiSecret;
	qDebug()<<"session Key = "<<aSessionKey;
	qDebug()<<"session Secret = "<<aSessionSecret;
	}


/**
 * Destructor
 */
FBPostProviderPlugin::~FBPostProviderPlugin( )
	{
	if(m_provider)
		delete m_provider;
	}

/**
 * Method that returns maximum no of chars (unicode) that service 
 * provider can post without truncation. Negative value means 
 * no limit
 * @return Max characters that can be posted without truncation
 */
qint32 FBPostProviderPlugin::maxCharsInPost( ) const
	{
	qDebug()<<"Inside FBPostProviderPlugin::maxCharsInPost()";
	qint32 maxCharsInPost = 256;
	return maxCharsInPost;
	}
/**
 * Method that returns maximum no of items that can be returned 
 * in a single query to getPosts. Negative value means feature 
 * not supported.
 * @return Max items that can be returned in a single query
 */
qint32 FBPostProviderPlugin::maxItems( ) const
	{
	qDebug()<<"Inside FBPostProviderPlugin::maxItems()";
	qint32 maxItems = 10;
	return maxItems;
	}

/**
 * <Method that returns all the formatting of posts that this 
 * service provider supports. May return 0 items to mean 
 * only QString is supported.
 * @return Supported formats of posts
 */
QVector<QTextFormat> FBPostProviderPlugin::supportedFormats ( ) const
	{
	qDebug()<<"Inside FBPostProviderPlugin::supportedFormats()";
	QTextFormat f1, f2;
	QVector<QTextFormat> data;
	data.append(f1);
	data.append(f2);
	return data;
	}

/**
 * Method that returns whether this SP supports Appearence
 * @return Returns true if Appearance is supported, else false. 
 * @see SmfAppearenceInfo
 */
bool FBPostProviderPlugin::supportsAppearence ( ) const
	{
	qDebug()<<"Inside FBPostProviderPlugin::supportsAppearence()";
	return true;
	}

/**
 * Method to get the latest posts
 * @param aRequest [out] The request data to be sent to network
 * @param aUser The user's contact in this SP, omit for self contact
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::retrieve( SmfPluginRequestData &aRequest,
		const SmfContact *aUser,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	qDebug()<<"Inside FBPostProviderPlugin::retrieve()";
	
	if(0 != aUser->value("Guid").value<QContactGuid>().guid().size())
		chance = 1;
	
	//// Currently considering for self contact , ie, omitting aUser
	if(0 == chance)
		return getFacebookUserId(aRequest);
	else
		return getPosts(aRequest, aUser, aPageNum , aItemsPerPage);
	}


/**
 * Method to get the user's facebook ID
 * @param aRequest [out] The request data to be sent to network
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::getFacebookUserId( 
		SmfPluginRequestData &aRequest )
	{
	qDebug()<<"Inside FBPostProviderPlugin::getFacebookUserId()";
	
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString sessionKey;
	QString sessionSecret;
	fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
	
	// Get the current date and time and convert it to sec as a string
	QString call_id = QString::number(QDateTime::currentDateTime().toTime_t(), 10);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key="+apiKey);
	baseString.append("call_id="+call_id);
#ifdef SMF_XMLPARSING
	baseString.append("format=XML");
#else
	baseString.append("format=JSON");
#endif
	baseString.append("method=users.getLoggedInUser");
	baseString.append("session_key="+sessionKey);
	baseString.append("ss=1");
	baseString.append("v=1.0");
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://api.facebook.com/restserver.php?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("call_id", call_id);
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "XML");
#else
	url.addQueryItem("format", "JSON");
#endif
	url.addQueryItem("method", "users.getLoggedInUser");
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("ss", "1");
	url.addQueryItem("v", "1.0");	
	url.addQueryItem("sig", generateSignature(baseString));
			
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactRetrievePosts;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get the user's posts
 * @param aRequest [out] The request data to be sent to network
 * @param aUser The user's contact in this SP, omit for self contact
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::getPosts( 
		SmfPluginRequestData &aRequest,
		const SmfContact *aUser, 
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	qDebug()<<"Inside FBPostProviderPlugin::getPosts()";
	qDebug()<<"Required guid = "<<aUser->value("Guid").value<QContactGuid>().guid();
	qDebug()<<"aPageNum = "<<aPageNum;
	qDebug()<<"aItemsPerPage = "<<aItemsPerPage;
	
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString sessionKey;
	QString sessionSecret;
	fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
	
	// Get the current date and time and convert it to sec as a string
	QString call_id = QString::number(QDateTime::currentDateTime().toTime_t(), 10);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key="+apiKey);
	baseString.append("call_id="+call_id);
#ifdef SMF_XMLPARSING
	baseString.append("format=XML");
#else
	baseString.append("format=JSON");
#endif
	
#ifdef SETLIMITOFFIVEFORSMFDEMO  // Hard coding the number of posts fetched as recent 5
	qDebug()<<"Hardcoding pagenumber and itemperpage to fetch 5 recent posts";
	int val = 5;
	baseString.append("limit="+QString::number(val));
#else
	if(0 == aPageNum)
		baseString.append("limit="+QString::number((aItemsPerPage*(aPageNum+1)), 10));
	else
		baseString.append("limit="+QString::number((aItemsPerPage*aPageNum), 10));
#endif

	baseString.append("method=stream.get");
	baseString.append("session_key="+sessionKey);
	if(0 != aUser->value("Guid").value<QContactGuid>().guid().size())
		baseString.append("source_ids="+aUser->value("Guid").value<QContactGuid>().guid());
	else
	baseString.append("source_ids="+uid);
	baseString.append("ss=1");
	baseString.append("v=1.0");
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://api.facebook.com/restserver.php?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("call_id", call_id);
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "XML");
#else
	url.addQueryItem("format", "JSON");
#endif
		
#ifdef SETLIMITOFFIVEFORSMFDEMO  // Hard coding the number of posts fetched as recent 5
	qDebug()<<"Hardcoding pagenumber and itemperpage to fetch 5 recent posts";
	val = 5;
	url.addQueryItem("limit", QString::number(val));
#else
	if(0 == aPageNum)
		url.addQueryItem("limit", QString::number((aItemsPerPage*(aPageNum+1)), 10));
	else
		url.addQueryItem("limit", QString::number((aItemsPerPage*aPageNum), 10));
#endif

	url.addQueryItem("method", "stream.get");
	url.addQueryItem("session_key", sessionKey);
	if(0 != aUser->value("Guid").value<QContactGuid>().guid().size())
		url.addQueryItem("source_ids",aUser->value("Guid").value<QContactGuid>().guid());
	else
	url.addQueryItem("source_ids",uid);
	url.addQueryItem("ss", "1");
	url.addQueryItem("v", "1.0");	
	url.addQueryItem("sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactRetrievePosts;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to update a post to own area.
 * @param aRequest [out] The request data to be sent to network
 * @param aPostData The post data to be posted
 * @param aLocation The location
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::post( SmfPluginRequestData &aRequest,
		const SmfPost &aPostData, 
		const SmfLocation &aLocation )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aPostData)
	Q_UNUSED(aLocation)
	qDebug()<<"Inside FBPostProviderPlugin::post()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error; 
	}

/**
 * Method to update the last post to own area with new data
 * @param aRequest [out] The request data to be sent to network
 * @param aPostData The edited/new data to be posted
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::updatePost( SmfPluginRequestData &aRequest,
		const SmfPost &aPostData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aPostData)
	qDebug()<<"Inside FBPostProviderPlugin::updatePost()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error; 
	}

/**
 * Method to update a post to a particular contact
 * @param aRequest [out] The request data to be sent to network
 * @param aPostData The post data to be posted
 * @param aContact The contact where the data has to be posted
 * @param aLocation The location
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::postDirected( SmfPluginRequestData &aRequest,
		const SmfPost &aPostData, 
		const SmfContact &aContact, 
		const SmfLocation *aLocation  )
	{
	Q_UNUSED(aLocation)
	qDebug()<<"Inside FBPostProviderPlugin::postDirected()";
	
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	// invalid arguments
	qDebug()<<"To be Post data = "<<aPostData.description();
	qDebug()<<"To be posted Contact id = "<<aContact.value("Guid").value<QContactGuid>().guid();
	
	if( (0 == aPostData.description().size()) || (0 == aContact.value("Guid").value<QContactGuid>().guid().size()) )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
			
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString sessionKey;
	QString sessionSecret;
	fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
			
	// Get the current date and time and convert it to sec as a string
	QString call_id = QString::number(QDateTime::currentDateTime().toTime_t(), 10);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key="+apiKey);
	baseString.append("call_id="+call_id);
#ifdef SMF_XMLPARSING
	baseString.append("format=XML");
#else
	baseString.append("format=JSON");
#endif
	baseString.append("message="+aPostData.description());
	baseString.append("method=stream.publish");
	baseString.append("session_key="+sessionKey);
	baseString.append("ss=1");
	baseString.append("target_id="+aContact.value("Guid").value<QContactGuid>().guid());
	baseString.append("v=1.0");
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://api.facebook.com/restserver.php?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("call_id", call_id);
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "XML");
#else
	url.addQueryItem("format", "JSON");
#endif
	url.addQueryItem("message", aPostData.description());
	url.addQueryItem("method", "stream.publish");
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("ss", "1");
	url.addQueryItem("target_id", aContact.value("Guid").value<QContactGuid>().guid());
	url.addQueryItem("v", "1.0");	
	url.addQueryItem("sig", generateSignature(baseString));
			
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactPostDirected;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}


/**
 * Method to post a comment on a post.
 * @param aRequest [out] The request data to be sent to network
 * @param aTarget Post on which comment has to be posted
 * @param aComment comment to be posted
 * @param aLocation location data
 */
SmfPluginError FBPostProviderPlugin::commentOnAPost(SmfPluginRequestData &aRequest,
		const SmfPost &aTarget,
		const SmfPost &aComment, 
		const SmfLocation *aLocation )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aTarget)
	Q_UNUSED(aComment)
	Q_UNUSED(aLocation)
	qDebug()<<"Inside FBPostProviderPlugin::commentOnAPost()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error; 
	}

/**
 * Method to update the presence information of the user
 * @param aRequest [out] The request data to be sent to network
 * @param aAppearence The appearence information
 * @param aStatus The status string
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::postAppearence( SmfPluginRequestData &aRequest,  
		const SmfPresenceInfo &aAppearence,
		const QString &aStatus )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aAppearence)
	Q_UNUSED(aStatus)
	qDebug()<<"Inside FBPostProviderPlugin::postAppearence()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error; 
	}

/**
 * Share a contact's post to user's friends and followers
 * (e.g. retweet in twitter, share on facebook)
 * @param aRequest [out] The request data to be sent to network
 * @param aPostData data to be posted
 * @param aContact contact to which the post belonged
 * @param aEdited whether user changed items within the post
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::sharePost( SmfPluginRequestData &aRequest, 
		const SmfPost &aPostData, 
		const SmfContact &aContact, 
		const bool &aEdited)
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aPostData)
	Q_UNUSED(aContact)
	Q_UNUSED(aEdited)
	qDebug()<<"Inside FBPostProviderPlugin::sharePost()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error; 
	}

/**
 * Customised method for SmfPostProviderPlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBPostProviderPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, 
		QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	qDebug()<<"Inside FBPostProviderPlugin::customRequest()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error; 
	}


/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void FBPostProviderPlugin::initialize( )
	{
	// Create an instance of FlickrProviderBase
	m_provider = new FBProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* FBPostProviderPlugin::getProviderInfo( )
	{
	return m_provider;
	}

/**
 * Method to get the result for a network request.
 * @param aOperation The type of operation to be requested
 * @param aTransportResult The result of transport operation
 * @param aResponse The QByteArray instance containing the network response.
 * The plugins should delete this instance once they have read the 
 * data from it.
 * @param aResult [out] An output parameter to the plugin manager.If the 
 * return value is SmfSendRequestAgain, QVariant will be of type 
 * SmfPluginRequestData.
 * For SmfPostProviderPlugin: If last operation was retrieve(), aResult will be 
 * of type QList<SmfPost>. If last operation was post() or updatePost() or 
 * postDirected() or commentOnAPost() or postAppearence() or sharePost(), 
 * aResult will be of type bool
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError FBPostProviderPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	qDebug()<<"Inside FBPostProviderPlugin::responseAvailable()";
	Q_UNUSED(aPageResult)
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	QFile respFile("c://data//SmfPluginFBPostResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		{
		qDebug()<<"File to write the response could not be opened, so writing to this file";
		qDebug()<<"Flickr response = "<<QString(response);
		}
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfPluginFBPostResponse.txt'";
		}
	
	qDebug()<<"FB response size = "<<response.size();
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";

		if(SmfContactRetrievePosts == aOperation)
			{
			qDebug()<<"Response for retrieving posts";
			if(0 == chance)
				{
				chance = 1;
				qDebug()<<"For Getting Current logged in User";
				
				QString errStr;
				errStr.clear();
				uid.clear();
#ifdef SMF_XMLPARSING
				qDebug()<<"Xml parsing";
				// For getting contacts from xml response
				QXmlStreamReader xml(response);
				while (!xml.atEnd())
					{
					xml.readNext();
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
						if (xml.name() == "users_getLoggedInUser_response")
							{
							qDebug()<<"users_getLoggedInUser_response tag found";
							QString message(xml.readElementText());
							uid.append(message);
							qDebug()<<"current logged in uid  = "<<uid;
							}
						else if("error_msg" == xml.name())
							{
							qDebug()<<"error_msg tag found";
							errStr.append(xml.readElementText());
							break;
							}
						}
					}
#else
				qDebug()<<"Json parsing";
				if(response.contains(QByteArray("error_msg")))
					{
					qDebug()<<"Response contains error, so parse and get the error code";
					
					bool ok;
					SmfPluginUtil util;
					QVariant result = util.parse(response, &ok);
					if (!ok) 
						{
						qDebug()<<"An error occurred during json parsing, error = "<<util.errorString();
						aRetType = SmfRequestError;
						return SmfPluginErrParsingFailed;
						}
					else
						{
						QVariantMap map = result.toMap();
						errStr.append(map["error_msg"].toString());
						}
					}
				else
					{
					qDebug()<<"Response is ok, so don't parse";
					uid.append(response);
					}
#endif
				
				if(errStr.size())
					{
					qDebug()<<"Response error found = "<<errStr;
					error = SmfPluginErrInvalidRequest;
					aRetType = SmfRequestError;
					aResult->setValue(errStr);
					}
				else
					{
					qDebug()<<"current logged in uid  = "<<uid;
					aRetType = SmfSendRequestAgain;
					error = SmfPluginErrNone;
					}
				}
			else
				{
				qDebug()<<"For Fetching posts";
			
				QList<SmfPost> list;
				QString errStr;
				errStr.clear();
				
#ifdef SMF_XMLPARSING
				qDebug()<<"Xml parsing";
				
				// For getting contacts from xml response
				QXmlStreamReader xml(response);
				while (!xml.atEnd())
					{
					xml.readNext();
					
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
						if (xml.name() == "post_id")
							{
							QString message(xml.readElementText());
							mypost.setId(message);
							qDebug()<<"post post_id = "<<mypost.id();
							}
						else if (xml.name() == "actor_id")
							{
							QString message(xml.readElementText());
	
							QContactGuid guid;
							guid.setGuid(message);
							QVariant varGuid = QVariant::fromValue(guid);
							contact.setValue("Guid", varGuid);
							mypost.setOwner(contact);
							qDebug()<<"post actor_id = "<<contact.value("Guid").value<QContactGuid>().guid();
							}
						else if (xml.name() == "message")
							{
							QString message(xml.readElementText());
							mypost.setDescription(message);
							qDebug()<<"post message = "<<mypost.description();
							}
						else if (xml.name() == "permalink")
							{
							QString message(xml.readElementText());
							QUrl url(message);
							mypost.setUrl(url);
							qDebug()<<"post url = "<<mypost.url().toString();
							}
						else if(xml.name() == "id")
							{
							currentId.clear();
							currentId.append(xml.readElementText());
							qDebug()<<"post owners id = "<<currentId;
							}
						else if(xml.name() == "name")
							{
							QString name = xml.readElementText();
							idNameMap.insert(currentId, name);
							currentId.clear();
							qDebug()<<"post owners name = "<<name;
							}
						else if("error_msg" == xml.name())
							{
							qDebug()<<"error_msg tag found";
							errStr.append(xml.readElementText());
							break;
							}
						}
					if (xml.tokenType() == QXmlStreamReader::EndElement)
						{
						if (xml.name() == "stream_post")
							{
							list.append(mypost);
							}
						}
					}
				// Loop for setting the "Name" of sctor_id, currently only "Guid" is there 
				// in the post.owner member
				if(0 == errStr.size())
					{
					for (int i=0 ; i <list.count() ; i++)
						{
						QContactGuid guid;
						guid.setGuid(list[i].owner().value("Guid").value<QContactGuid>().guid());
						QVariant guidVar = QVariant::fromValue(guid);
						contact.setValue("Guid", guidVar);
						
						QContactName contactName;
						contactName.setFirstName(idNameMap.value(list[i].owner().value("Guid").value<QContactGuid>().guid()));
						QVariant nameVar = QVariant::fromValue(contactName);
						contact.setValue("Name", nameVar);
						
						list[i].setOwner(contact);
						}
					}
#else
				qDebug()<<"Json parsing";
				
				bool ok;
				SmfPluginUtil util;
				QVariantMap result = util.parse(response, &ok).toMap();
				if (!ok) 
					{
					qDebug()<<"An error occurred during json parsing";
					aRetType = SmfRequestError;
					return SmfPluginErrParsingFailed;
					}
				
				chance = 0;
				
				if(response.contains(QByteArray("error_msg")))
					{
					errStr.append(result["error_msg"].toString());
					}
				else
					{
					QList<QVariant> list1 = result["posts"].toList();
					QListIterator<QVariant> iter(list1);
					while(iter.hasNext())
						{
						SmfPost post;
	
						QVariantMap map2 = iter.next().toMap();
						qDebug()<<"post_id = "<<map2["post_id"].toString();
						qDebug()<<"viewer_id = "<<map2["viewer_id"].toString();
						qDebug()<<"message = "<<map2["message"].toString();
						qDebug()<<"updated_time = "<<map2["updated_time"].toString();
						qDebug()<<"created_time = "<<map2["created_time"].toString();
						qDebug()<<"permalink = "<<map2["permalink"].toString();
						
						// Set the posts's description
						post.setDescription(map2["message"].toString());
						
						// Set the posts's link
						QUrl url(map2["permalink"].toString());
						post.setUrl(url);
						
						// Set the posts's Id
						post.setId(map2["post_id"].toString());
						
						SmfContact contact;
						QContactGuid guid;
						guid.setGuid(map2["actor_id"].toString());
						QVariant varGuid = QVariant::fromValue(guid);
						contact.setValue("Guid", varGuid);
						
						// Set the posts's owner details
						post.setOwner(contact);
						
						list.append(post);
						}
					
					// Loop for getting the "Name" of actor_id
					QList<QVariant> list2 = result["profiles"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map2 = iter2.next().toMap();
						qDebug()<<"owner's id = "<<map2["id"].toString();
						qDebug()<<"owner's name = "<<map2["name"].toString();
						qDebug()<<"owner's profile image url = "<<map2["pic_square"].toString();
						idNameMap.insert(map2["id"].toString(), map2["name"].toString());
						idPicMap.insert(map2["id"].toString(), map2["pic_square"].toString());
						}
					
					// Loop for setting the "Name" of actor_id
					for (int i=0 ; i <list.count() ; i++)
						{
						SmfContact contact = list[i].owner();
						
						QContactName contactName;
						contactName.setFirstName(idNameMap.value(contact.value("Guid").value<QContactGuid>().guid()));
						qDebug()<<"Name = "<<contactName.firstName();
						QVariant nameVar = QVariant::fromValue(contactName);
						contact.setValue("Name", nameVar);
						
						QContactAvatar avatar;
						QUrl url(idPicMap.value(contact.value("Guid").value<QContactGuid>().guid()));
						avatar.setImageUrl(url);
						qDebug()<<"Profile Image Url = "<<url.toString();
						QVariant urlVar = QVariant::fromValue<QContactAvatar>(avatar);
						contact.setValue("Avatar", urlVar);
						
						list[i].setOwner(contact);
						}
					}
#endif
				if(errStr.size())
					{
					qDebug()<<"Response error found = "<<errStr;
					error = SmfPluginErrInvalidRequest;
					aRetType = SmfRequestError;
					aResult->setValue(errStr);
					}
				else
					{
					qDebug()<<"list count = "<<list.count();
					aResult->setValue(list);
					aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;
					}
				}
			}
		else if(SmfContactPostDirected == aOperation)
			{
			qDebug()<<"Response for post directed";
					
			bool posted = false;
			
#ifdef SMF_XMLPARSING
				// For getting contacts from xml response
				QXmlStreamReader xml(response);
				while (!xml.atEnd())
					{
					xml.readNext();
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
						if (xml.name() == "stream_publish_response")
							{
							qDebug()<<"stream_publish_response tag found";
							QString message(xml.readElementText());
							qDebug()<<"response content is = "<<message;
							qDebug()<<"response size is = "<<message.size();
							if(message.size())
								posted = true;
							else
								posted = false;
							}
						}
					}
#else
				bool ok;
				SmfPluginUtil util;
				QVariantMap result = util.parse(response, &ok).toMap();
				if (!ok) 
					{
					qDebug()<<"Response cannot be parsed";
					posted = true;
					aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;
					}
				else
					{
					qDebug()<<"Error!!!";
					aRetType = SmfRequestError;
					error =  SmfPluginErrInvalidRequest;
					}
#endif
				
			qDebug()<<"post posted???  = "<<posted;
			aResult->setValue(posted);
			}
		else
			{
			qDebug()<<"Service unsupported, currently only SmfContactRetrievePosts and SmfContactPostDirected!!!";
			aRetType = SmfRequestError;
			error = SmfPluginErrServiceNotSupported;
			}
		}

	else if(SmfTransportOpOperationCanceledError == aTransportResult)
		{
		qDebug()<<"Operation Cancelled !!!";
		error = SmfPluginErrCancelComplete;
		aRetType = SmfRequestComplete;
		}

	else
		{
		qDebug()<<"Transport Error !!!";
		error = SmfPluginErrNetworkError;
		aRetType = SmfRequestError;
		}
	
	return error;
	}


/**
 * Destructor
 */
FBProviderBase::~FBProviderBase( )
	{
	}

/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString FBProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}

/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage FBProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}

/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString FBProviderBase::description( ) const
	{
	return m_description;
	}

/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl FBProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}

/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl FBProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}

/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage FBProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> FBProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList FBProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString FBProviderBase::pluginId( ) const
	{
	return m_pluginId;
	}

/**
 * Method to get the ID of the authentication application 
 * for this service
 * @param aProgram The authentication application name
 * @param aArguments List of arguments required for authentication app
 * @param aMode Strting mode for authentication application
 * @return The ID of the authentication application 
 */
QString FBProviderBase::authenticationApp( QString &aProgram, 
		QStringList & aArguments, 
		QIODevice::OpenModeFlag aMode ) const
	{
	Q_UNUSED(aProgram)
	Q_UNUSED(aArguments)
	Q_UNUSED(aMode)
	return m_authAppId;
	}

/**
 * Method to get the unique registration ID provided by the 
 * Smf for authorised plugins
 * @return The unique registration ID/token provided by the Smf for 
 * authorised plugins
 */
QString FBProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}

/**
 * Method that initializes this class. This method should be called 
 * from the initialize() method of the FBPostProviderPlugin class
 */
void FBProviderBase::initialize()
	{
	m_serviceName = "Facebook";
	m_description = "Facebook post plugin description";
	m_serviceUrl = QUrl(QString("http://api.facebook.com"));
	m_pluginId = "fbpostproviderplugin.qtplugin";
	m_authAppId = "0xEFE2FD23";
	m_supportedInterfaces.append("org.symbian.smf.plugin.contact.posts/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("FBCMRegToken").toString();
	m_validity = iSettings.value("FBExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : fbpostproviderplugin
 * plugin class : FBPostProviderPlugin
 */
Q_EXPORT_PLUGIN2( fbpostproviderplugin, FBPostProviderPlugin )

