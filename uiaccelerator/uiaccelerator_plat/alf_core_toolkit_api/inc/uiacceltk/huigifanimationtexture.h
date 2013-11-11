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
* Description:   ?Description
*
*/



#ifndef __HUIGIFANIMATIONTEXTURE__
#define __HUIGIFANIMATIONTEXTURE__

#include <uiacceltk/HuiAnimatedTexture.h>
#include <uiacceltk/HuiTextureManager.h>

NONSHARABLE_CLASS(CHuiGifAnimationTexture) : public CHuiAnimatedTexture,
                                public MHuiTextureLoadingCompletedObserver
    {
public:
    enum TAnimationState
        {
        EInitializing   = 0x1,
        ERunning        = 0x2,
        EStopped        = 0x4,
        EEnabled        = 0x8
        };
    IMPORT_C static CHuiGifAnimationTexture* NewL(const TDesC& aFilename,
                                                  CHuiTextureManager& aManager,
                                                  TInt aID,
                                                  THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault);
    IMPORT_C virtual ~CHuiGifAnimationTexture();

    void AdvanceTime(TReal32 aElapsedTime) __SOFTFP;
    void TextureLoadingCompleted(CHuiTexture& aTexture,
                                         TInt aTextureId,
                                         TInt aErrorCode);
                                       
    IMPORT_C TInt Id();
    IMPORT_C void Start();
    IMPORT_C void Stop();
    IMPORT_C void EnableAnimation(TBool aEnable);
protected:
    CHuiGifAnimationTexture(CHuiTextureManager& aManager,THuiTextureUploadFlags aFlags, TInt aId);
    void ConstructL(const TDesC& aFilename);
    void ReplaceTexture(CHuiTexture* aTexture);

private:
    CHuiTexture* iTexture1;
    CHuiTexture* iTexture2;
    TInt iTextureId1;
    TInt iTextureId2;
    TInt iCurrentTexture;
    HBufC* iFilename;
    
    TAnimationState iAnimationState;
    
    TInt iFrameInterval;
    TInt iFrameCount;
    THuiTextureUploadFlags iFlags;
    TInt iId;
    TReal32 iElapsedTime;
    TInt iAnimatedTextureGroupId;
    TBool iOriginalFileInUse;
    };


#endif
