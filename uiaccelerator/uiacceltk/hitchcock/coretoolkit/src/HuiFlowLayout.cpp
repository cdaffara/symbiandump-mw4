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


 
#include <AknUtils.h>

#include "uiacceltk/HuiFlowLayout.h"  // Class definition
#include "uiacceltk/HuiControl.h"


EXPORT_C CHuiFlowLayout* CHuiFlowLayout::AddNewL(CHuiControl& aOwnerControl, 
                                                 TFlowDirection aDirection,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiFlowLayout* flow = STATIC_CAST(CHuiFlowLayout*,
        aOwnerControl.AppendLayoutL(EHuiLayoutTypeFlow, aParentLayout));
    flow->SetFlowDirection(aDirection);       
    return flow;
    }


CHuiFlowLayout::CHuiFlowLayout(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner), iMode(EFlowHorizontal)
    {
    }


void CHuiFlowLayout::ConstructL()
    {    
    CHuiLayout::ConstructL();
    }

	
EXPORT_C CHuiFlowLayout::~CHuiFlowLayout()
    {    
    }


EXPORT_C void CHuiFlowLayout::SetSize(const THuiRealSize& aSize, 
                                      TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    UpdateChildrenLayout(aTransitionTime);    
    }


EXPORT_C TBool CHuiFlowLayout::ChildSize(TInt aOrdinal, TSize& aSize)
    {    
    TBool result(EFalse);
    THuiRealRect rect;
    TInt childRectStatus(THuiLayoutChildRectUpdateNotNeeded);
    childRectStatus = ChildRect(aOrdinal, rect);
    if(childRectStatus != THuiLayoutChildRectNotImplemented)
        {
        result = (childRectStatus & THuiLayoutChildRectSizeUpdateNeeded);
        if(result)
            {
            THuiRealPoint size(rect.Width(), rect.Height());
            aSize = LocalPointInPixels(size).AsSize();
            }
        }
    return result;        
    }
    
    
EXPORT_C TBool CHuiFlowLayout::ChildPos(TInt aOrdinal, TPoint& aPos)
    {
    TBool result(EFalse);
    THuiRealRect rect;
    TInt childRectStatus(THuiLayoutChildRectUpdateNotNeeded);
    childRectStatus = ChildRect(aOrdinal, rect);
    if(childRectStatus != THuiLayoutChildRectNotImplemented)
        {
        result = (childRectStatus & THuiLayoutChildRectPosUpdateNeeded);
        if(result)
            {
            aPos = LocalPointInPixels(rect.iTl);
            }
        }
    return result;   
    }

EXPORT_C TInt CHuiFlowLayout::ChildRect(TInt aOrdinal, THuiRealRect& aRect)
    {    
    THuiRealPoint childTopLeft;    
    THuiRealSize childSize = Visual(aOrdinal).Size().RealTarget().AsSize();
    THuiRealSize parentSize = InnerSize();
    TReal32 horizDirection = 1.0;
    
    // By default, flow layout modifies only positions of the children
    TInt ret = THuiLayoutChildRectPosUpdateNeeded;
    
    // Adjust the child size if perpendicular fit is specified
    if(iMode & EModeFitPerpendicular)
        {
        // Expand the child's size to fit the layout rectangle along
        // the minor axis.        
        if(iDirection == EFlowVertical)
            {
            childSize.iWidth = parentSize.iWidth;
            }
        else
            {
            childSize.iHeight = parentSize.iHeight;
            }                                
        // Modify also the size of the child.        
        ret |= THuiLayoutChildRectSizeUpdateNeeded;
        }      
    
    // Center the child perpendicular to the flow direction
    if(iMode & EModeCenterPerpendicular)
        {
        if(iDirection == EFlowHorizontal)
            {            
            childTopLeft.iY = parentSize.iHeight/2 - childSize.iHeight/2;
            }
        else
            {
            childTopLeft.iX = parentSize.iWidth/2 - childSize.iWidth/2;
            }
        }
        
    // Apply parent padding
    THuiRealPoint topLeft = InnerTopLeft();
    childTopLeft += topLeft;

    // Iterate through previous children and add their width or height along with
    // inner padding to the position.
    THuiRealPoint innerPadding = InnerPaddingInBaseUnits();
    for(TInt i = 0; i < aOrdinal; ++i)
        {
        if(!(Visual(i).Flags() & EHuiVisualFlagManualPosition))
            {
            // Take child visual into calculations unless it is manually positioned.
            THuiRealSize size = Visual(i).Size().RealTarget().AsSize();
            
            if(iDirection == EFlowHorizontal)
                {
                size.iWidth += innerPadding.iX;
                childTopLeft.iX += horizDirection * size.iWidth;
                }
            else
                {
                size.iHeight += innerPadding.iY;
                childTopLeft.iY += size.iHeight;
                }            
            }
        }      
    
    // Compose the final child rectangle    
    aRect = THuiRealRect(childTopLeft, childSize);
    
    // Return the bitmask of modified components
    return ret;
    }


EXPORT_C void CHuiFlowLayout::SetFlowDirection(TFlowDirection aDirection)
    {
    if(iDirection != aDirection)
        {
        iDirection = aDirection;
        SetChanged();        
        }
    }


EXPORT_C void CHuiFlowLayout::SetCentering(TBool aCentering)
    {
    if(aCentering)
        {
        iMode |= EModeCenterPerpendicular;
        }
    else
        {
        iMode &= ~EModeCenterPerpendicular;
        }
    SetChanged();        
    }


EXPORT_C void CHuiFlowLayout::SetMode(TInt aMode)
    {
    iMode = aMode;
    SetChanged();
    }


EXPORT_C TInt CHuiFlowLayout::Mode() const
    {
    return iMode;
    }
