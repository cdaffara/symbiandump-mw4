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
* Description:   Declares CAlfTextureProcessor class.
*
*/



#ifndef C_ALFTEXTUREPROCESSOR_H
#define C_ALFTEXTUREPROCESSOR_H


#include <e32base.h>


/* Forward declarations. */
class CAlfEnv;
class CAlfTexture;

/**
 * CAlfTextureProcessor is a collection of texture filters. Each filter 
 * takes a texture as input, processes it, and produces a new texture as a 
 * result. Examples of processing filters are blurring and shadow generation. 
 * Concrete serverside implementations of this class are rendering plugin specific.
 * CAlfTextureProcessor is the public, user-friendly way of filtering 
 * textures, because each filtering operation can be applied with a single
 * function call.
 * Usage:
 * @code
 * //Get texture processor
 * CAlfTextureProcessor* textureProcessor = &iTxrMgr->TextureProcessor();
 * 
 *  //Blur texture, source and destination textures are same
 * textureProcessor->BlurL(*tex,*tex);
 * 
 * @endcode
 * @see CAlfTextureProcessor::BlurL()
 * @lib alfclient.lib
 * @since S60 v3.2
 *
 */
NONSHARABLE_CLASS(CAlfTextureProcessor) : public CBase
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
    virtual ~CAlfTextureProcessor();

    /**
     * Constructor.
     */
    static CAlfTextureProcessor* NewL(CAlfEnv& aEnv);    

    /**
     * Constructor.
     */
    static CAlfTextureProcessor* NewLC(CAlfEnv& aEnv);    

    /* Methods. */

    /** @beginAPI */

    /**
     * Blurs a texture. The dimensions of the resulting image are chosen
     * automatically. Returns a pointer to a new texture which contains
     * a blurred version of the texture. Ownership of the new texture is
     * transferred to the caller.
     *
     * Note that the algorithm used for blurring favors speed over accuracy.
     * This means that when hardware acceleration is available, the blurring
     * can be redone for each frame. However, the quality of the blurring 
     * may not be optimal. For example, color range reduction and pixel 
     * artifacts may be visible.
     *
     * @param aSrc  Source texture to be blurred. Not modified.
     * @param aDest Destination texture to be blurred. Modified. Allowed to be the same as aSrc.
     * @param aFilterSize  Size of the blurring filter. 3 = 3x3 filter, 4 = 4x4 filter.
     * @param aFlags  Flags that modify the filter's parameters. @see TBlurFlags
     */    
     IMPORT_C void BlurL(const CAlfTexture& aSrc,
                       CAlfTexture& aDest,
                       TInt aFilterSize = 3, TInt aFlags = 0);

    /**
     * Blurs a texture. The dimensions of the resulting image are as close to
     * aPreferredSize as possible, depending on the limitations of the 
     * implementation. Returns a pointer to a new texture which contains
     * a blurred version of the texture. Ownership of the new texture is
     * transferred to the caller.
     *
     * Note that the algorithm used for blurring favors speed over accuracy.
     * This means that when hardware acceleration is available, the blurring
     * can be redone for each frame. However, the quality of the blurring 
     * may not be optimal. For example, color range reduction and pixel 
     * artifacts may be visible.
     *
     * @param aSrc  Source texture to be blurred. Not modified.
     * @param aDest Destination texture to be blurred. Modified. Allowed to be the same as aSrc.
     * @param aPreferredSize  Preferred size of the result.
     * @param aFilterSize  Size of the blurring filter. 3 = 3x3 filter, 4 = 4x4 filter.
     * @param aFlags  Flags that modify the filter's parameters. @see TBlurFlags
     */
    IMPORT_C void BlurL(const CAlfTexture& aSrc, 
                       CAlfTexture& aDest,
                       const TSize& aPreferredSize,
                       TInt aFilterSize = 3, TInt aFlag = 0);

    /** @endAPI */

protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CAlfTextureProcessor();

    /**
     * Second-phase constructor.
     */
    void ConstructL(CAlfEnv& aEnv);


private:

	/** Private data */
    struct TPrivateData;    
    TPrivateData* iData;

    };

#endif  // C_ALFTEXTUREPROCESSOR_H
