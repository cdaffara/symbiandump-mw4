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
 * Private class implemented for implicit sharing of SmfComment class
 *
 */

#ifndef SMFCOMMENT_P_H_
#define SMFCOMMENT_P_H_

#include <QDateTime>
#include <QSharedData>

class SmfCommentPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfCommentPrivate( ) { m_text.clear(); m_commentId.clear(); }
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfCommentPrivate( const SmfCommentPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_text ( aOther.m_text ), 
		m_timeStamp ( aOther.m_timeStamp ),
		m_commentId ( aOther.m_commentId ) 	{ }
	
	/**
	 * Destructor
	 */
	~SmfCommentPrivate( )
		{
		}
  
	QString m_text;			// comment text
	QDateTime m_timeStamp;	// comment time stamp
	QString m_commentId;	// comment Id
	
};

#endif /* SMFCOMMENT_P_H_ */
