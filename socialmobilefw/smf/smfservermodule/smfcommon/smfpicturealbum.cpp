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
 * The album to contain a set pf pictures
 *
 */

#include <smfpicturealbum.h>
#include <smfpicturealbum_p.h>

/**
 * Constructor with default argument
 */
SmfPictureAlbum::SmfPictureAlbum( )
	{
	d = new SmfPictureAlbumPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfPictureAlbum::SmfPictureAlbum( const SmfPictureAlbum &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfPictureAlbum& SmfPictureAlbum::operator=( const SmfPictureAlbum &aOther )
	{
	d->m_albumId = aOther.d->m_albumId;
	d->m_title = aOther.d->m_title;
	d->m_description = aOther.d->m_description;
	d->m_albumVisibility = aOther.d->m_albumVisibility;
	d->m_postedOn = aOther.d->m_postedOn;
	d->m_comments = aOther.d->m_comments;
	d->m_keywords = aOther.d->m_keywords;
	d->m_url = aOther.d->m_url;
	d->m_picCount =aOther.d->m_picCount;
	d->m_thumbnail = aOther.d->m_thumbnail ;	
	return *this;
	}

/**
 * Destructor
 */
SmfPictureAlbum::~SmfPictureAlbum( )
	{
	
	}


/**
 * Method to get a album title
 * @return The title of the album
 */
QString SmfPictureAlbum::title( ) const
	{
	return d->m_title;
	}

/**
 * Method to get a album description
 * @return The description of the album
 */
QString SmfPictureAlbum::description( ) const
	{
	return d->m_description;
	}

/**
 * Method to get a visibility of a album for public
 * @return The visibility mode of this album for others
 */
SmfPictureVisibility SmfPictureAlbum::visibility( ) const
	{
	return d->m_albumVisibility;
	}

/**
 * Method to get the date of posting the album
 * @return The posted date of the album
 */
QDateTime SmfPictureAlbum::postedDate( ) const
	{
	return d->m_postedOn;
	}

/**
 * Method to get the comments for the album
 * @return The comments for the album
 */
QList<SmfComment> SmfPictureAlbum::comments( ) const
	{
	return d->m_comments;
	}

/**
 * Method to get the tags for the album
 * @return The tags for the album
 */
QStringList SmfPictureAlbum::keywords( ) const
	{
	return d->m_keywords;
	}

/**
 * Method to get the url of the album
 * @return The url of the album
 */
QUrl SmfPictureAlbum::url( ) const
	{
	return d->m_url;
	}

/**
 * Method to get the thumbnail for this album as QImage
 * @return The picture as QImage
 */
QImage SmfPictureAlbum::thumbnail( ) const
	{
	return d->m_thumbnail;
	}

/**
 * Method to get the album data as QImage
 * @return The album as QImage
 */
qint32 SmfPictureAlbum::pictureCount( ) const
	{
	return d->m_picCount;
	}
	
/**
 * Method to get the id of the album
 * @return The ID value 
 */
QString SmfPictureAlbum::id( ) const
	{
	return d->m_albumId;
	}

/**
 * Method to set a album title
 * @param aTitle The title of the album
 */
void SmfPictureAlbum::setTitle( const QString &aTitle )
	{
	d->m_title = aTitle;
	}

/**
 * Method to set a album description
 * @param aDescription The description of the album
 */
void SmfPictureAlbum::setDescription( const QString &aDescription )
	{
	d->m_description = aDescription;
	}

/**
 * Method to set a visibility of a album for public
 * @param aVisibility aVisibility The visibility mode of 
 * this album for others
 */
void SmfPictureAlbum::setVisibility( const SmfPictureVisibility &aVisibility )
	{
	d->m_albumVisibility = aVisibility;
	}

/**
 * Method to set the date of posting the album
 * @param aDate The post date of the album
 */
void SmfPictureAlbum::setPostedDate( const QDateTime &aDate )
	{
	d->m_postedOn = aDate;
	}

/**
 * Method to add comment on the album
 * @param aComment The comment for the album
 */
void SmfPictureAlbum::addComment( const SmfComment &aComment )
	{
	d->m_comments.append(aComment);
	}

/**
 * Method to add tags for the album
 * @param aTag The tag for the album
 */
void SmfPictureAlbum::addKeywords(const QStringList &aKeywords )
	{
	d->m_keywords = aKeywords;
	}

/**
 * Method to set the url of the album
 * @param aUrl The url of the album
 */
void SmfPictureAlbum::setUrl( const QUrl &aUrl )
	{
	d->m_url = aUrl;
	}

/**
 * Method to set the album data as QImage
 * @param aData The album as QImage
 */
void SmfPictureAlbum::setThumbnail( const QImage &aData )
	{
	d->m_thumbnail = aData;
	}

/**
 * Method to get the number of pictures in this album
 * @return number of pictures in this album
 */
 void SmfPictureAlbum::setPictureCount( const qint32 aCount)
	 {
	 d->m_picCount = aCount;
	 }

/**
 * Method to set the id of the album
 * @param aId The ID value 
 */
void SmfPictureAlbum::setId( const QString &aId )
	{
	d->m_albumId = aId;
	}


/**
 * Method for Externalization. Writes the SmfPictureAlbum object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPic The SmfPictureAlbum object to be externalized
 * @return reference to the written stream
 */
QDataStream& operator<<( QDataStream &aDataStream, 
		const SmfPictureAlbum &aAlbm )
	{
	//serialize d->m_albumId;
	aDataStream << aAlbm.d->m_albumId;

	//serialize d->m_title;
	aDataStream << aAlbm.d->m_title;

	//serialize d->m_description;
	aDataStream << aAlbm.d->m_description;

	//serialize d->m_albumVisibility;
	quint32 val = aAlbm.d->m_albumVisibility;
	aDataStream << val;

	//serialize d->m_postedOn;
	aDataStream << aAlbm.d->m_postedOn;

	//serialize d->m_comments;
	aDataStream << aAlbm.d->m_comments;

	//serialize d->m_keywords;
	aDataStream << aAlbm.d->m_keywords;

	//serialize d->m_url;
	aDataStream << aAlbm.d->m_url;

	//serialize d->m_picCount;
	aDataStream << aAlbm.d->m_picCount;

	//serialize d->m_thumbnail ;	
	aDataStream << aAlbm.d->m_thumbnail ;	
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfPictureAlbum object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPic The SmfPictureAlbum object to be internalized
 * @return reference to the stream
 */
QDataStream& operator>>( QDataStream &aDataStream, 
		SmfPictureAlbum &aAlbm)
	{
	//deserialize d->m_albumId;
	aDataStream >> aAlbm.d->m_albumId;

	//deserialize d->m_title;
	aDataStream >> aAlbm.d->m_title;

	//deserialize d->m_description;
	aDataStream >> aAlbm.d->m_description;

	//deserialize d->m_albumVisibility;
	quint32 val =  0;
	aDataStream >>val;
	aAlbm.d->m_albumVisibility = (SmfPictureVisibility)val;

	//deserialize d->m_postedOn;
	aDataStream >> aAlbm.d->m_postedOn;

	//deserialize d->m_comments;
	aDataStream >> aAlbm.d->m_comments;

	//deserialize d->m_keywords;
	aDataStream >> aAlbm.d->m_keywords;

	//deserialize d->m_url;
	aDataStream >> aAlbm.d->m_url;

	//deserialize d->m_picCount;
	aDataStream >> aAlbm.d->m_picCount;

	//deserialize d->m_thumbnail ;	
	aDataStream >> aAlbm.d->m_thumbnail ;	

	return aDataStream;
	}
