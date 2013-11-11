/*
 * lastfmmusiceventplugin.h
 *
 *  Created on: Jun 21, 2010
 *      Author: ankitg
 */

#ifndef LASTFMMUSICEVENTPLUGIN_H_
#define LASTFMMUSICEVENTPLUGIN_H_

// Include files
#include "smfeventsfetcherplugin.h"
#include "smfpluginutil.h"

#include <parser.h>

// Forward declarations
class MusicProviderBase;
class QVariant;
class QNetworkReply;

// Class declaration
class MusicEventPlugin : public QObject, public SmfEventsFetcherPlugin
{
	Q_OBJECT
	Q_INTERFACES( SmfEventsFetcherPlugin SmfPluginBase )

public:
	virtual ~MusicEventPlugin();
	
public: // From SmfMusicEventsPlugin
	
		/**
		 * Method to get the events based on location
		 * @param aRequest [out] The request data to be sent to network
		 * @param aLocation Location of the event
		 * @param aPageNum The page to be extracted
		 * @param aItemsPerPage Number of items per page
		 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
		 */
		/*SmfPluginError events( SmfPluginRequestData &aRequest,
				const QGeoPositionInfo &aLocation,
				const int aPageNum = SMF_FIRST_PAGE, 
				const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
		*/
		 
		/**
		 * Method to get the venues based on location
		 * @param aRequest [out] The request data to be sent to network
		 * @param aLocation Location of the venue
		 * @param aPageNum The page to be extracted
		 * @param aItemsPerPage Number of items per page
		 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
		 */
	void  fetchKeys( QString &aApiKey, 
			QString &aApiSecret, 
			QString &aSessionKey, 
			QString &aSessionSecret );
		SmfPluginError venues( SmfPluginRequestData &aRequest,
				const SmfLocation &aLocation,
				const int aPageNum = SMF_FIRST_PAGE, 
				const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
		
		/**
		 * Method to get the events based on venues
		 * @param aRequest [out] The request data to be sent to network
		 * @param aVenue Venue of the event
		 * @param aPageNum The page to be extracted
		 * @param aItemsPerPage Number of items per page
		 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
		 */
		SmfPluginError events( SmfPluginRequestData &aRequest,
				const SmfLocation &aVenue,
				const int aPageNum = SMF_FIRST_PAGE, 
				const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
		
		/**
		 * Method to post events
		 * @param aRequest [out] The request data to be sent to network
		 * @param aEventList The list of events to be posted
		 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
		 */
		SmfPluginError postEvents( SmfPluginRequestData &aRequest,
				const QList<SmfEvent> &aEventList );
		
		
		/*** Method to get the list of attendees for a musical event
						 * @param aRequest [out] The request data to be sent to network
						 * @param aEvent The event for which attendees should be fetched
						 * @param aPageNum The page to be extracted
						 * @param aItemsPerPage Number of items per page
						 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
						 */
		SmfPluginError attendees( SmfPluginRequestData &aRequest,
				const SmfEvent &aEvent, 
				const int aPageNum = SMF_FIRST_PAGE,
				const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
		
		/**
		 * Customised method for SmfMusicEventsPlugin interface
		 * @param aRequest [out] The request data to be sent to network
		 * @param aOperation The operation type (should be known between 
		 * the client interface and the plugin)
		 * @param aData The data required to form the request (The type 
		 * of data should be known between client and the plugin)
		 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
		 */
		SmfPluginError customRequest( SmfPluginRequestData &aRequest, 
				const int &aOperation, QByteArray *aData );
		
		 
			
	
public: // From SmfPluginBase
	     /**
	     * The first method to be called in the plugin that implements this interface.
	     * If this method is not called, plugin may not behave as expected.
	     * Plugins are expected to save the aUtil handle and use and when required.
	     * @param aUtil The instance of SmfPluginUtil
	     */
	     void initialize(/*SmfPluginUtil *aUtil*/);
	
	    /**
	     * Method to get the provider information
	     * @return Instance of SmfProviderBase
	     */
	    SmfProviderBase* getProviderInfo( );
	
	   /**
	    * Method to get the result for a network request.
	    * @param aTransportResult The result of transport operation
	    * @param aReply The QNetworkReply instance for the request
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
        /*	SmfPluginError responseAvailable( 
			const SmfTransportResult &aTransportResult, 
			QNetworkReply *aReply, 
			QVariant* aResult, 
			SmfPluginRetType &aRetType,
			SmfResultPage &aPageResult );*/
        SmfPluginError responseAvailable( 
			const SmfRequestTypeID aOperation,
			const SmfTransportResult &aTransportResult, 
			QByteArray *aResponse, 
			QVariant* aResult, 
			SmfPluginRetType &aRetType,
			SmfResultPage &aPageResult );
	
	     void writeLog(QString log) const;
	
	
private:
    
      QString generateSignature(const QString aBaseString);
    
    
private slots:
   
    
	
private:
	  /**
	   * Method called by plugins to generate a request data
	   * @param aRequest [out] The request data to be sent to network
	   * @param aOperation The type of http operation
	   * @param aSignatureMethod The signature method to be used
	   * @param aParams A map of parameters to its values
	   * @param aMode The mode of creation of the request
	   * @param aPostData The data to be posted (for HTTP POST 
	   * only, else it will be NULL)
	   * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	   */
	   SmfPluginError createRequest( SmfPluginRequestData &aRequest,
			const QNetworkAccessManager::Operation aOperation, 
			const SmfSignatureMethod aSignatureMethod, 
			QMultiMap<QByteArray, QByteArray> &aParams, 
			const SmfParsingMode aMode,
			QBuffer *aPostData );

private:
	   MusicProviderBase *m_provider;
	   SmfPluginUtil *m_util;
};

// Class declaration
class MusicProviderBase : public QObject, public SmfProviderBase
	{
	 Q_OBJECT
	 Q_INTERFACES( SmfProviderBase )

public:
	virtual ~MusicProviderBase( );

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
	friend class MusicEventPlugin;
	void initialize();
	QString m_serviceName;
	QImage m_serviceIcon;
	QString m_description;
	QUrl m_serviceUrl;
	QUrl m_applicationUrl;
	QImage m_applicationIcon;
	QStringList m_serviceType;
	QStringList m_supportedLanguages;
	QString m_pluginId;
	QString m_authAppId;
	QString m_smfRegToken;
	QDateTime m_validity;
	};
#endif /* LASTFMMUSICEVENTPLUGIN_H_ */
