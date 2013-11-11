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
* Description:   Definition of CHuiImageVisual. CHuiImageVisual is a visual
*                that is able to display images.
*
*/



#ifndef __HUIIMAGEVISUAL_H__
#define __HUIIMAGEVISUAL_H__


#include <e32base.h>
#include <gdi.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiGc.h>


/* Forward declarations */
class CHuiEnv;
class CHuiControl;
struct THuiMetric;
struct THuiXYMetric;


/**
 * Image visual displays an image.
 *
 * @see THuiImage
 * @see CHuiTexture
 */
NONSHARABLE_CLASS(CHuiImageVisual) : public CHuiVisual
    {
    /** @beginAPI */    
    
public:

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
        
public:

    /* Constructors and destructor. */

    /**
     * Constructs and appends a new image visual to the owner control.
     *
     * @param aOwnerControl  Control that will own the new visual.
     *
     * @return  The new visual.
     */
    IMPORT_C static CHuiImageVisual* AddNewL(CHuiControl& aOwnerControl,
                                             CHuiLayout* aParentLayout = 0);

    /** @endAPI */
    
    /**
     * Constructor.
     *
     * @param aOwner  Owner of the new visual.
     */
    CHuiImageVisual(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CHuiImageVisual();


	/** @beginAPI */

    /* Methods. */

    /**
     * Sets the scaling mode for the bitmap.
     * An unscaled image will be drawn at a 1:1 mapping between its
     * texture and the screen. A scaled image will be resized to fit the
     * visual's size exactly.
     * If unset, the default is EScaleFit.
     * 
     * @param aScaleMode The scaling mode of this image visual.
     */
    IMPORT_C void SetScaleMode(TScaleMode aScaleMode);

    /**
     * Sets the image displayed by the image visual. If no image
     * has been set for the visual, the visual does not draw anything.
     *
     * @note You may call SetImage also with images(/textures) that have not
     * been loaded (yet). The toolkit has been designed so that SetImage()
     * can be called before the texture has been finished loading. In the
     * meantime, a placeholder could be used. In other words, the CHuiTexture
     * that the THuiImage refers to does need to exist but it doesn't have to
     * be loaded yet.
     *
     * @note Redrawing of the visual happens automatically after SetImage() has
     * been called.
     * @note Redrawing also happens automatically when a texture has been fully
     * loaded by the texture manager. The manager notifies the environment,
     * which issues a full display redraw.
     *
     * @param aImage Primary image to be visualized.
     * @see SetSecondaryImage()
     */
    IMPORT_C void SetImage(const THuiImage& aImage);

    /**
     * Sets the secondary image displayed by the image visual.
     *
     * The reason there are two images is to make crossfading simpler from
     * the application's point of view. The blending ratio between the images
     * is controlled with the timed value iSecondaryAlpha of CHuiImageVisual.
     *
     * If iSecondaryAlpha is zero (the default), the secondary image will not be
     * drawn. When iSecondaryAlpha increases, the secondary image becomes
     * gradually visible as its opacity increases.
     *
     * When crossfading is not needed, SetSecondaryImage() and iSecondaryAlpha
     * can be ignored.
     *
     * @see SetImage()
     * @see iSecondaryAlpha
     */
    IMPORT_C void SetSecondaryImage(const THuiImage& aImage);

    /**
     * Returns the image of the visual.
     */
    IMPORT_C const THuiImage& Image() const;

    inline const THuiImage& SecondaryImage() const
        {
        return iSecondaryImage;
        }

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
    IMPORT_C void SetColorMode(TColorMode aColorMode, TReal32 aParam = 0.0) __SOFTFP;

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
    IMPORT_C void SetStretchMode(CHuiGc::TStretchMode aStretchMode);

    /**
     * @deprecated Use CHuiDropShadow
     *
     * Sets the drop shadow distance.
     *
     * @deprecated  Use metric unit version instead
     *
     * @param aShadowDistance  Pixels.
     */
    IMPORT_C void SetDropShadow(TInt aShadowDistance);

    /**
     * @deprecated Use CHuiDropShadow
     *
     * Sets the drop shadow distance.
     *
     * @param aShadowDistance  Shadow distance in metric units. Both X and Y will get same value.
     */
    IMPORT_C void SetDropShadow(const THuiMetric& aShadowDistance);

    /**
     * @deprecated Use CHuiDropShadow
     *
     * Sets the drop shadow distance.
     *
     * @param aShadowDistance  Shadow distance in metric units.
     */
    IMPORT_C void SetDropShadow(const THuiXYMetric& aShadowDistance);
    
    /** @endAPI */


    /**
     * Draw the visual.
     */
    
    void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;

    TBool Changed() const;

    void ClearChanged();

    void GetClassName(TDes& aName) const
        {
        aName = _L("CHuiImageVisual");
        }
        
    void ExpandRectWithContent(TRect& aRect) const;

    TBool PrepareDrawL();

private:

    /* Private methods */


private:

    /** Image displayed by the visual. By default scaled to fit the visual. */
    THuiImage iImage;

    /** Secondary image displayed by the visual. */
    THuiImage iSecondaryImage;

    /** Scalingmode of the visual. */
    TScaleMode iScaleMode;

    /** Width of the top stretch. */
    TInt iTopStretch;

    /** Width of the bottom stretch. */
    TInt iBottomStretch;

    /** Stretch mode. */
    CHuiGc::TStretchMode iStretchMode;

    /** @todo  Use THuiTimedRgb. */
    TRgb iColor;

    /** Color mode, e.g., modulate or blend. */
    TColorMode iColorMode;

    /** Color parameter. When blend mode, specifies amount of blending applied. */
    TReal32 iColorParam;

    // @deprecated Use CHuiDropShadow
    TInt iDropShadowDeprecated;


public:

    /* Public properties. */

    /** Angle for turning the image around its center point. */
    THuiTimedValue iTurnAngle;

    /** Uniform scale factor for the image. */
    THuiTimedValue iScale;

    /** Relative offset for scaled images. */
    THuiTimedPoint iOffset;

    /** Alpha for the secondary image. If one, the primary image is not
        visible at all. */
    THuiTimedValue iSecondaryAlpha;

private:

    
    // @deprecated Use CHuiDropShadow
	THuiXYMetric iDropShadow;

    };

#endif  // __HUIIMAGEVISUAL_H__
