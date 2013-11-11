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
 * The picture class represents an instance of a picture
 *
 */

#include "smfpicture.h"
#include "smfpicture_p.h"

/**
 * Constructor with default argument
 */
SmfPicture::SmfPicture(/*bool aDownloaded*/)
	{
	d = new SmfPicturePrivate;
	//d->isDownloaded = aDownloaded;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfPicture::SmfPicture( const SmfPicture &aOther )
	: d( aOther.d )
	{
	}

/**
 * CConstructs SmfPicture from QImage
 * @param aOther The QImage
 */
SmfPicture::SmfPicture( const QImage &aImage )
	{
	d = new SmfPicturePrivate(aImage);
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfPicture& SmfPicture::operator=( const SmfPicture &aOther )
	{
	d->m_photoId = aOther.d->m_photoId;
	d->m_owner = aOther.d->m_owner;
	d->m_title = aOther.d->m_title;
	d->m_description = aOther.d->m_description;
	d->m_picVisibility = aOther.d->m_picVisibility;
	d->m_postedOn = aOther.d->m_postedOn;
	d->m_comments = aOther.d->m_comments;
	d->m_tags = aOther.d->m_tags;
	d->m_url = aOther.d->m_url;
	d->m_picture = aOther.d->m_picture;
	return *this;
	}

/**
 * Destructor
 */
SmfPicture::~SmfPicture( )
	{
	}

/**
 * Method to get a picture owner
 * @return The owner of the picture
 */
QString SmfPicture::owner( ) const
	{
	return d->m_owner;
	}

/**
 * Method to get a picture title
 * @return The title of the picture
 */
QString SmfPicture::title( ) const
	{
	return d->m_title;
	}

/**
 * Method to get a picture description
 * @return The description of the picture
 */
QString SmfPicture::description( ) const
	{
	return d->m_description;
	}

/**
 * Method to get a visibility of a picture for public
 * @return The visibility mode of this picture for others
 */
SmfPictureVisibility SmfPicture::visibility( ) const
	{
	return d->m_picVisibility;
	}

/**
 * Method to get the date of posting the picture
 * @return The posted date of the picture
 */
QDateTime SmfPicture::postedDate( ) const
	{
	return d->m_postedOn;
	}

/**
 * Method to get the comments for the picture
 * @return The comments for the picture
 */
QList<SmfComment> SmfPicture::comments( ) const
	{
	return d->m_comments;
	}

/**
 * Method to get the tags for the picture
 * @return The tags for the picture
 */
QStringList SmfPicture::tags( ) const
	{
	return d->m_tags;
	}

/**
 * Method to get the url of the picture
 * @return The url of the picture
 */
QUrl SmfPicture::url( ) const
	{
	return d->m_url;
	}

/**
 * Method to get the picture data as QImage
 * @return The picture as QImage
 */
QImage SmfPicture::picture( ) const
	{
	return d->m_picture;
	}
	
/**
 * Method to get the id of the picture
 * @return The ID value 
 */
QString SmfPicture::id( ) const
	{
	return d->m_photoId;
	}
bool SmfPicture::downloadFlag()
	{
	return d->isDownloaded;
	}
/**
 * Method to set a picture owner
 * @param aOwner The owner of the picture
 */
void SmfPicture::setOwner( const QString &aOwner )
	{
	d->m_owner = aOwner;
	}

/**
 * Method to set a picture title
 * @param aTitle The title of the picture
 */
void SmfPicture::setTitle( const QString &aTitle )
	{
	d->m_title = aTitle;
	}

/**
 * Method to set a picture description
 * @param aDescription The description of the picture
 */
void SmfPicture::setDescription( const QString &aDescription )
	{
	d->m_description = aDescription;
	}

/**
 * Method to set a visibility of a picture for public
 * @param aVisibility aVisibility The visibility mode of 
 * this picture for others
 */
void SmfPicture::setVisibility( const SmfPictureVisibility &aVisibility )
	{
	d->m_picVisibility = aVisibility;
	}

/**
 * Method to set the date of posting the picture
 * @param aDate The post date of the picture
 */
void SmfPicture::setPostedDate( const QDateTime &aDate )
	{
	d->m_postedOn = aDate;
	}

/**
 * Method to add comment on the picture
 * @param aComment The comment for the picture
 */
void SmfPicture::addComment( const SmfComment &aComment )
	{
	d->m_comments.append(aComment);
	}

/**
 * Method to add tags for the picture
 * @param aTag The tag for the picture
 */
void SmfPicture::addTags( const QStringList &aTags )
	{
	d->m_tags = aTags;
	}

/**
 * Method to set the url of the picture
 * @param aUrl The url of the picture
 */
void SmfPicture::setUrl( const QUrl &aUrl )
	{
	d->m_url = aUrl;
	}

/**
 * Method to set the picture data as QImage
 * @param aData The picture as QImage
 */
void SmfPicture::setPicture( const QImage &aData )
	{
	d->m_picture = aData;
	}

/**
 * Method to set the id of the picture
 * @param aId The ID value 
 */
void SmfPicture::setId( const QString &aId )
	{
	d->m_photoId = aId;
	}


/**
 * Method for Externalization. Writes the SmfPicture object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPic The SmfPicture object to be externalized
 * @return reference to the written stream
 */
QDataStream& operator<<( QDataStream &aDataStream, 
		const SmfPicture &aPic )
	{
	// Serialize d->m_photoId
	aDataStream<<aPic.d->m_photoId;
	
	// Serialize d->m_owner
	aDataStream<<aPic.d->m_owner;
	
	// Serialize d->m_title
	aDataStream<<aPic.d->m_title;
	
	// Serialize d->m_description
	aDataStream<<aPic.d->m_description;
	
	// Serialize d->m_picVisibility
	aDataStream<<aPic.d->m_picVisibility;
	
	// Serialize d->m_postedOn
	aDataStream<<aPic.d->m_postedOn;
	
	// Serialize d->m_comments
	aDataStream<<aPic.d->m_comments;
	
	// Serialize d->m_tags
	aDataStream<<aPic.d->m_tags;
	
	// Serialize d->m_url
	aDataStream<<aPic.d->m_url;
	
	// Serialize d->m_picture
	aDataStream<<aPic.d->m_picture;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfPicture object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPic The SmfPicture object to be internalized
 * @return reference to the stream
 */
QDataStream& operator>>( QDataStream &aDataStream, 
		SmfPicture &aPic)
	{
	// Deserialize d->m_photoId
	aDataStream>>aPic.d->m_photoId;
	
	// Deserialize d->m_owner
	aDataStream>>aPic.d->m_owner;
	
	// Deserialize d->m_title
	aDataStream>>aPic.d->m_title;
	
	// Deserialize d->m_description
	aDataStream>>aPic.d->m_description;
	
	// Deserialize d->m_picVisibility
	quint32 val = 0;
	aDataStream>>val;
	aPic.d->m_picVisibility = (SmfPictureVisibility)val;

	// Deserialize d->m_postedOn
	aDataStream>>aPic.d->m_postedOn;
	
	// Deserialize d->m_comments
	aDataStream>>aPic.d->m_comments;
	
	// Deserialize d->m_tags
	aDataStream>>aPic.d->m_tags;
	
	// Deserialize d->m_url
	aDataStream>>aPic.d->m_url;
	
	// Deserialize d->m_picture
	aDataStream>>aPic.d->m_picture;
	
	return aDataStream;
	}
