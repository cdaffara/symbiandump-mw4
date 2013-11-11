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
* Description:   Gradient brush class
*
*/



#ifndef C_ALFGRADIENTBRUSH_H
#define C_ALFGRADIENTBRUSH_H

#include <alf/alfbrush.h>
#include <gdi.h>

class CAlfEnv;
class TAlfImage;

/**
 * Linear gradient brush. Draws a sequence of colors in a linear gradient.
 * A gradient brush can draw gradients in horizontal and vertical 
 * directions using any number of colors.
 * 
 * Usage:
 * @code
 * 
 * CAlfVisual* visual = CAlfVisual::AddNewL( control );
 * visual->EnableBrushesL();
 * CAlfGradientBrush* brush = CAlfGradientBrush::NewL( *iEnv );
 * 
 * // Configure the gradient: direction and colors.
 * brush->SetDirection( CAlfGradientBrush::EDirectionDown );
 * brush->SetColor( KRgbBlack, .5f );
 * brush->AppendColorL( 1, KRgbRed, 1.f );
 * brush->AppendColorL( 2, KRgbYellow, .1f );
 * 
 * //Append brush in brusharray with ownership to array 
 * visual->Brushes()->AppendL( brush, EAlfHasOwnership );
 * @endcode
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfGradientBrush ): public CAlfBrush
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

    /**
     * Constructor.
     */
    IMPORT_C static CAlfGradientBrush* NewL( CAlfEnv& aEnv );

    /**
     * Constructor. The new brush is left on the cleanup stack.
     */
    IMPORT_C static CAlfGradientBrush* NewLC( CAlfEnv& aEnv );
    
    /**
     * Destructor.
     */
    virtual ~CAlfGradientBrush();
    
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
    IMPORT_C void SetColor(const TRgb& aColor, TReal32 aOpacity = 1.0) __SOFTFP;

    /**
     * Sets the image for the gradient. By default, no image is used. The image
     * can be cleared with <code>SetImage(TAlfImage())</code>.
     *
     * @param aImage  Image to use in the gradient.
     */
    IMPORT_C void SetImage(const TAlfImage& aImage);

    /**
     * Returns a color from the gradient.
     */
    IMPORT_C TRgb Color(TInt aIndex) const;

    /**
     * Returns an opacity value from the gradient.
     */
    IMPORT_C TReal32 Opacity(TInt aIndex) const;

    /**
     * Returns the image of the gradient brush.
     */
    IMPORT_C const TAlfImage& Image() const;

    /**
     * Appends a new color for the entire gradient. The caller must
     * append colors in the correct order, by ascending position.
     *
     * @param aPosition  Position of the color.
     * @param aColor     Color at the position.
     * @param aOpacity   Opacity at the position.
     */
    IMPORT_C void AppendColorL(
        TReal32 aPosition, 
        const TRgb& aColor,
        TReal32 aOpacity = 1.0) __SOFTFP;
    

private:

    CAlfGradientBrush();

    void ConstructL( CAlfEnv& aEnv );

private: // data

    struct TGradientBrushPrivateData;
    TGradientBrushPrivateData* iGradientBrushData;
    
    };



#endif // C_ALFGRADIENTBRUSH_H
