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
 * Connection Monitor disconnect interface implementation.
 */

// System include files

#include <rconnmon.h>

// User include files

#include "wsfkeepaliveconnmondisc.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wsfkeepaliveconnmondiscTraces.h"
#endif

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CWsfKeepaliveConnMonDisc* CWsfKeepaliveConnMonDisc::NewL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMONDISC_NEWL_ENTRY );
    
    CWsfKeepaliveConnMonDisc* me = new ( ELeave ) CWsfKeepaliveConnMonDisc();
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMONDISC_NEWL_EXIT );
    return me;
    }

CWsfKeepaliveConnMonDisc::~CWsfKeepaliveConnMonDisc()
    {
    OstTraceFunctionEntry0(
        DUP1_CWSFKEEPALIVECONNMONDISC_CWSFKEEPALIVECONNMONDISC_ENTRY );
    
    iMonitor.Close();
    
    OstTraceFunctionExit0(
        DUP1_CWSFKEEPALIVECONNMONDISC_CWSFKEEPALIVECONNMONDISC_EXIT );
    }

void CWsfKeepaliveConnMonDisc::Disconnect( TUint aConnectionId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMONDISC_DISCONNECT_ENTRY );
    
    TInt KErr = iMonitor.SetBoolAttribute(
        aConnectionId,
        0,
        KConnectionStop,
        ETrue );
    
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMONDISC_DISCONNECT_EXIT );
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWsfKeepaliveConnMonDisc::CWsfKeepaliveConnMonDisc()
    {
    OstTraceFunctionEntry0(
        CWSFKEEPALIVECONNMONDISC_CWSFKEEPALIVECONNMONDISC_ENTRY );
    OstTraceFunctionExit0(
        CWSFKEEPALIVECONNMONDISC_CWSFKEEPALIVECONNMONDISC_EXIT );
    }

// ---------------------------------------------------------------------------
// Leaving constructor
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveConnMonDisc::ConstructL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMONDISC_CONSTRUCTL_ENTRY );
    
    iMonitor.ConnectL();
    
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMONDISC_CONSTRUCTL_EXIT );
    }
