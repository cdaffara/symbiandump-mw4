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
* Description:   Declares MHuiSegmentedTexture interface class for texture image objects.
*
*/



#ifndef __HUISEGMENTEDTEXTURE_H__
#define __HUISEGMENTEDTEXTURE_H__


#include <e32base.h>


/* Forward declarations. */
class THuiTextureHandle;


/**
 * MHuiSegmentedTexture is an abstract base class for texture image objects.
 * It specifies one or more texture images that comprise a larger (virtual) image.
 * THuiImage refers to objects that implement MHuiSegmentedTexture.
 *
 * A segmented texture is a texture that is composed of one or more segments.
 * Each segment has a position, dimensions, and a name that identifies the texture
 * image of the segment. The purpose is that it is possible to compose large images
 * out of smaller segments. The segments can overlap.
 *
 * One use case is where the OpenGL texture size is limited (e.g., 256x256). A class
 * implementing MHuiSegmentedTexture would in this case split the uploaded source
 * image to multiple segments and create a different OpenGL texture for each. All
 * of the segments would still be hidden inside the implementing
 * MHuiSegmentedTexture-derived class, though, so it could be treated as a single entity.
 *
 * When an object implements the MHuiSegmentedTexture interface, it can be drawn using
 * the DrawImage() methods in CHuiGc (since THuiImage specifies the image as
 * MHuiSegmentedTexture).
 *
 * @note CHuiGc and CHuiTexture only support single-segment textures at the moment.
 * @see CHuiGles10Texture
 * @see CHuiTexture
 * @see CHuiGc
 * @see THuiImage
 *
 */
class MHuiSegmentedTexture
    {
public:

    /** @beginAPI */

    /** Construction / Destruction */

    /**
     * Virtual destructor, needed because we have a couple of implemented
     * methods.
     */
    IMPORT_C virtual ~MHuiSegmentedTexture();


    /* Methods. */

    /**
     * Returns the logical size of this texture. Ie. the original size of the
     * image represented with this texture. This size is independent of the
     * possible segmentation of this texture.
     *
     * On the other hand the actual texture size may
     * vary across segments and typically has some driver/hw limitations.
     *
     * Note that all the segments must be contained inside the logical
     * area defined with this function.
     *
     * @see SegmentSize() To return logical size for a particular texture segment.
     * @see SegmentOffset() To return logical position for a particular texture segment.
     * @see SegmentTextureSize() To return texture size for a segment.
     */
    virtual TSize Size() const = 0;

    /**
     * Determines how many segments the texture has. Will return 1 if
     * the texture is not segmented.
     *
     * @return  Number of segments in this texture.
     */
    virtual TInt SegmentCount() const = 0;

    /**
     * Determines the texture name of a segment. For example with
     * OpenGL ES renderer, this will be the texture id generated
     * with glGenTextures().
     *
     * @param aSegment Segment index to access.
     */
    virtual TUint SegmentName(TInt aSegment) const = 0;

    /**
     * Determines the logical size of a texture segment.
     * Can be non-power-of-two (NPOT) and may be different
     * from the actual texture size.
     *
     * @param aSegment Valid segment index for this texture.
     * @see Size() to return logical size of the whole texture.
     * @see SegmentTextureSize() to determine the actual resolution of
     * the texture.
     */
    virtual TSize SegmentSize(TInt aSegment) const = 0;

    /**
     * Determines the actual resolution of the texture used
     * by the segment. Most likely only Power-of-Two, unless NPOT
     * supported by the graphics library.
     *
     * @param aSegment Valid segment index for this texture.
     */
    virtual TSize SegmentTextureSize(TInt aSegment) const = 0;

    /**
     * Determines the logical offset for a texture segment.
     *
     * @param aSegment Valid segment index for this texture.
     */
    virtual TPoint SegmentOffset(TInt aSegment) const = 0;

    /**
     * Returns a handle to given texture segment. Defaults to first segment.
     */
    IMPORT_C THuiTextureHandle Handle(TInt aSegment = 0) const;

    /**
     * Binds given texture segment to the texture target. Binding makes
     * the texture segment active.
     *
     * @param aSegment Index of a segment. Defaults to the first segment.
     * @param aTextureUnit  Which (multi-)texture unit to bind to. The default
     * value binds to the first texture unit (single-texturing mode).
     * @see SegmentCount() to get the number of available segments.
     */
    IMPORT_C void Bind(TInt aSegment = 0, TInt aTextureUnit = 0) const;

    /** @endAPI */
    
    };


#endif // __HUISEGMENTEDTEXTURE_H__
