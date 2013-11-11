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
* Description:   ?Description
*
*/



#include "uiacceltk/HuiTimedPoint.h"  // Class definition
#include "uiacceltk/HuiUtil.h"

EXPORT_C THuiTimedPoint::THuiTimedPoint()
    {
    iX.SetMappingMode(0);
    iY.SetMappingMode(1);
    }


EXPORT_C THuiTimedPoint::THuiTimedPoint(TReal32 aX, TReal32 aY) __SOFTFP
        : iX(aX), iY(aY)
    {
    iX.SetMappingMode(0);
    iY.SetMappingMode(1);
    }


EXPORT_C void THuiTimedPoint::SetStyle(THuiInterpolationStyle aStyle)
    {
    iX.SetStyle(aStyle);
    iY.SetStyle(aStyle);
    }


EXPORT_C void THuiTimedPoint::SetFunction(MHuiMappingFunction* aFunction)
    {
    // Both use the same function, but the components are different.
    iX.SetFunction(aFunction);
    iY.SetFunction(aFunction);
    }


EXPORT_C void THuiTimedPoint::Set(const THuiRealPoint& aPoint,
                                  TInt aTransitionTime)
    {
    iX.Set(aPoint.iX, aTransitionTime);
    iY.Set(aPoint.iY, aTransitionTime);
    }


EXPORT_C void THuiTimedPoint::SetWithSpeed(const THuiRealPoint& aPoint,
                                           TReal32 aUnitsPerSecond) __SOFTFP
    {
    iX.SetWithSpeed(aPoint.iX, aUnitsPerSecond);
    iY.SetWithSpeed(aPoint.iY, aUnitsPerSecond);
    }


EXPORT_C void THuiTimedPoint::Set(TReal32 aValue, TInt aTransitionTime) __SOFTFP
    {
    iX.Set(aValue, aTransitionTime);
    iY.Set(aValue, aTransitionTime);
    }


EXPORT_C void THuiTimedPoint::SetWithSpeed(TReal32 aValue, TReal32 aUnitsPerSecond) __SOFTFP
    {
    iX.SetWithSpeed(aValue, aUnitsPerSecond);
    iY.SetWithSpeed(aValue, aUnitsPerSecond);
    }


EXPORT_C TPoint THuiTimedPoint::Now() const
    {
    // NOTE! the THuiTimeValue Now() and Target() return floating point values
    // Round these floating point values to closest integer values
    return TPoint(HUI_ROUND_FLOAT_TO_INT(iX.Now()), HUI_ROUND_FLOAT_TO_INT(iY.Now()));
    }


EXPORT_C THuiRealPoint THuiTimedPoint::RealNow() const __SOFTFP
    {
    // NOTE! the THuiTimeValue Now() and Target() return floating point values
    return THuiRealPoint(iX.Now(), iY.Now());
    }


EXPORT_C TPoint THuiTimedPoint::Target() const
    {
    // NOTE! the THuiTimeValue Now() and Target() return floating point values
    // Round these floating point values to closest integer values
    return TPoint(HUI_ROUND_FLOAT_TO_INT(iX.Target()), HUI_ROUND_FLOAT_TO_INT(iY.Target()));
    }


EXPORT_C THuiRealPoint THuiTimedPoint::RealTarget() const __SOFTFP
    {
    return THuiRealPoint(iX.Target(), iY.Target());
    }


EXPORT_C void THuiTimedPoint::Finish()
    {
    iX.Finish();
    iY.Finish();
    }


TBool THuiTimedPoint::Changed() const
    {
    return iX.Changed() || iY.Changed();
    }


void THuiTimedPoint::ClearChanged()
    {
    iX.ClearChanged();
    iY.ClearChanged();
    }
