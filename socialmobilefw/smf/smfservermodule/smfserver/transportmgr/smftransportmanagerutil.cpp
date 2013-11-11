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
 * Manasij Roy, Nalina Hariharan
 * 
 * Description:
 * The Transport Manager Utility class provides the http/https transaction 
 * methods for the smf framework
 *
 */


// Include files
#define EMULATORTESTING

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <zlib.h>
#ifdef EMULATORTESTING
// For proxy settings on emulator only - REMOVE for device
#include <QNetworkProxy>
#endif

#include "smftransportmanagerutil.h"
#include "smfpluginmanager.h"

// Static Global constants
static const char kSmfUserAgent[] = "SmfFrameWork";
static const char kSmfCacheControl[] = "no-cache";
static const char kSmfAcceptEncoding[] = "gzip";

// Static data initialisation
SmfTransportManagerUtil* SmfTransportManagerUtil::m_myInstance = NULL;

/**
 * Method to get the instance of SmfTransportManagerUtil class
 * @return The instance of SmfTransportManager class
 */
SmfTransportManagerUtil* SmfTransportManagerUtil::getInstance ( )
	{
	if(NULL == m_myInstance)
		m_myInstance = new SmfTransportManagerUtil( );
	return m_myInstance;
	}

/**
 * Constructor with default argument
 * @param aParent The parent object
 */
SmfTransportManagerUtil::SmfTransportManagerUtil ( )
		: m_networkAccessManager ( this )
	{
	// Storage of settings (Only data usage as of now)
	m_settings = new QSettings("Sasken", "SmfTransportManager");

	// Create the settings if no data is stored
	if( !m_settings->contains("Sent Data") )
		m_settings->setValue("Sent Data", 0);
	if( !m_settings->contains("Received Data") )
		m_settings->setValue("Received Data", 0);
	
#ifdef EMULATORTESTING
	qDebug()<<"Using PROXY SETTINGS!!!, change for device testing";
	
	// Reading the keys, CSM Stubbed - START
	QFile winFile("c:\\data\\DoNotShare.txt");
	if (!winFile.open(QIODevice::ReadOnly))
		{
		qDebug()<<"File to read the windows username and password could not be opened, returning!!!";
		return;
		}
	
	QByteArray winArr = winFile.readAll();
	QList<QByteArray> winList = winArr.split(' ');
	winFile.close();
	
	QString httpUser(winList[0]);
	QString httpPass(winList[1]);

    // For proxy settings on emulator only - REMOVE for device
    QString httpProxy = "10.1.0.214";
    QString httpPort = "3128";
    
    //==Classes used from Network Module==
    QNetworkProxy proxy;

    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(httpProxy);
    proxy.setPort(httpPort.toInt());
    proxy.setUser(httpUser);
    proxy.setPassword(httpPass);
		
    QNetworkProxy::setApplicationProxy(proxy);
#endif
	}


/**
 * Destructor
 */
SmfTransportManagerUtil::~SmfTransportManagerUtil ( )
	{
	m_activeNetwReplyList.clear();
	
	if(m_settings)
		delete m_settings;
	
	if(m_myInstance)
		delete m_myInstance;
	}


/**
 * Method that does a http GET request. Returns the unique QNetworkReply
 * instance for this transaction to the plugin manager to trace the 
 * proper response.
 * @param aRequest The request formed by the plugin
 * @param aUrlList The list of accessible Urls for this plugin
 * @param aSOPCompliant [out] Output parameter indicating Same Origin 
 * Policy Compliance. Contains true if the request complies to the policy,
 * else false. If it is false, the network request will not performed
 * @return The unique QNetworkReply instance for this transaction
 */
QNetworkReply* SmfTransportManagerUtil::get ( 
		QNetworkRequest &aRequest,
		const QList<QUrl> &aUrlList,
		bool &aSOPCompliant )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::get()";;
	bool found = false;
	QString hostName = aRequest.url().host();
	
	// Same Origin Policy checking
	// Check if the host name of the url in the request created by the 
	// plugin is among the authorised url host name list
	foreach(QUrl url, aUrlList)
		{
		if(0 == hostName.compare(url.host()))
			{
			found = true;
			break;
			}
		}
	
	if(found)
		{
		aSOPCompliant = true;

		// Set header to accept gzip encoded data
		aRequest.setRawHeader("Accept-encoding", kSmfAcceptEncoding);
		
		// Set the cache control
		aRequest.setRawHeader("Cache-Control", kSmfCacheControl);
		
		// Put the same user agent for all requests sent by Smf
		aRequest.setRawHeader("User-Agent", kSmfUserAgent);
		
		QNetworkReply* reply = m_networkAccessManager.get(aRequest);
		qDebug()<<"QNetworkReply of get() = "<<(long)reply;
	
		connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), 
				this, SLOT(networkReplyFinished(QNetworkReply *)));
		
		connect(&m_networkAccessManager, SIGNAL(error(QNetworkReply::NetworkError)), 
				this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
		
		m_activeNetwReplyList.append(reply);
		
		return reply;
		}
	else
		{
		// Error :: SAME ORIGIN POLICY violation!!!
		aSOPCompliant = false;
		qDebug()<<"SOP violation";
		return NULL;
		}
	}


/**
 * Method that does a http POST request. Returns the unique QNetworkReply
 * instance for this transaction to the plugin manager to trace the 
 * proper response.
 * @param aRequest The request formed by the plugin
 * @param aPostData The data to be posted via http POST request
 * @param aUrlList The list of accessible Urls for this plugin
 * @param aSOPCompliant [out] Output parameter indicating Same Origin 
 * Policy Compliance. Contains true if the request complies to the policy,
 * else false. If it is false, the network request will not performed
 * @return The unique QNetworkReply instance for this transaction
 */
QNetworkReply* SmfTransportManagerUtil::post ( 
		QNetworkRequest &aRequest, 
		const QByteArray& aPostData,
		const QList<QUrl> &aUrlList,
		bool &aSOPCompliant )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::post()";
	bool found = false;
	QString hostName = aRequest.url().host();
	
	// Same Origin Policy checking
	// Check if the host name of the url in the request created by the 
	// plugin is among the authorised url host name list
	foreach(QUrl url, aUrlList)
		{
		if(0 == hostName.compare(url.host()))
			{
			found = true;
			break;
			}
		}
	
	if(found)
		{
		aSOPCompliant = true;

		// Set header to accept gzip encoded data
		aRequest.setRawHeader("Accept-encoding", kSmfAcceptEncoding);
		
		// Set the cache control
		aRequest.setRawHeader("Cache-Control", kSmfCacheControl);
		
		// Put the same user agent for all requests sent by Smf
		aRequest.setRawHeader("User-Agent", kSmfUserAgent);
		
#ifdef DETAILEDDEBUGGING
		qDebug()<<"post data size is = "<<aPostData.size();
		qDebug()<<"post data is = "<<QString(aPostData);
#endif
		
		QNetworkReply* reply = m_networkAccessManager.post(aRequest, aPostData);
		qDebug()<<"QNetworkReply of post() = "<<(long)reply;
	
		connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), 
				this, SLOT(networkReplyFinished(QNetworkReply *)));
		
		connect(&m_networkAccessManager, SIGNAL(error(QNetworkReply::NetworkError)), 
				this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
		
		m_activeNetwReplyList.append(reply);
		
		return reply;
		}
	else
		{
		// Error :: SAME ORIGIN POLICY violation!!!
		aSOPCompliant = false;
		qDebug()<<"SOP violation";
		return NULL;
		}
	}


/**
 * Method that does a http HEAD request. Returns the unique QNetworkReply
 * instance for this transaction to the plugin manager to trace the 
 * proper response.
 * @param aRequest The request formed by the plugin
 * @param aUrlList The list of accessible Urls for this plugin
 * @param aSOPCompliant [out] Output parameter indicating Same Origin 
 * Policy Compliance. Contains true if the request complies to the policy,
 * else false. If it is false, the network request will not performed
 * @return The unique QNetworkReply instance for this transaction
 */
QNetworkReply* SmfTransportManagerUtil::head ( 
		QNetworkRequest& aRequest,
		const QList<QUrl> &aUrlList,
		bool &aSOPCompliant )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::head()";
	bool found = false;
	QString hostName = aRequest.url().host();

	// Same Origin Policy checking
	// Check if the host name of the url in the request created by the 
	// plugin is among the authorised url host name list
	foreach(QUrl url, aUrlList)
		{
		if(0 == hostName.compare(url.host()))
			{
			found = true;
			break;
			}
		}
	
	if(found)
		{
		aSOPCompliant = true;
		
		// Set header to accept gzip encoded data
		aRequest.setRawHeader("Accept-encoding", kSmfAcceptEncoding);
		
		// Set the cache control
		aRequest.setRawHeader("Cache-Control", kSmfCacheControl);
		
		// Put the same user agent for all requests sent by Smf
		aRequest.setRawHeader("User-Agent", kSmfUserAgent);
		
		QNetworkReply* reply = m_networkAccessManager.head(aRequest);
		qDebug()<<"QNetworkReply of head() = "<<(long)reply;
	
		connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), 
				this, SLOT(networkReplyFinished(QNetworkReply *)));
		
		connect(&m_networkAccessManager, SIGNAL(error(QNetworkReply::NetworkError)), 
				this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
		
		m_activeNetwReplyList.append(reply);
		
		return reply;
		}
	else
		{
		// Error :: SAME ORIGIN POLICY violation!!!
		aSOPCompliant = false;
		qDebug()<<"SOP violation";
		return NULL;
		}
	}


/**
 * Method that does a http PUT request. Returns the unique QNetworkReply
 * instance for this transaction to the plugin manager to trace the 
 * proper response.
 * @param aRequest The request formed by the plugin
 * @param aPostData The data to be posted via http PUT request
 * @param aUrlList The list of accessible Urls for this plugin
 * @param aSOPCompliant [out] Output parameter indicating Same Origin 
 * Policy Compliance. Contains true if the request complies to the policy,
 * else false. If it is false, the network request will not performed
 * @return The unique QNetworkReply instance for this transaction
 */
QNetworkReply* SmfTransportManagerUtil::put ( 
		QNetworkRequest &aRequest,
		const QByteArray& aPostData,
		const QList<QUrl> &aUrlList,
		bool &aSOPCompliant )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::put()";
	bool found = false;
	QString hostName = aRequest.url().host();

	// Same Origin Policy checking
	// Check if the host name of the url in the request created by the 
	// plugin is among the authorised url host name list
	foreach(QUrl url, aUrlList)
		{
		if(0 == hostName.compare(url.host()))
			{
			found = true;
			break;
			}
		}
	
	if(found)
		{
		aSOPCompliant = true;
		
		// Set header to accept gzip encoded data
		aRequest.setRawHeader("Accept-encoding", kSmfAcceptEncoding);
		
		// Set the cache control
		aRequest.setRawHeader("Cache-Control", kSmfCacheControl);
		
		// Put the same user agent for all requests sent by Smf
		aRequest.setRawHeader("User-Agent", kSmfUserAgent);
		
		QNetworkReply* reply = m_networkAccessManager.put(aRequest, aPostData);
		qDebug()<<"QNetworkReply of put() = "<<(long)reply;
	
		connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), 
				this, SLOT(networkReplyFinished(QNetworkReply *)));
		
		connect(&m_networkAccessManager, SIGNAL(error(QNetworkReply::NetworkError)), 
				this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
		
		m_activeNetwReplyList.append(reply);
		
		return reply;
		}
	else
		{
		// Error :: SAME ORIGIN POLICY violation!!!
		aSOPCompliant = false;
		qDebug()<<"SOP violation";
		return NULL;
		}
	}


/**
 * Method that does a http DELETE request. Returns the unique QNetworkReply
 * instance for this transaction to the plugin manager to trace the 
 * proper response.
 * @param aRequest The request formed by the plugin
 * @param aUrlList The list of accessible Urls for this plugin
 * @param aSOPCompliant [out] Output parameter indicating Same Origin 
 * Policy Compliance. Contains true if the request complies to the policy,
 * else false. If it is false, the network request will not performed
 * @return The unique QNetworkReply instance for this transaction
 */
QNetworkReply* SmfTransportManagerUtil::deleteResource ( 
		QNetworkRequest &aRequest,
		const QList<QUrl> &aUrlList,
		bool &aSOPCompliant )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::deleteResource()";
	bool found = false;
	QString hostName = aRequest.url().host();

	// Same Origin Policy checking
	// Check if the host name of the url in the request created by the 
	// plugin is among the authorised url host name list
	foreach(QUrl url, aUrlList)
		{
		if(0 == hostName.compare(url.host()))
			{
			found = true;
			break;
			}
		}
	
	if(found)
		{
		aSOPCompliant = true;
		
		// Set header to accept gzip encoded data
		aRequest.setRawHeader("Accept-encoding", kSmfAcceptEncoding);
		
		// Set the cache control
		aRequest.setRawHeader("Cache-Control", kSmfCacheControl);
		
		// Put the same user agent for all requests sent by Smf
		aRequest.setRawHeader("User-Agent", kSmfUserAgent);
		
		QNetworkReply* reply = m_networkAccessManager.deleteResource(aRequest);
		qDebug()<<"QNetworkReply of deleteResource() = "<<(long)reply;
	
		connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), 
				this, SLOT(networkReplyFinished(QNetworkReply *)));
		
		connect(&m_networkAccessManager, SIGNAL(error(QNetworkReply::NetworkError)), 
				this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
		
		m_activeNetwReplyList.append(reply);
		
		return reply;
		}
	else
		{
		// Error :: SAME ORIGIN POLICY violation!!!
		aSOPCompliant = false;
		qDebug()<<"SOP violation";
		return NULL;
		}
	}


/**
 * Method that cancels the service request by cancelling the current 
 * http transaction.
 * @param aCancelReply The QNetworkReply instance whose transaction 
 * has to be cancelled
 * @return Returns true for success, else false 
 */
bool SmfTransportManagerUtil::cancelRequest ( 
		QNetworkReply *aCancelReply )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::cancelRequest()";
	
	// Checks if this transaction is running
	if( aCancelReply->isRunning() )
		{
		// Aborts the running transaction
		aCancelReply->abort();
		}
	// Close the connection
	aCancelReply->close();
	
	int index = m_activeNetwReplyList.indexOf(aCancelReply);
	if( index >= 0 )
		m_activeNetwReplyList.removeAt(index);
	
	return true;
	}



/*
 * Method that is called whenever a new network configuration is added to the system.
 * @param aResult SmfTransportResult
 */
void SmfTransportManagerUtil::configurationAdded ( const SmfTransportResult &aResult )
	{
	// Inform PM and plugins that a new IAP ia added, and hence resend the request
	SmfPluginManager::getInstance()->responseAvailable ( aResult, NULL, NULL );
	}

/*
 * Method that is called when the state of the network configuration changes.
 * @param aResult SmfTransportResult
 */
void SmfTransportManagerUtil::configurationChanged ( const SmfTransportResult &aResult )
	{
	// Inform PM and plugins that the IAP is changed, and hence resend the request
	SmfPluginManager::getInstance()->responseAvailable ( aResult, NULL, NULL );
	}

/*
 * Method that is called when a configuration is about to be removed from the system.
 * The removed configuration is invalid but retains name and identifier.
 * @param aResult SmfTransportResult
 */
void SmfTransportManagerUtil::configurationRemoved ( const SmfTransportResult &aResult )
	{
	// Inform PM and plugins that the IAP is to be deleted, and 
	// hence resend the request
	SmfPluginManager::getInstance()->responseAvailable ( aResult, NULL, NULL );

	}


/**
 * Method to indicate the http transaction has finished.
 * @param aNetworkReply The QNetworkReply instance for which the http 
 * transaction was made
 */
void SmfTransportManagerUtil::networkReplyFinished ( QNetworkReply *aNetworkReply )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::networkReplyFinished()";
	qDebug()<<"QNetworkReply = "<<(long)aNetworkReply;
	qDebug()<<"QNetworkReply error code = "<<aNetworkReply->error();
	
	// remove the QNetworkReply instance from the list of outstanding transactions
	int index = m_activeNetwReplyList.indexOf(aNetworkReply);
	if( index >= 0 )
		m_activeNetwReplyList.removeAt(index);
	
	// indicate the result of transaction to the plugin manager
	SmfTransportResult trResult;
	convertErrorType(aNetworkReply->error(), trResult);
	
	// Read the response from the device
	QByteArray response = aNetworkReply->readAll();
	qDebug()<<"Response size is = "<<response.size();
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Response is = "<<QString(response);
#endif

	// Store the number of bytes of data received
	bool converted = false;
	quint64 data = m_settings->value("Received Data").toULongLong(&converted);
	if(converted)
		{
		data += response.size();
		m_settings->setValue("Received Data", data);
		}

	SmfError error = SmfUnknownError;
	QByteArray *arr = NULL;
	
#ifdef DETAILEDDEBUGGING
	foreach(QByteArray arr, aNetworkReply->rawHeaderList())
			qDebug()<<QString(arr)<<" = "<<QString(aNetworkReply->rawHeader(arr));
#endif
	
	// Check if the http response header has the raw header "Content-Encoding:gzip"
	bool gzipEncoded = false;
	QByteArray headerKey("Content-Encoding");
	if(aNetworkReply->hasRawHeader(headerKey))
		{
		qDebug()<<"Content-Encoding:"<<QString(aNetworkReply->rawHeader(headerKey));
		
		// If http response header has the raw header "Content-Encoding:gzip"
		if( "gzip" == QString(aNetworkReply->rawHeader(headerKey)))
			{
			QByteArray firstByte(1, response[0]);
			QByteArray secondByte(1, response[1]);
			
#ifdef DETAILEDDEBUGGING
			qDebug()<<"first byte : "<<QString(firstByte.toHex());
			qDebug()<<"second byte : "<<QString(secondByte.toHex());
#endif
			
			// And the header says the response is a valid gzip data. 
			// If so, inflate the gzip deflated data
			if((QString("1f") == QString(firstByte.toHex())) && (QString("8b") == QString(secondByte.toHex())) )
				{
				gzipEncoded = true;
				qDebug()<<"Response is gzip encoded!!!";
				arr = inflateResponse(response, error);
				if(!arr)
					trResult = SmfTransportOpGzipError;
			}
		else
			{
				qDebug()<<"Header says gzip encoded, but response is not a valid data, so ignoring the header!!!";
				arr = new QByteArray(response);
				}
			}
		else
			{
			qDebug()<<"Unsupported encoding format!!! - error";
			trResult = SmfTransportOpUnsupportedContentEncodingFormat;
			}
		}
	
	else
		{
		qDebug()<<"Response is not gzip encoded";
		arr = new QByteArray(response);
		}
	
	// Write the uncompressed data to a file
#ifdef DETAILEDDEBUGGING
	if(arr)
		{
		QFile file1("c:\\data\\networkResponse.txt");
		if (!file1.open(QIODevice::WriteOnly))
			qDebug()<<"File to write the network response could not be opened";
		file1.write(*arr);
		file1.close();
		}
#endif

	SmfPluginManager::getInstance()->responseAvailable ( trResult, aNetworkReply, arr );
	}


/**
 * Method to inflate a gzipped network response.
 * @param aResponse The QByteArray holding the gzip encoded data
 * @param aError Argument indicating error
 * @return a QByteArray* containing the inflated data. If inflation fails, 
 * NULL is returned
 */
QByteArray* SmfTransportManagerUtil::inflateResponse ( QByteArray &aResponse, SmfError& aError )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::inflateResponse()";
	
	// Read the response from the device
	qDebug()<<"Encoded response content size = "<<aResponse.size();
	
	if(aResponse.size() <= 0)
		{
		aError = SmfTMUnknownContentError;
		return NULL;
		}
	
	// Get the uncompressed size of the response (last 4 bytes of the compressed data as per GZIP format spec)
	QByteArray sizeStr;
	for(int count = 1 ; count <= 4 ; count++)
		sizeStr.append(aResponse[aResponse.size()-count]);
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Size string as a string = "<<QString(sizeStr.toHex());
#endif
	bool ok = false;
	int uncomSize = sizeStr.toHex().toInt(&ok, 16);
	qDebug()<<"Size of uncompressed data = "<<uncomSize;

	// Initialize the data required for zlib method call
	z_stream stream;
	unsigned char *out = new unsigned char[uncomSize];
	if(out)
		qDebug()<<"Memory allocated for output buffer";
	else
		{
		//Error
		qDebug()<<"Memory not allocated for output buffer!!!";
		aError = SmfMemoryAllocationFailure;
		return NULL;
		}
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;
	
	int ret = inflateInit2(&stream, 16+MAX_WBITS);
#ifdef DETAILEDDEBUGGING
	qDebug()<<"return value of inflateInit2() = "<<ret;
#endif
	if(Z_OK != ret)
		{
		qDebug()<<"Error in inflateInit2, returning...";
		delete[] out;
		if(Z_STREAM_ERROR == ret)
			aError = SmfTMGzipStreamError;
		else if(Z_MEM_ERROR == ret)
			aError = SmfTMGzipMemoryError;
		else if(Z_DATA_ERROR == ret)
			aError = SmfTMGzipDataError;
		else
			aError = SmfUnknownError;
		return NULL;
		}

	// Initialize the data required for inflate() method call
	stream.avail_in = aResponse.size();
	stream.next_in = (unsigned char *)aResponse.data();
	stream.avail_out = uncomSize;
	stream.next_out = out;
	
	ret = inflate(&stream, Z_NO_FLUSH);
#ifdef DETAILEDDEBUGGING
	qDebug()<<"return value of inflate() = "<<ret;
#endif
	
	switch (ret) 
		{
		// fall through
		case Z_NEED_DICT:
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			{
			qDebug()<<"Error in inflate, returning...";
			(void)inflateEnd(&stream);
			delete[] out;
			aError = SmfTMGzipDataError;
			return NULL;
			}
		}

	qDebug()<<"total bytes read so far = "<<stream.total_in;
	qDebug()<<"total bytes output so far = "<<stream.total_out;

	// Write the inflated data to a QByteArray
	QByteArray *uncompressedData = new QByteArray((char *)out, stream.total_out);
	delete[] out;
	
	// If there is some unwanted data at the end of uncompressed data, chop them
	int chopLength = uncompressedData->size() - uncomSize;
#ifdef DETAILEDDEBUGGING
	qDebug()<<"old size of uncompressed data = "<<uncompressedData->size();
#endif
	uncompressedData->chop(chopLength);
#ifdef DETAILEDDEBUGGING
	qDebug()<<"new size of uncompressed data = "<<uncompressedData->size();
#endif
	
	return uncompressedData;
	}


/**
 * Method to deflate a network request to gzip format.
 * @param aResponse The QByteArray that has to be gzipped
 * @param aError Argument indicating error
 * @return a QByteArray* containing the deflated data. If deflation fails, 
 * NULL is returned
 */
QByteArray* SmfTransportManagerUtil::deflateRequest( QByteArray &aResponse, SmfError& aError )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::deflateRequest()";
	
	if(aResponse.size() <= 0)
		{
		aError = SmfTMUnknownContentError;
		return NULL;
		}
	
	// Initialize the data required for zlib initialize method call
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.next_in = (unsigned char *)aResponse.data();
	stream.avail_in = aResponse.size();
	stream.total_out = 0;

	int level = Z_DEFAULT_COMPRESSION;
	int method = Z_DEFLATED;
	int windowBits = 16+MAX_WBITS;
	int mem_level = 8;
	int strategy = Z_DEFAULT_STRATEGY;
	
	// Call deflateInit2 for gzip compression initialization
	int initError = deflateInit2(&stream, level, method, windowBits, mem_level, strategy);
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Return value of deflateInit2() = "<<initError;
	qDebug()<<"Error msg if any = "<<QString(zError(initError));
#endif
	
	if(Z_OK != initError)
		{
		qDebug()<<"deflateInit2() failed, returning error = "<<QString(zError(initError));
		
		if(Z_STREAM_ERROR == initError)
			aError = SmfTMGzipStreamError;
		else if(Z_MEM_ERROR == initError)
			aError = SmfTMGzipMemoryError;
		else if(Z_DATA_ERROR == initError)
			aError = SmfTMGzipDataError;
		else
			aError = SmfUnknownError;
		return NULL;
		}

	// Initialize the data required for inflate() method call
	int out_size = (int)((aResponse.size()*1.01)+17);
	unsigned char *out = new unsigned char[out_size];
	if(out)
		{
		qDebug()<<"Memory allocated for output buffer with size = "<<out_size;
		memset(out, 0, out_size);
		}
	else
		{
		//Error
		qDebug()<<"Memory not allocated for output buffer!!!";
		
		deflateEnd(&stream);
		aError = SmfTMGzipDataError;
		return NULL;
		}

	stream.next_out = out;
	stream.avail_out = out_size;
	
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Before calling deflate()";
	qDebug()<<"next_in = "<<(long)stream.next_in;
	qDebug()<<"avail_in = "<<stream.avail_in;
	qDebug()<<"next_out = "<<(long)stream.next_out;
	qDebug()<<"avail_out = "<<stream.avail_out;
	qDebug()<<"total_in = "<<stream.total_in;
	qDebug()<<"total_out = "<<stream.total_out;
#endif
	
	int deflateStatus;
	do
		{
		stream.next_out = out + stream.total_out;
		stream.avail_out = out_size - stream.total_out;
		
		deflateStatus = deflate(&stream, Z_FINISH);
		qDebug()<<"Return value of deflate() is = "<<deflateStatus;
		qDebug()<<"Error msg if any = "<<QString(zError(deflateStatus));
		
#ifdef DETAILEDDEBUGGING
		qDebug()<<"avail_in = "<<stream.avail_in;
		qDebug()<<"avail_out = "<<stream.avail_out;
		qDebug()<<"total_in = "<<stream.total_in;
		qDebug()<<"total_out = "<<stream.total_out;
#endif
		
		}while(Z_OK == deflateStatus);
	
     if (Z_STREAM_END != deflateStatus)  
     {  
         QString errorMsg;  
         switch (deflateStatus)  
         {  
             case Z_ERRNO:  
                 errorMsg.append("Error occured while reading file.");
                 aError = SmfUnknownError;
                 break;  
             case Z_STREAM_ERROR:  
                 errorMsg.append("The stream state was inconsistent (e.g., next_in or next_out was NULL).");
                 aError = SmfTMGzipStreamError;
                 break;  
             case Z_DATA_ERROR:  
                 errorMsg.append("The deflate data was invalid or incomplete.");
                 aError = SmfTMGzipDataError;
                 break;  
             case Z_MEM_ERROR:  
                 errorMsg.append("Memory could not be allocated for processing.");
                 aError = SmfTMGzipMemoryError;
                 break;  
             case Z_BUF_ERROR:  
                 errorMsg.append("Ran out of output buffer for writing compressed bytes.");
                 aError = SmfTMGzipMemoryError;
                 break;  
             case Z_VERSION_ERROR:  
                 errorMsg.append("The version of zlib.h and the version of the library linked do not match.");
                 aError = SmfUnknownError;
                 break;  
             default:  
                 errorMsg.append("Unknown error code.");
                 aError = SmfUnknownError;
         }  
   
         qDebug()<<"Error string for deflate() is = "<<errorMsg;
         
         // Free data structures that were dynamically created for the stream.  
         deflateEnd(&stream);  
   
         return NULL;  
     } 
			
     int retVal = deflateEnd(&stream);
	qDebug()<<"Return value of deflateEnd() = "<<retVal;
	qDebug()<<"Error msg if any = "<<QString(zError(retVal));

#ifdef DETAILEDDEBUGGING
	/*QByteArray byteArray;
	for (int i=0; i<stream.total_out;i++)
		{
		QString str(out[i]);
		byteArray.clear();
		byteArray.append(str.toAscii());
		qDebug()<<QString("out[%1] = '%2'").arg(i).arg(QString(byteArray.toHex()));
		}*/
#endif
	
	// Write the inflated data to a QByteArray
	QByteArray *compressedData = new QByteArray((char*)out, stream.total_out);
	delete[] out;
	qDebug()<<"Number of bytes of compressed data = "<<compressedData->size();
	
#ifdef DETAILEDDEBUGGING
	qDebug()<<"compressed data = "<<QString(compressedData->toHex());
#endif
	
	return compressedData;
	}



/**
 * Method called when the QNetworkReply detects an error in processing.
 * @param aError The QNetworkReply error code 
 */
void SmfTransportManagerUtil::networkReplyError ( QNetworkReply::NetworkError aError )
	{
	qDebug()<<"Inside SmfTransportManagerUtil::networkReplyError()";
	
	// indicate the error to the plugin manager
	SmfTransportResult trResult;
	convertErrorType(aError, trResult);
	
	SmfPluginManager::getInstance()->responseAvailable ( trResult, NULL, NULL );
	}


/**
 * Method to convert QNetworkReply Error to the type SmfTransportResult 
 * QNetworkRequest received before executing the web query.
 * @param aError The QNetworkReply Error
 * @param aResult [out] The SmfTransportResult error
 */
void SmfTransportManagerUtil::convertErrorType( const QNetworkReply::NetworkError &aError, 
		SmfTransportResult &aResult )
	{
	switch(aError)
		{
		case QNetworkReply::NoError:
			aResult = SmfTransportOpNoError;
			break;
			
		case QNetworkReply::ConnectionRefusedError:
			aResult = SmfTransportOpConnectionRefusedError;
			break;
			
		case QNetworkReply::RemoteHostClosedError:
			aResult = SmfTransportOpRemoteHostClosedError;
			break;
			
		case QNetworkReply::HostNotFoundError:
			aResult = SmfTransportOpHostNotFoundError;
			break;
			
		case QNetworkReply::TimeoutError:
			aResult = SmfTransportOpTimeoutError;
			break;
			
		case QNetworkReply::OperationCanceledError:
			aResult = SmfTransportOpOperationCanceledError;
			break;
			
		case QNetworkReply::SslHandshakeFailedError:
			aResult = SmfTransportOpSslHandshakeFailedError;
			break;
			
		case QNetworkReply::ProxyConnectionRefusedError:
			aResult = SmfTransportOpProxyConnectionRefusedError;
			break;
			
		case QNetworkReply::ProxyConnectionClosedError:
			aResult = SmfTransportOpProxyConnectionClosedError;
			break;
			
		case QNetworkReply::ProxyNotFoundError:
			aResult = SmfTransportOpProxyNotFoundError;
			break;
			
		case QNetworkReply::ProxyTimeoutError:
			aResult = SmfTransportOpProxyTimeoutError;
			break;
			
		case QNetworkReply::ProxyAuthenticationRequiredError:
			aResult = SmfTransportOpProxyAuthenticationRequiredError;
			break;
			
		case QNetworkReply::ContentAccessDenied:
			aResult = SmfTransportOpContentAccessDenied;
			break;
			
		case QNetworkReply::ContentOperationNotPermittedError:
			aResult = SmfTransportOpContentOperationNotPermittedError;
			break;
			
		case QNetworkReply::ContentNotFoundError:
			aResult = SmfTransportOpContentNotFoundError;
			break;
			
		case QNetworkReply::AuthenticationRequiredError:
			aResult = SmfTransportOpAuthenticationRequiredError;
			break;
			
		case QNetworkReply::ContentReSendError:
			aResult = SmfTransportOpContentReSendError;
			break;
			
		case QNetworkReply::ProtocolUnknownError:
			aResult = SmfTransportOpProtocolUnknownError;
			break;

		case QNetworkReply::ProtocolInvalidOperationError:
			aResult = SmfTransportOpProtocolInvalidOperationError;
			break;
			
		case QNetworkReply::UnknownNetworkError:
			aResult = SmfTransportOpUnknownNetworkError;
			break;
			
		case QNetworkReply::UnknownProxyError:
			aResult = SmfTransportOpUnknownProxyError;
			break;
			
		case QNetworkReply::UnknownContentError:
			aResult = SmfTransportOpUnknownContentError;
			break;
			
		case QNetworkReply::ProtocolFailure:
			aResult = SmfTransportOpProtocolFailure;
			break;
			
		default:
			;
		}
	}

