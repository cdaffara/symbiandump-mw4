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

#ifndef SMFCOMMENT_H_
#define SMFCOMMENT_H_

#include <QDateTime>
#include <qdatastream.h>
#include <QSharedData>
#include "smfclientglobal.h"
#include <QMetaType>

class SmfCommentPrivate;

/**
 * @ingroup smf_common_group
 * The comment class represents a comment (on a picture or a music track etc)
 */
class SMFCOMMON_EXPORT SmfComment
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfComment( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfComment( const SmfComment &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The current object reference
	 */
	SmfComment& operator=(const SmfComment &aOther);
	
	/**
	 * Destructor
	 */
	~SmfComment( );
	
	/**
	 * Method to get the comment text
	 * @return The comment text
	 */
	QString text( ) const;
	
	/**
	 * Method to get the comment time stamp
	 * @return The comment time stamp value
	 */
	QDateTime timeStamp( ) const;
	
	/**
	 * Method to get the id of the comment
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Method to set the comment text
	 * @param aText The comment text to be set
	 */
	void setText( const QString &aText );
	
	/**
	 * Method to set the time stamp
	 * @param aDateTime The comment time stamp value to be set
	 */
	void setTimeStamp( const QDateTime &aDateTime );
	
	/**
	 * Method to set the id of the comment
	 * @param aId The ID value to be set
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfCommentPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfComment &aComment );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfComment &aComment );
	
	};


/**
 * Method for Externalization. Writes the SmfComment object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aComment The SmfComment object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfComment &aComment );

/**
 * Method for Internalization. Reads a SmfComment object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aComment The SmfComment object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfComment &aComment);


// Make the class SmfComment known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfComment)
Q_DECLARE_METATYPE(QList<SmfComment>)

#endif /* SMFCOMMENT_H_ */
