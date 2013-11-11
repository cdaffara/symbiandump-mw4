/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Image visual displays an image.
*
*/



#ifndef C_ALFIMAGEVISUAL_H
#define C_ALFIMAGEVISUAL_H

#include <alf/alfvisual.h> 
#include <alf/alfgc.h> 

class TAlfImage;
class TRgb;
struct TAlfMetric;
struct TAlfXYMetric;

/**
 * Image visual displays an image.
 *
 * An image visual can be used to display portions of CAlfTextures on the UI.
 * Crossfading between two images in a single image visual is possible.
 * The secondaryAlpha property controls the blending between the primary and secondary images 
 * of the visual. The default value is 0.0, which results in only the primary image being drawn. 
 * The secondary image is ignored when doing scaling calculations.
 * 
 * Usage:
 * @code
 * //Create image visual and set image
 * //It will set primary image for imagevisual
 * CAlfImageVisual* imagevisual =  CAlfImageVisual::AddNewL( control );
 * imagevisual->SetImage( iImage );
 * 
 * 
 * //Set secondary image and alpha value for crossfading
 * imagevisual->SetSecondaryImage( iImage2 );
 * imagevisual->SetSecondaryAlpha( 0.5 );
 * 
 * @endcode
 * @see TAlfImage
 * @see CAlfTexture
 * 
 * @lib alfclient.lib
 * @since S60 v3.2
 */
class CAlfImageVisual : public CAlfVisual
    {

public:

    /** @beginAPI */

    /** Image scaling modes. */
    enum TScaleMode
        {
        /** Image is not scaled. It will be drawn at a 1:1 mapping between its texture and the screen pixels.*/
        EScaleNormal,
    
        /** Image is uniformly scaled to fit the width of the visual's area. The aspect
            ratio of the image is preserved. */
        EScaleFitWidth,
        
        /** Image is uniformly scaled to fit the height of the visual's area. The aspect
            ratio of the image is preserved. */
        EScaleFitHeight,
    
        /** Image is uniformly scaled to fit the visual's area so that the entire area
            is covered. The aspect ratio of the image is preserved. */
        EScaleCover,            
        
        /** Image is uniformly scaled to fit the visual's area. The aspect ratio of the 
            image is not preserved. This is the default scale mode. It allows
            full control of the appearance of the image by adjusting the 
            visual's size. */
        EScaleFit,
        
        /** Image is uniformly scaled to fit inside the visual's area. The aspect ratio 
            of the image is preserved. */
        EScaleFitInside
        };
    
    /** Color modes. */
    enum TColorMode
        {
        /** Default color mode. */
        EColorModulate,
        
        /** A dimmed color mode will apply fogging to this visual. */
        EColorDimmed
        };

    /** @endAPI */        
        
    /**
     * Constructor.
     */
    IMPORT_C CAlfImageVisual();

    /**
     * Second-phase constructor, from CAlfVisual 
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor
     */
    IMPORT_C virtual ~CAlfImageVisual();

	/** @beginAPI */
    
    /**
     * Constructs and appends a new image visual to the owner control.
     *
     * @param aOwnerControl  Control that will own the new visual.
     *
     * @return  The new visual.
     */
    IMPORT_C static CAlfImageVisual* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);


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
     * Gets the scaling mode of the Image Visual texture.
     * Returns the Scalemode set to Image Visual, using SetScaleMode()
     * Default is EScaleFit.
     * 
     * @return Scaling mode of the image visual.
     * @see SetScaleMode()
     */
    IMPORT_C CAlfImageVisual::TScaleMode ScaleMode();
       
    /**
     * Sets the image displayed by the image visual. If no image
     * has been set for the visual, the visual does not draw anything.
     *
     * @note You may call SetImage also with images(/textures) that have not
     * been loaded (yet). The toolkit has been designed so that SetImage()
     * can be called before the texture has been finished loading. In the
     * meantime, a placeholder could be used. In other words, the CAlfTexture
     * that the TAlfImage refers to does need to exist but it doesn't have to
     * be loaded yet.
     *
     * @note Redrawing of the visual happens automatically after SetImage() has
     * been called.
     * @note Redrawing also happens automatically when a texture has been fully
     * loaded by the texture manager. The manager notifies the environment,
     * which issues a full display redraw.
     *
     * @param aImage Primary image to be visualized.
     */
    IMPORT_C void SetImage(const TAlfImage& aImage); 

    /**
     * Sets the secondary image displayed by the image visual.
     *
     * The reason there are two images is to make crossfading simpler from
     * the application's point of view. The blending ratio between the images
     * is controlled with the timed value iSecondaryAlpha of CAlfImageVisual.
     *
     * If iSecondaryAlpha is zero (the default), the secondary image will not be
     * drawn. When iSecondaryAlpha increases, the secondary image becomes
     * gradually visible as its opacity increases.
     *
     * When crossfading is not needed, SetSecondaryImage() and iSecondaryAlpha
     * can be ignored.
     *
     * @see SetImage()
     * @see SetSecondaryAlpha()
     */
    IMPORT_C void SetSecondaryImage(const TAlfImage& aImage);

    /** 
     * Sets alpha for the secondary image. If one, the primary image is not
     * visible at all. 
     * @param aAlpha Alpha value for the image  
     */
    IMPORT_C void SetSecondaryAlpha(const TAlfTimedValue& aAlpha);

    /** 
     * Gets alpha for the secondary image. 
     * @return Alpha value for the image  
     */
    IMPORT_C TAlfTimedValue SecondaryAlpha();

    /**
     * Returns the image of the visual.
     */
    IMPORT_C const TAlfImage& Image() const;

    /**
     * Returns the secondary image of the visual.
     */
    IMPORT_C const TAlfImage& SecondaryImage() const;

    /**
     * Sets the color of the image. Defaults to white.
     */
    IMPORT_C void SetColor(const TRgb& aColor);

    /**
     * Sets the color mode of the image. Defaults to modulate.
     *
     * @param aColorMode  Color mode.
     * @param aParam      Parameter for the mode. With blend, specifies the
     *                    amount of color to blend (0...1). 1.0 will cause
     *                    the image to be fully blended to the visual's color.
     */
    IMPORT_C void SetColorMode(CAlfImageVisual::TColorMode aColorMode, TReal32 aParam = 0.0) __SOFTFP;

    /**
     * Sets the stretch widths of the image. Set both to zero to disable stretching.
     * A stretched image will be scaled over the whole width or height of the visual,
     * except for a band specified by the parameters, which will not be scaled (1:1 pixel mapping).
     * For doing uniform scaling of an image, use the SetScaleMode API instead.
     *
     * @deprecated  Deprecated as an unneeded and broken functionality.
     *
     * @param aTop     Top width in pixels.
     * @param aBottom  Bottom width in pixels.
     * @see SetStretchMode.
     * @see CHuiGc::DrawStretchImage
     */
    IMPORT_C void SetStretch(TInt aTop, TInt aBottom);
	
    /**
     * Sets the stretch mode of the image.
     *
     * @see CHuiGc::DrawStretchImage.
     * @see Setstretch.
     * @param aStretchMode  Horizontal or vertical.
     */
    IMPORT_C void SetStretchMode(CAlfGc::TStretchMode aStretchMode);

    /**
     * @deprecated Use CAlfDropShadow
     * 
     * Sets the drop shadow distance.
     *
     * @deprecated  Use metric unit version instead.
     *
     * @param aShadowDistance  Pixels.
     */
    IMPORT_C void SetDropShadow(TInt aShadowDistance);

    /**
     * @deprecated Use CAlfDropShadow
     *
     * Sets the drop shadow distance.
     *
     * @param aShadowDistance  Set shadow distance in metric units. Both X and Y will use this value.
     */
    IMPORT_C void SetDropShadow(const TAlfMetric& aShadowDistance);

    /**
     * @deprecated Use CAlfDropShadow
     *
     * Sets the drop shadow distance.
     *
     * @param aShadowDistance  Set shadow distance in metric units.
     */
    IMPORT_C void SetDropShadow(const TAlfXYMetric& aShadowDistance);

    /** 
     * Sets angle for turning the image around its center point. 
     * @param Turn angle
     */
    IMPORT_C void SetTurnAngle(const TAlfTimedValue& aAngle);

    /** 
     * Gets angle for turning the image around its center point. 
     * @return Turn angle
     */
    IMPORT_C TAlfTimedValue TurnAngle();

    /** 
     * Sets uniform scale factor for the image. 
     * @param aScale Scale factor
     */
    IMPORT_C void SetScale(const TAlfTimedValue& aScale);

    /** 
     * Gets uniform scale factor for the image. 
     * @return aScale Scale factor
     */
    IMPORT_C TAlfTimedValue Scale();

    /** 
     * Sets relative offset for scaled images.
     * @param aOffset relative offset
     */
    IMPORT_C void SetOffset(const TAlfTimedPoint& aOffset);

    /** 
     * Gets relative offset for scaled images.
     * @return relative offset
     */
    IMPORT_C TAlfTimedPoint Offset();

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);

    /** @endAPI */

protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private:

	/** Private data */
    struct TImageVisualPrivateData;    
    TImageVisualPrivateData* iImageVisualData;
    };


#endif // C_ALFIMAGEVISUAL_H
