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
 * Private class implemented for implicit sharing of SmfUrl class
 *
 */

#ifndef SMFURL_P_H_
#define SMFURL_P_H_

#include <QSharedData>
#include <QUrl>

class SmfUrlPrivate : public QSharedData
	{
public:
	/**
	 * Constructor
	 */
	SmfUrlPrivate( ) { 
		m_id.clear();
	}
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfUrlPrivate( const SmfUrlPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_id ( aOther.m_id ) { }
	
	/**
	 * Destructor
	 */
	~SmfUrlPrivate( )
		{
		}

	/**
	 * Data members
	 */
	QUrl 	m_href;		// The href attribute of an Atom link element, contains the link's IRI
	QString m_rel;		// The rel attribute that indicates the link relation type
	QString m_type;		// The type attribute. It is a hint about the type of the 
						// representation that is expected to be returned when the 
						// value of the href attribute is dereferenced
	QString m_hreflang;	// The hreflang attribute describes the language of the resource 
						// pointed to by the href attribute
	QString m_title;	// The title attribute conveys human-readable information about the link
	QString m_length;	// The length attribute indicates an advisory length 
						// of the linked content in octets
	QString m_id;		// The url ID
	
	};

#endif /* SMFURL_P_H_ */
