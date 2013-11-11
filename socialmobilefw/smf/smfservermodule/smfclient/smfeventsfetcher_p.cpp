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
 * Nalina Hariharan
 *
 * Description:
 * Private implementation of events related services
 *
 */

#include <qdebug.h>
#include <smfcontact.h>

#include "smfeventsfetcher.h"
#include "smfeventsfetcher_p.h"
#ifdef Q_OS_SYMBIAN
	#include "smfclientsymbian.h"
#else
	#include "smfclientqt.h"
#endif


SmfEventsFetcherPrivate::SmfEventsFetcherPrivate(SmfEventsFetcher* aEventFetcher)
		:m_eventFetcher(aEventFetcher)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	//TODO:- Use private impl for other platforms or else Qt impl
#endif
	}

SmfEventsFetcherPrivate::~SmfEventsFetcherPrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}
	}

SmfError SmfEventsFetcherPrivate::events(SmfLocation venue, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_eventFetcher->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<venue;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(eventFetcherInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfEventsGetEvents, maxalloc);
	return err;
	}

SmfError SmfEventsFetcherPrivate::venues(SmfLocation location, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_eventFetcher->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<location;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(eventFetcherInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfEventsGetVenues, maxalloc);
	return err;
	}

SmfError SmfEventsFetcherPrivate::attendees(SmfEvent event, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_eventFetcher->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<event;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(eventFetcherInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfEventsGetEventAttendees, maxalloc);
	return err;
	}

SmfError SmfEventsFetcherPrivate::postEvents(SmfEventList events)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_eventFetcher->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<events;
	
	write<<dataToPlugins;
	
	QString intfName(eventFetcherInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfEventsPostEvents, maxalloc);
	return err;
	}

SmfError SmfEventsFetcherPrivate::customRequest ( const int& operationId, QByteArray* customData )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_eventFetcher->getProvider();
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
	
	QString intfName(eventFetcherInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfEventsCustomRequest, maxalloc);
	return err;
	}

SmfError SmfEventsFetcherPrivate::cancelRequest()
	{
	qDebug()<<"Inside SmfEventsFetcherPrivate::cancelRequest()";
	QByteArray notused;
	QByteArray retData = m_SmfClientPrivate->sendSyncRequest(notused,SmfCancelRequest,1000, notused);
	
	//De-serialize it into SmfError
	QDataStream reader(&retData,QIODevice::ReadOnly);
	int val;
	reader>>val;
	SmfError error = (SmfError) val;
	return error;
	}

void SmfEventsFetcherPrivate::resultsAvailable(QByteArray result,SmfRequestTypeID opcode,SmfError error)
	{
	//note:- "result" is serialized and we need to de-serialize it as per opcode
	//Order of serialization Error value followed by data
	
	QDataStream reader(&result,QIODevice::ReadOnly);
	
	//Now de-serialize it based on opcode
	switch(opcode)
		{
		case SmfEventsGetEvents:
			{
			SmfEventList* m_events = new SmfEventList;
			reader>>*(m_events);
			
			// ToDo :-
			SmfResultPage page;
			
			m_eventFetcher->eventsAvailable(m_events,error,page);
			break;
			}
			
		case SmfEventsGetVenues:
			{
			SmfLocationList* m_venues = new SmfLocationList;
			reader>>*(m_venues);
			
			//ToDo :-
			SmfResultPage page;
			
			m_eventFetcher->venuesAvailable(m_venues,error,page);
			break;
			}
			
		case SmfEventsGetEventAttendees:
			{
			SmfContactList* m_list = new SmfContactList;
			reader>>*(m_list);
			
			//ToDo :-
			SmfResultPage page;
			
			m_eventFetcher->attendeesAvailable(m_list,error,page);
			break;
			}
			
		case SmfEventsPostEvents:
			m_eventFetcher->eventsUpdated(error);
			break;
			
		case SmfEventsCustomRequest:
   			{
   			int operationId;
   			QByteArray *data = new QByteArray;
   			reader>>operationId;
   			reader>>*data;
   			qDebug()<<"operationId = "<<operationId;
   			qDebug()<<"data size = "<<data->size();
   			emit m_eventFetcher->customDataAvailable(operationId, data);
   			break;
   			}
   			
		default:
			Q_ASSERT_X(1,"SmfEventsFetcherPrivate::resultsAvailable","unknown opcode");
		}
	}
