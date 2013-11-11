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



#include "HuiFxVisualLayer.h"
#include "Matrix.h"
#include "HuiFxParameterManager.h"
#include <uiacceltk/HuiVisual.h>
#include "HuiFxConstants.h"
#include <e32math.h>
#include "HuiCmdBufferBrush.h"

EXPORT_C CHuiFxVisualLayer* CHuiFxVisualLayer::NewL(MHuiEffectable* aVisual)
    {
    CHuiFxVisualLayer* e = new (ELeave) CHuiFxVisualLayer();
    CleanupStack::PushL(e);
    e->ConstructL(aVisual);
    CleanupStack::Pop(e);
    return e;
    }

EXPORT_C void CHuiFxVisualLayer::ConstructL(MHuiEffectable* aVisual)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxVisualLayer::ConstructL - 0x%x "), this);
#endif
    CHuiFxLayer::ConstructL( ELayerTypeVisual );
    iParameterManager = CHuiFxParameterManager::NewL();
    iVisual = aVisual;
    iSrcType = EVisualSrcVisual;
    iExtBitmapFile = NULL;
    
    iTranslationX = 0.0f;
    iTranslationY = 0.0f;
    iTranslationZ = 0.0f;
    
    iScaleX = 1.0f;
    iScaleY = 1.0f;
    iScaleZ = 1.0f;
    iScaleOriginX = 0.0f;
    iScaleOriginY = 0.0f;
    iScaleOriginZ = 0.0f;
    
    iRotationOriginX = 0.0f;
    iRotationOriginY = 0.0f;
    iRotationOriginZ = 0.0f;
    iRotationAngle = 0.0f;
    iRotationAxisX = 0.0f;
    iRotationAxisY = 0.0f;
    iRotationAxisZ = 1.0f;
    
    iSkewAngleX = 0.0f;
    iSkewAngleY = 0.0f;
    iSkewAngleZ = 0.0f;
    iSkewOriginX = 0.0f;
    iSkewOriginY = 0.0f;
    iSkewOriginZ = 0.0f;
    
    // We use the visual's own target opacity if we haven't set something explicitly    
    if (iVisual)
        {
        //iOpacity = iVisual->iOpacity.Target();
        iOpacity = iVisual->EffectOpacityTarget();
        }
    else
        {
        iOpacity = 1.0;
        }
    RegisterParameterL(KLitTranslationX, &iTranslationX);
    RegisterParameterL(KLitTranslationY, &iTranslationY);
    RegisterParameterL(KLitTranslationZ, &iTranslationZ);
    
    RegisterParameterL(KLitScaleX, &iScaleX);
    RegisterParameterL(KLitScaleY, &iScaleY);
    RegisterParameterL(KLitScaleZ, &iScaleZ);
    RegisterParameterL(KLitScaleOriginX, &iScaleOriginX);
    RegisterParameterL(KLitScaleOriginY, &iScaleOriginY);
    RegisterParameterL(KLitScaleOriginZ, &iScaleOriginZ);
   
    RegisterParameterL(KLitRotationOriginX, &iRotationOriginX);
    RegisterParameterL(KLitRotationOriginY, &iRotationOriginY);
    RegisterParameterL(KLitRotationOriginZ, &iRotationOriginZ);
    RegisterParameterL(KLitRotationAngle, &iRotationAngle);
    RegisterParameterL(KLitRotationAxisX, &iRotationAxisX);
    RegisterParameterL(KLitRotationAxisY, &iRotationAxisY);
    RegisterParameterL(KLitRotationAxisZ, &iRotationAxisZ);
    
    RegisterParameterL(KLitSkewAngleX, &iSkewAngleX);
    RegisterParameterL(KLitSkewAngleY, &iSkewAngleY);
    RegisterParameterL(KLitSkewAngleZ, &iSkewAngleZ);
    RegisterParameterL(KLitSkewOriginX, &iSkewOriginX);
    RegisterParameterL(KLitSkewOriginY, &iSkewOriginY);
    RegisterParameterL(KLitSkewOriginZ, &iSkewOriginZ);
    
    RegisterParameterL(KLitOpacityParameter, &iOpacity);
    }

EXPORT_C CHuiFxVisualLayer *CHuiFxVisualLayer::CloneL() const
{
    CHuiFxVisualLayer *layer = new (ELeave) CHuiFxVisualLayer;
    layer->CHuiFxLayer::ConstructL( ELayerTypeVisual );
    layer->CHuiFxLayer::CopyFromL(this);
    layer->iVisual = iVisual; // visual will be set later...
    layer->iParameterManager = iParameterManager->CloneL();
    layer->iParameterManager->CopyParameterL(KLitTranslationX, &layer->iTranslationX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitTranslationY, &layer->iTranslationY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitTranslationZ, &layer->iTranslationZ, iParameterManager);
    
    layer->iParameterManager->CopyParameterL(KLitScaleX, &layer->iScaleX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitScaleY, &layer->iScaleY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitScaleZ, &layer->iScaleZ, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitScaleOriginX, &layer->iScaleOriginX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitScaleOriginY, &layer->iScaleOriginY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitScaleOriginZ, &layer->iScaleOriginZ, iParameterManager);
   
    layer->iParameterManager->CopyParameterL(KLitRotationOriginX, &layer->iRotationOriginX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitRotationOriginY, &layer->iRotationOriginY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitRotationOriginZ, &layer->iRotationOriginZ, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitRotationAngle, &layer->iRotationAngle, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitRotationAxisX, &layer->iRotationAxisX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitRotationAxisY, &layer->iRotationAxisY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitRotationAxisZ, &layer->iRotationAxisZ, iParameterManager);
    
    layer->iParameterManager->CopyParameterL(KLitSkewAngleX, &layer->iSkewAngleX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitSkewAngleY, &layer->iSkewAngleY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitSkewAngleZ, &layer->iSkewAngleZ, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitSkewOriginX, &layer->iSkewOriginX, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitSkewOriginY, &layer->iSkewOriginY, iParameterManager);
    layer->iParameterManager->CopyParameterL(KLitSkewOriginZ, &layer->iSkewOriginZ, iParameterManager);
    
    layer->iParameterManager->CopyParameterL(KLitOpacityParameter, &layer->iOpacity, iParameterManager);

    layer->iSrcType = iSrcType;
    layer->iTranslationX = iTranslationX;
    layer->iTranslationY = iTranslationY;
    layer->iTranslationZ = iTranslationZ;
    layer->iScaleX = iScaleX;    
    layer->iScaleY = iScaleY;    
    layer->iScaleZ = iScaleZ;    
    layer->iScaleOriginX = iScaleOriginX;    
    layer->iScaleOriginY = iScaleOriginY;    
    layer->iScaleOriginZ = iScaleOriginZ;    
    layer->iRotationOriginX = iRotationOriginX;    
    layer->iRotationOriginY = iRotationOriginY;    
    layer->iRotationOriginZ = iRotationOriginZ;    
    layer->iRotationAngle = iRotationAngle;
    layer->iRotationAxisX = iRotationAxisX;
    layer->iRotationAxisY = iRotationAxisY;
    layer->iRotationAxisZ = iRotationAxisZ;
    layer->iSkewAngleX = iSkewAngleX;
    layer->iSkewAngleY = iSkewAngleY;
    layer->iSkewAngleZ = iSkewAngleZ;
    
    layer->iSkewOriginX = iSkewOriginX;
    layer->iSkewOriginY = iSkewOriginY;
    layer->iSkewOriginZ = iSkewOriginZ;
    if (iExtBitmapFile)
        layer->iExtBitmapFile = iExtBitmapFile->AllocL();
    else
        layer->iExtBitmapFile = NULL;
    layer->iOpacity = iOpacity;
    layer->iOpaqueHint = iOpaqueHint;
    return layer;
}
EXPORT_C void CHuiFxVisualLayer::SetExtRect( TRect * /*aExtRect*/ )
    {
    // not needed..
    }
EXPORT_C void CHuiFxVisualLayer::SetVisual( CHuiVisual * aVisual )
    {
    iVisual = aVisual->Effectable();
    
    if (Math::IsInfinite(iOpacity))
        {
        iOpacity = aVisual->iOpacity.Target();
        }
    }
EXPORT_C void CHuiFxVisualLayer::SetVisual( MHuiEffectable *aVisual )
    {
    iVisual = aVisual;
    
    if (Math::IsInfinite(iOpacity))
        {
        //iOpacity = iVisual->iOpacity.Target();
        iOpacity = iVisual->EffectOpacityTarget();
        }    
    }



EXPORT_C CHuiFxVisualLayer::~CHuiFxVisualLayer()
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxVisualLayer::~CHuiFxVisualLayer - 0x%x "), this);
#endif
    delete iExtBitmapFile;
    delete iParameterManager;
    iParameterManager = NULL;
    }

EXPORT_C TBool CHuiFxVisualLayer::Changed() const
    {
    TBool ret = iParameterManager->HasAnimatedParameters();
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxVisualLayer::Changed - 0x%x , ret %d"), this, ret);    
#endif
    return ret;
    }

EXPORT_C void CHuiFxVisualLayer::AdvanceTime(TReal32 aElapsedTime)
    {
    iParameterManager->AdvanceTime(aElapsedTime);
    }

EXPORT_C TBool CHuiFxVisualLayer::PrepareDrawL(CHuiFxEngine& aEngine)
    {
    iParameterManager->Update(aEngine, VisualRect());
    return ETrue;
    }

EXPORT_C void CHuiFxVisualLayer::Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer &aTarget, CHuiFxRenderbuffer& /*aSource*/, TBool /*aHasSurface*/)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxVisualLayer::Draw - 0x%x "), this);
#endif
    aTarget.BindAsRenderTarget();
    aGc.Push(EHuiGcMatrixModel);
    
    THuiFxEngineType engineType = aEngine.EngineType();
    TRenderBufferType bufferType = aTarget.BufferType();
    if(engineType == EHuiFxEngineGles20)
        {
        if(bufferType == EBufferTypeOffscreen)
            {
            aGc.Enable(CHuiGc::EFeatureBlending, ETrue);
            aGc.SetBlendMode(CHuiGc::EBlendReplace);
            }
        TRect viewPort(TPoint(0,0), aTarget.Size());
        aGc.SetProjection(CHuiGc::EProjectionOrthographic, viewPort);
        }
    
    aGc.Translate(EHuiGcMatrixModel, VisualRect().iTl.iX, VisualRect().iTl.iY, 0.0f);
    ApplyTransformations(aGc);
    aGc.Translate(EHuiGcMatrixModel, -VisualRect().iTl.iX, -VisualRect().iTl.iY, 0.0f);
    
    aGc.SetBlendMode( CHuiGc::EBlendNormal );
    //iVisual->iOpacity.Set( iOpacity );
    iVisual->EffectSetOpacity( iOpacity );   
    
    // Set new clipping region which does not clip anything. 
    // We want always draw visual fully to the aTarget. 
    aGc.PushClip();
    TRect bufferArea = TRect(TPoint(0,0), aTarget.Size()); 
    aGc.SetClip(bufferArea); // this does not transform bufferArea which is fine for us    
    
    // Draw visual content to aTarget
    //iVisual->DrawSelf(aGc, TargetRect());
    iVisual->EffectSetSource(iSrcType==EVisualSrcInput1);
    iVisual->EffectDrawSelf( aGc, TargetRect() );
    iVisual->EffectSetSource(EFalse);
    
    // Restore original clipping
    aGc.PopClip();
    
    if(engineType == EHuiFxEngineGles20)
        {
        if(bufferType == EBufferTypeOffscreen)
            {
            aGc.SetBlendMode(CHuiGc::EBlendNormal);
            }
        aGc.SetProjection(CHuiGc::EProjectionOrthographic);
        }
    
    aGc.Pop(EHuiGcMatrixModel);
    aTarget.UnbindAsRenderTarget();
    }

void CHuiFxVisualLayer::ApplyTransformations(CHuiGc& aGc)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxVisualLayer::ApplyTransformations - 0x%x "), this);
#endif
    // Matrix multiplication is generally not commutative.
    // Therefore the order of the transformations matters.
    // In order to prevent the scaling ang skewing from affecting the translation,
    // if seems wiser to do the translation first, otherwise the results seem to be unpredictable.
    
    // Translation
	TReal32 translationX = 0.0f;
	TReal32 translationY = 0.0f;
	TReal32 translationZ = 0.0f;

    if (iTranslationX != 0.0f)
		{
		translationX = iTranslationX - iVisual->EffectDisplayRect().iTl.iX;
		}
	if (iTranslationY != 0.0f)
        {
        translationY = iTranslationY - iVisual->EffectDisplayRect().iTl.iY;
        }    

	if (iTranslationZ != 0.0f)
        {
		// Z translation not supported by the model
        translationZ = iTranslationZ;
        }    

	  if (translationX != 0.0f || translationY != 0.0f || translationZ != 0.0f)
	  	{
        aGc.Translate(EHuiGcMatrixModel, translationX, translationY, translationZ);
		}
        
    // Scaling
    if (iScaleX != 1.0f || iScaleY != 1.0f || iScaleZ != 1.0f)
        {
        if ( iScaleOriginX != 0 || iScaleOriginY != 0 || iScaleOriginZ != 0 )
            {
            aGc.Translate(EHuiGcMatrixModel, iScaleOriginX, iScaleOriginY, iScaleOriginZ);
            aGc.Scale(EHuiGcMatrixModel, iScaleX, iScaleY, iScaleZ); 
            aGc.Translate(EHuiGcMatrixModel, -iScaleOriginX, -iScaleOriginY, -iScaleOriginZ);
            }
        else
            {
            aGc.Scale(EHuiGcMatrixModel, iScaleX, iScaleY, iScaleZ); 
            }
        }
    
    // Skewing
    if (iSkewAngleX != 0.0f || iSkewAngleY != 0.0f || iSkewAngleZ != 0.0f)
        {
        const TReal32 radsPerDeg = 2.0f * (float)M_PI / 360.0f;
        TReal32 skewX = tan(iSkewAngleX * radsPerDeg);
        TReal32 skewY = tan(iSkewAngleY * radsPerDeg);
        TReal32 skewZ = tan(iSkewAngleZ * radsPerDeg);
        if ( iSkewOriginX != 0 || iSkewOriginY != 0 || iSkewOriginZ != 0 )
            {
            aGc.Translate(EHuiGcMatrixModel, iSkewOriginX, iSkewOriginY, iSkewOriginZ);
            aGc.Shear(EHuiGcMatrixModel, skewX, skewY, skewZ); 
            aGc.Translate(EHuiGcMatrixModel, -iSkewOriginX, -iSkewOriginY, -iSkewOriginZ);
            }
        else
            {
            aGc.Shear(EHuiGcMatrixModel, skewX, skewY, skewZ); 
            }
        }

    // Rotation
    if (iRotationAngle != 0.0f)
        {
        if ( iRotationOriginX != 0 || iRotationOriginY != 0 || iRotationOriginZ != 0 )
            {
            aGc.Translate(EHuiGcMatrixModel, iRotationOriginX, iRotationOriginY, iRotationOriginZ);
            aGc.Rotate(EHuiGcMatrixModel, iRotationAngle, iRotationAxisX, iRotationAxisY, iRotationAxisZ); 
            aGc.Translate(EHuiGcMatrixModel, -iRotationOriginX, -iRotationOriginY, -iRotationOriginZ);
            }
        else
            {
            aGc.Rotate(EHuiGcMatrixModel, iRotationAngle, iRotationAxisX, iRotationAxisY, iRotationAxisZ); 
            }
        }

    // Translation
/*    
    if (iTranslationX != 0.0f || iTranslationY != 0.0f || iTranslationZ != 0.0f)
        {
        aGc.Translate(EHuiGcMatrixModel, iTranslationX, iTranslationY, iTranslationZ);
        }    
*/        
    }

EXPORT_C TBool CHuiFxVisualLayer::VisualArea( TRect& aRect )
    {
    // TODO: cache?
    // TODO: transforms
    //aRect = iVisual->DisplayRect();        
    THuiRealRect rect = iVisual->EffectDisplayRect();
    aRect = rect;
    return ETrue;
    }

EXPORT_C TBool CHuiFxVisualLayer::Margin(TMargins &m)
   {
   m.iLeft = iTranslationX < 0.0 ? -iTranslationX : 0.0;
   m.iRight = iTranslationX > 0.0 ? iTranslationX : 0.0;
   m.iTop = iTranslationY < 0.0 ? -iTranslationY : 0.0;
   m.iBottom = iTranslationY > 0.0 ? iTranslationY : 0.0;
   return ETrue;
   }

EXPORT_C MHuiFxParameter* CHuiFxVisualLayer::Parameter(const TDesC& aName)
    {
    return iParameterManager->Parameter(aName);
    }

EXPORT_C void CHuiFxVisualLayer::RegisterParameterL(const TDesC& aName, TReal32* aValue)
    {
    iParameterManager->RegisterParameterL(aName, aValue);
    }

EXPORT_C void CHuiFxVisualLayer::RegisterParameterL(const TDesC& aName, TRgb* aValue)
    {
    iParameterManager->RegisterParameterL(aName, aValue);
    }

void CHuiFxVisualLayer::SetSourceType( const THuiFxVisualSrcType aSrcType )
    {
    iSrcType = aSrcType; 
    }
    
THuiFxVisualSrcType CHuiFxVisualLayer::SourceType() const
    {
    return iSrcType;
    }
    
void CHuiFxVisualLayer::SetExtBitmapFileL( TDesC& aFilename )
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxVisualLayer::SetExtBitmapFileL - 0x%x, %S "), this, &aFilename);
#endif    
    delete iExtBitmapFile;
    iExtBitmapFile = NULL;
    if ( aFilename.Length() == 0 )
        {
        // don't set an empty filename
        return;
        }
    iExtBitmapFile = HBufC::NewL( aFilename.Length() );
    *iExtBitmapFile = aFilename;
    //TODO: the bitmap from this filename should be loaded as texture to the visual.
    }
void CHuiFxVisualLayer::FxmlVisualInputs(RArray<THuiFxVisualSrcType> &aArray)
    {
    aArray.Append(iSrcType);
    }

TBool CHuiFxVisualLayer::FxmlUsesOpaqueHint() const
    {
    return iOpaqueHint;
    }

void  CHuiFxVisualLayer::SetFxmlUsesOpaqueHint(TBool aValue)
    { 
    iOpaqueHint = aValue; 
    }

TBool CHuiFxVisualLayer::IsSemitransparent() const
    {
    if (iOpacity < 1.f)
        {
        return ETrue;        
        }
    else
        {
        return EFalse;        
        }
    }
