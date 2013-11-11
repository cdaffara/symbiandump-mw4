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
 * The Plugin that does music search related functionalities from last.fm site
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
#include <qlocale.h>
#include <smfprovider.h>

#include "lastfmmusicsearchplugin.h"

static int gPageNum = 0; 
static int gItemsPerPage = 0;
static quint8 forTracksOfAlbum = 0;
QString albumId;

int gOperationId;


/**
 * Destructor
 */
LastFmMusicSearchPlugin::~LastFmMusicSearchPlugin( )
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
void LastFmMusicSearchPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aToken )
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
	
	qDebug()<<"Api Key = "<<aApiKey;
	qDebug()<<"Api Secret = "<<aApiSecret;
	qDebug()<<"Token = "<<aToken;
	}

/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString LastFmMusicSearchPlugin::generateSignature(const QString aBaseString)
	{
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}

/**
 * Method to get the current country name as per ISO 3166-1 standard.
 * @return Current country name
 */
QString LastFmMusicSearchPlugin::currentCountryName() const
	{
// ToDo :- complete remaining country code and names
	QLocale locale;
	QLocale::Country country = locale.country();

	switch(country)
		{
		case 0:
			return QString();
		case 1:
			return QString("AFGHANISTAN");
			// ToDo :- fill for other countries
		case 100:
			return QString("INDIA");
		case 224: // UK
			return QString("UNITED KINGDOM");
		default:
			return QString();
		}
	}

/**
 * Method to get recommended tracks
 * @param aRequest [out] The request data to be sent to network
 * @param aTrack The track for which similar recommendations 
 * need to be fetched.
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::recommendations( SmfPluginRequestData &aRequest,
		const SmfTrackInfo &aTrack,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aTrack)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	qDebug()<<"Inside LastFmMusicSearchPlugin::recommendations()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	
#if 0 // recommended artist not present in music.search interface
	qDebug()<<"Inside LastFmMusicSearchPlugin::recommendations()";
		
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
	fetchKeys(apiKey, apiSecret, token);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("methoduser.getRecommendedArtists");
	baseString.append("sk"+token);
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "user.getRecommendedArtists");
	url.addQueryItem("sk", token);
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetRecommendations;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
#endif
	}

/**
 * Method to search for tracks similar to a given track
 * @param aRequest [out] The request data to be sent to network
 * @param aTrack The track for which similar tracks 
 * need to be fetched.
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::tracksSimilar( SmfPluginRequestData &aRequest,
		const SmfTrackInfo &aTrack,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	// Note :
	// only mbid won't work
	// only artist won't work
	// only track wont work
	// a combination of artist mbid and track name won't work
	// Give a combination of artist name and track name for a valid response
	qDebug()<<"Inside LastFmMusicSearchPlugin::tracksSimilar()";
	qDebug()<<"Page num = "<<aPageNum;
	qDebug()<<"item per Page = "<<aItemsPerPage;
	
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || (0 == aTrack.artists().names().count()) || 
			aTrack.title().isEmpty() )
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
	fetchKeys(apiKey, apiSecret, token);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("artist"+aTrack.artists().names().at(0));	// artist name
	baseString.append("methodtrack.getSimilar");
	baseString.append("track"+aTrack.title());	// track name
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("artist", aTrack.artists().names().at(0));
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "track.getSimilar");
	url.addQueryItem("track", aTrack.title());
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetTracksSimilar;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search for tracks of a given album
 * @param aRequest [out] The request data to be sent to network
 * @param aAlbum The album whose tracks need to be fetched.
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::tracksOfAlbum( SmfPluginRequestData &aRequest,
		const SmfAlbum &aAlbum,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::tracksOfAlbum()";
	qDebug()<<"Page num = "<<aPageNum;
	qDebug()<<"item per Page = "<<aItemsPerPage;
	
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || aAlbum.name().isEmpty() 
			|| aAlbum.artists().names().at(0).isEmpty() )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";
	
	gPageNum = aPageNum;
	gItemsPerPage = aItemsPerPage;
	
	if(0 == forTracksOfAlbum)
		return getAlbumId(aRequest, aAlbum);
	else //if(1 == forTracksOfAlbum)
		return getTracksOfAlbum(aRequest, aAlbum);
	}

/**
 * Method to get the last.fm specific ID of the given album
 * @param aRequest [out] The request data to be sent to network
 * @param aAlbum The album whose tracks need to be fetched.
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::getAlbumId( SmfPluginRequestData &aRequest,
		const SmfAlbum &aAlbum )
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::getAlbumId()";

	SmfPluginError error = SmfPluginErrNone;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	fetchKeys(apiKey, apiSecret, token);
	
	// Create the API signature string
	QString baseString;
	baseString.append("album"+aAlbum.name());	// album name
	baseString.append("api_key"+apiKey);
	baseString.append("artist"+aAlbum.artists().names().at(0));	// artist name
	baseString.append("methodalbum.getInfo");
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("album", aAlbum.name());
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("artist", aAlbum.artists().names().at(0));
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "album.getInfo");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetTracksOfAlbum;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search for tracks of a given album using its ID in last.fm
 * @param aRequest [out] The request data to be sent to network
 * @param aAlbum The album whose tracks need to be fetched.
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::getTracksOfAlbum( SmfPluginRequestData &aRequest,
		const SmfAlbum &aAlbum)
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::getTracksOfAlbum()";
	Q_UNUSED(aAlbum)
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	fetchKeys(apiKey, apiSecret, token);
	
	QString playlistUrl("lastfm://playlist/album/");
	playlistUrl.append(albumId);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("methodplaylist.fetch");
	baseString.append("playlistURL"+playlistUrl);
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "playlist.fetch");
	url.addQueryItem("playlistURL", playlistUrl);
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetTracksOfAlbum;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search for tracks of the given artist(s)
 * @param aRequest [out] The request data to be sent to network
 * @param aArtist The artist(s) whose tracks need to be fetched.
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::tracksOfArtist( SmfPluginRequestData &aRequest,
		const SmfArtists &aArtist,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::tracksOfArtist()";
	qDebug()<<"Page num = "<<aPageNum;
	qDebug()<<"item per Page = "<<aItemsPerPage;
	
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
	fetchKeys(apiKey, apiSecret, token);
	
	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("artist"+aArtist.names().at(0));	// artist name
	baseString.append("methodartist.getTopTracks");
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("artist", aArtist.names().at(0));
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "artist.getTopTracks");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetTracksOfArtist;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get tracks having a similar finger print
 * @param aRequest [out] The request data to be sent to network
 * @param aSignature The finger print to be searched for need to be 
 * fetched.
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::trackInfo( SmfPluginRequestData &aRequest,
		const SmfMusicFingerPrint &aSignature,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::trackInfo()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || aSignature.id().isEmpty() ) 
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
	fetchKeys(apiKey, apiSecret, token);

	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("fingerprintid"+aSignature.id());
	baseString.append("methodtrack.getFingerprintMetadata");
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("fingerprintid", aSignature.id());
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "track.getFingerprintMetadata");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetTrackInfo;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to search information about where to buy this song from
 * @param aRequest [out] The request data to be sent to network
 * @param aTrack The track for which stores need to be searched
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::stores( SmfPluginRequestData &aRequest,
		const SmfTrackInfo &aTrack,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::stores()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || (0 == aTrack.artists().names().count()) || 
			aTrack.album().name().isEmpty() )
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
	fetchKeys(apiKey, apiSecret, token);

	QString countryName = currentCountryName();
	
	// Create the API signature string
	QString baseString;
	baseString.append("album"+aTrack.album().name());
	baseString.append("api_key"+apiKey);
	baseString.append("artist"+aTrack.artists().names().at(0));
	baseString.append("country"+countryName);
	baseString.append("methodalbum.getBuylinks");
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("album", aTrack.album().name());
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("artist", aTrack.artists().names().at(0));
	url.addQueryItem("country", countryName);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "album.getBuylinks");
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetStores;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Customised method for SmfMusicSearchPlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError LastFmMusicSearchPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	qDebug()<<"Inside LastFmMusicSearchPlugin::customRequest()";
			
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( NULL == aData )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	qDebug()<<"Valid arguments";

	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString token;
	fetchKeys(apiKey, apiSecret, token);


	if(107 == aOperation)
		{
		gOperationId = aOperation;
		int limit;
		int pageNum;
		SmfAlbum album;
		
		QDataStream read(aData, QIODevice::ReadOnly);
		read>>limit;
		read>>pageNum;
		read>>album;
		
		gItemsPerPage = limit;
		gPageNum = pageNum;
		
		// Create the API signature string
		QString baseString;
		baseString.append("album"+album.name());
		baseString.append("api_key"+apiKey);
		baseString.append("limit"+QString::number(limit));
		baseString.append("methodalbum.search");
		baseString.append("page"+QString::number(pageNum));
		baseString.append(apiSecret);
	
		// Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("album", album.name());
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("limit", QString::number(limit));
		url.addQueryItem("page", QString::number(pageNum));
		url.addQueryItem("format", "json");
		url.addQueryItem("method", "album.search");
		url.addQueryItem("api_sig", generateSignature(baseString));
		
		// Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfMusicSearchCustomRequest;
		aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
	
		qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
		}
	return error;
	}

/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void LastFmMusicSearchPlugin::initialize( )
	{
	// Create an instance of LastFmMusicSearchProviderBase
	m_provider = new LastFmMusicSearchProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* LastFmMusicSearchPlugin::getProviderInfo( )
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
 * For SmfMusicSearchPlugin: If last operation was recommendations(), 
 * tracksSimilar() or tracksOfAlbum() or tracksOfArtist() or trackInfo(), 
 * aResult will be of type QList<SmfTrackInfo>. 
 * If last operation was stores(), aResult will be of type QList<SmfProvider>.
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError LastFmMusicSearchPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	Q_UNUSED(aPageResult)
	qDebug()<<"Inside LastFmMusicSearchPlugin::responseAvailable()";
	
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	QFile respFile("c://data//SmfMusicSearchPluginResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		qDebug()<<"File to write the response could not be opened";
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfMusicSearchPluginResponse.txt'";
		}
	qDebug()<<"FB response size = "<<response.size();
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";

		if((SmfMusicGetTracksSimilar == aOperation) || (SmfMusicGetTracksOfArtist == aOperation))
			{
			qDebug()<<"Response for music search get similar tracks/artist's tracks";
			
			QList<SmfTrackInfo> list;
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
				QVariantMap map1;
				if(SmfMusicGetTracksSimilar == aOperation)
					map1 = result["similartracks"].toMap();
				else //if(SmfMusicGetTracksOfArtist == aOperation) // tracks of artists
					map1 = result["toptracks"].toMap();
				QList<QVariant> list1 = map1["track"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfTrackInfo track;

					QVariantMap map2 = iter.next().toMap();
					qDebug()<<"track title = "<<map2["name"].toString();
					if(SmfMusicGetTracksSimilar == aOperation)
						qDebug()<<"duration in milli sec = "<<map2["duration"].toString();
					
					// Set the track title
					track.setTitle(map2["name"].toString());
					
					// Set the tracks duration in seconds
					if(SmfMusicGetTracksSimilar == aOperation)
						{
						int timeValInSec = map2["duration"].toInt();
						timeValInSec /= 1000;
						QString str = QString::number(timeValInSec);
						str.prepend("0.");
						QTime time = QTime::fromString(str, "m.z");
						qDebug()<<" Time value = "<<time;
						track.setDuration(time);
						}
					
					// Set the track's Id - last.fm's mbid
					track.setId(map2["mbid"].toString());
					
					// Set the tracks artist details
					QVariantMap map3 = map2["artist"].toMap();
					SmfArtists artists;
					
					// Set the artist's name
					QStringList namesList;
					namesList.append(map3["name"].toString());
					artists.setNames(namesList);
					
					// Set the artist's url
					QUrl url(map3["url"].toString());
					artists.setUrl(url);
					
					// Set the artist's id
					artists.setId(map3["mbid"].toString());
					
					// Set the artist's image url
					QList<QVariant> list2 = map3["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map4 = iter.next().toMap();
						
						// Set the artist's image url
						QUrl url(map4["#text"].toString());
						//artists.setImageUrlurl);
						break;
						}
					
					track.setArtists(artists);
					
					list.append(track);
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
		else if (SmfMusicGetTracksOfAlbum == aOperation)
			{
			qDebug()<<"Response for music search tracks of album";
			
			if(0 == forTracksOfAlbum) // for fetching album ID
				{
				qDebug()<<"FOR GETTING ALBUMID";
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
					QVariantMap map1 = result["album"].toMap();
					albumId.clear();
					albumId.append(map1["id"].toString());
					forTracksOfAlbum = 1;
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
					qDebug()<<"album id = "<<albumId;
					aRetType = SmfSendRequestAgain;
					error = SmfPluginErrNone;
					}
				}
			else	// for fetching tracks of the album
				{
				qDebug()<<"FOR GETTING TRACKS OF ALBUM";
				QList<SmfTrackInfo> list;
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
					QVariantMap map1 = result["playlist"].toMap();
					QVariantMap map2 = map1["trackList"].toMap();
					QList<QVariant> list1 = map2["track"].toList();
					QListIterator<QVariant> iter(list1);
					while(iter.hasNext())
						{
						SmfTrackInfo track;
	
						QVariantMap map3 = iter.next().toMap();
						qDebug()<<"track title = "<<map3["title"].toString();
						
						// Set the track's Id - last.fm's identifier
						track.setId(map3["identifier"].toString());
						
						// Set the track title
						track.setTitle(map3["title"].toString());
						
						// Set the tracks album
						SmfAlbum album;
						album.setName(map3["album"].toString());
						track.setAlbum(album);
						
						// Set the tracks artist details
						SmfArtists artists;
						QStringList namesList;
						namesList.append(map3["creator"].toString());
						artists.setNames(namesList);
						
						QUrl url(map3["info"].toString());
						artists.setUrl(url);
						
						//QUrl imageUrl(map3["image"].toString());
						//artists.setImageUrl(imageUrl);
						
						track.setArtists(artists);
						
						// Set the tracks duration in seconds
						int timeValInSec = map3["duration"].toInt();
						timeValInSec /= 1000;
						QTime time = QTime::fromString(QString::number(timeValInSec), "z");
						track.setDuration(time);
	
						list.append(track);
						
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
			}
		else if(SmfMusicGetStores == aOperation)
			{
			qDebug()<<"Response for music search get stores";
			
			QList<SmfProvider> list;
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
				QVariantMap map1 = result["affiliations"].toMap();
				QVariantMap map2 = map1["physicals"].toMap();
				QList<QVariant> list1 = map2["affiliation"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfProvider provider;
	
					QVariantMap map3 = iter.next().toMap();
					qDebug()<<"provider name = "<<map3["supplierName"].toString();
					qDebug()<<"provider URL = "<<map3["buyLink"].toString();
					
					// Set the provider name
					QString str(map3["supplierName"].toString());
					provider.setServiceName(str);
					
					// Set the provider url
					QUrl url(map3["buyLink"].toString());
					provider.setServiceUrl(url);
					
					// Set the provider icon
					//QUrl url(map3["supplierIcon"].toString());
					//provider.setServiceIcon(url);
					
					list.append(provider);
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
		
		else if (SmfMusicGetTrackInfo == aOperation)
			{
			qDebug()<<"Response for music search get fingerprint tracks";
			
			QList<SmfTrackInfo> list;
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
				QVariantMap map1 = result["tracks"].toMap();
				QList<QVariant> list1 = map1["track"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfTrackInfo track;
		
					QVariantMap map3 = iter.next().toMap();
					
					// Set the track's Id - last.fm's identifier
					track.setId(map3["mbid"].toString());
					
					// Set the track title
					track.setTitle(map3["name"].toString());
					
					// Set the tracks artist details
					QVariantMap map4 = map3["artist"].toMap();
					SmfArtists artists;
					QStringList namesList;
					namesList.append(map4["name"].toString());
					artists.setNames(namesList);
					
					QUrl url(map4["url"].toString());
					artists.setUrl(url);
					
					artists.setId(map4["mbid"].toString());
					
					QList<QVariant> list2 = map4["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map5 = iter2.next().toMap();
						QUrl imageUrl(map5["#text"].toString());
						//artists.setImageUrl(imageUrl);
						break;
						}
								
					track.setArtists(artists);
					
					list.append(track);
					
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
		
		else if (SmfMusicSearchCustomRequest == aOperation)
			{
			qDebug()<<"Response for music search custom request = album.search";
			
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
				QByteArray customResponseData;
				QByteArray entireData;
				QDataStream writeResponse(&customResponseData, QIODevice::WriteOnly);
				qDebug()<<"list count = "<<list.count();			
				writeResponse<<list;
				
				QDataStream writeFull(&entireData, QIODevice::WriteOnly);
				qDebug()<<"operation id = "<<gOperationId;
				writeFull<<gOperationId;
				qDebug()<<"custom Data size = "<<customResponseData.size();
				writeFull<<customResponseData;

				aResult->setValue(entireData);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
			}
		
		else
			{
			qDebug()<<"Service unsupported!!! ="<<aOperation;
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
LastFmMusicSearchProviderBase::~LastFmMusicSearchProviderBase( )
	{
	}


/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString LastFmMusicSearchProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}


/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage LastFmMusicSearchProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}


/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString LastFmMusicSearchProviderBase::description( ) const
	{
	return m_description;
	}


/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl LastFmMusicSearchProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}


/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl LastFmMusicSearchProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}


/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage LastFmMusicSearchProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> LastFmMusicSearchProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList LastFmMusicSearchProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString LastFmMusicSearchProviderBase::pluginId( ) const
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
QString LastFmMusicSearchProviderBase::authenticationApp( QString &aProgram, 
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
QString LastFmMusicSearchProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}


/**
 * Method that initializes this class. This method should be called 
 * from the initialize() method of the FBContactFetcherPlugin class
 */
void LastFmMusicSearchProviderBase::initialize()
	{
	m_serviceName = "last.fm";
	m_description = "Last.fm music search plugin description";
	m_serviceUrl = QUrl(QString("http://www.last.fm"));
	m_pluginId = "lastfmmusicsearchplugin.qtplugin";
	m_authAppId = "0x12345678";
	m_supportedInterfaces.append("org.symbian.smf.plugin.music.search/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("LastFmRegToken").toString();
	m_validity = iSettings.value("LastFmExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : lastfmmusicsearchplugin
 * plugin class : LastFmMusicSearchPlugin
 */
Q_EXPORT_PLUGIN2( lastfmmusicsearchplugin, LastFmMusicSearchPlugin )
