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
 * The music rating class represents an instance of rating 
 * about a music track
 *
 */

#ifndef SMFMUSICRATING_H_
#define SMFMUSICRATING_H_

#include <QSharedData>
#include "smfclientglobal.h"
#include <QMetaType>

class SmfTrackInfo;
class SmfMusicRatingPrivate;

/**
 * Indicates range of values to represent ratings,
 * SP should normalize their rating value to this scale
 */
const int SMF_MAX_RATING = 10;
const int SMF_MIN_RATING = 0;

/**
 * @ingroup smf_common_group
 * The music rating class represents an instance of rating 
 * about a music track 
 */
class SMFCOMMON_EXPORT SmfMusicRating
	{
public:
	/**
	 * Constructor with default argument
	 * @param aParent The SmfTrackInfo instance
	 */
	SmfMusicRating( SmfTrackInfo *aParent = 0 );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfMusicRating( const SmfMusicRating &aOther );
	
	/**
	 * Overloaded = operator 
	 * @param aOther The reference object
	 */
	SmfMusicRating& operator=( const SmfMusicRating &aOther );
	
	/**
	 * Destructor
	 */
	~SmfMusicRating( );
	
	/**
	 * Method to get the rating
	 * @return The rating value
	 */
	int rating( ) const;

	/**
	 * Method to get the max rating
	 * @return The max rating value
	 */
	int maxRating( ) const;
	
	/**
	 * Method to get the min rating
	 * @return The min rating value
	 */
	int minRating( ) const;
	
	/**
	 * Method to set the rating
	 * @param aRating The rating value
	 */
	void setRating( const int &aRating );

	/**
	 * Method to set the max rating
	 * @param aMax The max rating value
	 */
	void setMaxRating( const int &aMax );
	
	/**
	 * Method to set the min rating
	 * @param aMin The min rating value
	 */
	void setMinRating( const int &aMin );
	
private:
	QSharedDataPointer<SmfMusicRatingPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfMusicRating &aMusicRating );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfMusicRating &aMusicRating );
	
	};


/**
 * Method for Externalization. Writes the SmfMusicRating object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aMusicRating The SmfMusicRating object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfMusicRating &aMusicRating );

/**
 * Method for Internalization. Reads a SmfMusicRating object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aMusicRating The SmfMusicRating object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfMusicRating &aMusicRating);


// Make the class SmfMusicRating known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfMusicRating)
Q_DECLARE_METATYPE(QList<SmfMusicRating>)

#endif /* SMFMUSICRATING_H_ */
