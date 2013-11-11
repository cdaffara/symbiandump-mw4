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
 * Private class implemented for implicit sharing of SmfAlbum class
 *
 */

#ifndef SMFALBUM_P_H_
#define SMFALBUM_P_H_

#include <smfalbum.h>
#include <smfartists.h>
#include <smflocation.h>
#include <QStringList>
#include <QSharedData>

class SmfAlbumPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfAlbumPrivate( ) { 
		m_name.clear();
		m_imageUrl.clear();
		m_itemCount = 0;
		m_type = SmfAlbumUnspecified;
		m_albumId.clear();
		m_secondaryId.clear();
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfAlbumPrivate( const SmfAlbumPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_name ( aOther.m_name ), 
		m_imageUrl ( aOther.m_imageUrl ),
		m_artists ( aOther.m_artists ),
		m_location ( aOther.m_location ),
		m_itemCount ( aOther.m_itemCount ),
		m_type ( aOther.m_type ),
		m_albumId ( aOther.m_albumId ),
		m_secondaryId ( aOther.m_secondaryId ) { }
	
	/**
	 * Destructor
	 */
	~SmfAlbumPrivate( )
		{
		}
  
	QString	m_name;			// album name
	QUrl m_imageUrl;		// album's image url
	SmfArtists m_artists;	// album's artists
	SmfLocation m_location;	// album's location
	int m_itemCount;		// count of items in the album
	SmfAlbumMediaType m_type;// type of media in the album
	QString m_albumId;		// album Id
	QString m_secondaryId;	// the secondary id of the album, e.g.- 
							//-musicbrainz id of the album for a music album
	
};

#endif /* SMFALBUM_P_H_ */
