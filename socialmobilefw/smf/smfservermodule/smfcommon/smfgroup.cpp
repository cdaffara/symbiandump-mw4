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
 * The group class represents an instance of a group as per SN site terminolgy
 */
	
#include "smfgroup.h"
#include "smfgroup_p.h"

/**
 * Constructor with default argument
 * @param list The list of members in the group
 */
SmfGroup::SmfGroup( QList<SmfContact>* list )
	{
	d = new SmfGroupPrivate;
	if(list)
		d->m_members = list;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfGroup::SmfGroup( const SmfGroup &aOther ): d(aOther.d)
	{
	
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfGroup& SmfGroup::operator=( const SmfGroup &aOther )
	{
	d->m_members = aOther.d->m_members;
	d->m_groupName = aOther.d->m_groupName;
	d->m_groupId = aOther.d->m_groupId;
	return *this;
	}

/**
 * Destructor
 */
SmfGroup::~SmfGroup( )
	{
	
	}
	
/**
 * Method to get the list of members in the group
 * @return The list of members in the group
 */
QList<SmfContact> SmfGroup::members( ) const
	{
	return *(d->m_members) ;
	}
	
/**
 * Method to set members
 * 
 */
void SmfGroup::setMembers(QList<SmfContact>* mems)
	{
	d->m_members = mems;
	}

/**
 * Method to get the name of the group
 * @return The name of the group
 */
QString SmfGroup::name( ) const
	{
	return d->m_groupName;
	}

/**
 * Method to set name
 */
void SmfGroup::setName(QString& name)
	{
	d->m_groupName = name;
	}

/**
 * Method to get the id of the group
 * @return The ID value 
 */
QString SmfGroup::id( ) const
	{
	return d->m_groupId;
	}

/**
 * Method to set id
 */
void SmfGroup::setId(QString& id)
	{
	d->m_groupId = id;
	
	}
/**
 * Method for Externalization. Writes the SmfGroup object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aGroup The SmfGroup object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfGroup &aGroup )
	{
	aDataStream<<aGroup.members();
	aDataStream<<aGroup.name();
	aDataStream<<aGroup.id();
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfGroup object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aGroup The SmfGroup object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfGroup &aGroup)
	{
	QList<SmfContact> list;
	aDataStream>>list;
	aGroup.setMembers(&list);
	
	QString grpname;
	aDataStream>>grpname;
	aGroup.d->m_groupName = grpname;
	
	QString grpId;
	aDataStream>>grpId;
	aGroup.d->m_groupId = grpId;
	
	return aDataStream;
	}
