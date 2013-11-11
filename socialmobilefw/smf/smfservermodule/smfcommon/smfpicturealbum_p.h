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
 * Private class implemented for implicit sharing of SmfPictureAlbum class
 *
 */

#ifndef SMFPICTUREALBUM_P_H_
#define SMFPICTUREALBUM_P_H_


#include <QDateTime>
#include <QStringList>
#include <QUrl>
#include <QSharedData>
#include <smfclientglobal.h>
#include <smfpicturealbum.h>

class SmfPictureAlbumPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfPictureAlbumPrivate( ) { 
		m_title.clear();		// album title
		m_description.clear();	// description
		m_albumVisibility = SMFVisibilityPublic;// album visibility
		m_comments.clear();		 // comments
		m_keywords.clear();		// tags
		m_url.clear();				// url
		m_picCount=0;	//count of pictures
		m_albumId.clear();		// unique ID of the album, service provider specific	
	}
	

	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfPictureAlbumPrivate( const SmfPictureAlbumPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_albumId ( aOther.m_albumId),
		m_title ( aOther.m_title ),
		m_description ( aOther.m_description ),
		m_albumVisibility ( aOther.m_albumVisibility ),
		m_postedOn ( aOther.m_postedOn ),
		m_comments ( aOther.m_comments ),
		m_keywords ( aOther.m_keywords ),
		m_url ( aOther.m_url ),
		m_picCount (aOther.m_picCount),
		m_thumbnail ( aOther.m_thumbnail )	{ }
	
	/**
	 * Destructor
	 */
	~SmfPictureAlbumPrivate( )
		{
		}
  
	/**
	 * Member variables
	 */
	QString m_title;		// album title
	QString m_description;	// description
	SmfPictureVisibility m_albumVisibility;// album visibility
	QDateTime m_postedOn;	// date posted
	QList<SmfComment> m_comments;		 // comments
	QStringList m_keywords;		// tags
	QUrl m_url;				// url
	QImage m_thumbnail;		// thumbnail data
	quint32	 m_picCount;	//count of pictures
	QString m_albumId;		// unique ID of the album, service provider specific	
};

#endif /* SMFPICTUREALBUM_P_H_ */
