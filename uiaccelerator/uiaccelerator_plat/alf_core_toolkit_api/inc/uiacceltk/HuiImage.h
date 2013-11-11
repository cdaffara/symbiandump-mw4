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
* Description:   Image specification. Specifies an area on a (segmented) texture object.
*
*/



#ifndef __HUIIMAGE_H__
#define __HUIIMAGE_H__


#include <e32base.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/HuiTextureHandle.h>
#include <uiacceltk/HuiTexture.h>


/**
 * Image specification. Specifies an area on a texture object.
 *
 * The texture to with THuiImage refers is expected to the implement 
 * the MHuiSegmentedTexture interface.
 */
NONSHARABLE_CLASS(THuiImage)
    {
public:

    /* Constructors. */

    /** @beginAPI */

    /**
     * Default constructor, creates an empty THuiImage.
     * @see SetTexture()
     * @see SetTexCoords()
     */
    IMPORT_C THuiImage();

    /**
     * Constructor. Textures coordinates will be normalized to match the
     * size of the texture, so that (1,1) is always the lower right corner.
     * Note that the THuiImage instance stores a pointer to aTexture.
     * aTexture must continue to exist for the entire lifetime of the
     * THuiImage instance. For example, passing a temporary THuiTextureHandle 
     * instance will result in a crash when the image's texture is accessed.
     *
     * @param aTexture  Texture being drawn.
     *                  The texture is expected to the implement the 
     *                  MHuiSegmentedTexture interface.
     * @param aTlX      Top left X coordinate (normalized).
     * @param aTlY      Top left Y coordinate (normalized).
     * @param aBrX      Bottom right X coordinate (normalized).
     * @param aBrY      Bottom right Y coordinate (normalized).
     */
    IMPORT_C THuiImage(const MHuiTexture& aTexture,
                       TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                       TReal32 aBrX = 1.f, TReal32 aBrY = 1.f) __SOFTFP;


    /**
     * Constructor. Textures coordinates will be normalized to match the
     * size of the texture, so that (1,1) is always the lower right corner.
     * Note that the THuiImage instance stores a pointer to aTexture.
     * aTexture must continue to exist for the entire lifetime of the
     * THuiImage instance. For example, passing a temporary THuiTextureHandle 
     * instance will result in a crash when the image's texture is accessed.
     *
     * @param aTexture  Texture being drawn.
     *                  The texture is expected to be derived from CHuiTexture.
     * @param aTlX      Top left X coordinate (normalized).
     * @param aTlY      Top left Y coordinate (normalized).
     * @param aBrX      Bottom right X coordinate (normalized).
     * @param aBrY      Bottom right Y coordinate (normalized).
     */
    IMPORT_C THuiImage(const CHuiTexture& aTexture,
                       TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                       TReal32 aBrX = 1.f, TReal32 aBrY = 1.f) __SOFTFP;


    /**
     * Returns the number of segments in this image.
     */
    IMPORT_C TInt SegmentCount() const;

    /**
     * Calculate vertex coordinates for an image segment.
     *
     * @param aOutTexCoords  The texture coordinates for a quad are output to
     *                       this array, two coordinates (x,y) per corner point.
     *                       Must have room for at least 8 TInt16s.
     * @param aSegment       Image segment number to return the coordinates for.
     *
     * @see SegmentCount() to return the number of image segments in this image.
     */
    IMPORT_C void GetVertexCoords(TInt16* aOutCoords, TInt aSegment = 0) const;

    /**
     * Calculate actual texture coordinates for image segment.
     *
     * @param aOutTexCoords  The texture coordinates for a quad are output to
     *                       this array, two coordinates (u,v) per corner point.
     *                       Must have room for at least 8 TReal32s.
     * @param aSegment       Image segment number to return the coordinates for.
     *
     * @see SegmentCount() to return the number of image segments in thiss image.
     */
    IMPORT_C void GetTexCoords(TReal32* aOutTexCoords, TInt aSegment = 0) const;

    /**
     * Sets the texture coordinates for the whole image.
     * Raw coords flag is cleared.
     */
    IMPORT_C void SetTexCoords(TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                               TReal32 aBrX = 1.f, TReal32 aBrY = 1.f,
                               TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Determines the current texture coordinates of the top-left corner of
     * this image.
     */
    IMPORT_C THuiRealPoint TopLeft() const __SOFTFP;

    /**
     * Determines the current texture coordinates of the bottom-right corner of
     * this image.
     */
    IMPORT_C THuiRealPoint BottomRight() const __SOFTFP;

    /**
     * Determines if a texture has been specified for the image.
     */
    IMPORT_C TBool HasTexture() const;

    /**
     * Sets the texture of the image. 
     *
     * The texture is expected to the implement the MHuiSegmentedTexture interface.
     */
    IMPORT_C void SetTexture(const MHuiTexture& aTexture);

    /**
     * Sets the texture of the image. 
     *
     * The texture is expected to be derived from CHuiTexture.
     */
    IMPORT_C void SetTexture(const CHuiTexture& aTexture);

    /**
     * Returns the texture object associated with the image.
     */
    IMPORT_C const MHuiSegmentedTexture& Texture() const;

    /**
     * Returns the interface of the texture object associated with the image.
     *
     * @return  Reference to MHuiTexture. This interface can be queried for 
     *          the implemented texture interfaces.
     */
    IMPORT_C const MHuiTexture& TextureIf() const;
    
    /**
     * Determines if the texture has a shadow.
     */
    IMPORT_C TBool HasShadow() const;
    
    /**
     * Returns the shadow texture object associated with the image.
     *
     * @return  NULL, if no shadow available.
     */
    IMPORT_C THuiTextureHandle ShadowTexture() const;

    /**
     * Apply an offset to the texture coordinates.
     */
    IMPORT_C void Translate(TReal32 aDx, TReal32 aDy, TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Apply a scaling factor to the texture coordinates.
     */
    IMPORT_C void Scale(TReal32 aScaleX, TReal32 aScaleY, TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Returns true if the image has changed since the last change
     * notification. Changed means that the image is dirtied, and must
     * be redrawn.
     */

	IMPORT_C TBool Changed() const;

    /**
     * Clears the changed status of the visual.
     */
	
	IMPORT_C void ClearChanged();

    /** @endAPI */

    /**
     * Returns pointer to the texture object associated with the image. 
     *
     * @return  Pointer to MHuiTexture object. May return NULL if no texture is
     * associated with the image.
     */
    const MHuiTexture* ImageTexture() const;

    /**
     * Returns pointer to the texture object associated with the image. 
     *
     * @return  Pointer to CHuiTexture object. May return NULL if no texture is
     * associated with the image or if the texture type is not derived from CHuiTexture. 
     */
    const CHuiTexture* ImageDefaultTexture() const;

private:

    /** Texture containing the image data. */
    const MHuiTexture* iTexture;

    /** Texture coordinates of the top left corner, for the whole image. */
    THuiTimedPoint iTl;

    /** Texture coordinates of the bottom right corner, for the whole image. */
    THuiTimedPoint iBr;

    /** Flags for internal use */
    TInt iFlags;
    
    TInt iSpare2;
    };

#endif  // __HUIIMAGE_H__
