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
* Description:   Implementation for CHuiDeckLayout. Deck layout is a very simple layout
*                that overlays all its children to match the layout's own rectangle.
*                Think of it as a deck of cards.
*
*/



#include "uiacceltk/HuiDeckLayout.h"  // Class definition
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiPanic.h"


EXPORT_C CHuiDeckLayout* CHuiDeckLayout::AddNewL(CHuiControl& aOwnerControl,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiDeckLayout* layout = new (ELeave) CHuiDeckLayout(aOwnerControl);
    CleanupStack::PushL(layout);
    layout->ConstructL();    
    aOwnerControl.AppendL(layout, aParentLayout);
    CleanupStack::Pop(layout);           
    return layout;
    }


CHuiDeckLayout::CHuiDeckLayout(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner)
    {
    }


void CHuiDeckLayout::ConstructL()
    {    
    CHuiLayout::ConstructL();
    }

    
EXPORT_C CHuiDeckLayout::~CHuiDeckLayout()
    {    
    }


void CHuiDeckLayout::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    UpdateChildrenLayout(aTransitionTime);    
    }

TBool CHuiDeckLayout::ChildSize(TInt aOrdinal, TSize& aSize)
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

TBool CHuiDeckLayout::ChildPos(TInt aOrdinal, TPoint& aPos)
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

TInt CHuiDeckLayout::ChildRect(TInt /*aOrdinal*/, THuiRealRect& aPos)
    {
    THuiRealSize innerSize = InnerSize();
    aPos.iTl = InnerTopLeft(); 
    aPos.iBr = aPos.iTl + THuiRealPoint(innerSize.iWidth, innerSize.iHeight) ; 
    return THuiLayoutChildRectLayoutUpdateNeeded;
    }

