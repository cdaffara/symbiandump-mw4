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

#ifndef SMFTRANSPORTMANAGERUTIL_H_
#define SMFTRANSPORTMANAGERUTIL_H_

#include <QNetworkReply>
#include <smfglobal.h>
#include <smfserverglobal.h>

// Forward declaration
class QNetworkAccessManager;
class QSettings;

/**
 * The Transport Manager Utility class provides the http/https transaction 
 * methods for the smf framework
 * 
 */
class SmfTransportManagerUtil : public QObject
	{
	Q_OBJECT
	
public:
	/**
	 * Method to get the instance of SmfTransportManagerUtil class
	 * @return The instance of SmfTransportManagerUtil class
	 */
	static SmfTransportManagerUtil* getInstance ( );
	
	/**
	 * Destructor
	 */
	~SmfTransportManagerUtil ( );
	

public:
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
    QNetworkReply* get ( QNetworkRequest &aRequest,
    		const QList<QUrl> &aUrlList,
    		bool &aSOPCompliant );

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
    QNetworkReply* post ( QNetworkRequest &aRequest, 
    		const QByteArray& aPostData,
    		const QList<QUrl> &aUrlList,
    		bool &aSOPCompliant );
    
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
    QNetworkReply* head ( QNetworkRequest &aRequest,
    		const QList<QUrl> &aUrlList,
    		bool &aSOPCompliant );
    
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
    QNetworkReply* put ( QNetworkRequest &aRequest,
    		const QByteArray& aPostData,
    		const QList<QUrl> &aUrlList,
    		bool &aSOPCompliant );
	
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
    QNetworkReply* deleteResource ( QNetworkRequest &aRequest,
    		const QList<QUrl> &aUrlList,
    		bool &aSOPCompliant );
    
	/**
	 * Method that cancels the service request by cancelling the current 
	 * http transaction.
	 * @param aCancelReply The QNetworkReply instance whose transaction 
	 * has to be cancelled
	 * @return Returns true for success, else false 
	 */
	bool cancelRequest ( QNetworkReply *aCancelReply );
	
public slots:
	/*
	 * Method that is called whenever a new network configuration is added to the system.
	 * @param aResult SmfTransportResult
	 */
	void configurationAdded ( const SmfTransportResult &aResult );

	/*
	 * Method that is called when the state of the network configuration changes.
	 * @param aResult SmfTransportResult
	 */
	void configurationChanged ( const SmfTransportResult &aResult );

	/*
	 * Method that is called when a configuration is about to be removed from the system.
	 * The removed configuration is invalid but retains name and identifier.
	 * @param aResult SmfTransportResult
	 */
	void configurationRemoved ( const SmfTransportResult &aResult );
	
private slots:
	/**
	 * Method to indicate http transaction has finished.
	 * @param aNetworkReply The QNetworkReply instance for which the http 
	 * transaction was made
	 */
	void networkReplyFinished ( QNetworkReply *aNetworkReply );
	
	/**
	 * Method called when the QNetworkReply detects an error in processing.
	 * @param aError The QNetworkReply error code 
	 */
	void networkReplyError ( QNetworkReply::NetworkError aError );
	
private:
	/**
	 * Method to convert QNetworkReply Error to the type SmfTransportResult 
	 * QNetworkRequest received before executing the web query.
	 * @param aError The QNetworkReply Error
	 * @param aResult [out] The SmfTransportResult error
	 */
	void convertErrorType( const QNetworkReply::NetworkError &aError,  
			SmfTransportResult &aResult );
	
	/**
	 * Method to inflate a gzipped network response.
	 * @param aResponse The QByteArray holding the gzip encoded data
	 * @param aError Argument indicating error
	 * @return a QByteArray* containing the inflated data. If inflation fails, 
	 * NULL is returned
	 */
	QByteArray* inflateResponse ( QByteArray &aResponse, SmfError& aError );
	
	/**
	 * Method to deflate a network request to gzip format.
	 * @param aResponse The QByteArray that has to be gzipped
	 * @param aError Argument indicating error
	 * @return a QByteArray* containing the deflated data. If deflation fails, 
	 * NULL is returned
	 */
	QByteArray* deflateRequest( QByteArray &aResponse, SmfError& aError );
	
private:
	/**
	 * Constructor with default argument
	 */
	SmfTransportManagerUtil ( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfTransportManagerUtil ( const SmfTransportManagerUtil &aOther );
	
private:
	/**
	 * The single instance of SmfTransportManagerUtil
	 */
	static SmfTransportManagerUtil* m_myInstance;
	
	/**
	 * Member variable - QNetworkAccessManager instance
	 */
	QNetworkAccessManager m_networkAccessManager;
	
	/**
	 * Member variable - List of QNetworkReply awaiting response
	 */
	QList<QNetworkReply *> m_activeNetwReplyList;
	
	/**
	 * Member variable - instance of QSettings that stores network 
	 * data usage of Smf
	 */
	QSettings *m_settings;
	};


#endif /* SMFTRANSPORTMANAGERUTIL_H_ */
