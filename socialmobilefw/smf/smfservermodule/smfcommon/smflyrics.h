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

#ifndef SMFLYRICS_H_
#define SMFLYRICS_H_

#include <qdatastream.h>
#include <QDateTime>
#include <QSharedData>
#include <smfclientglobal.h>
#include <QMetaType>

class SmfLyricsPrivate;

const int SmfMusicLyricsMaxSize = 5000;

/**
 * @ingroup smf_common_group
 * The lyrics class represents an instance of a music track's lyrics
 */
class SMFCOMMON_EXPORT SmfLyrics
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfLyrics( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfLyrics( const SmfLyrics &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The current object reference
	 */
	SmfLyrics& operator=( const SmfLyrics &aOther );
	
	/**
	 * Destructor
	 */
	~SmfLyrics( );
	
	/**
	 * Method to get the lyrics
	 * @return The lyrics data
	 */
	QByteArray lyrics( ) const;
	
	/**
	 * Method to get the language
	 * @return The language
	 */
	QString language( ) const;
	
	/**
	 * Method to get the release year
	 * @return The release year
	 */
	QDateTime releaseYear( ) const;
	
	/**
	 * Method to get the id of the lyrics
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Method to set the lyrics
	 * @param aLyrics The lyrics data
	 */
	void setLyrics( const QByteArray &aLyrics );
	
	/**
	 * Method to set the language
	 * @param aLang The language
	 */
	void setLanguage( const QString &aLang );
	
	/**
	 * Method to set the release year
	 * @param aRelYear The release year
	 */
	void setReleaseYear( const QDateTime &aRelYear );
	
	/**
	 * Method to set the id of the lyrics
	 * @param aId The ID value 
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfLyricsPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfLyrics &aLyrics );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfLyrics &aLyrics );
	
	};


/**
 * Method for Externalization. Writes the SmfLyrics object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aLyrics The SmfLyrics object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfLyrics &aLyrics );

/**
 * Method for Internalization. Reads a SmfLyrics object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aLyrics The SmfLyrics object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfLyrics &aLyrics);


typedef QList<SmfLyrics> SmfLyricsList;

// Make the class SmfLyrics known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfLyrics)
Q_DECLARE_METATYPE(QList<SmfLyrics>)


#endif /* SMFLYRICS_H_ */
