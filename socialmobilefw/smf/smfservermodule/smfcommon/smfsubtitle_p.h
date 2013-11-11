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
 * Private class implemented for implicit sharing of SmfSubtitle class
 *
 */

#ifndef SMFSUBTITLE_P_H_
#define SMFSUBTITLE_P_H_

#include <QDateTime>
#include <QSharedData>
#include <QUrl>

class SmfSubtitlePrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfSubtitlePrivate( ) { 
		m_language.clear();
		m_frameRate = 0;
		m_duration = 0;
		m_subtitleId.clear();
		m_subtitleUrl.clear();
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfSubtitlePrivate( const SmfSubtitlePrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_subtitle ( aOther.m_subtitle ), 
		m_language ( aOther.m_language ),
		m_frameRate ( aOther.m_frameRate ),
		m_duration ( aOther.m_duration ),
		m_releaseYr ( aOther.m_releaseYr ),
		m_subtitleId ( aOther.m_subtitleId ),
		m_subtitleUrl (aOther.m_subtitleUrl){ }
	
	/**
	 * Destructor
	 */
	~SmfSubtitlePrivate( )
		{
		}
  
	QByteArray m_subtitle;	// subtitle data
	QString m_language;		// language
	double m_frameRate;		// frame rate
	double m_duration;		// duration
	QDateTime m_releaseYr;	// release year
	QString m_subtitleId;	// subtitle id
	QUrl m_subtitleUrl;  	// subtitle Url
	
};

#endif /* SMFSUBTITLE_P_H_ */
