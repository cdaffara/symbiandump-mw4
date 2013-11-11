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
* Description:   Viewport layout
*
*/



#include "alf/alfviewportlayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfViewportLayout::CAlfViewportLayout()
    : CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfViewportLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfViewportLayout::~CAlfViewportLayout()
    {
    }


// ---------------------------------------------------------------------------
// Creates new anchor layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfViewportLayout* CAlfViewportLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfViewportLayout* layout = STATIC_CAST(CAlfViewportLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeViewport, aParentLayout));
    return layout;
    }

// ---------------------------------------------------------------------------
// Set virtual size of view area.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfViewportLayout::SetVirtualSize(const TAlfRealSize& aSize, TInt aTransitionTime)
    {
    TAlfTimedPoint safeSize(aSize.iWidth, aSize.iHeight, aTransitionTime);
    safeSize.SetStyle(EAlfTimedValueStyleLinear);

    TPckgC<TAlfTimedPoint> pckg(safeSize);
    
    TInt err = Comms()->DoCmdNoReply(EAlfViewportLayoutSetVirtualSize, pckg );                    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfViewportLayout::SetVirtualSize ignore error %d", err )
        }  
        
    // No need to update children. That is done on the server side.
    }

// ---------------------------------------------------------------------------
// Sets viewport size in virtual units.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfViewportLayout::SetViewportSize(const TAlfRealSize& aSize, TInt aTransitionTime)
    {
    TAlfTimedPoint safeSize(aSize.iWidth, aSize.iHeight, aTransitionTime);
    safeSize.SetStyle(EAlfTimedValueStyleLinear);

    TPckgC<TAlfTimedPoint> pckg(safeSize);

    TInt err = Comms()->DoCmdNoReply(EAlfViewportLayoutSetViewportSize, pckg );                    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfViewportLayout::SetViewportSize ignore error %d", err )
        }      
        
    // No need to update children. That is done on the server side.
    }

// ---------------------------------------------------------------------------
// Sets viewport pos in virtual units.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfViewportLayout::SetViewportPos(const TAlfRealPoint& aPos, TInt aTransitionTime)
    {
    TAlfTimedPoint safeSize(aPos.iX, aPos.iY, aTransitionTime);
    safeSize.SetStyle(EAlfTimedValueStyleLinear);    

    TPckgC<TAlfTimedPoint> pckg(safeSize);
    
    TInt err = Comms()->DoCmdNoReply(EAlfViewportLayoutSetViewportPos, pckg );                    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfViewportLayout::SetViewportPos ignore error %d", err )
        }     
        
    // No need to update children. That is done on the server side.
    }


// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfViewportLayout::RemoveAndDestroyAllD()
    {
    CAlfLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfViewportLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfViewportLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfViewportLayout::DoRemoveAndDestroyAllD()
    {
    CAlfLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfViewportLayout::BaseUnit() const
    {
    return CAlfLayout::BaseUnit();
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfViewportLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
   
