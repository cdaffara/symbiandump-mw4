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
 * Private class implemented for implicit sharing of SmfPlaylist class
 *
 */

#ifndef SMFPLAYLIST_P_H_
#define SMFPLAYLIST_P_H_

#include <smftrackinfo.h>
#include <QSharedData>

class SmfPlaylistPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfPlaylistPrivate( ) { 
		m_version.clear();
		m_title.clear();
		m_author.clear();
		m_comments.clear();
		m_info.clear();
		m_location.clear();
		m_playlistId.clear();
		m_image.clear();
		m_license.clear();
		m_attribution.clear();
		m_trackList.clear();
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfPlaylistPrivate( const SmfPlaylistPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_version ( aOther.m_version ), 
		m_title ( aOther.m_title ),
		m_author ( aOther.m_author ),
		m_comments ( aOther.m_comments ), 
		m_info ( aOther.m_info ),
		m_location ( aOther.m_location ),
		m_playlistId ( aOther.m_playlistId ), 
		m_image ( aOther.m_image ),
		m_creationDate ( aOther.m_creationDate ),
		m_license ( aOther.m_license ), 
		m_attribution ( aOther.m_attribution ),
		m_metadata ( aOther.m_metadata ),
		m_extension ( aOther.m_extension ),
		m_trackList ( aOther.m_trackList ) { }
	
	/**
	 * Destructor
	 */
	~SmfPlaylistPrivate( )
		{
		}
  
	/**
	 * Data members
	 */
	QString m_version;				// playlist version
	QString m_title;				// playlist title
	QString m_author;				// playlist creator
	QList<SmfComment> m_comments;	// comments about the playlist
	QUrl m_info;					// URI of a web page to find out more about this playlist
	QUrl m_location;				// The Source URI for this playlist
	QString m_playlistId;			// The playlist ID
	QUrl m_image;					// URI of an image to display in the absence of an 
									// image for playlist's tracks
	QDateTime m_creationDate;		// creation date of the playlist
	QUrl m_license;					// URI of a resource that describes the license under 
									// which this playlist was released
	QList<QUrl> m_attribution;		// An ordered list of URIs. The purpose is to satisfy licenses 
									// allowing modification but requiring attribution
	QVariantMap m_metadata;			// The meta element allows metadata fields to be added to XSPF
	QVariantMap m_extension; 		// The extension element allows non-XSPF XML 
									// to be included in XSPF documents
	QList<SmfTrackInfo> m_trackList;// list of tracks in the playlist
	
};

#endif /* SMFPLAYLIST_P_H_ */
