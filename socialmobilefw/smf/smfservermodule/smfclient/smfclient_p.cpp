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
 * Private Qt wrapper
 */

#include <qdebug.h>

#include "smfclient_p.h"

SmfClientPrivate::SmfClientPrivate(SmfClient* client):m_client(client)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	m_SmfClientPrivate = new SmfClientQt(this);
#endif
	}


QList<SmfProvider>* SmfClientPrivate::GetServices(const QString serviceName)
	{
	qDebug()<<"Inside SmfClientPrivate::GetServices()";
	m_baseProvider = NULL;
	  
	QString intfName(serviceName);
	m_providerSerialized.clear();
	
	//Getting the serialized list of SmfProvider synchronously
	//TODO:- Max size for SmfProviderList 1000
	qDebug()<<"Before m_SmfClientPrivate->sendRequest";
	QByteArray notused;
	m_providerSerialized = m_SmfClientPrivate->sendSyncRequest(intfName,SmfGetService,1000, notused);
	
	if(m_baseProvider)
		{
		delete m_baseProvider;
		m_baseProvider = NULL;
		}
	m_baseProvider = new QList<SmfProvider>;
	
	//De-serialize it into QList<SmfProvider>
	QDataStream reader(&m_providerSerialized,QIODevice::ReadOnly);
	reader>>*(m_baseProvider);
	return m_baseProvider;
	}

//This api is not reqd as result will be synchronous
void SmfClientPrivate::resultsAvailable(QByteArray result,SmfRequestTypeID opcode,SmfError error)
	{
	Q_UNUSED(result)
	Q_UNUSED(opcode)
	Q_UNUSED(error)
	}
