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
* Description:   Implementation for CHuiCurvePath. Curve paths are composed of 
*                path segments and can be used as mapping functions, and in 
*                line visuals.
*
*/



#include "uiacceltk/HuiCurvePath.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiRealPoint.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/huifixmath.h"
#include "huicurvepathprivdata.h"
#include <e32math.h>



EXPORT_C CHuiCurvePath* CHuiCurvePath::NewL()
    {
    CHuiCurvePath* self = CHuiCurvePath::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
    
EXPORT_C CHuiCurvePath* CHuiCurvePath::NewLC()
    {
    // Create a curve path suitable for the current renderer.
    CHuiCurvePath* self = CHuiStatic::Renderer().CreateCurvePathL();
    self->iExtension = new (ELeave) THuiCurvePathExtension;
    self->iExtension->iLooped = ETrue;
    self->iExtension->iLegacyAPIUsed = EFalse;
    self->iExtension->iVisual = NULL;
    CleanupStack::PushL(self);
    return self;
    }


EXPORT_C CHuiCurvePath::CHuiCurvePath()
        : iExtension(NULL), iChanged(ETrue)
    {
    }
    
    
EXPORT_C CHuiCurvePath::~CHuiCurvePath()
    {
    iSegments.Close();
    delete iExtension;
    }
  
    
EXPORT_C void CHuiCurvePath::Reset()
    {
    iSegments.Reset();
    iTotalLength = 0;
    iOffset.Set(TPoint(0, 0));
    iPosOrigin = 0;
    iChanged = ETrue;
    }
    
      
EXPORT_C void CHuiCurvePath::EnableLoop(TBool aEnable)
    {
    iExtension->iLooped = aEnable;
    iChanged = ETrue;
    }


EXPORT_C TBool CHuiCurvePath::Loop() const
    {
    return iExtension->iLooped;
    }
    
    
EXPORT_C TBool CHuiCurvePath::NeedUpdate() const
    {
    if ( iOffset.Changed() )
        {
        return ETrue;
        }
    
    return iNeedUpdate;
    }
    
EXPORT_C void CHuiCurvePath::SetNeedsUpdate()
    {
    iNeedUpdate = ETrue;
    }
    
    
EXPORT_C void CHuiCurvePath::SetOrigin(TReal32 aPosOrigin) __SOFTFP
    {
    iPosOrigin = aPosOrigin;
    iNeedUpdate = ETrue;    
    iChanged = ETrue;
    }


/**
 * @deprecated 
 */
EXPORT_C void 
CHuiCurvePath::AppendLineL(const TPoint& aStart, const TPoint& aEnd, 
                           TReal32 aLength) __SOFTFP
    {
    AppendLineL(THuiRealPoint(aStart), THuiRealPoint(aEnd), aLength);
    iExtension->iLegacyAPIUsed = ETrue;
    }
    

EXPORT_C void 
CHuiCurvePath::AppendLineL(const THuiRealPoint& aStart, const THuiRealPoint& aEnd, 
                           TReal32 aLength) __SOFTFP
    {
    TSegment segment;
    segment.iType = ESegmentTypeLine;
    segment.iOrigin = aStart;
    segment.iDelta = aEnd - aStart;
    segment.iStartAngle = segment.iAngleDelta = 0;
    if(aLength < 0)
        {
        TReal length2 = segment.iDelta.iX * segment.iDelta.iX + 
                        segment.iDelta.iY * segment.iDelta.iY;
        TReal target;
        Math::Sqrt(target, length2);
        segment.iLength = TReal32(target);
        }
    else
        {
        segment.iLength = aLength;
        }
    TReal32 length = CalculateLength();        
    segment.iTotalPos = length;

    User::LeaveIfError(iSegments.Append(segment)); 
    
    iTotalLength = length + segment.iLength;
    iNeedUpdate = ETrue;
    iChanged = ETrue;
    }
    

/**
 * @deprecated 
 */
EXPORT_C void 
CHuiCurvePath::AppendArcL(const TPoint& aOrigin, const TSize& aSize,
                          TReal32 aStartAngle, TReal32 aEndAngle,
                          TReal32 aLength) __SOFTFP
    {
    AppendArcL(THuiRealPoint(aOrigin), THuiRealSize(aSize), aStartAngle, aEndAngle, aLength);    
    iExtension->iLegacyAPIUsed = ETrue;
    }



EXPORT_C void 
CHuiCurvePath::AppendArcL(const THuiRealPoint& aOrigin, const THuiRealSize& aSize,
                          TReal32 aStartAngle, TReal32 aEndAngle,
                          TReal32 aLength) __SOFTFP
    {
    TSegment segment;
    segment.iType = ESegmentTypeArc;
    segment.iOrigin = aOrigin;
    segment.iDelta = THuiRealPoint(aSize.iWidth, aSize.iHeight);
    segment.iStartAngle = aStartAngle;
    segment.iAngleDelta = aEndAngle - aStartAngle;
    if(aLength < 0)
        {
        // Calculate ellipse arc length.
        // Approximation: 2 * pi * sqrt((a^2+b^2)/2) 
        TReal temp = 0;
        TReal a = (aSize.iWidth*aSize.iWidth + 
                   aSize.iHeight*aSize.iHeight) / 2.0;
        Math::Sqrt(temp, a);
        TReal32 angleLength = segment.iAngleDelta;
        if(angleLength < 0)
            {
            angleLength = -angleLength;
            }
        segment.iLength = 2.f * KPi * temp * angleLength / 360.f;
        }
    else
        {
        segment.iLength = aLength;
        }

    TReal32 length = CalculateLength();        
    segment.iTotalPos = length;
        
    User::LeaveIfError(iSegments.Append(segment));
    
    iTotalLength = length + segment.iLength;
    iNeedUpdate = ETrue;
    iChanged = ETrue;
    }


EXPORT_C TReal32 CHuiCurvePath::CalculateLength() const
    {
    TReal32 length = 0;
    
    for(TInt i = 0; i < iSegments.Count(); ++i)
        {
        length += iSegments[i].iLength;
        }
    return length;        
    }
    
    
EXPORT_C TReal32 CHuiCurvePath::Length() const __SOFTFP
    {
    return iTotalLength;
    }
     

EXPORT_C void CHuiCurvePath::Evaluate(TReal32 aPos, THuiRealPoint& aPoint) const __SOFTFP
    {
    if(!iSegments.Count())
        {
        // No segments to evalute.
        aPoint = THuiRealPoint(0.f, 0.f);
        return;
        }
    
    aPos += iPosOrigin;
    
    TReal32 length = Length();
    if(iExtension->iLooped)
        {
        // Wrap to range so that negative positions are mapped to the end.
        HuiUtil::WrapValue(aPos, 0.f, length);
        }
    else
        {
        aPos = Max(0.f, aPos);
        aPos = Min(aPos, length);
        }
    
    if(aPos < 0)
        {
        // Not a valid position on the path. Beginning of the first segment.
        EvaluateSegment(0.f, iSegments[0], aPoint);
        return;
        }                        
        
    for(TInt i = 0; i < iSegments.Count(); ++i)
        {
        const TSegment& segment = iSegments[i];
        
        if(aPos < segment.iLength)
            {
            EvaluateSegment(aPos, segment, aPoint);
            return;
            }
            
        aPos -= segment.iLength;
        }
        
    // The position is past the end of the last segment.
    const TSegment& lastSegment = iSegments[iSegments.Count() - 1];
    EvaluateSegment(lastSegment.iLength, lastSegment, aPoint);
    }


EXPORT_C TInt CHuiCurvePath::SegmentCount() const
    {
    return iSegments.Count();
    }
                         

EXPORT_C CHuiCurvePath::TSegment CHuiCurvePath::Segment(TInt aIndex) const
    {
    return iSegments[aIndex];
    }


EXPORT_C void CHuiCurvePath::EvaluateSegment(TReal32 aPos, const TSegment& aSegment,
                                    THuiRealPoint& aPoint,
                                    THuiRealPoint* aNormal) const
    {
    // aPos is in range [0,width].
    if(aSegment.iLength > 0)
        {
        // Normalize position.
        aPos /= aSegment.iLength;
        }
    
    if(aSegment.iType == ESegmentTypeLine)
        {
        aPoint.iX = aSegment.iOrigin.iX + aPos * aSegment.iDelta.iX;
        aPoint.iY = aSegment.iOrigin.iY + aPos * aSegment.iDelta.iY;
        
        if(aNormal)
            {
            aNormal->iX = -aSegment.iDelta.iY;
            aNormal->iY = aSegment.iDelta.iX;
            HuiUtil::QuickNormalize(*aNormal);
            }
        }
    else if(aSegment.iType == ESegmentTypeArc)
        {
        TReal a = 0.0;
        TReal b = 0.0;
        const TReal angle = ((aSegment.iStartAngle + aPos * aSegment.iAngleDelta) / 180.f ) * KPi;
        
        Math::Cos( a, angle );
        Math::Sin( b, angle );

        aPoint.iX = aSegment.iOrigin.iX + aSegment.iDelta.iX * a;
        aPoint.iY = aSegment.iOrigin.iY + aSegment.iDelta.iY * b;
        
        if(aNormal)
            {
            aNormal->iX = a * aSegment.iDelta.iX;
            aNormal->iY = b * aSegment.iDelta.iY;
            HuiUtil::QuickNormalize(*aNormal);
            
            // Reverse normal, if arc going the other way.
            if(aSegment.iAngleDelta > 0)
                {
                aNormal->iX = -aNormal->iX;
                aNormal->iY = -aNormal->iY;
                }
            }
        }
    else
        {
        // for PC lint
        }

    // Apply an offset.        
    aPoint += iOffset.RealNow();        
    }


EXPORT_C TReal32 CHuiCurvePath::MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP
    {
    THuiRealPoint point;
    Evaluate(aValue, point);
    if(aComponent == 0)
        {
        return point.iX;
        }
    else
        {
        return point.iY;
        }
    }
    
    
EXPORT_C TBool CHuiCurvePath::MappingFunctionChanged() const
    {
    return iChanged;
    }
    
    
EXPORT_C void CHuiCurvePath::MappingFunctionClearChanged()
    {
    iChanged = EFalse;
    }

    
EXPORT_C void CHuiCurvePath::Update(TReal32 /*aStartPos*/, 
                           TReal32 /*aEndPos*/, 
                           TReal32 /*aAlphaFactor*/,
                           MHuiMappingFunction* /*aAlphaFunction*/,
                           MHuiMappingFunction* /*aWidthFunction*/) __SOFTFP
    {
    iNeedUpdate = EFalse;
    iOffset.ClearChanged();
    }
    

EXPORT_C void CHuiCurvePath::Draw(const TPoint& /*aOrigin*/, CHuiGc* /*aGc*/) const
    {
    }

EXPORT_C void CHuiCurvePath::SetVisual(const CHuiVisual* aVisual) const
    {
    if (!iExtension->iLegacyAPIUsed)
        {
        iExtension->iVisual = const_cast<CHuiVisual*>(aVisual);   
        }
    }

EXPORT_C const CHuiVisual* CHuiCurvePath::GetVisual() const
    {
    return iExtension->iVisual;
    }


EXPORT_C TBool CHuiCurvePath::IsLegacyApiUsed() const
	{
	return(iExtension->iLegacyAPIUsed);
	}

EXPORT_C void CHuiCurvePath::CurvePathExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }

