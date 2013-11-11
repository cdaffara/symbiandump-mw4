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



#ifndef HUIFXGROUPLAYER_H_
#define HUIFXGROUPLAYER_H_

#include "HuiFxLayer.h"

class CHuiFxGroupLayer: public CHuiFxLayer
    {
public:
    IMPORT_C ~CHuiFxGroupLayer();
    IMPORT_C static CHuiFxGroupLayer* NewL(TBool aIsRoot = EFalse);
    IMPORT_C TBool PrepareDrawL(CHuiFxEngine& aEngine);
    IMPORT_C void Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource, TBool aHasSurface);
    IMPORT_C void AddLayerL(const CHuiFxLayer* aLayer); // takes ownership
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime);
    IMPORT_C TBool Changed() const;
    
    IMPORT_C TBool VisualArea( TRect& aRect );
    IMPORT_C TBool Margin( TMargins &aMargin ); 
    IMPORT_C void ApplyMargin( TRect &aRect );   
    
    // effect cache methods
    IMPORT_C CHuiFxGroupLayer *CloneL() const;
    IMPORT_C void SetExtRect( TRect *aExtRect );
    IMPORT_C void SetVisual( CHuiVisual *aVisual );
    IMPORT_C void SetVisual( MHuiEffectable *aVisual );

    TBool IsAnimated() const;
    TBool IsTransformed() const; 
    TBool IsFiltered() const; 
    TBool IsSemitransparent() const;
    
    TInt LayerCount() const;
    CHuiFxLayer& Layer(TInt aIndex) const;
    void FxmlVisualInputs(RArray<THuiFxVisualSrcType> &aArray);
    TBool FxmlUsesOpaqueHint() const;
    virtual void EnableMarginApplyChildren(TBool aEnable);
    virtual void SetAlwaysReadSurfacePixels(TBool aAlwaysReadSurfacePixels);

protected:
    IMPORT_C CHuiFxGroupLayer();
    IMPORT_C void ConstructL(TBool aIsRoot);
private:
    RPointerArray<CHuiFxLayer> iLayers;
    TBool                   iIsRoot;
    TRect                   iBackbufferRect;
    };

#endif /*HUIFXGROUPLAYER_H_*/
