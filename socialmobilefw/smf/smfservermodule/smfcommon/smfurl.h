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
 * The SmfUrl class represents an URL. This class has been constructed based on 
 * the link element of the The Atom Syndication Format 
 * (refer http://tools.ietf.org/html/rfc4287).
 * For detailed description about atom link relations, refer 
 * "http://www.iana.org/assignments/link-relations/link-relations.xhtml"
 *
 */

#ifndef SMFURL_H_
#define SMFURL_H_

#include <qdatastream.h>
#include <QSharedData>
#include <smfclientglobal.h>
#include <QMetaType>

class SmfUrlPrivate;

/**
 * @ingroup smf_common_group
 * The SmfUrl class represents an URL. This class has been constructed based on 
 * the link element of the The Atom Syndication Format 
 * (refer http://tools.ietf.org/html/rfc4287)
 * For detailed description about atom link relations, refer 
 * "http://www.iana.org/assignments/link-relations/link-relations.xhtml"
 */
class SMFCOMMON_EXPORT SmfUrl
	{
public:
	/**
	 * Constructor with default argument
	 */
	SmfUrl( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfUrl( const SmfUrl &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 */
	SmfUrl& operator=( const SmfUrl &aOther );
	
	/**
	 * Destructor
	 */
	~SmfUrl( );
	
	/**
	 * Method to get the href attribute of the link
	 * @return The href attribute of the link
	 */
	QUrl href( ) const;
	
	/**
	 * Method to get the rel attribute of the link
	 * @return The rel attribute of the link
	 */
	QString rel( ) const;
	
	/**
	 * Method to get the type attribute of the link
	 * @return The type attribute of the link
	 */
	QString type( ) const;
	
	/**
	 * Method to get the hreflang attribute of the link
	 * @return The hreflang attribute of the link
	 */
	QString hreflang( ) const;
	
	/**
	 * Method to get the title attribute of the link
	 * @return The title attribute of the link
	 */
	QString title( ) const;
	
	/**
	 * Method to get the length attribute of the link
	 * @return The length attribute of the link
	 */
	QString length( ) const;
	
	/**
	 * Method to get the id of the URL
	 * @return The ID of the URL
	 */
	QString id( ) const;
	
	/**
	 * Method to set the href attribute of the link
	 * @param aData The href attribute of the link
	 */
	void setHref( const QUrl& aData );
	
	/**
	 * Method to set the rel attribute of the link
	 * @param aData The rel attribute of the link
	 */
	void setRel( const QString &aData );
	
	/**
	 * Method to set the type attribute of the link
	 * @param aData The type attribute of the link
	 */
	void setType( const QString &aData );
	
	/**
	 * Method to set the hreflang attribute of the link
	 * @param aData The hreflang attribute of the link
	 */
	void setHhreflang( const QString &aData );
	
	/**
	 * Method to set the title attribute of the link
	 * @param aData The title attribute of the link
	 */
	void setTitle( const QString &aData );
	
	/**
	 * Method to set the length attribute of the link
	 * @param aData The length attribute of the link
	 */
	void setLength( const QString &aData );
	
	/**
	 * Method to set the id of the URL
	 * @param aId The ID of the URL
	 */
	void setId( const QString &aId );
	
private:
	QSharedDataPointer<SmfUrlPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfUrl &aUrl );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfUrl &aUrl );
	
	};


/**
 * Method for Externalization. Writes the SmfUrl object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aUrl The SmfUrl object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfUrl &aUrl );

/**
 * Method for Internalization. Reads a SmfUrl object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aUrl The SmfUrl object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfUrl &aUrl);

typedef QList<SmfUrl> SmfUrlList;

// Make the class SmfUrl known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfUrl)
Q_DECLARE_METATYPE(QList<SmfUrl>)

#endif /* SMFURL_H_ */

