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
 * The Plugin that fetches activities from the logged in user's facebook account
 *
 */

// Include files
#include <QtPlugin>
#include <QDebug>
#include <QCryptographicHash>
#include <QFile>
#include <QMap>
#include <QXmlStreamReader>
#include <QSettings>
#include <smfactions.h>
#include <smfpluginutil.h>

#include "fbactivityfetcherplugin.h"

// Global and static global variables
static int chance = 0;
QString currentUserId;
SmfActivityEntry myEntry;
SmfContact currentAuthor;
QList<SmfActivityObject> currentObjectList;
SmfActivityObject currentObject;
bool forEntryXmlTag;
bool forActivityObjectXmlTag;

/**
 * Destructor
 */
FBActivityFetcherPlugin::~FBActivityFetcherPlugin( )
	{
	if(m_provider)
		delete m_provider;
	}


/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString FBActivityFetcherPlugin::generateSignature(const QString aBaseString)
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::generateSignature()";
	
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
 * @param aAppId [out] The application ID
 */
void FBActivityFetcherPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aSessionKey, 
		QString &aSessionSecret, 
		QString &aAppId)
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::fetchKeys()";

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
	
	keyName.clear();
    keyName.append("AppId");
	aAppId.append(keys.value(keyName));
	
	qDebug()<<"Api Key = "<<aApiKey;
	qDebug()<<"Api Secret = "<<aApiSecret;
	qDebug()<<"session Key = "<<aSessionKey;
	qDebug()<<"session Secret = "<<aSessionSecret;
	qDebug()<<"App Id Secret = "<<aAppId;
	}


/**
 * Method to get the list of self activities, e.g. shown in own wall
 * @param aRequest [out] The request data that plugin generates (to be sent to network)
 * @param aPageNum[in] The page to be extracted
 * @param aItemsPerPage[in] Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBActivityFetcherPlugin::selfActivities( SmfPluginRequestData &aRequest,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::selfActivities()";
		
	//// Currently considering for self contact , ie, omitting aUser
	if(0 == chance)
		return getFacebookUserId(aRequest);
	else
		return getSelfActivities(aRequest, aPageNum, aItemsPerPage);
	}


/**
 * Method to get the user's facebook ID
 * @param aRequest [out] The request data to be sent to network
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBActivityFetcherPlugin::getFacebookUserId( 
		SmfPluginRequestData &aRequest )
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::getFacebookUserId()";
	
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString sessionKey;
	QString sessionSecret;
	QString appId;
	fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret, appId);
	
	// Get the current date and time and convert it to sec as a string
	QString call_id = QString::number(QDateTime::currentDateTime().toTime_t(), 10);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key="+apiKey);
	baseString.append("call_id="+call_id);
	baseString.append("format=JSON");
	baseString.append("method=users.getLoggedInUser");
	baseString.append("session_key="+sessionKey);
	baseString.append("ss=1");
	baseString.append("v=1.0");
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://api.facebook.com/restserver.php?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("call_id", call_id);
	url.addQueryItem("format", "JSON");
	url.addQueryItem("method", "users.getLoggedInUser");
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("ss", "1");
	url.addQueryItem("v", "1.0");	
	url.addQueryItem("sig", generateSignature(baseString));
			
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfActivitySelfActivity;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get the list of self activities, e.g. shown in own wall
 * @param aRequest [out] The request data that plugin generates (to be sent to network)
 * @param aPageNum[in] The page to be extracted
 * @param aItemsPerPage[in] Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBActivityFetcherPlugin::getSelfActivities( SmfPluginRequestData &aRequest,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::getSelfActivities()";
		
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
	QString applicationId;
	fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret, applicationId);
	
	// Create the API signature string
	QString baseString;
	baseString.append("app_id="+applicationId);
	baseString.append("session_key="+sessionKey);
	baseString.append("source_id="+currentUserId);
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://www.facebook.com/activitystreams/feed.php?");
	url.addQueryItem("app_id", applicationId);
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("source_id", currentUserId);
	url.addQueryItem("sig", generateSignature(baseString));
	url.addQueryItem("v", "0.7");
	QString str(url.toString());
	str.append("&read");
			
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(QUrl(str));
	aRequest.iRequestType = SmfActivitySelfActivity;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get the list of activities for other, e.g. shown in a friends wall
 * @param aRequest [out] The request data plugin generated (to be sent to network)
 * @param aContact [in] The contact containing the URI, name or id of the user 
 * @param aPageNum[in] The page to be extracted
 * @param aItemsPerPage[in] Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBActivityFetcherPlugin::friendsActivities( SmfPluginRequestData &aRequest,
		const SmfContact &aContact,			
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::friendsActivities()";
		
	SmfPluginError error = SmfPluginErrInvalidArguments;
	chance = 1;

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
	QString applicationId;
	fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret, applicationId);
	
	// ToDo :- Taking QContactGuid for the time being
	QString sourceId = aContact.value("Guid").value<QContactGuid>().guid();
	qDebug()<<"Friends facebook ID = "<<sourceId;
	
	// Create the API signature string
	QString baseString;
	baseString.append("app_id="+applicationId);
	baseString.append("session_key="+sessionKey);
	baseString.append("source_id="+sourceId);
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://www.facebook.com/activitystreams/feed.php?");
	url.addQueryItem("app_id", applicationId);
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("source_id", sourceId);
	url.addQueryItem("sig", generateSignature(baseString));
	url.addQueryItem("v", "0.7");
	QString str(url.toString());
	str.append("&read");
			
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(QUrl(str));
	aRequest.iRequestType = SmfActivityFriendsActivities;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get list of self activities filtered by activity type, e.g. only photo updates in own wall
 * @param aRequest [out] The request data plugin generated (to be sent to network)
 * @param aFilters [in] The list of activity types to be included in result
 * @param aPageNum[in] The page to be extracted
 * @param aItemsPerPage[in] Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBActivityFetcherPlugin::filtered( SmfPluginRequestData &aRequest,
		QList<SmfActivityObjectType> &aFilters,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FBActivityFetcherPlugin::filtered()";
		
	Q_UNUSED(aRequest)
	Q_UNUSED(aFilters)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	return SmfPluginErrServiceNotSupported; 
	}

/**
 * Customised method for SmfActivityFetcherPlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBActivityFetcherPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	qDebug()<<"Inside FBActivityFetcherPlugin::customRequest()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void FBActivityFetcherPlugin::initialize( )
	{
	// Set the global value
	forActivityObjectXmlTag = false;
	forEntryXmlTag = false;
	
	// Create an instance of FBActivityProviderBase
	m_provider = new FBActivityProviderBase;
	m_provider->initialize();
	}


/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* FBActivityFetcherPlugin::getProviderInfo( )
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
 * For SmfActivityFetcherPlugin: If last operation was selfActivities() or 
 * friendsActivities() or filtered(), aResult will be of type 
 * QList<SmfActivityEntry>. 
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError FBActivityFetcherPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	Q_UNUSED(aPageResult)
	qDebug()<<"Inside FBActivityFetcherPlugin::responseAvailable()";
	
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	QFile respFile("c://data//SmfPluginActivityResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		qDebug()<<"File to write the response could not be opened";
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfPluginActivityResponse.txt'";
		}
	qDebug()<<"FB response size = "<<response.size();
	
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";
		
		if((SmfActivitySelfActivity == aOperation) ||
				(SmfActivityFriendsActivities == aOperation))
			{
			qDebug()<<"Response for retrieving self/friends activities";
			if(0 == chance)
				{
				chance = 1;
				qDebug()<<"For Getting Current logged in User for self activity";
				
				QString errStr;
				errStr.clear();
				currentUserId.clear();

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
					currentUserId.append(response);
					}
				
				if(errStr.size())
					{
					qDebug()<<"Response error found = "<<errStr;
					error = SmfPluginErrInvalidRequest;
					aRetType = SmfRequestError;
					aResult->setValue(errStr);
					}
				else
					{
					qDebug()<<"current logged in uid  = "<<currentUserId;
					aRetType = SmfSendRequestAgain;
					error = SmfPluginErrNone;
					}
				}
			else // chance == 1
				{
				qDebug()<<"For Fetching self/friends activities";
				chance = 0;
		
				QList<SmfActivityEntry> list;
				QString errStr;
				errStr.clear();
			
				qDebug()<<"Xml parsing";
				
				// For getting contacts from xml response
				QXmlStreamReader xml(response);
				while (!xml.atEnd())
					{
					xml.readNext();
					
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
						if (xml.qualifiedName().toString() == "entry")
							{
							currentObjectList.clear();
							forEntryXmlTag = true;
							}
						else if (xml.qualifiedName().toString() == "title")
							{
							if(forEntryXmlTag)
								{
								SmfPost post;
								post.setTitle(xml.readElementText());
								myEntry.setTitle(post);
								qDebug()<<"entry title = "<<myEntry.title().title();
								}
							}
						else if (xml.qualifiedName().toString() == "fb:id")
							{
							if(forEntryXmlTag)
								{
								myEntry.setId(xml.readElementText());
								qDebug()<<"entry id = "<<myEntry.id();
								}
							}
						else if (xml.qualifiedName().toString() == "name") // author name
							{
							if(forEntryXmlTag)
								{
								QContactName name;
								name.setFirstName(xml.readElementText());
								QVariant var = QVariant::fromValue<QContactName>(name);
								currentAuthor.setValue("Name", var);
								}
							}
						else if(xml.qualifiedName().toString() == "uri")
							{
							if(forEntryXmlTag)
								{
								QContactUrl url;
								url.setUrl(xml.readElementText());
								QVariant var = QVariant::fromValue<QContactUrl>(url);
								currentAuthor.setValue("Url", var);
								myEntry.setAuthor(currentAuthor);
								qDebug()<<"entry author name = "<<myEntry.author().value("Name").value<QContactName>().firstName();
								//qDebug()<<"entry author uri = "<<myEntry.author().value("Url").value<QContactUrl>().url();
								}
							}
						else if(xml.qualifiedName().toString() == "activity:verb")
							{
							if(forEntryXmlTag)
								{
								QString str(xml.readElementText());
								str.remove(' ');
								str.remove('\n');
								SmfActivityVerb verb = convertActivityverb(str);
								myEntry.setActionName(verb);
								//qDebug()<<"entry actionname = "<<myEntry.actionName();
								}
							}
						else if(xml.qualifiedName().toString() == "category")
							{
							if(forEntryXmlTag)
								{
								QStringRef str = xml.attributes().value("term");
								SmfPost post;
								post.setDescription(str.toString());
								myEntry.setDetails(post);
								qDebug()<<"entry details = "<<myEntry.details().description();
								}
							}
						else if(xml.qualifiedName().toString() == "activity:object")
							{
							forActivityObjectXmlTag = true;
							}
						else if(xml.qualifiedName().toString() == "id")
							{
							if(forActivityObjectXmlTag)
								currentObject.setId(xml.readElementText());
							}
						else if(xml.qualifiedName().toString() == "caption")
							{
							if(forActivityObjectXmlTag)
								currentObject.setCaption(xml.readElementText());
							}
						else if(xml.qualifiedName().toString() == "link")
							{
							if(forActivityObjectXmlTag)
								{
								QStringRef str = xml.attributes().value("href");
								currentObject.setLink(str.toString());
								}
							}
						else if(xml.qualifiedName().toString() == "activity:object-type")
							{
							if(forActivityObjectXmlTag)
								{
								QString str(xml.readElementText());
								str.remove(' ');
								str.remove('\n');
								SmfActivityObjectType type = convertActivityObjectType(str);
								currentObject.setType(type);
								}
							}
						else if(xml.qualifiedName().toString() == "published")
							{
							if(forActivityObjectXmlTag)
								{
								QDateTime dateTime = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
								currentObject.setTime(dateTime);
								}
							}
						else if(xml.qualifiedName().toString() == "content")
							{
							if(forActivityObjectXmlTag)
								{
								currentObject.setContent(xml.readElementText());
								}
							}
						else if("error_msg" == xml.qualifiedName().toString())
							{
							qDebug()<<"error_msg tag found";
							errStr.append(xml.readElementText());
							break;
							}
						}
					if (xml.tokenType() == QXmlStreamReader::EndElement)
						{
						if (xml.qualifiedName().toString() == "entry")
							{
							forEntryXmlTag = false;
							myEntry.setTargetObj(currentObjectList[0]);
							myEntry.setActivities(currentObjectList);
							list.append(myEntry);
							if(list.count() == 5)
								break;
							}
						else if(xml.qualifiedName().toString() == "activity:object")
							{
							forActivityObjectXmlTag = false;
							
							//qDebug()<<"object id = "<<currentObject.id();
							//qDebug()<<"object caption = "<<currentObject.caption();
							//qDebug()<<"object type = "<<currentObject.type();
							//qDebug()<<"object link = "<<currentObject.link();
							//qDebug()<<"object time = "<<currentObject.time().toString(Qt::TextDate);
							//qDebug()<<"object content = "<<currentObject.content();
							
							currentObjectList.append(currentObject);
							}
						}
					}
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
		else
			{
			qDebug()<<"Service unsupported, currently only SmfActivitySelfActivity and SmfActivityFriendsActivities!!!";
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
 * Method to convert the Activity verb string obtained from facebook to the 
 * enum SmfActivityVerb
 * @param verbStr The activity verb as a string
 * @return The corresponding enum SmfActivityVerb
 */
SmfActivityVerb FBActivityFetcherPlugin::convertActivityverb( const QString& verbStr )
	{
	//qDebug()<<"verb as a string = "<<verbStr;
	
	SmfActivityVerb verb = (SmfActivityVerb)-1;
	
	if(QString("http://activitystrea.ms/schema/1.0/favorite/") == verbStr)	//0
		verb = SmfActivityMarkAsFavorite;
		
	else if(QString("http://activitystrea.ms/schema/1.0/follow/") == verbStr)	//1
		verb = SmfActivityStartFollowing;

	else if(QString("http://activitystrea.ms/schema/1.0/like/") == verbStr)	//2
		verb = SmfActivityMarkLiked;

	else if(QString("http://activitystrea.ms/schema/1.0/make-friend/") == verbStr)	//3
		verb = SmfActivityMakeFriend;

	else if(QString("http://activitystrea.ms/schema/1.0/join/") == verbStr)	//4
		verb = SmfActivityJoin;

	else if(QString("http://activitystrea.ms/schema/1.0/play/") == verbStr)	//5
		verb = SmfActivityPlay;

	else if(QString("http://activitystrea.ms/schema/1.0/post/") == verbStr)	//6
		verb = SmfActivityPost;

	else if(QString("http://activitystrea.ms/schema/1.0/save/") == verbStr)	//7
		verb = SmfActivitySave;

	else if(QString("http://activitystrea.ms/schema/1.0/share/") == verbStr)	//8
		verb = SmfActivityShare;

	else if(QString("http://activitystrea.ms/schema/1.0/tag/") == verbStr)	//9
		verb = SmfActivityTag;

	else if(QString("http://activitystrea.ms/schema/1.0/update/") == verbStr)	//10
		verb = SmfActivityUpdate;

	//qDebug()<<"returned verb enum = "<<verb;
	return verb;
	}


/**
 * Method to convert the Activity object type string obtained from 
 * facebook to the enum SmfActivityObjectType
 * @param objType The activity object type as a string
 * @return The corresponding enum SmfActivityObjectType
 */
SmfActivityObjectType FBActivityFetcherPlugin::convertActivityObjectType( const QString& objType )
	{
	//qDebug()<<"type as a string = "<<objType;
	
	SmfActivityObjectType type = (SmfActivityObjectType)-1;
	
	if(QString("http://activitystrea.ms/schema/1.0/article/") == objType)	//0
		type = SmfActivityObjTypeArticle;

	else if(QString("http://activitystrea.ms/schema/1.0/audio/") == objType)	//1
		type = SmfActivityObjTypeAudio;

	else if(QString("http://activitystrea.ms/schema/1.0/bookmark/") == objType)	//2
		type = SmfActivityObjTypeBookmark;

	else if(QString("http://activitystrea.ms/schema/1.0/comment/") == objType)	//3
		type = SmfActivityObjTypeComment;

	else if(QString("http://activitystrea.ms/schema/1.0/file/") == objType)	//4
		type = SmfActivityObjTypeFile;

	else if(QString("http://activitystrea.ms/schema/1.0/folder/") == objType)	//5
		type = SmfActivityObjTypeFolder;

	else if(QString("http://activitystrea.ms/schema/1.0/group/") == objType)	//6
		type = SmfActivityObjTypeGroup;

	else if(QString("http://activitystrea.ms/schema/1.0/list/") == objType)  // 7, ToDo :- Note sure about this string
		type = SmfActivityObjTypeList;

	else if(QString("http://activitystrea.ms/schema/1.0/note/") == objType)	//8
		type = SmfActivityObjTypeNote;

	else if(QString("http://activitystrea.ms/schema/1.0/person/") == objType)	//9
		type = SmfActivityObjTypePerson;

	else if(QString("http://activitystrea.ms/schema/1.0/photo/") == objType)	//10
		type = SmfActivityObjTypePhoto;

	else if(QString("http://activitystrea.ms/schema/1.0/photo-album/") == objType)	//11
		type = SmfActivityObjTypePhotoAlbum;

	else if(QString("http://activitystrea.ms/schema/1.0/place/") == objType)	//12
		type = SmfActivityObjTypePlace;

	else if(QString("http://activitystrea.ms/schema/1.0/playlist/") == objType)	//13
		type = SmfActivityObjTypePlaylist;

	else if(QString("http://activitystrea.ms/schema/1.0/product/") == objType)	//14
		type = SmfActivityObjTypeProduct;

	else if(QString("http://activitystrea.ms/schema/1.0/review/") == objType)	//15
		type = SmfActivityObjTypeReview;

	else if(QString("http://activitystrea.ms/schema/1.0/service/") == objType)	//16
		type = SmfActivityObjTypeService;

	else if(QString("http://activitystrea.ms/schema/1.0/status/") == objType)	//17
		type = SmfActivityObjTypeStatus;

	else if(QString("http://activitystrea.ms/schema/1.0/video/") == objType)	//18
		type = SmfActivityObjTypeVideo;

	else if(QString("http://activitystrea.ms/schema/1.0/song/") == objType)	//19
		type = SmfActivityObjTypeMusic;

	else if(QString("http://activitystrea.ms/schema/1.0/event/") == objType)	//20
		type = SmfActivityObjTypeEvent;

	// ToDo :- No strings found for 'SmfActivityObjTypeAdvanced" in 
	// http://activitystrea.ms/schema/1.0/activity-schema-01.html
	// So setting it as default when no standard string is found
	else
		type = SmfActivityObjTypeAdvanced;									//21
			
	//qDebug()<<"returned type enum = "<<type;
	return type;
	}

/**
 * Destructor
 */
FBActivityProviderBase::~FBActivityProviderBase( )
	{
	}


/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString FBActivityProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}


/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage FBActivityProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}


/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString FBActivityProviderBase::description( ) const
	{
	return m_description;
	}


/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl FBActivityProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}


/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl FBActivityProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}


/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage FBActivityProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> FBActivityProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList FBActivityProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString FBActivityProviderBase::pluginId( ) const
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
QString FBActivityProviderBase::authenticationApp( QString &aProgram, 
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
QString FBActivityProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}


/**
 * Method that initializes this class. This method should be called 
 * from the initialize() method of the FBActivityFetcherPlugin class
 */
void FBActivityProviderBase::initialize()
	{
	m_serviceName = "Facebook";
	m_description = "Facebook activity fetcher plugin description";
	m_serviceUrl = QUrl(QString("http://www.facebook.com"));
	m_pluginId = "fbactivityfetcherplugin.qtplugin";
	m_authAppId = "0xEFE2FD23";
	m_supportedInterfaces.append("org.symbian.smf.plugin.activity.fetcher/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("FBCMRegToken").toString();
	m_validity = iSettings.value("FBExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : fbactivityfetcherplugin
 * plugin class : FBActivityFetcherPlugin
 */
Q_EXPORT_PLUGIN2( fbactivityfetcherplugin, FBActivityFetcherPlugin )

