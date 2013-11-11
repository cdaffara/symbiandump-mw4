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

#ifndef _FLICKRCONTACTFETCHERPLUGIN_H
#define _FLICKRCONTACTFETCHERPLUGIN_H

// Include files
#include <QDateTime>
#include <smfcontactfetcherplugin.h>

// Forward declarations
class FlickrProviderBase;
class QVariant;


/**
 * The Plugin that fetches contacts from the logged in user's flickr account
 */
class FlickrContactFetcherPlugin : public QObject, public SmfContactFetcherPlugin
	{
	Q_OBJECT
	Q_INTERFACES( SmfContactFetcherPlugin SmfPluginBase )

public:
	/**
	 * Destructor
	 */
	virtual ~FlickrContactFetcherPlugin( );
	
public: // From SmfContactFetcherPlugin interface
	
	/**
	 * Method to get the list of friends
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError friends( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to get the list of followers
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError followers( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to search for a contact
	 * @param aRequest [out] The request data to be sent to network
	 * @param aContact contact to be searched
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError search( SmfPluginRequestData &aRequest,
			const SmfContact &aContact,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
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
	SmfPluginError searchNear( SmfPluginRequestData &aRequest,
			const SmfLocation &aLocation,
			const SmfLocationSearchBoundary &aProximity,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to get the list of groups
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError groups( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to search for a contact in a group
	 * @param aRequest [out] The request data to be sent to network
	 * @param aGroup the group in which to search
	 * @param aContact The contact to be searched, default (NULL) is the self contact.
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError searchInGroup( SmfPluginRequestData &aRequest,
			const SmfGroup &aGroup,
			SmfContact *aContact = NULL,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Customised method for SmfContactFetcherPlugin interface
	 * @param aRequest [out] The request data to be sent to network
	 * @param aOperation The operation type (should be known between 
	 * the client interface and the plugin)
	 * @param aData The data required to form the request (The type 
	 * of data should be known between client and the plugin)
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
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
	 * For SmfGalleryPlugin: If last operation was pictures(), aResult will 
	 * be of type QList<SmfPicture>. If last operation was description(), 
	 * aResult will be of type QString. If last operation was upload() or 
	 * postComment(), aResult will be of type bool.
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
	 * @param aAuthToken [out] The auth token provided by Flickr
	 */
	void fetchKeys(	QString &aApiKey, 
			QString &aApiSecret, 
			QString &aAuthToken );
    
private:
	FlickrProviderBase *m_provider;
	};


/**
 * The Plugin class that implements SmfProviderBase for this flickr plugin
 */
class FlickrProviderBase : public QObject, public SmfProviderBase
	{
	Q_OBJECT
	Q_INTERFACES( SmfProviderBase )

public:
	/**
	 * Destructor
	 */
	virtual ~FlickrProviderBase( );

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
	 * from the initialize() method of the FlickrContactProviderBase class
	 */
	void initialize();
	
private:
	friend class FlickrContactFetcherPlugin;
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

#endif /*_FLICKRCONTACTFETCHERPLUGIN_H*/
