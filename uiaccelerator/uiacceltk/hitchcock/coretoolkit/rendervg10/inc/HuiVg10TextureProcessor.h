/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiVg10TextureProcessor. 
*                CHuiVg10TextureProcessor is a concrete implementation of 
*                a texture processor for OpenVG 1.0.
*
*/



#ifndef __HUIVG10TEXTUREPROCESSOR_H__
#define __HUIVG10TEXTUREPROCESSOR_H__


#include "uiacceltk/HuiTextureProcessor.h"


/* Forward declarations. */
class CHuiVg10PBufferSurface;


/**
 * Texture processor implementation for OpenVG.
 */
NONSHARABLE_CLASS(CHuiVg10TextureProcessor) : public CHuiTextureProcessor
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructs a new texture manager instance.
     */
    static CHuiVg10TextureProcessor* NewL(CHuiEnv& aEnv);
    
    /**
     * Constructs a new texture manager instance and leaves it on 
     * the cleanup stack.
     */
    static CHuiVg10TextureProcessor* NewLC(CHuiEnv& aEnv);

    /**
     * Destructor.
     */
    virtual ~CHuiVg10TextureProcessor();
    
    
    /* Methods. */
    
    virtual void BlurL(const THuiTextureHandle& aSrc, THuiTextureHandle& aDest,
                       TInt aFilterSize, TInt aFlag);

    virtual void BlurL(const THuiTextureHandle& aSrc, THuiTextureHandle& aDest,
                       const TSize& aPreferredSize,
                       TInt aFilterSize, TInt aFlag);
    virtual void BlurSegmentedL(const MHuiSegmentedTexture& aSrc,
                       THuiTextureHandle& aDest,
                       TInt aFilterSize, TInt aFlags); 
    
    virtual void BlurSegmentedL(const MHuiSegmentedTexture& aSrc, 
                       THuiTextureHandle& aDest,
                       const TSize& aPreferredSize,
                       TInt aFilterSize, TInt aFlags);                       
protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiVg10TextureProcessor(CHuiEnv& aEnv);

    /**
     * Second-phase constructor.
     */
    void ConstructL();


    /* Methods. */
    
    /**
     * Renders a blurred texture into the processing buffer. BlurL() calls this
     * one or more times to produce the final blurred result.
     *
     * @param aBlurConfigPtr  Pointer to a THuiBlurConfig struct, which holds 
     *                        the parameters of the blurring filter. Note that
     *                        this is an internal struct defined only in the
     *                        implementation of CHuiVg10TextureProcessor.
     * @param aSrc  Source texture.
     * @param aDestTextureName  Name of the OpenVG texture where the blurred
     *                          result will be stored.
     * @param aFlags  The same flags that were used in the call to BlurL().
     */
    void DrawBlurred(const TAny* aBlurConfigPtr, 
                     const THuiTextureHandle& aSrc, 
                     TUint aDestTextureName,
                     TInt aFlags);

    /**
     * Combines two textures so that the end result is an average of both.
     * The resulting texture uses the same size as the source textures.
     * Used as part of BlurL() to produce a higher-quality blurring effect.
     *
     * @param aSrcTextureName1  Name of the first source texture.
     * @param aSrcTextureName2  Name of the second source texture.
     * @param aDestTextureName  Name of the destination texture.
     * @param aSize  Size of all the three textures.
     * @param aFlags  The same flags that were used in the call to BlurL().
     */
    void DrawAverage(TUint aSrcTextureName1, 
                     TUint aSrcTextureName2,
                     TUint aDestTextureName,
                     const TSize& aSize,
                     TInt aFlags);

    void Release();
    
    void RestoreL();                     

private:
    };


#endif  // __HUIVG10TEXTUREPROCESSOR_H__
