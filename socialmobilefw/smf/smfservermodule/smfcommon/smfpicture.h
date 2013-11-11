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
 * The picture class represents an instance of a picture
 *
 */

#ifndef SMFPICTURE_H_
#define SMFPICTURE_H_

#include <QImage>
#include <QDateTime>
#include <QStringList>
#include <QUrl>
#include <qdatastream.h>
#include <QSharedData>
#include <smfclientglobal.h>
#include <QMetaType>
#include <smfcomment.h>

class SmfPicturePrivate;
/**
 * Max size for one SmfPicture
 */
const int MaxSmfPictureSize = 1000;
/**
 * SmfPictureVisibility enumeration
 */
enum SmfPictureVisibility
	{
	SMFVisibilityFriend,
	SMFVisibilityPersonal,
	SMFVisibilityFamily,
	SMFVisibilityGroup,
	SMFVisibilityPublic
	};

/**
 * @ingroup smf_common_group
 * The picture class represents an instance of a picture
 */
class SMFCOMMON_EXPORT SmfPicture
	{
public:
	/**
	 * Constructor with default argument.
	 * @param aDownloaded This flag is to know whether the content is downloaded
	 * in the device(if true). If false it signifies that SmfPicture contains
	 * a link/url to the location of the content
	 */
	SmfPicture(/*bool aDownloaded = false*/ );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfPicture( const SmfPicture &aOther );
	
	/**
	 * CConstructs SmfPicture from QImage
	 * @param aImage The QImage
	 */
	SmfPicture( const QImage &aImage );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The current object reference
	 */
	SmfPicture& operator=(const SmfPicture &aOther);
	
	/**
	 * Destructor
	 */
	~SmfPicture( );
	
	/**
	 * Method to get a picture owner
	 * @return The owner of the picture
	 */
	QString owner( ) const;
	
	/**
	 * Method to get a picture title
	 * @return The title of the picture
	 */
	QString title( ) const;
	
	/**
	 * Method to get a picture description
	 * @return The description of the picture
	 */
	QString description( ) const;
	
	/**
	 * Method to get a visibility of a picture for public
	 * @return The visibility mode of this picture for others
	 */
	SmfPictureVisibility visibility( ) const;
	
	/**
	 * Method to get the date of posting the picture
	 * @return The posted date of the picture
	 */
	QDateTime postedDate( ) const;
	
	/**
	 * Method to get the comments for the picture
	 * @return The comments for the picture
	 */
	QList<SmfComment> comments( ) const;
	
	/**
	 * Method to get the tags for the picture
	 * @return The tags for the picture
	 */
	QStringList tags( ) const;
	
	/**
	 * Method to get the url of the picture
	 * @return The url of the picture
	 */
	QUrl url( ) const;
	
	/**
	 * Method to get the picture data as QImage
	 * @return The picture as QImage
	 */
	QImage picture( ) const;
	
	/**
	 * Method to get the id of the picture
	 * @return The ID value 
	 */
	QString id( ) const;
	
	/**
	 * Gets download flag. This flag is to know whether the content is downloaded
	 * in the device(if true). If false it signifies that SmfPicture contains
	 * a link/url to the location of the content
	 */
	bool downloadFlag();
			
	/**
	 * Method to set a picture owner
	 * @param aOwner The owner of the picture
	 */
	void setOwner( const QString &aOwner );
	
	/**
	 * Method to set a picture title
	 * @param aTitle The title of the picture
	 */
	void setTitle( const QString &aTitle );
	
	/**
	 * Method to set a picture description
	 * @param aDescription The description of the picture
	 */
	void setDescription( const QString &aDescription );
	
	/**
	 * Method to set a visibility of a picture for public
	 * @param aVisibility aVisibility The visibility mode of 
	 * this picture for others
	 */
	void setVisibility( const SmfPictureVisibility &aVisibility );
	
	/**
	 * Method to set the date of posting the picture
	 * @param aDate The post date of the picture
	 */
	void setPostedDate( const QDateTime &aDate );
	
	/**
	 * Method to add comment on the picture
	 * @param aComment The comment for the picture
	 */
	void addComment( const SmfComment &aComment );
	
	/**
	 * Method to add tags for the picture
	 * @param aTag The tag for the picture
	 */
	void addTags( const QStringList &aTags );
	
	/**
	 * Method to set the url of the picture
	 * @param aUrl The url of the picture
	 */
	void setUrl( const QUrl &aUrl );
	
	/**
	 * Method to set the picture data as QImage
	 * @param aData The picture as QImage
	 */
	void setPicture( const QImage &aData );
	
	/**
	 * Method to set the id of the picture
	 * @param aId The ID value 
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfPicturePrivate> d;
	
	friend QDataStream& operator<<( QDataStream &aDataStream, 
			const SmfPicture &aPic );

	friend QDataStream& operator>>( QDataStream &aDataStream, 
			SmfPicture &aPic );
	
	};


/**
 * Method for Externalization. Writes the SmfPicture object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aPic The SmfPicture object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream& operator<<( QDataStream &aDataStream, 
		const SmfPicture &aPic );

/**
 * Method for Internalization. Reads a SmfPicture object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aPic The SmfPicture object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream& operator>>( QDataStream &aDataStream, 
		SmfPicture &aPic);

typedef QList<SmfPicture> SmfPictureList;

// Make the class SmfPicture known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfPicture)
Q_DECLARE_METATYPE(QList<SmfPicture>)

#endif /* SMFPICTURE_H_ */
