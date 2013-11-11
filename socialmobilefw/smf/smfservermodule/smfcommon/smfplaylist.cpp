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

#include <smfplaylist.h>
#include <smfplaylist_p.h>

/**
 * Constructor with default argument
 */
SmfPlaylist::SmfPlaylist( )
	{
	d = new SmfPlaylistPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfPlaylist::SmfPlaylist( const SmfPlaylist &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfPlaylist& SmfPlaylist::operator=( const SmfPlaylist &aOther )
	{
	d->m_version = aOther.d->m_version;
	d->m_title = aOther.d->m_title;
	d->m_author = aOther.d->m_author;
	d->m_comments = aOther.d->m_comments;
	d->m_info = aOther.d->m_info;
	d->m_location = aOther.d->m_location;
	d->m_playlistId = aOther.d->m_playlistId;
	d->m_image = aOther.d->m_image;
	d->m_creationDate = aOther.d->m_creationDate;
	d->m_license = aOther.d->m_license;
	d->m_attribution = aOther.d->m_attribution;
	d->m_metadata = aOther.d->m_metadata;
	d->m_extension = aOther.d->m_extension;
	d->m_trackList = aOther.d->m_trackList;
	return *this;
	}

/**
 * Destructor
 */
SmfPlaylist::~SmfPlaylist( )
	{
	}

/**
 * Method to get the version of the playlist
 * @return The version of the playlist
 */
QString SmfPlaylist::version( ) const
	{
	return d->m_version;
	}

/**
 * Method to get the playlist title
 * @return The title of the playlist
 */
QString SmfPlaylist::playListTitle( ) const
	{
	return d->m_title;
	}

/**
 * Method to get the author of the playlist
 * @return The author of the playlist
 */
QString SmfPlaylist::author( ) const
	{
	return d->m_author;
	}

/**
 * Method to get the comments on the playlist
 * @return The comments on the playlist
 */
QList<SmfComment> SmfPlaylist::comments( ) const
	{
	return d->m_comments;
	}

/**
 * Method to get the URI of a web page to find out more about this playlist
 * @return The info Url
 */
QUrl SmfPlaylist::info( ) const
	{
	return d->m_info;
	}
		
/**
 * Method to get the Source URI for this playlist
 * @return The Source URI for this playlist
 */
QUrl SmfPlaylist::location( ) const
	{
	return d->m_location;
	}

/**
 * Method to get the id of the playlist
 * @return The ID value 
 */
QString SmfPlaylist::id( ) const
	{
	return d->m_playlistId;
	}

/**
 * Method to get the URI of an image to display in the absence 
 * of an image for playlist's tracks
 * @return The deafult image Url
 */
QUrl SmfPlaylist::image( ) const
	{
	return d->m_image;
	}

/**
 * Method to get the creation date of the playlist
 * @return The date and time of creation of the playlist
 */
QDateTime SmfPlaylist::creationDate( ) const
	{
	return d->m_creationDate;
	}

/**
 * Method to get the URI of a resource that describes the license 
 * under which this playlist was released
 * @return The license Url
 */
QUrl SmfPlaylist::license( ) const
	{
	return d->m_license;
	}

/**
 * Method to get the ordered list of URIs. The purpose is to satisfy 
 * licenses allowing modification but requiring attribution
 * @return The list of license Urls
 */
QList<QUrl> SmfPlaylist::attribution( ) const
	{
	return d->m_attribution;
	}

/**
 * Method to get the meta element that allows metadata fields to be added to XSPF
 * @return The meta elements as a QVarintMap
 */
QVariantMap SmfPlaylist::metadata( ) const
	{
	return d->m_metadata;
	}

/**
 * Method to get the extension element that allows non-XSPF XML 
 * to be included in XSPF documents
 * @return The extension elements as a QVarintMap
 */
QVariantMap SmfPlaylist::extension( ) const
	{
	return d->m_extension;
	}

/**
 * Method to get the list of tracks in the playlist
 * @return The list of tracks in the playlist
 */
QList<SmfTrackInfo> SmfPlaylist::trackList( ) const
	{
	return d->m_trackList;
	}

/**
 * Method to set the version of the playlist
 * @param aVersion The version of the playlist
 */
void SmfPlaylist::setVersion( const QString& aVersion )
	{
	d->m_version = aVersion;
	}

/**
 * Method to set the playlist title
 * @param aTitle The title of the playlist
 */
void SmfPlaylist::setPlayListTitle( const QString &aTitle )
	{
	d->m_title = aTitle;
	}

/**
 * Method to set the author of the playlist
 * @param aAuthor The author of the playlist
 */
void SmfPlaylist::setAuthor( const QString& aAuthor )
	{
	d->m_author = aAuthor;
	}

/**
 * Method to set the comments on the playlist
 * @param aComments The comments on the playlist
 */
void SmfPlaylist::setComments( const QList<SmfComment>& aComments )
	{
	d->m_comments = aComments;
	}

/**
 * Method to set the URI of a web page to find out more about this playlist
 * @param aInfoUrl The info Url
 */
void SmfPlaylist::setInfo( const QUrl& aInfoUrl )
	{
	d->m_info = aInfoUrl;
	}

/**
 * Method to set the Source URI for this playlist
 * @param aLocation The Source URI for this playlist
 */
void SmfPlaylist::setLocation( const QUrl& aLocation )
	{
	d->m_location = aLocation;
	}

/**
 * Method to set the id of the playlist
 * @param aId The ID value 
 */
void SmfPlaylist::setId( const QString &aId)
	{
	d->m_playlistId = aId;
	}

/**
 * Method to set the URI of an image to display in the absence 
 * of an image for playlist's tracks
 * @param aImage The default image Url
 */
void SmfPlaylist::setImage( const QUrl& aImage )
	{
	d->m_image = aImage;
	}

/**
 * Method to set the creation date of the playlist
 * @param aDate The date and time of creation of the playlist
 */
void SmfPlaylist::setCreationDate( const QDateTime& aDate )
	{
	d->m_creationDate = aDate;
	}

/**
 * Method to set the URI of a resource that describes the license 
 * under which this playlist was released
 * @param aLicense The license Url
 */
void SmfPlaylist::setLicense( const QUrl& aLicense )
	{
	d->m_license = aLicense;
	}

/**
 * Method to set the ordered list of URIs. The purpose is to satisfy 
 * licenses allowing modification but requiring attribution
 * @param aAttribution The list of license Urls
 */
void SmfPlaylist::setAttribution( const QList<QUrl>& aAttribution )
	{
	d->m_attribution = aAttribution;
	}

/**
 * Method to set the meta element that allows metadata fields to be added to XSPF
 * @param aMetaData The meta elements as a QVarintMap
 */
void SmfPlaylist::setMetadata( const QVariantMap& aMetaData )
	{
	d->m_metadata = aMetaData;
	}

/**
 * Method to set the extension element that allows non-XSPF XML 
 * to be included in XSPF documents
 * @param aExtension The extension elements as a QVarintMap
 */
void SmfPlaylist::setExtension( const QVariantMap& aExtension )
	{
	d->m_extension = aExtension;
	}

/**
 * Method to set the list of tracks in the playlist
 * @param aList The new list of tracks in the playlist
 */
void SmfPlaylist::setTrackList( const QList<SmfTrackInfo> &aList )
	{
	d->m_trackList = aList;
	}

/**
 * Method for Externalization. Writes the SmfPlaylist object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPlaylist The SmfPlaylist object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfPlaylist &aPlaylist )
	{
	// Serialize d->m_version
	aDataStream<<aPlaylist.d->m_version;
	
	// Serialize d->m_title
	aDataStream<<aPlaylist.d->m_title;
	
	// Serialize d->m_author
	aDataStream<<aPlaylist.d->m_author;
	
	// Serialize d->m_comments
	aDataStream<<aPlaylist.d->m_comments;
	
	// Serialize d->m_info
	aDataStream<<aPlaylist.d->m_info;
	
	// Serialize d->m_location
	aDataStream<<aPlaylist.d->m_location;
	
	// Serialize d->m_playlistId
	aDataStream<<aPlaylist.d->m_playlistId;
	
	// Serialize d->m_image
	aDataStream<<aPlaylist.d->m_image;
	
	// Serialize d->m_creationDate
	aDataStream<<aPlaylist.d->m_creationDate;
	
	// Serialize d->m_license
	aDataStream<<aPlaylist.d->m_license;
	
	// Serialize d->m_attribution
	aDataStream<<aPlaylist.d->m_attribution;
	
	// Serialize d->m_metadata
	aDataStream<<aPlaylist.d->m_metadata;
	
	// Serialize d->m_extension
	aDataStream<<aPlaylist.d->m_extension;
	
	// Serialize d->m_trackList
	aDataStream<<aPlaylist.d->m_trackList;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfPlaylist object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPlaylist The SmfPlaylist object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfPlaylist &aPlaylist)
	{
	// Deserialize d->m_version
	aDataStream>>aPlaylist.d->m_version;
	
	// Deserialize d->m_title
	aDataStream>>aPlaylist.d->m_title;
	
	// Deserialize d->m_author
	aDataStream>>aPlaylist.d->m_author;
	
	// Deserialize d->m_comments
	aDataStream>>aPlaylist.d->m_comments;
	
	// Deserialize d->m_info
	aDataStream>>aPlaylist.d->m_info;
	
	// Deserialize d->m_location
	aDataStream>>aPlaylist.d->m_location;
	
	// Deserialize d->m_playlistId
	aDataStream>>aPlaylist.d->m_playlistId;
	
	// Deserialize d->m_image
	aDataStream>>aPlaylist.d->m_image;
	
	// Deserialize d->m_creationDate
	aDataStream>>aPlaylist.d->m_creationDate;
	
	// Deserialize d->m_license
	aDataStream>>aPlaylist.d->m_license;
	
	// Deserialize d->m_attribution
	aDataStream>>aPlaylist.d->m_attribution;
	
	// Deserialize d->m_metadata
	aDataStream>>aPlaylist.d->m_metadata;
	
	// Deserialize d->m_extension
	aDataStream>>aPlaylist.d->m_extension;
	
	// Deserialize d->m_trackList
	aDataStream>>aPlaylist.d->m_trackList;
	
	return aDataStream;
	}
