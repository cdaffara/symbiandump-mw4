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
 * Sangeetha Prasad, Nalina Hariharan
 * 
 * Description:
 * The Plugin that fetches gallery related items from the logged in user's flickr account
 *
 */

// Include files
#include <QtPlugin>
#include <QCryptographicHash>
#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QListIterator>
#include <QXmlStreamReader>
#include <QSettings>
#include <smfpluginutil.h>

#include "flickrgalleryplugin.h"

static int count = 0;
static int chance = 0; // 0 = for pics from album, 1 = pics not in any album
static int listIndex = 0;// For Mutliple Load
QByteArray payload;
QByteArray boundary("---ThIsIsAsAmPleBouNDaRyStrInGFrOmNaliNa---");

// Todo:- Macro added for limiting items fetched to recent 5
// Remove after demo
#define SETLIMITOFFIVEFORSMFDEMO 1

/**
 * Method to interpret the key sets obtained from credential manager 
 * @param aApiKey [out] The api key
 * @param aApiSecret [out] The api secret
 * @param aAuthToken [out] The auth token provided by Flickr
 */
void FlickrGalleryPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aAuthToken )
	{
	qDebug()<<"Inside FlickrGalleryPlugin::fetchKeys()";

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
 * Destructor
 */
FlickrGalleryPlugin::~FlickrGalleryPlugin( )
	{
	if(m_provider)
		delete m_provider;
	}

/**
 * Method to get a list of albums
 * @param aRequest [out] The request data to be sent to network
 * @param aNames The subject or any keywords to be used to filter albums with that name
 * @param aUser The user whose albums are requested
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrGalleryPlugin::albums( SmfPluginRequestData &aRequest, 
		const QStringList &aNames, 
		const SmfContact *aUser, 
		const int aPageNum,
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FlickrGalleryPlugin::albums()";
	Q_UNUSED(aNames)
	Q_UNUSED(aPageNum)
	Q_UNUSED(aItemsPerPage)
	
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	QString userId = aUser->value("Guid").value<QContactGuid>().guid();
	qDebug()<<"Flickr user's ID for fetching photos = "<<userId;
	
	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
    baseString.append("formatjson");
    baseString.append("methodflickr.photosets.getList");
#ifdef SETLIMITOFFIVEFORSMFDEMO
    baseString.append("page"+QString::number(1));
    baseString.append("per_page"+QString::number(5));
#else
    baseString.append("page"+QString::number(aPageNum));
    baseString.append("per_page"+QString::number(aItemsPerPage));
#endif
    if(userId.length())
    	baseString.append("user_id"+userId);
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("format","json");
	if(userId.length())
		url.addQueryItem("user_id", userId);
	url.addQueryItem("method", "flickr.photosets.getList");
#ifdef SETLIMITOFFIVEFORSMFDEMO
	url.addQueryItem("page", QString::number(1));
	url.addQueryItem("per_page", QString::number(5));
#else
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("per_page", QString::number(aItemsPerPage));
#endif
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfPictureGetAlbums;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
 * Method to get a list of pictures
 * @param aRequest [out] The request data to be sent to network
 * @param aAlbums The album(s) whose pictures are being requested
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrGalleryPlugin::pictures( SmfPluginRequestData &aRequest, 
		const SmfPictureAlbumList &aAlbums,
		const int aPageNum, 
		const int aItemsPerPage )
	{
	qDebug()<<"Inside FlickrGalleryPlugin::pictures()";
	
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	if( (aPageNum < 0) ||	(aItemsPerPage < 0))
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );

	QString albumId;
	albumId.clear();
	if(aAlbums.count())
		{
		chance = 0;
		albumId = aAlbums.at(0).id();
		qDebug()<<"Fetching photos from a photoset with its id = "<<albumId;
		}
	else
		{
		qDebug()<<"Fetching photos which are not in any sets";
		chance = 1;
		}
	
	QString extras("date_upload,description,owner_name,tags,url_sq,url_t,url_s,url_m,url_o");

	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString.append("extras"+extras);
    baseString.append("formatjson");
    if(aAlbums.count())
    	baseString.append("methodflickr.photosets.getPhotos");
    else
    	baseString.append("methodflickr.photos.getNotInSet");
#ifdef SETLIMITOFFIVEFORSMFDEMO
    baseString.append("page"+QString::number(1));
    baseString.append("per_page"+QString::number(5));
#else
    baseString.append("page"+QString::number(aPageNum));
    baseString.append("per_page"+QString::number(aItemsPerPage));
#endif
    if(aAlbums.count())
    	baseString.append("photoset_id"+albumId);
	
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("extras", extras);
	url.addQueryItem("format","json");
	if(aAlbums.count())
		url.addQueryItem("method", "flickr.photosets.getPhotos");
	else
		url.addQueryItem("method", "flickr.photos.getNotInSet");
#ifdef SETLIMITOFFIVEFORSMFDEMO
	url.addQueryItem("page", QString::number(1));
	url.addQueryItem("per_page", QString::number(5));
#else
	url.addQueryItem("page", QString::number(aPageNum));
	url.addQueryItem("per_page", QString::number(aItemsPerPage));
#endif
	if(aAlbums.count())
		url.addQueryItem("photoset_id", albumId);
	url.addQueryItem("api_sig", generateSignature(baseString));
	
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfPictureGetPictures;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}

/**
* Method to get a description
* @param aRequest [out] The request data to be sent to network
* @param aImage The image abot which the description is required
* @return SmfPluginError Plugin error if any, else SmfPluginErrNone
*/
SmfPluginError FlickrGalleryPlugin::description(SmfPluginRequestData &aRequest,
			const SmfPicture &aImage)
	{
	qDebug()<<"Inside FlickrGalleryPlugin::description()";
	
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	if( 0 == aImage.id().size())
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
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
	baseString.append("methodflickr.photos.getInfo");
	baseString.append("photo_id"+aImage.id());
		
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("format","json");
	url.addQueryItem("method", "flickr.photos.getInfo");
	url.addQueryItem("photo_id",aImage.id());
	url.addQueryItem("api_sig", generateSignature(baseString));
		
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfPictureDescription;
	aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}


/**
 * Method to upload a picture
 * @param aRequest [out] The request data to be sent to network
 * @param aImage The image to be uploaded
 * @param aAlbum the optional destination album name
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrGalleryPlugin::upload( SmfPluginRequestData &aRequest,
		const SmfPicture &aImage,
		const SmfPictureAlbum* aAlbum  )
	{
	qDebug()<<"Inside FlickrGalleryPlugin::upload()";
	Q_UNUSED(aAlbum)
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	if( 0 == aImage.picture().byteCount())
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );

	// Create the Payload data
	QDataStream stream(&payload, QIODevice::WriteOnly);
	int bytes = 0;
	
	// Create the api_key field
    QByteArray keyField = constructField ( "api_key", apiKey, boundary );
    bytes = stream.writeRawData(keyField.data(), keyField.length());
    qDebug()<<"Bytes written for API key field = "<<QString::number(bytes, 10);
	
	// Create the auth_token field
    QByteArray tokenField = constructField ( "auth_token", authToken, boundary );
    bytes = stream.writeRawData(tokenField.data(), tokenField.length());
    qDebug()<<"Bytes written for Auth token field = "<<QString::number(bytes, 10);

	// Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	baseString = generateSignature(baseString);
    
	// Create the signature field
    QByteArray sigField = constructField ( "api_sig", baseString, boundary );
    bytes = stream.writeRawData(sigField.data(), sigField.length());
    qDebug()<<"Bytes written for API signature field = "<<QString::number(bytes, 10);
	
    // Create the file header field
    QByteArray fileField = constructField ( "photo", "", boundary, "c:\\data\\TestUploadPics\\BlueHills.jpg" );
    bytes = stream.writeRawData(fileField.data(), fileField.length());
    qDebug()<<"Bytes written for File header field = "<<QString::number(bytes, 10);
    
    // GEt the image data into a Bytearray
    QByteArray pic;
    QBuffer buffer(&pic);
    buffer.open(QIODevice::WriteOnly);
    aImage.picture().save(&buffer, "JPG");

    qDebug()<<"Number of bytes in the pic = "<<QString::number(pic.size(), 10);
        
    // Add the file content to the stream
    if(pic.size())
    	{
    	bytes = stream.writeRawData ( pic.data(), pic.length() );
    	qDebug()<<"Bytes written for File content field = "<<QString::number(bytes, 10);
    	}
    
    // Create the end field
    QByteArray endField;
    endField.append ( "\r\n--" );
    endField.append ( boundary );
    endField.append ( "--\r\n\r\n" );
    bytes = stream.writeRawData(endField.data(), endField.length());
    qDebug()<<"Bytes written for end field = "<<QString::number(bytes, 10);
		
    QUrl url("http://api.flickr.com/services/upload/");
    
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iNetworkRequest.setRawHeader("Content-Type", "multipart/form-data; boundary="+boundary);
	aRequest.iRequestType = SmfPictureUpload;
	aRequest.iPostData = new QBuffer(&payload);
	aRequest.iHttpOperationType = QNetworkAccessManager::PostOperation;
	error = SmfPluginErrNone;

	qDebug()<<"Url string is : "<<aRequest.iNetworkRequest.url().toString();
	return error;
	}


/**
 * Method to construct individual fields for photo upload
 * @param aName The name argument
 * @param aContent The content of this field
 * @param aBoundary The boundary string (need to be unique in the payload data)
 * @param aFilename The filename if for photo field 
 * @return The field data constructed by this method
 */
QByteArray FlickrGalleryPlugin::constructField( const QString &aName, 
		const QString &aContent, 
		const QByteArray &aBoundary, 
		const QString & aFilename )
	{
    QByteArray data;
    data.append ( "--" );
    data.append ( aBoundary );
    data.append ( "\r\n" );
    data.append ( "Content-Disposition: form-data; name=\"" );
    data.append ( aName.toAscii() );
    if ( aFilename.isEmpty() )
    	{
        data.append ( "\"\r\n\r\n" );
        data.append ( aContent.toAscii() );
        data.append ( "\r\n" );
    	}
    else
    	{
        data.append ( "\"; filename=\"" );
        data.append ( aFilename.toAscii() );
        data.append ( "\"\r\n" );
        data.append ( "Content-Type: image/jpeg\r\n\r\n" );
    	}
    return data;
	}


/**
* Method to upload a list of pictures
* @param aRequest [out] The request data to be sent to network
* @param aImages The list of images to be uploaded
* @return SmfPluginError Plugin error if any, else SmfPluginErrNone
*/
SmfPluginError FlickrGalleryPlugin::upload( SmfPluginRequestData &aRequest,
			const QList<SmfPicture> &aImages,
			const SmfPictureAlbum* aAlbum )
	{
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
	qDebug()<<("\r\n multiple Uploaded Pictures.....= "+QString::number(aImages.count(),10));
	Q_UNUSED(aAlbum)
	// Get the key sets from SMF Plugin Utility class.
	QString apiKey;
	QString apiSecret;
	QString authToken;
	fetchKeys(apiKey, apiSecret, authToken );
	
	count =aImages.count();
	
	
	QByteArray boundary("---ThIsIsAsAmPleBouNDaRyStrInGFrOmSaNgEeTa---");
	QDataStream stream(&payload, QIODevice::WriteOnly);
	int bytes = 0;
	
	// Create the api_key field
	QByteArray keyField = constructField("api_key",apiKey,boundary,"");//,apiKey,boundary);
	bytes = stream.writeRawData(keyField.data(), keyField.length());
	qDebug()<<("Bytes written for API key field = "+QString::number(bytes, 10));
	
	// Create the auth_token field
	QByteArray tokenField = constructField ( "auth_token", authToken, boundary,"");
	bytes = stream.writeRawData(tokenField.data(), tokenField.length());
	qDebug()<<("Bytes written for Auth token field = "+QString::number(bytes, 10));
	
	//Create the API signature string
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token"+authToken);
	//baseString.append("formatjson"); //not working this format
	baseString = generateSignature(baseString);
	
	//extract the smfpicture object 
	SmfPicture aImage=aImages[listIndex];
	
	//Create the signature field
	QByteArray sigField = constructField ("api_sig", baseString, boundary,"" );
	bytes = stream.writeRawData(sigField.data(), sigField.length());
	
	qDebug()<<("QImage String = "+aImage.url().toString());
	
	//Create the file header field
	QByteArray fileField = constructField ( "photo", "", boundary, aImage.url().toString());
	bytes = stream.writeRawData(fileField.data(), fileField.length());
	
	
	//QByteArray pic ;
	QImage image=aImage.picture();
	
	//for checking only 
	int byte=image.numBytes();
	qDebug()<<("Bytes written for QImage = "+QString::number(byte,10));
	
	//For finding the format of Image file (eg. c:\\data\\image.jpg);
	QString fileName=aImage.url().toString();
	int last = fileName.lastIndexOf(".",-1);
	qDebug()<<("last  QImage = "+QString::number(last,10));
	QString Format=fileName.right(fileName.size()-last-1);  //-1 for removing "." from .jpg
	qDebug()<<("Format of  QImage = "+Format);
	
	
	QByteArray pic;
	QBuffer buffer(&pic);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer,Format.toLatin1(),-1); // writes image into format given by Format
	
	qDebug()<<("Number of bytes in the picture = "+QString::number(buffer.size(), 10));
	
	// Add the file content to the stream
	if (pic.size())
	{
	bytes = stream.writeRawData ( pic.data(), pic.length() );
	qDebug()<<("Bytes written for File content field = "+QString::number(bytes, 10));
	}
	qDebug()<<("Before Url string is : "+QString(payload));
	
	// Create the end field
	QByteArray endField;
	endField.append ( "\r\n--" );
	endField.append ( boundary );
	endField.append ( "--\r\n\r\n" );
	bytes = stream.writeRawData(endField.data(), endField.length());
	qDebug()<<("Bytes written for end field = "+QString::number(bytes, 10));
	
	//Create the url
	QUrl url("http://api.flickr.com/services/upload/");
	//url.addQueryItem("format","json"); //Not working
	
	
	//Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	
	aRequest.iNetworkRequest.setRawHeader("Content-Type", "multipart/form-data; boundary="+boundary);
	aRequest.iNetworkRequest.setRawHeader("Host", "api.flickr.com");
	aRequest.iRequestType = SmfPictureUpload;
	aRequest.iPostData=new QBuffer(&payload);
	
	
	aRequest.iHttpOperationType = QNetworkAccessManager::PostOperation;
	error = SmfPluginErrNone;
	return error;
	}

 /**
 * Method to post comment on a picture is available
 * @param aRequest [out] The request data to be sent to network
 * @param aImage The image on which comment is to be posted
 * @param aComment The comment to be posted
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError FlickrGalleryPlugin::postComment( SmfPluginRequestData &aRequest,
		 const SmfPicture &aImage, 
		 const SmfComment &aComment )
	{
	qDebug()<<"Inside FlickrGalleryPlugin::postComment()";
	
	SmfPluginError error = SmfPluginErrInvalidArguments;
	
	if( 0 == aImage.id().size() || (0 == aComment.text().size()) )
		{
		qDebug()<<"Invalid arguments";
		return error;
		}
	
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
	baseString.append("comment_text"+aComment.text());
	baseString.append("formatjson");
	baseString.append("methodflickr.photos.comments.addComment");
	baseString.append("photo_id"+aImage.id());
		
	// Create the url
	QUrl url("http://api.flickr.com/services/rest/?");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("auth_token", authToken);
	url.addQueryItem("comment_text", aComment.text());
	url.addQueryItem("format","json");
	url.addQueryItem("method", "flickr.photos.comments.addComment");
	url.addQueryItem("photo_id",aImage.id());
	url.addQueryItem("api_sig", generateSignature(baseString));
		
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(url);
	aRequest.iRequestType = SmfPicturePostComment;
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
QString FlickrGalleryPlugin::generateSignature(const QString aBaseString)
	{
	qDebug()<<("FlickrGalleryPlugin::generateSignature");
	
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
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
SmfPluginError FlickrGalleryPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	return SmfPluginErrServiceNotSupported; 
	}

/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 */
void FlickrGalleryPlugin::initialize( )
	{
	// Create an instance of FBContactProviderBase
	m_provider = new FlickrProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* FlickrGalleryPlugin::getProviderInfo( )
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
 * For SmfGalleryPlugin: If last operation was albums, aResult will be of 
 * type QList<SmfPictureAlbum>. If the last operation was pictures(), aResult 
 * will be of type QList<SmfPicture>. If last operation was description(), 
 * aResult will be of type QString. If last operation was upload() or 
 * postComment(), aResult will be of type bool.
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError FlickrGalleryPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{

	qDebug()<<"Inside FlickrGalleryPlugin::responseAvailable()";
	Q_UNUSED(aPageResult)
	SmfPluginError error = SmfPluginErrNetworkError;
	
	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<"Response is NULL or empty";
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;

	QFile respFile("c://data//SmfPluginFlickrResponse.txt");
	if(!respFile.open(QIODevice::WriteOnly))
		{
		qDebug()<<"File to write the response could not be opened, so writing to this file";
		qDebug()<<"Flickr response = "<<QString(response);
		}
	else
		{
		respFile.write(response);
		respFile.close();
		qDebug()<<"Writing FB response to a file named 'SmfPluginFlickrResponse.txt'";
		}
	
	qDebug()<<"Response size = "<<response.size();

	
	if(SmfTransportOpNoError == aTransportResult)
		{
		qDebug()<<"No transport error";

		if(SmfPictureGetAlbums == aOperation)
			{
			qDebug()<<"Response for retrieving album lists";

			QList<SmfPictureAlbum> albumList;
			QString errStr;
			errStr.clear();
			
			// Remove the "jsonFlickrApi(" from starting of the response and ")" from its end
			response.remove(0, 14);
			response.chop(1);
			
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
				
				
			if(response.contains(QByteArray("fail")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1 = result["photosets"].toMap();
				
				QList<QVariant> list1 = map1["photoset"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfPictureAlbum album;

					QVariantMap map2 = iter.next().toMap();
					
					// Set the album's title
					QVariantMap map3 = map2["title"].toMap();
					album.setTitle(map3["_content"].toString());
					qDebug()<<"title = "<<album.title();
							
					// Set the album's description
					QVariantMap map4 = map2["description"].toMap();
					album.setDescription(map4["_content"].toString());
					qDebug()<<"desc = "<<album.description();

					// Set the album's picture count
					album.setPictureCount(map2["photos"].toInt());
					qDebug()<<"pic count = "<<album.pictureCount();
					
					// Set the album's id
					album.setId(map2["id"].toString());
					qDebug()<<"id = "<<album.id();
		
					albumList.append(album);
#ifdef SETLIMITOFFIVEFORSMFDEMO
					if(5 == albumList.count())
						break;
#endif
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
				qDebug()<<"list count = "<<albumList.count();
				aResult->setValue(albumList);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
			}
		else if(SmfPictureGetPictures == aOperation)
			{
			qDebug()<<"Response for retrieving photos";
	
			QList<SmfPicture> picList;
			QString errStr;
			errStr.clear();
			
			// Remove the "jsonFlickrApi(" from starting of the response and ")" from its end
			response.remove(0, 14);
			response.chop(1);
			
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
				
				
			if(response.contains(QByteArray("fail")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1;
				if(0 == chance)
					map1 = result["photoset"].toMap();
				else
					map1 = result["photos"].toMap();
				
				QList<QVariant> list1 = map1["photo"].toList();
				QListIterator<QVariant> iter(list1);
				while(iter.hasNext())
					{
					SmfPicture pic;
	
					QVariantMap map2 = iter.next().toMap();
					
					// Set the pic's id
					pic.setId(map2["id"].toString());
					
					// Set the pic's description
					QVariantMap map3 = map2["description"].toMap();
					pic.setDescription(map3["_content"].toString());
					
					// Set the pic's title
					pic.setTitle(map2["title"].toString());
					qDebug()<<"title = "<<map2["title"].toString();
					
					// Set the pic's owner
					pic.setOwner(map2["ownername"].toString());
					
					// Set the pic's posted date
					QDateTime date = QDateTime::fromTime_t(map2["dateupload"].toUInt());
					qDebug()<<"time = "<<date;
					pic.setPostedDate(date);
										
					// Set the pic's tags
					QString tags(map2["tags"].toString());
					QStringList tagList = tags.split(' ');
					pic.addTags(tagList);
					
					// Set the pic's url
					// url_sq,url_t,url_s,url_m,url_o
					if(map2["url_o"].toString().size())
						pic.setUrl(map2["url_o"].toString());
					else if(map2["url_m"].toString().size())
						pic.setUrl(map2["url_m"].toString());
					else if(map2["url_sq"].toString().size())
						pic.setUrl(map2["url_sq"].toString());
					else if(map2["url_s"].toString().size())
						pic.setUrl(map2["url_s"].toString());
					else if(map2["url_t"].toString().size())
						pic.setUrl(map2["url_t"].toString());
					
					qDebug()<<"url = "<<pic.url();
					picList.append(pic);
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
				qDebug()<<"list count = "<<picList.count();
				aResult->setValue(picList);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
			}
		else if(SmfPictureDescription == aOperation)
			{
			qDebug()<<"Response for photo decscription";
	
			QString description;
			QString errStr;
			errStr.clear();
			
			// Remove the "jsonFlickrApi(" from starting of the response and ")" from its end
			response.remove(0, 14);
			response.chop(1);
			
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
	
			if(response.contains(QByteArray("fail")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				QVariantMap map1 = result["photo"].toMap();
				QVariantMap map2 = map1["description"].toMap();
				
				description = map2["_content"].toString();
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
				qDebug()<<"description = "<<description;
				aResult->setValue(description);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
			}
		else if (SmfPictureUpload == aOperation)
			{
#if 1
			qDebug()<<"Response for single photo upload";
	
			bool uploaded = false;
			QString errStr;
			errStr.clear();
			
			if(response.contains(QByteArray("err code")))
				{
				QXmlStreamReader xml(response);
				while (!xml.atEnd())
					{
					xml.readNext();
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						if (xml.name() == "err")
							errStr.append(xml.attributes().value("msg"));
					}				
				}
			else
				uploaded = true;

			if(errStr.size())
				{
				qDebug()<<"Response error found = "<<errStr;
				error = SmfPluginErrInvalidRequest;
				aRetType = SmfRequestError;
				aResult->setValue(errStr);
				}
			else
				{
				qDebug()<<"photo uploaded ? "<<uploaded;
				aResult->setValue(uploaded);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
#endif
			}
		   else if (SmfPictureMultiUpload == aOperation)
			   {
				    bool result;
					qDebug()<<("SmfMultiPictureUpload");
					QXmlStreamReader xml(response);
					while (!xml.atEnd())
					   {
						 xml.readNext();
						 if (xml.tokenType() == QXmlStreamReader::StartElement)
						 {
						 	qDebug()<<("inside tag");
						 	//If the tag is contact
						 	if (xml.name() == "photoid")
						 	{
						 		qDebug()<<("photoid tag found");
						 		result=TRUE;
				             }
						 	 else
						 		result=FALSE;
						 	}//end If
						}//endWhile;
						 		
						aResult->setValue(result);
						if (listIndex < count)
						 {
						    listIndex=listIndex+1;
						    error = SmfPluginErrNone;
							aRetType = SmfSendRequestAgain;
							  
						  }
						 else
							{
						      //Final result sent 
						      count=1; //clear counter 
						      aRetType = SmfRequestComplete;
				 		      error = SmfPluginErrNone;
							}
				 
			 }
		else if (SmfPicturePostComment == aOperation)
			{
			qDebug()<<"Response for adding comment on a photo";
	
			bool commentPosted = false;
			QString errStr;
			errStr.clear();
			
			// Remove the "jsonFlickrApi(" from starting of the response and ")" from its end
			response.remove(0, 14);
			response.chop(1);
			
			bool ok;
			SmfPluginUtil util;
			QVariantMap result = util.parse(response, &ok).toMap();
			if (!ok) 
				{
				qDebug()<<"An error occurred during json parsing";
				aRetType = SmfRequestError;
				return SmfPluginErrParsingFailed;
				}
	
			if(response.contains(QByteArray("fail")))
				{
				errStr.append(result["message"].toString());
				}
			else
				{
				qDebug()<<"Comment posted on the photo";
				commentPosted = true;
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
				qDebug()<<"comment posted ? "<<commentPosted;
				aResult->setValue(commentPosted);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				}
			}
		else
			{
			qDebug()<<"Service unsupported, currently only SmfContactRetrievePosts and SmfContactPostDirected!!!";
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
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
#if 0
	qDebug()<<("FlickrGalleryPlugin::responseAvailable");
	Q_UNUSED(aPageResult)
	SmfPluginError error = SmfPluginErrNetworkError;

	if( !aResponse || (0 == aResponse->size()) )
		{
		qDebug()<<("Response is NULL or empty");
		aRetType = SmfRequestError;
		return error;
		}
	
	QByteArray response(*aResponse);
	delete aResponse;
	
	//Write the response to a file
	QFile file("c:\\data\\flickrresponse.txt");
	qDebug()<<("response data written to c:\\data\\flickrresponse.txt");
			
	if (!file.open(QIODevice::Append | QIODevice::Text));
	file.write(response);
	file.close();
	
	qDebug()<<("Gallery response size = "+QString::number(response.size(), 10));
	
	QList<SmfPicture> list;
	
	if(SmfTransportOpNoError == aTransportResult)
		{
		  qDebug()<<("No transport error");
		  QVariantMap map1;
		  bool resType=response.startsWith("<?xml");
		  if(resType) //XML REsponse
			{
		       qDebug()<<("Before XML Parser--");
			  
		  }
		  else //JSON RESPONSE
		  {
			  response.remove(0, 14);
			  response.chop(1);
			  bool ok;
			  qDebug()<<("Before JSON Parser--");
	
			  SmfPluginUtil util;
			  QVariant result= util.parse(response, &ok);
			  if (!ok) 
			   {
					qDebug()<<("An error occurred during json parsing");
					aRetType = SmfRequestError;
					return SmfPluginErrParsingFailed;
			   }
												
			   QVariantMap map1 = result.toMap();
		  }
		  if( SmfPictureGetAlbums == aOperation )
			  {}
 
			  }
		  else if(SmfPictureGetPictures == aOperation)
		  {}//end of   if(SmfPictureGetPictures == aOperation)
		 else if(SmfPicturePostComment == aOperation)
			 {}
		 else if(SmfPictureDescription == aOperation)
			 {}
		 else if(SmfPictureUpload == aOperation)
		    {
		      bool result;
		      qDebug()<<("SmfPictureUpload");
		       QXmlStreamReader xml(response);
		 	   while (!xml.atEnd())
		 	   {
		 		 xml.readNext();
		 		 if (xml.tokenType() == QXmlStreamReader::StartElement)
		 		  {
		 		    
		 		    qDebug()<<("inside tag");
		 				//If the tag is contact
		 				if (xml.name() == "photoid")
		 					{
		 					qDebug()<<("photoid tag found");
		 					result=TRUE;
                            }
		 				else
		 					result=FALSE;
		 				}
		 			}
		 		
		 		aResult->setValue(result);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
						
			}
		 else if (SmfPictureMultiUpload == aOperation)
			{
		       bool result;
			   qDebug()<<("SmfPictureUpload");
			   QXmlStreamReader xml(response);
			   while (!xml.atEnd())
			   {
				 xml.readNext();
				 if (xml.tokenType() == QXmlStreamReader::StartElement)
				 {
				 	qDebug()<<("inside tag");
				 	//If the tag is contact
				 	if (xml.name() == "photoid")
				 	{
				 		qDebug()<<("photoid tag found");
				 		result=TRUE;
		             }
				 	 else
				 		result=FALSE;
				 	}//end If
				 }//endWhile;
				 		
				 aResult->setValue(result);
				 if (listIndex < count)
					{
				
				      listIndex=listIndex+1;
				      error = SmfPluginErrNone;
					  aRetType = SmfSendRequestAgain;
					  
				     
					}
				 else
					{
				      //Final result sent 
				      count=1; //clear counter 
				      aRetType = SmfRequestComplete;
		 		      error = SmfPluginErrNone;
					}
		 
			}
		 else
			{
			qDebug()<<("Service unsupported!!!");
			aRetType = SmfRequestError;
			error = SmfPluginErrServiceNotSupported;
			}
		}//end if of if(SmfTransportOpNoError == aTransportResult)

	else if(SmfTransportOpOperationCanceledError == aTransportResult)
		{
		qDebug()<<("Operation Cancelled !!!");
		error = SmfPluginErrCancelComplete;
		aRetType = SmfRequestComplete;
		}

	else
		{
		qDebug()<<("Transport Error !!!");
		error = SmfPluginErrNetworkError;
		aRetType = SmfRequestError;
		}
	return error;
#endif
	
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
	m_description = "Flickr gallery plugin description";
	m_serviceUrl = QUrl(QString("http://api.flickr.com"));
	m_pluginId = "flickrgalleryplugin.qtplugin";
	m_authAppId = "0xE1D8C7D7";
	m_supportedInterfaces.append("org.symbian.smf.plugin.gallery/v0.2");
	QSettings iSettings;
	m_smfRegToken = iSettings.value("CMFlickrRegToken").toString();
	m_validity = iSettings.value("FlckrExpiryTime").toDateTime();
	}


/*
 * Export Macro
 * plugin name : flickrGalleryplugin
 * plugin class : FlickrGalleryPlugin
 */
Q_EXPORT_PLUGIN2( flickrgalleryplugin, FlickrGalleryPlugin )
