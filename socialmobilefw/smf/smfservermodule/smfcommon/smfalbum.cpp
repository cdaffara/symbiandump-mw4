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
 * The SmfAlbum class represents an album
 *
 */

#include <smfalbum.h>
#include <smfalbum_p.h>


/**
 * Constructor with default argument
 */
SmfAlbum::SmfAlbum( )
	{
	d = new SmfAlbumPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfAlbum::SmfAlbum( const SmfAlbum &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfAlbum& SmfAlbum::operator=( const SmfAlbum &aOther )
	{
	d->m_name = aOther.d->m_name;
	d->m_imageUrl = aOther.d->m_imageUrl;
	d->m_artists = aOther.d->m_artists;
	d->m_location = aOther.d->m_location;
	d->m_itemCount = aOther.d->m_itemCount;
	d->m_type = aOther.d->m_type;
	d->m_albumId = aOther.d->m_albumId;
	d->m_secondaryId = aOther.d->m_secondaryId;
	return *this;
	}

/**
 * Destructor
 */
SmfAlbum::~SmfAlbum( )
	{
	}

/**
 * Method to get the album name
 * @return The album name
 */
QString SmfAlbum::name( ) const
	{
	return d->m_name;
	}

/**
 * Method to get the album's image url if any
 * @return The album's image url if any
 */
QUrl SmfAlbum::imageUrl( ) const
	{
	return d->m_imageUrl;
	}

/**
 * Method to get the artist names
 * @return The list of artists in the album
 */
SmfArtists SmfAlbum::artists( ) const
	{
	return d->m_artists;
	}

/**
 * Method to get the location of this album
 * @return The location of this album
 */
SmfLocation SmfAlbum::location( ) const
	{
	return d->m_location;
	}

/**
 * Method to get the number of items in this album
 * @return The number of items in this album
 */
int SmfAlbum::itemsCount( ) const
	{
	return d->m_itemCount;
	}

/**
 * Method to get the type of this album
 * @return The type of this album
 */
SmfAlbumMediaType SmfAlbum::type( ) const
	{
	return d->m_type;
	}

/**
 * Method to get the id of the album
 * @return The ID value 
 */
QString SmfAlbum::id( ) const
	{
	return d->m_albumId;
	}

/**
 * Method to get the secondary id of the album, e.g. musicbrainz id of the album for a music album
 * @return The ID value 
 */
QString SmfAlbum::secondaryId( ) const
	{
	return d->m_secondaryId;
	}	


/**
 * Method to set the album name
 * @param aName The album name
 */
void SmfAlbum::setName( const QString &aName )
	{
	d->m_name = aName;
	}

/**
 * Method to set the album's image url
 * @param aUrl The album's image url
 */
void SmfAlbum::setImageUrl( const QUrl &aUrl )
	{
	d->m_imageUrl = aUrl;
	}

/**
 * Method to set the artist names
 * @param aArtists The list of artists in the album
 */
void SmfAlbum::setArtists( const SmfArtists &aArtists )
	{
	d->m_artists = aArtists;
	}

/**
 * Method to set the location of this album
 * @param aLoc The location of the album
 */
void SmfAlbum::setLocation(const SmfLocation &aLoc )
	{
	d->m_location = aLoc;
	}

/**
 * Method to set the number of items in this album
 * @param aCount the number of items in this album
 */
void SmfAlbum::setItemsCount( const int aCount )
	{
	d->m_itemCount = aCount;
	}

/**
 * Method to set the type of this album
 * @param aType the type of this album
 */
void SmfAlbum::setType(SmfAlbumMediaType aType)
	{
	d->m_type = aType;
	}

/**
 * Method to set the id of the album
 * @param aId The ID value 
 */
void SmfAlbum::setId( const QString &aId )
	{
	d->m_albumId = aId;
	}

/**
 * Method to set the secondary id of the album, e.g. musicbrainz id of the album for a music album
 * @param aSecondaryID the secondaryID of this album 
 */
void SmfAlbum::setSecondaryId( const QString &aSecondaryID )
	{
	d->m_secondaryId = aSecondaryID;
	}


/**
 * Method for Externalization. Writes the SmfAlbum object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aAlbum The SmfAlbum object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfAlbum &aAlbum )
	{
	// Serialize d->m_name
	aDataStream<<aAlbum.d->m_name;
	
	// Serialize d->m_imageUrl
	aDataStream<<aAlbum.d->m_imageUrl;
	
	// Serialize d->m_artists
	aDataStream<<aAlbum.d->m_artists;
	
	// Serialize d->m_location
	aDataStream<<aAlbum.d->m_location;
	
	// Serialize d->m_itemCount
	aDataStream<<aAlbum.d->m_itemCount;
	
	// Serialize d->m_type
	aDataStream<<aAlbum.d->m_type;
	
	// Serialize d->m_albumId
	aDataStream<<aAlbum.d->m_albumId;
	
	// Serialize d->m_secondaryId
	aDataStream<<aAlbum.d->m_secondaryId;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfAlbum object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aAlbum The SmfAlbum object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfAlbum &aAlbum)
	{
	// Deserialize d->m_name
	aDataStream>>aAlbum.d->m_name;
	
	// Deserialize d->m_imageUrl
	aDataStream>>aAlbum.d->m_imageUrl;
	
	// Deserialize d->m_artists
	aDataStream>>aAlbum.d->m_artists;
	
	// Deserialize d->m_location
	aDataStream>>aAlbum.d->m_location;
	
	// Deserialize d->m_itemCount
	aDataStream>>aAlbum.d->m_itemCount;
	
	// Deserialize d->m_type
	int val;
	aDataStream>>val;
	aAlbum.d->m_type = (SmfAlbumMediaType)val;
	
	// Deserialize d->m_albumId
	aDataStream>>aAlbum.d->m_albumId;
	
	// Deserialize d->m_secondaryId
	aDataStream>>aAlbum.d->m_secondaryId;
	
	return aDataStream;
	}
