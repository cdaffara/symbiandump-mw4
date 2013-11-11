/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for using IPSec policy server
*
*/


#include "ipsecsalist.h"

// CLASS HEADER
#include "ipsecpolicyutil.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsecPolicyUtil* CIpsecPolicyUtil::NewL()
    {
    CIpsecPolicyUtil* self = new (ELeave) CIpsecPolicyUtil();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIpsecPolicyUtil::~CIpsecPolicyUtil()
    {
    iIpsecPolicyServ.Close();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIpsecPolicyUtil::CIpsecPolicyUtil()
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CIpsecPolicyUtil::ConstructL()
    {
    User::LeaveIfError( iIpsecPolicyServ.Connect() );
    }

// ---------------------------------------------------------------------------
// Get acceptable IPsec policies for specified selectors. Get all acceptable
// SA specifications with sequential GetIPSecSAInfo() method calls.
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsecSaSpecList* CIpsecPolicyUtil::GetIpseSaSpecListLC( TInetAddr aLocalAddr, TInetAddr aLocalMask, 
                                                                  TInetAddr aRemoteAddr, TInetAddr aRemoteMask,
                                                                  TInt aProtocol, TUint32 aVpnNetId )
    {
    CIpsecSaSpecList* ipsecSaList = new (ELeave)CIpsecSaSpecList();
    CleanupStack::PushL(ipsecSaList);
    
    aLocalAddr.ConvertToV4Mapped();
    aLocalMask.ConvertToV4Mapped();
    aRemoteAddr.ConvertToV4Mapped();
    aRemoteAddr.SetScope(aVpnNetId);
    aRemoteMask.ConvertToV4Mapped();
    
    
    TIpsecSelectorInfo selectorInfo;
    selectorInfo.iLocal = aLocalAddr;
    selectorInfo.iLocalMask = aLocalMask;                
    selectorInfo.iRemote = aRemoteAddr;
    selectorInfo.iRemoteMask = aRemoteMask;
    selectorInfo.iProtocol  = aProtocol;
    selectorInfo.iSaIndex   = 0;
    TPckg<TIpsecSelectorInfo> pckgSelectorInfo(selectorInfo);
    TIpsecSaSpec saInfo;
    do
        {
        TRequestStatus requestStatus;        
        TPckg<TIpsecSaSpec> pckgSASpec(saInfo);
        
        iIpsecPolicyServ.MatchSelector( pckgSelectorInfo, 
                                        pckgSASpec,
                                        requestStatus );
        User::WaitForRequest(requestStatus);
        User::LeaveIfError(requestStatus.Int());
        
        ipsecSaList->AppendL(saInfo);
        selectorInfo.iSaIndex++;
        }
    while(saInfo.iMoreSasExist);        

    return ipsecSaList;    
    }


