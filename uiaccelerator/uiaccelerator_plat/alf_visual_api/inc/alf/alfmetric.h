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
* Description:   Definition of TAlfMetric, which is used to achieve 
*                       text size dependent layouts, normalized child coordinate system and 
*                       unified metrics system for all layout calculations. 
*
*/



#ifndef __ALFMETRIC_H__
#define __ALFMETRIC_H__


#include <e32base.h>
#include <e32std.h>

#include <alf/alftextstylemanager.h>

/**
 * Metric units. The unit defines how the @c iMagnitude member of the TAlfMetric 
 * struct is interpreted in practice. The interpretation may be dependent on 
 * the size of the layout where it is being used, the size of a font, or 
 * for example a globally defined constant.
 */ 
enum TAlfUnit
    {
    /** Real pixel that corresponds the pixels on the physical device screen. */
    EAlfUnitPixel,

    /** Normalized to parent layout size (0.0 -- 1.0). */
    EAlfUnitNormalized,
    EAlfUnitParentSize = EAlfUnitNormalized,
    
    /** Relative to the display where the metric is being used (0.0 -- 1.0). */
    EAlfUnitRelativeToDisplay,
    EAlfUnitDisplaySize = EAlfUnitRelativeToDisplay,
    
    /**
     * Layout unit defined by S60 for a particular display device.
     */
    EAlfUnitS60,

    /** 
     * Relative to the size of the visual where the metric is being used (0.0 -- 1.0). 
     * This is particularly useful for anchor layout offsets, and for paddings.
     */
    EAlfUnitRelativeToMySize,
    EAlfUnitMySize = EAlfUnitRelativeToMySize,

    /** relative weight; interpretation depends on sibling elements 
     * (for example, cells in a grid) 
     */
    EAlfUnitWeight,
    
    /** 
     * Not supported at the moment. 
     * Line height of the font of the reference text style (the "em width") 
     */
    EAlfUnitLine,
    
    /** 
     * Not Supported at the moment. 
     * Physical millimeters on the screen (for instance for icons). 
     */
    EAlfUnitMillimeter,
    
    /** 
     * Similar to EAlfUnitMysize. Can be used for example to constrain the aspect ratio of a visual,
     * by constraining one axis to be defined in relation to its orthogonal axis.
     */
    EAlfUnitMyWidth,
    EAlfUnitMyHeight,
    
    /** 
     * Similar to EAlfUnitNormalized. Can be used for example to constrain the aspect ratio of a visual
     * by constraining one axis to be defined in relation to its parent's orthogonal axis.
     */
    EAlfUnitParentWidth,
    EAlfUnitParentHeight,

    /** 
     * Not Supported at the moment. 
     * Similar to EAlfUnitMysize. The minimum of a visual's dimensions.
     */
    EAlfUnitMyDimensionMinimum,
    
    /** 
     * Similar to EAlfUnitMysize. The Average of a visual's dimensions.
     */
    EAlfUnitMyDimensionAverage,
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
 * Usage:
 * @code
 * TAlfMetric metricX(1.0, EAlfUnitNormalized); 
 * TAlfMetric metricY(2.0, EAlfUnitNormalized);
 * //Set Layout base unit using Metrc
 * layout->SetBaseUnit(metricX);
 * 
 * //Create XYMetric using Metric 
 * TAlfXYMetric metricXY(metricX, metricY);
 * //Set Layout base unit using XYMetrc
 * layout->SetBaseUnit( metricXY );
 * 
 * //Create Anchor offset as boxMetric
 * TAlfBoxMetric offset(
 *       TAlfMetric(0.1, EAlfUnitMySize),
 *       TAlfMetric(-0.1, EAlfUnitMySize),
 *       TAlfMetric(0.1, EAlfUnitMySize),
 *       TAlfMetric(-0.1, EAlfUnitMySize));
 * TAlfAnchorAttachmentOrigin attachmentOrigin(EAlfAnchorAttachmentOriginNone);
 * TInt attachmentOrdinal(EAlfAnchorAttachToParent);
 *  
 * //Attach anchor origin
 * anchorLayout->Attach(ordinal, offset, attachmentOrigin, attachmentOrdinal);
 * @endcode
 * 
 */
struct TAlfMetric
    {
public:
    /**
     * Constructor. The constructor defaults to using [1px] units.
     *
     * @param aMagnitude the size of the metric value, for example it may represent a length or a scale factor
     * @param aUnit the unit type in which the metric value is represented
     * @param aReferenceTextStyleID can be used to specify a text style in the case of EAlfUnitLine. 
     */    
    IMPORT_C TAlfMetric(TReal32 aMagnitude = 1.f, TInt aUnit = EAlfUnitPixel, TInt aReferenceTextStyleId = EAlfTextStyleNormal) __SOFTFP;

    /**
     * Constructor. Allows the compiler to pass integers instead of TAlfMetric
     * in parameters, in place of pixel metrics.
     *
     * @param aPixels  Number of pixels for the metric.
     */
    IMPORT_C TAlfMetric(TInt aPixels);
    
public:
    TReal32 iMagnitude; 

    /**
      * @see TAlfUnit
     */
    TInt iUnit;
    
    /**
     * @see CAlfTextStyleManager::TextStyle
     */
    TInt iReferenceTextStyleId;
    };

/**
 * A pair of metrics used to represent a value in both X and Y directions, similar to a TAlfRealPoint.
 */
struct TAlfXYMetric
    {
public:
    /**
     * Constructor.
     */
    IMPORT_C TAlfXYMetric();

    /**
     * Constructor. Uses the same metric for both axes.
     *
     * @param aMetric  Metric for the X and Y axes.
     */
    IMPORT_C TAlfXYMetric(const TAlfMetric& aMetric);

    /**
     * Constructor.
     *
     * @param aX metric for the X axis
     * @param aY metric for the Y axis
     */
    IMPORT_C TAlfXYMetric(const TAlfMetric& aX,  const TAlfMetric& aY);

public:
    TAlfMetric iX;
    TAlfMetric iY;
    };

/**
 * Four metrics used to indicate space around the four edges of a rectangle.
 * Not called a rectangle because those are typically made up of two XY points,
 * while this contains just four edge metrics (named top, bottom, left, and right).
 */
struct TAlfBoxMetric
    {
public:
    /**
     * Constructor.
     */
    IMPORT_C TAlfBoxMetric();
    
    /**
     * Constructor. Uses the same metric on all sides of the box.
     *
     * @param aMetric  Metric for all sides of the box.
     */
    IMPORT_C TAlfBoxMetric(const TAlfMetric& aMetric);

    /**
     * Constructor. The X metric is used for the left and right sides, and the
     * Y metric is used for the top and bottom sides.
     *
     * @param aMetric  Metric for all sides of the box.
     */
    IMPORT_C TAlfBoxMetric(const TAlfXYMetric& aMetric);

    /**
     * Constructor. The top and left edges are specified in a single parameter, 
     * and the bottom and right edges are also specified in a single parameter.
     *
     * @param aTopLeft  Metrics for the left and top sides.
     * @param aBottomRight  Metrics for the right and bottom sides.
     */
    IMPORT_C TAlfBoxMetric(const TAlfXYMetric& aTopLeft, 
        const TAlfXYMetric& aBottomRight);

    /**
     * Constructor.
     *
     * @param aLeft Metric for the left edge of the box.
     * @param aRight Metric for the right edge of the box.
     * @param aTop Metric for the top edge of the box.
     * @param aBottom Metric for the bottom edge of the box.
     */    
    IMPORT_C TAlfBoxMetric(const TAlfMetric& aLeft, const TAlfMetric& aRight,
        const TAlfMetric& aTop, const TAlfMetric& aBottom);
        
public:
    TAlfMetric iLeft;        
    TAlfMetric iRight;
    TAlfMetric iTop;
    TAlfMetric iBottom;
    };


#endif  // __ALFMETRIC_H__
