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
* Description:   LCT Grid Layout
*
*/



#include "alf/alflctgridlayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"
#include "alfuids.h"

#include <uiacceltk/HuiUtil.h>

#ifdef RD_ALF_IN_PLATFORM
#include <aknlayout2hierarchy.h>
#endif


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTGridLayout::CAlfLCTGridLayout()
    : CAlfGridLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLCTGridLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfGridLayout::ConstructL(aOwner);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTGridLayout::~CAlfLCTGridLayout()
    {
    }


// ---------------------------------------------------------------------------
// Creates new grid layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTGridLayout* CAlfLCTGridLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfLCTGridLayout* layout = STATIC_CAST(CAlfLCTGridLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeLCTGrid, aParentLayout));
    return layout;
    }

// ---------------------------------------------------------------------------
// Sets cols and rows
// ---------------------------------------------------------------------------
//
#ifdef RD_ALF_IN_PLATFORM            
EXPORT_C void CAlfLCTGridLayout::SetColsAndRows(
    TAknLayoutHierarchyComponentHandle& aComponentHandle,
    const TAlfTimedPoint& aOffset)
    {
    TAlfLCTGridSetColsAndRowsParams params;
    
    params.iApiId = aComponentHandle.ApiId();
    params.iComponentId = aComponentHandle.ComponentId();
    params.iOptionIndex = 0; // not used yet
    params.iDrawingOrderIndex = 0;  // not used yet
    params.iVarietyIndex = aComponentHandle.VarietyIndex();
    params.iColumn = aComponentHandle.Column();
    params.iRow = aComponentHandle.Row();
    
    params.iOffset = aOffset;
    
    TPckg<TAlfLCTGridSetColsAndRowsParams> paramsPckg(params);

    TInt err = Comms()->DoCmdNoReply(EAlfLCTGridLayoutSetColsAndRows, paramsPckg );
    if ( err == KErrNone )
        {
        __ALFLOGSTRING( "LCTGridLayoutSetColsAndRows ok" )
        }
    else
        {
        __ALFLOGSTRING1( "LCTGridLayoutSetColsAndRows return error %d", err )
        USER_INVARIANT(); 
        }
    }
#else // RD_ALF_IN_PLATFORM
EXPORT_C void CAlfLCTGridLayout::SetColsAndRows(TAknLayoutHierarchyComponentHandle& /*aComponentHandle*/, const TAlfTimedPoint& /*aOffset*/)
    {
    }
#endif // RD_ALF_IN_PLATFORM

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfLCTGridLayout::RemoveAndDestroyAllD()
    {
    CAlfGridLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTGridLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfGridLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfLCTGridLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfGridLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfLCTGridLayout::BaseUnit() const
    {
    return CAlfGridLayout::BaseUnit();
    }
 
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTGridLayout::DoRemoveAndDestroyAllD()
    {
    CAlfGridLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTGridLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfGridLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    
