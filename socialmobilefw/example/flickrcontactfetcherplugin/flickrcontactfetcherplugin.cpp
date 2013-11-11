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
 * The Plugin that fetches contacts from the logged in user's flickr account
 *
 */

// Include files
#include <QtPlugin>
#include <QCryptographicHash>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QListIterator>
#include <QDebug>
#include <QSettings>
#include <smfpluginutil.h>
#ifdef SMF_XMLPARSING
#include <QXmlStreamReader>
#endif

#include "flickrcontactfetcherplugin.h"

/**
 * Destructor
 */
FlickrContactFetcherPlugin::~FlickrContactFetcherPlugin( )
	{
	if(m_provider)
		delete m_provider;
	}

/**
 * Method to interpret the key sets obtained from credential manager 
 * @param aApiKey [out] The api key
 * @param aApiSecret [out] The api secret
 * @param aAuthToken [out] The auth token provided by Flickr
 */
void FlickrContactFetcherPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aAuthToken )
	{
	qDebug()<<"Inside FlickrContactFetcherPlugin::fetchKeys()";

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
    keyName.append("AuthToken");
    aAuthToken.append(keys.value(keyName));
		
	qDebug()<<"Api Key = "<<aApiKey;
	qDebug()<<"Api Secret = "<<aApiSecret;
	qDebug()<<"Auth Token = "<<aAuthToken;
	}


/**
 * Method to get the list of friends
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrContactFetcherPlugin::friends( SmfPluginRequestData &aRequest,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FlickrContactFetcherPlugin::friends()";
	
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
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("filterfriends");
#ifdef SMF_XMLPARSING
	baseString.append("formatxml");
#else
	baseString.append("formatjson");
#endif
	baseString.append("methodflickr.contacts.getList");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append("per_page"+QString::number(aItemsPerPage));
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("filter", "friends");
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "x");
#else
	url.addQueryItem("format", "json");
#endif
	url.addQueryItem("method", "flickr.contacts.getList");
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("per_page", QString::number(aItemsPerPage));
	url.addQueryItem("api_sig", generateSignature(baseString));
	
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
QString FlickrContactFetcherPlugin::generateSignature(const QString aBaseString)
	{
	qDebug()<<"Inside FlickrContactFetcherPlugin::generateSignature()";
	
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    qDebug()<<"generated signature = "<<QString(returnString);
    return returnString;
	}

/**
 * Method to get the list of followers
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrContactFetcherPlugin::followers( SmfPluginRequestData &aRequest,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
#ifndef TESTINGTHISFUNCTION
	Q_UNUSED(aRequest)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FlickrContactFetcherPlugin::followers()";
	return SmfPluginErrServiceNotSupported; 
#else
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
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("filterfriends");
#ifdef SMF_XMLPARSING
	baseString.append("formatxml");
#else
	baseString.append("formatjson");
#endif
	baseString.append("methodflickr.contacts.getList");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append("per_page"+QString::number(aItemsPerPage));
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("filter", "friends");
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "x");
#else
	url.addQueryItem("format", "json");
#endif
	url.addQueryItem("method", "flickr.contacts.getList");
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("per_page", QString::number(aItemsPerPage));
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactGetFollowers;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error; 
#endif
	}

/**
 * Method to search for a contact
 * @param aRequest [out] The request data to be sent to network
 * @param aContact contact to be searched
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrContactFetcherPlugin::search( SmfPluginRequestData &aRequest,
		const SmfContact &aContact,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
#ifndef TESTINGTHISFUNCTION
	Q_UNUSED(aRequest)
	Q_UNUSED(aContact)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FlickrContactFetcherPlugin::search()";
	return SmfPluginErrServiceNotSupported; 
#else
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("formatjson");
	baseString.append("methodflickr.people.findByUsername");
	//baseString.append("page"+QString::number(aPageNum));
	//baseString.append("per_page"+QString::number(aItemsPerPage));
	baseString.append("username"+(aContact.value("Name").value<QContactName>().firstName()));
	
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "flickr.people.findByUsername");
	url.addQueryItem("username",aContact.value("Name").value<QContactName>().firstName());
	//url.addQueryItem("page", QString::number(10));
	//url.addQueryItem("per_page", QString::number(10));

	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactSearch;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
//	writeLog("Url string is :"+aRequest.iNetworkRequest.url().toString());
	return error; 

#endif
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
SmfPluginError FlickrContactFetcherPlugin::searchNear( SmfPluginRequestData &aRequest,
		const SmfLocation &aLocation,
		const SmfLocationSearchBoundary &aProximity,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
#ifndef TESTINGTHISFUNCTION
	Q_UNUSED(aRequest)
	Q_UNUSED(aLocation)
	Q_UNUSED(aProximity)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FlickrContactFetcherPlugin::searchNear()";
	return SmfPluginErrServiceNotSupported; 
#else
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
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("filterfriends");
#ifdef SMF_XMLPARSING
	baseString.append("formatxml");
#else
	baseString.append("formatjson");
#endif
	baseString.append("methodflickr.contacts.getList");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append("per_page"+QString::number(aItemsPerPage));
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("filter", "friends");
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "x");
#else
	url.addQueryItem("format", "json");
#endif
	url.addQueryItem("method", "flickr.contacts.getList");
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("per_page", QString::number(aItemsPerPage));
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactSearchNear;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error; 
#endif
	}

/**
 * Method to get the list of groups
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrContactFetcherPlugin::groups( SmfPluginRequestData &aRequest,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
	qDebug()<<"Inside FlickrContactFetcherPlugin::groups()";

	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
		/*if( aPageNum < 0 || aItemsPerPage < 0 )
			{
			qDebug()<<"Invalid arguments";
			return error;
			}*/
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("formatjson");
	baseString.append("methodflickr.groups.pools.getGroups");
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("format", "json");
	url.addQueryItem("method","flickr.groups.pools.getGroups");
	url.addQueryItem("api_sig", generateSignature(baseString));
		
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactGetGroups;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
//	writeLog("Url string is : "+aRequest.iNetworkRequest.url().toString());
	return error; 
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
SmfPluginError FlickrContactFetcherPlugin::searchInGroup( SmfPluginRequestData &aRequest,
		const SmfGroup &aGroup,
		SmfContact *aContact,
		const int aPageNum , 
		const int aItemsPerPage  )
	{
#ifndef TESTINGTHISFUNCTION
	Q_UNUSED(aRequest)
	Q_UNUSED(aGroup)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside FlickrContactFetcherPlugin::searchInGroup()";
	return SmfPluginErrServiceNotSupported; 
#else
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
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("filterfriends");
#ifdef SMF_XMLPARSING
	baseString.append("formatxml");
#else
	baseString.append("formatjson");
#endif
	baseString.append("methodflickr.contacts.getList");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append("per_page"+QString::number(aItemsPerPage));
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("filter", "friends");
#ifdef SMF_XMLPARSING
	url.addQueryItem("format", "x");
#else
	url.addQueryItem("format", "json");
#endif
	url.addQueryItem("method", "flickr.contacts.getList");
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("per_page", QString::number(aItemsPerPage));
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfContactSearchInGroup;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error; 
#endif
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
SmfPluginError FlickrContactFetcherPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	qDebug()<<"Inside FlickrContactFetcherPlugin::customRequest()";
	return SmfPluginErrServiceNotSupported; 
	}

/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void FlickrContactFetcherPlugin::initialize( )
	{
	// Create an instance of FlickrProviderBase
	m_provider = new FlickrProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* FlickrContactFetcherPlugin::getProviderInfo( )
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
SmfPluginError FlickrContactFetcherPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	Q_UNUSED(aPageResult)
	qDebug()<<"Inside FlickrContactFetcherPlugin::responseAvailable()";
	
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	QFile respFile("c://data//SmfPluginFlickrContactResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		{
		qDebug()<<"File to write the response could not be opened, so writing to this file";
		qDebug()<<"Flickr response = "<<QString(response);
		}
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfPluginFlickrContactResponse.txt'";
		}
	qDebug()<<"FB response size = "<<response.size();
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";
		
#ifndef TESTINGTHISFUNCTION	
		if(SmfContactGetFriends == aOperation)
#else
		if(SmfContactGetFriends == aOperation ||aOperation == SmfContactGetFollowers||aOperation== SmfContactSearch ||aOperation ==SmfContactSearchNear||aOperation ==SmfContactSearchInGroup)
#endif
			{
			qDebug()<<"For getting friends response";
			
			QList<SmfContact> list;
			
#ifdef SMF_XMLPARSING // Xml parsing
			// For getting contacts from xml response
			QXmlStreamReader xml(response);
			while (!xml.atEnd())
				{
				xml.readNext();
				if (xml.tokenType() == QXmlStreamReader::StartElement)
					{
					// If the tag is contact
					if (xml.name() == "contact")
						{
						qDebug()<<"Contact tag found";
						SmfContact contact;
						QStringRef str;
						QContactName contactname;
						QString username = xml.attributes().value("username").toString();
						qDebug()<<"Username = "<<username;
						contactname.setFirstName(username);
						contactname.setLastName(username);
						QVariant namevar1 = QVariant::fromValue(contactname);
						contact.setValue("Name",namevar1);
						list.append(contact);
						}
					}
				}
#else
			// To remove the "jsonFlickrApi(" and also remove the last ")" from the response,
			// as these gives a Json parsing error
			response.remove(0, 14);
			response.chop(1);
			
			// For getting contacts from json response
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) {
				qDebug()<<"An error occurred during json parsing";
				aResult->setValue(list);
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
			}
			
			QVariantMap map1 = result["contacts"].toMap();
			qDebug()<<"page = "<<map1["page"].toString();
			qDebug()<<"pages = "<<map1["pages"].toString();
			qDebug()<<"per_page = "<<map1["per_page"].toString();
			qDebug()<<"perpage = "<<map1["perpage"].toString();
			qDebug()<<"total = "<<map1["perpage"].toString();
			
			QList<QVariant> list1 = map1["contact"].toList();
			
			QListIterator<QVariant> i(list1);
			while(i.hasNext())
				{
				SmfContact contact;
				QVariantMap map2 = i.next().toMap();
				qDebug()<<"nsid = "<<map2["nsid"].toString();
				qDebug()<<"username = "<<map2["username"].toString();
				qDebug()<<"iconserver = "<<map2["iconserver"].toString();
				qDebug()<<"iconfarm = "<<map2["iconfarm"].toString();
				qDebug()<<"ignored = "<<map2["ignored"].toString();
				qDebug()<<"realname = "<<map2["realname"].toString();
				qDebug()<<"friend = "<<map2["friend"].toString();
				qDebug()<<"family = "<<map2["family"].toString();
				qDebug()<<"path_alias = "<<map2["path_alias"].toString();
				qDebug()<<"location = "<<map2["location"].toString();
				
				// Contact Name
				QContactName contactname;
				QString username = map2["username"].toString();
				qDebug()<<"Username = "<<username;
				contactname.setFirstName(username);
				contactname.setLastName(username);
				QVariant nameVar = QVariant::fromValue(contactname);
				contact.setValue("Name",nameVar);
				
				// Contact's Flickr Specific ID to QContactGuid
				QContactGuid guid;
				guid.setGuid(map2["nsid"].toString());
				QVariant guidVar = QVariant::fromValue(guid);
				contact.setValue("Guid",guidVar);
					
				// Contact's profile image url
				QUrl url;
				if((0 == map2["iconfarm"].toInt()) && (0 == map2["iconserver"].toInt()))
					url = QString("http://www.flickr.com/images/buddyicon.jpg");
				else
					{
					QString str("http://farm");
					str.append(map2["iconfarm"].toString());
					str.append(".static.flickr.com/");
					str.append(map2["iconserver"].toString());
					str.append("/buddyicons/");
					str.append(map2["nsid"].toString());
					str.append(".jpg");
					url = str;
					}
				QContactAvatar avatar;
				qDebug()<<"Profile image URL = "<<url.toString();
				avatar.setImageUrl(url);
				QVariant avatarVar = QVariant::fromValue(avatar);
				contact.setValue("Avatar",avatarVar);
				
				
				list.append(contact);
				}
#endif
			
			qDebug()<<"list count = "<<list.count();
			aResult->setValue(list);
			aRetType = SmfRequestComplete;
			error = SmfPluginErrNone;
			}
		else if(aOperation==SmfContactGetGroups)
			{
		        response.remove(0, 14);
				response.chop(1);
			
			    bool ok;
				qDebug()<<"Before Parser--";

				SmfPluginUtil util;
				QVariant result = util.parse(response, &ok);
				if (!ok) 
				{
				    qDebug()<<"An error occurred during json parsing";
					aRetType = SmfRequestError;
					return SmfPluginErrParsingFailed;
								 //return 0;
				}
							
				QVariantMap map1 = result.toMap();
		        QList<SmfGroup> list;
				QVariantMap map2 = map1["groups"].toMap();
				int page = map2["page"].toInt(&ok);
				qDebug()<<"PAGE = "<<map2["page"].toString();
				QList<QVariant> list1 = map2["group"].toList();
				QListIterator<QVariant> iter(list1);
						
				//Getting the group list from QJson Parser 
				 while(iter.hasNext())
				 {
					QVariantMap map2 = iter.next().toMap();
					qDebug()<<"name = "<<map2["name"].toString();
					qDebug()<<"id = "<<map2["id"].toString();
					SmfGroup group;
					QString id(map2["id"].toString());
					group.setId(id);
					QString grpname(map2["name"].toString());
					group.setName(grpname);
						   
					list.append(group);
						   
				 }//end While
				 qDebug()<<"list count = "<<QString::number(list.count(),10);
				aResult->setValue(list);
			}
		else
			{
			qDebug()<<"Service unsupported, currently only SmfContactGetFriends !!!";
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
FlickrProviderBase::~FlickrProviderBase( )
	{
	}

/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString FlickrProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}

/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage FlickrProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}

/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString FlickrProviderBase::description( ) const
	{
	return m_description;
	}

/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl FlickrProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}

/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl FlickrProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}

/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage FlickrProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> FlickrProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList FlickrProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString FlickrProviderBase::pluginId( ) const
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
QString FlickrProviderBase::authenticationApp( QString &aProgram, 
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
QString FlickrProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}

void FlickrProviderBase::initialize()
	{
	m_serviceName = "Flickr";
	m_description = "Flickr contact fetcher plugin description";
	m_serviceUrl = QUrl(QString("http://api.flickr.com"));
	m_pluginId = "flickrcontactfetcherplugin.qtplugin";
	m_authAppId = "0xE1D8C7D7";
	m_supportedInterfaces.append("org.symbian.smf.plugin.contact.fetcher/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("CMFlickrRegToken").toString();
	m_validity = iSettings.value("FlckrExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : flickrcontactfetcherplugin
 * plugin class : FlickrContactFetcherPlugin
 */
Q_EXPORT_PLUGIN2( flickrcontactfetcherplugin, FlickrContactFetcherPlugin )

