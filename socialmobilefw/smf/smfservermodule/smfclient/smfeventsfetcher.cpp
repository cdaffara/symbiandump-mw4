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
 * Events related services
 *
 */

#include "smfeventsfetcher.h"
#include "smfeventsfetcher_p.h"

SmfEventsFetcher::SmfEventsFetcher(SmfProvider *baseProvider)
		:m_baseProvider(baseProvider)
	{
	m_private = new SmfEventsFetcherPrivate(this);
	}

SmfEventsFetcher::~SmfEventsFetcher()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

SmfError SmfEventsFetcher::events(SmfLocation venue, int pageNum, int perPage)
	{
	return m_private->events(venue,pageNum,perPage);
	}

SmfError SmfEventsFetcher::venues(SmfLocation location, int pageNum, int perPage)
	{
	return m_private->venues(location,pageNum,perPage);
	}

SmfError SmfEventsFetcher::attendees(SmfEvent event, int pageNum, int perPage )
	{
	return m_private->attendees(event,pageNum,perPage);
	}

SmfError SmfEventsFetcher::postEvents(SmfEventList events)
	{
	return m_private->postEvents(events);
	}

SmfError SmfEventsFetcher::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfProvider* SmfEventsFetcher::getProvider() const
	{
	return m_baseProvider;
	}

SmfError SmfEventsFetcher::cancelRequest()
	{
	return m_private->cancelRequest();
	}
