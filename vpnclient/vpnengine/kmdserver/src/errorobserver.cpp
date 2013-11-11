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
* Description:  active object, that monitors IKE plugin session error
*
*/


#include "ikedebug.h"
#include "ikepluginsessionif.h"
#include "ikepluginsessionhandler.h"

// CLASS HEADER
#include "errorobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CErrorObserver* CErrorObserver::NewL( MIkePluginSessionIf& aIkePluginSession,
                                      MIkePluginSessionHandlerCallback& aCallback,
                                      MIkeDebug& aDebug )
    {
    CErrorObserver* self = new (ELeave) CErrorObserver( aIkePluginSession,
                                                        aCallback,
                                                        aDebug );
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CErrorObserver::~CErrorObserver()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CErrorObserver::CErrorObserver( MIkePluginSessionIf& aIkePluginSession,
                                MIkePluginSessionHandlerCallback& aCallback,
                                MIkeDebug& aDebug )
 : CActive( EPriorityStandard ),
   iIkePluginSession( aIkePluginSession ),
   iCallback( aCallback ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
void CErrorObserver::ConstructL()
    {   
    }

// ---------------------------------------------------------------------------
// Requests asynchronous error notification.
// ---------------------------------------------------------------------------
//
void CErrorObserver::StartObserving()
    {
    iIkePluginSession.NotifyError( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous notification request. 
// ---------------------------------------------------------------------------
//
void CErrorObserver::RunL()
    {
    DEBUG_LOG1(_L("IKE plugin session error=%d"), iStatus.Int());
    iCallback.IkePluginSessionError( iStatus.Int() );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous notification request. 
// ---------------------------------------------------------------------------
//
void CErrorObserver::DoCancel()
    {
    iIkePluginSession.CancelNotifyError();
    }

