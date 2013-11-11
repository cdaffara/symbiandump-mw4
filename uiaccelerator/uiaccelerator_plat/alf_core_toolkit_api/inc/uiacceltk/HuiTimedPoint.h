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



#ifndef __HUITIMEDPOINT_H__
#define __HUITIMEDPOINT_H__


#include <e32base.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiRealPoint.h>


/**
 * X/Y coordinate pair using timed values.
 *
 * @see THuiTimedValue
 */
class THuiTimedPoint
    {
public:

    /* Constructors and destructor. */

    IMPORT_C THuiTimedPoint();

    /**
     * Constructor that sets default values.
     *
     * @param aX Sets the timed value's initial X component.
     * @param aY Sets the timed value's initial Y component.
     */
    IMPORT_C THuiTimedPoint(TReal32 aX, TReal32 aY) __SOFTFP;


    /* Methods. */

    /**
     * Sets the interpolation style of both components of the point.
     *
     * @param aStyle  Timed value interpolation style.
     */
    IMPORT_C void SetStyle(THuiInterpolationStyle aStyle);

    /**
     * Sets the mapping function for both the X and Y components. For
     * example you can use a mapping function to constrain your
     * visuals onto a path.
     *
     * @note If the mapping function does not support multiple modes
     * (mode for X calculation and mode for Y calculation), both the
     * X and Y components of the timed point will share the same value.
     *
     * The mapping component/mode parameter used to calculate
     * value for X is mode 0, and the value for U is mode 1.
     * @see MHuiMappingFunction::MapValue() is called twice with mode
     * 0 for X and mode 1 for Y.
     * @see CHuiCurvePath for curve path mapping fuction.
     */
    IMPORT_C void SetFunction(MHuiMappingFunction* aFunction);

    /**
     * Sets the target for the timed point.
     *
     * @param aPoint           Target point.
     * @param aTransitionTime  Duration for reaching the target.
     */
    IMPORT_C void Set(const THuiRealPoint& aPoint, TInt aTransitionTime = 0);

    /**
     * Sets the target value of both components of the timed point.
     *
     * @param aValue           New target value.
     * @param aTransitionTime  Duration for reaching the target.
     */
    IMPORT_C void Set(TReal32 aValue, TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Sets the target value for the timed point, with a transition that is
     * done using a specific speed.
     *
     * @param aPoint  Target point.
     * @param aUnitsPerSecond  Units per second during the transition.
     */
    IMPORT_C void SetWithSpeed(const THuiRealPoint& aPoint, TReal32 aUnitsPerSecond) __SOFTFP;

    /**
     * Sets the target value of both components of the timed point.
     *
     * @param aValue  New target value.
     * @param aUnitsPerSecond  Speed of change.
     */
    IMPORT_C void SetWithSpeed(TReal32 aValue, TReal32 aUnitsPerSecond) __SOFTFP;

    /**
     * Returns the current interpolatad value of the point rounded to nearest
     * integer values. Note that the THuiTimedPoint values are
     * floating-point.
     *
     * @see RealNow() For returning the floating-point current value.
     *
     * @return  Current values as a regular TPoint.
     */
    IMPORT_C TPoint Now() const;

    /**
     * Returns the current value of the point.
     *
     * @return  Current values as a regular THuiRealPoint.
     */
    IMPORT_C THuiRealPoint RealNow() const __SOFTFP;

    /**
     * Returns the target value of the point rounded to nearest
     * integer values. Note that the THuiTimedPoint values are
     * floating-point.
     *
     * @see RealTarget() For returning the floating-point values.
     *
     * @return Target values as a normal TPoint - target value rounded to
     * nearest integer.
     */
    IMPORT_C TPoint Target() const;

    /**
     * Returns the target value of the point.
     *
     * @return  Target as a THuiRealPoint.
     */
    IMPORT_C THuiRealPoint RealTarget() const __SOFTFP;

    /**
     * Finishes the current interpolation.
     */
    IMPORT_C void Finish();

    /**
     * Checks if the point has been changed.
     *
     * @return ETrue if X or Y values have changed, EFalse if not.
     *
     * @see THuiTimedValue::Changed()
     */
    TBool Changed() const;

    /**
     * Clears the changed flags for X and Y values.
     *
     * @see THuiTimedValue::Changed()
     */
    void ClearChanged();


private:

    /* Private methods. */


public:

    /* Public member variables. */

    /** X coordinate. */
    THuiTimedValue iX;

    /** Y coordinate. */
    THuiTimedValue iY;

private:
    TInt iSpare1;
    TInt iSpare2;
    };

#endif  // __HUITIMEDPOINT_H__
