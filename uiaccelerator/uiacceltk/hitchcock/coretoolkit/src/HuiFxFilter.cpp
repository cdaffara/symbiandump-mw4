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



#include "HuiFxFilter.h"
#include "HuiFxScalarParameter.h"
#include "HuiFxColorParameter.h"
#include "HuiFxParameterManager.h"
#include "HuiFxConstants.h"


EXPORT_C void CHuiFxFilter::ConstructL()
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxFilter::ConstructL - 0x%x "), this);
#endif
    iMargin = TSize(0, 0);
    iIsFreeForm = EFalse;
    iOpacity = 1.0f;
    iParameterChanged = EFalse;
    iFirstParameterCheck = ETrue;
    iFrameCount = ETrue;
    iParameterManager = CHuiFxParameterManager::NewL();
    RegisterParameterL(KLitOpacityParameter, &iOpacity);
    }

EXPORT_C void CHuiFxFilter::CopyFromL(const CHuiFxFilter *aOldFilter)
    {
    iMargin = aOldFilter->iMargin;
    iIsFreeForm = aOldFilter->iIsFreeForm;
    iOpacity = aOldFilter->iOpacity;
    iParameterChanged = aOldFilter->iParameterChanged;
    iFirstParameterCheck = aOldFilter->iFirstParameterCheck;
    iFrameCount = aOldFilter->iFrameCount;
    iParameterManager = aOldFilter->iParameterManager->CloneL();
    CopyParameterL(KLitOpacityParameter, &iOpacity, aOldFilter);
    }

EXPORT_C CHuiFxFilter::~CHuiFxFilter()
    {
    delete iParameterManager;
    iParameterManager = NULL;
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxFilter::~CHuiFxFilter - 0x%x "), this);
#endif
    }

EXPORT_C TBool CHuiFxFilter::Changed() const
    {
    return iParameterManager->HasAnimatedParameters();
    }

EXPORT_C TBool CHuiFxFilter::PrepareDrawL(CHuiFxEngine& aEngine)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxFilter::PrepareDrawL - 0x%x "), this);
#endif
    iFrameCount++;
    iParameterChanged = iParameterManager->Update(aEngine, VisualRect());
    return ETrue;
    }
EXPORT_C void CHuiFxFilter::CalculateMargin(TMargins &aMargin) const
    {
    aMargin.iLeft = 0;
    aMargin.iRight = 0;
    aMargin.iTop = 0;
    aMargin.iBottom = 0;
    }

EXPORT_C void CHuiFxFilter::AdvanceTime(TReal32 aElapsedTime)
    {
    iParameterManager->AdvanceTime(aElapsedTime);
    }

EXPORT_C void CHuiFxFilter::SetMargin(const TSize& aMargin)
    {
    iMargin = aMargin;
    }

EXPORT_C const TSize& CHuiFxFilter::Margin() const
    {
    return iMargin;
    }
EXPORT_C const TMargins &CHuiFxFilter::Margin2() const
    {
    CalculateMargin(iMargin2);
    return iMargin2;
    }

EXPORT_C void CHuiFxFilter::SetFreeForm(TBool aEnable)
    {
    iIsFreeForm = aEnable;
    }

EXPORT_C TBool CHuiFxFilter::IsFreeForm() const
    {
    return iIsFreeForm;
    }

EXPORT_C MHuiFxParameter* CHuiFxFilter::Parameter(const TDesC& aName)
    {
    return iParameterManager->Parameter(aName);
    }

EXPORT_C void CHuiFxFilter::RegisterParameterL(const TDesC& aName, TReal32* aValue)
    {
    iParameterManager->RegisterParameterL(aName, aValue);
    }

EXPORT_C void CHuiFxFilter::RegisterParameterL(const TDesC& aName, TRgb* aValue)
    {
    iParameterManager->RegisterParameterL(aName, aValue);
    }
EXPORT_C void CHuiFxFilter::CopyParameterL(const TDesC& aName, TRgb* aValue, const CHuiFxFilter *aOldFilter)
    {
    iParameterManager->CopyParameterL(aName, aValue, aOldFilter->iParameterManager);
    }
EXPORT_C void CHuiFxFilter::CopyParameterL(const TDesC& aName, TReal32* aValue, const CHuiFxFilter *aOldFilter)
    {
    iParameterManager->CopyParameterL(aName, aValue, aOldFilter->iParameterManager);    
    }

EXPORT_C TBool CHuiFxFilter::ParameterChanged()
    {
    if(iFirstParameterCheck)
        {
        iFirstParameterCheck = EFalse;
        return ETrue;
        }
    else
        {
        return iParameterChanged || (iFrameCount <= 1);
        }
    }

EXPORT_C void CHuiFxFilter::SetVisualRect(const TRect& aRect)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxFilter::SetVisualRect - 0x%x (%d,%d,%d,%d))"), this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
#endif
    iVisualRect = aRect;
    }

EXPORT_C const TRect& CHuiFxFilter::VisualRect() const
    {
    return iVisualRect;
    }
