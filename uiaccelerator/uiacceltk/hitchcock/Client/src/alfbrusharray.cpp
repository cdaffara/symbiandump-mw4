/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Array for brushes
*
*/



#include "alf/alfbrusharray.h"
#include "alf/alfvisual.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfbrush.h"
#include "alflogger.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"

#include <uiacceltk/HuiUtil.h>

/**
 * This watcher is a helper class to detect misusages of the brush arrays.
 *
 * A classic example is that the user creates a brush which is deleted before
 * it is removed from all the arrays where it is added. 
 *
 * This idle-loop is started when the system notices when a brush is deleted which
 * is still in this array. This is only acceptable if the owning visual is deleted
 * in the same scheduler loop. See the CAlfBrushArrayUsageWatcher::RunL
 * for the result if the array is not deleted right away.
 */ 
NONSHARABLE_CLASS(CAlfBrushArrayUsageWatcher) : public CActive
    {
public:
    CAlfBrushArrayUsageWatcher();
    ~CAlfBrushArrayUsageWatcher();
protected:
    void RunL();
    void DoCancel(){};
    };

// Constructor, which initiates the idle-loop
CAlfBrushArrayUsageWatcher::CAlfBrushArrayUsageWatcher() : CActive( EPriorityIdle )
    {
    CActiveScheduler::Add( this );
    
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// destructor which cancels the loop    
CAlfBrushArrayUsageWatcher::~CAlfBrushArrayUsageWatcher()
    {
    Cancel();
    }

// RunL is called when the idle-loop is executed.
void CAlfBrushArrayUsageWatcher::RunL()
    {
    // In normal usage, this code snippet should never ne run.
    // This is called, when a user deletes a brush (directly or through a brush array)
    // which is still left in this brush array. 
    
    __ALFLOGSTRING( "CAlfBrushArrayUsageWatcher::RunL Incorrect brush deletion!" )
#ifdef _DEBUG
    USER_INVARIANT();
#endif
    User::Leave( KErrGeneral ); 
    }


// Private data structure.
struct CAlfBrushArray::TPrivateData
    {
    CAlfVisual* iOwner;
    RPointerArray<CAlfBrush> iBrushes;
    RPointerArray<CAlfBrush> iOwnedBrushes;
    CAlfBrushArrayUsageWatcher* iWatcher;
    };
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CAlfBrushArray::CAlfBrushArray()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAlfBrushArray::ConstructL(CAlfVisual& aOwner)
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iOwner = &aOwner;
    iData->iOwnedBrushes.Reset();
    iData->iBrushes.Reset();
    iData->iWatcher = NULL;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAlfBrushArray* CAlfBrushArray::NewL(CAlfVisual& aOwner)
    {
    CAlfBrushArray* self = new( ELeave ) CAlfBrushArray;
    CleanupStack::PushL( self );
    self->ConstructL(aOwner);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CAlfBrushArray::~CAlfBrushArray()
    {
    if ( iData )
        {
        Reset();
        }
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Resets the array.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBrushArray::Reset()
    {
    for ( TInt i = iData->iBrushes.Count()-1 ; i>= 0; i-- )
        {
        iData->iBrushes[i]->RemoveContainingArray( *this );
        }

    iData->iOwnedBrushes.ResetAndDestroy();
    iData->iBrushes.Reset();
    
    delete iData->iWatcher;
    iData->iWatcher= NULL;
    
    // Update the server side.
    TBuf8<1> dummy;   
    TInt err = iData->iOwner->Comms()->DoSynchronousCmd(EAlfVisualBrushArrayReset, KNullDesC8(), dummy); 

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfBrushArray::Reset panic error %d", err )
        USER_INVARIANT();
        }

    }

// ---------------------------------------------------------------------------
// Add brush onto the array.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBrushArray::AppendL(CAlfBrush* aBrush, TAlfOwnership aOwnership)
    {
    // Add to iBrushes array.
    TInt err = iData->iBrushes.Append( aBrush );
    if ( err != KErrNone )
        {
        // on failure, log'n'leave
        __ALFLOGSTRING1( "CAlfBrushArray::AppendL leave error %d", err )
        User::Leave( err );
        }
        
    err = aBrush->AppendContainingArray( *this );
    if ( err != KErrNone )
        {
        // on failure remove from iBrushes, log'n'leave
        iData->iBrushes.Remove( iData->iBrushes.Count() - 1 );
        iData->iBrushes.Compress();
        __ALFLOGSTRING1( "CAlfBrushArray::AppendL leave error %d", err )
        User::Leave( err );
        }

    // Update the server side.
    TInt2 int2(aBrush->Identifier(), aOwnership); 
    TPckgC<TInt2> buf(int2);
    TBuf8<1> dum;

    err = iData->iOwner->Comms()->DoSynchronousCmd(EAlfVisualBrushArrayAppend, buf, dum ); 
    if ( err != KErrNone )
        {
        // On error, remove it from the iBrushes and log'n'leave
        RemoveBrush(aBrush, iData->iBrushes.Count() - 1);        
        
        __ALFLOGSTRING1( "CAlfBrushArray::AppendL leave error %d", err )
        User::Leave(err);
        }
    // append ownership only on success (otherwise double deletion takes place on leave)
    if ( aOwnership == EAlfHasOwnership )
        {
        err = iData->iOwnedBrushes.Append( aBrush );
        if ( err != KErrNone )
            {
            // on failure remove brush (inform server as well) and log'n'leave
            Remove( iData->iBrushes.Count() - 1);
            __ALFLOGSTRING1( "CAlfBrushArray::AppendL leave error %d", err )
            User::Leave( err );
            }
        }
    }

// ---------------------------------------------------------------------------
// Insert
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBrushArray::InsertL(TInt aPos, CAlfBrush* aBrush, TAlfOwnership aOwnership)
    {
    // Add to iBrushes array.
    TInt err = iData->iBrushes.Insert( aBrush, aPos );
    if ( err != KErrNone )
        {
        // on failure, log'n'leave
        __ALFLOGSTRING1( "CAlfBrushArray::InsertL leave error %d", err )
        User::Leave( err );
        }
        
    err = aBrush->AppendContainingArray( *this );
    if ( err != KErrNone )
        {
        // on failure remove from iBrushes, log'n'leave
        iData->iBrushes.Remove( aPos );
        iData->iBrushes.Compress();
        __ALFLOGSTRING1( "CAlfBrushArray::InsertL leave error %d", err )
        User::Leave( err );
        }
        
    // Update the server side.
    TInt3 int3(aBrush->Identifier(), aOwnership, aPos); 
    TPckgC<TInt3> buf(int3);
    TBuf8<1> dum;

    err = iData->iOwner->Comms()->DoSynchronousCmd(EAlfVisualBrushArrayInsert, buf, dum ); 
    if ( err != KErrNone )
        {
        // On error, remove it from the iBrushes and log'n'leave 
        RemoveBrush(aBrush, aPos);
        
        __ALFLOGSTRING1( "CAlfBrushArray::InsertL leave error %d", err )
        User::Leave(err);
        }
    
    // append ownership only on success (otherwise double deletion takes place on leave)    
    if ( aOwnership == EAlfHasOwnership )
        {
        err = iData->iOwnedBrushes.Append( aBrush );
        if ( err != KErrNone )
            {
            // on failure remove brush (inform server as well) and log'n'leave
            Remove( aPos );
            __ALFLOGSTRING1( "CAlfBrushArray::InsertL leave error %d", err )
            User::Leave( err );
            }
        }
    }
  
// ---------------------------------------------------------------------------
// Remove
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfBrushArray::Remove(TInt aPos)
    {
    // Update the server side.
    TPckgC<TInt> buf(aPos);
    TBuf8<1> dum;

    TInt err = iData->iOwner->Comms()->DoSynchronousCmd(EAlfVisualBrushArrayRemove, buf, dum ); 
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfBrushArray::Remove panic error %d", err )
        USER_INVARIANT();
        }
    
    RemoveBrush(iData->iBrushes[aPos], aPos);
    }
  
// ---------------------------------------------------------------------------
// Brush count
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfBrushArray::Count() const
    {
    if ( iData )
        {
        return iData->iBrushes.Count();
        }
    return 0;
    }
  
// ---------------------------------------------------------------------------
// []
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfBrush& CAlfBrushArray::operator [] (TInt aPos)
    {
    return At(aPos);
    }

// ---------------------------------------------------------------------------
// At
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBrush& CAlfBrushArray::At(TInt aPos)
    {
    return *iData->iBrushes[aPos];
    }


// ---------------------------------------------------------------------------
// Remove brush and owned brushes if they exist
// ---------------------------------------------------------------------------
//  
void CAlfBrushArray::RemoveBrush(CAlfBrush* aBrush, TInt aPosIndex)
    {
    aBrush->RemoveContainingArray( *this );
            
    const TInt ownedIndex = iData->iOwnedBrushes.Find( aBrush );
    if ( ownedIndex != KErrNotFound )
        {
        iData->iOwnedBrushes.Remove( ownedIndex );
        iData->iOwnedBrushes.Compress();
        delete aBrush;
        }
    
    if ( aPosIndex != KErrNotFound )
        {
        iData->iBrushes.Remove( aPosIndex );
        iData->iBrushes.Compress();
        }
    }

// ---------------------------------------------------------------------------
// Brush is been deleted. Remove it from the array (if exists - might be several times).
// ---------------------------------------------------------------------------
// 
void CAlfBrushArray::HandleBrushDestroyed( CAlfBrush& aBrush )
    {
    for ( TInt index = Count()-1 ; index>=0 ; index-- )
        {
        if ( &At(index) == &aBrush )
            {
            // Update the server side.
            TPckgC<TInt> buf(index);
            TBuf8<1> dum;
            TInt err = iData->iOwner->Comms()->DoSynchronousCmd(EAlfVisualBrushArrayRemove, buf, dum ); 
            if ( err )
                {
                __ALFLOGSTRING1( "CAlfBrushArray::HandleBrushDestroyed panic error %d", err )
                USER_INVARIANT();
                }
            
            // Check the owned brushes - should never found!
            const TInt ownedIndex = iData->iOwnedBrushes.Find( &aBrush );
            __ASSERT_ALWAYS( ownedIndex == KErrNotFound, USER_INVARIANT() );
            
            // Remove from iBrushes array
            iData->iBrushes.Remove( index );
            iData->iBrushes.Compress();
            
            // create watcher
            if ( !iData->iWatcher )
                {
                // do not mind the OOM
                iData->iWatcher = new CAlfBrushArrayUsageWatcher;
                }
            }
        }
    }


