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
* Description:   Timed value class
*
*/


#include "alf/alftimedvalue.h"
#include "alf/alfutil.h"

EXPORT_C TAlfTimedValue::TAlfTimedValue()
    :iValueNow(0),
    iValueTarget(0),
    iTimeToTarget(0),
    iInterpolationStyle(0),
    iMappingFunctionIdentifier(0),
    iSpeed(0),
    iFlags(EAlfValueFlagsNone)
    {
    }

EXPORT_C TAlfTimedValue::TAlfTimedValue(TReal32 aInitialValue) __SOFTFP
    :iValueNow(aInitialValue),
    iValueTarget(0),
    iTimeToTarget(0),
    iInterpolationStyle(0),
    iMappingFunctionIdentifier(0),
    iSpeed(0),
    iFlags(EAlfValueNowChanged)
    {
    }

EXPORT_C TAlfTimedValue::TAlfTimedValue(TReal32 aTargetValue, TInt aTime) __SOFTFP
    :iValueNow(0),
    iValueTarget(aTargetValue),
    iTimeToTarget(aTime),
    iInterpolationStyle(0),
    iMappingFunctionIdentifier(0),
    iSpeed(0),
    iFlags(EAlfValueTargetChanged)
    {        
    }
    
    
EXPORT_C void TAlfTimedValue::SetValueNow(const TReal32& aValueNow) __SOFTFP
    {
    iFlags |= EAlfValueNowChanged;    
    iValueNow = aValueNow;
    }
    
EXPORT_C void TAlfTimedValue::SetTarget(const TReal32& aTarget, TInt aTime) __SOFTFP
    {
    iFlags |= EAlfValueTargetChanged;    
    iValueTarget = aTarget;
    iTimeToTarget = aTime;
    }

EXPORT_C void TAlfTimedValue::SetTargetWithSpeed(TReal32 aValue, TReal32 aUnitsPerSecond) __SOFTFP
    {
    iFlags |= EAlfValueTargetChanged;    
    iFlags |= EAlfSpeedChanged;    
    iValueTarget = aValue;
    iSpeed = aUnitsPerSecond;    
    }

EXPORT_C TReal32 TAlfTimedValue::Speed() const __SOFTFP
    {
    return iSpeed;    
    }


EXPORT_C void TAlfTimedValue::SetStyle(TAlfInterpolationStyle aStyle)
    {
    iFlags |= EAlfInterpolationStyleChanged;    
    iInterpolationStyle = aStyle;
    }

EXPORT_C TAlfInterpolationStyle TAlfTimedValue::Style()
    {
    return TAlfInterpolationStyle(iInterpolationStyle);        
    }
    
EXPORT_C void TAlfTimedValue::SetMappingFunctionIdentifier(TInt aIdentifier) 
    {
    iFlags |= EAlfMappingFunctionChanged;    
    iMappingFunctionIdentifier = aIdentifier;
    }

// Todo: real use case for inline perhaps    
EXPORT_C TReal32 TAlfTimedValue::ValueNow() const __SOFTFP
    {
    return iValueNow;
    }

// Todo: real use case for inline perhaps        
EXPORT_C TReal32 TAlfTimedValue::Target() const __SOFTFP
    {
    return iValueTarget;    
    }
// Todo: real use case for inline perhaps        
EXPORT_C TInt TAlfTimedValue::TimeToTargetinMilliSeconds() const
    {
    return iTimeToTarget;
    }

EXPORT_C TInt& TAlfTimedValue::Flags()
    {
    return iFlags;
    }

EXPORT_C TInt TAlfTimedValue::MappingFunctionIdentifier() const 
    {
    if (iFlags&EAlfMappingFunctionChanged)
        {
        return iMappingFunctionIdentifier;
        }
        
    return KErrNotFound;
    }



EXPORT_C TAlfTimedPoint::TAlfTimedPoint()
    {}

EXPORT_C TAlfTimedPoint::TAlfTimedPoint(TReal32 aX, TReal32 aY) __SOFTFP
    {
    iX = TAlfTimedValue(aX);
    iY = TAlfTimedValue(aY);    
    }

EXPORT_C TAlfTimedPoint::TAlfTimedPoint(TReal32 aXTarget, TReal32 aYTarget, TInt aTransitionTime) __SOFTFP
    {
    iX = TAlfTimedValue(aXTarget, aTransitionTime);
    iY = TAlfTimedValue(aYTarget, aTransitionTime);            
    }
    

EXPORT_C void TAlfTimedPoint::SetMappingFunctionIdentifier(TInt aIdentifier)
    {
    // Both use the same function, but the components are different.
    iX.SetMappingFunctionIdentifier(aIdentifier);
    iY.SetMappingFunctionIdentifier(aIdentifier);        
    }

EXPORT_C void TAlfTimedPoint::SetStyle(TAlfInterpolationStyle aStyle)
    {
    iX.SetStyle(aStyle);
    iY.SetStyle(aStyle);        
    }

EXPORT_C void TAlfTimedPoint::SetTarget(const TAlfRealPoint& aPoint, TInt aTransitionTime)
    {
    iX.SetTarget(aPoint.iX, aTransitionTime);
    iY.SetTarget(aPoint.iY, aTransitionTime);                
    }
    
EXPORT_C void TAlfTimedPoint::SetTarget(TReal32 aValue, TInt aTransitionTime) __SOFTFP
    {
    iX.SetTarget(aValue, aTransitionTime);
    iY.SetTarget(aValue, aTransitionTime);                        
    }

EXPORT_C void TAlfTimedPoint::SetTargetWithSpeed(const TAlfRealPoint& aPoint, TReal32 aUnitsPerSecond) __SOFTFP
    {
    iX.SetTargetWithSpeed(aPoint.iX, aUnitsPerSecond);
    iY.SetTargetWithSpeed(aPoint.iY, aUnitsPerSecond);                        
    }
  
EXPORT_C void TAlfTimedPoint::SetTargetWithSpeed(TReal32 aValue, TReal32 aUnitsPerSecond) __SOFTFP
    {
    iX.SetTargetWithSpeed(aValue, aUnitsPerSecond);
    iY.SetTargetWithSpeed(aValue, aUnitsPerSecond);                                
    }
    
EXPORT_C TPoint TAlfTimedPoint::IntValueNow() const
    {
    // Round the float returned by ValueNow to the closest integer.
    return TPoint(AlfUtil::RoundFloatToInt(iX.ValueNow()), AlfUtil::RoundFloatToInt(iY.ValueNow()));
    }

EXPORT_C TPoint TAlfTimedPoint::IntTarget() const
    {
    // Round the float returned by ValueNow to the closest integer.
    return TPoint(AlfUtil::RoundFloatToInt(iX.Target()), AlfUtil::RoundFloatToInt(iY.Target()));
    }



// End of file

