/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* This class is used to handle IKEv2 configuration payload attributes.
*
*/
#ifndef __IKEV2CONFIG_H_
#define __IKEV2CONFIG_H_
#include <e32base.h>
#include "vpnmandefs.h"

class CIkev2Acquire;
class TCPPayloadIkev2;
class TDataAttributes;
class TInetAddr;
//
//
//  CIkev2Config
//  This class is used to handle IKEv2 config payload attributes
//

NONSHARABLE_CLASS(CIkev2Config) : public CBase
{
	public:
		static CIkev2Config* NewL(CIkev2Acquire* aAcquire, TInetAddr* aRemoteIp=NULL);		
		~CIkev2Config();
		TBool ProcessCpL(TCPPayloadIkev2* aCpPayload);
		TPtrC8 Cp() const;
		TUint8 CpType() const;
		TUint32 ExpireTime() const;		
		TVPNAddress VirtualIp();
		
		
	private:
		void ConstructL(CIkev2Acquire* aAcquire, TInetAddr* aRemoteIp);
		TInt AddAttribute(TDataAttributes* aAttr, TUint8 aType, TInt aLth, TUint8* aData);		

	private:
		TBool     iRequestGoing;  // CP Request going
		TUint32   iAddressExpiry; // Address expiration time
		HBufC8*   iCp;            // CP Payload buffer
		TUint8    iCpType;
		TVPNAddress iVIP;   // Virtual IP 
};


#endif
