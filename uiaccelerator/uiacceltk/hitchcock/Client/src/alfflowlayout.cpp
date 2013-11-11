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
* Description:   Flow layout
*
*/



#include "alf/alfflowlayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

// Private structure
struct CAlfFlowLayout::TFlowLayoutPrivateData
    {
    TInt iMode; // flow mode flags
    };


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFlowLayout::CAlfFlowLayout()
    : CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFlowLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);
    
    iFlowLayoutData = new (ELeave) TFlowLayoutPrivateData;
    iFlowLayoutData->iMode = 0;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFlowLayout::~CAlfFlowLayout()
    {
    delete iFlowLayoutData;
    iFlowLayoutData = NULL;
    }


// ---------------------------------------------------------------------------
// Creates new anchor layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFlowLayout* CAlfFlowLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfFlowLayout* layout = STATIC_CAST(CAlfFlowLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeFlow, aParentLayout));
    return layout;
    }

EXPORT_C void CAlfFlowLayout::SetFlowDirection(TFlowDirection aDirection)
    {
    TPckgC<TFlowDirection> inBuf(aDirection);

    TInt err = Comms()->DoCmdNoReply( EAlfFlowLayoutSetFlowDirection, inBuf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFlowLayout::SetFlowDirection panic error %d", err )
        USER_INVARIANT();
        }
    }
    
EXPORT_C void CAlfFlowLayout::SetCentering(TBool aCentering)
    {
    TPckgC<TBool> inBuf(aCentering);

    TInt err = Comms()->DoCmdNoReply( EAlfFlowLayoutSetCentering, inBuf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFlowLayout::SetCentering panic error %d", err )
        USER_INVARIANT();
        }
        
	// Update client side data
    if(aCentering)
        {
        iFlowLayoutData->iMode |= EModeCenterPerpendicular;
        }
    else
        {
        iFlowLayoutData->iMode &= ~EModeCenterPerpendicular;
        }
        
    }

EXPORT_C void CAlfFlowLayout::SetMode(TInt aMode)
    {
    TPckgC<TInt> inBuf(aMode);

    TInt err = Comms()->DoCmdNoReply( EAlfFlowLayoutSetMode, inBuf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFlowLayout::SetMode panic error %d", err )
        USER_INVARIANT();
        }
        
    // Update local copy of data.
    iFlowLayoutData->iMode = aMode;        
    }
    
EXPORT_C TInt CAlfFlowLayout::Mode() const
    {
    return iFlowLayoutData->iMode;    
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfFlowLayout::RemoveAndDestroyAllD()
    {
    CAlfLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfFlowLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfFlowLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfLayout::FindTag( aTag );
    }
// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfFlowLayout::BaseUnit() const
    {
    return CAlfLayout::BaseUnit();
    }
 
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfFlowLayout::DoRemoveAndDestroyAllD()
    {
    CAlfLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfFlowLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    


