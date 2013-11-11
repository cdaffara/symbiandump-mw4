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

#ifndef HUIFXPARAMETER_INL
#define HUIFXPARAMETER_INL

#include <e32debug.h>

#include "HuiFxTimeLine.h"

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::CHuiFxGenericParameter(TYPE* aValue):
    iValue(aValue),
    iRelValue(*aValue),
    iTimeLine(NULL),
    iTime(0.0f),
    iAnimationFinished(EFalse),
    iLastFrameDrawn(EFalse),
    iRefPoint(EReferencePointIdentity),
    iRefValue(0.0f),
    iStartRef(EReferencePointUndefined),
    iStartValue(0.0f),
    iStartMultiplier(1.0f),
    iEndRef(EReferencePointUndefined),
    iEndValue(0.0f),
    iEndMultiplier(1.0f)
      {
      }

void CleanupFxParameterL(MHuiFxParameter* aParameter);

template<typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE> *CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::CloneL() const
{
    CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE> *p = new (ELeave) CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>(iValue);
    p->iRelValue = iRelValue;
    if (iTimeLine)
        {
        CleanupFxParameterL(p);
        p->iTimeLine = iTimeLine->CloneL();
        CleanupStack::Pop();
        }
    else
        {
        p->iTimeLine = NULL;
        }
    p->iTime = iTime;
    p->iAnimationFinished = iAnimationFinished;
    p->iLastFrameDrawn = iLastFrameDrawn;
    p->iRefPoint = iRefPoint;
    p->iRefValue = iRefValue;
    p->iStartRef = iStartRef;
    p->iStartValue = iStartValue;
    p->iStartMultiplier = iStartMultiplier;
    p->iEndRef = iEndRef;
    p->iEndValue = iEndValue;
    p->iEndMultiplier = iEndMultiplier;
    p->iValue = iValue;
    return p;
}
void ReRefInternal(TReal32 *aValue, TReal32 **aValue2);
void ReRefInternal(TRgb *aValue, TRgb **aValue2);
void ReRefInternal(TReal32 *aValue, TRgb **aValue2);
void ReRefInternal(TRgb *aValue, TReal32 **aValue2);

template<typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::ReRef(TReal32 *aValue)
    {
    ReRefInternal(aValue, &iValue);
    }
template<typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::ReRef(TRgb *aValue)
    {
    ReRefInternal(aValue, &iValue);
    }
template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::~CHuiFxGenericParameter()
    {
    delete iTimeLine;
    iTimeLine = NULL;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetTimeLine(TIMELINE_TYPE* aTimeLine)
    {
    if (iTimeLine != aTimeLine)
        {
        delete iTimeLine;
        iTimeLine = aTimeLine;
        iAnimationFinished = EFalse;
        iLastFrameDrawn = EFalse;
        }
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetValue(TYPE aValue)
    {
    iRelValue = *iValue = aValue;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
THuiFxParameterType CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::Type() const
    {
    return TYPE_CODE;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
TBool CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::IsAnimated() const
    {
    if (!iTimeLine)
        {
        return EFalse;
        }
    return !iAnimationFinished;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetReferencePoint(THuiFxReferencePoint aRef)
    {
    iRefPoint = aRef;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
THuiFxReferencePoint CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::ReferencePoint() const
    {
    return iRefPoint;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetReferenceValue(TReal32 aValue)
    {
    iRefValue = aValue;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
TReal32 CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::ReferenceValue() const
    {
    return iRefValue;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetStartReference(THuiFxReferencePoint aRef)
    {
    iStartRef = aRef;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
THuiFxReferencePoint CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::StartReference() const
    {
    return iStartRef;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetStartValue(TReal32 aValue)
    {
    iStartValue = aValue;
//    RDebug::Print(_L("parameter iStartValue = %f"), iStartValue );
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetStartMultiplier(TReal32 aValue)
    {
    iStartMultiplier = aValue;
//    RDebug::Print(_L("parameter iStartValue = %f"), iStartValue );
    }


template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
TReal32 CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::StartValue() const
    {
    return iStartValue;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetEndReference(THuiFxReferencePoint aRef)
    {
    iEndRef = aRef;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
THuiFxReferencePoint CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::EndReference() const
    {
    return iEndRef;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetEndValue(TReal32 aValue)
    {
    iEndValue = aValue;
//    RDebug::Print(_L("parameter iEndValue = %f"), iEndValue );
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
TReal32 CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::EndValue() const
    {
    return iEndValue;
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::SetEndMultiplier(TReal32 aValue)
    {
    iEndMultiplier = aValue;
//    RDebug::Print(_L("parameter iStartValue = %f"), iStartValue );
    }


template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::AdvanceTime(TReal32 aElapsedTime)
    {
    if (!iTimeLine || iAnimationFinished)
        {
        return;
        }

    iTime += aElapsedTime;

    switch (iTimeLine->LoopingMode())
        {
        case ELoopingModeRepeat:
            {
            // TODO: precalc
            // TODO: keep everything in fixed point
            TInt fpLoopStart = (TInt)(iTimeLine->LoopStart() * iTimeLine->Duration() * 0x10000);
            TInt fpLoopEnd = (TInt)(iTimeLine->LoopEnd() * iTimeLine->Duration() * 0x10000);
            TInt fpTime = (TInt)(iTime * 0x10000);
            
            if (fpLoopEnd - fpLoopStart > 0)
                {
                fpTime = fpLoopStart + ((fpTime - fpLoopStart) % (fpLoopEnd - fpLoopStart));
                }
            else
                {
                fpTime = fpLoopEnd;
                }
            iTime = fpTime / (TReal32)0x10000;
            break;
            }
        case ELoopingModeNone:
            if (iTime > iTimeLine->Duration())
                {
                if ( iLastFrameDrawn )
                    {
                    iAnimationFinished = ETrue;
                    }
                else
                    {
                    iLastFrameDrawn = ETrue;
                    }
                }
            break;
        }
    }

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
void CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE>::Update()
    {
    // Calculate the current value
    if (iTimeLine)
        {
        iRelValue = iTimeLine->ValueAt(iTime);
//        RDebug::Print(_L("parameter rel value = %f at time %f"), iRelValue, iTime );
        }
        
    TBool extRefUsed = ( iStartRef != EReferencePointUndefined && iEndRef != EReferencePointUndefined );    
        
    if ( extRefUsed )
        {
        // mix start and end values
        // Note: this will break horribly for anything other than scalar values
        ASSERT(TYPE_CODE == EParameterTypeScalar);
//        *iValue = ( 1 - (*reinterpret_cast<TReal32*>(&iRelValue)) ) * iStartValue + (*reinterpret_cast<TReal32*>(&iRelValue)) * iEndValue;
        iRelValue = ( 1 - (*reinterpret_cast<TReal32*>(&iRelValue)) ) * iStartValue * iStartMultiplier  + (*reinterpret_cast<TReal32*>(&iRelValue)) * iEndValue * iEndMultiplier;
//        RDebug::Print(_L("parameter with extrect %f at time %f"), iRelValue, iTime);
        }
    
    // Transform by the reference point
    switch (iRefPoint)
        {
        case EReferencePointIdentity:
        case EReferencePointUndefined:
            *iValue = iRelValue;
            break;
        default:
            // Note: this will break horribly for anything other than scalar values
            ASSERT(TYPE_CODE == EParameterTypeScalar);
            if ( extRefUsed )
                {
                if ( iRefValue != 0 )
                    {
                    *iValue = (*reinterpret_cast<TReal32*>(&iRelValue)) / iRefValue;
                    }
                else
                    {
                    // If the reference value is 0, no calculations make sense.
                    *iValue = iRefValue;
                    }
//                RDebug::Print(_L("scaled parameter with extrect %f at time %f"), *iValue, iTime);
                }
            else
                {
                *iValue = (*reinterpret_cast<TReal32*>(&iRelValue)) * iRefValue;
                }
            break;
        }
    }

#endif // HUIFXPARAMETER_INL
