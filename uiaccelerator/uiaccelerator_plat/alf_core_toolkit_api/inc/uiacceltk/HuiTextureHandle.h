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
* Description:   Definition of THuiTextureHandle. THuiTextureHandle is a 
*                handle to a named texture object. It does not have ownership 
*                of the texture object.
*
*/



#ifndef __HUITEXTUREHANDLE_H__
#define __HUITEXTUREHANDLE_H__


#include <uiacceltk/HuiSegmentedTexture.h>
#include <uiacceltk/HuiShadowedTexture.h>


/**
 * Handle to a single-segment texture object. Only used for referencing existing
 * textures. Destroying the handle does not affect the texture object itself.
 */
NONSHARABLE_CLASS(THuiTextureHandle) : public MHuiTexture, 
                          public MHuiSegmentedTexture
    {
public:

    /** Constructors and destructor. */

    /** @beginAPI */

    /**
     * Constructor.
     */
    IMPORT_C THuiTextureHandle();

    /**
     * Constructs a texture handle.
     * @see Set()
     */
    IMPORT_C THuiTextureHandle(TUint aName,
                               const TSize& aSize,
                               const TSize& aTextureSize);

    
    /* Methods. */
    
    /**
     * Determines if this is a null handle.
     *
     * @return  ETrue, if this is a null handle.
     */
    IMPORT_C TBool IsNull() const;
    
    IMPORT_C TBool HasContent() const;
    
    /**
     * Releases the texture object that the handle references.
     */
    IMPORT_C void Release();

    /**
     * Sets the handle's texture name, logical size and texture size. This is
     * equivalent to calling SetName(), SetSize(), and SetTextureSize().
     *
     * @param aName  Assigns texture name of this handle. For example, with
     * OpenGL ES renderer, this will be the texture name generated
     * with glGenTextures().
     * @param aSize  The logical size of this texture.
     * @param aSize  The actual texture size of this texture.
     */
    IMPORT_C void Set(TUint aName, const TSize& aSize, const TSize& aTextureSize);

    /** 
     * Sets the name of the named texture object that this handle refers to. 
     *
     * @param aName  Name of the texture object.
     */
    IMPORT_C void SetName(TUint aName);
    
    /** 
     * Sets the logical size of the texture object this handle refers to.
     *
     * @param aSize  Logical size.
     */
    IMPORT_C void SetSize(const TSize& aSize);
    
    /** 
     * Sets the actual size of the texture object this handle refers to.
     * For example, with OpenGL ES, this is the size of the texture object
     * itself, which is limited to powers of two.
     *
     * @param aTextureSize  Texture size.
     */
    IMPORT_C void SetTextureSize(const TSize& aTextureSize);
    
    
public:

    /* Implementation of MHuiTexture. */
    
    IMPORT_C virtual MHuiSegmentedTexture* SegmentedTexture();

    IMPORT_C virtual const MHuiSegmentedTexture* SegmentedTexture() const;

    IMPORT_C virtual MHuiShadowedTexture* ShadowedTexture();

    IMPORT_C virtual const MHuiShadowedTexture* ShadowedTexture() const;

	IMPORT_C virtual TBool TextureChanged() const;
	
	IMPORT_C virtual void TextureClearChanged() const;    

    /* Implementation of MHuiSegmentedTexture. */

    IMPORT_C TUint Name() const;
    
    IMPORT_C TSize Size() const;
    
    IMPORT_C TInt SegmentCount() const;

    IMPORT_C TUint SegmentName(TInt aSegment) const;

    IMPORT_C TSize SegmentSize(TInt aSegment) const;

    IMPORT_C TSize SegmentTextureSize(TInt aSegment) const;

    IMPORT_C TPoint SegmentOffset(TInt aSegment) const;

protected:
    
    IMPORT_C void TextureExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);
    
public:

    /** Name of the GL texture. */
    TUint iName;

    /** Logical size of this texture. */
    TSize iSize;

    /** Real size of the GL texture object. */
    TSize iTextureSize;

    };


#endif // __HUITEXTUREHANDLE_H__
