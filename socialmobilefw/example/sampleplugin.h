#ifndef _SAMPLEPLUGIN_H
#define _SAMPLEPLUGIN_H

// Include files
#include <QVariant>
#include <qplugin.h>

#include "smfglobal.h"
#include "smfpicture.h"
#include "smfcomment.h"
#include "smfpluginbase.h"
#include "smfproviderbase.h"
#include "smfgalleryplugin.h"
#include "smfpluginutil.h"


// Forward declarations
class SampleProviderBase;


// Class declaration
class SamplePlugin : public SmfGalleryPlugin
{
	Q_OBJECT
	Q_INTERFACES( SmfGalleryPlugin )

public:
	/**
	 * Constructor with default argument
	 * @param aUtil The SmfPluginUtil instance. The plugins can 
	 * call the method getAuthKeys() of this class, with its pluginID to 
	 * get the OAuth keys, keys are returned only if this plugin is 
	 * authorised by Smf franework
	 */
	SamplePlugin( SmfPluginUtil *aUtil );
	
	/**
	 * Destructor
	 */
	~SamplePlugin( );
	
public: // From SmfGalleryPlugin
	
	/**
	 * Method to get a list of pictures
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError pictures( SmfPluginRequestData &aRequest, 
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Method to get a description
	 * @param aRequest [out] The request data to be sent to network
	 * @param aImage The image abot which the description is required
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError description( SmfPluginRequestData &aRequest,
			const SmfPicture &aImage );
	
	/**
	 * Method to upload a picture
	 * @param aRequest [out] The request data to be sent to network
	 * @param aImage The image to be uploaded
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError upload( SmfPluginRequestData &aRequest,
			const SmfPicture &aImage );
	
	/**
	 * Method to upload a list of pictures
	 * @param aRequest [out] The request data to be sent to network
	 * @param aImages The list of images to be uploaded
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError upload( SmfPluginRequestData &aRequest,
			const QList<SmfPicture> &aImages );
	
	/**
	 * Method to post comment on a picture is available
	 * @param aRequest [out] The request data to be sent to network
	 * @param aImage The image on which comment is to be posted
	 * @param aComment The comment to be posted
	 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
	 */
	SmfPluginError postComment( SmfPluginRequestData &aRequest,
			const SmfPicture &aImage, 
			const SmfComment &aComment );
	
public: // From SmfPluginBase
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
	SmfPluginError responseAvailable( 
			const SmfTransportResult &aTransportResult, 
			QNetworkReply *aReply, 
			QVariant* aResult, 
			SmfPluginRetType &aRetType,
			SmfResultPage &aPageResult );
	
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
	/**
	 * Function to retrieve registration token supplied by SMF to authentication app.
	 */
	QString retrievePrivateRegToken();
private:
	SampleProviderBase *m_provider;
	SmfPluginUtil *m_util;
};


// Class declaration
class SampleProviderBase : public SmfProviderBase
	{
	Q_OBJECT

public:
	/**
	 * Constructor with default argument
	 * @param aParent The parent object
	 */
	SampleProviderBase( QObject* aParent = 0 );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SampleProviderBase( const SampleProviderBase &aOther );
	
	/**
	 * Destructor
	 */
	~SampleProviderBase( );

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
	
	/**
	 * Method to get the data usage of each plugin
	 * @return The data usage structure
	 */
	SmfPluginDataUsage getDataUsage( ) const;
	
private:
	/**
	 * Method to update the data usage of this plugin. This method is called 
	 * after the plugin sends request to Plugin manager and after it receives 
	 * data from plugin manager.
	 * @param aBytesSent The number of bytes sent, when this argument has 
	 * some value other than 1, aBytesReceived should be zero.
	 * @param aBytesReceived The number of bytes received, when this argument 
	 * has some value other than 1, aBytesSent  should be zero.
	 * @return Returns true if success else returns false
	 */
	bool updateDataUsage( const uint &aBytesSent, const uint &aBytesReceived );

	friend class SamplePlugin;
	
private:
	QString m_serviceName;
	QImage m_serviceIcon;
	QString m_description;
	QUrl m_serviceUrl;
	QUrl m_applicationUrl;
	QImage m_applicationIcon;
	QString m_pluginId;
	QString m_authAppId;
	QString m_smfRegToken;
	SmfPluginDataUsage m_dataUsage;
	};

#endif /*_SAMPLEPLUGIN_H*/
