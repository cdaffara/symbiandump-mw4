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
* The post class represents an instance of a post to a SN site
*
*/

#include "smfpost.h"
#include "smfpost_p.h"

/**
 * Constructor with default argument
 * @param text Text
 */
SmfPost::SmfPost( )
	{
	d = new SmfPostPrivate();
	}

/**
 * Constructor
 * @param aText The post's text
 * @param aImage The post's image
 * @param aUrl The post's url
 */
SmfPost::SmfPost( SmfContact aOwner,QString aTitle, QString aDesc, QImage aImage, QUrl aUrl )
	{
	d = new SmfPostPrivate(aOwner,aTitle, aDesc, aImage, aUrl);
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfPost::SmfPost( const SmfPost &aOther )
: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfPost& SmfPost::operator=( const SmfPost &aOther )
	{
	d->m_title = aOther.d->m_title;
	d->m_desc = aOther.d->m_desc;
	d->m_image = aOther.d->m_image;
	d->m_url = aOther.d->m_url;
	d->m_postId = aOther.d->m_postId;
	return *this;
	}

/**
 * Destructor
 */
SmfPost::~SmfPost( )
	{
	}

/**
 * Returns the owner(who posted this message)
 */
SmfContact SmfPost::owner() const
	{
	return d->m_owner;
	}

/**
 * Method to get the title of the post
 * @return The post's title
 */
QString SmfPost::title( ) const
	{
	return d->m_title;
	}

/**
 * Method to get the description of the post
 * @return The post's description
 */
QString SmfPost::description( ) const
	{
	return d->m_desc;
	}

/**
 * Method to get the image of the post
 * @return The post's image
 */
QImage SmfPost::image( ) const
	{
	return d->m_image;
	}

/**
 * Method to get the url of the post
 * @return The post's url
 */
QUrl SmfPost::url( ) const
	{
	return d->m_url;
	}

/**
 * Method to get the posted date and time of the post
 * @return The post's date and time of posting
 */
QDateTime SmfPost::postedDateTime() const
	{
	return d->m_date;
	}

/**
 * Method to get the id of the post
 * @return The ID value 
 */
QString SmfPost::id( ) const
	{
	return d->m_postId;
	}

/**
 * sets the owner of the post
 */
void SmfPost::setOwner(const SmfContact& aOwner)
	{
	d->m_owner = aOwner;
	}

/**
 * Method to set the title of the post
 * @param aTitle The post's new title
 */
void SmfPost::setTitle( const QString &aTitle )
	{
	d->m_title = aTitle;
	}

/**
 * Method to set the description of the post
 * @param aDesc The post's new description
 */
void SmfPost::setDescription( const QString &aDesc )
	{
	d->m_desc = aDesc;
	}

/**
 * Method to set the image of the post
 * @param aPic The post's image
 */
void SmfPost::setImage( const QImage& aPic )
	{
	d->m_image = aPic;
	}

/**
 * Method to set the url of the post
 * @param aUrl The post's url
 */
void SmfPost::setUrl( QUrl& aUrl )
	{
	d->m_url = aUrl;
	}

/**
 * Method to set the posted date and time of the post
 * @param aDate The post's date and time of posting
 */
void SmfPost::setPostedDateTime( QDateTime &aDate )
	{
	d->m_date = aDate;
	}

/**
 * Method to get the id of the post
 */
void SmfPost::setId(QString aPostId)
	{
	d->m_postId = aPostId;
	}

/**
 * Method for Externalization. Writes the SmfPost object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPost The SmfPost object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfPost &aPost )
	{
	aDataStream<<aPost.owner();
	aDataStream<<aPost.title();
	aDataStream<<aPost.description();
	aDataStream<<aPost.image();
	aDataStream<<aPost.url();
	aDataStream<<aPost.id();
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfPost object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPost The SmfPost object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfPost &aPost)
	{
	SmfContact owner;
	aDataStream>>owner;
	aPost.setOwner(owner);
	QString title;
	aDataStream>>title;
	aPost.setTitle(title);
	
	QString desc;
	aDataStream>>desc;
	aPost.setDescription(desc);

	QImage img;
	aDataStream>>img;
	aPost.setImage(img);

	QUrl url;
	aDataStream>>url;
	aPost.setUrl(url);

	QString id;
	aDataStream>>id;
	aPost.setId(id);

	return aDataStream;
	}
