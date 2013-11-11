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



#ifndef HUIFXTIMELINE_INL_
#define HUIFXTIMELINE_INL_

#include <e32def.h>
#include "uiacceltk/huifixmath.h"

template <typename TYPE, typename FPTYPE>
RHuiFxTimeLine<TYPE, FPTYPE>::RHuiFxTimeLine():
    iLastIndex(0),
    iDuration(5.0f),
    iLoopStart(0.0f),
    iLoopEnd(0.0f),
    iLoopingMode(ELoopingModeNone),
    iInterpolationMode(EInterpolationModeHold)
    {
    }

template <typename TYPE, typename FPTYPE>
RHuiFxTimeLine<TYPE, FPTYPE>::~RHuiFxTimeLine()
    {
    iKeyFrames.Close();
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::SetLoopStart(TReal32 aTime)
    {
    iLoopStart = aTime;
    }

template <typename TYPE, typename FPTYPE>
TReal32 RHuiFxTimeLine<TYPE, FPTYPE>::LoopStart() const
    {
    return iLoopStart;
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::SetLoopEnd(TReal32 aTime)
    {
    iLoopEnd = aTime;
    }

template <typename TYPE, typename FPTYPE>
TReal32 RHuiFxTimeLine<TYPE, FPTYPE>::LoopEnd() const
    {
    return iLoopEnd;
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::SetDuration(TReal32 aDuration)
    {
    // Can't change this when keyframes have been defined
    __ASSERT_DEBUG(iKeyFrames.Count() == 0, User::Invariant());
    iDuration = aDuration;
    }

template <typename TYPE, typename FPTYPE>
TReal32 RHuiFxTimeLine<TYPE, FPTYPE>::Duration() const
    {
    return iDuration;
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::SetInterpolationMode(THuiFxInterpolationMode aMode)
    {
    iInterpolationMode = aMode;
    }

template <typename TYPE, typename FPTYPE>
THuiFxInterpolationMode RHuiFxTimeLine<TYPE, FPTYPE>::InterpolationMode() const
    {
    return iInterpolationMode;
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::SetLoopingMode(THuiFxLoopingMode aMode)
    {
    iLoopingMode = aMode;
    }

template <typename TYPE, typename FPTYPE>
THuiFxLoopingMode RHuiFxTimeLine<TYPE, FPTYPE>::LoopingMode() const
    {
    return iLoopingMode;
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::AppendKeyFrameL(TReal32 aTime, TYPE aValue)
    {
    // The aux values don't matter (non-bezier), just pass aValue
    AppendKeyFrameL(aTime, aValue, aValue, aValue);
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::AppendKeyFrameL(TReal32 aTime, TYPE aValue, TYPE aAuxValue1)
    {
    // The second aux value doesn't matter (quad bezier), just pass aValue
    AppendKeyFrameL(aTime, aValue, aAuxValue1, aValue);
    }

template <typename TYPE, typename FPTYPE>
void RHuiFxTimeLine<TYPE, FPTYPE>::AppendKeyFrameL(TReal32 aTime, TYPE aValue, TYPE aAuxValue1, TYPE aAuxValue2)
    {
    aTime *= iDuration;
    
    // Don't allow out of order keyframes
    if (iKeyFrames.Count() &&
        iKeyFrames[iKeyFrames.Count() - 1].iTime >= aTime)
        {
        User::Leave(KErrArgument);
        }
    
    TKeyFrame k;
    k.iTime = aTime;
    k.iFpTime = (TInt)(k.iTime * 0x10000);
    k.iValue = aValue;
    k.iFpValue = ToFixedPoint(aValue);
    k.iAuxValue[0] = aAuxValue1;
    k.iAuxValue[1] = aAuxValue2;
    k.iFpAuxValue[0] = ToFixedPoint(aAuxValue1);
    k.iFpAuxValue[1] = ToFixedPoint(aAuxValue2);
    iKeyFrames.AppendL(k);
    }

template <typename TYPE, typename FPTYPE>
TYPE RHuiFxTimeLine<TYPE, FPTYPE>::ValueAt(TReal32 aTime)
    {
    TInt i;
    
    if (!iKeyFrames.Count())
        {
        return 0;
        }
    
    // Can we use the last index?
    if (iLastIndex < iKeyFrames.Count() - 1 &&
        iKeyFrames[iLastIndex].iTime <= aTime)
        {
        i = iLastIndex;
        }
    else
        {
        i = 0;
        }
    
    for (; i < iKeyFrames.Count() - 1; i++)
        {
        if (iKeyFrames[i + 1].iTime > aTime)
            {
            break;
            }
        }
    
    iLastIndex = i;
    
    if (iInterpolationMode == EInterpolationModeHold ||
        i == iKeyFrames.Count() - 1)
        {
        return iKeyFrames[i].iValue;
        }
    
    ASSERT(i < iKeyFrames.Count() - 1);
    TInt dt = iKeyFrames[i + 1].iFpTime - iKeyFrames[i].iFpTime;
    TInt t = (TInt)(aTime * 0x10000); 

    ASSERT(dt > 0);
    t -= iKeyFrames[i].iFpTime;
    TInt64 t2 = t;
    t = (TInt)((t2 << 16) / dt);
    ASSERT(t >= 0 && t <= 0x10000);

    FPTYPE a = iKeyFrames[i].iFpValue;
    FPTYPE b = iKeyFrames[i + 1].iFpValue;
    FPTYPE c = 0; // only used for Bezier
    TInt64 t64 = t; // time as 64bit integer
//    TInt angle = 0; // used for sine curves
//    TInt anglePos = 0; // used for sine curves
    
    switch (iInterpolationMode)
        {
        case EInterpolationModeLinear:
            { 
            return FromFixedPoint(Blend(a, b, t));
            break;
            }
            
        case EInterpolationModeQuadraticBezier:
            {
            // a has been set outside the switch to iKeyFrames[i].iFpValue
            b = iKeyFrames[i].iFpAuxValue[0];
            c = iKeyFrames[i + 1].iFpValue;
            TInt64 oneminust = 0x10000 - t;

            ASSERT(0 <= oneminust && oneminust <= 0x10000);
            
            TInt64 wa = ((oneminust * oneminust) >> 16);            // (1 - t)^2
            TInt64 wb = (t64 * oneminust) >> 15;                    // 2t(1 - t)
            TInt64 wc = (t64 * t64) >> 16; wc = (wc * t64) >> 16;   // t^2
            
            // All weights [0,1]
            ASSERT(0 <= wa && wa <= 0x10000);
            ASSERT(0 <= wb && wb <= 0x10000);
            ASSERT(0 <= wc && wc <= 0x10000);
            return FromFixedPoint(WeightedSum4(a, b, c, a, (TInt)wa, (TInt)wb, (TInt)wc, 0));
            }
            break;
            
        case EInterpolationModeCubicBezier:
            {
            // a has been set outside the switch to iKeyFrames[i].iFpValue
            b = iKeyFrames[i].iFpAuxValue[0];
            c = iKeyFrames[i].iFpAuxValue[1];
            FPTYPE d = iKeyFrames[i + 1].iFpValue;
            TInt64 oneminust = 0x10000 - t;
            
            ASSERT(0 <= oneminust && oneminust <= 0x10000);

            TInt64 tsq = t64 * t64;
            TInt64 oneminustsq = oneminust * oneminust;
            
            // (1 - t)^3 (48 bits if t = 0)
            TInt64 wa = (oneminust * oneminustsq) >> 32;
            // 3t(1 - t)^2
            TInt64 wb = (3 * t64 * oneminustsq) >> 32;
            // 3t^2(1 - t)
            TInt64 wc = (3 * tsq * oneminust) >> 32;
            // t^3
            TInt64 wd = (t64 * tsq) >> 32;
            
            // All weights [0,1]
            ASSERT(0 <= wa && wa <= 0x10000);
            ASSERT(0 <= wb && wb <= 0x10000);
            ASSERT(0 <= wc && wc <= 0x10000);
            ASSERT(0 <= wd && wd <= 0x10000);
            return FromFixedPoint(WeightedSum4(a, b, c, d, wa, wb, wc, wd));
            }
            break;
            
        case EInterpolationModeInQuad:
            {
            // definition: c*(t/=d)*t + b
            TInt quadt = (t64 * t64) >> 16;
            return FromFixedPoint(Blend(a, b, quadt));
            break;
            }
        case EInterpolationModeOutQuad:
            {
            // definition: -c *(t/=d)*(t-2) + b
            // our t is aldready divided by d, so the coefficient becomes
            // (2 - t)*t = 2*t - t*t
            TInt quadt = (t64 * t64) >> 16;
            TInt twot = t << 1; // t*2
            twot = twot - quadt;
            return FromFixedPoint(Blend(a, b, twot));
            }
            break;
        case EInterpolationModeInOutQuad:
            {
            // acceleration until halfway, then deceleration
            // if((t/=d/2) < 1 c/2*t*t + b
            // else -c/2 *((--t)*(t-2) - 1) + b
            // Out t has been divided by d, but it must be multiplied by 2 to get t/=d/2
            TInt64 twot = t64 << 1; // t*2 (t/=d/2), this is what is used instead of t in the following equations
            
            TInt64 quadt = (twot * twot) >> 17; // (t*t)/2;
            if ( twot < 0x10000 )
                {
                return FromFixedPoint(Blend(a, b, quadt));
                }
            else
                {
                // -((t-1)*(t-1-2)-1)/2 = -((t-1)*(t-3)-1)/2 = -((t*t-4*t+3)-1)/2
                // = -(t*t-4*t+2)/2 = 2*t-t*t/2-1
                twot = (twot << 1) - quadt - 0x10000; 
                return FromFixedPoint(Blend(a, b, (TInt)twot));
                }
            }
            break;
        case EInterpolationModeOutInQuad:
            {
            // decleration until halfway, then acceleration
            // Out t has been divided by d, but it must be multiplied by 2 to get t/=d/2
            
            TInt64 twot = t64 << 1; // t*2 (t/=d/2), this is what is used instead of t in the following equations

            if ( twot < 0x10000 )
                {
                TInt64 quadt = (twot * twot) >> 16;
                twot = twot << 1; // t*2
                twot = twot - quadt;
                b = ( a + b ) / 2;
                return FromFixedPoint(Blend(a, b, (TInt)twot));
                }
            else
                {
                twot = twot - 0x10000;
                TInt64 quadt = (twot * twot) >> 16;
                a = ( a + b ) / 2;
                return FromFixedPoint(Blend(a, b, (TInt)quadt));
                }
            }
            break;
        case EInterpolationModeInBack:
            {
            // Easing equation function for a back (overshooting cubic easing:
            // (s+1)*t^3 - s*t^2 easing in: accelerating from zero velocity.
            c = iKeyFrames[i].iFpAuxValue[0];  // This is already scaled to the same fixed scale as time
            TInt64 quadt = (t64 * t64) >> 16;
            TInt64 cubict = (quadt * t64) >> 16;
            TInt64 splusone = c + 0x10000;
            c = (splusone * cubict + c * quadt) >> 16;
            
            return FromFixedPoint(Blend(a, b, c));
            }
            break;
        case EInterpolationModeOutBack:
            {
            // Easing equation function for a back (overshooting cubic easing:
            // (s+1)*t^3 - s*t^2 easing out: decelerating from zero velocity.
            
            // definition: c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b (note variable names not ours)
            // this becomes (s+1)*(t-1)^3 + s*(t-1)^2 + 1
            c = iKeyFrames[i].iFpAuxValue[0]; // this is s
            
            TInt64 tminusone = t - 0x10000;
            TInt64 quadt = (tminusone * tminusone) >> 16;
            TInt64 cubict = (quadt * tminusone) >> 16;
            TInt64 splusone = c + 0x10000;
            c = ((splusone * cubict + c * quadt) >> 16) + 0x10000; // TP 1 => 0x10000
            
            return FromFixedPoint(Blend(a, b, c));
            }
            break;
        case EInterpolationModeInOutBack:
            {
            // Easing equation function for a back (overshooting cubic easing:
            // (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
            
            // definition (note variable names not ours):
            // if((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b
            // else return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b

            // Out t has been divided by d, but it must be multiplied by 2 to get t/=d/2
            TInt64 twot = t64 << 1; // t*2 (t/=d/2), this is what is used instead of t in the following equations

            c = iKeyFrames[i].iFpAuxValue[0];
            TInt64 magic1 = ToFixedPoint( 1.525 );
            c = ( magic1 * c ) >> 16;
            
            if ( twot < 0x10000 )
                {
                // ((s+1)*t^3 - s*t^2)/2
                TInt64 quadt = (twot * twot) >> 16;
                TInt64 cubict = (quadt * twot) >> 16;
                quadt = (c * quadt) >> 16;
                cubict = ((c + 0x10000) * cubict) >> 16;
                c = (cubict - quadt) >> 1;
                return FromFixedPoint(Blend(a, b, c));
                }
            else
                {
                twot = twot - 0x20000; // t-=2
                // ((s+1)*t^3 - s*t^2 + 2)/2
                TInt64 quadt = (twot * twot) >> 16;
                TInt64 cubict = (quadt * twot) >> 16;
                quadt = (c * quadt) >> 16;
                cubict = ((c + 0x10000) * cubict) >> 16;
                c = (cubict - quadt + 0x20000) >> 1;
                return FromFixedPoint(Blend(a, b, c));
                }
            }
            break;
        case EInterpolationModeOutInBack:
            {
            // Easing equation function for a back (overshooting cubic easing:
            // (s+1)*t^3 - s*t^2 easing out/in: deceleration until halfway, then acceleration.
            c = iKeyFrames[i].iFpAuxValue[0];
            TInt64 twot = t64 << 1; // t*2
            
            if ( twot < 0x10000 ) // TP t => twot (NOTE, THIS CHANGE IS STILL UNCERTAIN IF IT IS VALID!)
                {
                // outback
                // (s+1)*(t-1)^3 + s*(t-1)^2 + 1
                TInt64 tminusone = twot - 0x10000;
                TInt64 quadt = (tminusone * tminusone) >> 16;
                TInt64 cubict = (quadt * tminusone) >> 16;
                TInt64 splusone = c + 0x10000;
                c = ((splusone * cubict + c * quadt) >> 16) + 0x10000; // TP 1 => 0x10000
                b = ( a + b ) / 2;
                return FromFixedPoint(Blend(a, b, c));
                }
            else
                {
                // inback
                //["inback"]((t*2)-d, b+c/2, c/2, d, s)
                // (s+1)*t^3 - s*t^2
                twot = twot - 0x10000;
                TInt64 quadt = (twot * twot) >> 16;
                TInt64 cubict = (quadt * twot) >> 16;
                TInt64 splusone = c + 0x10000;
                c = (splusone * cubict + c * quadt) >> 16;
                a = ( a + b ) / 2;
                return FromFixedPoint(Blend(a, b, c));
                }
            break;
            }
/*            
        case EInterpolationModeDecelerate:
            {
            // Decelerating quarter of a sine wave.
            angle = HuiFixMath::FixMul((t/2)+32768,KFixPi);
            anglePos = HuiFixMath::FixCos(angle);
            t = (((65536 - anglePos) / 2) - 32768) * 2;
            return FromFixedPoint(Blend(a, b, t));
            }
            break;
        case EInterpolationModeAccelerate:
            {
            // Accelerating quarter of a sine wave.
            angle = HuiFixMath::FixMul( ( t / 2 ), KFixPi );
            anglePos = HuiFixMath::FixCos( angle );
            t = 65536 - anglePos;
            return FromFixedPoint( Blend( a, b, t ) );
            }
            break;
        case EInterpolationModeImpulse:
            {
            // Sine wave curve.
            angle = HuiFixMath::FixMul( t, KFixPi );
            anglePos = HuiFixMath::FixCos( angle );
            t = ( 65536 - anglePos ) / 2;
            return FromFixedPoint( Blend( a, b, t ) );
            }
            break;
*/            
        default:
            {
            // Unsupported interpolation modes default to linear
            return FromFixedPoint(Blend(a, b, t));
            }
            break;
        }
    // not reached
    return 0;
    }

template <typename TYPE, typename FPTYPE>
RHuiFxTimeLine<TYPE, FPTYPE>* RHuiFxTimeLine<TYPE, FPTYPE>::CloneL()
    {
    RHuiFxTimeLine* dup = new (ELeave) RHuiFxTimeLine;
    
    dup->iTime = iTime;
    dup->iDuration = iDuration;
    
    TInt err = KErrNone;
    for (TInt i = 0; i < iKeyFrames.Count() && err == KErrNone; ++i)
        {
        err = dup->iKeyFrames.Append(iKeyFrames[i]);
        }
    if ( err != KErrNone )
        {
        delete dup;
        User::Leave( err );
        }
    
    dup->iLastIndex = iLastIndex;
    dup->iLoopStart = iLoopStart;
    dup->iLoopEnd = iLoopEnd;
    dup->iLoopingMode = iLoopingMode;
    dup->iInterpolationMode = iInterpolationMode;
    
    return dup;
    }

#endif /*HUIFXTIMELINE_INL_*/
