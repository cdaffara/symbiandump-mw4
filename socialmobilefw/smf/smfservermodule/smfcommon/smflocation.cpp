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
 * The SmfLocation class represents a location and its related information
 * Note: This class has dependencies on QtMobility project
 *
 */


#include <smflocation.h>
#include <smflocation_p.h>

/**
 * Constructor with default argument
 */
SmfLocation::SmfLocation( )
	{
	d = new SmfLocationPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfLocation::SmfLocation( const SmfLocation &aOther )
	:d( aOther.d )
	{
	}

/**
 * Overloaded = operator 
 * @param aOther The reference object
 */
SmfLocation& SmfLocation::operator=( const SmfLocation &aOther )
	{
	d->m_name = aOther.d->m_name;
	d->m_city = aOther.d->m_city;
	d->m_street = aOther.d->m_street;
	d->m_zipcode = aOther.d->m_zipcode;
	d->m_country = aOther.d->m_country;
	d->m_geo = aOther.d->m_geo;
	d->m_url = aOther.d->m_url;
	d->m_placeId = aOther.d->m_placeId;
	return *this;
	}

/**
 * Destructor
 */
SmfLocation::~SmfLocation( )
	{
	}

/**
 * Method to get the place name
 * @return The place name
 */
QString SmfLocation::name( ) const
	{
	return d->m_name;
	}

/**
 * Method to get the city of place
 * @return The city of the place
 */
QString SmfLocation::city( ) const
	{
	return d->m_city;
	}

/**
 * Method to get the street information of place
 * @return The street information of the place
 */
QString SmfLocation::street( ) const
	{
	return d->m_street;
	}

/**
 * Method to get the zip code of place
 * @return The zip code of place
 */
QString SmfLocation::zipCode( ) const
	{
	return d->m_zipcode;
	}

/**
 * Method to get the country of place
 * @return The country of place
 */
QString SmfLocation::country( ) const
	{
	return d->m_country;
	}

/**
 * Method to get the Geo Position information (like information gathered 
 * on a global position, direction and velocity at a particular point 
 * in time) of the place.
 * @return The Geo Position information of place
 */
QGeoPositionInfo SmfLocation::geoPositionInfo( ) const
	{
	return d->m_geo;
	}

/**
 * Method to get the url indicating the place
 * @return The url indicating the place
 */
QUrl SmfLocation::url( ) const
	{
	return d->m_url;
	}

/**
 * Method to get the id of the place
 * @return The ID value 
 */
QString SmfLocation::id( ) const
	{
	return d->m_placeId;
	}

/**
 * Method to set the place name
 * @param aPlace The new place name
 */
void SmfLocation::setName( const QString& aPlace )
	{
	d->m_name = aPlace;
	}

/**
 * Method to set the city of  place
 * @param aCity The new city of the place
 */
void SmfLocation::setCity( const QString& aCity )
	{
	d->m_city = aCity;
	}

/**
 * Method to set the street information of place
 * @param aStreet The new street information of the place
 */
void SmfLocation::setStreet( const QString& aStreet )
	{
	d->m_street = aStreet;
	}

/**
 * Method to set the zip code of place
 * @param aZipCode The new zip code of place
 */
void SmfLocation::setZipCode( const QString& aZipCode )
	{
	d->m_zipcode = aZipCode;
	}

/**
 * Method to set the country of place
 * @param aCountry The new country of place
 */
void SmfLocation::setCountry( const QString& aCountry )
	{
	d->m_country = aCountry;
	}

/**
 * Method to set the Geo Postion information (like information gathered 
 * on a global position, direction and velocity at a particular point 
 * in time) of the place.
 * @param aGeoPosInfo The new Geo Position information of place
 */
void SmfLocation::setGeoPositionInfo( const QGeoPositionInfo& aGeoPosInfo )
	{
	d->m_geo = aGeoPosInfo;
	}

/**
 * Method to set the url indicating the place
 * @param aUrl The new url indicating the place
 */
void SmfLocation::setUrl( const QUrl& aUrl )
	{
	d->m_url = aUrl;
	}

/**
 * Method to set the id of the place
 * @return The ID value 
 */
void SmfLocation::setId( const QString &aId )
	{
	d->m_placeId = aId;
	}


/**
 * Method for Externalization. Writes the SmfLocation object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPlace The SmfLocation object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfLocation &aPlace )
	{
	// Serialize d->m_name
	aDataStream<<aPlace.d->m_name;
	
	// Serialize d->m_city
	aDataStream<<aPlace.d->m_city;
	
	// Serialize d->m_street
	aDataStream<<aPlace.d->m_street;
	
	// Serialize d->m_zipcode
	aDataStream<<aPlace.d->m_zipcode;
	
	// Serialize d->m_country
	aDataStream<<aPlace.d->m_country;
	
	// Serialize d->m_geo
	aDataStream<<aPlace.d->m_geo;
	
	// Serialize d->m_url
	aDataStream<<aPlace.d->m_url;
	
	// Serialize d->m_placeId
	aDataStream<<aPlace.d->m_placeId;
		
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfLocation object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPlace The SmfLocation object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfLocation &aPlace)
	{
	// Deserialize d->m_name
	aDataStream>>aPlace.d->m_name;
	
	// Deserialize d->m_city
	aDataStream>>aPlace.d->m_city;
	
	// Deserialize d->m_street
	aDataStream>>aPlace.d->m_street;
	
	// Deserialize d->m_zipcode
	aDataStream>>aPlace.d->m_zipcode;
	
	// Deserialize d->m_country
	aDataStream>>aPlace.d->m_country;
	
	// Deserialize d->m_geo
	aDataStream>>aPlace.d->m_geo;
	
	// Deserialize d->m_url
	aDataStream>>aPlace.d->m_url;
	
	// Deserialize d->m_placeId
	aDataStream>>aPlace.d->m_placeId;
		
	return aDataStream;
	}
