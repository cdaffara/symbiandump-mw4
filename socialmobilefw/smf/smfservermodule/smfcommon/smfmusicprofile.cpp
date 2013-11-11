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
 * The music profile class represents a user's profile in music site
 *
 */

#include <smfmusicprofile.h>
#include <smfmusicprofile_p.h>

/**
 * Constructor with default argument
 */
SmfMusicProfile::SmfMusicProfile( )
	{
	d = new SmfMusicProfilePrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfMusicProfile::SmfMusicProfile( const SmfMusicProfile &aOther )
	:d ( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfMusicProfile& SmfMusicProfile::operator=( const SmfMusicProfile &aOther )
	{
	d->m_recent = aOther.d->m_recent;
	d->m_favorites = aOther.d->m_favorites;
	d->m_events = aOther.d->m_events;
	d->m_profileId = aOther.d->m_profileId;
	return *this;
	}

/**
 * Destructor
 */
SmfMusicProfile::~SmfMusicProfile( )
	{
	}

/**
 * Method to get the user's most recently heard tracks
 * @return The users most recently heard tracks
 */
QList<SmfTrackInfo> SmfMusicProfile::recentTracks( ) const
	{
	return d->m_recent;
	}

/**
 * Method to get the user's favorite tracks e.g. could be user.getLovedTracks for last.fm
 * @return The users favorite track list
 */
QList<SmfTrackInfo> SmfMusicProfile::favorites( ) const
	{
	return d->m_favorites;
	}

/**
 * Method to get a list of upcoming events that this user is attending
 * @return The list of upcoming events
 */
QList<SmfEvent> SmfMusicProfile::userEvents( ) const
	{
	return d->m_events;
	}

/**
 * Method to get the id of the music profile
 * @return The ID value 
 */
QString SmfMusicProfile::id( ) const
	{
	return d->m_profileId;
	}

/**
 * Method to set the user's recent tracks
 * @param aUsage The users new track list
 */
void SmfMusicProfile::setRecentTracks( const QList<SmfTrackInfo>& aUsage )
	{
	d->m_recent = aUsage;
	}

/**
 * Method to set the user's favorite tracks
 * @param aInterest The users new favorite track list
 */
void SmfMusicProfile::setFavorites( const QList<SmfTrackInfo>& aInterest )
	{
	d->m_favorites = aInterest;
	}

/**
 * Method to set the user events as list of SmfEvents
 * @param aList The list of events
 */
void SmfMusicProfile::setUserEvents( const QList<SmfEvent> &aList )
	{
	d->m_events = aList;
	}

/**
 * Method to set the id of the music profile
 * @param aId The ID value 
 */
void SmfMusicProfile::setId( const QString &aId )
	{
	d->m_profileId = aId;
	}


/**
 * Method for Externalization. Writes the SmfMusicProfile object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aProfile The SmfMusicProfile object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfMusicProfile &aProfile )
	{
	// Serialize d->m_recent
	aDataStream<<aProfile.d->m_recent;
	
	// Serialize d->m_favorites
	aDataStream<<aProfile.d->m_favorites;
	
	// Serialize d->m_events
	aDataStream<<aProfile.d->m_events;
	
	// Serialize d->m_profileId
	aDataStream<<aProfile.d->m_profileId;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfMusicProfile object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aProfile The SmfMusicProfile object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfMusicProfile &aProfile)
	{
	// Deserialize d->m_recent
	aDataStream>>aProfile.d->m_recent;
	
	// Deserialize d->m_favorites
	aDataStream>>aProfile.d->m_favorites;
	
	// Deserialize d->m_events
	aDataStream>>aProfile.d->m_events;
	
	// Deserialize d->m_profileId
	aDataStream>>aProfile.d->m_profileId;
	
	return aDataStream;
	}

