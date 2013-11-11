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

#include <QDir>
#include <QDebug>
#include <QLibraryInfo>
#include <QFileSystemWatcher>
#include <QPluginLoader>
#include <QList>
#include <smfpluginbase.h>
#include <smfpluginutil.h>
#include <smfprovider.h>
#include <smfcredmgrclient.h>

#include "smfpluginmanager.h"
#include "smfpluginmanagerutil.h"
#include "smftransportmanagerutil.h"

// Static data initialisation
SmfPluginManager* SmfPluginManager::m_myInstance = NULL;


/**
 * Method to get the instance of SmfPluginManager class
 * @param aServer The Smf server instance
 * @return The instance of SmfPluginManager class
 */
SmfPluginManager* SmfPluginManager::getInstance ( SmfServer *aServer)
	{
	if(NULL == m_myInstance)
		m_myInstance = new SmfPluginManager(aServer);
	return m_myInstance;
	}


/**
 * Constructor with default argument
 * @param aServer The Smf server instance
 */
SmfPluginManager::SmfPluginManager ( SmfServer *aServer )
	{
	// Save the server instance
	m_server = aServer;
	
	// initialize the file watcher to monitor plugin addition/upgradation/removal
	initializeFileWatcher ( );
	
	// create a database to store the folder structure of the path "c://resource//qt//plugins"
	initializeSmfPluginDataBase ( );
	
	// create teh Plugin Manager utility class instance
	m_util = new SmfPluginManagerUtil(this);
	
	// Get handle to the Transport MAnager utility instance
	m_transMngrUtil = SmfTransportManagerUtil::getInstance();
	}


/**
 * Destructor
 */
SmfPluginManager::~SmfPluginManager ( )
	{
	qDebug()<<"Inside SmfPluginManager::~SmfPluginManager()";
	
	// delete file watcher
	if(m_fileWatcher)
		delete m_fileWatcher;
	
	if(m_tempStruct)
		delete m_tempStruct;
	
	// delete the plugin information hash
	if(m_waitingPluginHash.count() > 0)
		{
		foreach(SmfWaitingPluginInfoStruc *str, m_waitingPluginHash.values())
			delete str;
		}
	
	// delete the SmfPluginManagerUtil class instance
	if(m_util)
		delete m_util;
	
	// Close the database
	if(m_pluginDataBase.isOpen())
		{
		m_pluginDataBase.close();
		m_pluginDataBase.removeDatabase("SmfPluginsInfoDatabase");
		QFile::remove("SmfPluginsInfoDatabase");
		}
	
	// unload all loaded plugins
	unload(m_pluginLoaderHash.keys());
	
	// Finally remove the existing singleton instance of plugin manager itself
	if(m_myInstance)
		delete m_myInstance;
	}


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
SmfError SmfPluginManager::createRequest ( const quint32& aSessionID, 
		const QString& aPluginID, 
		const SmfRequestTypeID& aOperation, 
		QByteArray& aInputData )
	{
	qDebug()<<"Inside SmfPluginManager::createRequest()";
	
	SmfError result = SmfUnknownError;

	// Load the plugin
	QObject *plugin = load(aPluginID, result);
	
	// Check if plugin is loaded
	if(plugin && (SmfNoError == result))
		{
		SmfPluginBase* instance = qobject_cast<SmfPluginBase *>(plugin);
		
		if(instance)
		{
		QList<QUrl> urlList;
		SmfPluginRequestData reqData;

		// check if the plugin is authorised (with CSM)
		if( authorisePlugin(aPluginID, urlList ))
			{
			qDebug()<<"Plugin authorised";
			
			// call the utility method to create plugin specific request
			result = SmfUnknownError;
			QByteArray notused;
			m_util->createRequest(plugin, aOperation, aInputData, reqData, result, notused);

			// If the request is created successfully, call the TM method to send the request
			if( SmfNoError == result )
				{
				qDebug()<<"Plugin request creation successful";
				m_tempStruct = new SmfWaitingPluginInfoStruc();
				m_tempStruct->iSessionID = aSessionID;
				m_tempStruct->iPluginId = aPluginID;
				m_tempStruct->iInstance = instance;
				m_tempStruct->iOperation = aOperation;
				m_tempStruct->iInputData = aInputData;
			
				// send the request
				sendRequest ( reqData, result, urlList );
				}
				else
					qDebug()<<"Plugin request creation failed!!!, error = "<<result;
			}
		
		else
			{
			// plugin not authorised, so unload
			qDebug()<<"Plugin not authorised!!!";
			unload(instance);
				result = SmfPMPluginNotAuthorised;
			}
		}
		else
			{
			// plugin instance cannot be casted, so unload
			qDebug()<<"Plugin instance cannot be casted to SmfPluginBase*!!!";
			unload(instance);
			result = SmfPMPluginLoadError;
			}
		}

	else
		{
		// plugin not loaded
		qDebug()<<"Plugin not loaded!!!";
		}
	
	return result;
	}


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
SmfError SmfPluginManager::createSyncRequest ( const QString& aPluginID, 
		const SmfRequestTypeID& aOperation, 
		QByteArray& aInputData,
		QByteArray& aOutputData )
	{
	qDebug()<<"Inside SmfPluginManager::createSyncRequest()";
	
	SmfError result = SmfUnknownError;

	// Load the plugin
	QObject *plugin = load(aPluginID, result);
	
	// Check if plugin is loaded
	if(plugin && (SmfNoError == result))
		{
		SmfPluginBase* instance = qobject_cast<SmfPluginBase *>(plugin);
		
		if(instance)
		{
		QList<QUrl> urlList;
		SmfPluginRequestData reqData;

		// check if the plugin is authorised (with CSM)
		if( authorisePlugin(aPluginID, urlList ))
			{
			qDebug()<<"Plugin authorised";
			
			// call the utility method to create plugin specific request
			result = SmfUnknownError;
			m_util->createRequest(plugin, aOperation, aInputData, reqData, result, aOutputData );

			// If the request is created successfully, call the TM method to send the request
			if( SmfNoError == result )
				qDebug()<<"Plugin request creation successful";
			else
				qDebug()<<"Plugin request creation failed!!!, error = "<<result;
			}
		
		else
			{
			// plugin not authorised, so unload
			qDebug()<<"Plugin not authorised!!!";
			unload(instance);
				result = SmfPMPluginNotAuthorised;
			}
		}
		else
			{
			// plugin instance cannot be casted, so unload
			qDebug()<<"Plugin instance cannot be casted to SmfPluginBase*!!!";
			unload(instance);
			result = SmfPMPluginLoadError;
			}
		}

	else
		{
		// plugin not loaded
		qDebug()<<"Plugin not loaded!!!";
		}
	
	return result;
	}



/**
 * Method called by Transport Manager when network response is available
 * @param aTransportResult The result of Transport Operation
 * @param aReply The QNetworkReply instance that requested 
 * this transaction
 * @param aResponse The network response data, may be NULL for error
 */
void SmfPluginManager::responseAvailable ( 
		const SmfTransportResult &aTransportResult, 
		QNetworkReply *aReply,
		QByteArray *aResponse )
	{
	qDebug()<<"Inside SmfPluginManager::responseAvailable()";
		
	// get the details of the plugin which made this request
	SmfWaitingPluginInfoStruc* info = NULL;
	info = m_waitingPluginHash.value(aReply);
	
	if(info)
			{
			quint32 sessionId = m_waitingPluginHash.value(aReply)->iSessionID;
			QString pluginId = m_waitingPluginHash.value(aReply)->iPluginId;
			SmfRequestTypeID operation = m_waitingPluginHash.value(aReply)->iOperation;
			QByteArray inputData = m_waitingPluginHash.value(aReply)->iInputData;
			
			QVariant result;
			SmfPluginRetType retType = SmfRequestError;
			SmfResultPage pageResult;
			
			// call the utility method to send response to appropriate plugins
			SmfError retValue = m_util->responseAvailable( info->iInstance, operation, 
					aTransportResult, aResponse, &result, retType, pageResult );
			
			qDebug()<<"m_util->responseAvailable() return = "<<retValue;
		
			// remove the plugin from the waiting list
			delete m_waitingPluginHash.value(aReply);
			m_waitingPluginHash.remove(aReply);
		
			QByteArray arr;
			QDataStream stream(&arr, QIODevice::ReadWrite);
		
			if( SmfNoError == retValue )
				{
				qDebug()<<"Parsing of response is successful";
				
				// serialize the response to suitable class and pass the data to server
				serializeResult(operation, &result, stream);
			
				// Send the response data to the server
				m_server->resultsAvailable(sessionId, &arr, retValue);
				}
		
			// Send the request again
			else if( SmfPMPluginSendRequestAgain == retValue )
				{
				qDebug()<<"Send request again";
				retValue = createRequest( sessionId, pluginId, operation, inputData );
				}
			
			// Error
			else
				{
				qDebug()<<"Plugin responseAvailable() failed!!!, error = "<<retValue;
				
				// Error in parsing, sent to server
// ToDo :- For testing:-
				stream<<result.toString();
				m_server->resultsAvailable(sessionId, &arr, retValue);
			}
		}
	else
		qDebug()<<"No outstanding requests for this QNetworkReply!!!";
		
	// delete aReply later, when event loop is re-entered
	aReply->deleteLater();
	}
	

/**
 * Method to cancel the service request
 * @param aSessionID The session to be cancelled.
 * If the plugin is not loaded currently, this method just returns true.
 * @return Returns true if the plugin operation could be cancelled 
 * else returns false.
 */
bool SmfPluginManager::cancelRequest (  const quint32& aSessionID )
	{
	bool retValue = true;
	qDebug()<<"Inside SmfPluginManager::cancelRequest()";
	
	// Get the plugin for which cancel is requested
	foreach(SmfWaitingPluginInfoStruc* iPluginInfo, m_waitingPluginHash.values())
		{
		if( aSessionID == iPluginInfo->iSessionID )
			{
			qDebug()<<"Plugin to be cancelled found in the waiting list";
			
			// Notify Transport Manager
			m_transMngrUtil->cancelRequest(m_waitingPluginHash.key(iPluginInfo));
			
			}
		else
			retValue =  true;
		}
	return retValue;
	}


/**
 * Method called to initialize the file watcher watching the file  
 * system for adition/upgradation/removal of plugins
 */
void SmfPluginManager::initializeFileWatcher ( )
	{
	qDebug()<<"Inside SmfPluginManager::initializeFileWatcher()";
	
	// Create the file watcher for the plugins in /Smf folder of the Qt plugin directory
	m_fileWatcher = new QFileSystemWatcher(this);
	
	// Get the directory having the Qt plugin stubs
	QDir dir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
	
	// If Smf folder exists
	if(dir.cd("smf/plugin"))
		{
		// Add each service provider folders to the file watcher
		foreach(QString folder, dir.entryList(QDir::AllDirs))
			{
			dir.cd(folder);
			m_fileWatcher->addPath(dir.absolutePath());
			dir.cdUp();
			}
		}
	else
		m_fileWatcher->addPath(dir.absolutePath());
	
	connect(m_fileWatcher, SIGNAL(directoryChanged(const QString&)), 
			this, SLOT (directoryChanged(const QString&)));
	
	connect(m_fileWatcher, SIGNAL(fileChanged(const QString&)), 
				this, SLOT (directoryChanged(const QString&)));
	}



/**
 * Method called to initialize the database holding the plugin 
 * directory sructure information. This is called only once when 
 * the Plugin Manager is instantiated.
 * This method creates and updates m_pluginIdPathHash member 
 * of this class
 * @return Returns true the database is successfully created and updated, 
 * else returns false
 */
bool SmfPluginManager::initializeSmfPluginDataBase ( )
	{
	qDebug()<<"Inside SmfPluginManager::initializeSmfPluginDataBase()";
	
	// Find QSQLite driver and create a connection to database
	QFile::remove("SmfPluginsInfoDatabase");
	m_pluginDataBase = QSqlDatabase::addDatabase("QSQLITE");
	m_pluginDataBase.setDatabaseName("SmfPluginsInfoDatabase");
	
	// Open the database
	bool opened = m_pluginDataBase.open();
	if(!opened)
		{
		qDebug()<<"Database could not be opened, returning !!!";	
		return false;
		}
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Database opened";
#endif
	
	// Create a query to create the DB table for Plugin Manager (if it doesn't exists)
	QSqlQuery query;
	bool tableCreated = false;

	tableCreated = query.exec("CREATE TABLE IF NOT EXISTS pluginDetails ("
			"pluginId TEXT PRIMARY KEY, interfaceName TEXT, serviceProvider TEXT, "
			"description TEXT, serviceUrl TEXT, authAppId TEXT)");

	// Error - table not created, Plugin Manager might not work properly
	if(!tableCreated)
		{
		qDebug()<<"Table not created!!!, error = "<<query.lastError().text();
		
		// Close the database
		m_pluginDataBase.close();
		return false;
		}
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Table created";
#endif
	
	// Get the directory having the Qt plugin stubs
	QDir dir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
	
	// If Smf folder exists
	if(dir.cd("smf/plugin"))
		{
#ifdef DETAILEDDEBUGGING
		qDebug()<<"Smf/plugin folder exists";
#endif
		// Get each interface folders names
		foreach(QString intfName, dir.entryList(QDir::AllDirs))
			{
			dir.cd(intfName);
#ifdef DETAILEDDEBUGGING
			qDebug()<<"Interface name : "<<dir.dirName();
#endif
			
			// Get each plugin in this folder
			foreach(QString pluginName, dir.entryList(QDir::Files))
				{
#ifdef DETAILEDDEBUGGING
				qDebug()<<"plugins for this Interface : "<<pluginName;
#endif
				
				// load this plugin
				QPluginLoader pluginLoader(dir.absoluteFilePath(pluginName));
				QObject *instance = pluginLoader.instance();
				if (instance)
					{
#ifdef DETAILEDDEBUGGING
					qDebug()<<"instance found";
#endif
					SmfPluginBase* plugin = qobject_cast<SmfPluginBase *>(instance);
				    if (plugin)
				    	{
#ifdef DETAILEDDEBUGGING
						qDebug()<<"SmfPluginBase found";
#endif
						plugin->initialize();
				    
						// get the plugin id
						QString id = plugin->getProviderInfo()->pluginId();
						
						// get the interface implemented by the plugin
						QString intfImplemented = dir.dirName();
						intfImplemented.prepend("org.symbian.smf.plugin.");
#ifdef DETAILEDDEBUGGING
						qDebug()<<"intfImplemented = "<<intfImplemented;
#endif
						
						// get the service provider
						QString serProv = plugin->getProviderInfo()->serviceName();
						
						//get the description
						QString desc = plugin->getProviderInfo()->description();
						
						// get the service URL
						QString servURL = plugin->getProviderInfo()->serviceUrl().toString();
						
						// get the authentication application id
						QString str;
						QStringList list;
						QString authAppId = plugin->getProviderInfo()->authenticationApp(
								str, list, QIODevice::ReadWrite);
						
						// Update m_pluginIdPathHash
						m_pluginIdPathHash.insert(id, dir.absoluteFilePath(pluginName));
						
						QSqlQuery rowInsertQuery;
						
						// insert into database
						bool rowInserted = rowInsertQuery.exec(QString("INSERT INTO pluginDetails VALUES ('%1', "
								"'%2', '%3', '%4', '%5', '%6')").arg(id).arg(intfImplemented).arg(serProv)
								.arg(desc).arg(servURL).arg(authAppId));
						
						if(rowInserted)
							qDebug()<<QString("This Plugin's information is added to database : '%1' '%2' '%3'")
											.arg(id).arg(intfImplemented).arg(serProv);
						else
							if(0 != query.lastError().text().size())
								qDebug()<<"plugins data not written to database!!!, error = "<<query.lastError().text();
				    	}
				    else
				    	{
						qDebug()<<"Plugin could not be converted to SmfpluginBase* - returning!!!";
						
						// Close the database
						m_pluginDataBase.close();
						
				    	return false;
				    	}
				    
				    pluginLoader.unload();
					}
				else
					{
					qDebug()<<"Plugin could not be loaded - returning!!!, error = "<<pluginLoader.errorString();
					
					// Close the database
					m_pluginDataBase.close();
										
					return false;
					}
				}
			dir.cdUp();
			}
		}
	else
		qDebug()<<"No Smf plugins installed!!!";
	
	// Close the database
	m_pluginDataBase.close();
	
	return true;
	}



/**
 * Method to load a plugin using its Plugin Id.
 * @param aPluginId The unique ID of the plugin 
 * @param aLoadResult [out] Output paramater indicating the result 
 * of the loading. It can be:-
 * SmfPluginNoError (if plugin was loaded successfully) or
 * SmfPluginNotFound (if plugin with the given id could not be found) 
 * SmfPluginNotLoaded (if plugin could not be loaded) or 
 * @return The instance of the loaded plugin if loaded, else NULL
 */
QObject* SmfPluginManager::load ( const QString &aPluginId,
		SmfError &aLoadResult)
	{
	qDebug()<<"Inside SmfPluginManager::load()";
	
	// Find the plugin Path
	QString pluginPath = m_pluginIdPathHash.value(aPluginId);
	
	if(!pluginPath.isEmpty())
		{
		// create the plugin loader and load the plugin
		QPluginLoader *pluginLoader = NULL;
		pluginLoader = new QPluginLoader(pluginPath);
		if(!pluginLoader)
			{
			// Plugin loader could not be created, error
			qDebug()<<"QPluginLoader allocation failed!!!";
			aLoadResult = SmfPMPluginNotLoaded;
			return NULL;
			}
		
		SmfPluginBase *plugin = qobject_cast<SmfPluginBase *>(pluginLoader->instance());
		
		// If the plugin is loaded
		if( pluginLoader->isLoaded() && plugin )
			{
			// Initialize the plugin
			plugin->initialize();
			
			// update the plugin loader and the loaded plugin lists
			m_pluginLoaderHash.insertMulti(plugin, pluginLoader);
			aLoadResult = SmfNoError;
			qDebug()<<"Plugin loaded";
			return pluginLoader->instance();
			}
		else
			{
			// Plugin could not be loaded, error
			qDebug()<<"Plugin not loaded!!!, error = "<<pluginLoader->errorString();
			aLoadResult = SmfPMPluginNotLoaded;
			return NULL;
		}
		}
	else
		{
		// plugin could not be found in the hash maintained by PM
		qDebug()<<"Plugin not found!!!";
		aLoadResult = SmfPMPluginNotFound;
		return NULL;
		}
	}


/**
 * Method to unload a loaded plugin. Returns true if success, else 
 * returns false.
 * @param aPlugin The plugin instance to be unloaded
 * @return Returns true if the mentioned plugin could be unloaded. Returns 
 * false, if the plugin instance is NULL, or if it could not be unloaded.
 */
bool SmfPluginManager::unload ( SmfPluginBase *aPlugin )
	{
	qDebug()<<"Inside SmfPluginManager::unload()";
	bool unloadResult = true;
	
	if(aPlugin)
		{
		// Get all the loaders for this plugin
		QList<QPluginLoader*> loaderList = m_pluginLoaderHash.values(aPlugin);
		if(loaderList.size())
			{
			foreach(QPluginLoader *loader, loaderList)
				{
				// for each loader unload the plugin
				unloadResult = loader->unload();
				
				// delete the instance of the loader
				delete loader;
				loader = NULL;
				}
			
			// Remove the plugin and its associated loaders from the Hash
			m_pluginLoaderHash.remove(aPlugin);
			return unloadResult;
			}
		}
	else
		{
		qDebug()<<"Plugin  instance is NULL!!!";
		unloadResult = false;
		}
	
	return unloadResult;
	}


/**
 * Method to unload the list of loaded plugins. Returns true if all unload 
 * are success, else returns false if any one fails.
 * @param aPluginList The list of instances for all plugins that are 
 * to be unloaded. This method does nothing and returns false if the list is empty.
 * @return Returns true if all are success, else returns false if any 
 * one fails. Also returns false if the input list is empty.
 */
bool SmfPluginManager::unload ( const QList<SmfPluginBase *> &aPluginList)
	{
	qDebug()<<"Inside SmfPluginManager::unload() - overloaded fn";
	bool unloaded = false;
	
	if(aPluginList.size())
		{
		//unload all the required plugins
		foreach(SmfPluginBase *plugin, aPluginList)
			{
			// unload individual plugins in the list
			bool ret = unload(plugin);
			
			// indicate error if any one of the plugin failed to unload
			if(!ret)
				unloaded = ret;
			}
		}
	return unloaded;
	}


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
void SmfPluginManager::sendRequest ( SmfPluginRequestData &aReqData, 
		SmfError &aResult,
		const QList<QUrl> &aUrlList )
	{
	qDebug()<<"Inside SmfPluginManager::sendRequest()";
	
	QNetworkReply* reply;
	bool sopCompliant = false;
	
	// Check the type of Http operation to be performed
	switch(aReqData.iHttpOperationType)
		{
		// Http HEAD
		case QNetworkAccessManager::HeadOperation:
#ifdef DETAILEDDEBUGGING
			qDebug()<<"http::head Operation requested";
#endif
			reply = m_transMngrUtil->head(aReqData.iNetworkRequest, aUrlList, sopCompliant);
			break;
		
		// Http GET
		case QNetworkAccessManager::GetOperation:
#ifdef DETAILEDDEBUGGING
			qDebug()<<"http::get Operation requested";
#endif
			reply = m_transMngrUtil->get(aReqData.iNetworkRequest, aUrlList, sopCompliant);
			break;
			
		// Http PUT	
		case QNetworkAccessManager::PutOperation:
#ifdef DETAILEDDEBUGGING
			qDebug()<<"http::put Operation requested";
#endif
			reply = m_transMngrUtil->put(aReqData.iNetworkRequest, aReqData.iPostData->buffer(), aUrlList, sopCompliant);
			delete aReqData.iPostData;
			break;
			
		// Http POST
		case QNetworkAccessManager::PostOperation:
#ifdef DETAILEDDEBUGGING
			qDebug()<<"http::post Operation requested";
#endif
			reply = m_transMngrUtil->post(aReqData.iNetworkRequest, aReqData.iPostData->buffer(), aUrlList, sopCompliant);
			delete aReqData.iPostData;
			break;
			
		// Http DELETE
		case QNetworkAccessManager::DeleteOperation:
#ifdef DETAILEDDEBUGGING
			qDebug()<<"http::delete Operation requested";
#endif
			reply = m_transMngrUtil->deleteResource(aReqData.iNetworkRequest, aUrlList, sopCompliant);
			break;
			
		default:
#ifdef DETAILEDDEBUGGING
			qDebug()<<"unknown http Operation requested!!!";
#endif
			aResult = SmfPMPluginUnknownHttpService;
			return;
		}
	
	if( sopCompliant )
		{
		if( reply )
			{
			// SOP compliant, sending successful
			qDebug()<<"No error, request sent";
			m_waitingPluginHash.insert(reply, m_tempStruct);
			m_tempStruct = NULL;
			aResult = SmfNoError;
			}
		else
			{
			// reply is NULL, sending failed
			qDebug()<<"QNetworkReply returned NULL - request not sent";
			aResult = SmfPMPluginRequestSendingFailed;
			}
		}
	
	else
		{
		// SOP violation
		qDebug()<<"SOP checking failed";
		aResult = SmfPMPluginSOPCheckFailed;
		}
	}


/**
 * Method that checks if a plugin is authorised to make a request. 
 * This method communicates with Credential and Settings Manager 
 * through Smf server, giving the registration token and getting 
 * the valid url list if available for this plugin.
 * @param aPluginId The ID of the plugin
 * @param aUrlList [out] The list of Urls that the plugin can send 
 * request to (to be filled by CSM). This list will be empty if 
 * aPluginId is not valid or not authorised.
 * @return Returns true if plugin is authorised, else returns false.
 * Also returns false if aRegToken is empty.
 */
bool SmfPluginManager::authorisePlugin( const QString &aPluginId, 
		QList<QUrl> &aUrlList )
	{
	qDebug()<<"Inside SmfPluginManager::authorisePlugin()";
	
	bool authorised = false;
	SmfCredMgrClient csmClient;
	
	aUrlList = csmClient.URLList(aPluginId);
	if(aUrlList.count())
		authorised = true;
	
	return authorised;
	}


/**
 * Method to serialize the result of parsing (which is done by the 
 * plugins) to QByteArray to be sent to Smf server.
 * @param aOperation The type of operation to be performed
 * @param aResult The data to be serialized (should not be NULL)
 * @param aDataStream Stream to be written to
 */
void SmfPluginManager::serializeResult ( 
		const SmfRequestTypeID &aOperation, 
		QVariant* aResult,
		QDataStream &aDataStream )
	{
	qDebug()<<"Inside SmfPluginManager::serializeResult()";
	
	// Call the utlity class method to serialize the result
	m_util->serializeResult(aOperation, aResult, aDataStream);
	}


/**
 * Method for the directoryChanged signal of QFileSystemWatcher.
 * This will update the iPluginHash member and also the Plugin 
 * Information List.
 * @param aPath The path of the directory that has changed
 */
void SmfPluginManager::directoryChanged ( const QString &aPath )
	{
	qDebug()<<"Inside SmfPluginManager::directoryChanged()";
	qDebug()<<"Changed path = "<<aPath;
	
	// Create a QDir instance with the given path
	QDir dir(aPath);
	QString pluginId;
	QString oldpluginId;
	QString interfaceName;
	QString serviceProv;
	QString authAppId;

	// Get all the files in the directory at a specified path(sorted)
	QStringList newPlugins = dir.entryList(QDir::Files, QDir::Name);
	QStringList::const_iterator newListIterator = newPlugins.constBegin();
	
	foreach(QString name, newPlugins)
		qDebug()<<"New plugin = "<<name;
	
	// Get all plugins who were in this path, before this directory was changed
	QStringList availablePlugins = m_pluginIdPathHash.keys(aPath);
	availablePlugins.sort();
	QStringList::const_iterator oldListIterator = availablePlugins.constBegin();
	
	foreach(QString name, availablePlugins)
		qDebug()<<"Old plugin = "<<name;
	
	// Open the database
	bool opened = m_pluginDataBase.open();
	if(!opened)
		return;
	
	// If plugin is changed
	if( newPlugins.count() == availablePlugins.count() )
		{
		qDebug()<<"Plugin upgraded...";
		// Check for equality
		while( newListIterator != newPlugins.constEnd() )
			{
			if( *newListIterator == *oldListIterator )
				{
				newListIterator++;
				oldListIterator++;
				}
			else
				break;
			}
		// replace *oldListIterator with *newListIterator
		QHash<QString, QString>::iterator i = m_pluginIdPathHash.find(*oldListIterator);
		m_pluginIdPathHash.insert(*newListIterator, i.value());
		m_pluginIdPathHash.remove(*oldListIterator);
		
		// Also update database with *newListIterator
	    QSqlQuery updateQuery;
	    
	    bool updated = updateQuery.exec(QString("UPDATE pluginDetails SET pluginId = '%1' "
	    		"WHERE pluginId = '%2'").arg(*newListIterator).arg(*oldListIterator));
	    if (!updated)
	    	if(0 != updateQuery.lastError().text().size())
	    		qDebug()<<"Database table not updated, error = "<<updateQuery.lastError().text();
		
		// Get the new and old plugin Ids
		pluginId = *newListIterator;
		oldpluginId = *oldListIterator;
		
		// Load the plugin and get its service provider name
		SmfError result;
		SmfPluginBase* instance = qobject_cast<SmfPluginBase *>(load(pluginId, result));
		
		if(instance && (SmfNoError == result))
			{
			instance->initialize();
			serviceProv = instance->getProviderInfo()->serviceName();
			interfaceName = dir.dirName();
			}
		
		unload(instance);
		
		qDebug()<<QString("Upgraded plugin details are : '%1' '%2' '%3' '%4'").arg(oldpluginId)
				.arg(pluginId).arg(interfaceName).arg(serviceProv);
		
		// Inform server that plugin has been changed
#ifdef CSM_INTEGRATED
		//Remove after Server Integration
		m_server->pluginChanged(oldPluginId, pluginId, interfaceName, serviceProv);
#endif
		}
		
	// If plugin is added
	else if(newPlugins.count() > availablePlugins.count())
		{
		qDebug()<<"Plugin Added...";
		// Check for equality
		while( oldListIterator != availablePlugins.constEnd() )
			{
			if( *newListIterator == *oldListIterator )
				{
				newListIterator++;
				oldListIterator++;
				}
			else
				break;
			}
		// replace *oldListIterator with *newListIterator
		m_pluginIdPathHash.insert(*newListIterator, aPath);
		
		// Get the plugin Id
		pluginId = *newListIterator;
		
		// Load the plugin and get its service provider name
		SmfError result;
		SmfPluginBase* instance = qobject_cast<SmfPluginBase *>(load(pluginId, result));
		
		if(instance && (SmfNoError == result))
			{
			instance->initialize();
			serviceProv = instance->getProviderInfo()->serviceName();
			interfaceName = dir.dirName();
			interfaceName.prepend("org.symbian.smf.plugin.");
			QString prgm;
			QStringList list;
			authAppId = instance->getProviderInfo()->authenticationApp(prgm, list, QIODevice::ReadWrite);
			}
		
		unload(instance);
		
		// Also add to the database the value newListIterator and aPath
		QSqlQuery insertRowQuery;
		bool rowInserted = insertRowQuery.exec(QString("INSERT INTO pluginDetails VALUES "
				"('%1', '%2', '%3', '%4')").arg(pluginId).arg(interfaceName).arg(serviceProv).arg(authAppId));

		 // Error
		if (!rowInserted)
			qDebug()<<"Database table not inserted, error = "<<insertRowQuery.lastError().text();
		
		qDebug()<<QString("Added plugin detailes are : '%1' '%2' '%3'").arg(pluginId)
				.arg(interfaceName).arg(serviceProv);
		
		// Inform server that plugin has been added
#ifdef CSM_INTEGRATED
		//Remove after Server Integration
		m_server->pluginAdded(pluginId, interfaceName, serviceProv);
#endif
		}
	
	// If plugin is removed
	else //for newPlugins.count() < availablePlugins.count()
		{
		qDebug()<<"Plugin removed...";
		
		// Check for equality
		while( newListIterator != newPlugins.constEnd() )
			{
			if( *newListIterator == *oldListIterator )
				{
				newListIterator++;
				oldListIterator++;
				}
			else
				break;
			}
		// remove *oldListIterator
		m_pluginIdPathHash.remove(*oldListIterator);
		
		// Also remove oldListIterator from the database
		QSqlQuery deleteRowQuery;
		bool rowDeleted = deleteRowQuery.exec(QString("DELETE FROM pluginDetails WHERE pluginId = '%1'")
				.arg(*oldListIterator));
		 
		// Error
		if (!rowDeleted)
			qDebug()<<"Database table row not deleted, error = "<<deleteRowQuery.lastError().text();
		
		// Get the plugin Id
		pluginId = *oldListIterator;
		
		// Load the plugin and get its service provider name
		SmfError result;
		SmfPluginBase* instance = qobject_cast<SmfPluginBase *>(load(pluginId, result));
		
		if(instance && (SmfNoError == result))
			{
			instance->initialize();
			serviceProv = instance->getProviderInfo()->serviceName();
			interfaceName = dir.dirName();
			}
		
		unload(instance);
		
		qDebug()<<QString("Added plugin detailes are : '%1' '%2' '%3'").arg(pluginId)
				.arg(interfaceName).arg(serviceProv);
		
		// Inform server that plugin has removed
#ifdef CSM_INTEGRATED
		//Remove after Server Integration
		m_server->pluginRemoved(pluginId, interfaceName, serviceProv);
#endif
		}
	// Close the database
	m_pluginDataBase.close();
	}


/**
 * Method to get the list of the SmfProvider for all the plugins that implement 
 * the mentioned Interface 
 * @param aInterface The interface for which list of plugins is required 
 * @param aMap The map of pluginID and its corresponding SmfProvider. The Map 
 * will be empty if no plugins for the given interface could be found.
 */
void SmfPluginManager::getPlugins(const QString& aInterface, QMap<QString,SmfProvider>& aMap)
	{
	qDebug()<<"Inside SmfPluginManager::getPlugins()";
	qDebug()<<"Argument, intf name = "<<aInterface;
	
	aMap.clear();
	
	if(aInterface.isEmpty())
		{
		qDebug()<<"Interface name is empty!!!";
		return;
		}
	
	// Open the database
	bool opened = m_pluginDataBase.open();
	if(!opened)
		{
		qDebug()<<"Data base not opened, exiting getplugins()!!!";
		return;
		}
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Data base opened";
#endif
	
	// Query the database for all pluginIDs that implement the given interface
	QSqlQuery query(QString("SELECT pluginId, interfaceName, serviceProvider, description, "
			"serviceUrl FROM pluginDetails where interfaceName = '%1'").arg(aInterface));
	
	while(query.next())
		{
#ifdef DETAILEDDEBUGGING
		qDebug()<<"Query is success";
#endif

		SmfProvider prov;
		
		// get the pluginId
		QString pluginId = query.value(0).toString();
		qDebug()<<"Found Plugin Id = "<<pluginId;
		
		// get the service type / interface name
		QStringList servicetypes;
		servicetypes.insert(0, query.value(1).toString());
		prov.setSupportedInterfaces(servicetypes);
#ifdef DETAILEDDEBUGGING
		qDebug()<<"  Its interface = "<<servicetypes.at(0);
#endif
		
		// Get the serv provider
		QString servName = query.value(2).toString();
		prov.setServiceName(servName);
#ifdef DETAILEDDEBUGGING
		qDebug()<<"  Its serv prov = "<<servName;
#endif
		
		// Get the description
		QString desc = query.value(3).toString();
		prov.setDescription(desc);
#ifdef DETAILEDDEBUGGING
		qDebug()<<"  Its description = "<<desc;
#endif
		
		// Get the service URL
		QUrl url(query.value(4).toString());
		prov.setServiceUrl(url);
#ifdef DETAILEDDEBUGGING
		qDebug()<<"  Its url = "<<url.toString();
#endif

		aMap.insert(pluginId, prov);
		}
	
	if(0 != query.lastError().text().size())
		qDebug()<<"Data base query->next() exited, error = "<<query.lastError().text();
	else
		qDebug()<<"Data base query->next() exited";
	
	qDebug()<<"Count of SmfProvider returned = "<<aMap.count();
	
	// Close the database
	m_pluginDataBase.close();
	}


/**
 * Method to get the pluginID for the mentioned interface and service provider 
 * @param aInterface The interface implemented by the plugin
 * @param aProv The plugin's service provider
 * @param aPluginId The required pluginID. This argument will be empty if no plugin
 * for the given interface and service provider could be found.
 */
void SmfPluginManager::getPluginId(const QString& aInterface, const SmfProvider& aProv, QString& aPluginId)
	{
	qDebug()<<"SmfPluginManager::getPluginId()";
	aPluginId.clear();
	
	if(aInterface.isEmpty())
		{
		qDebug()<<"Interface name is empty!!!";
		return;
		}
	
	// Open the database
	bool opened = m_pluginDataBase.open();
	if(!opened)
		{
		qDebug()<<"Data base not opened, exiting!!!";
		return;
		}
	
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Data base opened";
#endif

	// Query the database for a pluginID with given interface name and service provider
	QSqlQuery query(QString("SELECT pluginId FROM pluginDetails where interfaceName = '%1' AND "
			"serviceProvider = '%2'").arg(aInterface).arg(aProv.serviceName()));
	
	if (query.next())
		{
#ifdef DETAILEDDEBUGGING
		qDebug()<<"Query is success";
#endif
		
		// get the pluginId
		aPluginId = query.value(0).toString();
		qDebug()<<"returned pluginID = "<<aPluginId;
		}
	else
		{		
		if(0 != query.lastError().text().size())
			qDebug()<<"Data base query->next() returned false, error = "<<query.lastError().text();
		else
			qDebug()<<"Data base query->next() returned false";
		}
	
	// Close the database
	m_pluginDataBase.close();
	}
