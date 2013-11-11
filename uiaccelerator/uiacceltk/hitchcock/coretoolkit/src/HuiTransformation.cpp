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
* Description:   Implementation of CHuiTransformaton. CHuiTransformation 
*                describes a series of matrix transformation operations.
*
*/



#include "uiacceltk/HuiTransformation.h"
#include "HuiRenderPlugin.h"


EXPORT_C CHuiTransformation* CHuiTransformation::NewL()
    {
    CHuiTransformation* self = CHuiTransformation::NewLC();
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CHuiTransformation* CHuiTransformation::NewLC()
    {
    CHuiTransformation* self = new (ELeave) CHuiTransformation();
    CleanupStack::PushL(self);
    return self;
    }


CHuiTransformation::CHuiTransformation()
    : iOriginPoint(0.5f,0.5f)
    {
    }

EXPORT_C CHuiTransformation::~CHuiTransformation()
    {
    iTransforms.ResetAndDestroy();
    }


EXPORT_C TInt CHuiTransformation::Count() const
    {
    return iTransforms.Count();
    }


EXPORT_C TInt CHuiTransformation::NonIdentityCount() const
    {
    TInt i = 0;
    TInt nonIdentityCount = 0;

    for(i = 0; i < iTransforms.Count(); ++i)
        {
        const TTransform& xf = *iTransforms[i];
        switch(xf.iType)
            {
            case ETypeTranslate:
                if(xf.iParams[EHuiTransformParamTranslateX].Now() != 0.0f ||
                   xf.iParams[EHuiTransformParamTranslateY].Now() != 0.0f ||
                   xf.iParams[EHuiTransformParamTranslateZ].Now() != 0.0f)
                    {
                    nonIdentityCount++;
                    }
                break;

            case ETypeScale:
                if(xf.iParams[EHuiTransformParamScaleX].Now() != 1.0f ||
                   xf.iParams[EHuiTransformParamScaleY].Now() != 1.0f ||
                   xf.iParams[EHuiTransformParamScaleZ].Now() != 1.0f)
                    {
                    nonIdentityCount++;
                    }
                break;

            case ETypeRotate:
                if(xf.iParams[EHuiTransformParamRotateAngle].Now() != 0.0f)
                    {
                    nonIdentityCount++;
                    }
                break;
            }
        }
    return nonIdentityCount;
    }


EXPORT_C CHuiTransformation::TTransform& CHuiTransformation::Step(TInt aIndex)
    {
    return (*this)[aIndex];
    }


EXPORT_C CHuiTransformation::TTransform& 
    CHuiTransformation::operator [] (TInt aIndex)
    {
    iChanged = ETrue;
    return *iTransforms[aIndex];
    }


EXPORT_C void CHuiTransformation::LoadIdentity()
    {
    iChanged = ETrue;
    iTransforms.ResetAndDestroy();
    }


EXPORT_C void CHuiTransformation::Translate(TReal32 aX, TReal32 aY) __SOFTFP
    {
    TTransform translate;

    translate.iType = ETypeTranslate;
    translate.iParams[EHuiTransformParamTranslateX] = THuiTimedValue(aX);
    translate.iParams[EHuiTransformParamTranslateY] = THuiTimedValue(aY);

    /*TInt err = */Append(translate);
    }


EXPORT_C void CHuiTransformation::Translate(const THuiTimedValue& aX,
                                            const THuiTimedValue& aY)
    {
    TTransform translate;

    translate.iType = ETypeTranslate;
    translate.iParams[EHuiTransformParamTranslateX] = aX;
    translate.iParams[EHuiTransformParamTranslateY] = aY;

    /*TInt err = */Append(translate);
    }


EXPORT_C void CHuiTransformation::Scale(TReal32 aX, TReal32 aY) __SOFTFP
    {
    TTransform scale;

    scale.iType = ETypeScale;
    scale.iParams[EHuiTransformParamScaleX] = THuiTimedValue(aX);
    scale.iParams[EHuiTransformParamScaleY] = THuiTimedValue(aY);
    scale.iParams[EHuiTransformParamScaleZ] = THuiTimedValue(1.f);

    /*TInt err = */Append(scale);
    }


EXPORT_C void CHuiTransformation::Scale(const THuiTimedValue& aX,
                                        const THuiTimedValue& aY)
    {
    TTransform scale;

    scale.iType = ETypeScale;
    scale.iParams[EHuiTransformParamScaleX] = aX;
    scale.iParams[EHuiTransformParamScaleY] = aY;
    scale.iParams[EHuiTransformParamScaleZ] = THuiTimedValue(1.f);

    /*TInt err = */Append(scale);
    }


EXPORT_C void CHuiTransformation::Scale(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP
    {
    TTransform scale;

    scale.iType = ETypeScale;
    scale.iParams[EHuiTransformParamScaleX] = THuiTimedValue(aX);
    scale.iParams[EHuiTransformParamScaleY] = THuiTimedValue(aY);
    scale.iParams[EHuiTransformParamScaleZ] = THuiTimedValue(aZ);

    /*TInt err = */Append(scale);
    }


EXPORT_C void CHuiTransformation::Scale(const THuiTimedValue& aX,
                                        const THuiTimedValue& aY,
                                        const THuiTimedValue& aZ)
    {
    TTransform scale;

    scale.iType = ETypeScale;
    scale.iParams[EHuiTransformParamScaleX] = aX;
    scale.iParams[EHuiTransformParamScaleY] = aY;
    scale.iParams[EHuiTransformParamScaleZ] = aZ;

    /*TInt err = */Append(scale);
    }


EXPORT_C void CHuiTransformation::Rotate(TReal32 aAngle) __SOFTFP
    {
    Rotate(aAngle, 0.f, 0.f, -1.f);
    }


EXPORT_C void CHuiTransformation::Rotate(const THuiTimedValue& aAngle)
    {
    Rotate(aAngle, 0.f, 0.f, -1.f);
    }


EXPORT_C void CHuiTransformation::Rotate(
    TReal32 aAngle, 
    TReal32 aAxisX, 
    TReal32 aAxisY, 
    TReal32 aAxisZ) __SOFTFP
    {
    TTransform rotate;
    rotate.iType = ETypeRotate;
    rotate.iParams[EHuiTransformParamRotateAngle] = THuiTimedValue(aAngle);
    rotate.iParams[EHuiTransformParamRotateAxisX] = THuiTimedValue(aAxisX);
    rotate.iParams[EHuiTransformParamRotateAxisY] = THuiTimedValue(aAxisY);
    rotate.iParams[EHuiTransformParamRotateAxisZ] = THuiTimedValue(aAxisZ);

    /*TInt err = */Append(rotate);
    }


EXPORT_C void CHuiTransformation::Rotate(
    const THuiTimedValue& aAngle,
    TReal32 aAxisX, 
    TReal32 aAxisY, 
    TReal32 aAxisZ) __SOFTFP
    {
    TTransform rotate;
    rotate.iType = ETypeRotate;
    rotate.iParams[EHuiTransformParamRotateAngle] = aAngle;
    rotate.iParams[EHuiTransformParamRotateAxisX] = THuiTimedValue(aAxisX);
    rotate.iParams[EHuiTransformParamRotateAxisY] = THuiTimedValue(aAxisY);
    rotate.iParams[EHuiTransformParamRotateAxisZ] = THuiTimedValue(aAxisZ);

    /*TInt err = */Append(rotate);
    }


TInt CHuiTransformation::Append(const TTransform& aTransform)
    {
    TTransform* copy = new TTransform;
    if ( !copy )
        {
        return KErrNoMemory;
        }

    *copy = aTransform;
    TInt err = iTransforms.Append(copy);
    
    if ( err == KErrNone )
        {
        iChanged = ETrue;
        }
    else
        {
        delete copy;
        }
    return err;
    }


EXPORT_C void CHuiTransformation::Execute(
    THuiGcMatrix aMatrix, 
    CHuiGc& aGc) const
    {
    for(TInt i = 0; i < iTransforms.Count(); ++i)
        {
        const TTransform& x = *iTransforms[i];
        switch(x.iType)
            {
            case ETypeTranslate:
                aGc.Translate(aMatrix,
                              x.iParams[EHuiTransformParamTranslateX].Now(),
                              x.iParams[EHuiTransformParamTranslateY].Now(),
                              x.iParams[EHuiTransformParamTranslateZ].Now());
                break;

            case ETypeScale:
                aGc.Scale(aMatrix,
                          x.iParams[EHuiTransformParamScaleX].Now(),
                          x.iParams[EHuiTransformParamScaleY].Now(),
                          x.iParams[EHuiTransformParamScaleZ].Now());
                break;

            case ETypeRotate:
                aGc.Rotate(aMatrix,
                           x.iParams[EHuiTransformParamRotateAngle].Now(),
                           x.iParams[EHuiTransformParamRotateAxisX].Now(),
                           x.iParams[EHuiTransformParamRotateAxisY].Now(),
                           x.iParams[EHuiTransformParamRotateAxisZ].Now());
                break;
            }
        }
    }


TBool CHuiTransformation::Changed() const
    {
    if(iChanged)
        {
        return ETrue;
        }

    for(TInt i = 0; i < iTransforms.Count(); ++i)
        {
        const TTransform& xf = *iTransforms[i];
        switch(xf.iType)
            {
            case ETypeTranslate:
                if(xf.iParams[EHuiTransformParamTranslateX].Changed() ||
                   xf.iParams[EHuiTransformParamTranslateY].Changed() ||
                   xf.iParams[EHuiTransformParamTranslateZ].Changed())
                    {
                    return ETrue;
                    }
                break;

            case ETypeScale:
                if(xf.iParams[EHuiTransformParamScaleX].Changed() ||
                   xf.iParams[EHuiTransformParamScaleY].Changed() ||
                   xf.iParams[EHuiTransformParamScaleZ].Changed())
                    {
                    return ETrue;
                    }
                break;

            case ETypeRotate:
                if(xf.iParams[EHuiTransformParamRotateAngle].Changed() ||
                   xf.iParams[EHuiTransformParamRotateAxisX].Changed() ||
                   xf.iParams[EHuiTransformParamRotateAxisY].Changed() ||
                   xf.iParams[EHuiTransformParamRotateAxisZ].Changed())
                    {
                    return ETrue;
                    }
                break;
            }
        }

    return EFalse;
    }


void CHuiTransformation::ClearChanged()
    {
    TInt i = 0;
    TInt k = 0;

    for(i = 0; i < iTransforms.Count(); ++i)
        {
        for(k = 0; k < KHuiTransformParamCount; ++k)
            {
            iTransforms[i]->iParams[k].ClearChanged();
            }
        }

    iChanged = EFalse;
    }

const THuiTimedPoint& CHuiTransformation::OriginPoint() const
    {
    return iOriginPoint;
    }
