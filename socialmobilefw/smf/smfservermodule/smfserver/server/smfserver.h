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
 * SMF Server component which handles the client requests and delegates 
 * them propoerly to the appropriate component
 * 
 */

#ifndef SMFSERVER_H
#define SMFSERVER_H

#include <QObject>
#include <QMap>
#include <smfglobal.h>
#include <smfcredmgrclientglobal.h>
#ifdef CLIENT_SERVER_TEST
	#include <QTimer>
	#include <QTextStream>
#endif

#include "smfserverglobal.h"

// Forward declarations(Other components of the SMF)
class SmfTransportManager;
class SmfPluginManager;
class SmfCredMgrClient;
class SmfProvider;
//Private implementation for different platforms
#ifdef Q_OS_SYMBIAN
	class SmfServerSymbian;
	class SmfServerSymbianSession;
#else
	class SmfServerQt;
	class SmfServerQtSession;
#endif


//For the time being, need to change later
typedef QString SmfInterfaceID;
//For the time being, need to change later
typedef QString SmfPluginID;
//TODO:- define proper enums after consulting with CM owner
typedef int NotificationType;

	
/**
 * SmfServer manages the client requests and delegates them to the appropriate SMF component,
 * service the clients synchronously or asynchronously
 */
class SmfServer : public QObject
	{
	Q_OBJECT
	
public:
	SmfServer(QObject* parent = 0);
	~SmfServer();
	
	/**
	 * Does the following,-
	 * First it'll check whether server is already running, if already 
	 * running it'll simply return
	 * else it'll start the server exe and initialize all other SMF 
	 * components returns whether server is started successfully or not
	 */
	bool startServer();
	
public:
	/**
	 * Requests Plugin Manager to get a list of plugin IDs who implement
	 * the interface interfaceID.
	 * This is used for SmfClient::GetServices () where we need a list of plugins
	 */
	void getPlugins ( const SmfInterfaceID& interfaceID, 
			QMap<SmfPluginID,SmfProvider>& pluginIDMap);
	
	/**
	 * Same as above, except this is used for rest of the requests where we need
	 * a particular plugin
	 */
	SmfPluginID getPlugin ( const SmfInterfaceID& interfaceID, SmfProvider provider);
	
	/**
	 * Requests the Credential Manager to filter out non-authorized plugin IDs
	 * from the list and get authorized plugins into authList.
	 */
	void getAuthorizedPlugins ( QList<SmfPluginID>& list, QList<SmfPluginID>& authList );
	
	/**
	 * Request CM API to get the list of authenticated plugins
	 * @see SmfCredMgrClient::authenticatedPluginList()
	 */
	QStringList getAuthenticatedPluginList ( QString RegistrationToken );
	
	/**
	 * Request the Plugin manager to get the data.
	 * @param requestID Corresponds to a client's session
	 * @param pluginID Plugin for which the request is intended
	 * @param interfaceID Interface name
	 * @param dataForPlugin Data to be sent for this request
	 */
	SmfError sendToPluginManager ( int requestID, SmfPluginID pluginID, 
			SmfInterfaceID interfaceID, SmfRequestTypeID requestTypeID,
			QByteArray dataForPlugin = QByteArray() );
	
	/**
	 * Request the Plugin manager to get the data.
	 * @param requestID Corresponds to a client's session
	 * @param pluginID Plugin for which the request is intended
	 * @param interfaceID Interface name
	 * @param dataForPlugin Data to be sent for this request
	 */
	SmfError sendToPluginManager ( SmfPluginID pluginID, 
			SmfInterfaceID interfaceID, SmfRequestTypeID requestTypeID,
			QByteArray dataForPlugin, QByteArray &outputData);
	
	/**
	 * Delegates the request to DSM and receives data synshronously.
	 * @param qtdataForDSM Data to be passed to DSM
	 * @param opcode Opcode
	 * @param qtdataFromDSM Data received from DSM
	 * @return Error value returned from DSM
	 */
	SmfError sendToDSM ( QByteArray qtdataForDSM, SmfRequestTypeID opcode,
			QByteArray& qtdataFromDSM );
	
public slots:
	/**
	 * This slot is called when Credential Manager is done with the autherizing
	 * the client for the first time. See isClientAuthorized() and authorizeClient().
	 * success specifies the success of the authorization, authID is the authentication
	 * ID in case its not same as SID of the client.
	 */
	void clientAuthorizationFinished ( bool success, SmfClientAuthID authID );
	
	/**
	 * This slot is called as a result of trigger from Plugin manager when the 
	 * parsed data is available.
	 * @param requestID The request ID for which result is available
	 * @param parsedData Serialized data
	 */
	void resultsAvailable ( int requestID, QByteArray* parsedData, SmfError error );

#ifdef CLIENT_SERVER_TEST
	/**
	 * Services the client request by sending the requested data.
	 * Note:- This will be handled by private implementation.
	 */
/*	void serviceClient ( QByteArray* parsedData );*/
		
	/**
	 * This slot is called for every cleanup timer expiry, in this slot, we need
	 * to call SmfDataStoreManager's API to refresh data store
	 */
	void timerExpired ( ) {}
	
	/**
	 * This method is called every time timerExpired slot is trigerred
	 * Fetches the last saved requests through Transport Manager and Plugin Manager
	 * Who will save the last request (Tranport Manager or Data Store Manager) TBD later
	 */
	void runSavedRequest ( ) {}
#endif
	/**
	 * This slot is called when the data store updates are available as a result of
	 * "runSavedRequest()".
	 * Note:- The "10.4.1.11	updateDatastore" can be merged with this 
	 */
	void dataStoreUpdateAvailable ( QByteArray* respData ){ Q_UNUSED(respData) }
	
	/**
	 * Server calls this method when it receives message from the CM
	 * that authentication keys for the pluginID has expired
	 */
	void authenticationKeysExpired ( NotificationType type, SmfPluginID id );

private:
	SmfTransportInitializeResult prepareTransport();

private:
	//private impl
#ifdef Q_OS_SYMBIAN
	SmfServerSymbian* m_SmfServerPrivate;
#else
	SmfServerQt* m_SmfServerPrivate;
#endif
	//Handles of other SMF components
	SmfTransportManager* m_transportManager;
	SmfPluginManager* m_pluginManager;
	SmfCredMgrClient* m_credentialMngr;
	};

#ifdef CLIENT_SERVER_TEST
class dummyPM : public QObject
	{
		Q_OBJECT
public:
		dummyPM(SmfServer* server,QObject* parent=0);
		~dummyPM();
		SmfError createRequest ( const quint32& aSessionID, 
				const QString& aPluginID, 
				const SmfRequestTypeID& aOperation, 
				QByteArray& aInputData );
public slots:
	void responseAvailable();
private:
		QTimer* m_timer;
		SmfServer* m_server;
	};
#endif

#endif // SMFSERVER_H
