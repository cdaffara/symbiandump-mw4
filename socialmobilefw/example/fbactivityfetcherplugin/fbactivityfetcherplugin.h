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

#ifndef _FBACTIVITYFETCHERPLUGIN_H
#define _FBACTIVITYFETCHERPLUGIN_H

// Include files
#include <QDateTime>
#include <smfactivityfetcherplugin.h>

// Forward declarations
class FBActivityProviderBase;
class QVariant;

/**
 * The Plugin class that fetches activities from the logged in user's 
 * facebook account
 */
class FBActivityFetcherPlugin : public QObject, public SmfActivityFetcherPlugin
	{
	Q_OBJECT
	Q_INTERFACES( SmfActivityFetcherPlugin SmfPluginBase )

public:
	/**
	 * Destructor
	 */
	virtual ~FBActivityFetcherPlugin( );
	
public: // From SmfActivityFetcherPlugin interface
	
	/**
	 * Method to get the list of self activities, e.g. shown in own wall
	 * @param aRequest [out] The request data that plugin generates (to be sent to network)
	 * @param aPageNum[in] The page to be extracted
	 * @param aItemsPerPage[in] Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError selfActivities( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Method to get the list of activities for other, e.g. shown in a friends wall
	 * @param aRequest [out] The request data plugin generated (to be sent to network)
	 * @param aContact [in] The contact containing the URI, name or id of the user 
	 * @param aPageNum[in] The page to be extracted
	 * @param aItemsPerPage[in] Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError friendsActivities( SmfPluginRequestData &aRequest,
			const SmfContact &aContact,			
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Method to get list of self activities filtered by activity type, e.g. only photo updates in own wall
	 * @param aRequest [out] The request data plugin generated (to be sent to network)
	 * @param aFilters [in] The list of activity types to be included in result
	 * @param aPageNum[in] The page to be extracted
	 * @param aItemsPerPage[in] Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError filtered( SmfPluginRequestData &aRequest,
			QList<SmfActivityObjectType> &aFilters,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Customised method for SmfActivityFetcherPlugin interface
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
	 * For SmfActivityFetcherPlugin: If last operation was selfActivities() or 
	 * friendsActivities() or filtered(), aResult will be of type 
	 * QList<SmfActivityEntry>. 
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
	QString generateSignature(const QString aBaseString);
	
	/**
	 * Method to get the user's facebook ID
	 * @param aRequest [out] The request data to be sent to network
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError getFacebookUserId( SmfPluginRequestData &aRequest );
	
	/**
	 * Method to get the list of self activities, e.g. shown in own wall
	 * @param aRequest [out] The request data that plugin generates (to be sent to network)
	 * @param aPageNum[in] The page to be extracted
	 * @param aItemsPerPage[in] Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError getSelfActivities(  SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to convert the Activity verb string obtained from facebook to the 
	 * enum SmfActivityVerb
	 * @param verbStr The activity verb as a string
	 * @return The corresponding enum SmfActivityVerb
	 */
	SmfActivityVerb convertActivityverb( const QString& verbStr );
	
	/**
	 * Method to convert the Activity object type string obtained from 
	 * facebook to the enum SmfActivityObjectType
	 * @param objType The activity object type as a string
	 * @return The corresponding enum SmfActivityObjectType
	 */
	SmfActivityObjectType convertActivityObjectType( const QString& verbStr );
	
	/**
	 * Method to interpret the key sets obtained from credential manager 
	 * @param aApiKey [out] The api key
	 * @param aApiSecret [out] The api secret
	 * @param aSessionKey [out] The session key
	 * @param aSessionSecret [out] The session secret
	 * @param aAppId [out] The application ID
	 */
	void fetchKeys(	QString &aApiKey, 
			QString &aApiSecret, 
			QString &aSessionKey, 
			QString &aSessionSecret, 
			QString &aAppId);
	
private:
	FBActivityProviderBase *m_provider;
	};



/**
 * The Plugin class that implements SmfProviderBase for this facebook plugin
 */
class FBActivityProviderBase : public QObject, public SmfProviderBase
	{
	Q_OBJECT
	Q_INTERFACES( SmfProviderBase )

public:
	/**
	 * Destructor
	 */
	virtual ~FBActivityProviderBase( );

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
	 * from the initialize() method of the FBActivityFetcherPlugin class
	 */
	void initialize();
	
private:
	friend class FBActivityFetcherPlugin;
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

#endif /*_FBACTIVITYFETCHERPLUGIN_H*/
