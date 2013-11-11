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
 * Contains private implementation of the smfactions
 */

#ifndef SMFACTIONS_P_H_
#define SMFACTIONS_P_H_

#include <QDateTime>
#include <QSharedData>
#include <QVariant>
#include <QImage>
#include <QDateTime>
#include <QStringList>
#include <QUrl>
#include <qdatastream.h>
#include <QSharedData>
#include <QMetaType>

#include "smfactions.h"
#include "smfpost.h"
#include "smfcontact.h"

class SmfActivityObjectPrivate: public QSharedData
	{
public:	
	SmfActivityObjectPrivate( ) {
		m_id.clear();
		m_caption.clear();
		m_link.clear();
		m_content.clear();
	}
	
	SmfActivityObjectPrivate( const SmfActivityObjectPrivate& aOther ) :
		QSharedData (aOther),
		m_id (aOther.m_id),
		m_thumbnail (aOther.m_thumbnail),
		m_caption (aOther.m_caption),
		m_type (aOther.m_type),
		m_objData (aOther.m_objData),
		m_link (aOther.m_link),
		m_time (aOther.m_time),
		m_content (aOther.m_content) { }

	~SmfActivityObjectPrivate( ) {}
	
	/**
	 * Data members
	 */
	QString m_id;
	QImage m_thumbnail;
	QString m_caption;
	SmfActivityObjectType m_type;
	QVariant m_objData;
	QString m_link;
	QDateTime m_time;
	QString m_content;
	};



#endif /* SMFACTIONS_P_H_ */
