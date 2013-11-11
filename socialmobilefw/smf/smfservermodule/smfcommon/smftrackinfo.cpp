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
 * The track info class represents information about a music track
 *
 */

#include <smftrackinfo.h>
#include <smftrackinfo_p.h>

/**
 * Constructor with default argument
 */
SmfTrackInfo::SmfTrackInfo( )
	{
	d = new SmfTrackInfoPrivate( );
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfTrackInfo::SmfTrackInfo( const SmfTrackInfo &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfTrackInfo& SmfTrackInfo::operator=( const SmfTrackInfo &aOther )
	{
	d->m_trackId = aOther.d->m_trackId;
	d->m_title = aOther.d->m_title;
	d->m_album = aOther.d->m_album;
	d->m_artists = aOther.d->m_artists;
	d->m_genre = aOther.d->m_genre;
	d->m_tags = aOther.d->m_tags;
	d->m_year = aOther.d->m_year;
	d->m_rating = aOther.d->m_rating;
	d->m_comments = aOther.d->m_comments;
	d->m_duration = aOther.d->m_duration;
	return *this;
	}

/**
 * Destructor
 */
SmfTrackInfo::~SmfTrackInfo( )
	{
	}

/**
 * Method to get the track title
 * @return The title of the track
 */
QString SmfTrackInfo::title( ) const
	{
	return d->m_title;
	}

/**
 * Method to get the track album
 * @return The album of the track
 */
SmfAlbum SmfTrackInfo::album( ) const
	{
	return d->m_album;
	}

/**
 * Method to get the track's artists
 * @return The artists of the track
 */
SmfArtists SmfTrackInfo::artists( ) const
	{
	return d->m_artists;
	}		

/**
 * Method to get the genre of the track
 * @return The genre of the track
 */
QString SmfTrackInfo::genre( ) const
	{
	return d->m_genre;
	}

/**
 * Method to get the track's tag
 * @return The tag of the track
 */
QStringList SmfTrackInfo::tags( ) const
	{
	return d->m_tags;
	}

/**
 * Method to get the release year of the track
 * @return The release year of the track
 */
QDateTime SmfTrackInfo::year( ) const
	{
	return d->m_year;
	}

/**
 * Method to get the track's rating
 * @return The rating of the track
 */
SmfMusicRating SmfTrackInfo::rating( ) const
	{
	return d->m_rating;
	}

/**
 * Method to get the comments on the track
 * @return The comments on the track
 */
QList<SmfComment> SmfTrackInfo::comments( ) const
	{
	return d->m_comments;
	}

/**
 * Method to get the track's duration
 * @return The duration of the track
 */
QTime SmfTrackInfo::duration( ) const
	{
	return d->m_duration;
	}

/**
 * Method to get the id of the track
 * @return The ID value 
 */
QString SmfTrackInfo::id( ) const
	{
	return d->m_trackId;
	}

/**
 * Method to set the track title
 * @param aTitle The new title of the track
 */
void SmfTrackInfo::setTitle( const QString &aTitle )
	{
	d->m_title = aTitle;
	}

/**
 * Method to set the track album
 * @param aAlbum The new album of the track
 */
void SmfTrackInfo::setAlbum( const SmfAlbum &aAlbum )
	{
	d->m_album = aAlbum;
	}

/**
 * Method to set the track's artists
 * @param aArtists The new artists of the track
 */
void SmfTrackInfo::setArtists( const SmfArtists &aArtists )
	{
	d->m_artists = aArtists;
	}

/**
 * Method to set the genre of the track
 * @param aGenre The new genre of the track
 */
void SmfTrackInfo::setGenre( const QString &aGenre )
	{
	d->m_genre = aGenre;
	}

/**
 * Method to set the track's tag
 * @param aTag The tag of the track
 */
void SmfTrackInfo::setTags( const QStringList &aTags )
	{
	d->m_tags = aTags;
	}

/**
 * Method to set the release year of the track
 * @param aYear The new release year of the track
 */
void SmfTrackInfo::setYear( const QDateTime &aYear )
	{
	d->m_year = aYear;
	}

/**
 * Method to set the track's rating
 * @param aRating The new rating of the track
 */
void SmfTrackInfo::setRating( const SmfMusicRating &aRating )
	{
	d->m_rating = aRating;
	}

/**
 * Method to set the comments on the track
 * @param aComment The new comment of the track
 */
void SmfTrackInfo::setComment( const QList<SmfComment> &aComments )
	{
	d->m_comments = aComments;
	}

/**
 * Method to set the duration of the track
 * @param aDuration The new duration of the track
 */
void SmfTrackInfo::setDuration( const QTime &aDuration )
	{
	d->m_duration = aDuration;
	}

/**
 * Method to set the id of the track
 * @param aId The ID value 
 */
void SmfTrackInfo::setId( const QString &aId )
	{
	d->m_trackId = aId;
	}


/**
 * Method for Externalization. Writes the SmfTrackInfo object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aTrackInfo The SmfTrackInfo object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfTrackInfo &aTrackInfo )
	{
	// Serialize d->m_trackId
	aDataStream<<aTrackInfo.d->m_trackId;
	
	// Serialize d->m_title
	aDataStream<<aTrackInfo.d->m_title;
	
	// Serialize d->m_album
	aDataStream<<aTrackInfo.d->m_album;
	
	// Serialize d->m_artists
	aDataStream<<aTrackInfo.d->m_artists;
	
	// Serialize d->m_genre
	aDataStream<<aTrackInfo.d->m_genre;
	
	// Serialize d->m_tags
	aDataStream<<aTrackInfo.d->m_tags;
	
	// Serialize d->m_year
	aDataStream<<aTrackInfo.d->m_year;
	
	// Serialize d->m_rating
	aDataStream<<aTrackInfo.d->m_rating;
	
	// Serialize d->m_comments
	aDataStream<<aTrackInfo.d->m_comments;
	
	// Serialize d->m_duration
	aDataStream<<aTrackInfo.d->m_duration;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfTrackInfo object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aTrackInfo The SmfTrackInfo object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfTrackInfo &aTrackInfo)
	{
	// Deserialize d->m_trackId
	aDataStream>>aTrackInfo.d->m_trackId;
	
	// Deserialize d->m_title
	aDataStream>>aTrackInfo.d->m_title;
	
	// Deserialize d->m_album
	aDataStream>>aTrackInfo.d->m_album;
	
	// Deserialize d->m_artists
	aDataStream>>aTrackInfo.d->m_artists;
	
	// Deserialize d->m_genre
	aDataStream>>aTrackInfo.d->m_genre;
	
	// Deserialize d->m_tags
	aDataStream>>aTrackInfo.d->m_tags;
	
	// Deserialize d->m_year
	aDataStream>>aTrackInfo.d->m_year;
	
	// Deserialize d->m_rating
	aDataStream>>aTrackInfo.d->m_rating;
	
	// Deserialize d->m_comments
	aDataStream>>aTrackInfo.d->m_comments;
	
	// Deserialize d->m_duration
	aDataStream>>aTrackInfo.d->m_duration;
	
	return aDataStream;
	}

