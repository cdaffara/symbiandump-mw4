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
* Description:   Definition of CHuiTextureProcessor. CHuiTextureProcessor is 
*                the abstract base class for texture processors.
*
*/



#ifndef __HUITEXTUREPROCESSOR_H__
#define __HUITEXTUREPROCESSOR_H__


#include <e32base.h>
#include <uiacceltk/HuiTextureHandle.h>


/* Forward declarations. */
class CHuiEnv;


/**
 * CHuiTextureProcessor is a collection of texture filters. Each filter 
 * takes a texture as input, processes it, and produces a new texture as a 
 * result. Examples of processing filters are blurring and shadow generation. 
 * Concrete implementations of this class are rendering plugin specific.
 * CHuiTextureProcessor is the public, user-friendly way of filtering 
 * textures, because each filtering operation can be applied with a single
 * function call.
 *
 * @todo  Create an abstract CHuiTextureFilter class to represent a filter
 *        operation. Each filter could then be configured with an arbitrary
 *        number of parameters, and then applied on a texture. This would
 *        allow filters to have persistent state information.
 */
class CHuiTextureProcessor : public CBase
    {
public:

    /* Types. */

    /** @beginAPI */

    /** Flags for blur filtering. */
    enum TBlurFlags
        {
        /** Default blurring settings. */
        EBlurFlagDefault = 0,
        
        /** Include an alpha channel in the result. */
        EBlurFlagAlpha = 1,
        
        /** Retain extra space for edges so that the blurred edges that will
            always remain within the bounds of the resulting texture. Note
            that when the blurred texture is drawn, it has to be expanded 
            so that it will match the original images content. */
        EBlurFlagExpandEdges = 2,
        
        /** Replace source color RGB with white. Note that this flag is 
            mutually exclusive with flag EBlurFlagBlack. If both are defined
            then this flag overrides the EBlurFlagBlack. */
        EBlurFlagWhite = 4,
        
        /** Replace source color RGB with black. Note that this flag is 
            mutually exclusive with flag EBlurFlagWhite. If both are defined
            then this flag overridden by EBlurFlagWhite. */
        EBlurFlagBlack = 8,
        
        /** High quality blur. The blurring process is done twice, and the 
            results are averaged. */
        EBlurFlagHighQuality = 16,
        };

    /** @endAPI */


    /* Destructor. */

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHuiTextureProcessor();
    

    /* Methods. */

    /**
     * Returns the environment of the texture processor.
     */
    IMPORT_C CHuiEnv& Env();


    /** @beginAPI */

    /**
     * Blurs a texture. The dimensions of the resulting image are chosen
     * automatically. 
     *
     * Returns a handle to a new texture which contains
     * a blurred version of the texture. Ownership of the new texture is
     * transferred to the caller, which means it is responsible for either
     * creating a CHuiTexture out of the handle, or calling 
     * THuiTextureHandle::Release() when the generated texture is no longer
     * needed.
     *
     * Note that the algorithm used for blurring favors speed over accuracy.
     * This means that when hardware acceleration is available, the blurring
     * can be redone for each frame. However, the quality of the blurring 
     * may not be optimal. For example, color range reduction and pixel 
     * artifacts may be visible.
     *
     * @param aSrc  Texture to be blurred (single segment). Not modified.
     * @param aDest  New blurred texture. Ownership is transferred to the caller.
     *               Allowed to be the same as aSrc.
     * @param aFilterSize  Size of the blurring filter. 3 = 3x3 filter, 4 = 4x4 filter.
     * @param aFlags  Flags that modify the filter's parameters. @see TBlurFlags
     */    
    IMPORT_C virtual void BlurL(const THuiTextureHandle& aSrc,
                       THuiTextureHandle& aDest,
                       TInt aFilterSize = 3, TInt aFlags = 0) = 0;

    /**
     * Blurs a texture. The dimensions of the resulting image are as close to
     * aPreferredSize as possible, depending on the limitations of the 
     * implementation. 
     * 
     * See comments avobe.
     *
     * @param aSrc  Texture to be blurred (single segment). Not modified.
     * @param aDest  New blurred texture. Ownership is transferred to the caller.
     *               Allowed to be the same as aSrc.
     * @param aPreferredSize  Preferred size of the result.
     * @param aFilterSize  Size of the blurring filter. 3 = 3x3 filter, 4 = 4x4 filter.
     * @param aFlags  Flags that modify the filter's parameters. @see TBlurFlags
     */
    IMPORT_C virtual void BlurL(const THuiTextureHandle& aSrc, 
                       THuiTextureHandle& aDest,
                       const TSize& aPreferredSize,
                       TInt aFilterSize = 3, TInt aFlag = 0) = 0;

    
    /**
     * Blurs a texture. The dimensions of the resulting image are chosen
     * automatically. 
     *
     * See comments avobe.
     *
     * @param aSrc  Texture to be blurred (multi segment). Not modified.
     * @param aDest  New blurred texture. Ownership is transferred to the caller.
     *               Allowed to be the same as aSrc.
     * @param aFilterSize  Size of the blurring filter. 3 = 3x3 filter, 4 = 4x4 filter.
     * @param aFlags  Flags that modify the filter's parameters. @see TBlurFlags
     */ 
    IMPORT_C virtual void BlurSegmentedL(const MHuiSegmentedTexture& aSrc,
                       THuiTextureHandle& aDest,
                       TInt aFilterSize = 3, TInt aFlags = 0) = 0;
    
    /**
     * Blurs a texture. The dimensions of the resulting image are as close to
     * aPreferredSize as possible, depending on the limitations of the 
     * implementation. 
     *
     * See comments avobe.
     *
     *
     * @param aSrc  Texture to be blurred (multi segment). Not modified.
     * @param aDest  New blurred texture. Ownership is transferred to the caller.
     *               Allowed to be the same as aSrc.
     * @param aPreferredSize  Preferred size of the result.
     * @param aFilterSize  Size of the blurring filter. 3 = 3x3 filter, 4 = 4x4 filter.
     * @param aFlags  Flags that modify the filter's parameters. @see TBlurFlags
     */                
    IMPORT_C virtual void BlurSegmentedL(const MHuiSegmentedTexture& aSrc, 
                       THuiTextureHandle& aDest,
                       const TSize& aPreferredSize,
                       TInt aFilterSize = 3, TInt aFlag = 0) = 0;

    /** @endAPI */
    
    
    IMPORT_C virtual void Release();
    
    IMPORT_C virtual void RestoreL();


protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    IMPORT_C CHuiTextureProcessor(CHuiEnv& aEnv);


protected: // New methods
    
    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void TextureProcessorExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Environment. */
    CHuiEnv& iEnv;

    /** Spare member to help keeping binary compatibility, since
        HuiTextureProcessor is now dll derivable **/
    TInt iSpare;
    };

#endif  // __HUITEXTUREPROCESSOR_H__
