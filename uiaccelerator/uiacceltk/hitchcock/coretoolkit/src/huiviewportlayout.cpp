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
* Description:   Viewport visual.
*
*/



#include "uiacceltk/huiviewportlayout.h"  // Class definition

#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiUtil.h>


EXPORT_C CHuiViewportLayout* CHuiViewportLayout::AddNewL(CHuiControl& aOwnerControl, CHuiLayout* aParentLayout)
    {
    CHuiViewportLayout* layout = new (ELeave) CHuiViewportLayout(aOwnerControl);
    CleanupStack::PushL(layout);
    layout->ConstructL();    
    aOwnerControl.AppendL(layout, aParentLayout);
    CleanupStack::Pop(layout);           
    return layout;    
    }


CHuiViewportLayout::CHuiViewportLayout(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner)
    {
    iVirtualSize = THuiRealSize(1.0f, 1.0f);
    iViewportSize = THuiRealSize(1.0f, 1.0f);  
    iViewportPos = THuiRealPoint(0.0f, 0.0f);
      
    iWrappingFlags = 0;
    }


void CHuiViewportLayout::ConstructL()
    {    
    CHuiLayout::ConstructL();
    SetClipping(ETrue);    
    }
    
CHuiViewportLayout::~CHuiViewportLayout()
    {    
    }

EXPORT_C void CHuiViewportLayout::SetVirtualSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    if ( !HuiUtil::RealCompare( iVirtualSize.iHeight, aSize.iHeight ) || 
         !HuiUtil::RealCompare( iVirtualSize.iWidth, aSize.iWidth ) )
        {   
        iVirtualSize = aSize;

        UpdateChildrenLayout(aTransitionTime);    
            
        SetChanged();
        }
    }

    
EXPORT_C void CHuiViewportLayout::SetViewportSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    // Divide by zero will be caught later.
    if ( !HuiUtil::RealCompare( iViewportSize.iHeight, aSize.iHeight ) || 
         !HuiUtil::RealCompare( iViewportSize.iWidth, aSize.iWidth ) )
        { 
        iViewportSize = aSize;
            
        UpdateChildrenLayout(aTransitionTime);            
            
        SetChanged();   
        }
    }

EXPORT_C void CHuiViewportLayout::SetViewportPos(const THuiRealPoint& aPos, TInt aTransitionTime)
    {
     if ( !HuiUtil::RealCompare( iViewportPos.iY, aPos.iY ) || 
         !HuiUtil::RealCompare( iViewportPos.iX, aPos.iX ) )
        { 
        iViewportPos = aPos;
        
        UpdateChildrenLayout(aTransitionTime);        
        
        SetChanged();
        }
    }

void CHuiViewportLayout::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    UpdateChildrenLayout(aTransitionTime);    
    }
    

TBool CHuiViewportLayout::ChildSize(TInt /*aOrdinal*/, TSize& aSize)
    {
    THuiRealSize mysize = Size().Target().AsSize();
    
    if ((iViewportSize.iWidth == 0.0) || (iViewportSize.iHeight == 0.0))
        {
        // Could not obtain new size because of divide by zero error.
        return EFalse;            
        }
    
    aSize.iWidth = HUI_ROUND_FLOAT_TO_INT((iVirtualSize.iWidth / iViewportSize.iWidth) * mysize.iWidth);
    aSize.iHeight = HUI_ROUND_FLOAT_TO_INT((iVirtualSize.iHeight / iViewportSize.iHeight) * mysize.iHeight);

    return ETrue;
    }


TBool CHuiViewportLayout::ChildPos(TInt /*aOrdinal*/, TPoint& aPos)
    {
    THuiRealSize mysize = Size().RealTarget().AsSize();

    if ((iViewportSize.iWidth == 0.0) || (iViewportSize.iHeight == 0.0))
        {
        // Could not obtain new size because of divide by zero error.
        return EFalse;            
        }
         
    aPos.iX = HUI_ROUND_FLOAT_TO_INT( -iViewportPos.iX * mysize.iWidth / iViewportSize.iWidth );
    aPos.iY = HUI_ROUND_FLOAT_TO_INT( -iViewportPos.iY * mysize.iHeight / iViewportSize.iHeight );
    
    return ETrue;
    }

EXPORT_C TInt CHuiViewportLayout::ChildRect(TInt /*aOrdinal*/, THuiRealRect& /*aPos*/)
    {
    return THuiLayoutChildRectNotImplemented;
    }
