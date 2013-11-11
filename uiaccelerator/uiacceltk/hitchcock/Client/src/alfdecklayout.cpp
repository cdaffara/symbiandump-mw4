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
* Description:   Deck layout
*
*/



#include "alf/alfdecklayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"

struct CAlfDeckLayout::TDeckLayoutPrivateData
    {
    // Add member variables here.
    };

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDeckLayout::CAlfDeckLayout()
    : CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDeckLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);
    
    // Allocate iDeckLayoutData here if it gets member data.
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDeckLayout::~CAlfDeckLayout()
    {
    delete iDeckLayoutData;
    iDeckLayoutData = NULL;
    }


// ---------------------------------------------------------------------------
// Creates new anchor layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDeckLayout* CAlfDeckLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfDeckLayout* layout = STATIC_CAST(CAlfDeckLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeDeck, aParentLayout));
    return layout;
    }


// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfDeckLayout::RemoveAndDestroyAllD()
    {
    CAlfLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfDeckLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfDeckLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfDeckLayout::BaseUnit() const
    {
    return CAlfLayout::BaseUnit();
    }
 
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfDeckLayout::DoRemoveAndDestroyAllD()
    {
    CAlfLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfDeckLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }

