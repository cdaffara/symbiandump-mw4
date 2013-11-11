/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Image brush definition
*
*/



#ifndef C_ALFIMAGEBRUSH_H
#define C_ALFIMAGEBRUSH_H

#include <alf/alfbrush.h>
#include <alf/alfimagevisual.h>

class CAlfEnv;
class TAlfImage;
class TRgb;

/**
 * Image brush can be used to fill the entire background of a visual. 
 * It can also be used for stretching out a single bitmap.
 * For example, in the case below, image is used as the borders and background of a visual.
 * 
 * Usage:
 * @code
 * 
 * // Create layout and visual, and set up some padding.
 * CAlfDeckLayout* deck = CAlfDeckLayout::AddNewL( control );
 * deck->SetPadding( TPoint( 10, 10 ) );
 * CAlfTextVisual* text = CAlfTextVisual::AddNewL( control, deck );
 * text->EnableBrushesL();
 * 
 * //For image in imagebrush
 * CAlfTexture* texture = iEnv->TextureManager().LoadTexture( _L("1.bmp"), 
 * 						EAlfTextureFlagDefault, AUTO_GENERATED_TEXTURE_ID );
 * 
 * // Image brush with borders within the brush area.
 * CAlfImageBrush* brush = CAlfImageBrush::NewL( &iEnv, TAlfImage( *texture ) );
 * brush->SetLayer( EAlfBrushLayerBackground );
 * text->Brushes()->AppendL( brush, EAlfHasOwnership );
 * 
 * @endcode
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfImageBrush ): public CAlfBrush
    {

public:

    /**
     * Constructor.
     */
    IMPORT_C static CAlfImageBrush* NewL(CAlfEnv& aEnv, const TAlfImage & aImage);

    /**
     * Constructor. The new brush is left on the cleanup stack.
     */
    IMPORT_C static CAlfImageBrush* NewLC(CAlfEnv& aEnv, const TAlfImage & aImage);

    /**
     * Sets the scaling mode for the bitmap.
     * An unscaled image will be drawn at a 1:1 mapping between its
     * texture and the screen. A scaled image will be resized to fit the
     * visual's size exactly.
     * If unset, the default is EScaleFit.
     * 
     * @param aScaleMode The scaling mode of this image visual.
     */
    IMPORT_C void SetScaleMode(CAlfImageVisual::TScaleMode aScaleMode);

    /**
     * Sets an image for this brush.
     *
     * @param aImage            Texture image for this brush.
     * 
     * @see SetBorders() 	    Call this if the image also includes 
     * 							borders.
     */
    IMPORT_C void SetImage(const TAlfImage& aImage);
    
    /**
     * Returns the brush image.
     */
    IMPORT_C const TAlfImage& Image() const;

	/**
	 * Define widths and heights for the borders of this image brush.
	 * Note that image won't be scaled if this brush has borders.
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
    
    /** 
     * Sets relative offset for scaled images.
     * 
     * @param aOffset relative offset Default value is (0,0).
     *                (0.5,1) moves the brush half the visual width right
     *                and the visual height downwards.
     */
    IMPORT_C void SetOffset(const TAlfTimedPoint& aOffset);
    
    /** 
     * Gets relative offset for scaled images.
     * @return relative offset
     */
    IMPORT_C const TAlfTimedPoint& Offset() const;
    
    /** 
     * Sets uniform scale factor for the image. 
     * Note that image won't be scaled if this brush has borders.
     *
     * @param aScale Scale factor. Value 1 is default.
     *               2 means double the size and 0.5 half the size.
     */
    IMPORT_C void SetScale(const TAlfTimedValue& aScale);
    
    /** 
     * Gets uniform scale factor for the image. 
     * @return aScale Scale factor
     */
    IMPORT_C const TAlfTimedValue& Scale() const;
        
    /**
     * Destructor.
     */
    virtual ~CAlfImageBrush();


private:

    /**
     * 1st pahse constructor.
     */
    CAlfImageBrush();

    /**
     * 1nd pahse constructor.
     */
    void ConstructL(CAlfEnv& aEnv, const TAlfImage & aImage);

private:

    // Private data. Owned.
    struct TImageBrushPrivateData;
    TImageBrushPrivateData* iImageBrushData;

    };

    
#endif // C_ALFIMAGEBRUSH_H
