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
 * Private Implementation of Relation Manager
 * 
 */
#include <qdebug.h>

#include "smfrelationmgr.h"
#include "smfrelationmgr_p.h"

#ifdef Q_OS_SYMBIAN
	#include "smfclientsymbian.h"
#else
	#include "smfclientqt.h"
#endif


SmfRelationMgrPrivate::SmfRelationMgrPrivate(SmfRelationMgr* relnMngr)
	{
	Q_UNUSED(relnMngr)
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL();
#endif
	}

SmfRelationMgrPrivate::~SmfRelationMgrPrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}
	}

SmfRelationId SmfRelationMgrPrivate::create( SmfProvider *provider, SmfContact *contact)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	if(provider)
		{
		m_argFlag = 1;
		writeStream<<m_argFlag;
		writeStream<<*provider;
		}
	else
		{
		m_argFlag = 0;
		writeStream<<m_argFlag;
		}
	if(contact)
		{
		m_argFlag = 1;
		writeStream<<m_argFlag;
		writeStream<<*contact;
		}
	else
		{
		m_argFlag = 0;
		writeStream<<m_argFlag;
		}
	
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	SmfRelationId relnId;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationCreate,m_dataForDSM,err,maxSmfRelationIdSize);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		readStream>>relnId;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return relnId;
	}

SmfError SmfRelationMgrPrivate::associate( SmfRelationId& relation,
		const SmfContact* contact, SmfProvider* provider)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
	if(contact)
		{
		m_argFlag = 1;
		writeStream<<m_argFlag;
		writeStream<<*contact;
		}
	else
		{
		m_argFlag = 0;
		writeStream<<m_argFlag;
		}
	if(provider)
		{
		m_argFlag = 1;
		writeStream<<m_argFlag;
		writeStream<<*provider;
		}
	else
		{
		m_argFlag = 0;
		writeStream<<m_argFlag;
		}
	
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	int maxAlloc = 100;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationAssociate,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		int errInt;
		readStream>>errInt;
		err = (SmfError)errInt;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return err;
	}

SmfError SmfRelationMgrPrivate::remove(SmfRelationId& relation,const SmfContact* contact)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
	if(contact)
		{
		m_argFlag = 1;
		writeStream<<m_argFlag;
		writeStream<<*contact;
		}
	else
		{
		m_argFlag = 0;
		writeStream<<m_argFlag;
		}
	
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	int maxAlloc = 100;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationRemove,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		int errInt;
		readStream>>errInt;
		err = (SmfError)errInt;
		}
	else
		{
		const QString errStr = QString::number(err);
		Q_ASSERT_X(1,"SmfRelationMgrPrivate::remove","error");
		}
	return err;
	}

SmfRelationItem* SmfRelationMgrPrivate::searchById(const SmfRelationId& relation)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
	
	m_dataFromDSM.clear();
	int maxAlloc = MaxSmfContactSize;
	SmfError err = SmfNoError;
	SmfRelationItem *relnItem = NULL;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationSearchById,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		m_argFlag = 0;
		readStream>>m_argFlag;
		if(m_argFlag)
			{
			relnItem = new SmfRelationItem;
			readStream>>*(relnItem);
			}
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return relnItem;
	}

SmfRelationId SmfRelationMgrPrivate::searchByContact( SmfContact contact)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<contact;
	
	m_dataFromDSM.clear();
	int maxAlloc = 1000;
	SmfError err = SmfNoError;
	SmfRelationId relnId;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationSearchByContact,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		readStream>>relnId;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return relnId;
	}

//number of contacts in a relation
uint SmfRelationMgrPrivate::count(SmfRelationId relation)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
	
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	int maxAlloc = 100;
	uint count;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationCount,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		readStream>>count;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return count;
	}

SmfRelationItem* SmfRelationMgrPrivate::get(SmfRelationId& relation, quint32 index)
	{
	m_dataForDSM.clear();
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
	writeStream<<index;
	
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	int maxAlloc = maxSmfRelationItemSize;
	SmfRelationItem* relationItem = NULL;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationGet,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		m_argFlag = 0;
		readStream>>m_argFlag;
		if(m_argFlag)
			{
			relationItem = new SmfRelationItem;
			readStream>>*(relationItem);
			}
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return relationItem;
	}

QList<SmfRelationItem> SmfRelationMgrPrivate::getAll(SmfRelationId& relation)
	{
	m_dataForDSM.clear();
	
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
		
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	QList<SmfRelationItem> relationItemList;
	int maxAlloc = maxSmfRelationItemSize*maxRelationItems;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationGetAll,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		readStream>>relationItemList;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return relationItemList;
	}

QList<SmfRelationId> SmfRelationMgrPrivate::getAllRelations()
	{
	m_dataForDSM.clear();
	
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	int maxAlloc = 1000*maxRelationItems;
	QList<SmfRelationId> relationIdList;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationGetAllRelations,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		readStream>>relationIdList;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return relationIdList;
	}

SmfError SmfRelationMgrPrivate::Delete(SmfRelationId& relation)
	{
	m_dataForDSM.clear();
	
	QDataStream writeStream(&m_dataForDSM,QIODevice::WriteOnly);
	writeStream<<relation;
		
	m_dataFromDSM.clear();
	SmfError err = SmfNoError;
	int maxAlloc = 100;
	m_dataFromDSM = m_SmfClientPrivate->sendDSMSyncRequest(SmfRelationDeleteRelation,m_dataForDSM,err,maxAlloc);
	if(err == SmfNoError)
		{
		QDataStream readStream(&m_dataFromDSM,QIODevice::ReadOnly);
		int errInt;
		readStream>>errInt;
		err = (SmfError)errInt;
		}
	else
		{
		User::Panic(_L("DSM err = "),err);
		}
	return err;
	}
