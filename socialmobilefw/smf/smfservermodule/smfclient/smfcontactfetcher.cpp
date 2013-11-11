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
 * The SmfContactFetcher class is for fetching SmfContact related info
 *
 */

#include "smfcontactfetcher.h"
#include "smfcontactfetcher_p.h"

SmfContactFetcher::SmfContactFetcher(SmfProvider* baseProvider)
	{
	m_baseProvider = baseProvider;
	
	//creating private impl wrapper
	m_private = new SmfContactFetcherPrivate(this);
	}

SmfContactFetcher::SmfContactFetcher(SmfProvider* baseProvider, SmfContact* contact)
	{
	m_baseProvider = baseProvider;
	m_frndContact = contact;
	
	//creating private impl wrapper
	m_private = new SmfContactFetcherPrivate(this);
	}

SmfContactFetcher::~SmfContactFetcher()
	{
	if(m_baseProvider)
		{
		delete m_baseProvider;
		m_baseProvider = NULL;
		}
	if(m_frndContact)
		{
		delete m_frndContact;
		m_frndContact = NULL;
		}
	/** @TODO:- Request to unload the plugin*/
	}

SmfError SmfContactFetcher::friends(int pageNum,int perPage)
	{
	return m_private->friends(pageNum,perPage);
	}

SmfError SmfContactFetcher::followers(int pageNum,int perPage)
	{
	return m_private->followers(pageNum,perPage);
	}

SmfError  SmfContactFetcher::search(SmfContact* contact,int pageNum,int perPage) 
	{
	return m_private->search(contact,pageNum,perPage);
	}

SmfError  SmfContactFetcher::searchNear(SmfLocation* location,SmfLocationSearchBoundary proximity,int pageNum,int perPage) 
	{
	return m_private->searchNear(location, proximity, pageNum, perPage);
	}

SmfError SmfContactFetcher::groups(int pageNum,int perPage) 
	{
	return m_private->groups(pageNum,perPage);
	}

SmfError  SmfContactFetcher::searchInGroup(SmfGroup group, SmfContact *contact, int pageNum,int perPage) 
	{
	return m_private->searchInGroup(group,contact,pageNum,perPage);
	}

SmfError SmfContactFetcher::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfProvider* SmfContactFetcher::getProvider() const
	{
	return m_baseProvider;
	}

SmfError SmfContactFetcher::cancelRequest()
	{
	return m_private->cancelRequest();
	}
