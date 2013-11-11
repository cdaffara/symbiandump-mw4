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
 * Private class implemented for implicit sharing of SmfMusicProfile class
 *
 */

#ifndef SMFMUSICPROFILE_P_H_
#define SMFMUSICPROFILE_P_H_


#include <smftrackinfo.h>
#include <smfevent.h>
#include <QSharedData>

class SmfMusicProfilePrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfMusicProfilePrivate( ) { 
		m_recent.clear(); 
		m_favorites.clear();
		m_events.clear();
		m_profileId.clear();
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfMusicProfilePrivate( const SmfMusicProfilePrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_recent ( aOther.m_recent ), 
		m_favorites ( aOther.m_favorites ),
		m_events ( aOther.m_events ),
		m_profileId ( aOther.m_profileId ) 	{ }
	
	/**
	 * Destructor
	 */
	~SmfMusicProfilePrivate( )
		{
		}
  
	QList<SmfTrackInfo> m_recent;	// recently heard tracks
	QList<SmfTrackInfo> m_favorites;// favorite tracks
	QList<SmfEvent> m_events;		// events list
	QString m_profileId;			// profile Id
	
};

#endif /* SMFMUSICPROFILE_P_H_ */
