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
* Description:  Utility methods to handle PFKEY messaging
*
*/
#ifndef __IKEV2PFKEY
#define __IKEV2PFKEY

#include <e32base.h>

class CIkev2Acquire;
class TIkeV2IpsecSAData;
class TIkev2SAData;
class CIkev2PluginSession;

class Ikev2Pfkey
{
	public:
        static void UpdateIpsecSaDataBaseL(const TIkev2SAData& aIkev2SA, 
                                           const TIkeV2IpsecSAData& aChild,
                                           CIkev2PluginSession& aIkePluginSession,
                                           CIkev2Acquire& aAcquire);
		
		static CIkev2Acquire* DeleteInboundSPI(const TIkev2SAData& aIkev2SA, 
                                          CIkev2PluginSession& aIkePluginSession, 
                                          CIkev2Acquire* aAcquire);

	private:
		static const TUint8* GetIpsecKeys(TPtrC8* aEncrKey, TPtrC8* aIntegKey, const TUint8* aKeyMaterial, TInt aCipherKeyLth, TInt aIntegKeyLth);		
};

#endif
