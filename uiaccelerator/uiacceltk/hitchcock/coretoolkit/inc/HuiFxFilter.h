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



#ifndef HUIFXFILTER_H_
#define HUIFXFILTER_H_

class CHuiFxParameterManager;

#include "HuiFxEngine.h"
#include "HuiFxRenderbuffer.h"
#include "HuiFxParameter.h"
#include "HuiFxConstants.h"
#include "HuiFxParameterCollection.h"

class CHuiFxFilter: public CBase, public MHuiFxParameterCollection
    {
public:
    IMPORT_C virtual ~CHuiFxFilter();
    IMPORT_C virtual TBool PrepareDrawL(CHuiFxEngine& aEngine);
    IMPORT_C virtual TBool Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
                               const TRect& aTargetRect, const TRect& aSourceRect, TBool aHasSurface) = 0;
    // CalculateMargin will be called before PrepareDrawL(), so 
    // parameters are not ready, so margin calculation need to use 
    // either previous frame parameter values, or should be independent 
    // of parameters. Should do nothing else than set value for aMargin.
    IMPORT_C virtual void CalculateMargin(TMargins &aMargin) const;

    // TODO: which of these are really needed?
    IMPORT_C void SetMargin(const TSize& aMargin);
    IMPORT_C const TSize& Margin() const;
    IMPORT_C const TMargins &Margin2() const;
    IMPORT_C void SetFreeForm(TBool aEnable);
    IMPORT_C TBool IsFreeForm() const;
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime);
    IMPORT_C TBool Changed() const;
    IMPORT_C void SetVisualRect(const TRect& aVisualRect);
    IMPORT_C const TRect& VisualRect() const;

    IMPORT_C MHuiFxParameter* Parameter(const TDesC& aName);
    IMPORT_C virtual CHuiFxFilter *CloneL() const=0;

protected:
    IMPORT_C void ConstructL();
    IMPORT_C void RegisterParameterL(const TDesC& aName, TRgb* aValue);
    IMPORT_C void RegisterParameterL(const TDesC& aName, TReal32* aValue);
    IMPORT_C void CopyParameterL(const TDesC& aName, TRgb* aValue, const CHuiFxFilter *aOldFilter);
    IMPORT_C void CopyParameterL(const TDesC& aName, TReal32* aValue, const CHuiFxFilter *aOldFilter);
    IMPORT_C void CopyFromL(const CHuiFxFilter *aOldFilter); // copies all baseclass CHuiFxFilter parameters
    IMPORT_C TBool ParameterChanged();
    
    TReal32 iOpacity;

private:
    CHuiFxParameterManager* iParameterManager;
    TSize               iMargin; // not used
    TBool               iIsFreeForm;
    TBool               iParameterChanged;
    TBool               iFirstParameterCheck;
    TBool               iFrameCount;
    TRect               iVisualRect;
    mutable TMargins            iMargin2;
    };

#endif /*HUIFXFILTER_H_*/
