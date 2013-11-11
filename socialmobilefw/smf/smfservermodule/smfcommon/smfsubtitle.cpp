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
 * The subtitle class represents information about a track's subtitle
 *
 */

#include <smfsubtitle.h>
#include <smfsubtitle_p.h>

/**
 * Constructor with default argument
 */
SmfSubtitle::SmfSubtitle( )
	{
	d = new SmfSubtitlePrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfSubtitle::SmfSubtitle( const SmfSubtitle &aOther )
	:d(aOther.d)
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfSubtitle& SmfSubtitle::operator=( const SmfSubtitle &aOther )
	{
	d->m_subtitle = aOther.d->m_subtitle;
	d->m_language = aOther.d->m_language;
	d->m_frameRate = aOther.d->m_frameRate;
	d->m_duration = aOther.d->m_duration;
	d->m_releaseYr = aOther.d->m_releaseYr;
	d->m_subtitleId = aOther.d->m_subtitleId;
	d->m_subtitleUrl = aOther.d->m_subtitleUrl;
	return *this;
	}

/**
 * Destructor
 */
SmfSubtitle::~SmfSubtitle( )
	{
	}

/**
 * Method to get the subtitle as a bytearray
 * @return The subtitle content
 */
QByteArray SmfSubtitle::subtitle( ) const
	{
	return d->m_subtitle;
	}

/**
 * Method to get the Url to the subtitle
 * @return the subtitle url
 */
QUrl SmfSubtitle::subtitleUrl() const
	{
	return d->m_subtitleUrl;
	}
	
/**
 * Method to get the language
 * @return The language
 */
QString SmfSubtitle::language( ) const
	{
	return d->m_language;
	}

/**
 * Method to get the frame rate
 * @return the frame rate
 */
double SmfSubtitle::frameRate( ) const
	{
	return d->m_frameRate;
	}

/**
 * Method to get the duration
 * @return the duration
 */
double SmfSubtitle::duration( ) const
	{
	return d->m_duration;
	}

/**
 * Method to get the release year
 * @return The release year
 */
QDateTime SmfSubtitle::releaseYear( ) const
	{
	return d->m_releaseYr;
	}

/**
 * Method to get the id of the subtitle
 * @return The ID value 
 */
QString SmfSubtitle::id( ) const
	{
	return d->m_subtitleId;
	}

/**
 * Method to set the subtitle as a bytearray
 * @param aSubtitle The subtitle content
 */
void SmfSubtitle::setSubtitle( const QByteArray &aSubtitle )
	{
	d->m_subtitle = aSubtitle;
	}

/**
 * Method to set the subtitle as a bytearray
 * @param aSubtitle The subtitle content
 */
void SmfSubtitle::setSubtitleUrl( const QUrl &aSubtitleUrl)
	{
	 d->m_subtitleUrl = aSubtitleUrl;
	}

/**
 * Method to set the language
 * @param aLang The language
 */
void SmfSubtitle::setLanguage( const QString &aLang )
	{
	d->m_language = aLang;
	}

/**
 * Method to set the frame rate
 * @param aFramerate the frame rate
 */
void SmfSubtitle::setFrameRate( const double &aFramerate )
	{
	d->m_frameRate = aFramerate;
	}

/**
 * Method to set the duration
 * @param aDuration the duration
 */
void SmfSubtitle::setDuration( const double &aDuration )
	{
	d->m_duration = aDuration;
	}

/**
 * Method to set the release year
 * @param aRelYear The release year
 */
void SmfSubtitle::setReleaseYear( const QDateTime &aRelYear )
	{
	d->m_releaseYr = aRelYear;
	}

/**
 * Method to set the id of the subtitle
 * @param aId The ID value 
 */
void SmfSubtitle::setId( const QString &aId )
	{
	d->m_subtitleId = aId;
	}


/**
 * Method for Externalization. Writes the SmfSubtitle object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aSubtitle The SmfSubtitle object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfSubtitle &aSubtitle )
	{
	// Serialize m_subtitle
	aDataStream<<aSubtitle.d->m_subtitle;
	
	// Serialize m_subtitleUrl
	aDataStream<<aSubtitle.d->m_subtitleUrl;
	
	// Serialize m_language
	aDataStream<<aSubtitle.d->m_language;
	
	// Serialize m_frameRate
	aDataStream<<aSubtitle.d->m_frameRate;
	
	// Serialize m_duration
	aDataStream<<aSubtitle.d->m_duration;
	
	// Serialize m_releaseYr
	aDataStream<<aSubtitle.d->m_releaseYr;
	
	// Serialize m_subtitleId
	aDataStream<<aSubtitle.d->m_subtitleId;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfSubtitle object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aSubtitle The SmfSubtitle object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfSubtitle &aSubtitle)
	{
	// Deserialize m_subtitle
	aDataStream>>aSubtitle.d->m_subtitle;
	
	// Deserialize m_subtitleUrl
	aDataStream>>aSubtitle.d->m_subtitleUrl;
	
	// Deserialize m_language
	aDataStream>>aSubtitle.d->m_language;
	
	// Deserialize m_frameRate
	aDataStream>>aSubtitle.d->m_frameRate;
	
	// Deserialize m_duration
	aDataStream>>aSubtitle.d->m_duration;
	
	// Deserialize m_releaseYr
	aDataStream>>aSubtitle.d->m_releaseYr;
	
	// Deserialize m_subtitleId
	aDataStream>>aSubtitle.d->m_subtitleId;
	
	return aDataStream;
	}

