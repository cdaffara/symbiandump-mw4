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
 * The comment class represents a comment (on a picture or a music track etc)
 *
 */


#include "smfcomment.h"
#include "smfcomment_p.h"

/**
 * Constructor with default argument
 */
SmfComment::SmfComment()
	{
	d = new SmfCommentPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfComment::SmfComment( const SmfComment &aOther )
	:d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The current object reference
 */
SmfComment& SmfComment::operator=(const SmfComment &aOther)
	{
	d->m_text = aOther.d->m_text;
	d->m_timeStamp = aOther.d->m_timeStamp;
	d->m_commentId = aOther.d->m_commentId;
	return *this;
	}

/**
 * Destructor
 */
SmfComment::~SmfComment( )
	{
	}

/**
 * Method to get the comment text
 * @return The comment text
 */
QString SmfComment::text( ) const
	{
	return d->m_text;
	}

/**
 * Method to get the comment time stamp
 * @return The comment time stamp value
 */
QDateTime SmfComment::timeStamp( ) const
	{
	return d->m_timeStamp;
	}

/**
 * Method to get the id of the comment
 * @return The ID value 
 */
QString SmfComment::id( ) const	
	{
	return d->m_commentId;
	}

/**
 * Method to set the comment text
 * @param aText The comment text to be set
 */
void SmfComment::setText( const QString &aText )
	{
	d->m_text = aText;
	}

/**
 * Method to set the time stamp
 * @param aDateTime The comment time stamp value to be set
 */
void SmfComment::setTimeStamp( const QDateTime &aDateTime )
	{
	d->m_timeStamp = aDateTime;
	}

/**
 * Method to set the id of the comment
 * @param aId The ID value to be set
 */
void SmfComment::setId( const QString &aId )
	{
	d->m_commentId = aId;
	}


/**
 * Method for Externalization. Writes the SmfComment object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aComment The SmfComment object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfComment &aComment )
	{
	// Serialize d->m_text
	aDataStream<<aComment.d->m_text;
	
	// Serialize d->m_timeStamp
	aDataStream<<aComment.d->m_timeStamp;
	
	// Serialize d->m_commentId
	aDataStream<<aComment.d->m_commentId;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfComment object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aComment The SmfComment object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfComment &aComment)
	{
	// Deserialize d->m_text
	aDataStream>>aComment.d->m_text;
	
	// Deserialize d->m_timeStamp
	aDataStream>>aComment.d->m_timeStamp;
	
	// Deserialize d->m_commentId
	aDataStream>>aComment.d->m_commentId;
	
	return aDataStream;
	}

