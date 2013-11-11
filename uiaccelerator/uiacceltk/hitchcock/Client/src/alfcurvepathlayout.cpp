/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Layout which uses a curve path for the child positions
*
*/



#include "alf/alfcurvepathlayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfcurvepath.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"
#include "alf/alfconstants.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfCurvePathLayout::TCurvePathLayoutPrivateData
    {
    CAlfCurvePath* iCurvePath; // Own
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCurvePathLayout::CAlfCurvePathLayout()
: CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCurvePathLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);
    
    iCurvePathLayoutData = new (ELeave) TCurvePathLayoutPrivateData;
    iCurvePathLayoutData->iCurvePath = NULL;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCurvePathLayout* CAlfCurvePathLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfCurvePathLayout* layout = STATIC_CAST(CAlfCurvePathLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeCurvePath, aParentLayout));
    return layout;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCurvePathLayout::~CAlfCurvePathLayout()
    {
    if ( iCurvePathLayoutData )
        {
        delete iCurvePathLayoutData->iCurvePath;
        iCurvePathLayoutData->iCurvePath = NULL;
        }
    delete iCurvePathLayoutData;
    iCurvePathLayoutData = NULL;
    }
 
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfCurvePath& CAlfCurvePathLayout::CurvePath()
    {
    // The curve path cannot be contructed in the ConstructL() because
    // the comms is not allocated yet.
    if ( !iCurvePathLayoutData->iCurvePath )
        {
        // Create curve path. The curve path uses this subsesison for its
        // communication with the server side instance.
        TRAPD( err, iCurvePathLayoutData->iCurvePath = CAlfCurvePath::NewL( Env(), Identifier() ) );
        if ( err )
            {
            __ALFLOGSTRING1( "CAlfCurvePathLayout::CurvePath panic error %d", err )
            USER_INVARIANT();
            }
        }
    
    return *iCurvePathLayoutData->iCurvePath;
    }

EXPORT_C TInt CAlfCurvePathLayout::MappingFunctionIdentifier() const
    {
    return Comms()->Identifier();
    }
    
EXPORT_C TReal32 CAlfCurvePathLayout::MapValue(TReal32 aValue, TInt aMode) const __SOFTFP
    {
    TIntTReal mapValues( aMode, aValue );
    TPckgC<TIntTReal> mapValuesPckg( mapValues );
    
    TReal32 returnValue = aValue;
    TPckg<TReal32> returnBuf(returnValue);
    
    TInt err = Comms()->DoSynchronousCmd(
        EAlfCurvePathLayoutMapValue, 
        mapValuesPckg, 
        returnBuf );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePathLayout::MapValue panic error %d", err )
        USER_INVARIANT();
        }
    
    return returnValue;
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCurvePathLayout::RemoveAndDestroyAllD()
    {
    CAlfLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfCurvePathLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfCurvePathLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfCurvePathLayout::DoRemoveAndDestroyAllD()
    {
    CAlfLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfCurvePathLayout::BaseUnit() const
    {
    return CAlfLayout::BaseUnit();
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfCurvePathLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
   
