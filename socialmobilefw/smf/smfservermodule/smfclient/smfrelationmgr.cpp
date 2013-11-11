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
 * Interface spefication for managing associations between various social contacts
 * 
 */

#include "smfrelationmgr.h"
#include "smfrelationmgr_p.h"

SmfRelationItem::SmfRelationItem()
	{
	}

SmfProvider SmfRelationItem::getProvider() const
	{
	return m_provider;
	}

void SmfRelationItem::setProvider(SmfProvider& provider)
	{
	m_provider = provider;
	}

SmfContact SmfRelationItem::getContact() const
	{
	return m_contact;
	}

void SmfRelationItem::setContact(SmfContact& contact)
	{
	m_contact = contact;
	}

quint32	SmfRelationItem::getIndex() const
	{
	return m_index;
	}

void SmfRelationItem::setIndex(quint32 index)
	{
	m_index = index;
	}

QDataStream &operator<<( QDataStream &aDataStream,const SmfRelationItem &aRelnItem )
	{
	aDataStream<<aRelnItem.getProvider();
	aDataStream<<aRelnItem.getContact();
	aDataStream<<aRelnItem.getIndex();
	
	return aDataStream;
	}

QDataStream &operator>>( QDataStream &aDataStream,SmfRelationItem &aRelnItem)
	{
	SmfContact contact;
	SmfProvider provider;
	quint32 index;
	
	aDataStream>>provider;
	aDataStream>>contact;
	aDataStream>>index;
	
	aRelnItem.setProvider(provider);
	aRelnItem.setContact(contact);
	aRelnItem.setIndex(index);
	
	return aDataStream;
	}



SmfRelationMgr::SmfRelationMgr(QObject* parent):QObject(parent)
	{
	m_private = new SmfRelationMgrPrivate(this);
	}

SmfRelationMgr::~SmfRelationMgr()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

SmfRelationId SmfRelationMgr::create(SmfProvider *provider, SmfContact *contact)
	{
	return m_private->create(provider,contact);
	}

SmfError SmfRelationMgr::associate(SmfRelationId& relation,const SmfContact* contact,SmfProvider* provider)
	{
	return m_private->associate(relation,contact,provider);
	}

SmfError SmfRelationMgr::remove(SmfRelationId& relation,const SmfContact* contact)
	{
	return m_private->remove(relation,contact);
	}

SmfRelationItem* SmfRelationMgr::searchById(const SmfRelationId& relation)
	{
	return m_private->searchById(relation);
	}

SmfRelationId SmfRelationMgr::searchByContact( SmfContact contact)
	{
	return m_private->searchByContact(contact);
	}

uint SmfRelationMgr::count(SmfRelationId relation)
	{
	return m_private->count(relation);
	}

SmfRelationItem* SmfRelationMgr::get(SmfRelationId& relation, quint32 index)
	{
	return m_private->get(relation,index);
	}

QList<SmfRelationItem> SmfRelationMgr::getAll(SmfRelationId& relation)
	{
	return m_private->getAll(relation);
	}

QList<SmfRelationId> SmfRelationMgr::getAllRelations()
	{
	return m_private->getAllRelations();
	}

SmfError SmfRelationMgr::Delete(SmfRelationId& relation)
	{
	return m_private->Delete(relation);
	}
