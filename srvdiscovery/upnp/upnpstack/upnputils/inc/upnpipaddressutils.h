/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares IP Address Util functions
*
*/


#ifndef C_UPNIPADDRESSUTILS_H
#define C_UPNIPADDRESSUTILS_H

#include <e32base.h>
#include <in_sock.h>

// NAMESPACE DECLARATION
namespace UpnpIpAddressUtil
    {
	/**
	GetIfNameForIap
	
	Reads the IF name for the specified IAP ID.
	@param	aIapId IAP ID for reading IF name
	@param	aIfName interface name
	**/
	IMPORT_C void GetIfNameForIap( TUint32 aIapId, TDes& aIfName );
	
	/**
	MatchIfNameWithIpInterface
		
	Matches IF name with IP address of the TCP/IP stack interfaces.
	@param	aSocketServer	Socket Server to be used for reading interfaces
	@param	aIfName	IF name to be found from IP interfaces
	@param	aIfIndex reference that will be set to interface index
	@return	mimetype
	**/	
	IMPORT_C TInetAddr MatchIfNameWithIpInterface( RSocketServ& aSocketServer,
                                                   TDesC& aIfName,
                                                   TInt& aInterfaceIndex  );
    }

#endif // C_UPNIPADDRESSUTILS_H