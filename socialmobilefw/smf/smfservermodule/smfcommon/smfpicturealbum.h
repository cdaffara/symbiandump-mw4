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
 * The album class represents an online picture album (collection/gallery)
 *
 */

#ifndef SMFPICTUREALBUM_H_
#define SMFPICTUREALBUM_H_

#include <QImage>
#include <QDateTime>
#include <QStringList>
#include <QUrl>
#include <qdatastream.h>
#include <QSharedData>
#include <smfclientglobal.h>
#include <QMetaType>
#include <smfcomment.h>
#include <smfpicture.h>

class SmfPictureAlbumPrivate;
/**Implementation const */
const int MaxSmfPictureAlbumSize = 20000;//20KB

/**
 * @ingroup smf_common_group
 * album class represents an online picture album (collection)
 */
class SMFCOMMON_EXPORT SmfPictureAlbum
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfPictureAlbum( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfPictureAlbum( const SmfPictureAlbum &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The current object reference
	 */
	SmfPictureAlbum& operator=(const SmfPictureAlbum &aOther);
	
	/**
	 * Destructor
	 */
	~SmfPictureAlbum( );
	
	/**
	 * Method to get a album title
	 * @return The title of the album
	 */
	QString title( ) const;
	
	/**
	 * Method to get a album description
	 * @return The description of the album
	 */
	QString description( ) const;
	
	/**
	 * Method to get a visibility of a album for public
	 * @return The visibility mode of this album for others
	 */
	SmfPictureVisibility visibility( ) const;
	
	/**
	 * Method to get the date of posting the album
	 * @return The posted date of the album
	 */
	QDateTime postedDate( ) const;
	
	/**
	 * Method to get the comments for the album
	 * @return The comments for the album
	 */
	QList<SmfComment> comments( ) const;
	
	/**
	 * Method to get the keywords for the album
	 * @return The keywords for the album
	 */
	QStringList keywords( ) const;
	
	/**
	 * Method to get the url of the album
	 * @return The url of the album
	 */
	QUrl url( ) const;
	
	/**
	 * Method to get the thumbnail for this album as QImage
	 * @return The picture as QImage
	 */
	QImage thumbnail( ) const;

	/**
	 * Method to get the number of pictures in this album
	 * @return number of pictures in this album
	 */
	qint32 pictureCount( ) const;

	/**
	 * Method to get the id of the album
	 * @return The ID value 
	 */
	QString id( ) const;
		
	/**
	 * Method to set a album title
	 * @param aTitle The title of the album
	 */
	void setTitle( const QString &aTitle );
	
	/**
	 * Method to set a album description
	 * @param aDescription The description of the album
	 */
	void setDescription( const QString &aDescription );
	
	/**
	 * Method to set a visibility of a album for public
	 * @param aVisibility aVisibility The visibility mode of 
	 * this album for others
	 */
	void setVisibility( const SmfPictureVisibility &aVisibility );
	
	/**
	 * Method to set the date of posting the album
	 * @param aDate The post date of the album
	 */
	void setPostedDate( const QDateTime &aDate );
	
	/**
	 * Method to add comment on the album
	 * @param aComment The comment for the album
	 */
	void addComment( const SmfComment &aComment );
	
	/**
	 * Method to add keywords for the album
	 * @param aTag The tag for the album
	 */
	void addKeywords( const QStringList &aWords );
	
	/**
	 * Method to set the url of the album
	 * @param aUrl The url of the album
	 */
	void setUrl( const QUrl &aUrl );

	/**
	 * Method to set the thumbnail for this album as QImage
	 * @return The picture as QImage
	 */
	void setThumbnail(const QImage &aImage );
	
	/**
	 * Method to get the number of pictures in this album
	 * @return number of pictures in this album
	 */
	 void setPictureCount( const qint32 aCount);

	/**
	 * Method to set the id of the album
	 * @param aId The ID value 
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfPictureAlbumPrivate> d;
	
	friend QDataStream& operator<<( QDataStream &aDataStream, 
			const SmfPictureAlbum &aAlbm );

	friend QDataStream& operator>>( QDataStream &aDataStream, 
			SmfPictureAlbum &aAlbm );
	
	};


/**
 * Method for Externalization. Writes the SmfPictureAlbum object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aAlbm The SmfPictureAlbum object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream& operator<<( QDataStream &aDataStream, 
		const SmfPictureAlbum &aAlbm );

/**
 * Method for Internalization. Reads a SmfPictureAlbum object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aAlbm The SmfPictureAlbum object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream& operator>>( QDataStream &aDataStream, 
		SmfPictureAlbum &aAlbm);

typedef QList<SmfPictureAlbum> SmfPictureAlbumList;

// Make the class SmfPictureAlbum known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfPictureAlbum)
Q_DECLARE_METATYPE(QList<SmfPictureAlbum>)

#endif /* SMFPICTUREALBUM_H_ */
