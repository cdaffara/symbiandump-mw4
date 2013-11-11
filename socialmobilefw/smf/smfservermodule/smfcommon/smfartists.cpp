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
 * The SmfArtists class represents the artists in a track or an album
 *
 */

#include <smfartists.h>
#include <smfartists_p.h>

/**
 * Constructor with default argument
 */
SmfArtists::SmfArtists( )
	{
	d = new SmfArtistsPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfArtists::SmfArtists( const SmfArtists &aOther )
	:d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfArtists& SmfArtists::operator=( const SmfArtists &aOther )
	{
	d->m_names = aOther.d->m_names;
	d->m_image = aOther.d->m_image;
	d->m_url = aOther.d->m_url;
	d->m_tags = aOther.d->m_tags;
	d->m_id = aOther.d->m_id;
	return *this;
	}

/**
 * Destructor
 */
SmfArtists::~SmfArtists( )
	{
	}

/**
 * Method to get the artists names
 * @return The list of artists
 */
QStringList SmfArtists::names( ) const
	{
	return d->m_names;
	}

/**
 * Method to get the image of the artists
 * @return The image of te artists
 */
QImage SmfArtists::image( ) const
	{
	return d->m_image;
	}

/**
 * Method to get the url of the artists
 * @return The url of the artists
 */
QUrl SmfArtists::url( ) const
	{
	return d->m_url;
	}

/**
 * Method to get the tags applied for artist(s) 
 * @return list of tags, NULL if not supported
 */
QStringList SmfArtists::tags( ) const
	{
	return d->m_tags;
	}

/**
 * Service specific Id for this artist(s), e.g. could be musicbrainz id for this artist 
 * @return service specific id of the artists, NULL if not supported
 */
QString SmfArtists::id( ) const
	{
	return d->m_id;
	}

/**
 * Method to set the artists names
 * @param aList The list of artists names
 */
void SmfArtists::setNames( const QStringList &aList )
	{
	d->m_names = aList;
	}

/**
 * Method to set the image of the artists
 * @param aImage The image of the artists
 */
void SmfArtists::setImage( const QImage &aImage )
	{
	d->m_image = aImage;
	}

/**
 * Method to set the url of the artists
 * @param aUrl The url of the artists
 */
void SmfArtists::setUrl( const QUrl &aUrl )
	{
	d->m_url = aUrl;
	}

/**
 * Method to set tags of the artists
 * @param aTags tags for the artists
 */
void SmfArtists::setTags( const QStringList &aTags )
	{
	d->m_tags = aTags;
	}

/**
 * Method to set the service specific ID of the artists
 * @param aId The id of the artists
 */
void SmfArtists::setId( const QString &aId )
	{
	d->m_id = aId;
	}

/**
 * Method for Externalization. Writes the SmfArtists object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aArtists The SmfArtists object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfArtists &aArtists )
	{
	// Serialize d->m_image
	aDataStream<<aArtists.d->m_image;
	
	// Serialize d->m_names
	aDataStream<<aArtists.d->m_names;
	
	// Serialize d->m_url
	aDataStream<<aArtists.d->m_url;
	
	// Serialize d->m_tags
	aDataStream<<aArtists.d->m_tags;
	
	// Serialize d->m_id
	aDataStream<<aArtists.d->m_id;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfArtists object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aArtists The SmfArtists object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfArtists &aArtists)
	{
	// Deserialize d->m_image
	aDataStream>>aArtists.d->m_image;
	
	// Deserialize d->m_names
	aDataStream>>aArtists.d->m_names;
	
	// Deserialize d->m_url
	aDataStream>>aArtists.d->m_url;
	
	// Deserialize d->m_tags
	aDataStream>>aArtists.d->m_tags;
	
	// Deserialize d->m_id
	aDataStream>>aArtists.d->m_id;
	
	return aDataStream;
	}

