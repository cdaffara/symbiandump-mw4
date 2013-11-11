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

#include <smfevent.h>
#include <smfevent_p.h>

/**
 * Constructor with default argument
 */
SmfEvent::SmfEvent( )
	{
	d = new SmfEventPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfEvent::SmfEvent( const SmfEvent &aOther )
	:d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfEvent& SmfEvent::operator=( const SmfEvent &aOther )
	{
	d->m_name = aOther.d->m_name;
	d->m_dateTime = aOther.d->m_dateTime;
	d->m_duration = aOther.d->m_duration;
	d->m_artists = aOther.d->m_artists;
	d->m_venue = aOther.d->m_venue;
	d->m_url = aOther.d->m_url;
	d->m_eventId = aOther.d->m_eventId;
	return *this;
	}

/**
 * Destructor
 */
SmfEvent::~SmfEvent( )
	{
	}

/**
 * Method to get the event name
 * @return The event name
 */
QString SmfEvent::title( ) const
	{
	return d->m_name;
	}

/**
 * Method to get the event date and time
 * @return The date and time of the event
 */
QDateTime SmfEvent::eventDateTime( ) const
	{
	return d->m_dateTime;
	}

/**
 * Method to get the event duration
 * @return The duration of the event
 */
QTime SmfEvent::duration( ) const
	{
	return d->m_duration;
	}

/**
 * Method to get the artist names
 * @return The list of artists in the event
 */
SmfArtists SmfEvent::artists( ) const
	{
	return d->m_artists;
	}

/**
 * Method to get the venue of the event
 * @return The venue of the event
 */
SmfLocation SmfEvent::venue( ) const
	{
	return d->m_venue;
	}

/**
 * Method to get the URL for getting tickets for the event
 * @return The Url for getting ticket for the event
 */
QUrl SmfEvent::ticketUrl( ) const
	{
	return d->m_url;
	}

/**
 * Method to get the id of the event
 * @return The ID value 
 */
QString SmfEvent::id( ) const
	{
	return d->m_eventId;
	}

/**
 * Method to set the event name
 * @param aName The new event name
 */
void SmfEvent::setTitle( const QString &aName )
	{
	d->m_name = aName;
	}

/**
 * Method to set the event date and time
 * @param aDateTime The new date and time of the event
 * 
 */
void SmfEvent::setEventDateTime( const QDateTime &aDateTime )
	{
	d->m_dateTime = aDateTime;
	}

/**
 * Method to set the event duration
 * @param aDuration The new duration of the event
 * 
 */
void SmfEvent::setDuration( const QTime &aDuration )
	{
	d->m_duration = aDuration;
	}

/**
 * Method to set the artist
 * @param aArtists The new artists in the event
 */
void SmfEvent::setArtists( const SmfArtists &aArtists )
	{
	d->m_artists = aArtists;
	}

/**
 * Method to set the venue name
 * @param aVenue The new venue of the event
 */
void SmfEvent::setVenue( const SmfLocation &aVenue )
	{
	d->m_venue = aVenue;
	}

/**
 * Method to set the URL for getting tickets for the event
 * @param aUrl The new Url for getting ticket for the event
 */
void SmfEvent::setTicketUrl( const QUrl &aUrl )
	{
	d->m_url = aUrl;
	}

/**
 * Method to set the id of the event
 * @param aId The ID value 
 */
void SmfEvent::setId( const QString &aId )
	{
	d->m_eventId = aId;
	}


/**
 * Method for Externalization. Writes the SmfEvent object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aEvent The SmfEvent object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfEvent &aEvent )
	{
	// Serialize d->m_name
	aDataStream<<aEvent.d->m_name;
		
	// Serialize d->m_dateTime
	aDataStream<<aEvent.d->m_dateTime;
	
	// Serialize d->m_duration
	aDataStream<<aEvent.d->m_duration;
	
	// Serialize d->m_artists
	aDataStream<<aEvent.d->m_artists;
	
	// Serialize d->m_venue
	aDataStream<<aEvent.d->m_venue;
	
	// Serialize d->m_url
	aDataStream<<aEvent.d->m_url;
	
	// Serialize d->m_eventId
	aDataStream<<aEvent.d->m_eventId;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfEvent object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aEvent The SmfEvent object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfEvent &aEvent)
	{
	// Deserialize d->m_name
	aDataStream>>aEvent.d->m_name;
		
	// Deserialize d->m_dateTime
	aDataStream>>aEvent.d->m_dateTime;
	
	// Deserialize d->m_duration
	aDataStream>>aEvent.d->m_duration;
	
	// Deserialize d->m_artists
	aDataStream>>aEvent.d->m_artists;
	
	// Deserialize d->m_venue
	aDataStream>>aEvent.d->m_venue;
	
	// Deserialize d->m_url
	aDataStream>>aEvent.d->m_url;
	
	// Deserialize d->m_eventId
	aDataStream>>aEvent.d->m_eventId;
	
	return aDataStream;
	}
