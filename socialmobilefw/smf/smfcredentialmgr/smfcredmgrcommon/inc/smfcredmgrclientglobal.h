/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 * The header represents the error code, typedefs and export macro declaration for
 * Smf Credential Manager Server-client
 *
 */

#ifndef SMFCREDMGRCLIENTGLOBAL_H
#define SMFCREDMGRCLIENTGLOBAL_H

#include <QtCore/qglobal.h>
#include <QByteArray>
#include <QMap>

typedef QMap<QByteArray, QByteArray> SmfAuthParams;

#if defined(SMFCREDMGRCLIENT_LIBRARY)
#define SmfCredMgrClient_EXPORT Q_DECL_EXPORT
#else
#define SmfCredMgrClient_EXPORT Q_DECL_IMPORT
#endif

/**
 *Enumeration to represent Error occured during API call
 *todo- to be used later
 */
enum SMFCredMgrErrorCode
	{
	/**
	 *No error has occured
	 */
	SmfErrNone = 200,
			
	/**
	 *Plugin not autherised
	 */
	SmfErrUnAutherised,
	
	/**
	 *Parameters of API are unexpected/invalid
	 */
	SmfErrBadParameter,
	
	/**
	 *Error reason is not known
	 */
	SmfErrUnKnown
	};

#endif // SMFCREDMGRCLIENTGLOBAL_H
