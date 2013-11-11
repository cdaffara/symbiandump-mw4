/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "HuiFxVg10TransformFilter.h"
#include <math.h>
#include "HuiFxConstants.h"


CHuiFxVg10TransformFilter* CHuiFxVg10TransformFilter::NewL()
    {
    CHuiFxVg10TransformFilter* self = new (ELeave) CHuiFxVg10TransformFilter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
void CHuiFxVg10TransformFilter::ConstructL()
    {
    CHuiFxFilter::ConstructL();
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
    }

CHuiFxVg10TransformFilter *CHuiFxVg10TransformFilter::CloneL() const
    {
    CHuiFxVg10TransformFilter *filter = new(ELeave)CHuiFxVg10TransformFilter;
    filter->CHuiFxFilter::CopyFromL(this);
    filter->iTranslationX = iTranslationX;
    filter->iTranslationY = iTranslationY;
    filter->iTranslationZ = iTranslationZ;
    
    filter->iScaleX = iScaleX;
    filter->iScaleY = iScaleY;
    filter->iScaleZ = iScaleZ;
    filter->iScaleOriginX = iScaleOriginX;
    filter->iScaleOriginY = iScaleOriginY;
    filter->iScaleOriginZ = iScaleOriginZ;
    
    filter->iRotationOriginX = iRotationOriginX;
    filter->iRotationOriginY = iRotationOriginY;
    filter->iRotationOriginZ = iRotationOriginZ;
    filter->iRotationAngle = iRotationAngle;
    filter->iRotationAxisX = iRotationAxisX;
    filter->iRotationAxisY = iRotationAxisY;
    filter->iRotationAxisZ = iRotationAxisZ;
    
    filter->iSkewAngleX = iSkewAngleX;
    filter->iSkewAngleY = iSkewAngleY;
    filter->iSkewAngleZ = iSkewAngleZ;
    filter->iSkewOriginX = iSkewOriginX;
    filter->iSkewOriginY = iSkewOriginY;
    filter->iSkewOriginZ = iSkewOriginZ;
    
    filter->CopyParameterL(KLitTranslationX, &filter->iTranslationX, this);
    filter->CopyParameterL(KLitTranslationY, &filter->iTranslationY, this);
    filter->CopyParameterL(KLitTranslationZ, &filter->iTranslationZ, this);
    
    filter->CopyParameterL(KLitScaleX, &filter->iScaleX, this);
    filter->CopyParameterL(KLitScaleY, &filter->iScaleY, this);
    filter->CopyParameterL(KLitScaleZ, &filter->iScaleZ, this);
    filter->CopyParameterL(KLitScaleOriginX, &filter->iScaleOriginX, this);
    filter->CopyParameterL(KLitScaleOriginY, &filter->iScaleOriginY, this);
    filter->CopyParameterL(KLitScaleOriginZ, &filter->iScaleOriginZ, this);
    
    filter->CopyParameterL(KLitRotationOriginX, &filter->iRotationOriginX, this);
    filter->CopyParameterL(KLitRotationOriginY, &filter->iRotationOriginY, this);
    filter->CopyParameterL(KLitRotationOriginZ, &filter->iRotationOriginZ, this);
    filter->CopyParameterL(KLitRotationAngle, &filter->iRotationAngle, this);
    filter->CopyParameterL(KLitRotationAxisX, &filter->iRotationAxisX, this);
    filter->CopyParameterL(KLitRotationAxisY, &filter->iRotationAxisY, this);
    filter->CopyParameterL(KLitRotationAxisZ, &filter->iRotationAxisZ, this);
    
    filter->CopyParameterL(KLitSkewAngleX, &filter->iSkewAngleX, this);
    filter->CopyParameterL(KLitSkewAngleY, &filter->iSkewAngleY, this);
    filter->CopyParameterL(KLitSkewAngleZ, &filter->iSkewAngleZ, this);
    filter->CopyParameterL(KLitSkewOriginX, &filter->iSkewOriginX, this);
    filter->CopyParameterL(KLitSkewOriginY, &filter->iSkewOriginY, this);
    filter->CopyParameterL(KLitSkewOriginZ, &filter->iSkewOriginZ, this);
    return filter;
    }


void CHuiFxVg10TransformFilter::CalculateMargin(TMargins &aMargin) const
{
    aMargin.iLeft = iTranslationX < 0.0 ? -iTranslationX : 0.0;
    aMargin.iRight = iTranslationX > 0.0 ? iTranslationX : 0.0;
    aMargin.iTop = iTranslationY < 0.0 ? -iTranslationY : 0.0;
    aMargin.iBottom = iTranslationY > 0.0 ? iTranslationY : 0.0;
    // TODO, how to deal with translationZ/ scale / rotation / skew?
}

TBool CHuiFxVg10TransformFilter::Draw(CHuiFxEngine& /*aEngine*/, CHuiGc& /*aGc*/,
        CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
        const TRect& /*aTargetRect*/, const TRect& aSourceRect, TBool /*aHasSurface*/)
    {
    aSource.BindAsTexture(ERenderbufferUsageReadOnly);
    aTarget.BindAsRenderTarget();

    VGImage srcImage  = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aSource))->AcquireSubImage(aSourceRect);

    vgLoadIdentity();
    vgScale(iScaleX, iScaleY);
    vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
    vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
    
    identity();
    
    // Matrix multiplication is generally not commutative.
    // Therefore the order of the transformations matters.
    // In order to prevent the scaling ang skewing from affecting the translation,
    // if seems wiser to do the translation first, otherwise the results seem to be unpredictable.
    
    // Translation
    if (iTranslationX != 0.0f || iTranslationY != 0.0f || iTranslationZ != 0.0f)
        {
        translate(iTranslationX, iTranslationY, iTranslationZ);
        }    

    // Scaling
    if (iScaleX != 1.0f || iScaleY != 1.0f || iScaleZ != 1.0f)
        {
        translate(iScaleOriginX, iScaleOriginY, iScaleOriginZ);
        scale(iScaleX, iScaleY, iScaleZ); 
        translate(-iScaleOriginX, -iScaleOriginY, -iScaleOriginZ);
        }
    
    // Skewing
    if (iSkewAngleX != 0.0f || iSkewAngleY != 0.0f || iSkewAngleZ != 0.0f)
        {
        const TReal32 radsPerDeg = 2.0f * (float)M_PI / 360.0f;
        TReal32 skewX = tan(iSkewAngleX * radsPerDeg);
        TReal32 skewY = tan(iSkewAngleY * radsPerDeg);
        TReal32 skewZ = tan(iSkewAngleZ * radsPerDeg);
        translate(iSkewOriginX, iSkewOriginY, iSkewOriginZ);
        shear(skewX, skewY, skewZ); 
        translate(-iSkewOriginX, -iSkewOriginY, -iSkewOriginZ);
        }

    // Rotation
    if (iRotationAngle != 0.0f)
        {
        translate(iRotationOriginX, iRotationOriginY, iRotationOriginZ);
        rotate(iRotationAngle, iRotationAxisX, iRotationAxisY, iRotationAxisZ); 
        translate(-iRotationOriginX, -iRotationOriginY, -iRotationOriginZ);
        }

/*
    // Translation
    if (iTranslationX != 0.0f || iTranslationY != 0.0f || iTranslationZ != 0.0f)
        {
        translate(iTranslationX, iTranslationY, iTranslationZ);
        }    
*/
        
   ASSERT(!"TODO: implement the following:");
    // TODO: project vertices
    // TODO: create warp quad matrix from projected vertices,
    // see http://torus.untergrund.net/misc/projective_image_warping.pdf
    // TODO load matrix
    vgDrawImage(srcImage);
    
    // TODO: pixel relative parameters
    HUIFX_VG_INVARIANT();
    (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aSource))->ReleaseSubImage(srcImage);

    aTarget.UnbindAsRenderTarget();
    aSource.UnbindAsTexture();
    
    return ETrue;
    }
