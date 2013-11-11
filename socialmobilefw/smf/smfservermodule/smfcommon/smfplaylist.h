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
 * The playlist class represents an instance of a playlist.
 * SmfPlaylist class is in accordance with the XML Shareable Playlist Format (XSPF)
 * as mentioned in http://xspf.org/xspf-v1.html
 *
 */

#ifndef SMFPLAYLIST_H_
#define SMFPLAYLIST_H_

#include <qdatastream.h>
#include <QSharedData>
#include <QMetaType>
#include <QVariant>

#include "smfclientglobal.h"
#include "smftrackinfo.h"
#include "smfcomment.h"

class SmfPlaylistPrivate;

const int SmfMusicPlaylistMaxSize = 5000;

/**
 * @ingroup smf_common_group
 * The playlist class represents an instance of a playlist.
 * SmfPlaylist class is in accordance with the XML Shareable Playlist Format (XSPF)
 * as mentioned in http://xspf.org/xspf-v1.html
 */
class SMFCOMMON_EXPORT SmfPlaylist
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfPlaylist( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfPlaylist( const SmfPlaylist &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 */
	SmfPlaylist& operator=( const SmfPlaylist &aOther );
	
	/**
	 * Destructor
	 */
	~SmfPlaylist( );
	
	/**
	 * Method to get the version of the playlist
	 * @return The version of the playlist
	 */
	QString version( ) const;
	
	/**
	 * Method to get the playlist title
	 * @return The title of the playlist
	 */
	QString playListTitle( ) const;
	
	/**
	 * Method to get the author of the playlist
	 * @return The author of the playlist
	 */
	QString author( ) const;
	
	/**
	 * Method to get the comments on the playlist
	 * @return The comments on the playlist
	 */
	QList<SmfComment> comments( ) const;
	
	/**
	 * Method to get the URI of a web page to find out more about this playlist
	 * @return The info Url
	 */
	QUrl info( ) const;
	
	/**
	 * Method to get the Source URI for this playlist
	 * @return The Source URI for this playlist
	 */
	QUrl location( ) const;
	
	/**
	 * Method to get the id of the playlist
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Method to get the URI of an image to display in the absence 
	 * of an image for playlist's tracks
	 * @return The deafult image Url
	 */
	QUrl image( ) const;
	
	/**
	 * Method to get the creation date of the playlist
	 * @return The date and time of creation of the playlist
	 */
	QDateTime creationDate( ) const;
	
	/**
	 * Method to get the URI of a resource that describes the license 
	 * under which this playlist was released
	 * @return The license Url
	 */
	QUrl license( ) const;
	
	/**
	 * Method to get the ordered list of URIs. The purpose is to satisfy 
	 * licenses allowing modification but requiring attribution
	 * @return The list of license Urls
	 */
	QList<QUrl> attribution( ) const;
	
	/**
	 * Method to get the meta element that allows metadata fields to be added to XSPF
	 * @return The meta elements as a QVarintMap
	 */
	QVariantMap metadata( ) const;
	
	/**
	 * Method to get the extension element that allows non-XSPF XML 
	 * to be included in XSPF documents
	 * @return The extension elements as a QVarintMap
	 */
	QVariantMap extension( ) const;
	
	/**
	 * Method to get the list of tracks in the playlist
	 * @return The list of tracks in the playlist
	 */
	QList<SmfTrackInfo> trackList( ) const;
	
	/**
	 * Method to set the version of the playlist
	 * @param aVersion The version of the playlist
	 */
	void setVersion( const QString& aVersion );
	
	/**
	 * Method to set the playlist title
	 * @param aTitle The title of the playlist
	 */
	void setPlayListTitle( const QString &aTitle );
	
	/**
	 * Method to set the author of the playlist
	 * @param aAuthor The author of the playlist
	 */
	void setAuthor( const QString& aAuthor );
	
	/**
	 * Method to set the comments on the playlist
	 * @param aComments The comments on the playlist
	 */
	void setComments( const QList<SmfComment>& aComments );
	
	/**
	 * Method to set the URI of a web page to find out more about this playlist
	 * @param aInfoUrl The info Url
	 */
	void setInfo( const QUrl& aInfoUrl );
	
	/**
	 * Method to set the Source URI for this playlist
	 * @param aLocation The Source URI for this playlist
	 */
	void setLocation( const QUrl& aLocation );
	
	/**
	 * Method to set the id of the playlist
	 * @param aId The ID value 
	 */
	void setId( const QString &aId);
	
	/**
	 * Method to set the URI of an image to display in the absence 
	 * of an image for playlist's tracks
	 * @param aImage The default image Url
	 */
	void setImage( const QUrl& aImage );
	
	/**
	 * Method to set the creation date of the playlist
	 * @param aDate The date and time of creation of the playlist
	 */
	void setCreationDate( const QDateTime& aDate );
	
	/**
	 * Method to set the URI of a resource that describes the license 
	 * under which this playlist was released
	 * @param aLicense The license Url
	 */
	void setLicense( const QUrl& aLicense );
	
	/**
	 * Method to set the ordered list of URIs. The purpose is to satisfy 
	 * licenses allowing modification but requiring attribution
	 * @param aAttribution The list of license Urls
	 */
	void setAttribution( const QList<QUrl>& aAttribution );
	
	/**
	 * Method to set the meta element that allows metadata fields to be added to XSPF
	 * @param aMetaData The meta elements as a QVarintMap
	 */
	void setMetadata( const QVariantMap& aMetaData );
	
	/**
	 * Method to set the extension element that allows non-XSPF XML 
	 * to be included in XSPF documents
	 * @param aExtension The extension elements as a QVarintMap
	 */
	void setExtension( const QVariantMap& aExtension );
	
	/**
	 * Method to set the list of tracks in the playlist
	 * @param aList The new list of tracks in the playlist
	 */
	void setTrackList( const QList<SmfTrackInfo> &aList );
	
private:
	QSharedDataPointer<SmfPlaylistPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfPlaylist &aPlaylist );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfPlaylist &aPlaylist );
	
	};


/**
 * Method for Externalization. Writes the SmfPlaylist object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPlaylist The SmfPlaylist object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfPlaylist &aPlaylist );

/**
 * Method for Internalization. Reads a SmfPlaylist object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPlaylist The SmfPlaylist object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfPlaylist &aPlaylist);

typedef QList<SmfPlaylist> SmfPlaylistList;

// Make the class SmfPlaylist known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfPlaylist)
Q_DECLARE_METATYPE(QList<SmfPlaylist>)

#endif /* SMFPLAYLIST_H_ */

