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
 * The music rating class represents an instance of rating 
 * about a music track
 *
 */

#include "smfmusicrating.h"
#include "smfmusicrating_p.h"

/**
 * Constructor with default argument
 * @param aParent The SmfTrackInfo instance
 */
SmfMusicRating::SmfMusicRating( SmfTrackInfo *aParent )
	{
	Q_UNUSED(aParent)
	d = new SmfMusicRatingPrivate();
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfMusicRating::SmfMusicRating( const SmfMusicRating &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator 
 * @param aOther The reference object
 */
SmfMusicRating& SmfMusicRating::operator=( const SmfMusicRating &aOther )
	{
	d->m_rating = aOther.d->m_rating;
	d->m_max = aOther.d->m_max;
	d->m_min = aOther.d->m_min;
	return *this;
	}

/**
 * Destructor
 */
SmfMusicRating::~SmfMusicRating( )
	{
	}

/**
 * Method to get the rating
 * @return The rating value
 */
int SmfMusicRating::rating( ) const
	{
	return d->m_rating;
	}

/**
 * Method to get the max rating
 * @return The max rating value
 */
int SmfMusicRating::maxRating( ) const
	{
	return d->m_max;
	}

/**
 * Method to get the min rating
 * @return The min rating value
 */
int SmfMusicRating::minRating( ) const
	{
	return d->m_min;
	}

/**
 * Method to set the rating
 * @param aRating The rating value
 */
void SmfMusicRating::setRating( const int &aRating )
	{
	d->m_rating = aRating;
	}

/**
 * Method to set the max rating
 * @param aMax The max rating value
 */
void SmfMusicRating::setMaxRating( const int &aMax )
	{
	d->m_max = aMax;
	}

/**
 * Method to set the min rating
 * @param aMin The min rating value
 */
void SmfMusicRating::setMinRating( const int &aMin )
	{
	d->m_min = aMin;
	}


/**
 * Method for Externalization. Writes the SmfMusicRating object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aMusicRating The SmfMusicRating object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfMusicRating &aMusicRating )
	{
	// Serialize d->m_rating
	aDataStream<<aMusicRating.d->m_rating;
	
	// Serialize d->m_max
	aDataStream<<aMusicRating.d->m_max;
	
	// Serialize d->m_min
	aDataStream<<aMusicRating.d->m_min;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfMusicRating object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aMusicRating The SmfMusicRating object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfMusicRating &aMusicRating)
	{
	// Deserialize d->m_rating
	aDataStream>>aMusicRating.d->m_rating;
	
	// Deserialize d->m_max
	aDataStream>>aMusicRating.d->m_max;
	
	// Deserialize d->m_min
	aDataStream>>aMusicRating.d->m_min;
	
	return aDataStream;
	}

