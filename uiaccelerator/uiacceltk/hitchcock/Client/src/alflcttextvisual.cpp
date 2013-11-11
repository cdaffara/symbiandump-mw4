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
* Description:   LCT Text visual
*
*/



#include "alf/alflcttextvisual.h"
#include "alf/alfcontrol.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"
#include "alfuids.h"

#include <uiacceltk/HuiUtil.h>

#ifdef RD_ALF_IN_PLATFORM
#include <aknlayout2hierarchy.h>
#endif

struct CAlfLCTTextVisual::TLCTTextVisualPrivateData
    {
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTTextVisual::CAlfLCTTextVisual()
    :
    iLCTTextVisualData(NULL)
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLCTTextVisual::ConstructL(CAlfControl& aOwner)
    {
    CAlfTextVisual::ConstructL(aOwner);
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTTextVisual::~CAlfLCTTextVisual()
    {
    delete iLCTTextVisualData;
    iLCTTextVisualData = NULL;
    }


// ---------------------------------------------------------------------------
// Creates new CAlfLCTTextVisual
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLCTTextVisual* CAlfLCTTextVisual::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout)
    {
    CAlfLCTTextVisual* text = STATIC_CAST(CAlfLCTTextVisual*,
        aOwnerControl.AppendVisualL(EAlfVisualTypeLCTText, aParentLayout));
    return text;
    }
    


// ---------------------------------------------------------------------------
// Sets the layout component handle for the text pane
// ---------------------------------------------------------------------------
//
#ifdef RD_ALF_IN_PLATFORM            
EXPORT_C void CAlfLCTTextVisual::SetTextPane(TAknLayoutHierarchyComponentHandle& aComponentHandle)
    {
    TAlfLCTTextVisualSetTextPaneParams params;
    
    params.iApiId = aComponentHandle.ApiId();
    params.iComponentId = aComponentHandle.ComponentId();
    params.iOptionIndex = 0; // not used yet
    params.iDrawingOrderIndex = 0;  // not used yet
    params.iVarietyIndex = aComponentHandle.VarietyIndex();
    params.iColumn = aComponentHandle.Column();
    params.iRow = aComponentHandle.Row();
    
    TPckg<TAlfLCTTextVisualSetTextPaneParams> paramsPckg(params);

    TInt err = Comms()->DoCmdNoReply(EAlfLCTTextVisualSetTextPane, paramsPckg );
    if ( err == KErrNone )
        {
        __ALFLOGSTRING( "LCTTextVisualSetTextPane ok" )
        }
    else
        {
        __ALFLOGSTRING1( "LCTTxtVisualSetTextPane return error %d", err )
        USER_INVARIANT(); 
        }
    }
#else // RD_ALF_IN_PLATFORM            
EXPORT_C void CAlfLCTTextVisual::SetTextPane(TAknLayoutHierarchyComponentHandle& /*aComponentHandle*/)
    {
    }
#endif // RD_ALF_IN_PLATFORM            
    

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfLCTTextVisual::RemoveAndDestroyAllD()
    {
    CAlfTextVisual::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTTextVisual::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfTextVisual::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfLCTTextVisual::FindTag(const TDesC8& aTag)
    {
    return CAlfTextVisual::FindTag( aTag );
    }


// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTTextVisual::DoRemoveAndDestroyAllD()
    {
    CAlfTextVisual::DoRemoveAndDestroyAllD();
    }
    

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLCTTextVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfTextVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    
// end of file
