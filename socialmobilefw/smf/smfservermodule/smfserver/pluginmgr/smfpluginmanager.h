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
 * The Plugin Manager class manages the loading and unloading of plug-ins
 *
 */

#ifndef SMFPLUGINMANAGER_H_
#define SMFPLUGINMANAGER_H_

#include <QtSql>
#include <smfglobal.h>
#include "smfserverglobal.h"
#include "smfserver.h"

// Forward declaration
class SmfPluginManagerUtil;
class SmfTransportManagerUtil;
class QFileSystemWatcher;
class QPluginLoader;
class SmfPluginBase;
class SmfProvider;

/**
 * Structure that holds information about a plugin that is awaiting a 
 * response from Transport Manager
 */
struct SmfWaitingPluginInfoStruc
	{
	/**
	 * The server session ID for this request
	 */
	quint32 iSessionID;
	/**
	 * The plugin Id who creates this request
	 */
	QString iPluginId;
	
	/**
	 * The instance of the loaded plugin
	 */
	SmfPluginBase *iInstance;
	
	/**
	 * The type of operation requested by the plugin
	 * @see smfglobal.h
	 */
	SmfRequestTypeID iOperation;
	
	/**
	 * The input data for this request (if request need to be send again)
	 */
	QByteArray iInputData;
	
	};


/**
 * The Plugin Manager class manages the loading and unloading of plug-ins
 */
class SmfPluginManager : public QObject
	{
	Q_OBJECT
	
public:
	/**
	 * Method to get the instance of SmfPluginManager class
	 * @param aServer The Smf server instance
	 * @return The instance of SmfPluginManager class
	 */
	static SmfPluginManager* getInstance ( SmfServer *aServer = NULL );
	
	/**
	 * Debugging only
	 */
	SmfServer* server ( ) { return m_server; }
	
	/**
	 * Destructor
	 */
	~SmfPluginManager ( );
	
public:
	/**
	 * Method called by Smf server to create a web query.
	 * Plugin Manager calls the appropriate web query creation method 
	 * using the aOperation and aInputData parameters. Once the web request 
	 * is ready, it calls the appropriate methods exposed by the Transport 
	 * Manager to send the network request.
	 * @param aSessionID The session ID provided by Smf Server
	 * @param aPluginID The plugin ID that need to perform this operation
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @return SmfError The result of the operation. It can be :-
	 * SmfPluginNoError (if the request is sent successfully) or 
	 * SmfPluginLoadError (if plugin could not be loaded) or
	 * SmfPluginNotAuthorised (if the plugin is not authorised) or
	 * SmfPluginUnknownPluginService (if the requested service is not known or unsupported) or
	 * SmfPluginRequestCreationFailed (if request creation has failed) or
	 * SmfPluginSOPCheckFailed (if plugins request doesnot comply to the Same Origin Policy) or
	 * SmfPluginRequestSendingFailed (if request could not be sent) or 
	 * SmfPluginUnknownHttpService (if the plugin requested any unknown http 
	 * method other than get, post, put, head or delete)  
	 */
	SmfError createRequest ( const quint32& aSessionID, 
			const QString& aPluginID, 
			const SmfRequestTypeID& aOperation, 
			QByteArray& aInputData );
	
	/**
	 * Method called by Smf server to create a synchronous plugin request.
	 * @param aPluginID The plugin ID that need to perform this operation
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required by the plugins
	 * @param aOutputData [out] The output data to be filled by the plugins
	 * @return SmfError The result of the operation. It can be :-
	 * SmfPluginNoError (if the request is success) or 
	 * SmfPluginLoadError (if plugin could not be loaded) or
	 * SmfPluginNotAuthorised (if the plugin is not authorised) or
	 * SmfPluginUnknownPluginService (if the requested service is not known or unsupported)
	 */
	SmfError createSyncRequest ( const QString& aPluginID, 
			const SmfRequestTypeID& aOperation, 
			QByteArray& aInputData,
			QByteArray& aOutputData );
	
	/**
	 * Method called by Transport Manager when network response is available
	 * @param aTransportResult The result of Transport Operation
	 * @param aReply The QNetworkReply instance that requested 
	 * this transaction
	 * @param aResponse The network response data, may be NULL for error
	 */
	void responseAvailable ( const SmfTransportResult &aTransportResult, 
			QNetworkReply *aReply,
			QByteArray *aResponse );
	
	/**
	 * Method to cancel the service request
	 * @param aSessionID The session to be cancelled.
	 * If the plugin is not loaded currently, this method just returns true.
	 * @return Returns true if the plugin operation could be cancelled 
	 * else returns false.
	 */
	bool cancelRequest ( const quint32& aSessionID );
	
	/**
	 * Method to get the list of the SmfProvider for all the plugins that implement 
	 * the mentioned Interface 
	 * @param aInterface The interface for which list of plugins is required 
	 * @param aMap The map of pluginID and its corresponding SmfProvider. The Map 
	 * will be empty if no plugins for the given interface could be found.
	 */
	void getPlugins ( const QString& aInterface, 
			QMap<QString,SmfProvider>& aMap );
	
	/**
	 * Method to get the pluginID for the mentioned interface and service provider 
	 * @param aInterface The interface implemented by the plugin
	 * @param aProv The plugin's service provider
	 * @param aPluginId The required pluginID. This argument will be empty if no plugin
	 * for the given interface and service provider could be found.
	 */
	void getPluginId ( const QString& aInterface, 
			const SmfProvider& aProv, QString& aPluginId );
	
private:
	/**
	 * Method called to initialize the file watcher watching the file  
	 * system for adition/upgradation/removal of plugins
	 */
	void initializeFileWatcher ( );
	
	/**
	 * Method called to initialize the database holding the plugin 
	 * directory sructure information. This is called only once when 
	 * the Plugin Manager is instantiated.
	 * This method creates and updates m_pluginIdPathHash member 
	 * of this class
	 * @return Returns true the database is successfully created and updated, 
	 * else returns false
	 */
	bool initializeSmfPluginDataBase ( );
	
	/**
	 * Method to load a plugin using its Plugin Id.
	 * @param aPluginId The unique ID of the plugin 
	 * @param aLoadResult [out] Output paramater indicating the result 
	 * of the loading. It can be:-
	 * SmfPluginNoError (if plugin was loaded successfully) or 
	 * SmfPluginNotLoaded (if plugin could not be loaded) or 
	 * SmfPluginNotFound (if plugin with the given id could not be found)
	 * @return The instance of the loaded plugin if loaded, else NULL
	 */
	QObject* load ( const QString &aPluginId, 
			SmfError &aLoadResult);
	
	/**
	 * Method to unload a loaded plugin. Returns true if success, else 
	 * returns false.
	 * @param aPlugin The plugin instance to be unloaded
	 * @return Returns true if the mentioned plugin could be unloaded. Returns 
	 * false, if the plugin instance is NULL, or if it could not be unloaded.
	 */
	bool unload ( SmfPluginBase *aPlugin ); 
	
	/**
	 * Method to unload the list of loaded plugins. Returns true if all unload 
	 * are success, else returns false if any one fails.
	 * @param aPluginList The list of instances for all plugins that are 
	 * to be unloaded. This method does nothing and returns false if the list is empty.
	 * @return Returns true if all are success, else returns false if any 
	 * one fails. Also returns false if the input list is empty.
	 */
	bool unload ( const QList<SmfPluginBase*> &aPluginList);
	
	/**
	 * Method that calls the Transport Manager Utility class method to 
	 * send the request created by the plugins over the network
	 * @param aReqData The request data created by the plugin
	 * @param aResult [out] The output parameter indicating the result 
	 * of this method. This can be :-
	 * SmfPluginNoError (if the request is sent successfully) or 
	 * SmfPluginSOPCheckFailed (if plugins request doesnot comply to 
	 * the Same Origin Policy) or 
	 * SmfPluginRequestSendingFailed (if request could not be sent) or 
	 * SmfPluginUnknownHttpService (if the plugin requested any unknown http 
	 * method other than get, post, put, head or delete)  
	 * @param aUrlList The list of accessible Urls for this plugin
	 */
	void sendRequest ( SmfPluginRequestData &aReqData, 
			SmfError &aResult,
			const QList<QUrl> &aUrlList );
	
	/**
	 * Method that checks if a plugin is authorised to make a request. 
	 * This method communicates with Credential and Settings Manager 
	 * through Smf server, giving the registration token and getting 
	 * the valid url list if available for this plugin.
	 * @param aPluginId The ID of the plugin
	 * @param aUrlList [out] The list of Urls that the plugin can send 
	 * request to (to be filled by CSM). This list will be empty if 
	 * aRegToken is empty
	 * @return Returns true if plugin is authorised, else returns false.
	 * Also returns false if aRegToken is empty.
	 */
	bool authorisePlugin( const QString &aPluginId, 
			QList<QUrl> &aUrlList );
	
	/**
	 * Method to serialize the result of parsing (which is done by the 
	 * plugins) to QByteArray to be sent to Smf server.
	 * @param aOperation The type of operation to be performed
	 * @param aResult The data to be serialized (should not be NULL)
	 * @param aDataStream Stream to be written to
	 */
	void serializeResult ( const SmfRequestTypeID &aOperation, 
			QVariant* aResult,
			QDataStream &aDataStream );
	
private slots:
	/**
	 * Method for the directoryChanged signal of QFileSystemWatcher.
	 * This will update the iPluginHash member and also the Plugin 
	 * Information List.
	 * @param aPath The path of the directory that has changed
	 */
	void directoryChanged ( const QString &aPath );

private:
	/**
	 * Constructor with default argument
	 * @param aServer The Smf server instance
	 */
	SmfPluginManager ( SmfServer *aServer );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfPluginManager ( const SmfPluginManager &aOther );
	
private:
	/**
	 * The single instance of SmfPluginManager
	 */
	static SmfPluginManager* m_myInstance;
	
	/**
	 * Member variable that maps the plugin instances to their 
	 * respective QPluginLoader instances (loaders may be multiple)
	 */
	QHash<SmfPluginBase*, QPluginLoader*> m_pluginLoaderHash;
	
	/**
	 * Member variable that watches the file system for 
	 * addition/upgradation/removal of plugins 
	 */
	QFileSystemWatcher *m_fileWatcher;
	
	/**
	 * Member variable that is an instance of a structure that 
	 * holds a loaded plugin information 
	 */
	SmfWaitingPluginInfoStruc *m_tempStruct;
	
	/**
	 * Member variable that maps a QNetworkReply instance to 
	 * its corresponding loaded plugin information structure  
	 */
	QHash<QNetworkReply*, SmfWaitingPluginInfoStruc*> m_waitingPluginHash;
	
	/**
	 * Member variable that maps a pluginId to its path
	 */
	QHash<QString, QString> m_pluginIdPathHash;
	
	/**
	 * Member variable that holds the instance of 
	 * SmfServer class
	 */
	SmfServer *m_server;
	
	/**
	 * Member variable that holds the instance of 
	 * SmfTransportManagerUtil class
	 */
	SmfTransportManagerUtil *m_transMngrUtil;

	/**
	 * Member variable that holds the instance of 
	 * SmfPluginManagerUtil class. SmfPluginManager is a friend of SmfPluginManagerUtil.
	 */
	SmfPluginManagerUtil *m_util;

	/**
	 * Member variable that holds QSqlDatabase instance.
	 */
	QSqlDatabase m_pluginDataBase;
	
	};

#endif /* SMFPLUGINMANAGER_H_ */
