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
 * Private class implemented for implicit sharing of SmfGroup class
 *
 */

#ifndef SMFGROUP_P_H_
#define SMFGROUP_P_H_

#include <smfcontact.h>
#include <QSharedData>

class SmfGroupPrivate : public QSharedData
{
public:
	/**
	 * Constructor
	 */
	SmfGroupPrivate( ) { m_members = NULL; m_groupName.clear(); m_groupId.clear(); }
	
	/**
	 * Copy Consturctor
	 * @param aOther The reference object to be copy constructed
	 */
	SmfGroupPrivate( const SmfGroupPrivate &aOther ) : 
		QSharedData ( aOther ), 
		m_members ( aOther.m_members ), 
		m_groupName ( aOther.m_groupName ),
		m_groupId ( aOther.m_groupId ) 	{ }
	
	/**
	 * Destructor
	 */
	~SmfGroupPrivate( ){};
  
	QList<SmfContact> *m_members;	// members in the group
	QString m_groupName;
	QString m_groupId;
	// Other details to be added
};


#endif /* SMFGROUP_P_H_ */
