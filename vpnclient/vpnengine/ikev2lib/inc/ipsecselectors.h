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
* Description: IPSec Traffic Selector handling.
*
*/

#ifndef __IPSECSELECTORS_H_
#define __IPSECSELECTORS_H_
#include <e32base.h>

class TPfkeyMessage;
class TTSPayloadIkev2;
class TTrafficSelector;
class TInetAddr;
class TPfkeyIdentity;
class CIkev2Acquire;
class CIkev2PluginSession;
class CIkev2Payloads;
class TIkeV2TrafficSelector;
class CIkev2PluginSession;

class IpsecSelectors
{
public:
    /**
     * Add aDhGroup, if PFS is used.
     */
	static CIkev2Acquire* GetIpsecPolicyL(CIkev2PluginSession& aPluginSession, CIkev2Payloads* aIkeMsg, TInt aDhGroup = 0);
	static CIkev2Acquire* BuildVirtualAcquireL(CIkev2PluginSession& aPluginSession);	
	static TBool VerifyTrafficSelectorsL(CIkev2Acquire* aAcquire, TTSPayloadIkev2* aTsI, TTSPayloadIkev2* aTsR );
	static void BuildTrafficSelectorsL(CIkev2Acquire* aAcquire, const TInetAddr& aLocalAddr,
                                       const TPfkeyIdentity& aSrcIdent, const TPfkeyIdentity& aDstIdent,
                                       TUint8 aProtocol);
	static TIkeV2TrafficSelector IdentityToSelectorL(const TDesC8& aIdentity, TUint8 aProtocol=0);
	static TBool GetRangeEndAddresses(TInetAddr& aStartAddr, TInetAddr& aEndAddr, TInt aPrefixLen);
	static TBool ValidataTs(const CArrayFix<TIkeV2TrafficSelector>& aTsRef, 
                            const CArrayFix<TIkeV2TrafficSelector>& aTs);
	static TBool CheckPorts(TUint16 aStartRef, TUint16 aEndRef, TUint16 aStart, TUint16 aEnd );
	static TBool CheckAddresses(TUint8 aType, TUint8* aRefAddresses, TUint8* aAddresses );
	
};
	
#endif
