/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Visual owner
*
*/



#include "alfsrvvisualowner.h"
#include "alf/alfappsrvsessionbase.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvVisualOwner::CAlfSrvVisualOwner( 
    CAlfAppSrvSessionBase& aSession,
    MHuiVisualOwner* aRealOwner )
    : iSession( aSession ), iRealOwner( aRealOwner )
    {
    iVisualLayoutUpdatedNotificationId = KErrNotFound;
    }


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAlfSrvVisualOwner::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvVisualOwner* CAlfSrvVisualOwner::NewL( 
    CAlfAppSrvSessionBase& aSession,
    MHuiVisualOwner* aRealOwner )
    {
    CAlfSrvVisualOwner* self = new( ELeave ) CAlfSrvVisualOwner( aSession, aRealOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvVisualOwner::~CAlfSrvVisualOwner()
    {
    if ( iVisualLayoutUpdatedNotificationId != KErrNotFound )
        {
        iSession.CompleteCmd( iVisualLayoutUpdatedNotificationId , KErrCancel );
        }
    iRealOwner = NULL;
    }
    
    
// ---------------------------------------------------------------------------
// Return real owner
// ---------------------------------------------------------------------------
//    
MHuiVisualOwner* CAlfSrvVisualOwner::RealOwner()
    {
    return iRealOwner;
    }

// ---------------------------------------------------------------------------
// Set layout notification ID
// ---------------------------------------------------------------------------
//    
void CAlfSrvVisualOwner::SetVisualLayoutUpdatedNotificationId( TInt aNotificationId )
    {
    ASSERT( iVisualLayoutUpdatedNotificationId == KErrNotFound );
    if ( iVisualLayoutUpdatedNotificationId != KErrNotFound )
        {
        iSession.CompleteCmd( iVisualLayoutUpdatedNotificationId , KErrCancel );
        }
        
    iVisualLayoutUpdatedNotificationId = aNotificationId;
    }
    
// ---------------------------------------------------------------------------
// Return environment
// ---------------------------------------------------------------------------
//
CHuiEnv& CAlfSrvVisualOwner::Env() const
    {
    if ( iRealOwner )
        {
        return iRealOwner->Env();
        }
    return *iSession.SharedHuiEnv();
    }

// ---------------------------------------------------------------------------
// Return control group
// ---------------------------------------------------------------------------
//    
CHuiControlGroup* CAlfSrvVisualOwner::ControlGroup() const
    {
    if ( iRealOwner )
        {
        return iRealOwner->ControlGroup();
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// Return event handler of owner
// ---------------------------------------------------------------------------
//    
MHuiEventHandler* CAlfSrvVisualOwner::EventHandler()
    {
    if ( iRealOwner )
        {
        return iRealOwner->EventHandler();
        }
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// Append visual to owner
// ---------------------------------------------------------------------------
//    
void CAlfSrvVisualOwner::AppendL(CHuiVisual* aVisual)
    {
    if ( iRealOwner )
        {
        iRealOwner->AppendL(aVisual);
        }
    }
    
// ---------------------------------------------------------------------------
// Remove visual from owner
// ---------------------------------------------------------------------------
//    
void CAlfSrvVisualOwner::Remove(CHuiVisual* aVisual)
    {
    if ( iRealOwner )
        {
        iRealOwner->Remove(aVisual);
        }
    }
 
// ---------------------------------------------------------------------------
// Return if visual destroyed
// ---------------------------------------------------------------------------
//   
void CAlfSrvVisualOwner::VisualDestroyed(CHuiVisual& aVisual)
    {
    if ( iRealOwner )
        {
        iRealOwner->VisualDestroyed(aVisual);
        }
    }

// ---------------------------------------------------------------------------
// Return if visual layout updated
// ---------------------------------------------------------------------------
//    
void CAlfSrvVisualOwner::VisualLayoutUpdated(CHuiVisual& aVisual)
    {
    if ( iVisualLayoutUpdatedNotificationId != KErrNotFound )
        {
        iSession.CompleteCmd( iVisualLayoutUpdatedNotificationId , KErrNone );
        iVisualLayoutUpdatedNotificationId = KErrNotFound;
        }
    if ( iRealOwner )
        {
        iRealOwner->VisualLayoutUpdated(aVisual);
        }
    }

// ---------------------------------------------------------------------------
// Return if visual prepare draw failed
// ---------------------------------------------------------------------------
//    
void CAlfSrvVisualOwner::VisualPrepareDrawFailed(CHuiVisual& aVisual, TInt aErrorCode)
    {
    if ( iRealOwner )
        {
        iRealOwner->VisualPrepareDrawFailed(aVisual,aErrorCode);
        }
    }


