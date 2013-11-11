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



#ifndef HUIFXLAYER_
#define HUIFXLAYER_

#include "HuiFxEngine.h"
#include "HuiFxRenderbuffer.h"

//FORWARD DECLARATIONS
class MHuiEffectable;

class CHuiFxLayer: public CBase
    {
public:
    IMPORT_C virtual ~CHuiFxLayer();

    IMPORT_C virtual TBool PrepareDrawL(CHuiFxEngine& aEngine);
    virtual void Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource, TBool aHasSurface) = 0;
    IMPORT_C virtual TBool VisualArea(TRect& aRect);
    IMPORT_C virtual TBool Margin( TMargins &m );
    IMPORT_C virtual void AdvanceTime(TReal32 aElapsedTime);
    IMPORT_C virtual TBool Changed() const;
    
    IMPORT_C THuiFxLayerType Type();
    
    IMPORT_C void SetBlendingMode(THuiFxBlendingMode aOp);
    IMPORT_C THuiFxBlendingMode BlendingMode() const;

    IMPORT_C void SetDisplayArea(const TRect& aRect);
    IMPORT_C void SetSourceRect(const TRect& aRect);
    IMPORT_C void SetTargetRect(const TRect& aRect);
    IMPORT_C void SetVisualRect(const TRect& aRect);

    IMPORT_C const TRect& SourceRect() const;
    IMPORT_C const TRect& TargetRect() const;
    IMPORT_C const TRect& VisualRect() const;
public: // effect cache
    IMPORT_C virtual CHuiFxLayer *CloneL() const=0;
    IMPORT_C virtual void SetExtRect( TRect *aExtRect )=0;
    IMPORT_C virtual void SetVisual( CHuiVisual *aVisual )=0;
    IMPORT_C virtual void SetVisual( MHuiEffectable *aVisual )=0;
    virtual TBool FxmlUsesOpaqueHint() const { return EFalse; }
    IMPORT_C void CopyFromL(const CHuiFxLayer *aOldLayer);


    // Internal flag to tell wheter layer is animated
    virtual TBool IsAnimated() const;
    void SetAnimated(TBool aAnimated);

    // Internal flag to tell wheter layer is transformed
    virtual TBool IsTransformed() const; 
    void SetTransformed(TBool aTransformed);

    // Internal flag to tell wheter layer is filtered
    virtual TBool IsFiltered() const; 

    // Internal flag to tell wheter margin is enabled
    void EnableMargin(TBool aEnable);
    virtual void EnableMarginApplyChildren(TBool aEnable);
    TBool IsMarginEnabled() const;
    
    // Internal flag to tell wheter transparency is enabled
    virtual TBool IsSemitransparent() const;
    
    virtual void FxmlVisualInputs(RArray<THuiFxVisualSrcType> & /*aArray*/) { }
    
    TBool AlwaysReadSurfacePixels() const;
    virtual void SetAlwaysReadSurfacePixels(TBool aAlwaysReadSurfacePixels);
    
protected:
    IMPORT_C CHuiFxLayer();
    IMPORT_C void ConstructL( THuiFxLayerType aType );

protected:
    TRect               iDisplayArea; // used for clipping the effects to the real display area
    
private:
    THuiFxBlendingMode  iBlendingOp;
    THuiFxLayerType     iLayerType;
    TRect               iSourceRect;
    TRect               iTargetRect;
    TRect               iVisualRect;
    TBool               iAnimated;
    TBool               iTransformed;
    TBool               iMarginEnabled;
    TBool               iAlwaysReadSurfacePixels;
    };

#endif /*HUIFXLAYER_*/
