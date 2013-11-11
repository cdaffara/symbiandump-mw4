/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/



#include "HuiFxLayer.h"

EXPORT_C CHuiFxLayer::CHuiFxLayer() :
    iLayerType( ELayerTypeUnknown )
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxLayer::CHuiFxLayer - 0x%x "), this);
#endif
    }

EXPORT_C CHuiFxLayer::~CHuiFxLayer()
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxLayer::~CHuiFxLayer - 0x%x "), this);
#endif
    }

EXPORT_C void CHuiFxLayer::ConstructL( THuiFxLayerType aType )
    {
    iBlendingOp = EBlendingModeReplace;
    iLayerType = aType;
    iSourceRect = TRect(0, 0, 0, 0);
    iTargetRect = TRect(0, 0, 0, 0);
    iVisualRect = TRect(0, 0, 0, 0);
    iAnimated = EFalse;
    iTransformed = EFalse;
    iMarginEnabled = ETrue;
    }
EXPORT_C void CHuiFxLayer::CopyFromL(const CHuiFxLayer *aOldLayer)
    {
    iBlendingOp = aOldLayer->iBlendingOp;
    iLayerType = aOldLayer->iLayerType;
    iSourceRect = aOldLayer->iSourceRect;
    iTargetRect = aOldLayer->iTargetRect;
    iVisualRect = aOldLayer->iVisualRect;
    iAnimated = aOldLayer->iAnimated;
    iTransformed = aOldLayer->iTransformed;
    iMarginEnabled = aOldLayer->iMarginEnabled;
    }

EXPORT_C TBool CHuiFxLayer::PrepareDrawL(CHuiFxEngine& /*aEngine*/)
    {
    // Nothing to do
    return ETrue;
    }

EXPORT_C TBool CHuiFxLayer::VisualArea( TRect& /*aRect*/ )
    {
    // No area in base class
    return EFalse;
    }
EXPORT_C TBool CHuiFxLayer::Margin( TMargins & /* aMargin */ )
{
   // empty implementation in base class
   return EFalse;
}

EXPORT_C TBool CHuiFxLayer::Changed() const
    {
    return EFalse;
    }

EXPORT_C void CHuiFxLayer::AdvanceTime(TReal32 /*aElapsedTime*/)
    {
    // Nothing to do
    }

EXPORT_C THuiFxLayerType CHuiFxLayer::Type()
    {
    return iLayerType;
    }

EXPORT_C void CHuiFxLayer::SetBlendingMode(THuiFxBlendingMode aOp)
    {
    iBlendingOp = aOp;
    }

EXPORT_C THuiFxBlendingMode CHuiFxLayer::BlendingMode() const
    {
    return iBlendingOp;
    }

EXPORT_C void CHuiFxLayer::SetDisplayArea(const TRect& aRect)
    {
    iDisplayArea = aRect;
    }

EXPORT_C void CHuiFxLayer::SetSourceRect(const TRect& aRect)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxLayer::SetSourceRect - 0x%x (%d,%d,%d,%d))"), this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
#endif
    iSourceRect = aRect;
    }

EXPORT_C void CHuiFxLayer::SetTargetRect(const TRect& aRect)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxLayer::SetTargetRect - 0x%x (%d,%d,%d,%d))"), this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
#endif
    iTargetRect = aRect;
    }

EXPORT_C void CHuiFxLayer::SetVisualRect(const TRect& aRect)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxLayer::SetVisualRect - 0x%x (%d,%d,%d,%d))"), this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
#endif
    iVisualRect = aRect;
    }

EXPORT_C const TRect& CHuiFxLayer::SourceRect() const
    {
    return iSourceRect;
    }

EXPORT_C const TRect& CHuiFxLayer::TargetRect() const
    {
    return iTargetRect;
    }

EXPORT_C const TRect& CHuiFxLayer::VisualRect() const
    {
    return iVisualRect;
    }

TBool CHuiFxLayer::IsAnimated() const
    {
    return iAnimated;
    }

void CHuiFxLayer::SetAnimated(TBool aAnimated)
    {
    iAnimated = aAnimated;
    }

TBool CHuiFxLayer::IsTransformed() const
    {
    return iTransformed;
    }    

void CHuiFxLayer::SetTransformed(TBool aTransformed)
    {
    iTransformed = aTransformed;
    }

void CHuiFxLayer::EnableMargin(TBool aEnable)
    {
    iMarginEnabled = aEnable;
    EnableMarginApplyChildren(aEnable);
    }
void CHuiFxLayer::EnableMarginApplyChildren(TBool /*aEnable*/)
    {
    /* empty in baseclass */
    }


TBool CHuiFxLayer::IsMarginEnabled() const
    {
    return iMarginEnabled;
    }

TBool CHuiFxLayer::IsFiltered() const
    {
    return EFalse;
    }

TBool CHuiFxLayer::IsSemitransparent() const
    {
    return EFalse;
    }

TBool CHuiFxLayer::AlwaysReadSurfacePixels() const
    {
    return iAlwaysReadSurfacePixels;
    }    

void CHuiFxLayer::SetAlwaysReadSurfacePixels(TBool aAlwaysReadSurfacePixels)
    {
    iAlwaysReadSurfacePixels = aAlwaysReadSurfacePixels;
    }
