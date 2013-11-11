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
* Description:   Defines CHuiLinearGradientBrush - a Brush that is able to 
*                draw linear color gradients.
*
*/



#ifndef __HUIGRADIENTBRUSH_H__
#define __HUIGRADIENTBRUSH_H__

#include <e32base.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiImage.h>


/**
 * Linear gradient brush. Draws a sequence of colors in a linear gradient.
 */
NONSHARABLE_CLASS(CHuiGradientBrush) : public CHuiBrush
    {
public:

    /** Linear gradient directions. */
    enum TDirection
        {
        EDirectionUp,
        EDirectionRight,
        EDirectionDown,
        EDirectionLeft
        };

    /* Constructors and destructor. */

    /**
     * Constructor.
     */
    IMPORT_C static CHuiGradientBrush* NewL();

    /**
     * Constructor. The new brush is left on the cleanup stack.
     */
    IMPORT_C static CHuiGradientBrush* NewLC();

    /**
     * Destructor.
     */
    ~CHuiGradientBrush();


    /* Methods. */

    /**
     * Sets the gradient direction.
     */
    IMPORT_C void SetDirection(TDirection aDirection);

    /**
     * Replaces all colors with a single color.
     *
     * @param aColor    Solid color for the gradient.
     * @param aOpacity  Opacity.
     */
    IMPORT_C virtual void SetColor(const TRgb& aColor, TReal32 aOpacity = 1.0) __SOFTFP;

    /**
     * Sets the image for the gradient. By default, no image is used. The image
     * can be cleared with <code>SetImage(THuiImage())</code>.
     *
     * @param aImage  Image to use in the gradient.
     */
    IMPORT_C virtual void SetImage(const THuiImage& aImage);

    /**
     * Returns a color from the gradient.
     */
    IMPORT_C virtual TRgb Color(TInt aIndex) const;

    /**
     * Returns an opacity value from the gradient.
     */
    IMPORT_C virtual TReal32 Opacity(TInt aIndex) const __SOFTFP;

    /**
     * Returns the image of the gradient brush.
     */
    IMPORT_C virtual const THuiImage& Image() const;

    /**
     * Appends a new color for the entire gradient. The caller must
     * append colors in the correct order, by ascending position.
     *
     * @param aPosition  Position of the color.
     * @param aColor     Color at the position.
     * @param aOpacity   Opacity at the position.
     */
    IMPORT_C virtual void AppendColorL(TReal32 aPosition, const TRgb& aColor,
                                       TReal32 aOpacity = 1.0) __SOFTFP;

    /**
     * Draw the brush.
     */
    virtual void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;


protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiGradientBrush();

    /**
     * Second-phase constructor.
     */
    void ConstructL();


private:

    /* Private data */

    /** One color in the gradient. */
    struct SColor
        {
        /** Position in the range 0..1. */
        TReal32 iPosition;

        /** RGB color. */
        TRgb iColor;

        /** Opacity. */
        TReal32 iOpacity;
        };


private:

    /* Private methods */


private:

    /** Direction of a linear gradient. */
    TDirection iDirection;

    /** Array of gradient colors. */
    RArray<SColor> iColors;

    /** Image for the gradient. */
    THuiImage iImage;

    };

#endif  // __HUIGRADIENTBRUSH_H__
