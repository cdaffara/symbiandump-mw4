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
 * The Plugin that does music services related functionalities from last.fm site
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
#include <smfcontact.h>

#include "lastfmmusicserviceplugin.h"


static int gPageNum = 0; 
static int gItemsPerPage = 0;

QString gUserId;
static int userMusicInfoChance = 0;
QList<SmfTrackInfo> gRecentTracks;
QList<SmfTrackInfo> gFavorites;
QList<SmfEvent> gUserEvents;

/**
 * Destructor
 */
LastFmMusicServicePlugin::~LastFmMusicServicePlugin( )
	{
	if(m_provider)
		delete m_provider;
	}

/**
 * Method to interpret the key sets obtained from credential manager 
 * @param aApiKey [out] The api key
 * @param aApiSecret [out] The api secret
 * @param aSessionKey [out] The session key
 * @param aToken [out] The session token
 */
void LastFmMusicServicePlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aToken,
		QString &aName )
	{
	qDebug()<<"Reg Token = "<<m_provider->m_smfRegToken;
	qDebug()<<"Expiry Date as int = "<<m_provider->m_validity.toTime_t();
	
	SmfAuthParams keys;
	SmfPluginUtil util;
	util.getAuthKeys(keys, m_provider->m_smfRegToken, 
			m_provider->m_validity, m_provider->m_pluginId);
	
    QByteArray keyName;
    keyName.append("ApiKey");
	aApiKey.append(keys.value(keyName));
	
    keyName.clear();
    keyName.append("ApiSecret");
	aApiSecret.append(keys.value(keyName));
	
	keyName.clear();
    keyName.append("Token");
	aToken.append(keys.value(keyName));
	
	keyName.clear();
    keyName.append("Name");
	aName.append(keys.value(keyName));
	
	qDebug()<<"Api Key = "<<aApiKey;
	qDebug()<<"Api Secret = "<<aApiSecret;
	qDebug()<<"Token = "<<aToken;
	qDebug()<<"Name = "<<aName;
	}

/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString LastFmMusicServicePlugin::generateSignature(const QString aBaseString)
	{
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}

/**
 * Method to get self profile information
 * @param aRequest [out] The request data to be sent to network
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::userMusicInfo( 
		SmfPluginRequestData &aRequest )
	{
	qDebug()<<"Inside LastFmMusicServicePlugin::userMusicInfo()";
	
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	QString userName;
	fetchKeys(apiKey, apiSecret, token, userName);
	
	// Get the current logged in user id
	if(0 == userMusicInfoChance)
		{
		// Clear the global variables
		gUserId.clear();
		gRecentTracks.clear();
		gFavorites.clear();
		gUserEvents.clear();
		gPageNum = 1;
		gItemsPerPage = 5;
		
		// Create the API signature string
		QString baseString;
		baseString.append("api_key"+apiKey);
		baseString.append("methoduser.getInfo");
		baseString.append("user"+userName);
		baseString.append(apiSecret);
	
		// Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format", "json");
		url.addQueryItem("method", "user.getInfo");
		url.addQueryItem("user", userName);
		url.addQueryItem("api_sig", generateSignature(baseString));
		
		// Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfMusicGetUserInfo;
		aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
		}

	// Getting the user's recent tracks
	else if(1 == userMusicInfoChance)
		{
		// Create the API signature string
		QString baseString;
		baseString.append("api_key"+apiKey);
		baseString.append("limit"+QString::number(gItemsPerPage));
		baseString.append("methoduser.getRecentTracks");
		baseString.append("page"+QString::number(gPageNum));
		baseString.append("user"+userName);
		baseString.append(apiSecret);
	
		// Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format", "json");
		url.addQueryItem("limit", QString::number(gItemsPerPage));		
		url.addQueryItem("method", "user.getRecentTracks");
		url.addQueryItem("page", QString::number(gPageNum));
		url.addQueryItem("user", userName);
		url.addQueryItem("api_sig", generateSignature(baseString));
		
		// Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfMusicGetUserInfo;
		aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
		}
	
	// Getting the user's favorites
	else if(2 == userMusicInfoChance)
		{
		// Create the API signature string
		QString baseString;
		baseString.append("api_key"+apiKey);
		baseString.append("limit"+QString::number(gItemsPerPage));
		baseString.append("methoduser.getLovedTracks");
		baseString.append("page"+QString::number(gPageNum));
		baseString.append("user"+userName);
		baseString.append(apiSecret);
	
		// Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format", "json");
		url.addQueryItem("limit", QString::number(gItemsPerPage));		
		url.addQueryItem("method", "user.getLovedTracks");
		url.addQueryItem("page", QString::number(gPageNum));
		url.addQueryItem("user", userName);
		url.addQueryItem("api_sig", generateSignature(baseString));
		
		// Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfMusicGetUserInfo;
		aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
		}
	
	// Get the user's events
	else //if(3 == userMusicInfoChance)
		{
		// Create the API signature string
		QString baseString;
		baseString.append("api_key"+apiKey);
		baseString.append("methoduser.getEvents");
		baseString.append("user"+userName);
		baseString.append(apiSecret);
	
		// Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format", "json");
		url.addQueryItem("method", "user.getEvents");
		url.addQueryItem("user", userName);
		url.addQueryItem("api_sig", generateSignature(baseString));
		
		// Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfMusicGetUserInfo;
		aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
		}
		
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search information about artists. All information 
 * in SmfArtists is not required, however more available the better
 * @param aRequest [out] The request data to be sent to network
 * @param aArtist The artist which is the search criteria
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::searchArtist( 
		SmfPluginRequestData &aRequest,
		const SmfArtists &aArtist,
		const int aPageNum , 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicServicePlugin::searchArtist()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || aArtist.names().at(0).isEmpty() )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
	
	gPageNum = aPageNum;
	gItemsPerPage = aItemsPerPage;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	QString userName;
	fetchKeys(apiKey, apiSecret, token, userName);

	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("artist"+aArtist.names().at(0));
	baseString.append("limit"+QString::number(aItemsPerPage));
	baseString.append("methodartist.search");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("artist", aArtist.names().at(0));
	url.addQueryItem("format", "json");
	url.addQueryItem("limit", QString::number(aItemsPerPage));
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("method", "artist.search");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicSearchArtist;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search information about album. All information 
 * in SmfAlbum is not required, however more available the better
 * @param aRequest [out] The request data to be sent to network
 * @param aAlbum [in] The album which is the search criteria
 * @param aPageNum [in] The page to be extracted
 * @param aItemsPerPage [in] Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::searchAlbum( 
		SmfPluginRequestData &aRequest,
		const SmfAlbum &aAlbum,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicServicePlugin::searchAlbum()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || aAlbum.name().isEmpty() )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
	
	gPageNum = aPageNum;
	gItemsPerPage = aItemsPerPage;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	QString userName;
	fetchKeys(apiKey, apiSecret, token, userName);

	// Create the API signature string
	QString baseString;
	baseString.append("album"+aAlbum.name());
	baseString.append("api_key"+apiKey);
	baseString.append("limit"+QString::number(aItemsPerPage));
	baseString.append("methodalbum.search");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("album", aAlbum.name());
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("limit", QString::number(aItemsPerPage));
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "album.search");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicSearchAlbum;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search information about events. All information 
 * in SmfEvent is not required, however more available the better
 * @param aRequest [out] The request data to be sent to network
 * @param aEvent [in] The event which is the search criteria
 * @param aPageNum [in] The page to be extracted
 * @param aItemsPerPage [in] Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::searchEvents( 
		SmfPluginRequestData &aRequest,
		const SmfEvent &aEvent,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicServicePlugin::searchEvents()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || aEvent.id().isEmpty() )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
	
	gPageNum = aPageNum;
	gItemsPerPage = aItemsPerPage;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	QString userName;
	fetchKeys(apiKey, apiSecret, token, userName);

	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("event"+aEvent.id());
	baseString.append("methodevent.getInfo");
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("event", aEvent.id());
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "event.getInfo");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicSearchEvent;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search information about venue. All information 
 * in SmfLocation is not required, however more available the better
 * @param aRequest [out] The request data to be sent to network
 * @param aVenue [in] The venue which is the search criteria
 * @param aPageNum [in] The page to be extracted
 * @param aItemsPerPage [in] Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::searchVenue( 
		SmfPluginRequestData &aRequest,
		const SmfLocation &aVenue,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicServicePlugin::searchVenue()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || aVenue.name().isEmpty() )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
	
	gPageNum = aPageNum;
	gItemsPerPage = aItemsPerPage;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	QString userName;
	fetchKeys(apiKey, apiSecret, token, userName);

	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("limit"+QString::number(aItemsPerPage));
	baseString.append("methodvenue.search");
	baseString.append("page"+QString::number(aPageNum));
	baseString.append("venue"+aVenue.name());
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("venue", aVenue.name());
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("limit", QString::number(aItemsPerPage));
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("method", "venue.search");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicSearchVenue;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search information about other service users for a 
 * particular place
 * @param aRequest [out] The request data to be sent to network
 * @param aPlace [in] The place which is the search criteria
 * @param aPageNum [in] The page to be extracted
 * @param aItemsPerPage [in] Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::searchUser( 
		SmfPluginRequestData &aRequest,
		const SmfLocation &aPlace,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aPlace)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside LastFmMusicServicePlugin::searchUser()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	
#if 0
	Q_UNUSED(aPlace)
	qDebug()<<"Inside LastFmMusicServicePlugin::searchUser()";
			
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
	QString token;
	QString userName;
	fetchKeys(apiKey, apiSecret, token, userName);

	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("methoduser.getNeighbours");
	baseString.append("user"+userName);
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "user.getNeighbours");
	url.addQueryItem("user", userName);
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicSearchUser;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
#endif
	}

/**
 * Method to post the currently playing track
 * @param aRequest [out] The request data to be sent to network
 * @param aTrack [in] The current playing track, that should be posted
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::postCurrentPlaying( 
		SmfPluginRequestData &aRequest,
		const SmfTrackInfo &aTrack )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aTrack)
	qDebug()<<"Inside LastFmMusicServicePlugin::postCurrentPlaying()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	}

/**
 * Method to post the rating on a track
 * @param aRequest [out] The request data to be sent to network
 * @param aTrack [in] The track on which rating should be posted
 * @param aRating [in] The rating values
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::postRating( 
		SmfPluginRequestData &aRequest, 
		const SmfTrackInfo &aTrack, 
		const SmfMusicRating &aRating )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aTrack)
	Q_UNUSED(aRating)
	qDebug()<<"Inside LastFmMusicServicePlugin::postRating()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	}

/**
 * Method to post comment on a track
 * @param aRequest [out] The request data to be sent to network
 * @param aTrack [in] The track on which comment should be posted
 * @param aComment [in] The comment content
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::postComments( 
		SmfPluginRequestData &aRequest, 
		const SmfTrackInfo &aTrack, 
		const SmfComment &aComment )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aTrack)
	Q_UNUSED(aComment)
	qDebug()<<"Inside LastFmMusicServicePlugin::postComments()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	}

/**
 * Customised method for SmfMusicServicePlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation [in] The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData [in] The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmMusicServicePlugin::customRequest( 
		SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	qDebug()<<"Inside LastFmMusicServicePlugin::customRequest()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	}

/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void LastFmMusicServicePlugin::initialize( )
	{
	// Create an instance of LastFmMusicServiceProviderBase
	m_provider = new LastFmMusicServiceProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* LastFmMusicServicePlugin::getProviderInfo( )
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
 * For SmfMusicServicePlugin: If last operation was userMusicInfo(), aResult
 * will be of type SmfMusicProfile. If last operation was searchArtist(),
 * aResult will be of type QList<SmfArtists>. If last operation was searchAlbum(),
 * aResult will be of type QList<SmfAlbum>. If last operation was searchEvents(),
 * aResult will be of type QList<SmfEvent>. If last operation was searchVenue(),
 * aResult will be of type QList<Smfocation>. If last operation was searchUser(),
 * aResult will be of type QList<SmfMusicProfile>. If last operation was
 * postCurrentPlaying() or postRating() or postComments(), aResult will be of
 * type bool.
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError LastFmMusicServicePlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	Q_UNUSED(aPageResult)
	qDebug()<<"Inside LastFmMusicServicePlugin::responseAvailable()";
	
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	QFile respFile("c://data//SmfMusicServicePluginResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		qDebug()<<"File to write the response could not be opened";
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfMusicServicePluginResponse.txt'";
		}
	qDebug()<<"FB response size = "<<response.size();
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";
		
		// Get music user info
		if(SmfMusicGetUserInfo == aOperation)
			{
			qDebug()<<"Response for getting user profile";
						
			QString errStr;
			errStr.clear();
				
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
					
			qDebug()<<"Json parsing complete";
			
			if(response.contains(QByteArray("error")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				if(0 == userMusicInfoChance)
					{
					qDebug()<<"For getting logged in user's name";
	
					QVariantMap map1 = result["user"].toMap();
					gUserId.clear();
					gUserId.append(map1["id"].toString());
					}
				else if(1 == userMusicInfoChance)
					{
					qDebug()<<"For getting recent tracks";
				
					QVariantMap map1 = result["recenttracks"].toMap();
					QList<QVariant> list1 = map1["track"].toList();
					QListIterator<QVariant> iter1(list1);
					while(iter1.hasNext())
						{
						QVariantMap map2 = iter1.next().toMap();
						SmfTrackInfo track;
						
						SmfArtists artist;
						QStringList names;
						QVariantMap map3 = map2["artist"].toMap();
						names.append(map3["#text"].toString());
						artist.setNames(names);
						
						artist.setId(map3["mbid"].toString());
						
						track.setArtists(artist);
						
						track.setTitle(map2["name"].toString());
						track.setId(map2["mbid"].toString());
						
						SmfAlbum album;
						QVariantMap map4 = map2["album"].toMap();
						album.setName(map4["#text"].toString());
						album.setId(map4["mbid"].toString());
						
						track.setAlbum(album);
						
						gRecentTracks.append(track);
						
						if(gItemsPerPage == gRecentTracks.count())
							break;
						}
					}
				else if(2 == userMusicInfoChance)
					{
					qDebug()<<"For getting favorites";
					
					QVariantMap map1 = result["lovedtracks"].toMap();
					QList<QVariant> list1 = map1["track"].toList();
					QListIterator<QVariant> iter1(list1);
					while(iter1.hasNext())
						{
						QVariantMap map2 = iter1.next().toMap();
						SmfTrackInfo track;
						
						track.setTitle(map2["name"].toString());
						qDebug()<<"Track name : "<<track.title();
						track.setId(map2["mbid"].toString());
						
						SmfArtists artist;
						QStringList names;
						QVariantMap map3 = map2["artist"].toMap();
						names.append(map3["name"].toString());
						artist.setNames(names);
						artist.setId(map3["mbid"].toString());
						QUrl url(map3["url"].toString());
						artist.setUrl(url);
						QList<QVariant> list2 = map3["image"].toList();
						QListIterator<QVariant> iter2(list2);
						while(iter2.hasNext())
							{
							QVariantMap map4 = iter2.next().toMap();
		
							// Set the artist's image url
							QUrl url(map4["#text"].toString());
							//artist.setImageUrlurl);
							qDebug()<<"Track artists image url : "<<url.toString();
							}
												
						track.setArtists(artist);
						qDebug()<<"Favorite tracks count = "<<gFavorites.count();
						gFavorites.append(track);
						
						if(gItemsPerPage == gFavorites.count())
							break;
						}
					}
				else //if(3 == userMusicInfoChance)
					{
					qDebug()<<"For getting user's registered events";
					QVariantMap map1 = result["events"].toMap();
					QList<QVariant> list1 = map1["event"].toList();
					QListIterator<QVariant> iter1(list1);
					while(iter1.hasNext())
						{
						QVariantMap map2 = iter1.next().toMap();
					
						SmfEvent event;
						event.setId(map2["id"].toString());
						event.setTitle(map2["title"].toString());
								
						// Set the events date and time
						QDateTime dateTime;// = QDateTime::fromString("M1d1y9800:01:02","'M'M'd'd'y'yyhh:mm:ss");
						event.setEventDateTime(dateTime);
						
						// Set the events artists
						SmfArtists artists;
						QStringList names;
						names.clear();
						QVariantMap map3 = map2["artists"].toMap();
						QList<QVariant> list1 = map3["artist"].toList();
						names.clear();
						QListIterator<QVariant> iter1(list1);
						while(iter1.hasNext())
							names.append(iter1.next().toString());
						artists.setNames(names);
						event.setArtists(artists);
						
						// Set the events venue information
						QVariantMap map4 = map2["venue"].toMap();
						SmfLocation location;
						location.setId(map4["id"].toString());
						location.setName(map4["name"].toString());
						QUrl url(map4["url"].toString());
						location.setUrl(url);
						
						
						QVariantMap map5 = map4["location"].toMap();
						location.setCity(map5["city"].toString());
						location.setCountry(map5["country"].toString());
						location.setStreet(map5["street"].toString());
						location.setZipCode(map5["postalcode"].toString());
						
						QVariantMap map6 = map5["geo:point"].toMap();
						double latitude = map6["geo:lat"].toDouble();
						double longitude = map6["geo:long"].toDouble();
						QGeoCoordinate coordinate(latitude, longitude);
						QGeoPositionInfo geoInfo;
						geoInfo.setCoordinate(coordinate);
						location.setGeoPositionInfo(geoInfo);
						
						event.setVenue(location);
						
						// Set the events ticketUrl
						QUrl ticketUrl(map2["tickets"].toString());
						event.setTicketUrl(ticketUrl);
	
	
						gUserEvents.append(event);
						
						if(gItemsPerPage == gUserEvents.count())
							break;
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
				if(0 == userMusicInfoChance)
					{
					qDebug()<<"current logged in userId  = "<<gUserId;
					aRetType = SmfSendRequestAgain;
					error = SmfPluginErrNone;
					userMusicInfoChance = 1;
					}
				else if(1 == userMusicInfoChance)
					{
					qDebug()<<"Count of user's recent tracks  = "<<gRecentTracks.count();
					aRetType = SmfSendRequestAgain;
					error = SmfPluginErrNone;
					userMusicInfoChance = 2;
					}
				else if(2 == userMusicInfoChance)
					{
					qDebug()<<"Count of user's favorite tracks  = "<<gFavorites.count();
					aRetType = SmfSendRequestAgain;
					error = SmfPluginErrNone;
					userMusicInfoChance = 3;
					}
				else //if(3 == userMusicInfoChance)
					{
					SmfMusicProfile profile;
					profile.setFavorites(gFavorites);
					profile.setRecentTracks(gRecentTracks);
					profile.setUserEvents(gUserEvents);
					profile.setId(gUserId);
					
					qDebug()<<"profile id in last.fm = "<<profile.id();
					aResult->setValue(profile);
					aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;
					}
				}
			
			}
		// Artists search
		else if ( SmfMusicSearchArtist == aOperation )
			{
			qDebug()<<"Response for searching artists";
			
			QList<SmfArtists> list;
			QString errStr;
			errStr.clear();
				
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
					
			qDebug()<<"Json parsing complete";
			
			if(response.contains(QByteArray("error")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1 = result["results"].toMap();
				QVariantMap map2 = map1["artistmatches"].toMap();
				QList<QVariant> list1 = map2["artist"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfArtists artist;
					QVariantMap map3 = iter.next().toMap();
					
					// Name of the artist
					QStringList names;
					names.append(map3["name"].toString());
					artist.setNames(names);
					
					// last.fm ID of the artist
					artist.setId(map3["mbid"].toString());
					
					// last.fm url of the artist
					QUrl url(map3["url"].toString());
					artist.setUrl(url);
					
					// last.fm image of the artist
					QList<QVariant> list2 = map3["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map5 = iter2.next().toMap();
						QUrl imageUrl(map5["#text"].toString());
						//artists.setImageUrl(imageUrl);
						break;
						}
					
					list.append(artist);
					
					if(gItemsPerPage == list.count())
						break;
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
		
		// Albums search
		else if ( SmfMusicSearchAlbum == aOperation )
			{
			qDebug()<<"Response for searching albums";
			
			QList<SmfAlbum> list;
			QString errStr;
			errStr.clear();
				
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
					
			qDebug()<<"Json parsing complete";
			
			if(response.contains(QByteArray("error")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1 = result["results"].toMap();
				QVariantMap map2 = map1["albummatches"].toMap();
				QList<QVariant> list1 = map2["album"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfAlbum album;
			
					QVariantMap map3 = iter.next().toMap();
					
					album.setName(map3["name"].toString());
					
					SmfArtists artists;
					QStringList names;
					names.append(map3["artist"].toString());
					artists.setNames(names);
					
					QList<QVariant> list2 = map3["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map5 = iter2.next().toMap();
						QUrl imageUrl(map5["#text"].toString());
						//artists.setImageUrl(imageUrl);
						break;
						}
					
					album.setArtists(artists);
					
					album.setId(map3["id"].toString());
					
					//QUrl url(map3["url"].toString())
					//album.setUrl(url);
					
					list.append(album);
					
					if(gItemsPerPage == list.count())
						break;
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
		
		// Events search
		else if (SmfMusicSearchEvent == aOperation)
			{
			qDebug()<<"Response for searching events";
			
			QList<SmfEvent> list;
			QString errStr;
			errStr.clear();
				
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
					
			qDebug()<<"Json parsing complete";
			
			if(response.contains(QByteArray("error")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1 = result["event"].toMap();
				SmfEvent event;
				
				// Set the events title
				event.setTitle(map1["title"].toString());
				
				// Set the events id
				event.setId(map1["id"].toString());
				
				// Set the events date and time
				QDateTime dateTime;// = QDateTime::fromString("M1d1y9800:01:02","'M'M'd'd'y'yyhh:mm:ss");
				event.setEventDateTime(dateTime);
				
				// Set the events artists
				SmfArtists artists;
				QStringList names;
				names.clear();
				QVariantMap map2 = map1["artists"].toMap();
				QList<QVariant> list1 = map2["artist"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					names.append(iter.next().toString());
				artists.setNames(names);
				event.setArtists(artists);
				
				// Set the events venue information
				QVariantMap map3 = map1["venue"].toMap();
				SmfLocation location;
				location.setName(map3["name"].toString());
				QUrl url(map3["url"].toString());
				location.setUrl(url);
				location.setId(map3["id"].toString());

				QVariantMap map4 = map3["location"].toMap();
				location.setCity(map4["city"].toString());
				location.setCountry(map4["country"].toString());
				location.setStreet(map4["street"].toString());
				location.setZipCode(map4["postalcode"].toString());

				QVariantMap map5 = map4["geo:point"].toMap();
				double latitude = map5["geo:lat"].toDouble();
				double longitude = map5["geo:long"].toDouble();
				QGeoCoordinate coordinate(latitude, longitude);
				QGeoPositionInfo geoInfo;
				geoInfo.setCoordinate(coordinate);
				location.setGeoPositionInfo(geoInfo);

				event.setVenue(location);
				
				// Set the events ticketUrl
				QUrl ticketUrl(map1["tickets"].toString());
				event.setTicketUrl(ticketUrl);
				
				// Set the events id
				event.setId(map1["id"].toString());
				
				list.append(event);
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
		
		// Venues Search
		else if(SmfMusicSearchVenue == aOperation)
			{
			qDebug()<<"Response for searching venues";
			
			QList<SmfLocation> list;
			QString errStr;
			errStr.clear();
				
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
					
			qDebug()<<"Json parsing complete";
			
			if(response.contains(QByteArray("error")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1 = result["results"].toMap();
				QVariantMap map2 = map1["venuematches"].toMap();
				QList<QVariant> list1 = map2["venue"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfLocation location;
					QVariantMap map3 = iter.next().toMap();
					
					location.setName(map3["name"].toString());
					QUrl url(map3["url"].toString());
					location.setUrl(url);
					location.setId(map3["id"].toString());
	
					QVariantMap map4 = map3["location"].toMap();
					location.setCity(map4["city"].toString());
					location.setCountry(map4["country"].toString());
					location.setStreet(map4["street"].toString());
					location.setZipCode(map4["postalcode"].toString());
	
					QVariantMap map5 = map4["geo:point"].toMap();
					double latitude = map5["geo:lat"].toDouble();
					double longitude = map5["geo:long"].toDouble();
					QGeoCoordinate coordinate(latitude, longitude);
					QGeoPositionInfo geoInfo;
					geoInfo.setCoordinate(coordinate);
					location.setGeoPositionInfo(geoInfo);
					
					list.append(location);
					
					if(gItemsPerPage == list.count())
						break;
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
		
#if 0
		// User search
		else if(SmfMusicSearchUser == aOperation)
			{
			qDebug()<<"Response for searching users";
			
			QList<SmfMusicProfile> list;
			QString errStr;
			errStr.clear();
			
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
			
			if(response.contains(QByteArray("error")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				qDebug()<<"For getting user's neighbours";
				
				QVariantMap map1 = result["neighbours"].toMap();
				QList<QVariant> list1 = map1["user"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfMusicProfile profile;
					QVariantMap map2 = iter.next().toMap();
					
/*					QContactName name;
					name.setFirstName(map2["name"].toString());
					QVariant nameVar = QVariant::fromValue<QContactName>(name);
					contact.setValue("Name", nameVar);
											
					QContactUrl url;
					url.setUrl(map2["url"].toString());
					QVariant urlVar = QVariant::fromValue<QContactUrl>(url);
					contact.setValue("Url", urlVar);
					
					QContactAvatar avatar;
					QList<QVariant> list2 = map2["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map3 = iter2.next().toMap();
						QUrl imageUrl(map3["#text"].toString());
						avatar.setImageUrl(imageUrl);
						break;
						}

					QVariant avatarVar = QVariant::fromValue<QContactAvatar>(avatar);
					contact.setValue("Avatar", avatarVar);*/
					
					profile.setId(map2["name"].toString());
					
					list.append(profile);
					
					if(gItemsPerPage == list.count())
						break;
					
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
#endif
		
		else
			{
			qDebug()<<"Service unsupported!!!";
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
LastFmMusicServiceProviderBase::~LastFmMusicServiceProviderBase( )
	{
	}


/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString LastFmMusicServiceProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}


/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage LastFmMusicServiceProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}


/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString LastFmMusicServiceProviderBase::description( ) const
	{
	return m_description;
	}


/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl LastFmMusicServiceProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}


/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl LastFmMusicServiceProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}


/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage LastFmMusicServiceProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> LastFmMusicServiceProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList LastFmMusicServiceProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString LastFmMusicServiceProviderBase::pluginId( ) const
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
QString LastFmMusicServiceProviderBase::authenticationApp( QString &aProgram, 
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
QString LastFmMusicServiceProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}


/**
 * Method that initializes this class. This method should be called 
 * from the initialize() method of the FBContactFetcherPlugin class
 */
void LastFmMusicServiceProviderBase::initialize()
	{
	m_serviceName = "last.fm";
	m_description = "Last.fm music service plugin description";
	m_serviceUrl = QUrl(QString("http://www.last.fm"));
	m_pluginId = "lastfmmusicserviceplugin.qtplugin";
	m_authAppId = "0x12345678";
	m_supportedInterfaces.append("org.symbian.smf.plugin.music.service/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("LastFmRegToken").toString();
	m_validity = iSettings.value("LastFmExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : LastFmMusicServicePlugin
 * plugin class : LastFmMusicServicePlugin
 */
Q_EXPORT_PLUGIN2( lastfmmusicserviceplugin, LastFmMusicServicePlugin )
