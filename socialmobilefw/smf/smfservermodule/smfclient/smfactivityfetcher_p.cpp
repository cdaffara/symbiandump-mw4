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
 * Private Implementation of SmfActivityFetcher
 * 
 */

#include <QDebug>

#include "smfactivityfetcher.h"
#include "smfactivityfetcher_p.h"
#ifdef Q_OS_SYMBIAN
	#include "smfclientsymbian.h"
#else
	#include "smfclientqt.h"
#endif


SmfActivityFetcherPrivate::SmfActivityFetcherPrivate(SmfActivityFetcher *activityFetcher)
		:m_activityFetcher(activityFetcher)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	//TODO:- Qt private impl
#endif
	}

SmfActivityFetcherPrivate::~SmfActivityFetcherPrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}
	}

SmfError SmfActivityFetcherPrivate::selfActivities(int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	SmfProvider* m_baseProvider = m_activityFetcher->getProvider();
	//serialize start
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	//SmfProvider
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	//serialize end
	
	QString intfName(activityFetcherInterface);
	int max = MaxSmfActivityEntrySize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfActivitySelfActivity, max);
	return err;
	}

SmfError SmfActivityFetcherPrivate::friendsActivities(const SmfContact & aFriend, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	SmfProvider* m_baseProvider = m_activityFetcher->getProvider();
	//serialize start
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	//SmfProvider
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<aFriend;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(activityFetcherInterface);
	int max = MaxSmfActivityEntrySize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfActivityFriendsActivities, max);
	return err;
	}

SmfError SmfActivityFetcherPrivate::filtered(QList<SmfActivityObjectType> filters, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	SmfProvider* m_baseProvider = m_activityFetcher->getProvider();
	
	//serialize start
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	//SmfProvider
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<filters;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(activityFetcherInterface);
	int max = MaxSmfActivityEntrySize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfActivityFiltered, max);
	return err;
	}

SmfError SmfActivityFetcherPrivate::customRequest ( const int& operationId, QByteArray* customData )
	{
	SmfError err = SmfNoError;
	SmfProvider* m_baseProvider = m_activityFetcher->getProvider();
	
	//serialize start
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<operationId;
	if(customData)
		{
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*customData;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
	
	write<<dataToPlugins;
	
	QString intfName(activityFetcherInterface);
//ToDo:- How much size to allocate for custom data? keeping MaxSmfActivityEntrySize for now
	int maxAllocation = MaxSmfActivityEntrySize;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfActivityCustomRequest, maxAllocation);
	return err;
	}

SmfError SmfActivityFetcherPrivate::cancelRequest()
	{
	qDebug()<<"Inside SmfActivityFetcherPrivate::cancelRequest()";
	QByteArray notused;
	QByteArray retData = m_SmfClientPrivate->sendSyncRequest(notused,SmfCancelRequest,1000, notused);
	
	//De-serialize it into SmfError
	QDataStream reader(&retData,QIODevice::ReadOnly);
	int val;
	reader>>val;
	SmfError error = (SmfError) val;
	return error;
	}


void SmfActivityFetcherPrivate::resultsAvailable(QByteArray result,SmfRequestTypeID opcode, SmfError error)
	{
	qDebug()<<"SmfActivityFetcherPrivate::resultsAvailable";
	qDebug()<<"opcode = "<<opcode;

	QDataStream reader(&result,QIODevice::ReadOnly);
	
	//Now de-serialize it based on opcode
	switch(opcode)
		{
		case SmfActivitySelfActivity:
		case SmfActivityFriendsActivities:
		case SmfActivityFiltered:
			{
			SmfActivityEntryList* m_entries = new SmfActivityEntryList;
			reader>>*m_entries;
			qDebug()<<"m_entries.count = "<<m_entries->count();
			
			//ToDo:- not incorporating paging now
			SmfResultPage page;

			emit m_activityFetcher->resultsAvailable(m_entries,error,page);
			}
			break;
			
   		case SmfActivityCustomRequest:
   			{
   			int operationId;
   			QByteArray *data = new QByteArray;
   			reader>>operationId;
   			reader>>*data;
   			
   			qDebug()<<"operationId = "<<operationId;
   			qDebug()<<"data size = "<<data->size();
   			emit m_activityFetcher->customDataAvailable(operationId, data);
   			}
   			break;
		default:
			Q_ASSERT_X(1,"Activity Private Unknown opcode","default case");
		}
	}
