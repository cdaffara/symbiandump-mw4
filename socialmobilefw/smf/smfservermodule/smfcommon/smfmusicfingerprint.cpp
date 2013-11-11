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
 * The SmfMusicFingerPrint class represents a music finger print used in searches
 *
 */

#include <smfmusicfingerprint.h>
#include <smfmusicfingerprint_p.h>

/**
 * Constructor with default argument
 */
SmfMusicFingerPrint::SmfMusicFingerPrint( )
	{
	d = new SmfMusicFingerPrintPrivate;
	}

/**
 * Constructor with default argument
 * @param aParent The parent object
 */
SmfMusicFingerPrint::SmfMusicFingerPrint( const SmfMusicFingerPrint &aOther )
	:d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfMusicFingerPrint& SmfMusicFingerPrint::operator=( const SmfMusicFingerPrint &aOther )
	{
	d->m_fingerPrintData = aOther.d->m_fingerPrintData;
	d->m_id = aOther.d->m_id;
	return *this;
	}

/**
 * Destructor
 */
SmfMusicFingerPrint::~SmfMusicFingerPrint( )
	{
	}

/**
 * GEt the music finger print data
 * @return The music finger print data
 */
QByteArray SmfMusicFingerPrint::musicFingerPrint ( ) const
	{
	return d->m_fingerPrintData;
	}

/**
 * Method to get the service specific id of this fingerprint data
 * @return The ID value 
 */
QString SmfMusicFingerPrint::id( ) const
	{
	return d->m_id;
	}	

/**
 * Method to set the music finger print data
 * @param aFp The music finger print data
 */
void SmfMusicFingerPrint::setMusicFingerPrint ( const QByteArray &aFp)
	{
	d->m_fingerPrintData = aFp;
	}

/**
 * Method to set the service specific id of this fingerprint data
 * @param aId The ID value 
 */
void SmfMusicFingerPrint::setId( const QString &aId )
	{
	d->m_id = aId;
	}


/**
 * Method for Externalization. Writes the SmfMusicFingerPrint object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aMFP The SmfMusicFingerPrint object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfMusicFingerPrint &aMFP )
	{
	// Serialize d->m_fingerPrintData
	aDataStream<<aMFP.d->m_fingerPrintData;
	
	// Serialize d->m_id
	aDataStream<<aMFP.d->m_id;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfMusicFingerPrint object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aMFP The SmfMusicFingerPrint object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfMusicFingerPrint &aMFP)
	{
	// Deserialize d->m_fingerPrintData
	aDataStream>>aMFP.d->m_fingerPrintData;
	
	// Deserialize d->m_id
	aDataStream>>aMFP.d->m_id;
	
	return aDataStream;
	}

