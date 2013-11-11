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
* Description:   Implementation of THuiTimedValue. THuiTimedValue is value 
*                that can change based on time.
*
*/



#include "uiacceltk/HuiTimedValue.h"  // Class definition
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/huifixmath.h"

#include <e32math.h>

EXPORT_C TBool MHuiMappingFunction::MappingFunctionChanged() const
    {
    return EFalse;
    }


EXPORT_C void MHuiMappingFunction::MappingFunctionClearChanged()
    {
    }


EXPORT_C THuiTimedValue::THuiTimedValue(THuiInterpolationStyle aStyle)
        :
          iStyle(aStyle),
          iInternalState(0),
          iInterpolationStartValue(0),
          iTargetValue(0),
          iMapFunction(0),
          iTimeRemainingWhenSuspended(0)
    {
    // Use current time.
    iInterpolationStartTime = CHuiStatic::MilliSecondsSinceStart();
    iInterpolationEndTime = iInterpolationStartTime;

    SetStyle(aStyle);
    }


EXPORT_C THuiTimedValue::THuiTimedValue(TReal32 aInitialValue, THuiInterpolationStyle aStyle) __SOFTFP
        : iStyle(aStyle),
          iInternalState(0),
          iInterpolationStartValue(aInitialValue),
          iTargetValue(aInitialValue),
          iMapFunction(0),
          iTimeRemainingWhenSuspended(0)
    {
    // Use current time.
    iInterpolationStartTime = CHuiStatic::MilliSecondsSinceStart();
    iInterpolationEndTime = iInterpolationStartTime;

    SetStyle(aStyle);
    }


EXPORT_C THuiInterpolationStyle THuiTimedValue::Style() const
    {
    return iStyle;
    }


EXPORT_C void THuiTimedValue::SetStyle(THuiInterpolationStyle aStyle)
    {
    iStyle = aStyle;

    if(iStyle == EHuiTimedValueStyleAdaptive)
        {
        iInternalState |= EFlagAdaptive;
        }
    else
        {
        iInternalState &= ~EFlagAdaptive;
        }
    }


EXPORT_C void THuiTimedValue::SetFunction(MHuiMappingFunction* aFunction)
    {
    TReal32 oldMapped = 0;

    if(!aFunction)
        {
        // We are switching to identity mapping, store the current mapped
        // value.
        oldMapped = Now();
        }

    iMapFunction = aFunction;

    if(!aFunction)
        {
        // No jumping around. Use the previous mapped value.
        Set(oldMapped);
        }
    }


TReal32 THuiTimedValue::SecondsFrom(TUint32 aStart, TUint32 aEnd) const
    {
    TUint32 diff;

    diff = aEnd - aStart;
    
    if (diff > 0x7FFFFFFF)
        {
        // Timer has wrapped, we need to account for overflow.
        diff = (0xFFFFFFFF - aStart) + aEnd;
        }

    return diff / 1000.0;
    }


TReal32 THuiTimedValue::MapValue(TReal32 aValue) const
    {
    if(!iMapFunction)
        {
        // No change.
        return aValue;
        }
    else
        {
        return iMapFunction->MapValue(aValue,
            (iInternalState & EFlagMappingModeMask ? 1 : 0));
        }
    }


EXPORT_C TReal32 THuiTimedValue::Now() const __SOFTFP
    {
    return MapValue(UnmappedNow());
    }

    // This macro is used for modifying the return value appropriately
    // for the clock flag.
#define HUI_EVALUATED_VALUE(aValue)  \
    ((aValue) + (iInternalState & EFlagAddSecondsSinceStart? CHuiStatic::SecondsSinceStart() : 0))

EXPORT_C TReal32 THuiTimedValue::UnmappedNow() const __SOFTFP
    {
    if(iInternalState & EFlagFrozen)
        {
        // Frozen values are not updated in any way.
        return iInterpolationStartValue;
        }

    /** @todo  Wrapping of iInterpolationEndTime and iInterpolationStartTime is 
               not handled at the moment. If wrapping occurs during interpolation, 
               the interpolation is cut short and the value skips to the target. */

    if((iInternalState & EFlagInterpolating) && iInterpolationEndTime > iInterpolationStartTime)
        {
        iInternalState |= EFlagChanged;

        TInt duration = HuiFixMath::FloatToFix(SecondsFrom(iInterpolationStartTime, iInterpolationEndTime));
        TInt elapsed = HuiFixMath::FloatToFix(SecondsFrom(iInterpolationStartTime, CHuiStatic::MilliSecondsSinceStart()));

        if(elapsed > duration)
            {
            // We have reached the destination.
            iInterpolationStartValue = iTargetValue;
            iInterpolationStartTime = iInterpolationEndTime;
            iInternalState &= ~EFlagInterpolating;
            return HUI_EVALUATED_VALUE(iInterpolationStartValue);
            }

        TReal32 t = 0.0f;
        const TInt fixt = HuiFixMath::FixDiv(elapsed,duration);

        switch(iStyle)
            {
            case EHuiTimedValueStyleLinear:
                // Linear interpolation.
                t = HuiFixMath::FixToFloat(fixt);
                break;

            case EHuiTimedValueStyleSineWave:
                {
                TInt angle = HuiFixMath::FixMul(fixt,KFixPi);
                TInt anglePos = HuiFixMath::FixCos(angle);
                t = HuiFixMath::FixToFloat((65535-anglePos)/2);
                break;
                }

            case EHuiTimedValueStyleDecelerate:
                {
                // Decelerating quarter of a sine wave.
                TInt angle = HuiFixMath::FixMul((fixt/2)+32768,KFixPi);
                TInt anglePos = HuiFixMath::FixCos(angle);
                t = HuiFixMath::FixToFloat((((65536 - anglePos) / 2) - 32768) * 2);
                break;
                }

            case EHuiTimedValueStyleAccelerate:
                {
                // Accelerating quarter of a sine wave.
                TInt angle = HuiFixMath::FixMul(fixt/2,KFixPi);
                TInt anglePos = HuiFixMath::FixCos(angle);
                t = HuiFixMath::FixToFloat((65536 - anglePos));
                break;
                }

            case EHuiTimedValueStyleImpulse:
                {
                // Sine wave curve.
                TInt angle = HuiFixMath::FixMul(fixt,KFixPi);
                TInt anglePos = HuiFixMath::FixCos(angle);
                t = HuiFixMath::FixToFloat((65536 - anglePos) / 2);
                break;
                }

            default:
                // No interpolation.
                t = 0;
                break;
            }

        return HUI_EVALUATED_VALUE(iInterpolationStartValue * (1 - t) + iTargetValue * t);
        }
    else
        {
        if(iInternalState & EFlagInterpolating)
            {
            iInternalState |= EFlagChanged;
            iInternalState &= ~EFlagInterpolating;
            }
        return HUI_EVALUATED_VALUE(iInterpolationStartValue);
        }
    }


EXPORT_C TReal32 THuiTimedValue::Target() const __SOFTFP
    {
    return MapValue(iTargetValue);
    }


EXPORT_C void THuiTimedValue::Set(TReal32 aValue) __SOFTFP
    {
    iTargetValue = iInterpolationStartValue = aValue;
    iInternalState &= ~EFlagInterpolating;
    iInternalState |= EFlagChanged;

    CHuiStatic::ContinueRefresh();
    }


EXPORT_C void THuiTimedValue::Set(TReal32 aValue, TInt aMilliSeconds) __SOFTFP
    {
    if(Math::IsNaN(aValue))
        {
        HUI_DEBUG(_L("THuiTimedValue::Set - Math::IsNaN(aValue)"));
        return;
        }
    if(aMilliSeconds <= 0)
        {
        Set(aValue);
        return;
        }

    if(iInternalState & EFlagInterpolating)
        {
        iInterpolationStartValue = UnmappedNow();
        }
    else
        {
        if( HuiUtil::RealCompare(iInterpolationStartValue, aValue ) )
            {
            // This is the current value of the timed value, and it won't
            // change, so we are already at the target.
            return;
            }
        }

    // Adaptive style depends on whether a change is currently underway.
    if(iInternalState & EFlagAdaptive)
        {
        if(iInternalState & EFlagInterpolating)
            {
            iStyle = EHuiTimedValueStyleDecelerate;
            }
        else
            {
            iStyle = EHuiTimedValueStyleSineWave;
            }
        }

    CHuiStatic::ContinueRefresh();

    iTargetValue = aValue;

    // If the difference between the current value and the target value
    // is insignificant, no interpolation is done (it would be subject
    // to round-off problems).
    const TReal32 KEpsilon = .0001;
    TReal32 delta = aValue - iInterpolationStartValue;
    if(delta > -KEpsilon && delta < KEpsilon)
        {
        iInterpolationStartValue = aValue;
        iInternalState &= ~EFlagInterpolating;
        iInternalState |= EFlagChanged;
        return;
        }

    iInterpolationStartTime = CHuiStatic::MilliSecondsSinceStart();

    // Target time is an offset to the current time.
    iInterpolationEndTime = iInterpolationStartTime + aMilliSeconds;

    iTargetValue = aValue;

    iInternalState |= EFlagChanged | EFlagInterpolating;
    }


EXPORT_C void THuiTimedValue::SetWithSpeed(TReal32 aValue,
                                           TReal32 aUnitsPerSecond) __SOFTFP
    {
    if( Math::IsNaN(aUnitsPerSecond) || Math::IsNaN(aValue) )
        {
        HUI_DEBUG(_L("THuiTimedValue::SetWithSpeed - Math::IsNaN()"));
        return;
        }
    TReal32 delta = UnmappedNow() - aValue;

    // don't divide by 0
    if( HuiUtil::RealCompare( delta, 0.f ) || HuiUtil::RealCompare( aUnitsPerSecond, 0.f ) )
        {
        // Already there.
        Set(aValue);
        return;
        }

    if(delta < 0.f)
        {
        delta = -delta;
        }

    Set(aValue, (TInt)((delta / aUnitsPerSecond) * 1000.0));
    }


EXPORT_C TInt THuiTimedValue::RemainingTime() const
    {
    if(iInternalState & EFlagInterpolating)
        {
        TUint32 lastTime = iInterpolationStartTime;
        if(!(iInternalState & EFlagFrozen))
            {
            // If the value is not frozen, just compare against the current time.
            lastTime = CHuiStatic::MilliSecondsSinceStart();
            }

        TReal32 remaining = SecondsFrom(lastTime, iInterpolationEndTime);
        if(remaining < 0)
            {
            return 0;
            }
        return TInt(remaining * 1000);
        }
    else
        {
        // No interpolation occuring.
        return 0;
        }
    }


EXPORT_C void THuiTimedValue::Finish()
    {
    iInterpolationStartTime = iInterpolationEndTime;
    iInterpolationStartValue = iTargetValue;
    }


EXPORT_C void THuiTimedValue::ClearChanged()
    {
    if(iMapFunction)
        {
        iMapFunction->MappingFunctionClearChanged();
        }
    
    if(!(iInternalState & EFlagInterpolating) &&
       !(iInternalState & EFlagAddSecondsSinceStart))
        {
        iInternalState &= ~EFlagChanged;
        }
    }


EXPORT_C void THuiTimedValue::SetMappingMode(TInt aMode)
    {
    switch(aMode)
        {
        case 0:
            iInternalState &= ~EFlagMappingModeMask;
            break;

        case 1:
            iInternalState |= EFlagMappingModeMask;
            break;
            
        default:
            break;
        }
    }


EXPORT_C void THuiTimedValue::EnableClock(TBool aEnable)
    {
    if(aEnable)
        {
        // EFlagChanged is always raised when the clock mode is enabled.
        iInternalState |= EFlagAddSecondsSinceStart | EFlagChanged;
        }
    else
        {
        iInternalState &= ~EFlagAddSecondsSinceStart;
        }
    }


EXPORT_C void THuiTimedValue::Suspend(TBool aDoSuspend)
    {
    if(aDoSuspend && !(iInternalState & EFlagFrozen))
        {
        iInterpolationStartValue = UnmappedNow(); // to report correct value
        iInternalState |= EFlagFrozen;
        iTimeRemainingWhenSuspended = RemainingTime();
        // UnmappedNow() will not update the state of the value any longer.
        }
    else if(!aDoSuspend && (iInternalState & EFlagFrozen))
        {
        iInternalState &= ~EFlagFrozen;

        if (iTimeRemainingWhenSuspended) // move for real
            {
            iInternalState &= ~EFlagInterpolating; // force us to continue from paused position 
                                                   // + to avoid interpolation mode change 
            Set(iTargetValue, iTimeRemainingWhenSuspended);
            iTimeRemainingWhenSuspended = 0;
            }
        else // old functionality
            {
            // Temporal relocation.
            TUint32 difference = CHuiStatic::MilliSecondsSinceStart() - iInterpolationStartTime;
            iInterpolationStartTime += difference;
            iInterpolationEndTime += difference;
            }
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C THuiTimedValue& THuiTimedValue::operator=(const THuiTimedValue& aOtherValue)
    {
    if ( this == &aOtherValue )
        {
        Suspend(EFalse);
        return *this;
        }
    
    iStyle = aOtherValue.iStyle;
    iInternalState = aOtherValue.iInternalState;
    iInterpolationStartTime = aOtherValue.iInterpolationStartTime;
    iInterpolationEndTime = aOtherValue.iInterpolationEndTime;
    iInterpolationStartValue = aOtherValue.iInterpolationStartValue;
    iTargetValue = aOtherValue.iTargetValue;
    iMapFunction = aOtherValue.iMapFunction;
    Suspend(EFalse);
    return *this;
    }
