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
* Esock library (RConnection) interface implementation.
*/

// System include files

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <extendedconnpref.h>

// User include files

#include "wsfkeepaliveesock.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wsfkeepaliveesockTraces.h"
#endif

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CWsfKeepaliveEsock* CWsfKeepaliveEsock::NewL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_NEWL_ENTRY );
    
    CWsfKeepaliveEsock* me = new ( ELeave ) CWsfKeepaliveEsock();
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_NEWL_EXIT );
    return me;
    }
 
CWsfKeepaliveEsock::~CWsfKeepaliveEsock()
    {
    OstTraceFunctionEntry0(
        DUP1_CWSFKEEPALIVEESOCK_CWSFKEEPALIVEESOCK_ENTRY );
 
    Cancel();
    iSocketServer.Close();
    
    OstTraceFunctionExit0( DUP1_CWSFKEEPALIVEESOCK_CWSFKEEPALIVEESOCK_EXIT );
    }

void CWsfKeepaliveEsock::ConnectL( TUint aIapId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_CONNECTL_ENTRY );

    // Open an RConnection object.
    User::LeaveIfError( iConnection.Open( iSocketServer ) );
    
    // Create overrides for connection preferences to force opening of the
    // given IAP without any user prompts.
    TConnPrefList prefList;
    TExtendedConnPref prefs;
    prefs.SetIapId( aIapId );
    prefs.SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnSilent );
    prefList.AppendL( &prefs );
     
    // Start a connection with connection preferences
    iConnection.Start( prefList, iStatus );
    
    SetActive();
    
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_CONNECTL_EXIT );
    }

void CWsfKeepaliveEsock::Disconnect()
    {      
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_DISCONNECT_ENTRY );
    
    iConnection.Close();
    
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_DISCONNECT_EXIT );
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWsfKeepaliveEsock::CWsfKeepaliveEsock() :
    CActive( EPriorityStandard )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_CWSFKEEPALIVEESOCK_ENTRY );
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_CWSFKEEPALIVEESOCK_EXIT );
    }

// ---------------------------------------------------------------------------
// Leaving constructor
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveEsock::ConstructL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_CONSTRUCTL_ENTRY );
    
    CActiveScheduler::Add( this );
    User::LeaveIfError( iSocketServer.Connect() );
    
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Handles an active object's request completion event
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveEsock::RunL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_RUNL_ENTRY );

    OstTrace1(
        TRACE_NORMAL,
        CWSFKEEPALIVEESOCK_RUNL,
        "CWsfKeepaliveEsock::RunL;iStatus.Int()=%d",
        iStatus.Int() );
    
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_RUNL_EXIT );
    }

// ---------------------------------------------------------------------------
// Implements cancellation of an outstanding request
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveEsock::DoCancel()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVEESOCK_DOCANCEL_ENTRY );
    
    // Disconnect a (possibly) ongoing request
    Disconnect();
    
    OstTraceFunctionExit0( CWSFKEEPALIVEESOCK_DOCANCEL_EXIT );
    }
