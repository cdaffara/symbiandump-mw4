/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of THuiMetric, which is used to achieve 
*                       text size dependent layouts, normalized child coordinate system and 
*                       unified metrics system for all layout calculations. 
*
*/



#ifndef __HUIMETRIC_H__
#define __HUIMETRIC_H__


#include <e32base.h>
#include <e32std.h>

#include <uiacceltk/huitextstylemanager.h>
#include <uiacceltk/HuiRealPoint.h>

/**
 * Metric units. The unit defines how the @c iMagnitude member of the THuiMetric 
 * struct is interpreted in practice. The interpretation may be dependent on 
 * the size of the layout where it is being used, the size of a font, or 
 * for example a globally defined constant.
 */ 
enum THuiUnit
    {
    /** Real pixel that corresponds the pixels on the physical device screen. */
    EHuiUnitPixel,

    /** Normalized to parent layout size (0.0 -- 1.0). */
    EHuiUnitNormalized,
    EHuiUnitParentSize = EHuiUnitNormalized,
    
    /** Relative to the display where the metric is being used (0.0 -- 1.0). */
    EHuiUnitRelativeToDisplay,
    EHuiUnitDisplaySize = EHuiUnitRelativeToDisplay,
    
    /**
     * Layout unit defined by S60 for a particular display device.
     * @see CHuiDisplay::UnitValue
     */
    EHuiUnitS60,

    /** 
     * Relative to the size of the visual where the metric is being used (0.0 -- 1.0). 
     * This is particularly useful for anchor layout offsets, and for paddings.
     */
    EHuiUnitRelativeToMySize,
    EHuiUnitMySize = EHuiUnitRelativeToMySize,

    /** relative weight; interpretation depends on sibling elements 
     * (for example, cells in a grid) 
     */
    EHuiUnitWeight,
    
    /** 
     * Not supported at the moment. 
     * Line height of the font of the reference text style (the "em width") 
     */
    EHuiUnitLine,
    
    /** 
     * Not Supported at the moment. 
     * Physical millimeters on the screen (for instance for icons). 
     */
    EHuiUnitMillimeter,
    
    /** 
     * Similar to EHuiUnitMysize. Can be used for example to constrain the aspect ratio of a visual,
     * by constraining one axis to be defined in relation to its orthogonal axis.
     */
    EHuiUnitMyWidth,
    EHuiUnitMyHeight,
    
    /** 
     * Similar to EHuiUnitNormalized. Can be used for example to constrain the aspect ratio of a visual
     * by constraining one axis to be defined in relation to its parent's orthogonal axis.
     */
    EHuiUnitParentWidth,
    EHuiUnitParentHeight,

    /** 
     * Not Supported at the moment. 
     * Similar to EHuiUnitMysize. The minimum of a visual's dimensions.
     */
    EHuiUnitMyDimensionMinimum,
    
    /** 
     * Similar to EHuiUnitMysize. The Average of a visual's dimensions.
     */
    EHuiUnitMyDimensionAverage,
    };


/**
 * A Metric is used to represent a value in a specified unit type. This is
 * to support the definition of Layouts using screen-size independent as well
 * as screen-size dependent measurements.
 * 
 * The metrics are used for different things in different contexts: 
 * coordinate base units, anchor offsets, grid column/row weights, paddings, etc. 
 * The interpretation of the units used in the metric depends on the context, 
 * the type of Layout in which they are used, etc.
 *
 */
struct THuiMetric
    {
public:
    /**
     * Constructor. The constructor defaults to using [1px] units.
     *
     * @param aMagnitude the size of the metric value, for example it may represent a length or a scale factor
     * @param aUnit the unit type in which the metric value is represented
     * @param aReferenceTextStyleID can be used to specify a text style in the case of EHuiUnitLine. 
     */    
    IMPORT_C THuiMetric(TReal32 aMagnitude = 1.f, TInt aUnit = EHuiUnitPixel, TInt aReferenceTextStyleId = EHuiTextStyleNormal) __SOFTFP;

    /**
     * Constructor. Allows the compiler to pass integers instead of THuiMetric
     * in parameters, in place of pixel metrics.
     *
     * @param aPixels  Number of pixels for the metric.
     */
    IMPORT_C THuiMetric(TInt aPixels);

    /**
     * Returns a copy of the metric with the absolute value of the magnitude component.
     */    
    inline THuiMetric Abs() const 
        {
        return THuiMetric(::Abs(iMagnitude), iUnit, iReferenceTextStyleId);
        }

    inline THuiMetric operator * (TReal32 aValue) const
        {
        THuiMetric result = *this;
  	    result.iMagnitude *= aValue;
  	    return result;
        }
    
public:
    TReal32 iMagnitude; 
    
    /**
     * @see THuiUnit
     */
    TInt iUnit;
    
    /**
     * @see CHuiTextStyleManager::TextStyle
     */
    TInt iReferenceTextStyleId;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * A pair of metrics used to represent a value in both X and Y directions, similar to a THuiRealPoint.
 */
struct THuiXYMetric
    {
public:
    /**
     * Constructor.
     */
    IMPORT_C THuiXYMetric();

    /**
     * Constructor. Uses the same metric for both axes.
     *
     * @param aMetric  Metric for the X and Y axes.
     */
    IMPORT_C THuiXYMetric(const THuiMetric& aMetric);

    /**
     * Constructor.
     *
     * @param aX metric for the X axis
     * @param aY metric for the Y axis
     */
    IMPORT_C THuiXYMetric(const THuiMetric& aX, const THuiMetric& aY);
    
    inline THuiXYMetric operator * (const THuiRealPoint& aPoint) const 
        {
        THuiXYMetric result = *this;
        result.iX.iMagnitude *= aPoint.iX;
        result.iY.iMagnitude *= aPoint.iY;
        return result;
        }
        
    inline THuiXYMetric Abs() const 
        {
        return THuiXYMetric(iX.Abs(), iY.Abs());
        }
        

public:
    THuiMetric iX;
    THuiMetric iY;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * Four metrics used to indicate space around the four edges of a rectangle.
 * Not called a rectangle because those are typically made up of two XY points,
 * while this contains just four edge metrics (named top, bottom, left, and right).
 */
struct THuiBoxMetric
    {
public:
    /**
     * Constructor.
     */
    IMPORT_C THuiBoxMetric();
    
    /**
     * Constructor. Uses the same metric on all sides of the box.
     *
     * @param aMetric  Metric for all sides of the box.
     */
    IMPORT_C THuiBoxMetric(const THuiMetric& aMetric);

    /**
     * Constructor. The X metric is used for the left and right sides, and the
     * Y metric is used for the top and bottom sides.
     *
     * @param aMetric  Metric for all sides of the box.
     */
    IMPORT_C THuiBoxMetric(const THuiXYMetric& aMetric);

    /**
     * Constructor. The top and left edges are specified in a single parameter, 
     * and the bottom and right edges are also specified in a single parameter.
     *
     * @param aTopLeft  Metrics for the left and top sides.
     * @param aBottomRight  Metrics for the right and bottom sides.
     */
    IMPORT_C THuiBoxMetric(const THuiXYMetric& aTopLeft, 
        const THuiXYMetric& aBottomRight);

    /**
     * Constructor.
     *
     * @param aLeft Metric for the left edge of the box.
     * @param aRight Metric for the right edge of the box.
     * @param aTop Metric for the top edge of the box.
     * @param aBottom Metric for the bottom edge of the box.
     */    
    IMPORT_C THuiBoxMetric(const THuiMetric& aLeft, const THuiMetric& aRight,
        const THuiMetric& aTop, const THuiMetric& aBottom);
        
public:
    THuiMetric iLeft;        
    THuiMetric iRight;
    THuiMetric iTop;
    THuiMetric iBottom;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


#endif  // __HUIMETRIC_H__
