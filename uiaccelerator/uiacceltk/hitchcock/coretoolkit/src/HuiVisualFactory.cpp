/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include "HuiVisualFactory.h"  // Class definition
#include "uiacceltk/HuiLayouts.h"
#include "uiacceltk/HuiVisuals.h"
#include "uiacceltk/HuiControl.h"

CHuiVisualFactory::CHuiVisualFactory(CHuiEnv& aEnv)
        : iEnv(aEnv)
    {
    }


	
CHuiVisualFactory::~CHuiVisualFactory()
    {    
    }


CHuiVisual* CHuiVisualFactory::NewVisualL(THuiVisualType aVisualType,
                                          CHuiControl& aOwner) const
    {
    CHuiVisual* visual = NewVisualLC(aVisualType, aOwner);
    CleanupStack::Pop(visual);
    return visual;
    }
    
    
CHuiVisual* CHuiVisualFactory::NewVisualLC(THuiVisualType aVisualType,
                                           CHuiControl& aOwner) const
    {
    CHuiVisual* visual = NULL;
    
    switch(aVisualType)
        {
        case EHuiVisualTypeVisual:
            visual = new (ELeave) CHuiVisual(aOwner);
            break;
            
        case EHuiVisualTypeLine:
            visual = new (ELeave) CHuiLineVisual(aOwner);
            break;

        /*case EHuiVisualTypeGradient:
            visual = new (ELeave) CHuiGradientVisual(aOwner);
            break;*/

        case EHuiVisualTypeText:
            visual = new (ELeave) CHuiTextVisual(aOwner);
            break;

        case EHuiVisualTypeImage:
            visual = new (ELeave) CHuiImageVisual(aOwner);
            break;
        
        case EHuiVisualTypeMesh:
            visual = new (ELeave) CHuiMeshVisual(aOwner);
            break;
            
        case EHuiVisualTypeCanvas:
            visual = new (ELeave) CHuiCanvasVisual(aOwner);
            break;
            
            
        default:
            User::Leave(KErrNotFound);
        }

    CleanupStack::PushL(visual);
    visual->ConstructL();
    return visual;    
    }
    
    
CHuiLayout* CHuiVisualFactory::NewLayoutL(THuiLayoutType aLayoutType,
                                          CHuiControl& aOwner) const
    {
    CHuiLayout* layout = NewLayoutLC(aLayoutType, aOwner);
    CleanupStack::Pop(layout);
    return layout;
    }
    
    
CHuiLayout* CHuiVisualFactory::NewLayoutLC(THuiLayoutType aLayoutType,
                                           CHuiControl& aOwner) const
    {
    CHuiLayout* layout = NULL;
    
    switch(aLayoutType)
        {
        case EHuiLayoutTypeLayout:
            layout = new (ELeave) CHuiLayout(aOwner);
            break;
        
        /*case EHuiLayoutTypeApp:
            layout = new (ELeave) CHuiAppLayout(aOwner);
            break;*/
            
        case EHuiLayoutTypeGrid:
            layout = new (ELeave) CHuiGridLayout(aOwner);
            break;
            
        case EHuiLayoutTypeFlow:
            layout = new (ELeave) CHuiFlowLayout(aOwner);
            break;
            
        case EHuiLayoutTypeAnchor:
            layout = new (ELeave) CHuiAnchorLayout(aOwner);
            break;
            
        default:
            User::Leave(KErrNotFound);
        }

    CleanupStack::PushL(layout);
    layout->ConstructL();
    return layout;    
    }
    
