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
 * Private class implemented for implicit sharing of SmfEvent class
 *
 */

#ifndef SMFEVENT_P_H_
#define SMFEVENT_P_H_


#include <smflocation.h>
#include <smfartists.h>
#include <QStringList>
#include <QSharedData>

class SmfEventPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfEventPrivate( ) { 
		m_name.clear(); 
		m_url.clear();
		m_eventId.clear();
		
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfEventPrivate( const SmfEventPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_name ( aOther.m_name ), 
		m_dateTime ( aOther.m_dateTime ),
		m_duration ( aOther.m_duration ),
		m_artists ( aOther.m_artists ),
		m_venue ( aOther.m_venue ),
		m_url ( aOther.m_url ),
		m_eventId ( aOther.m_eventId ) 	{ }
	
	/**
	 * Destructor
	 */
	~SmfEventPrivate( )
		{
		}
  
	QString		m_name;		// event name
	QDateTime 	m_dateTime;	// event date and time
	QTime		m_duration;	// event duration
	SmfArtists 	m_artists;	// event artist names
	SmfLocation	m_venue;	// event venue
	QUrl 		m_url;		// ticket url
	QString 	m_eventId;	// event Id
	
};

#endif /* SMFEVENT_P_H_ */
