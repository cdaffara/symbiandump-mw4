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
* Description:   Implementation for CHuiCurvePathLayout. Combines a layout 
*                visual and a curve path. The path is rescaled as the layout's 
*                size changes.
*
*/



#include "uiacceltk/HuiCurvePathLayout.h"  // Class definition
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiCurvePath.h"
#include "uiacceltk/HuiPanic.h"


const TReal32 KPathLogicalCoordsMax = 1000.0;


EXPORT_C CHuiCurvePathLayout* CHuiCurvePathLayout::AddNewL(CHuiControl& aOwnerControl,
                                                           CHuiLayout* aParentLayout)
    {
    CHuiCurvePathLayout* layout = new (ELeave) CHuiCurvePathLayout(aOwnerControl);
    CleanupStack::PushL(layout);
    layout->ConstructL();    
    aOwnerControl.AppendL(layout, aParentLayout);
    CleanupStack::Pop(layout);           
    return layout;
    }


CHuiCurvePathLayout::CHuiCurvePathLayout(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner)
    {
    }


void CHuiCurvePathLayout::ConstructL()
    {    
    CHuiLayout::ConstructL();
    iPath = CHuiCurvePath::NewL();
    }

    
CHuiCurvePathLayout::~CHuiCurvePathLayout()
    {    
    delete iPath;
    }


EXPORT_C CHuiCurvePath& CHuiCurvePathLayout::CurvePath()
    {
    return *iPath;
    }


TReal32 CHuiCurvePathLayout::MapValue(TReal32 aValue, TInt aMode) const __SOFTFP
    {
    TReal32 value = iPath->MapValue(aValue, aMode);  
    
    if (iPath->IsLegacyApiUsed())
    	{
    	// The path defines points in range (0, 1000).
    	value /= KPathLogicalCoordsMax;
    	if(aMode == 0) // Horizontal.
        	{
        	value *= Size().RealNow().iX;
        	}
    	else // Vertical.
        	{
        	value *= Size().RealNow().iY;
        	}
    	}
    
    return value;
    }

TBool CHuiCurvePathLayout::MappingFunctionChanged() const
    {
    return iPath->MappingFunctionChanged() || Size().Changed();
    }
    
    
void CHuiCurvePathLayout::MappingFunctionClearChanged()
    {
    iPath->MappingFunctionClearChanged();
    }

void CHuiCurvePathLayout::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    UpdateChildrenLayout(aTransitionTime);    
    }
    

TBool CHuiCurvePathLayout::ChildSize(TInt /*aOrdinal*/, TSize& /*aSize*/)
    {
    return EFalse;
    }

TBool CHuiCurvePathLayout::ChildPos(TInt /*aOrdinal*/, TPoint& /*aPos*/)
    {
    return EFalse;
    }
    
EXPORT_C TInt CHuiCurvePathLayout::ChildRect(TInt /*aOrdinal*/, THuiRealRect& /*aPos*/)
    {
    return THuiLayoutChildRectUpdateNotNeeded;
    }
