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
 * Sangeeta Prasad, Nalina Hariharan
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

#include "lastfmplaylistserviceplugin.h"


static int gPageNum = 0; 
static int gItemsPerPage = 0;

//Payload data array
QByteArray payload;

/**
 * Destructor
 */
LastFmPlaylistServicePlugin::~LastFmPlaylistServicePlugin( )
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
 * @param aName [out] The user name
 */
void LastFmPlaylistServicePlugin::fetchKeys( QString &aApiKey, 
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
QString LastFmPlaylistServicePlugin::generateSignature(const QString aBaseString)
	{
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}

/**
 * Method to get the playlist
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum [in] The page to be extracted
 * @param aItemsPerPage [in] Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmPlaylistServicePlugin::playlists( 
		SmfPluginRequestData &aRequest,
		const int aPageNum,
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmPlaylistServicePlugin::playlists()";
				
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 )
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
	baseString.append("methoduser.getPlaylists");
	baseString.append("user"+userName);
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "user.getPlaylists");
	url.addQueryItem("user", userName);
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetPlaylists;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;
	
	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get the playlist of a particular user
 * @param aRequest [out] The request data to be sent to network
 * @param aUser [in] The user whose playlists need to be fetched
 * @param aPageNum [in] The page to be extracted
 * @param aItemsPerPage [in] Number of items per page
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmPlaylistServicePlugin::playlistsOf( 
		SmfPluginRequestData &aRequest,
		const SmfContact &aUser,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside LastFmPlaylistServicePlugin::playlistsOf()";
					
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 || 
			aUser.value("Name").value<QContactName>().firstName().isEmpty() )
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
	
	userName.clear();
	userName = aUser.value("Name").value<QContactName>().firstName();
		
	// Create the API signature string
	QString baseString;
	baseString.append("api_key"+apiKey);
	baseString.append("methoduser.getPlaylists");
	baseString.append("user"+userName);
	baseString.append(apiSecret);

	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("format", "json");
	url.addQueryItem("method", "user.getPlaylists");
	url.addQueryItem("user", userName);
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfMusicGetPlaylistsOfUser;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}
	
/**
 * Method to add tracks to a playlist
 * @param aRequest [out] The request data to be sent to network
 * @param aPlaylist [in] The playlist where tracks should be added
 * @param aTracks [in] The tracks to be added to the playlist
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmPlaylistServicePlugin::addToPlaylist( 
		SmfPluginRequestData &aRequest,
		const SmfPlaylist &aPlaylist, 
		const QList<SmfTrackInfo> &aTracks )
	{
	qDebug()<<"Inside LastFmPlaylistServicePlugin::addToPlaylist()";
					
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
	if( aPlaylist.id().isEmpty() || (0 == aTracks.count()) )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	else if(aTracks.at(0).title().isEmpty() || aTracks.at(0).artists().names().at(0).isEmpty())
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	else
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
	baseString.append("artist"+aTracks.at(0).artists().names().at(0));
	baseString.append("methodplaylist.addTrack");
	baseString.append("playlistID"+aPlaylist.id());
	baseString.append("sk"+token);
	baseString.append("track"+aTracks.at(0).title());
	baseString.append(apiSecret);

	QUrl postData;
	postData.addQueryItem("api_key", apiKey);
	postData.addQueryItem("artist", aTracks.at(0).artists().names().at(0));
	postData.addQueryItem("format","json");
	postData.addQueryItem("method", "playlist.addTrack");
	postData.addQueryItem("playlistID",aPlaylist.id());
	postData.addQueryItem("sk",token);
	postData.addQueryItem("track",aTracks.at(0).title());
	postData.addQueryItem("api_sig", generateSignature(baseString));
		
	QString data(postData.toString());
	data.remove(0, 1); // Remove the first ?
			
	payload.clear();
	payload.append(data.toAscii());
	
	qDebug()<<"Data = "<<QString(payload);
	
	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/");
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iNetworkRequest.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	aRequest.iRequestType = SmfMusicAddToPlaylist;
	aRequest.iPostData = new QBuffer(&payload);
	aRequest.iHttpOperationType = QNetworkAccessManager::PostOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}
	
/**
 * Method to post the current playing playlist
 * @param aRequest [out] The request data to be sent to network
 * @param aPlaylist [in] The current playing playlist which should be posted
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmPlaylistServicePlugin::postCurrentPlayingPlaylist(
		SmfPluginRequestData &aRequest, 
		const SmfPlaylist &aPlaylist )
	{
	qDebug()<<"Inside LastFmPlaylistServicePlugin::postCurrentPlayingPlaylist()";
					
	SmfPluginError error = SmfPluginErrInvalidArguments;

	// invalid arguments
/*	if( aPlaylist.playListTitle().isEmpty() )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}*/

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
	baseString.append("methodplaylist.create");
	baseString.append("sk"+token);
	if( !aPlaylist.playListTitle().isEmpty()) //you can create a playlist without name also
		baseString.append("title"+aPlaylist.playListTitle());
	baseString.append(apiSecret);

	QUrl postData;
	postData.addQueryItem("api_key", apiKey);
	postData.addQueryItem("format","json");
	postData.addQueryItem("method", "playlist.create");
	postData.addQueryItem("sk",token);
	if( !aPlaylist.playListTitle().isEmpty())
		postData.addQueryItem("title",aPlaylist.playListTitle());
	postData.addQueryItem("api_sig", generateSignature(baseString));
		
	QString data(postData.toString());
	data.remove(0, 1);
			
	payload.clear();
	payload.append(data.toAscii());
	
	qDebug()<<"Data = "<<QString(payload);
	
	// Create the url
	QUrl url("http://ws.audioscrobbler.com/2.0/");
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iNetworkRequest.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	aRequest.iRequestType = SmfMusicPostCurrentPlayingPlaylist;
	aRequest.iPostData = new QBuffer(&payload);
	aRequest.iHttpOperationType = QNetworkAccessManager::PostOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}
	
/**
 * Customised method for SmfPlaylistServicePlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation [in] The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData [in] The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return Appropriate value of the enum SmfPluginError.
 * Plugin error if any, else SmfPluginErrNone for success
 */
SmfPluginError LastFmPlaylistServicePlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	qDebug()<<"Inside LastFmPlaylistServicePlugin::customRequest()";
	SmfPluginError error = SmfPluginErrInvalidRequest;
	return error;
	}
	
/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void LastFmPlaylistServicePlugin::initialize( )
	{
	// Create an instance of LastFmMusicServiceProviderBase
	m_provider = new LastFmPlaylistServiceProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* LastFmPlaylistServicePlugin::getProviderInfo( )
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
SmfPluginError LastFmPlaylistServicePlugin::responseAvailable(
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult,
		QByteArray *aResponse,
		QVariant* aResult,
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{

	Q_UNUSED(aPageResult)
	qDebug()<<"Inside LastFmPlaylistServicePlugin::responseAvailable()";
	
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	QFile respFile("c://data//SmfMusicPlaylistPluginResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		qDebug()<<"File to write the response could not be opened";
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfMusicPlaylistPluginResponse.txt'";
		}
	qDebug()<<"FB response size = "<<response.size();
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";
		
		// Get user's playlists
		if(SmfMusicGetPlaylists == aOperation)
			{
			qDebug()<<"Response for getting user's playlist";
			QList<SmfPlaylist> list;				
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
				QVariantMap map1 = result["playlists"].toMap();
				QList<QVariant> list1 = map1["playlist"].toList();
				QListIterator<QVariant> iter1(list1);
				bool ifList = false;
				while(iter1.hasNext())
					{
					ifList = true;
					QVariantMap map2 = iter1.next().toMap();
					SmfPlaylist playlist;
					
					playlist.setPlayListTitle(map2["title"].toString());
					playlist.setId(map2["id"].toString());
					playlist.setAuthor(map2["creator"].toString());
					QUrl url(map2["url"].toString());
					playlist.setInfo(url);
					playlist.setLocation(url);
					
					
					QList<QVariant> list2 = map2["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map3 = iter2.next().toMap();
						QUrl imageUrl(map3["#text"].toString());
						playlist.setImage(imageUrl);
						}
					
					list.append((playlist));
					
					if(gItemsPerPage == list.count())
						break;
					}
				if(!ifList)
					{
					QVariantMap map2 = map1["playlist"].toMap();
					SmfPlaylist playlist;
					
					playlist.setPlayListTitle(map2["title"].toString());
					playlist.setId(map2["id"].toString());
					playlist.setAuthor(map2["creator"].toString());
					QUrl url(map2["url"].toString());
					playlist.setInfo(url);
					playlist.setLocation(url);
					
					
					QList<QVariant> list2 = map2["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map3 = iter2.next().toMap();
						QUrl imageUrl(map3["#text"].toString());
						playlist.setImage(imageUrl);
						}
					
					list.append((playlist));
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
					qDebug()<<"Count of user's recent tracks  = "<<list.count();
					aResult->setValue(list);
					aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;
					}
				}
			}
		
		// Playlists of another user
		else if ( SmfMusicGetPlaylistsOfUser == aOperation )
			{
			qDebug()<<"Response for getting another user's playlist";
			QList<SmfPlaylist> list;				
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
				QVariantMap map1 = result["playlists"].toMap();
				QList<QVariant> list1 = map1["playlist"].toList();
				QListIterator<QVariant> iter1(list1);
				bool ifList = false;
				while(iter1.hasNext())
					{
					ifList = true;
					QVariantMap map2 = iter1.next().toMap();
					SmfPlaylist playlist;
					
					playlist.setPlayListTitle(map2["title"].toString());
					playlist.setId(map2["id"].toString());
					playlist.setAuthor(map2["creator"].toString());
					QUrl url(map2["url"].toString());
					playlist.setInfo(url);
					playlist.setLocation(url);
					
					
					QList<QVariant> list2 = map2["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map3 = iter2.next().toMap();
						QUrl imageUrl(map3["#text"].toString());
						playlist.setImage(imageUrl);
						}
					
					list.append((playlist));
					
					if(gItemsPerPage == list.count())
						break;
					}
				if(!ifList)
					{
					QVariantMap map2 = map1["playlist"].toMap();
					SmfPlaylist playlist;
					
					playlist.setPlayListTitle(map2["title"].toString());
					playlist.setId(map2["id"].toString());
					playlist.setAuthor(map2["creator"].toString());
					QUrl url(map2["url"].toString());
					playlist.setInfo(url);
					playlist.setLocation(url);
					
					
					QList<QVariant> list2 = map2["image"].toList();
					QListIterator<QVariant> iter2(list2);
					while(iter2.hasNext())
						{
						QVariantMap map3 = iter2.next().toMap();
						QUrl imageUrl(map3["#text"].toString());
						playlist.setImage(imageUrl);
						}
					
					list.append((playlist));
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
					qDebug()<<"Count of friend's playlists  = "<<list.count();
					aResult->setValue(list);
					aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;
					}
				}
			}
		
		// Adding track to playlist AND
		// Post current playing playlist
		else if ((SmfMusicAddToPlaylist == aOperation) || (SmfMusicPostCurrentPlayingPlaylist == aOperation))
			{
			qDebug()<<"Response for adding track to playlist";
			bool reqSuccess = false;		
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
				reqSuccess = true;
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
				qDebug()<<"Track added to playlist?? "<<reqSuccess;
				aResult->setValue(reqSuccess);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
			}
		
		// Other Service requests - NOT SUPPORTED
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
LastFmPlaylistServiceProviderBase::~LastFmPlaylistServiceProviderBase( )
	{
	}

/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString LastFmPlaylistServiceProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}


/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage LastFmPlaylistServiceProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}


/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString LastFmPlaylistServiceProviderBase::description( ) const
	{
	return m_description;
	}


/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl LastFmPlaylistServiceProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}


/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl LastFmPlaylistServiceProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}


/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage LastFmPlaylistServiceProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> LastFmPlaylistServiceProviderBase::supportedInterfaces( ) const
	{
	return m_supportedInterfaces;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList LastFmPlaylistServiceProviderBase::supportedLanguages( ) const
	{
	return m_supportedLangs;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString LastFmPlaylistServiceProviderBase::pluginId( ) const
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
QString LastFmPlaylistServiceProviderBase::authenticationApp( QString &aProgram, 
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
QString LastFmPlaylistServiceProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}


/**
 * Method that initializes this class. This method should be called 
 * from the initialize() method of the FBContactFetcherPlugin class
 */
void LastFmPlaylistServiceProviderBase::initialize()
	{
	m_serviceName = "last.fm";
	m_description = "Last.fm music playlist plugin description";
	m_serviceUrl = QUrl(QString("http://www.last.fm"));
	m_pluginId = "lastfmplaylistserviceplugin.qtplugin";
	m_authAppId = "0x12345678";
	m_supportedInterfaces.append("org.symbian.smf.plugin.music.playlist/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("LastFmRegToken").toString();
	m_validity = iSettings.value("LastFmExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : lastfmplaylistserviceplugin
 * plugin class : LastFmPlaylistServicePlugin
 */
Q_EXPORT_PLUGIN2( lastfmplaylistserviceplugin, LastFmPlaylistServicePlugin )
