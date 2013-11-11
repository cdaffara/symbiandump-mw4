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
 * Private Implementation of Relation Manager
 * 
 */

#ifndef SMFRELATIONMGR_P_H
#define SMFRELATIONMGR_P_H

#include <QStringList>
#include <smfprovider.h>
#include <smfclientglobal.h>
#include <smfglobal.h>
#include <smfobserver.h>

class SmfRelationMgr;
class SmfProvider;
#ifdef Q_OS_SYMBIAN
	class CSmfClientSymbian;
#else
	class SmfClientQt;
#endif
	
class SmfRelationMgrPrivate
	{
public:
	SmfRelationMgrPrivate(SmfRelationMgr* relnMngr);
	~SmfRelationMgrPrivate();
	
public:
	/** create a relation with first contact (id optional) */
	SmfRelationId create(SmfProvider *provider=NULL, SmfContact *contact=NULL);

	/** assign contact to a relation */
	SmfError associate( SmfRelationId& relation,
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
#ifdef Q_OS_SYMBIAN
	CSmfClientSymbian* m_SmfClientPrivate;
	friend class CSmfClientSymbian;
#else
	SmfClientQt* m_SmfClientPrivate;
	friend class SmfClientQt;
#endif
	QByteArray m_dataForDSM;
	quint8 m_argFlag;
	QByteArray m_dataFromDSM;
	};

#endif // SMFRELATIONMGR_P_H
