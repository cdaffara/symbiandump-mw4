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
* Description:   Declares the MHuiShadowedTexture interface. 
*                MHuiShadowedTexture is an interface for textures that support 
*                dropshadows that are generated out of the texture's content.
*
*/



#ifndef __HUISHADOWEDTEXTURE_H__
#define __HUISHADOWEDTEXTURE_H__


#include <uiacceltk/HuiTextureIf.h>


/* Forward declarations. */
class MHuiSegmentedTexture;
class MHuiShadowedTexture;
class THuiTextureHandle;


/** Generic shadowing styles. */
enum THuiShadowStyle
    {
    /** Shadow is going to be used with an icon. Little detail. */
    EHuiTextureShadowStyleIcon,
    
    /** Shadow is going to be used with rasterized text. High detail. */
    EHuiTextureShadowStyleRasterizedText
    };


/**
 * MHuiShadowedTexture is an interface for textures that support dropshadows 
 * that are generated out of the texture's content.
 */
class MHuiShadowedTexture
    {
public:
    
    /* Destructor. */
    
    /**
     * Virtual destructor.
     */
    virtual ~MHuiShadowedTexture() 
        {
        }
    
    
    /* Interface methods. */
    
    /**
     * @deprecated 
     *
     * Enables or disables the generation of a shadow when a new image is
     * uploaded. Note that when the shadow is enabled, a new texture image
     * needs to be uploaded before the shadow is actually generated.
     *
     * @param aEnable  Enables or disables shadow generation and usage.
     */
    virtual void EnableShadow(TBool aEnable = ETrue) = 0;

    /**
     * @deprecated 
     *
     * Determines if shadow generation has been enabled.
     */
    virtual TBool IsShadowEnabled() const = 0;

    /**
     * @deprecated 
     *
     * Gets a handle to the shadow texture. The handle can be a null handle 
     * if the shadow is disabled or has not been generated yet. 
     *
     * Does not return the handle as a return value because that would make it too
     * easy to construct a THuiImage directly out of the handle, and cause
     * a crash since the THuiImage would then reference a temporary object.
     *
     * @param aHandle  Handle to the shadow texture is returned here.
     *
     * @return  <code>ETrue</code>, if a shadow texture is available.
     *          Otherwise, <code>EFalse</code>.
     */
    virtual TBool GetShadowTexture(THuiTextureHandle& aHandle) const = 0;
    
    };


#endif // __HUISHADOWEDTEXTURE_H__
