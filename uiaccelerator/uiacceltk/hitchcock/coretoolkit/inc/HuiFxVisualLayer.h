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



#ifndef HUIFXVISUALLAYER_
#define HUIFXVISUALLAYER_

class CHuiFxParameterManager;

#include "HuiFxLayer.h"
#include "HuiFxParameter.h"
#include "HuiCmdBufferBrush.h" // MHuiEffectable

class CHuiFxVisualLayer: public CHuiFxLayer
    {
public:
    IMPORT_C static CHuiFxVisualLayer* NewL(MHuiEffectable* aVisual); // param mandatory
    IMPORT_C ~CHuiFxVisualLayer();
    IMPORT_C TBool PrepareDrawL(CHuiFxEngine& aEngine);
    IMPORT_C void Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource, TBool aHasSurface);
    IMPORT_C TBool VisualArea( TRect& aRect );
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime);
    IMPORT_C TBool Changed() const;
    IMPORT_C TBool Margin(TMargins &m);

    IMPORT_C MHuiFxParameter* Parameter(const TDesC& aName);
    void SetSourceType( const THuiFxVisualSrcType aSrcType );
    THuiFxVisualSrcType SourceType() const;
    void SetExtBitmapFileL( TDesC& aFilename );
    TBool IsSemitransparent() const;
    
public: // effect cache
    IMPORT_C CHuiFxVisualLayer *CloneL() const;
    IMPORT_C void SetExtRect( TRect *aExtRect );
    IMPORT_C void SetVisual( CHuiVisual *aVisual );
    IMPORT_C void SetVisual( MHuiEffectable *aVisual );

    void FxmlVisualInputs(RArray<THuiFxVisualSrcType> &aArray);
    void SetFxmlUsesOpaqueHint(TBool aValue);
    TBool FxmlUsesOpaqueHint() const;
    
protected:
    IMPORT_C void ConstructL(MHuiEffectable* aVisual);
    
    void RegisterParameterL(const TDesC& aName, TRgb* aValue);
    void RegisterParameterL(const TDesC& aName, TReal32* aValue);
    
private:
    void ApplyTransformations(CHuiGc& aGc);
    
    //CHuiVisual*         iVisual;
    MHuiEffectable *    iVisual;
    CHuiFxParameterManager* iParameterManager;
    THuiFxVisualSrcType iSrcType; // default is the visual itself
    
    TReal32             iTranslationX;
    TReal32             iTranslationY;
    TReal32             iTranslationZ;
    
    TReal32             iScaleX;
    TReal32             iScaleY;
    TReal32             iScaleZ;
    TReal32             iScaleOriginX;
    TReal32             iScaleOriginY;
    TReal32             iScaleOriginZ;
    
    TReal32             iRotationOriginX;
    TReal32             iRotationOriginY;
    TReal32             iRotationOriginZ;
    TReal32             iRotationAngle;
    TReal32             iRotationAxisX;
    TReal32             iRotationAxisY;
    TReal32             iRotationAxisZ;
    
    TReal32             iSkewAngleX;
    TReal32             iSkewAngleY;
    TReal32             iSkewAngleZ;
    TReal32             iSkewOriginX;
    TReal32             iSkewOriginY;
    TReal32             iSkewOriginZ;
    
    HBufC* iExtBitmapFile;
    TReal32 iOpacity;
    TBool iOpaqueHint;
    };

#endif /*HUIFXVISUALLAYER_*/
