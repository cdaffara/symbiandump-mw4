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
* Description:   Defines a CHuiBorderBrush class - a brush that draws a border 
*                at the edges of visuals.
*
*/



#ifndef __HUIBORDERBRUSH_H__
#define __HUIBORDERBRUSH_H__


#include <e32base.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/huimetric.h>


/**
 * CHuiBorderBrush draws a border at a configurable distance. It can be inside
 * or outside the visual edges. The horizontal and vertical edges can be
 * configured separately. The border can either be a solid color or
 * texture-mapped. A texture offset is supported for animated texture mapping.
 *
 * @todo  Should affect the visual's dirty region by growing it?
 */
NONSHARABLE_CLASS(CHuiBorderBrush): public CHuiBrush
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructor.
     * @param aThickness Border width in specified metric units.
     * @param aEdgeOffset Border edge offset in specified metric units.
     */
    IMPORT_C static CHuiBorderBrush* NewL(const THuiXYMetric& aThickness,
                                          const THuiXYMetric& aEdgeOffset);

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aThickness Border width in specified metric units.
     * @param aEdgeOffset Border edge offset in specified metric units.
     */
    IMPORT_C static CHuiBorderBrush* NewLC(const THuiXYMetric& aThickness,
                                           const THuiXYMetric& aEdgeOffset);
    /**
     * Constructor.
     * @deprecated
     * @param aThicknessWidth Border width in pixels.
     * @param aThicknessHeight Border height in pixels.
     * @param aEdgeOffsetX Edge offset X coordinate in pixels    
     * @param aEdgeOffsetY Edge offset Y coordinate in pixels    
     *     
     */
    IMPORT_C static CHuiBorderBrush* NewL(TInt aThicknessWidth, TInt aThicknessHeight,
                                          TInt aEdgeOffsetX, TInt aEdgeOffsetY);

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated
     * @param aThicknessWidth Border width in pixels.
     * @param aThicknessHeight Border height in pixels.
     * @param aEdgeOffsetX Edge offset X coordinate in pixels    
     * @param aEdgeOffsetY Edge offset Y coordinate in pixels    
     */
    IMPORT_C static CHuiBorderBrush* NewLC(TInt aThicknessWidth, TInt aThicknessHeight,
                                           TInt aEdgeOffsetX, TInt aEdgeOffsetY);


    /**
     * Destructor.
     */
    ~CHuiBorderBrush();


    /* Methods. */

    /**
     * Sets an image for the borders.
     *
     * @param aImage  Texture image for the borders.
     */
    IMPORT_C void SetImage(const THuiImage& aImage);

    /**
     * Sets a color for the borders.
     *
     * @param aColor  Border color.
     */
    IMPORT_C void SetColor(const TRgb& aColor);
        
    /**
     * Sets a thickness for the borders.
     *
     * @param aThickness  Border width.
     */
    IMPORT_C void SetThickness(const THuiXYMetric& aThickness);

    /**
     * Sets a edge offset for the borders.
     * Zero means the border edge is at the visual edge and
     * extends inwards. Positive values move the border outwards.
     *
     * @param aEdgeOffset  Edge offset.
     */
    IMPORT_C void SetEdgeOffset(const THuiXYMetric& aEdgeOffset);

    /**
     * Sets a image offset for the borders.
     *
     * @param aImageOffset  Image offset
     */
    IMPORT_C void SetImageOffset(const THuiXYMetric& aImageOffset);
    
    /**
     * From CHuiBrush.
     * @see CHuiBrush::Changed()
     */
    TBool Changed() const;
    
    /**
     * From CHuiBrush.
     * @see CHuiBrush::ClearChanged()
     */
    void ClearChanged();

    virtual void ExpandVisualRect(TRect& aRect) const;

    virtual void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;


protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiBorderBrush(TInt aThicknessWidth, TInt aThicknessHeight,
                    TInt aEdgeOffsetX, TInt aEdgeOffsetY);

    /**
     * Constructor.
     */
    CHuiBorderBrush(const THuiXYMetric& aThickness,
                    const THuiXYMetric& aEdgeOffset);

private:

    THuiRealPoint ThicknessInPixels(const CHuiVisual* aVisual) const;
    THuiRealPoint EdgeOffsetInPixels(const CHuiVisual* aVisual) const;
    THuiRealPoint ImageOffsetInPixels(const CHuiVisual* aVisual) const;
    THuiRealPoint PointInPixels(const CHuiVisual* aVisual, const THuiTimedPoint& aPoint, const THuiXYMetric& aMetric) const;

private:

    /* Private methods. */

    /** Color for the borders. */
    TRgb iColor;

    /** Image for the borders. */
    THuiImage iImage;


public:

    /* Public properties. */

    /** Border thickness. */
    THuiTimedPoint iThickness;

    /**
     * Border offset. Zero means the border edge is at the visual edge and
     * extends inwards. Positive values move the border outwards.
     */
    THuiTimedPoint iEdgeOffset;

    /** Texture offset. */
    THuiTimedPoint iImageOffset;

private:

    /** Border thickness metric. */
    THuiXYMetric iThicknessMetric;

    /** Edge offset metric. */
    THuiXYMetric iEdgeOffsetMetric;

    /** Image offset metric. */
    THuiXYMetric iImageOffsetMetric;

    };

#endif  // __HUIBORDERBRUSH_H__
