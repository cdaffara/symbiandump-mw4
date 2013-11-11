
// Include files
#include "sampleplugin.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <qfile.h>
#include <stdio.h>

/**
 * Constructor with default argument
 * @param aUtil The SmfPluginUtil instance. The plugins can
 * call the method getAuthKeys() of this class, with its pluginID to
 * get the OAuth keys, keys are returned only if this plugin is
 * authorised by Smf franework
 */
SamplePlugin::SamplePlugin( SmfPluginUtil *aUtil )
	{
	m_provider = new SampleProviderBase();
	m_util = aUtil;
	}

/**
 * Destructor
 */
SamplePlugin::~SamplePlugin( )
	{
	if(m_provider)
		delete m_provider;
	}

/**
 * Method to get a list of pictures
 * @param aRequest [out] The request data to be sent to network
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError SamplePlugin::pictures( SmfPluginRequestData &aRequest,
		const int aPageNum,
		const int aItemsPerPage )
	{
	SmfPluginError error = SmfPluginErrInvalidRequest;

	// invalid arguments
	if( aPageNum < 0 || aItemsPerPage < 0 )
		return error;
	else
		{
		// Create a map of the arguments keys and their repective values
		QMultiMap<QByteArray, QByteArray> params;
		QString pageNum, itemPerPage;
		pageNum.number(aPageNum);
		itemPerPage.number(aItemsPerPage);
		params.insert("method", "getpictures");
		params.insert("pagenumber", pageNum.toAscii());
		params.insert("itemsperpage", itemPerPage.toAscii());

		QNetworkAccessManager::Operation type = QNetworkAccessManager::GetOperation;
		SmfSignatureMethod signMethod = HMAC_SHA1;
		SmfParsingMode mode = ParseForInlineQuery;

		error = createRequest(aRequest, type, signMethod, params, mode, NULL);
		}
	return error;
	}


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
SmfPluginError SamplePlugin::createRequest( SmfPluginRequestData &aRequest,
		const QNetworkAccessManager::Operation aOperation,
		const SmfSignatureMethod aSignatureMethod,
		QMultiMap<QByteArray, QByteArray> &aParams,
		const SmfParsingMode aMode,
		QBuffer *aPostData )
	{
	SmfPluginError error;
	QString url = m_provider->serviceUrl().toString();

	// Get the oAuth keys from The Smf Server
	QMap<QString, QString> keys;
	QString registrationToken = retrievePrivateRegToken();
	m_util->getAuthKeys(keys, registrationToken, m_provider->pluginId());

	// Unable to get the tokens
	if(keys.isEmpty())
		error = SmfPluginErrInvalidApplication;
	else
		{

		// Get the token and token secret from keys
		QByteArray token;
		QByteArray tokenSecret;
		token.append(keys.value("oauth_token"));
		tokenSecret.append(keys.value("oauth_token_secret"));

		// convert the parameters to string and sign it
		QByteArray content = m_util->createParameterString(url, aOperation, token, tokenSecret,
				aSignatureMethod, aParams, aMode );

		// Unable to create the signed string
		if(content.isEmpty())
			error = SmfPluginErrInvalidRequest;
		else
			{
			// add the parameter string to the URL
			url.append(content);

			// set the url of the request
			aRequest.iNetworkRequest.setUrl(QUrl(url));

			// set the type of http operation to be performed
			aRequest.iHttpOperationType = aOperation;

			// As it is a GET operation, set iPostData to NULL
			aRequest.iPostData = aPostData;

			// For successful creation of request
			error = SmfPluginErrNone;
			}
		}
	return error;
	}


/**
 * Method to get a description
 * @param aRequest [out] The request data to be sent to network
 * @param aImage The image abot which the description is required
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError SamplePlugin::description( SmfPluginRequestData &aRequest,
		const SmfPicture &aImage )
	{
	SmfPluginError error;

	// Create a map of the arguments keys and their repective values
	QMultiMap<QByteArray, QByteArray> params;
	params.insert("method", "getpictureDescription");
	params.insert("photoId", aImage.id().toAscii());

	QNetworkAccessManager::Operation type = QNetworkAccessManager::GetOperation;
	SmfSignatureMethod signMethod = HMAC_SHA1;
	SmfParsingMode mode = ParseForInlineQuery;

	error = createRequest(aRequest, type, signMethod, params, mode, NULL);

	return error;
	}

/**
 * Method to upload a picture
 * @param aRequest [out] The request data to be sent to network
 * @param aImage The image to be uploaded
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError SamplePlugin::upload( SmfPluginRequestData &aRequest,
		const SmfPicture &aImage )
	{
	SmfPluginError error = SmfPluginErrInvalidRequest;

	// Create a map of the arguments keys and their repective values
	QMultiMap<QByteArray, QByteArray> params;
	params.insert("method", "upload");
	params.insert("title", aImage.title().toAscii());
	params.insert("owner", aImage.owner().toAscii());
	params.insert("description", aImage.description().toAscii());
	params.insert("tags", aImage.tags().join(" ").toAscii());
	switch(aImage.visibility())
		{
		case SMFVisibilityFriend:
			params.insert("isFriend", "true");
			break;
		case SMFVisibilityPublic:
			params.insert("isPublic", "true");
			break;
		case SMFVisibilityFamily:
			params.insert("isFamily", "true");
			break;
		case SMFVisibilityGroup:
			params.insert("isGroup", "true");
			break;
		default:// SMFVisibilityPersonal
			params.insert("isPrivate", "true");
		}

	QNetworkAccessManager::Operation type = QNetworkAccessManager::PostOperation;
	SmfSignatureMethod signMethod = HMAC_SHA1;
	SmfParsingMode mode = ParseForRequestContent;

	// Write the image as png format to the buffer
	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	aImage.picture().save(&buffer, "PNG");

	error = createRequest(aRequest, type, signMethod, params, mode, &buffer);

	return error;
	}

/**
 * Method to upload a list of pictures
 * @param aRequest [out] The request data to be sent to network
 * @param aImages The list of images to be uploaded
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError SamplePlugin::upload( SmfPluginRequestData &aRequest,
		const QList<SmfPicture> &aImages )
	{
	SmfPluginError error;

	for(int index = 0; index < aImages.count(); index++)
		{
		error = upload(aRequest, aImages.value(index));
		if(SmfPluginErrNone != error)
			break;
		}
	return error;
	}

/**
 * Method to post comment on a picture is available
 * @param aRequest [out] The request data to be sent to network
 * @param aImage The image on which comment is to be posted
 * @param aComment The comment to be posted
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError SamplePlugin::postComment( SmfPluginRequestData &aRequest,
		const SmfPicture &aImage,
		const SmfComment &aComment )
	{
	SmfPluginError error = SmfPluginErrInvalidRequest;

	// Create a map of the arguments keys and their repective values
	QMultiMap<QByteArray, QByteArray> params;
	params.insert("method", "postComment");
	params.insert("photoId", aImage.id().toAscii());
	params.insert("comment", "excellent Himalaya");

	QNetworkAccessManager::Operation type = QNetworkAccessManager::GetOperation;
	SmfSignatureMethod signMethod = HMAC_SHA1;
	SmfParsingMode mode = ParseForInlineQuery;

	error = createRequest(aRequest, type, signMethod, params, mode, NULL);
	return error;
	}

/**
 * This function retrieves the registration token that was provided to Authentication App
 * while authenticatiing user with the service
 * 
 * Plugin source codes are not open source - so free to use anything they like
 */
QString SamplePlugin::retrievePrivateRegToken()
	{

	/**
	 * This is a private implementation - 
	 * implementer might choose to use registry to store/retrieve this token
	 * or to write encrypted (symmetric) token to a file kept at known dir
	 */
	QFile qf("/resource/data/sampleplugindata.dat"); 
	qf.open(QIODevice::ReadOnly);
	QByteArray qba = qf.read(20); 
	qba.chop(5);
	QString rs(qba.toBase64());
	return rs;
	}


/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* SamplePlugin::getProviderInfo( )
	{
	return m_provider;
	}

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
SmfPluginError SamplePlugin::responseAvailable(
		const SmfTransportResult &aTransportResult,
		QNetworkReply *aReply,
		QVariant* aResult,
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	SmfPluginError error;
	if(SmfTransportOpNoError == aTransportResult)
		{
		// Assuming a JSON response, parse the response
		QByteArray response = aReply->readAll();
		m_provider->updateDataUsage(0, aReply->readBufferSize());
		bool parseResult = false;
		QVariant *result = new QVariant();
		/** see http://qjson.sourceforge.net/usage.html for more details */
		parseResult = m_util->getJsonHandle()->parse(response, &parseResult);

		// For parsing error
		if(!parseResult)
			{
			aRetType = SmfRequestError;
			error = SmfPluginErrInvalidRequest;
			}

		else
			{
			// The plugins should convert the result to suitable format,
			// like if last operation was pictures(), result should be converted to the
			// type QList<SmfPicture>. If last operation was description(), result should
			// be converted to the type QString. If last operation was upload() or
			// postComment(), result should be converted to the type bool.

			// After conversion, assign the value os result to aResult
			aResult = result;

			// if the request is complete
			aRetType = SmfRequestComplete;

			// if request need to be sent again
			aRetType = SmfSendRequestAgain;

			error = SmfPluginErrNone;
			}
		}
	else
		{
		error = SmfPluginErrInvalidRequest;
		aRetType = SmfRequestError;
		}

	return error;
	}


/**
 * Constructor with default argument
 * @param aParent The parent object
 */
SampleProviderBase::SampleProviderBase( QObject* aParent )
	: SmfProviderBase(aParent)
	{
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SampleProviderBase::SampleProviderBase( const SampleProviderBase &aOther )
	{
	}

/**
 * Destructor
 */
SampleProviderBase::~SampleProviderBase( )
	{
	}

/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString SampleProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}

/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage SampleProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}

/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString SampleProviderBase::description( ) const
	{
	return m_description;
	}

/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl SampleProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}

/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl SampleProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}

/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage SampleProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString SampleProviderBase::pluginId( ) const
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
QString SampleProviderBase::authenticationApp( QString &aProgram,
		QStringList & aArguments,
		QIODevice::OpenModeFlag aMode ) const
	{
	return m_authAppId;
	}

/**
 * Method to get the unique registration ID provided by the
 * Smf for authorised plugins
 * @return The unique registration ID/token provided by the Smf for
 * authorised plugins
 */
QString SampleProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}

/**
 * Method to get the data usage of each plugin
 * @return The data usage structure
 */
SmfPluginDataUsage SampleProviderBase::getDataUsage( ) const
	{
	return m_dataUsage;
	}

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
bool SampleProviderBase::updateDataUsage( const uint &aBytesSent,
		const uint &aBytesReceived )
	{
	bool ret = true;
	if( aBytesSent && !aBytesReceived )
		m_dataUsage.iBytesSent += aBytesSent;
	else if( !aBytesSent && aBytesReceived )
		m_dataUsage.iBytesReceived += aBytesReceived;
	else
		// don't update m_dataUsage, error in arguments
		ret = false;

	return ret;
	}

/*
 * Export Macro
 * plugin name : sampleplugin
 * plugin class : SamplePlugin
 */
//Q_EXPORT_PLUGIN2( sampleplugin, SamplePlugin )
