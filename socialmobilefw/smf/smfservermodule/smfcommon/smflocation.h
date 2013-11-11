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
 * The SmfLocation class represents a place and its related information
 * Note: This class has dependencies on QtMobility project
 *
 */

#ifndef SMFLOCATION_H_
#define SMFLOCATION_H_

#include <QUrl>
#include <qgeopositioninfo.h> // Qt mobility class
#include <qdatastream.h>
#include <QSharedData>
#include <smfclientglobal.h>
#include <QMetaType>

// Qt mobility - namespace
using namespace QtMobility;

enum SmfLocationSearchBoundary 
{
	SearchByStreet = 0x00,
	SearchByLocality,
	SearchByPostOffice,
	SearchByTown,
	SearchByRegion,
	SearchByState,
	SearchByCountry
};

class SmfLocationPrivate;

/**
 * @ingroup smf_common_group
 * The SmfLocation class represents a location and its related information.
 * 
 * Note: This class has dependencies on QtMobility project
 */
class SMFCOMMON_EXPORT SmfLocation
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfLocation( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfLocation( const SmfLocation &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The current object reference
	 */
	SmfLocation& operator=( const SmfLocation &aOther );
	
	/**
	 * Destructor
	 */
	~SmfLocation( );
	
	/**
	 * Method to get the place name
	 * @return The place name
	 */
	QString name( ) const;
	
	/**
	 * Method to get the city of place
	 * @return The city of the place
	 */
	QString city( ) const;
	
	/**
	 * Method to get the street information of place
	 * @return The street information of the place
	 */
	QString street( ) const;
	
	/**
	 * Method to get the zip code of place
	 * @return The zip code of place
	 */
	QString zipCode( ) const;
	
	/**
	 * Method to get the country of place
	 * @return The country of place
	 */
	QString country( ) const;
	
	/**
	 * Method to get the Geo Position information (like information gathered 
	 * on a global position, direction and velocity at a particular point 
	 * in time) of the place.
	 * @return The Geo Position information of place
	 */
	QGeoPositionInfo geoPositionInfo( ) const;
	
	/**
	 * Method to get the url indicating the place
	 * @return The url indicating the place
	 */
	QUrl url( ) const;
	
	/**
	 * Method to get the id of the place
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Method to set the place name
	 * @param aPlace The new place name
	 */
	void setName( const QString &aPlace );
	
	/**
	 * Method to set the city of  place
	 * @param aCity The new city of the place
	 */
	void setCity( const QString &aCity );
	
	/**
	 * Method to set the street information of place
	 * @param aStreet The new street information of the place
	 */
	void setStreet( const QString &aStreet );
	
	/**
	 * Method to set the zip code of place
	 * @param aZipCode The new zip code of place
	 */
	void setZipCode( const QString &aZipCode );
	
	/**
	 * Method to set the country of place
	 * @param aCountry The new country of place
	 */
	void setCountry( const QString &aCountry );
	
	/**
	 * Method to set the Geo Postion information (like information gathered 
	 * on a global position, direction and velocity at a particular point 
	 * in time) of the place.
	 * @param aGeoPosInfo The new Geo Position information of place
	 */
	void setGeoPositionInfo( const QGeoPositionInfo &aGeoPosInfo );
	
	/**
	 * Method to set the url indicating the place
	 * @param aUrl The new url indicating the place
	 */
	void setUrl( const QUrl &aUrl );
	
	/**
	 * Method to set the id of the place
	 * @return The ID value 
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfLocationPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfLocation &aPlace );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfLocation &aPlace );
	
	};


/**
 * Method for Externalization. Writes the SmfLocation object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPlace The SmfLocation object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfLocation &aPlace );

/**
 * Method for Internalization. Reads a SmfLocation object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPlace The SmfLocation object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfLocation &aPlace);

typedef QList<SmfLocation> SmfLocationList;

// Make the class SmfLocation known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfLocation)
Q_DECLARE_METATYPE(QList<SmfLocation>)

#endif /* SMFLOCATION_H_ */
