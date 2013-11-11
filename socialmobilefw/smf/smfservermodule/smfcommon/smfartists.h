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
 * The SmfArtists class represents the artists in a track or an album
 *
 */

#ifndef SMFARTISTS_H_
#define SMFARTISTS_H_

#include <QImage>
#include <QUrl>
#include <qdatastream.h>
#include <QSharedData>
#include <smfclientglobal.h>
#include <QMetaType>

class SmfArtistsPrivate;

/**
 * @ingroup smf_common_group
 * The SmfArtists class represents the artists in a track or an album
 */
class SMFCOMMON_EXPORT SmfArtists
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfArtists( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfArtists( const SmfArtists &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 */
	SmfArtists& operator=( const SmfArtists &aOther );
	
	/**
	 * Destructor
	 */
	~SmfArtists( );
	
	/**
	 * Method to get the artists names
	 * @return The list of artists
	 */
	QStringList names( ) const;
	
	/**
	 * Method to get the image of the artists
	 * @return The image of the artists if available, always small size is fetched
	 */
	QImage image( ) const;
	
	/**
	 * Method to get the url of the artists
	 * @return The url of the artists, NULL if not present
	 */
	QUrl url( ) const;

	/**
	 * Method to get the tags applied for artist(s) 
	 * @return list of tags, NULL if not supported
	 */
	QStringList tags( ) const;
	
	/**
	 * Service specific Id for this artist(s), e.g. could be musicbrainz id for this artist 
	 * @return service specific id of the artists, NULL if not supported
	 */
	QString id( ) const;

	/**
	 * Method to set the artists names
	 * @param aList The list of artists names
	 */
	void setNames( const QStringList &aList );
	
	/**
	 * Method to set the image of the artists
	 * @param aImage The image of the artists
	 */
	void setImage( const QImage &aImage );
	
	/**
	 * Method to set the url of the artists
	 * @param aUrl The url of the artists
	 */
	void setUrl( const QUrl &aUrl );
	
	/**
	 * Method to set tags of the artists
	 * @param aTags tags for the artists
	 */
	void setTags( const QStringList &aTags );

	/**
	 * Method to set the service specific ID of the artists
	 * @param aId The id of the artists
	 */
	void setId( const QString &aId );
		
private:
	QSharedDataPointer<SmfArtistsPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfArtists &aArtists );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfArtists &aArtists );
	
	};


/**
 * Method for Externalization. Writes the SmfArtists object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aArtists The SmfArtists object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfArtists &aArtists );

/**
 * Method for Internalization. Reads a SmfArtists object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aArtists The SmfArtists object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfArtists &aArtists);

typedef QList<SmfArtists> SmfArtistsList;

// Make the class SmfArtists known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfArtists)
Q_DECLARE_METATYPE(QList<SmfArtists>)

#endif /* SMFARTISTS_H_ */
