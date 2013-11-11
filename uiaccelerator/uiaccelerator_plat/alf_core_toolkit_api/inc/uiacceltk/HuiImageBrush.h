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
* Description:   A brush that is able to draw images.
*
*/



#ifndef __HUIImageBrush_H__
#define __HUIImageBrush_H__


#include <e32base.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/HuiImageVisual.h>


/**
 * HuiImageBrush is a brush that can be used to attach (draw) images to
 * visuals. For example: backgrounds, pop-up icons or other image-based
 * decorations.
 *
 * Has also ability to stretch-fit the image to the area and draw borders.
 * 
 */
NONSHARABLE_CLASS(CHuiImageBrush) : public CHuiBrush
    {
public:

    /* Constructors and destructor. */
    
    /** @beginAPI */

    /**
     * Constructs a new image brush.
     *
     * @param aImage            Texture image for this brush.
     */
    IMPORT_C static CHuiImageBrush* NewL(const THuiImage & aImage);

    /**
     * Constructs a new image brush.
     *
     * @param aImage            Texture image for this brush.
     * @note 					The new brush is left on the cleanup stack.
     */
    IMPORT_C static CHuiImageBrush* NewLC(const THuiImage & aImage);

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiImageBrush();


    /* Methods. */
    
    /**
     * Sets the scaling mode for the bitmap.
     * An unscaled image will be drawn at a 1:1 mapping between its
     * texture and the screen. A scaled image will be resized to fit the
     * visual's size exactly.
     * If unset, the default is EScaleFit.
     * 
     * @param aScaleMode The scaling mode of this image visual.
     *
     * @see CHuiImageVisual::SetScaleMode()
     */
    IMPORT_C void SetScaleMode( CHuiImageVisual::TScaleMode aScaleMode );

    /**
     * Sets an image for this brush.
     *
     * @param aImage            Texture image for this brush.
     * 
     * @see SetBorders() 	    Call this if the image also includes 
     * 							borders.
     */
    IMPORT_C void SetImage(const THuiImage& aImage);

    /**
     * Returns the image of the brush.
     */
    IMPORT_C const THuiImage& Image() const;

	/**
	 * Define widths and heights for the borders of this image brush.
	 * 
     * @param aLeftBorderWidth  Specifies if this image brush should also
     * 							draw a left border. Positive values specify
     * 							border the visuals left edge 
     * 							(extending the effective area of the visual),
     * 							negative values specify border inside the
     * 							area defined by the visual (visuals effective 
     * 							area is not extended).
     * 
     * 							Also the absolute value defines the width 
     * 							of the left border section of the image, 
     * 							in pixel columns. 
     * 							
     * 							Set to zero to draw without border.
     *
     * @param aRightBorderWidth Specifies if this image brush should also
     * 							draw a right border. Positive values specify
     * 							border outside the visuals right 
     * 							edge (extending the effective area of the 
     * 							visual), negative values specify border inside 
     * 							the	area defined by the visual (visuals 
     * 							effective area is not extended).
     * 
     * 							Also the absolute value defines the width 
     * 							of the right border section of the image, 
     * 							in pixel columns. 
     * 							
     * 							Set to zero to draw without border.
     * 
     * @param aTopBorderHeight  Specifies if this image brush should also
     * 							draw a top border. Positive values specify
     * 							border on top of the visuals top edge 
     * 							(extending the effective area of the visual),
     * 							negative values specify border inside the
     * 							area defined by the visual (visuals effective 
     * 							area is not extended).
     * 
     * 							Also the absolute value defines the height 
     * 							of the top border section of the image, 
     * 							in pixel rows. 
     * 							
     * 							Set to zero to draw without border.
     *
     * @param aBottomBorderHeight  Specifies if this image brush should also
     * 							draw a bottom border. Positive values specify
     * 							border below the visuals bottom edge 
     * 							(extending the effective area of the visual),
     * 							negative values specify border inside the
     * 							area defined by the visual (visuals effective 
     * 							area is not extended).
     * 
     * 							Also the absolute value defines the height 
     * 							of the top border section of the image, 
     * 							in pixel rows. 
     * 							
     * 							Set to zero to draw without border.
     * 
     * 
	 */                           
	IMPORT_C void SetBorders(TInt aLeftBorderWidth = 0,
                             TInt aRightBorderWidth = 0,
                             TInt aTopBorderHeight = 0,
                             TInt aBottomBorderHeight = 0);

	/**
	 * Returns true if borders should be drawn.
	 */
	IMPORT_C TBool BorderDrawingEnabled() const;

	/**
	 * Returns true if center should be also be drawn (not just borders).
	 */
	IMPORT_C TBool CenterDrawingEnabled() const;

    /** @endAPI */

    // From CHuiBrush ..

    IMPORT_C virtual void ExpandVisualRect(TRect& aRect) const;
    
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

    IMPORT_C virtual void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;

    virtual TInt Type2() const { return EImageBrush; }

protected:

    /* Constructors. */

    /**
     * Protected constructor.
     * @see NewL
     * @see NewLC
     */
    CHuiImageBrush(const THuiImage& aImage);

private:

    /* Private variables. */

    /** Image for this brush. */
    THuiImage iImage;
    
    /** Scalingmode of the visual. */
    CHuiImageVisual::TScaleMode iScaleMode;

    /** Border width for the left border, in pixels. 0 means no border. */
    TInt iLeftBorderWidth;
    /** Border width for the right border, in pixels. 0 means no border. */
    TInt iRightBorderWidth;
    /** Border width for the top border, in pixels. 0 means no border. */
    TInt iTopBorderWidth;
    /** Border width for the bottom border, in pixels. 0 means no border. */
    TInt iBottomBorderWidth;

public:
    /* Public properties. */
    
    /** Uniform scale factor for the image. */
    THuiTimedValue iScale;

    /** Texture offset. */
    THuiTimedPoint iImageOffset;

    };

#endif  // __HUIImageBrush_H__
