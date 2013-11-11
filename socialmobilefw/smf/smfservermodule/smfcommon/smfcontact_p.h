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
 * Private class implemented for implicit sharing of SmfContact class
 *
 */

#ifndef SMFCONTACT_P_H_
#define SMFCONTACT_P_H_

#include <QSharedData>

using namespace QtMobility;

class SmfContactPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfContactPrivate( ) { m_details.clear(); }
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfContactPrivate( const SmfContactPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_details ( aOther.m_details )	{ }
	
	/**
	 * Destructor
	 */
	~SmfContactPrivate( ){};
  
	QVariantMap	m_details;// contact details
	
};

#endif /* SMFCONTACT_P_H_ */
