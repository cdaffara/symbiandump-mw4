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
 * Represents activity in terms similar to standard activity stream http://activitystrea.ms/
 * 
 */

#include <smfprovider.h>
#include <smfcontact.h>

#include "smfactivityfetcher.h"
#include "smfactivityfetcher_p.h"


SmfActivityFetcher::SmfActivityFetcher(SmfProvider *baseprovider)
		:m_baseProvider(baseprovider)
	{
	  //creating private impl wrapper
	  m_private = new SmfActivityFetcherPrivate(this);
	}

SmfActivityFetcher::~SmfActivityFetcher()
	{
	}

SmfError SmfActivityFetcher::selfActivities(int pageNum, int perPage)
	{
	return m_private->selfActivities(pageNum,perPage);
	}

SmfError SmfActivityFetcher::friendsActivities(const SmfContact & aFriend, int pageNum, int perPage)
	{
	return m_private->friendsActivities(aFriend,pageNum,perPage);
	}

SmfError SmfActivityFetcher::filtered(QList<SmfActivityObjectType> filters, int pageNum, int perPage)
	{
	return m_private->filtered(filters,pageNum,perPage);
	}

SmfError SmfActivityFetcher::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfProvider* SmfActivityFetcher::getProvider() const
	{
	return m_baseProvider;
	}

SmfError SmfActivityFetcher::cancelRequest()
	{
	return m_private->cancelRequest();
	}
