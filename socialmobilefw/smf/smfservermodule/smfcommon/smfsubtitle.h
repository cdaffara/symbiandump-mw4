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

#ifndef SMFSUBTITLE_H_
#define SMFSUBTITLE_H_

#include <qdatastream.h>
#include <QDateTime>
#include <QSharedData>
#include "smfclientglobal.h"
#include <QMetaType>

enum SmfSubtitleSearchFilter
	{
	SubtitleLanguage = 0,
	SubtitleFrameRate,		// value = 1
	SubtitleDuration,		// value = 2
	SubtitleReleaseYear,	// value = 3
	SubtitleAll = SubtitleLanguage | SubtitleFrameRate | 
			SubtitleDuration | SubtitleReleaseYear
	};

class SmfSubtitlePrivate;

/**
 * @ingroup smf_common_group
 * The subtitle class represents information about a track's subtitle
 */
class SMFCOMMON_EXPORT SmfSubtitle
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfSubtitle( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfSubtitle( const SmfSubtitle &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 */
	SmfSubtitle& operator=( const SmfSubtitle &aOther );
	
	/**
	 * Destructor
	 */
	~SmfSubtitle( );
	
	/**
	 * Method to get the subtitle as a bytearray
	 * @return The subtitle content
	 */
	QByteArray subtitle( ) const;
	
	/**
	 * Method to get the Url to the subtitle
	 * @return the subtitle url
	 */
	QUrl subtitleUrl() const;
	
	/**
	 * Method to get the language
	 * @return The language
	 */
	QString language( ) const;
	
	/**
	 * Method to get the frame rate
	 * @return the frame rate
	 */
	double frameRate( ) const;
	
	/**
	 * Method to get the duration
	 * @return the duration
	 */
	double duration( ) const;
	
	/**
	 * Method to get the release year
	 * @return The release year
	 */
	QDateTime releaseYear( ) const;
	
	/**
	 * Method to get the id of the subtitle
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Method to set the subtitle as a bytearray
	 * @param aSubtitle The subtitle content
	 */
	void setSubtitle( const QByteArray &aSubtitle );
	
	/**
	 * Method to set the subtitle url
	 * @param aSubtitleUrl The subtitle url
	 */
	 
	void setSubtitleUrl( const QUrl &aSubtitleUrl);
	
	/**
	 * Method to set the language
	 * @param aLang The language
	 */
	void setLanguage( const QString &aLang );
	
	/**
	 * Method to set the frame rate
	 * @param aFramerate the frame rate
	 */
	void setFrameRate( const double &aFramerate );
	
	/**
	 * Method to set the duration
	 * @param aDuration the duration
	 */
	void setDuration( const double &aDuration );
	
	/**
	 * Method to set the release year
	 * @param aRelYear The release year
	 */
	void setReleaseYear( const QDateTime &aRelYear );
	
	/**
	 * Method to set the id of the subtitle
	 * @param aId The ID value 
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfSubtitlePrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfSubtitle &aSubtitle );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfSubtitle &aSubtitle );
	
	};


/**
 * Method for Externalization. Writes the SmfSubtitle object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aSubtitle The SmfSubtitle object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT  QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfSubtitle &aSubtitle );

/**
 * Method for Internalization. Reads a SmfSubtitle object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aSubtitle The SmfSubtitle object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfSubtitle &aSubtitle);


typedef QList<SmfSubtitle> SmfSubtitleList;

// Make the class SmfSubtitle known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfSubtitle)
Q_DECLARE_METATYPE(QList<SmfSubtitle>)

#endif /* SMFSUBTITLE_H_ */
