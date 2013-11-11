
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
 * Observer class to notify availability of asynchronous requests
 *
 */

#ifndef SMFOBSERVER_H_
#define SMFOBSERVER_H_

#include <QObject>

#include "smfglobal.h"

// Forward declaration
class QByteArray;

/**
 * Observer class to notify availability of asynchronous requests
 */
class smfObserver : public QObject
	{
public:
	/**
	 * To notify availibility of asynchronous requests.
	 * @param result Requested result, before using must check error param.
	 * @param opcode Requested opcode, for which the result has arrived.
	 * @param error Error
	 */
	virtual void resultsAvailable(QByteArray result,SmfRequestTypeID opcode, SmfError error) = 0;
	};

#endif
