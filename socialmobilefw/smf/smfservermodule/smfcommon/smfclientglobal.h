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
 * Client DLL macro and server executable name
 *
 */

#ifndef SMFCLIENTGLOBAL_H_
#define SMFCLIENTGLOBAL_H_

#include <QtCore/QtGlobal>
#include <QString>

#ifdef Q_OS_SYMBIAN
	_LIT(KSmfServerName,"smfserver");
	_LIT(KSmfServerSemaphoreName,"smfserverSemaphore");
	_LIT(KSmfServerFilename,"smfserver");
#else
	const QString KSmfServerName("smfserver");
	const QString KSmfServerSemaphoreName("smfserverSemaphore");
	const QString KSmfServerFilename("smfserver");
#endif


 #ifdef SMFCLIENT_LIB_EXPORT
	 #define SMFCLIENT_EXPORT Q_DECL_EXPORT
 #else
	 #define SMFCLIENT_EXPORT Q_DECL_IMPORT
 #endif
	
#ifdef SMFCOMMON_LIB_EXPORT
	 #define SMFCOMMON_EXPORT Q_DECL_EXPORT
#else
	 #define SMFCOMMON_EXPORT Q_DECL_IMPORT
#endif

	
/**
 * To be designed later
 */
#define SMF_GETSERVICES(INTERFACE,INTERFACESTRING)
	
#endif /* SMFCLIENTDEFS_H_ */
