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
* Description: IKE v2 Protocol plug-in.
*
*/
#if !defined(__IKEV2PLUGIN_H__)
#define __IKEV2PLUGIN_H__

#include <e32base.h>
#include <ipsecpolapi.h>

#include "ikepluginif.h"
#include "pfkeysocketif.h"

class CIkev2PluginSession;
class CIpsecSaSpecList;
class CIpsecPolicyUtil;


NONSHARABLE_CLASS(CIkev2PlugIn) : public CBase,                                   
                                  public MIkePluginIf,
                                  public MPFKeyMessageListener

/** 
* IKEv2 protocol plugin 
* @internalComponent
*/
    {
public:
	static CIkev2PlugIn* NewL( MKmdEventLoggerIf& aEventLogger,
                               MIkeDebug& aDebug );
	~CIkev2PlugIn();
	    
    MIkePluginSessionIf* CreateSessionL( TUint32 aVpnIapId,
                                         TUint32 aVpnNetId,
                                         TUint32 aVpnInterfaceIndex,
                                         MIkeDataInterface& aIkeDataInterface );    

    void PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage );       
    void PluginSessionDeleted(const MIkePluginSessionIf* aDeletedSession);

private:
    CIkev2PlugIn( MKmdEventLoggerIf& aEventLogger,
                  MIkeDebug& aDebug );
    void ConstructL();

	
	RPointerArray<CIkev2PluginSession> iPluginSessions;
	MKmdEventLoggerIf&   iEventLogger;
	MIkeDebug&           iDebug;
		
	CPFKeySocketIf*     iPfKeySocketIf;
	CIpsecPolicyUtil*   iIpsecPolicyUtil;
    };

#endif
