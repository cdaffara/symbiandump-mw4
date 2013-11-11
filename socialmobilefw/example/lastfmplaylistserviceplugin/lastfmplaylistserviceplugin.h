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
 * The Plugin that does music playlist services related 
 * functionalities from last.fm site
 *
 */

#ifndef _LASTFMPLAYLISTSERVICEPLUGIN_H_
#define _LASTFMPLAYLISTSERVICEPLUGIN_H_

//Include files
#include <smfplaylistserviceplugin.h>

// Forward declarations
class LastFmPlaylistServiceProviderBase;
class QVariant;

// Class declaration
class LastFmPlaylistServicePlugin : public QObject, public SmfPlaylistServicePlugin
{
	Q_OBJECT
	Q_INTERFACES( SmfPlaylistServicePlugin SmfPluginBase )

public:
	/**
	 * Destructor
	 */
	virtual ~LastFmPlaylistServicePlugin ( );
	
public: // From SmfPlaylistServicePlugin
	/**
	 * Method to get the playlist
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	SmfPluginError playlists( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to get the playlist of a particular user
	 * @param aRequest [out] The request data to be sent to network
	 * @param aUser [in] The user whose playlists need to be fetched
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	SmfPluginError playlistsOf( SmfPluginRequestData &aRequest,
			const SmfContact &aUser,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to add tracks to a playlist
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPlaylist [in] The playlist where tracks should be added
	 * @param aTracks [in] The tracks to be added to the playlist
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	SmfPluginError addToPlaylist( SmfPluginRequestData &aRequest,
			const SmfPlaylist &aPlaylist, 
			const QList<SmfTrackInfo> &aTracks );
	
	/**
	 * Method to post the current playing playlist
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPlaylist [in] The current playing playlist which should be posted
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	SmfPluginError postCurrentPlayingPlaylist(
			SmfPluginRequestData &aRequest, 
			const SmfPlaylist &aPlaylist );
	
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
	SmfPluginError customRequest( SmfPluginRequestData &aRequest, 
			const int &aOperation, QByteArray *aData );
	
public: // From SmfPluginBase interface
	/**
	 * The first method to be called in the plugin that implements this interface.
	 * If this method is not called, plugin may not behave as expected.
	 */
	void initialize( );

	/**
	 * Method to get the provider information
	 * @return Instance of SmfProviderBase
	 */
	SmfProviderBase* getProviderInfo( );

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
	SmfPluginError responseAvailable(
			const SmfRequestTypeID aOperation,
			const SmfTransportResult &aTransportResult,
			QByteArray *aResponse,
			QVariant* aResult,
			SmfPluginRetType &aRetType,
			SmfResultPage &aPageResult );
	
private:
	/**
	 * Method called by plugins to generate a signature string from a base string
	 * @param aBaseString The base string
	 * @return The md5 hash of the base string
	 */
	QString generateSignature( const QString aBaseString );

	/**
	 * Method to interpret the key sets obtained from credential manager 
	 * @param aApiKey [out] The api key
	 * @param aApiSecret [out] The api secret
	 * @param aSessionKey [out] The session key
	 * @param aToken [out] The session token
	 * @param aName [out] The user name
	 */
	void fetchKeys( QString &aApiKey, 
			QString &aApiSecret, 
			QString &aToken,
			QString &aName );

private:
	LastFmPlaylistServiceProviderBase *m_provider;
	};

/**
 * The Plugin class that implements SmfProviderBase for this last.fm plugin
 */
class LastFmPlaylistServiceProviderBase : public QObject, public SmfProviderBase
	{
	Q_OBJECT
	Q_INTERFACES( SmfProviderBase )

public:
	/**
	 * Destructor
	 */
	virtual ~LastFmPlaylistServiceProviderBase( );

	/**
	 * Method to get the Localisable name of the service.
	 * @return The Localisable name of the service.
	 */
	QString serviceName( ) const;

	/**
	 * Method to get the Logo of the service
	 * @return The Logo of the service
	 */
	QImage serviceIcon( ) const;

	/**
	 * Method to get the Readable service description
	 * @return The Readable service description
	 */
	QString description( ) const;

	/**
	 * Method to get the Website of the service
	 * @return The Website of the service
	 */
	QUrl serviceUrl( ) const;

	/**
	 * Method to get the URL of the Application providing this service
	 * @return The URL of the Application providing this service
	 */
	QUrl applicationUrl( ) const;

	/**
	 * Method to get the Icon of the application
	 * @return The Icon of the application
	 */
	QImage applicationIcon( ) const;

	/**
	* Method to get the list of interfaces that this provider support
	* @return List of supported Interafces
	*/
	QList<QString> supportedInterfaces( ) const;

	/**
	* Method to get the list of languages supported by this service provider
	* @return a QStringList of languages supported by this service
	* provider in 2 letter ISO 639-1 format.
	*/
	QStringList supportedLanguages( ) const;

	/**
	 * Method to get the Plugin specific ID
	 * @return The Plugin specific ID
	 */
	QString pluginId( ) const;

	/**
	 * Method to get the ID of the authentication application
	 * for this service
	 * @param aProgram The authentication application name
	 * @param aArguments List of arguments required for authentication app
	 * @param aMode Strting mode for authentication application
	 * @return The ID of the authentication application
	 */
	QString authenticationApp( QString &aProgram, QStringList & aArguments,
			QIODevice::OpenModeFlag aMode = QIODevice::ReadWrite ) const;

	/**
	 * Method to get the unique registration ID provided by the
	 * Smf for authorised plugins
	 * @return The unique registration ID/token provided by the Smf for
	 * authorised plugins
	 */
	QString smfRegistrationId( ) const;

private:
	/**
	 * Method that initializes this class. This method should be called
	 * from the initialize() method of the LastFmPlaylistServicePlugin class
	 */
	void initialize();

private:
	friend class LastFmPlaylistServicePlugin;
	QString m_serviceName;
	QImage m_serviceIcon;
	QString m_description;
	QUrl m_serviceUrl;
	QUrl m_applicationUrl;
	QImage m_applicationIcon;
	QString m_pluginId;
	QString m_authAppId;
	QString m_smfRegToken;
	QList<QString> m_supportedInterfaces;
	QStringList m_supportedLangs;
	QDateTime m_validity;
	};

#endif /* _LASTFMPLAYLISTSERVICEPLUGIN_H_ */
