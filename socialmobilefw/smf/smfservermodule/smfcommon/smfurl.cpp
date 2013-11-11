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
 * (refer http://tools.ietf.org/html/rfc4287)
 * For detailed description about atom link relations, refer 
 * "http://www.iana.org/assignments/link-relations/link-relations.xhtml"
 *
 */

#include <QUrl>

#include "smfurl.h"
#include "smfurl_p.h"

/**
 * Constructor with default argument
 */
SmfUrl::SmfUrl( )
	{
	d = new SmfUrlPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfUrl::SmfUrl( const SmfUrl &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfUrl& SmfUrl::operator=( const SmfUrl &aOther )
	{
	d->m_id = aOther.d->m_id;
	return *this;
	}

/**
 * Destructor
 */
SmfUrl::~SmfUrl( )
	{
	}


/**
 * Method to get the href attribute of the link
 * @return The href attribute of the link
 */
QUrl SmfUrl::href( ) const
	{
	return d->m_href;
	}

/**
 * Method to get the rel attribute of the link
 * @return The rel attribute of the link
 */
QString SmfUrl::rel( ) const
	{
	return d->m_rel;
	}

/**
 * Method to get the type attribute of the link
 * @return The type attribute of the link
 */
QString SmfUrl::type( ) const
	{
	return d->m_type;
	}

/**
 * Method to get the hreflang attribute of the link
 * @return The hreflang attribute of the link
 */
QString SmfUrl::hreflang( ) const
	{
	return d->m_hreflang;
	}

/**
 * Method to get the title attribute of the link
 * @return The title attribute of the link
 */
QString SmfUrl::title( ) const
	{
	return d->m_title;
	}

/**
 * Method to get the length attribute of the link
 * @return The length attribute of the link
 */
QString SmfUrl::length( ) const
	{
	return d->m_length;
	}

/**
 * Method to get the id of the URL
 * @return The ID of the URL
 */
QString SmfUrl::id( ) const
	{
	return d->m_id;
	}

/**
 * Method to set the href attribute of the link
 * @param aData The href attribute of the link
 */
void SmfUrl::setHref( const QUrl& aData )
	{
	d->m_href = aData;
	}

/**
 * Method to set the rel attribute of the link
 * @param aData The rel attribute of the link
 */
void SmfUrl::setRel( const QString &aData )
	{
	d->m_rel = aData;
	}

/**
 * Method to set the type attribute of the link
 * @param aData The type attribute of the link
 */
void SmfUrl::setType( const QString &aData )
	{
	d->m_type = aData;
	}

/**
 * Method to set the hreflang attribute of the link
 * @param aData The hreflang attribute of the link
 */
void SmfUrl::setHhreflang( const QString &aData )
	{
	d->m_hreflang = aData;
	}

/**
 * Method to set the title attribute of the link
 * @param aData The title attribute of the link
 */
void SmfUrl::setTitle( const QString &aData )
	{
	d->m_title = aData;
	}

/**
 * Method to set the length attribute of the link
 * @param aData The length attribute of the link
 */
void SmfUrl::setLength( const QString &aData )
	{
	d->m_length = aData;
	}

/**
 * Method to set the id of the URL
 * @param aId The ID of the URL
 */
void SmfUrl::setId( const QString &aId )
	{
	d->m_id = aId;
	}


/**
 * Method for Externalization. Writes the SmfUrl object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aUrl The SmfUrl object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfUrl &aUrl )
	{
	// Serialize d->m_href
	aDataStream<<aUrl.d->m_href;
	
	// Serialize d->m_rel
	aDataStream<<aUrl.d->m_rel;
	
	// Serialize d->m_type
	aDataStream<<aUrl.d->m_type;
	
	// Serialize d->m_hreflang
	aDataStream<<aUrl.d->m_hreflang;
	
	// Serialize d->m_title
	aDataStream<<aUrl.d->m_title;
	
	// Serialize d->m_length
	aDataStream<<aUrl.d->m_length;
	
	// Serialize d->m_id
	aDataStream<<aUrl.d->m_id;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfUrl object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aUrl The SmfUrl object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfUrl &aUrl)
	{
	// Deserialize d->m_href
	aDataStream>>aUrl.d->m_href;
	
	// Deserialize d->m_rel
	aDataStream>>aUrl.d->m_rel;
	
	// Deserialize d->m_type
	aDataStream>>aUrl.d->m_type;
	
	// Deserialize d->m_hreflang
	aDataStream>>aUrl.d->m_hreflang;
	
	// Deserialize d->m_title
	aDataStream>>aUrl.d->m_title;
	
	// Deserialize d->m_length
	aDataStream>>aUrl.d->m_length;
	
	// Deserialize d->m_id
	aDataStream>>aUrl.d->m_id;
	
	return aDataStream;
	}

