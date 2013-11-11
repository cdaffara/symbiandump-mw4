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
 * them properly to the appropriate component
 * 
 */

#include <QDebug>
#include <qglobal.h>
#include <smfcontact.h>
#include <smfgroup.h>
#include <smfpost.h>
#include <smflocation.h>
#include <smfpicture.h>
#include <smfcomment.h>
#include <smfcredmgrclient.h>
#include <smfrelationmgr.h>

#include "smfserver.h"
#include "smfpluginmanager.h"
#include "smftransportmanager.h"
#include "dsm.h"
#ifdef Q_OS_SYMBIAN
	#include "smfserversymbian_p.h"
#else
	#include "smfserverqt_p.h"
	#include "smfserverqtsession.h"
#endif


SmfServer::SmfServer(QObject* parent)
    : QObject(parent)
	{
	m_transportManager = NULL;
	m_pluginManager = NULL;
	m_credentialMngr = NULL;
	}

SmfServer::~SmfServer()
	{
	qDebug()<<"Inside Smfserver::~SmfServer()";
	if(m_transportManager)
		delete m_transportManager;
	if(m_pluginManager)
		delete m_pluginManager;
	if(m_credentialMngr)
		delete m_credentialMngr;
	if(m_SmfServerPrivate)
		{
		delete m_SmfServerPrivate;
		m_SmfServerPrivate = NULL;
		}
	}

bool SmfServer::startServer()
	{
	bool success = false;
	
	//Initialize all the component handles
	SmfTransportInitializeResult networkStatus = prepareTransport();
	
	m_pluginManager = SmfPluginManager::getInstance(this);
	qDebug()<<"After m_pluginManager construction";
	
	//	m_dataStoreManager = new SmfDataStoreManager();
	
#ifdef Q_OS_SYMBIAN
	//Initialize private implementation
	TRAPD(err, m_SmfServerPrivate = SmfServerSymbian::NewL(CActive::EPriorityStandard,this));
	qDebug()<<"SmfServerSymbian::NewL() = "<<err;

	if( KErrNone != err )
		return success;
	
	TInt error = m_SmfServerPrivate->Start( KSmfServerName );
	qDebug()<<"m_SmfServerPrivate->Start = "<<error;
	
    RSemaphore semaphore;
    User::LeaveIfError( semaphore.OpenGlobal( KSmfServerSemaphoreName ) );
    
    // Semaphore opened ok
    semaphore.Signal();
    semaphore.Close();
	
	if( KErrNone == error ) 
		{
		success = true;
		}
	else 
		{
		//error
		return success;
		}
#else
	// For non-symbian platforms
	m_SmfServerPrivate = new SmfServerQt(this);
	success = m_SmfServerPrivate->start();
	if (!success) 
		{
		return success;
		}
#endif
	
	m_credentialMngr = new SmfCredMgrClient();
    return success;
	}

//Note:- Almost all the following APIs are called by private impl via the handle
/**
 * This API is called by the private impl when client is authorized
 * @param interfaceID Interface id, provided by the private impl (it gets it from client)
 * @param pluginIDMap Map of plugins who implement this interface and corresponding provider,
 * this is returned to the private impl
 * It calls PM to get the list. Note:- PM may return SmfProviderBase which is superset of SmfProvider.
 * TODO:- session should store this map for future ref?
 */
void SmfServer::getPlugins(const SmfInterfaceID& interfaceID, QMap<SmfPluginID,SmfProvider>& pluginIDMap)
	{
	qDebug()<<"Inside SmfServer::getPlugins()";
	m_pluginManager->getPlugins(interfaceID,pluginIDMap);
	}

SmfPluginID SmfServer::getPlugin(const SmfInterfaceID& interfaceID,SmfProvider provider)
	{
	qDebug()<<"Inside SmfServer::getPlugin()";
	SmfPluginID id;
	m_pluginManager->getPluginId(interfaceID,provider, id);
	return id;
	}

/**
 * This API is called by the private impl to get a list of authorized plugins from CM
 * @param list List of plugins to be filtered
 * @param authList List of authorised plugins filled by CM
 * this is returned to the private impl
 * It calls CMclient to get the list synchronously
 * TODO:- session should store this for future ref?
 */
void SmfServer::getAuthorizedPlugins(QList<SmfPluginID>& list,QList<SmfPluginID>& authList)
	{
	qDebug()<<"Inside SmfServer::getAuthorizedPlugins()";
	authList.clear();
	for(int i=0;i<list.count();i++)
		{
		bool isAuthorized = m_credentialMngr->CheckPluginAuthentication(list[i]);
		if(isAuthorized)
			authList.append(list[i]);
		}
	}

SmfTransportInitializeResult SmfServer::prepareTransport()
	{
	m_transportManager = SmfTransportManager::getInstance();
	
	//checking the network status
	SmfTransportInitializeResult networkStatus = m_transportManager->initializeTransport();
	qDebug()<<"m_transportManager->initializeTransport() return = "<<networkStatus;
	return networkStatus;
	}

SmfError SmfServer::sendToPluginManager ( int requestID, SmfPluginID pluginID, 
		SmfInterfaceID interfaceID, SmfRequestTypeID requestTypeID, 
		QByteArray dataForPlugin )
	{
	qDebug()<<"Inside SmfServer::sendToPluginManager()";
	Q_UNUSED(interfaceID)
#ifdef DETAILEDDEBUGGING
	qDebug()<<"Request ID = "<<requestID;
	qDebug()<<"PluginID = "<<pluginID;
	qDebug()<<"Interface = "<<interfaceID;
	qDebug()<<"RequestType = "<<requestTypeID;
#endif
	
	//TODO:-PM should take page info too
	SmfError err = m_pluginManager->createRequest(requestID,pluginID,requestTypeID,dataForPlugin);
	qDebug()<<"m_pluginManager->createRequest() ret value = "<<err;
	
	return err;
	}

/**
 * Request the Plugin manager to get the data.
 * @param requestID Corresponds to a client's session
 * @param pluginID Plugin for which the request is intended
 * @param interfaceID Interface name
 * @param dataForPlugin Data to be sent for this request
 */
SmfError SmfServer::sendToPluginManager ( SmfPluginID pluginID, 
		SmfInterfaceID interfaceID, SmfRequestTypeID requestTypeID,
		QByteArray dataForPlugin, QByteArray &outputData)
	{
	qDebug()<<"Inside SmfServer::sendToPluginManager() for sync req";
	Q_UNUSED(interfaceID)
#ifdef DETAILEDDEBUGGING
	qDebug()<<"PluginID = "<<pluginID;
	qDebug()<<"Interface = "<<interfaceID;
	qDebug()<<"RequestType = "<<requestTypeID;
#endif

	//TODO:-PM should take page info too
	SmfError err = m_pluginManager->createSyncRequest(pluginID,requestTypeID,dataForPlugin, outputData);
	qDebug()<<"m_pluginManager->createSyncRequest() = "<<err;
	return err;
	}

SmfError SmfServer::sendToDSM ( QByteArray qtdataForDSM, SmfRequestTypeID opcode,
		QByteArray& qtdataFromDSM )
	{
	qDebug()<<"Inside SmfServer::sendToDSM()";
	
	DataStoreManager* dsm = DataStoreManager::getDataStoreManager();
	
	//Note:- deserialization and formation of user profile and social profile are done by server
	QDataStream readStream(&qtdataForDSM,QIODevice::ReadOnly);
	QDataStream writeStream(&qtdataFromDSM,QIODevice::WriteOnly);
	quint8 flag = 0;
	switch(opcode)
		{
		case SmfRelationCreate:
			{
			//read the incoming data
			SmfProvider *provider = new SmfProvider();
			SmfContact *contact = new SmfContact();
			readStream>>flag;
			if(flag)
				readStream>>*provider;
			else 
				{
				delete provider;
				provider = NULL;
				}
			readStream>>flag;
			if(flag)
				readStream>>*contact;
			else 
				{
				delete contact;
				contact = NULL;
				}
			SmfRelationId relnId = dsm->create(provider,contact);
			writeStream<<relnId;
			if(provider != NULL)
				delete provider;
			if(contact != NULL)
				delete contact;
			}
			break;
		case SmfRelationAssociate:
			{
			SmfRelationId relnId;
			SmfContact *contact = new SmfContact();
			SmfProvider *provider = new SmfProvider();
			readStream>>relnId;
			readStream>>flag;
			if(flag)
				readStream>>*contact;
			else 
				{
				delete contact;
				contact = NULL;
				}
			readStream>>flag;
			if(flag)
				readStream>>*provider;
			else 
				{
				delete provider;
				provider = NULL;
				}

			QString snsName = provider->serviceName();
			QString snsUrl = (provider->serviceUrl()).toString();
			QString snsDesc = provider->description();
			
			SmfError err = dsm->associate(relnId,contact,provider);
			int errInt = err;
			writeStream<<errInt;
			if(contact != NULL)
				delete contact;
			if(provider != NULL)
				delete provider;
			}
			break;
		case SmfRelationRemove:
			{
			SmfRelationId relnId;
			SmfContact *contact = new SmfContact();
			readStream>>relnId;
			readStream>>flag;
			if(flag)
				readStream>>*contact;
			else 
				{
				delete contact;
				contact = NULL;
				}
			SmfError err = dsm->remove(relnId, contact);
			int errInt = err;
			writeStream<<errInt;
			if(NULL != contact)
				delete contact;
			break;
			}
		case SmfRelationSearchById:
			{
			SmfRelationId relnId;
			readStream>>relnId;

			SmfRelationItem* relnItem = dsm->searchById(relnId);
			quint8 flag = 1;
			if(relnItem)
				{
				writeStream<<flag;
				writeStream<<*(relnItem);
				}
			else
				{
				flag = 0;
				writeStream<<flag;
				}
			}
			break;
		case SmfRelationSearchByContact:
			{
			SmfContact contact;
			readStream>>contact;

			SmfRelationId relnId = dsm->searchByContact(contact);
			writeStream<<relnId;
			break;
			}
		case SmfRelationCount:
			{
			SmfRelationId relationId;
			readStream>>relationId;
			int cnt = dsm->count(relationId);
			writeStream<<cnt;
			}
			break;
		case SmfRelationGet:
			{
			SmfRelationId relationId;
			quint32 index;
			readStream>>relationId;
			readStream>>index;
			SmfRelationItem* relnItem = dsm->getContact(relationId,index);
			quint8 flag = 1;
			if(relnItem)
				{
				writeStream<<flag;
				writeStream<<*(relnItem);
				}
			else
				{
				flag = 0;
				writeStream<<flag;
				}
			break;
			}
		case SmfRelationGetAll:
			{
			SmfRelationId relationId;
			readStream>>relationId;

			QList<SmfRelationItem> relnIditemList = dsm->getAll(relationId);
			writeStream<<relnIditemList;
			}
			break;
		case SmfRelationGetAllRelations:
			{
			QList<SmfRelationId> relnIdList = dsm->getAllRelations();
			writeStream<<relnIdList;
			}
			break;
		case SmfRelationDeleteRelation:
			{
			SmfRelationId relnId;
			readStream>>relnId;
			SmfError err = dsm->deleteRelation(relnId);
			int errInt = err;
			writeStream<<errInt;
			break;
			}
		default:
			break;
		}
	return SmfNoError;
	}

/**
 * This slot is invoked when CM finishes the authorization of the client.
 * @param authID As it contains the session ptr, sever directly invokes the session's API to notify success
 */
void SmfServer::clientAuthorizationFinished(bool success,SmfClientAuthID authID )
	{
	qDebug()<<"Inside SmfServer::clientAuthorizationFinished()";
	//TODO:- implement this api in session class
	//note:- in case success is false client completes the request with SmfErrClientAuthFailed
	//TODO:- define set of smf wide error after consulting with other module owners
	authID.session->clientAuthorizationFinished(success);
	}

/**
 * This API is called by PM once its done with request and parsing
 * @param requestID The request id which is completed
 * @param parsedData Serialized data(as per request type) filled by PM
 * @param error Error occured
 * TODO:- should use smf wide global errors instead
 */
void SmfServer::resultsAvailable ( int requestID, QByteArray* parsedData, SmfError error )
	{
	qDebug()<<"Inside SmfServer::resultsAvailable()";
#ifdef DETAILEDDEBUGGING
	qDebug()<<"requestID = "<<requestID;
	qDebug()<<"parsedData->size() = "<<parsedData->size();
	qDebug()<<"Error = "<<error;
#endif
	
	//Serialize error followed by actual data
	QByteArray dataWithError;
	QDataStream writer(&dataWithError,QIODevice::WriteOnly);
	writer<<error;
	if(parsedData->size())
		{
		writer<<*(parsedData);
		}
	
	//find out the appropriate session and request id and service that
	m_SmfServerPrivate->findAndServiceclient(requestID,&dataWithError,error);
	}

/**
 * This is called when CMclient notifies client expiry.
 * @param type notification type, set of enums for future expansion
 * @param id Plugin Id for which the authentication has expired
 */
void SmfServer::authenticationKeysExpired(NotificationType type,SmfPluginID id)
	{
	Q_UNUSED(type)
	Q_UNUSED(id)
	//resend the notify request
	//CMclient->requestAuthExpiryNotify();
	}

#ifdef CLIENT_SERVER_TEST
/**
 * Seems reduntant
 */
/*void SmfServer::serviceClient(QByteArray* parsedData)
	{
	Q_UNUSED(parsedData)
	}*/

dummyPM::dummyPM(SmfServer* server,QObject* parent)
: m_server(server),QObject(parent)
	{
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(responseAvailable()));
	}
SmfError dummyPM::createRequest ( const quint32& aSessionID, 
		const QString& aPluginID, 
		const SmfRequestTypeID& aOperation, 
		QByteArray& aInputData )
	{
	qDebug()<<QString::number(aSessionID);
	qDebug()<<aPluginID;
	qDebug()<<QString::number(aOperation);
	qDebug()<<QString::number(aInputData.size());
	m_timer->start(1000);
	}
void dummyPM::responseAvailable()
	{
	
	}
#endif
