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
 * The SmfEvent class represents an event
 *
 */

#ifndef SMFEVENT_H_
#define SMFEVENT_H_

#include "smflocation.h"
#include "smfartists.h"
#include <QStringList>
#include <QDatastream>
#include <QSharedData>
#include "smfclientglobal.h"
#include <QMetaType>

class SmfEventPrivate;

/**
 * @ingroup smf_common_group
 * The event class represents an event
 */
class SMFCOMMON_EXPORT SmfEvent
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfEvent( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfEvent( const SmfEvent &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The current object reference
	 */
	SmfEvent& operator=( const SmfEvent &aOther );
	
	/**
	 * Destructor
	 */
	~SmfEvent( );
	
	/**
	 * Method to get the event name
	 * @return The event name
	 */
	QString title( ) const;
	
	/**
	 * Method to get the event date and time
	 * @return The date and time of the event
	 */
	QDateTime eventDateTime( ) const;
	
	/**
	 * Method to get the event duration
	 * @return The duration of the event
	 */
	QTime duration( ) const;
	
	/**
	 * Method to get the artist names
	 * @return The list of artists in the event
	 */
	SmfArtists artists( ) const;
	
	/**
	 * Method to get the venue of the event
	 * @return The venue of the event
	 */
	SmfLocation venue( ) const;
	
	/**
	 * Method to get the URL for getting tickets for the event
	 * @return The Url for getting ticket for the event
	 */
	QUrl ticketUrl( ) const;
	
	/**
	 * Method to get the id of the event
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Method to set the event name
	 * @param aName The new event name
	 */
	void setTitle( const QString &aName );
	
	/**
	 * Method to set the event date and time
	 * @param aDateTime The new date and time of the event
	 * 
	 */
	void setEventDateTime( const QDateTime &aDateTime );
	
	/**
	 * Method to set the event duration
	 * @param aDuration The new duration of the event
	 * 
	 */
	void setDuration( const QTime &aDuration );
	
	/**
	 * Method to set the artist
	 * @param aArtists The new artists in the event
	 */
	void setArtists( const SmfArtists &aArtists );
	
	/**
	 * Method to set the venue name
	 * @param aVenue The new venue of the event
	 */
	void setVenue( const SmfLocation &aVenue );
	
	/**
	 * Method to set the URL for getting tickets for the event
	 * @param aUrl The new Url for getting ticket for the event
	 */
	void setTicketUrl( const QUrl &aUrl );
	
	/**
	 * Method to set the id of the event
	 * @param aId The ID value 
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfEventPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfEvent &aEvent );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfEvent &aEvent );
	
	};


/**
 * Method for Externalization. Writes the SmfEvent object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aEvent The SmfEvent object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfEvent &aEvent );

/**
 * Method for Internalization. Reads a SmfEvent object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aEvent The SmfEvent object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfEvent &aEvent);


typedef QList<SmfEvent> SmfEventList;

// Make the class SmfEvent known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfEvent)
Q_DECLARE_METATYPE(QList<SmfEvent>)


#endif /* SMFEVENT_H_ */
