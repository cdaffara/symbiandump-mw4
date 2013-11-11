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
 * Private Qt wrapper
 */

#ifndef SMFCLIENT_P_H_
#define SMFCLIENT_P_H_

#include <QObject>
#include <QDateTime>
#include <QStringList>

#include "smfclient.h"
#include "smfclientglobal.h"
#include "smfglobal.h"
#include "smfprovider.h"
#include "smfobserver.h"
#ifdef Q_OS_SYMBIAN
	#include "smfclientsymbian.h"
#else
	#include "smfclientqt.h"
#endif


/**
 * Private Qt wrapper
 * Provided to accomodate diff facilities in diff platforms.
 * Uses Symbian client-server for Symbian
 * QDbus for linux
 * QLocalserver for rest of the platforms
 * Note:- only symbian client-server impl is provided at the moment
 */
class SmfClientPrivate : public smfObserver
{
	Q_OBJECT

public:

	/**
	 * Constructs SmfClientPrivate
	 * @param client SmfClient instance, so that it can directly emit SmfClient's signal
	 */
	SmfClientPrivate(SmfClient* client = 0);
	
	/**
	 * Destructor
	 */
	~SmfClientPrivate(){};
	
public:
	/**
	 * Corresponds to SmfClient::GetServices(const QString )
	 */
	QList<SmfProvider>* GetServices(const QString serviceName);
	
	/**
	* From smfobserver.
	* This callback is invoked when requested result is available from
	* other components of Smf (specially from plugin manager).
	* @param result Parsed and serialized result.
	* @param opcode Request code.
	* @param error Error
	*/
	void resultsAvailable(QByteArray result,SmfRequestTypeID opcode,SmfError error);
	
private:
	SmfClient* m_client;
	QList<SmfProvider>* m_baseProvider;
	QByteArray m_providerSerialized;	// serialized byte array of provider
	
#ifdef Q_OS_SYMBIAN
	CSmfClientSymbian* m_SmfClientPrivate;
	friend class CSmfClientSymbian;
#else
	SmfClientQt* m_SmfClientPrivate;
	friend class SmfClientQt;
#endif
		
	};

#endif /* SMFCLIENT_P_H_ */
