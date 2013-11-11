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
* Description:   Factory for visuals and layouts.
*
*/




#include "alf/alfvisualfactory.h"  // Class definition
#include "alf/alftextvisual.h"
#include "alf/alflcttextvisual.h"
#include "alf/alfanchorlayout.h"
#include "alf/alfviewportlayout.h"
#include "alf/alfdecklayout.h"
#include "alf/alfflowlayout.h"
#include "alf/alfimagevisual.h"
#include "alf/alfgridlayout.h"
#include "alf/alflctgridlayout.h"
#include "alf/alfcurvepathlayout.h"
#include "alf/alfconstants.h"
#include "alf/alflinevisual.h"
#include "alf/alflctanchorlayout.h"
#include "alf/alfmeshvisual.h"
#include "alf/alfcanvasvisual.h"
#include "alfuids.h"
#include "alf/alfcontrol.h"

// ---------------------------------------------------------------------------
// Creates new visual
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual* AlfVisualFactory::NewVisualL(
        TAlfVisualType aVisualType,
	    CAlfLayout* aParentLayout,
	    CAlfControl& aOwner,
	    CAlfEnv& aEnv,
	    TInt aImplementationUid)
    {
    CAlfVisual* visual = NewVisualLC(aVisualType, aParentLayout, aOwner, aEnv, aImplementationUid);
    CleanupStack::Pop(visual);
    return visual;
    }
    
// ---------------------------------------------------------------------------
// Creates new visual
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfVisual* AlfVisualFactory::NewVisualLC(
        TAlfVisualType aVisualType,
	    CAlfLayout* aParentLayout,
	    CAlfControl& aOwner,
	    CAlfEnv& /*aEnv*/,
	    TInt aImplementationUid)
	{
    CAlfVisual* visual = NULL;
    TInt mappedType = KErrNotFound;
    
    switch(aVisualType)
        {
        case EAlfVisualTypeVisual:
            {
            if (aImplementationUid == 0)
                mappedType = EAlfVisualCreate;
            visual = new (ELeave) CAlfVisual();
            break;
            }
            
        case EAlfVisualTypeLine:
            if (aImplementationUid == 0)
                mappedType = EAlfLineVisualCreate;

            visual = new (ELeave) CAlfLineVisual();
            break;

        /*case EAlfVisualTypeGradient:
            visual = new (ELeave) CAlfGradientVisual();
            break;*/

        case EAlfVisualTypeText:
            {
            if (aImplementationUid == 0)
                mappedType = EAlfTextVisualCreate;

            visual = new (ELeave) CAlfTextVisual();
            break;    
            }

#ifdef RD_ALF_IN_PLATFORM            
        case EAlfVisualTypeLCTText:
            {
            if (aImplementationUid == 0)
                mappedType = EAlfLCTTextVisualCreate;

            visual = new (ELeave) CAlfLCTTextVisual();
            break;    
            }
#endif // RD_ALF_IN_PLATFORM
            
        case EAlfVisualTypeImage:
            {
            if (aImplementationUid == 0)
                mappedType = EAlfImageVisualCreate;
            
            visual = new (ELeave) CAlfImageVisual();
            break;
            }
            
        case EAlfVisualTypeMesh:
            if (aImplementationUid == 0)
                mappedType = EAlfMeshVisualCreate;
            
            visual = new (ELeave) CAlfMeshVisual();
            break;
        
        case EAlfVisualTypeCanvas:
            if (aImplementationUid == 0)
                mappedType = EAlfCanvasVisualCreate;
            
            visual = new (ELeave) CAlfCanvasVisual();
            break;
            
        
        default: // todo: should we leave or should extensions to be created via here?
            User::Leave(KErrNotFound);
        }

    CleanupStack::PushL(visual);
    
    ASSERT(aOwner.Identifier());
    
    TInt parentlayouthandle = 0;
    if (aParentLayout)
        {
        parentlayouthandle = aParentLayout->Identifier();
        }
    
    TInt2 inputParams(aOwner.Identifier(), parentlayouthandle);
   
    TPckg<TInt2> paramBuf(inputParams);
    
    visual->ConstructL( aOwner, 
                        (mappedType==KErrNotFound)?aVisualType:mappedType, 
                        aImplementationUid, paramBuf );

    return visual;    
    }
    
// ---------------------------------------------------------------------------
// Creates new layout - TODO: Is this obsolete ?
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfLayout* AlfVisualFactory::NewLayoutL(
        TAlfLayoutType aLayoutType,
	    CAlfLayout* aParentLayout,
	    CAlfControl& aOwner,
	    CAlfEnv& aEnv,
	    TInt aImplementationUid)
    {
    CAlfLayout* layout = NewLayoutLC(aLayoutType, aParentLayout, aOwner, aEnv, aImplementationUid);
    CleanupStack::Pop(layout);
    return layout;
    }
    
// ---------------------------------------------------------------------------
// Creates new layout
// ---------------------------------------------------------------------------
//      
EXPORT_C CAlfLayout* AlfVisualFactory::NewLayoutLC(	    
        TAlfLayoutType aLayoutType,
	    CAlfLayout* aParentLayout,
	    CAlfControl& aOwner,
	    CAlfEnv& /*aEnv*/,
	    TInt aImplementationUid )
    {
    CAlfLayout* layout = NULL;
    TInt mappedType = KErrNotFound; 
    
    switch(aLayoutType)
        {
        case EAlfLayoutTypeLayout:
            if (aImplementationUid == 0)
                mappedType = EAlfLayoutCreate;
            layout = new (ELeave) CAlfLayout();
            break;
                  
        case EAlfLayoutTypeGrid:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfGridLayoutCreate;
                }
            layout = new (ELeave) CAlfGridLayout();
            break;

#ifdef RD_ALF_IN_PLATFORM            
        case EAlfLayoutTypeLCTGrid:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfLCTGridLayoutCreate;
                }                
            layout = new (ELeave) CAlfLCTGridLayout();
            break;
#endif // RD_ALF_IN_PLATFORM            
            
            
        case EAlfLayoutTypeAnchor:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfAnchorLayoutCreate;
                }                
            layout = new (ELeave) CAlfAnchorLayout();
            break;

#ifdef RD_ALF_IN_PLATFORM            
        case EAlfLayoutTypeLCTAnchor:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfLCTAnchorLayoutCreate;
                }                
            layout = new (ELeave) CAlfLCTAnchorLayout();
            break;
#endif // RD_ALF_IN_PLATFORM            
            
        case EAlfLayoutTypeDeck:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfDeckLayoutCreate;
                }                
            layout = new (ELeave) CAlfDeckLayout();
            break;
            
        case EAlfLayoutTypeFlow:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfFlowLayoutCreate;
                }                
            layout = new (ELeave) CAlfFlowLayout();
            break;
            
        case EAlfLayoutTypeCurvePath:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfCurvePathLayoutCreate;
                }                
            layout = new (ELeave) CAlfCurvePathLayout();
            break;
            
        case EAlfLayoutTypeViewport:
            if (aImplementationUid == 0)
                {
                mappedType = EAlfViewportLayoutCreate;
                }                
            layout = new (ELeave) CAlfViewportLayout();
            break;            
            
        default:
            User::Leave(KErrNotFound);
        }

    CleanupStack::PushL(layout);

    ASSERT(aOwner.Identifier());
    
    TInt parentlayouthandle = 0;
    if (aParentLayout)
        {
        parentlayouthandle = aParentLayout->Identifier();
        }
    
    TInt2 inputParams(aOwner.Identifier(), parentlayouthandle);
   
    TPckg<TInt2> paramBuf(inputParams);

    layout->CAlfVisual::ConstructL( aOwner, 
                        (mappedType==KErrNotFound)?aLayoutType:mappedType, 
                        aImplementationUid, paramBuf );
    return layout;    
    }
    
