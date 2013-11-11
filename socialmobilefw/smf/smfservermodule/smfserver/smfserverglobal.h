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
 * SMF Server wide global const and macros, to be shared among SMF components.
 *
 */

#ifndef SMFSERVERGLOBAL_H_
#define SMFSERVERGLOBAL_H_

#include <QString>

/**
 * Enum declaration for Network status information
 */
enum SmfNetworkStatus
	{
	SmfNetworkConnectedHome = 0,
	SmfNetworkConnectedRoaming,	// value = 1
	SmfNetworkStateNotKnown,	// value = 2
	SmfNetworkConnected,		// value = 3
	SmfNetworkNotConnected		// value = 4
	};

/**
 * Enum declaration for result of transport initialize operation
 */
enum SmfTransportInitializeResult
	{
	SmfTransportInitNetworkNotAvailable = 0,
	SmfTransportInitRoamingNetworkUsageNotEnabled,	// value = 1
	SmfTransportInitNoError							// value = 2
	};


/**
 * The structure used to track the data usage of each plugins
 */
struct SmfPluginDataUsage
	{
	/**
	 * Application that called the plugin
	 */
	QString iAppName;
	
	/**
	 * Interface implemented by the plugin
	 */
	QString iInterfaceName;
	
	/**
	 * The service provider
	 */
	QString iServiceProviderName;
	
	/**
	 * number of bytes sent for this plugin
	 */
	uint iBytesSent;
	
	/**
	 * number of bytes received for this plugin
	 */
	uint iBytesReceived;
	
	};

#endif /* SMFSERVERGLOBAL_H_ */
