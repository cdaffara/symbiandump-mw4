/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   alfasynchgoomsession provides asynchronous 
*                connection to graphics out of memory monitor.
*
*/


#include "alfasynchgoomsession.h"
#include "alflogger.h"

// Granularity for pending uids array
const TInt KAlfPendingUidsGranularity = 2;

// Granularity for running apps array
const TInt KAlfRunningAppsGranularity = 8;

// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::CAlfAsynchGoomSession
// ---------------------------------------------------------------------------
//
CAlfAsynchGoomSession::CAlfAsynchGoomSession() 
    : CActive( EPriorityHigh ),
      iPendingUids( KAlfPendingUidsGranularity ),
      iRunningAppsList( KAlfRunningAppsGranularity )
    {
    __ALFLOGSTRING("CAlfAsynchGoomSession::CAlfAsynchGoomSession");        
    CActiveScheduler::Add(this);
    }
    
// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::~CAlfAsynchGoomSession
// ---------------------------------------------------------------------------
//
CAlfAsynchGoomSession::~CAlfAsynchGoomSession()
    {
    __ALFLOGSTRING("CAlfAsynchGoomSession::~CAlfAsynchGoomSession");
    Cancel();

    iGoomSession.Close();
    
    iPendingUids.Close();
    iRunningAppsList.Close();
    }

// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::AppAboutToStart
// ---------------------------------------------------------------------------
//
void CAlfAsynchGoomSession::AppAboutToStart( const TUid& aAppUid )
    {
    if ( iRunningAppsList.Find( aAppUid.iUid ) != KErrNotFound ) 
        {
        // Application already running - ingored.
        return;
        }
    iRunningAppsList.Append( aAppUid.iUid );
    
    if ( IsActive() )
        {
        __ALFLOGSTRING1("CAlfAsynchGoomSession::AppAboutToStart queue %x", aAppUid.iUid);
        iPendingUids.Append( aAppUid );
        return;
        }
        
    if ( !iConnected )
        {
        Start();
        }
    else
        {
        __ALFLOGSTRING1("CAlfAsynchGoomSession::AppAboutToStart %x", aAppUid.iUid);
        iGoomSession.AppAboutToStart( iStatus, aAppUid );
        SetActive();
        }        
    }

// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::AppClosed
// ---------------------------------------------------------------------------
//
void CAlfAsynchGoomSession::AppClosed( const TUid& aAppUid )
    {
    TInt index = iRunningAppsList.Find( aAppUid.iUid );
    if ( index != KErrNotFound )
        {
        iRunningAppsList.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::Start
// ---------------------------------------------------------------------------
//
void CAlfAsynchGoomSession::Start()
    {
    if ( iConnecting )
        {
        return;
        }
        
    TInt err = iGoomSession.Connect( iStatus );        
    
    __ALFLOGSTRING1("CAlfAsynchGoomSession::Start Connect %d", err);
    
    if ( err == KErrNone )
        {
        SetActive();
        iConnecting = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::RunL
// ---------------------------------------------------------------------------
//
void CAlfAsynchGoomSession::RunL()
    {       
    if ( iConnecting )
        {        
        iConnected = ( iStatus.Int() == KErrNone );
        iConnecting = EFalse;

        __ALFLOGSTRING1("CAlfAsynchGoomSession::RunL Connected %d", iConnected);
    
        if ( !iConnected )
            {
            iGoomSession.Close();
            }
        }
    
    if ( iConnected && iPendingUids.Count() )
        {
        TUid uid = iPendingUids[0];
        iPendingUids.Remove(0);
        
        __ALFLOGSTRING1("CAlfAsynchGoomSession::RunL %x", uid.iUid);

        iGoomSession.AppAboutToStart( iStatus, uid );
        SetActive();
        }
    }
    
// ---------------------------------------------------------------------------
// CAlfAsynchGoomSession::DoCancel
// ---------------------------------------------------------------------------
//
void CAlfAsynchGoomSession::DoCancel()
    {
    // There is not much we can do.
    }
