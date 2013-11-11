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
 * The SmfLyrics class represents an instance of a music track's lyrics
 *
 */

#include <smflyrics.h>
#include <smflyrics_p.h>

/**
 * Constructor with default argument
 */
SmfLyrics::SmfLyrics( )
	{
	d = new SmfLyricsPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfLyrics::SmfLyrics( const SmfLyrics &aOther )
	:d ( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfLyrics& SmfLyrics::operator=( const SmfLyrics &aOther )
	{
	d->m_lyrics = aOther.d->m_lyrics;
	d->m_language = aOther.d->m_language;
	d->m_releaseYr = aOther.d->m_releaseYr;
	d->m_lyricsId = aOther.d->m_lyricsId;
	return *this;
	}

/**
 * Destructor
 */
SmfLyrics::~SmfLyrics( )
	{
	}

/**
 * Method to get the lyrics
 * @return The lyrics data
 */
QByteArray SmfLyrics::lyrics( ) const
	{
	return d->m_lyrics;
	}

/**
 * Method to get the language
 * @return The language
 */
QString SmfLyrics::language( ) const
	{
	return d->m_language;
	}

/**
 * Method to get the release year
 * @return The release year
 */
QDateTime SmfLyrics::releaseYear( ) const
	{
	return d->m_releaseYr;
	}

/**
 * Method to get the id of the lyrics
 * @return The ID value 
 */
QString SmfLyrics::id( ) const
	{
	return d->m_lyricsId;
	}

/**
 * Method to set the lyrics
 * @param aLyrics The lyrics data
 */
void SmfLyrics::setLyrics( const QByteArray &aLyrics )
	{
	d->m_lyrics = aLyrics;
	}

/**
 * Method to set the language
 * @param aLang The language
 */
void SmfLyrics::setLanguage( const QString &aLang )
	{
	d->m_language = aLang;
	}

/**
 * Method to set the release year
 * @param aRelYear The release year
 */
void SmfLyrics::setReleaseYear( const QDateTime &aRelYear )
	{
	d->m_releaseYr = aRelYear;
	}

/**
 * Method to set the id of the lyrics
 * @param aId The ID value 
 */
void SmfLyrics::setId( const QString &aId )
	{
	d->m_lyricsId = aId;
	}


/**
 * Method for Externalization. Writes the SmfLyrics object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aLyrics The SmfLyrics object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfLyrics &aLyrics )
	{
	// Serialize d->m_lyrics
	aDataStream<<aLyrics.d->m_lyrics;
	
	// Serialize d->m_language
	aDataStream<<aLyrics.d->m_language;
	
	// Serialize d->m_releaseYr
	aDataStream<<aLyrics.d->m_releaseYr;
	
	// Serialize d->m_lyricsId
	aDataStream<<aLyrics.d->m_lyricsId;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfLyrics object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aLyrics The SmfLyrics object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfLyrics &aLyrics)
	{
	// Deserialize d->m_lyrics
	aDataStream>>aLyrics.d->m_lyrics;
	
	// Deserialize d->m_language
	aDataStream>>aLyrics.d->m_language;
	
	// Deserialize d->m_releaseYr
	aDataStream>>aLyrics.d->m_releaseYr;
	
	// Deserialize d->m_lyricsId
	aDataStream>>aLyrics.d->m_lyricsId;
	
	return aDataStream;
	}

