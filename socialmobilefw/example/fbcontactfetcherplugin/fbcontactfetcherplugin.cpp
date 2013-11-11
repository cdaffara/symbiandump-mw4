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
 * The Plugin that fetches contacts from the logged in user's facebook account
 *
 */

// Include files
#include <QtPlugin>
#include <QDebug>
#include <QCryptographicHash>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QListIterator>
#include <QSettings>
#include <smfpluginutil.h>
#ifdef SMF_XMLPARSING
#include <QXmlStreamReader>
#endif

#include "fbcontactfetcherplugin.h"

// Global variables
static int chance = 0;
QString uids;

#ifdef SMF_XMLPARSING
SmfContact contact;
#endif

// Todo:- Macro added for limiting items fetched to recent 5
// Remove after demo
#define SETLIMITOFFIVEFORSMFDEMO 1

/**
 * Destructor
 */
FBContactFetcherPlugin::~FBContactFetcherPlugin( )
	{
	if(m_provider)
		delete m_provider;
	}

/**
 * Method to interpret the key sets obtained from credential manager 
 * @param aApiKey [out] The api key
 * @param aApiSecret [out] The api secret
 * @param aSessionKey [out] The session key
 * @param aSessionSecret [out] The session secret
 * @param aAppId [out] The application ID
 */
void FBContactFetcherPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aSessionKey, 
		QString &aSessionSecret )
	{
	qDebug()<<"Inside FBContactFetcherPlugin::fetchKeys()";

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
 * Method to get the list of friends
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::friends( SmfPluginRequestData &aRequest,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FBContactFetcherPlugin::friends()";
	
	if(0 == chance)
		return getFriendsUids(aRequest, aPageNum, aItemsPerPage);
	else// if(1 == chance)
		return getFriendsDetails(aRequest, aPageNum, aItemsPerPage);
	}

/**
 * Method to get Facebook specific UIDs of the user's friends
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::getFriendsUids( SmfPluginRequestData &aRequest,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FBContactFetcherPlugin::getFriendsUids()";
	
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
		
	// Get the current date and time and convert it to seconds as a string
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
	if(0 == aPageNum)
		baseString.append("limit="+QString::number((aItemsPerPage*(aPageNum+1)), 10));
	else
		baseString.append("limit="+QString::number((aItemsPerPage*aPageNum), 10));
	baseString.append("method=friends.get");
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
	if(0 == aPageNum)
		url.addQueryItem("limit", QString::number((aItemsPerPage*(aPageNum+1)), 10));
	else
		url.addQueryItem("limit", QString::number((aItemsPerPage*aPageNum), 10));
	url.addQueryItem("method", "friends.get");
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("ss", "1");
	url.addQueryItem("v", "1.0");		
	url.addQueryItem("sig", generateSignature(baseString));
			
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactGetFriends;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}


/**
 * Method to get the details of users friends from facebook
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::getFriendsDetails( SmfPluginRequestData &aRequest,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FBContactFetcherPlugin::getFriendsDetails()";
	
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
	baseString.append("fields=uid,name,pic,pic_square,status");
#ifdef SMF_XMLPARSING
	baseString.append("format=XML");
#else
	baseString.append("format=JSON");
#endif
	baseString.append("method=users.getInfo");
	baseString.append("session_key="+sessionKey);
	baseString.append("ss=1");
	
#ifdef SETLIMITOFFIVEFORSMFDEMO // limiting to fetch only first 5 contact details
	
	QStringList firstFiveUids = uids.split(',');
	uids.clear();
	int i = 0;
	while(i < 5)
		{
		uids.append(firstFiveUids.at(i));
		uids.append(",");
		i++;
		}
	if(firstFiveUids.count()>5)
		uids.chop(1);
	qDebug()<<"Limited to 5 uid string = "<<uids;
#endif
	
	baseString.append("uids="+uids);
	baseString.append("v=1.0");
	baseString.append(sessionSecret);

	// Create the url
	QUrl url("http://api.facebook.com/restserver.php?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("call_id", call_id);
	url.addQueryItem("fields", "uid,name,pic,pic_square,status");
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "XML");
#else
	url.addQueryItem("format", "JSON");
#endif
	url.addQueryItem("method", "users.getInfo");
	url.addQueryItem("session_key", sessionKey);
	url.addQueryItem("ss", "1");
	url.addQueryItem("uids", uids);
	url.addQueryItem("v", "1.0");		
	url.addQueryItem("sig", generateSignature(baseString));
				
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactGetFriends;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;

	}


/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString FBContactFetcherPlugin::generateSignature(const QString aBaseString)
	{
	qDebug()<<"Inside FBContactFetcherPlugin::generateSignature()";
	
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}


/**
 * Method to get the list of followers
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::followers( SmfPluginRequestData &aRequest,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FBContactFetcherPlugin::followers()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * Method to search for a contact
 * @param aRequest [out] The request data to be sent to network
 * @param aContact contact to be searched
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::search( SmfPluginRequestData &aRequest,
		const SmfContact &aContact,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aContact)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FBContactFetcherPlugin::search()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * Method to search for contacts (friends) who are near the user.
 * Proximity defines accuracy level
 * @param aRequest [out] The request data to be sent to network
 * @param aLocation The location search criteria
 * @param aProximity location search boundary
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::searchNear( SmfPluginRequestData &aRequest,
		const SmfLocation &aLocation,
		const SmfLocationSearchBoundary &aProximity,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aLocation)
	Q_UNUSED(aProximity)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FBContactFetcherPlugin::searchNear()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * Method to get the list of groups
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::groups( SmfPluginRequestData &aRequest,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FBContactFetcherPlugin::groups()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * Method to search for a contact in a group
 * @param aRequest [out] The request data to be sent to network
 * @param aGroup the group in which to search
 * @param aContact The contact to be searched, default (NULL) is the self contact.
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::searchInGroup( SmfPluginRequestData &aRequest,
		const SmfGroup &aGroup,
		SmfContact *aContact,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aGroup)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FBContactFetcherPlugin::searchInGroup()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * Customised method for SmfContactFetcherPlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FBContactFetcherPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	qDebug()<<"Inside FBContactFetcherPlugin::customRequest()";
	return SmfPluginErrServiceNotSupported; 
	}


/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void FBContactFetcherPlugin::initialize( )
	{
	// Create an instance of FBContactProviderBase
	m_provider = new FBContactProviderBase;
	m_provider->initialize();
	}


/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* FBContactFetcherPlugin::getProviderInfo( )
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
 * For SmfGalleryPlugin: If last operation was pictures(), aResult will 
 * be of type QList<SmfPicture>. If last operation was description(), 
 * aResult will be of type QString. If last operation was upload() or 
 * postComment(), aResult will be of type bool.
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError FBContactFetcherPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	Q_UNUSED(aPageResult)
	qDebug()<<"Inside FBContactFetcherPlugin::responseAvailable()";
	
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;

	QFile respFile("c://data//SmfPluginFBContactResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		{
		qDebug()<<"File to write the response could not be opened, so writing to this file";
		qDebug()<<"Flickr response = "<<QString(response);
		}
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfPluginFBContactResponse.txt'";
		}
	
	qDebug()<<"Response size = "<<response.size();
	
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";
		
		if(SmfContactGetFriends == aOperation)
			{
			// For getting contact's UIDs
			if(0 == chance)
				{
				qDebug()<<"For Getting friends UIDs";
				chance = 1;
				
				QString errStr;
				errStr.clear();
				uids.clear();
				
#ifdef SMF_XMLPARSING
				qDebug()<<"Xml parsing";
				
				// For getting contacts from xml response
				QXmlStreamReader xml(response);
				int count = 0;
				while (!xml.atEnd())
					{
					xml.readNext();
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
						if (xml.name() == "uid")
							{
							qDebug()<<"uid tag found";
							QString message(xml.readElementText());
							uids.append(message);
							uids.append(",");
							count++;
#ifdef SETLIMITOFFIVEFORSMFDEMO
							if(5 == count)
								break;
#endif
							}
						else if("error_msg" == xml.name())
							{
							qDebug()<<"error_msg tag found";
							errStr.append(xml.readElementText());
							break;
							}
						}
					}
				// Remove the final appended ',' for no error
				if(0 == errStr.size())
					uids.chop(1);
#else
				qDebug()<<"Json parsing";
		
				// For error in response
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
					response.remove(0, 1);
					response.chop(1);
					uids.append(response);
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
					qDebug()<<"UIDs array = "<<uids;
					error = SmfPluginErrNone;
					aRetType = SmfSendRequestAgain;
					}
				}
			
			// For getting details of friends like name, pic etc.
			else// if(1 == chance)
				{
				QList<SmfContact> list;
			
				QString errStr;
				errStr.clear();
				qDebug()<<"For Getting friends details";
				chance = 0;
				
#ifdef SMF_XMLPARSING
				qDebug()<<"Xml parsing";
				
				// For getting contacts from xml response
				QXmlStreamReader xml(response);
				while (!xml.atEnd())
					{
					xml.readNext();
					
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
						if (xml.name() == "name")
							{
							QString message(xml.readElementText());
							
							QContactName contactname;
							contactname.setFirstName(message);
							contactname.setLastName(message);
							QVariant nameVar = QVariant::fromValue(contactname);
							contact.setValue("Name",nameVar);
							}
						else if (xml.name() == "uid")
							{
							QString message(xml.readElementText());
							QContactGuid guid;
							guid.setGuid(message);
							QVariant guidVar = QVariant::fromValue(guid);
							contact.setValue("Guid", guidVar);
							}
						else if (xml.name() == "pic_square")
							{
							QUrl url(xml.readElementText());
							qDebug()<<"pic_square = "<<url.toString();
							QContactAvatar avatar;
							avatar.setImageUrl(url);
							QVariant avatarVar = QVariant::fromValue(avatar);
							contact.setValue("Avatar", avatarVar);
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
						if (xml.name() == "user")
							{
							qDebug()<<"avatar url = "<<contact.value("Avatar").value<QContactAvatar>().imageUrl().toString();
							list.append(contact);
							}
						}
					}
#else
				qDebug()<<"Json parsing";
				
				bool ok;
				SmfPluginUtil util;
				QVariant result = util.parse(response, &ok);
				if (!ok) 
					{
					qDebug()<<"An error occurred during json parsing";
					aRetType = SmfRequestError;
					return SmfPluginErrParsingFailed;
					}
				
				if(response.contains(QByteArray("error_msg")))
					{
					QVariantMap map = result.toMap();
					errStr.append(map["error_msg"].toString());
					}
				else
					{
					QList<QVariant> list1 = result.toList();
					QListIterator<QVariant> iter(list1);
					while(iter.hasNext())
						{
						SmfContact contact;
						QVariantMap map2 = iter.next().toMap();
						qDebug()<<"name = "<<map2["name"].toString();
						qDebug()<<"pic = "<<map2["pic"].toString();
						qDebug()<<"uid = "<<map2["uid"].toString();
						qDebug()<<"pic_square = "<<map2["pic_square"].toString();
						
						QContactName contactname;
						contactname.setFirstName(map2["name"].toString());
						contactname.setLastName(map2["name"].toString());
						QVariant nameVar = QVariant::fromValue(contactname);
						contact.setValue("Name",nameVar);
						
						QContactAvatar avatar;
						QUrl url;
						if(0 != map2["pic_square"].toString().size())
							url = map2["pic_square"].toString();
						else
							url = QString("http://static.ak.fbcdn.net/rsrc.php/z5HB7/hash/ecyu2wwn.gif");
						avatar.setImageUrl(url);
						QVariant avatarVar = QVariant::fromValue(avatar);
						contact.setValue("Avatar", avatarVar);
						
						QContactGuid guid;
						guid.setGuid(map2["uid"].toString());
						QVariant guidVar = QVariant::fromValue(guid);
						contact.setValue("Guid", guidVar);
						
						QContactPresence presence;
						presence.setCustomMessage(map2["status"].toString());
						QVariant presVar = QVariant::fromValue(presence);
						contact.setValue("Presence", presVar);
						
						
#if 0 // tesing SmfContact serialization
						QContactAddress address;
						address.setCountry("sampleCountry");
						address.setLocality("samplelocatlity");
						address.setPostOfficeBox("12345");
						address.setPostcode("67890");
						address.setRegion("sampleregion");
						address.setStreet("sampleStreet");
						QStringList list1;
						list1.append("type1");
						address.setSubTypes(list1);
						QVariant var1 = QVariant::fromValue(address);
						contact.setValue("Address", var1);
								
						QContactAnniversary anni;
						anni.setCalendarId("12345");
						anni.setEvent("6547645");
						QDate date1(2010, 06, 18);
						anni.setOriginalDate(date1);
						anni.setSubType("76345764");
						QVariant var2 = QVariant::fromValue(anni);
						contact.setValue("Anniversary", var2);
						
						QContactAvatar avatar;
						QUrl url(map2["pic_square"].toString());
						avatar.setImageUrl(url);
						avatar.setVideoUrl(url);
						QVariant avatarVar = QVariant::fromValue(avatar);
						contact.setValue("Avatar", avatarVar);
						
						QContactBirthday bday;
						date1.setDate(1983,9,12);
						bday.setDate(date1);
						QVariant var3 = QVariant::fromValue(bday);
						contact.setValue("Birthday", var3);
						
						QContactId id;
						QContactLocalId localid = 10;
						id.setLocalId(localid);
						id.setManagerUri("djfhjhyd");
						QVariant v4 = QVariant::fromValue(id);
						contact.setValue("ContactId", v4);
						
						QContactEmailAddress email;
						email.setEmailAddress("sdjfhujsdhf@kjdfk.com");
						QVariant v5 = QVariant::fromValue(email);
						contact.setValue("EmailAddress", v5);
						
						QContactGender gender;
						gender.setGender("female");
						QVariant v6 = QVariant::fromValue(gender);
						contact.setValue("Gender", v6);
						
						QTime t(14,0,0);
						QDateTime time1(date1, t);
						QContactGeoLocation geo;
						geo.setAccuracy(1.23);
						geo.setAltitude(2.34);
						geo.setAltitudeAccuracy(3.45);
						geo.setHeading(4.56);
						geo.setLabel("hdgfhdgf");
						geo.setLatitude(6.78);
						geo.setLongitude(7.89);
						geo.setSpeed(23.456);
						geo.setTimestamp(time1);
						QVariant v7 = QVariant::fromValue(geo);
						contact.setValue("Geolocation", v7);
						
						QContactGuid guid;
						guid.setGuid(map2["uid"].toString());
						QVariant guidVar = QVariant::fromValue(guid);
						contact.setValue("Guid", guidVar);
						
						QContactName contactname;
						contactname.setFirstName(map2["name"].toString());
						contactname.setLastName(map2["name"].toString());
						contactname.setCustomLabel("jsdhfjhsdf");
						contactname.setMiddleName("kjtiuer");
						contactname.setPrefix("djfhj");
						contactname.setSuffix("jdhf");
						QVariant nameVar = QVariant::fromValue(contactname);
						contact.setValue("Name",nameVar);
						
						QContactNickname nickname;
						nickname.setNickname("ddfffff");
						QVariant v8 = QVariant::fromValue(nickname);
						contact.setValue("Nickname", v8);
						
						QContactNote note;
						note.setNote("jdhfjsdhf");
						QVariant v9 = QVariant::fromValue(note);
						contact.setValue("Note", v9);
						
						QContactOnlineAccount account;
						account.setAccountUri("jnjhd");
						account.setCapabilities(list1);
						account.setServiceProvider("jhjyhjk");
						list1.append("345");
						account.setSubTypes(list1);
						QVariant v10 = QVariant::fromValue(account);
						contact.setValue("OnlineAccount", v10);
						
						QContactOrganization org;
						org.setAssistantName("mnjhj");
						org.setDepartment(list1);
						org.setLocation("mnjh");
						org.setLogoUrl(url);
						org.setName("kkiujki");
						org.setRole("nhfg");
						org.setTitle("mnhfg");
						QVariant v11 = QVariant::fromValue(org);
						contact.setValue("Organization", v11);
						
						QContactPhoneNumber number;
						number.setNumber("76347");
						list1.append("jhsdjhsdf");
						number.setSubTypes(list1);
						QVariant v12 = QVariant::fromValue(number);
						contact.setValue("PhoneNumber", v12);
						
						QContactPresence presence;
						presence.setCustomMessage("djhfjhsd");
						presence.setNickname("kajsiju");
						QContactPresence::PresenceState state = QContactPresence::PresenceBusy;
						presence.setPresenceState(state);
						presence.setPresenceStateImageUrl(url);
						presence.setPresenceStateText("Busy Now");
						presence.setTimestamp(time1);
						QVariant v13 = QVariant::fromValue(presence);
						contact.setValue("Presence", v13);
						
						QContactThumbnail thumbnail;
						QImage image("c://Data//TestUploadPics//BlueHills.jpg");
						qDebug()<<"Image bytecount = "<<image.byteCount();
						thumbnail.setThumbnail(image);
						QVariant v14 = QVariant::fromValue(thumbnail);
						contact.setValue("Thumbnail", v14);
						
						QContactTimestamp timestamp;
						timestamp.setCreated(time1);
						timestamp.setLastModified(time1);
						QVariant v15 = QVariant::fromValue(timestamp);
						contact.setValue("Timestamp", v15);
						
						QContactType type;
						type.setType("default");
						QVariant v16 = QVariant::fromValue(type);
						contact.setValue("Type", v16);
						
						QContactUrl myurl;
						myurl.setUrl("My Url");
						myurl.setSubType("my url type");
						QVariant v17 = QVariant::fromValue(myurl);
						contact.setValue("Url", v17);

#endif
						
						list.append(contact);
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
		else
			{
			qDebug()<<"Service unsupported, as of now only SmfContactGetFriends!!!";
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
FBContactProviderBase::~FBContactProviderBase( )
	{
	}


/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString FBContactProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}


/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage FBContactProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}


/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString FBContactProviderBase::description( ) const
	{
	return m_description;
	}


/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl FBContactProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}


/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl FBContactProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}


/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage FBContactProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> FBContactProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList FBContactProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString FBContactProviderBase::pluginId( ) const
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
QString FBContactProviderBase::authenticationApp( QString &aProgram, 
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
QString FBContactProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}


/**
 * Method that initializes this class. This method should be called 
 * from the initialize() method of the FBContactFetcherPlugin class
 */
void FBContactProviderBase::initialize()
	{
	m_serviceName = "Facebook";
	m_description = "Facebook contact fetcher plugin description";
	m_serviceUrl = QUrl(QString("http://api.facebook.com"));
	m_pluginId = "fbcontactfetcherplugin.qtplugin";
	m_authAppId = "0xEFE2FD23";
	m_supportedInterfaces.append("org.symbian.smf.plugin.contact.fetcher/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("FBCMRegToken").toString();
	m_validity = iSettings.value("FBExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : fbcontactfetcherplugin
 * plugin class : FBContactFetcherPlugin
 */
Q_EXPORT_PLUGIN2( fbcontactfetcherplugin, FBContactFetcherPlugin )

