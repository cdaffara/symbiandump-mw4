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
* Description:   Anchor layout
*
*/



#include "alf/alflctanchorlayout.h"
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
EXPORT_C CAlfLCTAnchorLayout::CAlfLCTAnchorLayout()
    : CAlfAnchorLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLCTAnchorLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTAnchorLayout::~CAlfLCTAnchorLayout()
    {
    }


// ---------------------------------------------------------------------------
// Creates new anchor layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTAnchorLayout* CAlfLCTAnchorLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfLCTAnchorLayout* layout = STATIC_CAST(CAlfLCTAnchorLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeLCTAnchor, aParentLayout));
    return layout;
    }

// ---------------------------------------------------------------------------
// Sets anchor
// ---------------------------------------------------------------------------
//
#ifdef RD_ALF_IN_PLATFORM            
EXPORT_C void CAlfLCTAnchorLayout::SetAnchors(
    TInt aOrdinal,
    TAknLayoutHierarchyComponentHandle& aComponentHandle,
    const TAlfTimedPoint& aOffset)
    {
    TAlfLCTAnchorSetAnchorsParams params;
    
    params.iOrdinal = aOrdinal;
    params.iApiId = aComponentHandle.ApiId();
    params.iComponentId = aComponentHandle.ComponentId();
    params.iOptionIndex = 0; // not used yet
    params.iDrawingOrderIndex = 0;  // not used yet
    params.iVarietyIndex = aComponentHandle.VarietyIndex();
    params.iColumn = aComponentHandle.Column();
    params.iRow = aComponentHandle.Row();
    
    params.iOffset = aOffset;
    
    TPckg<TAlfLCTAnchorSetAnchorsParams> paramsPckg(params);

    TInt err = Comms()->DoCmdNoReply(EAlfLCTAnchorLayoutSetAnchors, paramsPckg  );
    if ( err == KErrNone )
        {
        __ALFLOGSTRING( "LCTAnchorSetAnchors ok" )
        }
    else
        {
        __ALFLOGSTRING1( "LCTAnchorSetAnchors return error %d", err )
        USER_INVARIANT(); 
        }
    }
#else // RD_ALF_IN_PLATFORM
EXPORT_C void CAlfLCTAnchorLayout::SetAnchors(TInt /*aOrdinal*/, TAknLayoutHierarchyComponentHandle& /*aComponentHandle*/, const TAlfTimedPoint& /*aOffset*/)
    {
    }
#endif // RD_ALF_IN_PLATFORM


// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfLCTAnchorLayout::RemoveAndDestroyAllD()
    {
    CAlfAnchorLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTAnchorLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfAnchorLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfLCTAnchorLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfAnchorLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfLCTAnchorLayout::BaseUnit() const
    {
    return CAlfAnchorLayout::BaseUnit();
    }
 
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTAnchorLayout::DoRemoveAndDestroyAllD()
    {
    CAlfAnchorLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTAnchorLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfAnchorLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    



