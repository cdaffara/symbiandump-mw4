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
 * Private class implemented for implicit sharing of SmfLyrics class
 *
 */

#ifndef SMFLYRICS_P_H_
#define SMFLYRICS_P_H_

#include <QDateTime>
#include <QSharedData>

class SmfLyricsPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfLyricsPrivate( ) { 
		m_lyrics.clear();
		m_language.clear();
		m_lyricsId.clear(); 
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfLyricsPrivate( const SmfLyricsPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_lyrics ( aOther.m_lyrics ),
		m_language ( aOther.m_language ),
		m_releaseYr ( aOther.m_releaseYr ),
		m_lyricsId ( aOther.m_lyricsId ) 	{ }
	
	/**
	 * Destructor
	 */
	~SmfLyricsPrivate( )
		{
		}
  
	QByteArray m_lyrics;	// lyrics data
	QString m_language;		// language
	QDateTime m_releaseYr;	// release year
	QString m_lyricsId;		// lyrics id
	
};

#endif /* SMFLYRICS_P_H_ */
