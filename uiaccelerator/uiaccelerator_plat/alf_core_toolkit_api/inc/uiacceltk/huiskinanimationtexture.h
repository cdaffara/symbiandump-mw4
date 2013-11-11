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
* Description:   Texture class to support Legacy theme animation
*
*/



#ifndef __HUISKINANIMATIONTEXTURE__
#define __HUISKINANIMATIONTEXTURE__

#include <uiacceltk/HuiAnimatedTexture.h>
#include <uiacceltk/HuiTextureManager.h>

class CHuiSkinBitmapProvider;

/**
 * @deprecated <b>Trying to create this class will leave with KErrNotSupported!</b>
 * 
 * Loads a sequence of images from S60 theme to support animated texture.
 */    
NONSHARABLE_CLASS(CHuiSkinAnimationTexture) : public CHuiAnimatedTexture,
                                public MHuiTextureLoadingCompletedObserver
    {
public:
    // Animation state
    enum TAnimationState
        {
        EInitializing   = 0x1,
        ERunning        = 0x2,
        EStopped        = 0x4
        };

     /**
     * @deprecated <b>Trying to create this class will leave with KErrNotSupported!</b>
     * 
     * Loads a sequence of images and makes it an animated texture.
     *
     * @param aSkinName             Logical name of animated element
     * @param aId                   Id for the texture. Must be unique.
     * @param aFlags                Specify load/upload behavior
     *
     * @return Reference to the texture.
     *
     * @see SetImagePathL() To set the image search path. Set to "" to use
     * absolute image filenames.
     * @see IsLoaded()
     */
    IMPORT_C static CHuiSkinAnimationTexture* NewL(const TDesC& aSkinName,
                                                      CHuiTextureManager& aManager,
                                                      TInt aID,
                                                      THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault);
    //destructor
    IMPORT_C ~CHuiSkinAnimationTexture();

    // animation framework callback
    void AdvanceTime(TReal32 aElapsedTime) __SOFTFP;
    
    // Framework callback after load texture complete
    void TextureLoadingCompleted(CHuiTexture& aTexture,
                                         TInt aTextureId,
                                         TInt aErrorCode);
                                       
    IMPORT_C TInt Id();
    
    /** Start animation, continue refresh from current frame */
    IMPORT_C void Start();
    
    /** Stop animation, stop refresh to current frame */
    IMPORT_C void Stop();

protected:
    CHuiSkinAnimationTexture(CHuiTextureManager& aManager,THuiTextureUploadFlags aFlags, TInt aId);
    void ConstructL(const TDesC& aFilename);
    void ReplaceTexture(CHuiTexture* aTexture);

private:
    TInt iCurrentTextureId;
    TInt iFrameInterval;
    TInt iFrameCount;
    
    TInt iAnimationState; // todo: is this supposed operate based on finary flags or not ?
    
    THuiTextureUploadFlags iFlags;
    TInt iId;
    TReal32 iElapsedTime;
    CHuiSkinBitmapProvider* iSkinBitmapProvider;	// Skin Bitmap provider
    };


#endif
