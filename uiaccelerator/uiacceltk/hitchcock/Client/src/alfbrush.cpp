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
* Description:   Base class for brushes
*
*/



#include "alf/alfbrush.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alf/alfbrusharray.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

// Private data structure
struct CAlfBrush::TPrivateData
    {
    CAlfGenComponent* iComms;   // owned 
    TAlfTimedValue iOpacity;
    TBool iClipToVisual;
    
    // Array keeps track of all the brush arrays in which this brush instance
    // has been added on. Duplicates are allowed.
    RPointerArray<CAlfBrushArray> iContainingBrushArrays;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBrush::CAlfBrush()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBrush::ConstructL( 
        CAlfEnv& aEnv,
        TInt aImplementationId, 
        TInt aImplementationUid, 
        const TDesC8& aConstructionParams )
    {
    ConstructL();
   
    if (!iData->iComms)
        {
        iData->iComms = CAlfGenComponent::NewL(
            aEnv,
            aImplementationId, 
            aImplementationUid, 
            aConstructionParams);         
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBrush::ConstructL()
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    iData->iClipToVisual = EFalse;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBrush::~CAlfBrush()
    {
    if ( iData )
        {
        const TInt containingBrushArrayCount = iData->iContainingBrushArrays.Count();
        if ( containingBrushArrayCount )
            {
            __ALFLOGSTRING1( "CAlfBrush::~CAlfBrush Warning: deleted brush still in %d arrays", containingBrushArrayCount )
            }
        
        // Remove from containing arrays
        for ( TInt i = containingBrushArrayCount-1 ; i>=0 ; i-- )
            {
            iData->iContainingBrushArrays[i]->HandleBrushDestroyed(*this);
            }
        
        iData->iContainingBrushArrays.Close();
        
        delete iData->iComms;
        }
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C const TAlfTimedValue& CAlfBrush::Opacity() const
    {
    TPckg<TAlfTimedValue> buf(iData->iOpacity);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfBrushGetOpacity, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBrush::Opacity panic error %d", err )
        USER_INVARIANT();
        }
    
    return iData->iOpacity;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBrush::SetOpacity( const TAlfTimedValue& aOpacity )
    {
    TPckgC<TAlfTimedValue> buf(aOpacity);

    TInt err = iData->iComms->DoCmdNoReply( EAlfBrushSetOpacity, buf );

    if ( err == KErrNone )
        {
        iData->iOpacity = aOpacity;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfBrush::SetOpacity panic error %d", err )
        USER_INVARIANT();
        }
    }
 
// ---------------------------------------------------------------------------
// Returns server handle. 0 if not set.
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfBrush::Identifier() const
    {
    return iData->iComms->Identifier();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfBrush::SetLayer(TAlfBrushLayer aLayer)
    {
    TPckgC<TAlfBrushLayer> buf(aLayer);

    TInt err = iData->iComms->DoCmdNoReply( EAlfBrushSetLayer, buf );

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfBrush::SetLayer panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfGenComponent* CAlfBrush::Comms() const
    {
    ASSERT(iData);
    return iData->iComms;
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfBrush::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfPropertyOwner::PropertyOwnerExtension(aExtensionUid, aExtensionParams);
    }


// ---------------------------------------------------------------------------
// Append a brush array which holds this brush
// ---------------------------------------------------------------------------
// 
TInt CAlfBrush::AppendContainingArray( CAlfBrushArray& aContainingArray )
    {
    __ASSERT_ALWAYS( iData , USER_INVARIANT() );
    
    // allow duplicates, because a brush can be in the same array many times.
    return iData->iContainingBrushArrays.Append( &aContainingArray );
    }

// ---------------------------------------------------------------------------
// Removes a brush array which holds this brush
// ---------------------------------------------------------------------------
// 
void CAlfBrush::RemoveContainingArray( CAlfBrushArray& aContainingArray )
    {
    __ASSERT_ALWAYS( iData , USER_INVARIANT() );
    
    for ( TInt i = iData->iContainingBrushArrays.Count()-1 ; i>=0 ; i-- )
        {
        if ( iData->iContainingBrushArrays[i] == &aContainingArray )
            {
            // remove one instance only once becuase it might be on the same array
            // several times
            iData->iContainingBrushArrays.Remove(i);
            iData->iContainingBrushArrays.Compress();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfBrush::SetClipToVisual(TBool aClipToVisual)
    {
    TPckgC<TBool> buf(aClipToVisual);

    TInt err = iData->iComms->DoCmdNoReply( EAlfBrushSetClipToVisual, buf );

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfBrush::SetClipToVisual panic error %d", err )
        USER_INVARIANT();
        }        
    }
    
// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CAlfBrush::ClipToVisual() const
    {
    TPckg<TBool> buf(iData->iClipToVisual);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfBrushClipToVisual, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBrush::ClipToVisual panic error %d", err )
        USER_INVARIANT();
        }
    
    return iData->iClipToVisual;
        
    }

