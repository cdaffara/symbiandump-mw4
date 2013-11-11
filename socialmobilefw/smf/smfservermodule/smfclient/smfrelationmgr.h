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
 * Interface spefication for managing associations between various social contacts
 * 
 */

#ifndef SMFRELATIONMGR_H
#define SMFRELATIONMGR_H

#include <smfprovider.h>
#include <smfcontact.h>
#include <smfglobal.h>

// forward declaation
class SmfRelationMgrPrivate;

// implementaion constants
const int maxSmfRelationIdSize = 1000;
const int maxSmfRelationItemSize = 1000;
const int maxRelationItems = 1000;


// persistent Id of a relation.
typedef QString SmfRelationId;

/**
 * @ingroup smf_client_group
 * One record of a contact in a relation. Index specifies their position in the relationship.
 */
class SMFCLIENT_EXPORT SmfRelationItem : public SmfContact
	{
public:
	SmfRelationItem();
	
public:
	SmfProvider getProvider() const;
	void setProvider(SmfProvider& provider);
	
	SmfContact getContact() const;
	void setContact(SmfContact& contact);
	
	quint32	getIndex() const;
	void setIndex(quint32 index);
	
private:
	SmfProvider m_provider;
	SmfContact  m_contact;
	quint32		 m_index;
	};

SMFCLIENT_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const SmfRelationItem &aRelnItem );

SMFCLIENT_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfRelationItem &aRelnItem);


/**
 * All remote profiles (e.g. facebook, twitter, flicker) for a particular
 * person (or entity) constitutes a relation. Relation Manager stores
 * these relations in persistent memory.
 *
 * Relations and Items can be visualized in rows and columns.
 * Each row is identified by a RelationId.
 * Each column is identified by index of that Item in the relation
 * Each Items holds necessary information for SmfProvider and SmfContact.
 * In the diagram below C1, C2... could be local or remote contacts, which means
 * that C1, C2 .. could be contacts from device-resident phonebook. For example, for contact C1,
 * items at index 1,2,3.. in this relationship denotes C1's id in Facebook, Twitter, flickr,... .
 *
 *                                  Relation Item           Relation Item          Relation Item      Relation Item        Relation Item
 *                                        Index1                  index2                  index3                  index4                  index5
 *                             _______________________________________________________________________
 *                             |                           |                             |                            |                          |                          |
 * RelationId 1         |           C1           |        Remote        |        Remote       |         Remote    |         Remote     |
 *                             |                           |        Contact        |        Contact       |         Contact     |         Contact     |
 *                             ------------------------------------------------------------------------------------------------------------------
 *                             |                           |                             |                           |                           |                          |
 * RelationId 2         |           C2           |        Remote        |        Remote       |                          |                          |
 *                             |                           |        Contact        |        Contact       |                          |                          |
 *                             -----------------------------------------------------------------------------------------------------------------
 *                             |                           |                             |                            |                          |                          |
 * RelationId 3         |          C3            |        Remote        |                            |                          |                          |
 *                             |                           |        Contact        |                            |                          |                          |
 *                             -----------------------------------------------------------------------------------------------------------------
 *                             |                           |                             |                           |                           |                          |
 * RelationId 4         |           C4           |        Remote        |        Remote       |                          |                          |
 *                             |                           |        Contact        |        Contact       |                          |                          |
 *                             -----------------------------------------------------------------------------------------------------------------
 *                             |                           |                             |                           |                           |                          |
 * RelationId 5         |           C5           |        Remote        |                           |                           |                          |
 *                             |                           |        Contact        |                           |                           |                          |
 *                             -----------------------------------------------------------------------------------------------------------------
 *
 * Items can be added (associated) to a given relation. So this is one-to-many
 * relationship. Typically no items be present in more than one relation.
 *
 * All functions are synchronous at this moment.
 */
class SMFCLIENT_EXPORT SmfRelationMgr : public QObject
	{
	Q_OBJECT
public:
	SmfRelationMgr(QObject* parent = 0);
	~SmfRelationMgr();

public slots:

	/** create a new relation. The contact provided is the first contact in this
	 * relation.
	 * Please note that contact should have valid Guid @ref QtMobility::QContactGuid
	 * (used by Smf to store the unique id of user from facebook, etc.) and valid Url
	 * @ref QtMobility::QContactUrl
	 *
	 * If the contact is already stored with ContactManager (e.g. contact exists in phonebook),
	 * then it would have QtMobility::QContactId properly filled with managerUri and localId.
	 * These two field would also be stored in SmfRelationMgr (for easily identifying the SmfContacts).
	 *
	 * After successful creation, a relationId would be returned. More and more SmfContacts can
	 * be added to this relation using this RelationId
	 */
	SmfRelationId create(SmfProvider *provider=NULL, SmfContact *contact=NULL);

	/** assign contact to a relation */
	SmfError associate( SmfRelationId& phoneId,
						const SmfContact* contact,
						SmfProvider* provider);

	/** remove contact from a relation */
	SmfError remove(SmfRelationId& relation,
					const SmfContact* contact);

	/** returns first relation item in the relation when exists, NULL otherwise */
	SmfRelationItem* searchById(const SmfRelationId& relation);

	/** returns relation Id for a given contacts if exists, NULL otherwise */
	SmfRelationId searchByContact( SmfContact contact);

	/** returns number of contacts in a relation*/
	uint count(SmfRelationId relation);

	/** contacts and their provider */
	SmfRelationItem* get(SmfRelationId& relation, quint32 index);

	/** list of contacts and their provider */
	QList<SmfRelationItem> getAll(SmfRelationId& relation);

	/** list of all relations */
	QList<SmfRelationId> getAllRelations();

	/** delete a particular relation*/
	SmfError Delete(SmfRelationId& relation);

private:
	SmfRelationMgrPrivate* m_private;	//private impl wrapper
	};

#endif // SMFRELATIONMGR_H
