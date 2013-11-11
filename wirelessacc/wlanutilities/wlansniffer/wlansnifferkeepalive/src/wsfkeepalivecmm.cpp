/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Connection Method Manager interface implementation.
 */

// System include files

#include <nifvar.h>
#include <cmmanagerdef.h>
#include <cmconnectionmethod.h>

// User include files

#include "wsfkeepalive.h"
#include "wsfkeepalivecmm.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wsfkeepalivecmmTraces.h"
#endif

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CWsfKeepaliveCmm* CWsfKeepaliveCmm::NewL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECMM_NEWL_ENTRY );
    
    CWsfKeepaliveCmm* me = new ( ELeave ) CWsfKeepaliveCmm();
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    
    OstTraceFunctionExit0( CWSFKEEPALIVECMM_NEWL_EXIT );
    return me;
    }

CWsfKeepaliveCmm::~CWsfKeepaliveCmm()
    {
    OstTraceFunctionEntry0(
        DUP1_CWSFKEEPALIVECMM_CWSFKEEPALIVECMM_ENTRY );

    iCmManager.Close();
    
    OstTraceFunctionExit0(
        DUP1_CWSFKEEPALIVECMM_CWSFKEEPALIVECMM_EXIT );
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWsfKeepaliveCmm::CWsfKeepaliveCmm()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECMM_CWSFKEEPALIVECMM_ENTRY );
    OstTraceFunctionExit0( CWSFKEEPALIVECMM_CWSFKEEPALIVECMM_EXIT );
    }

// ---------------------------------------------------------------------------
// Leaving constructor
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveCmm::ConstructL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECMM_CONSTRUCTL_ENTRY );
    
    iCmManager.OpenL();
    
    OstTraceFunctionExit0( CWSFKEEPALIVECMM_CONSTRUCTL_EXIT );
    }
 
// ---------------------------------------------------------
// Gets Hotspot metadata information
// ---------------------------------------------------------
//
TBool CWsfKeepaliveCmm::GetHotspotInfoL(TInt32 aIapId)
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECMM_GETHOTSPOTINFO_ENTRY );
    TBool isHotspotIap = EFalse; 

    RCmConnectionMethod connectionMethod = iCmManager.ConnectionMethodL(aIapId);
    
    TRAPD(err, isHotspotIap = connectionMethod.GetBoolAttributeL( CMManager::ECmMetaHotSpot ));
    
    if (err != KErrNone)
        {
        isHotspotIap = EFalse;
        }
    
    OstTrace1(
        TRACE_NORMAL,
        CWSFKEEPALIVECMM_GETHOTSPOTINFO_HOTSPOT,
        "CWsfKeepaliveCmm::GetHotspotInfoL isHotspotIap=%d",
        isHotspotIap );
    
    OstTraceFunctionExit0( CWSFKEEPALIVECMM_GETHOTSPOTINFO_EXIT );
    return isHotspotIap;
    }

// ---------------------------------------------------------
// Delete Hotspot IAP
// ---------------------------------------------------------
//
TBool CWsfKeepaliveCmm::DeleteHotspotIapL(TInt32 aIapId)
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECMM_DELETEHOTSPOTIAP_ENTRY );
    TBool ret = EFalse;
    
    RCmConnectionMethod connectionMethod = iCmManager.ConnectionMethodL(aIapId);
    TRAPD(err, ret = connectionMethod.DeleteL());
    
    if (err != KErrNone)
            {
            ret = EFalse;
            }
    
    OstTraceFunctionExit0( CWSFKEEPALIVECMM_DELETEHOTSPOTIAP_EXIT );
    return ret;
    }
