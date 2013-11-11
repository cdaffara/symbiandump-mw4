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
* Description:   Implementation of CHuiSkin. CHuiSkin is the abstract base 
*                class of skins.
*
*/



#include "uiacceltk/HuiSkin.h"  // Class definition
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiDisplay.h" 
#include "huiextension.h"

CHuiSkin::CHuiSkin(CHuiEnv& aEnv)
        : iEnv(aEnv)
    {
    }


CHuiSkin::~CHuiSkin()
    {
    delete iShadowTexture;
    delete iLineTexture;
    delete iGlyphShadowTexture;
    iGc = NULL;
    }


EXPORT_C THuiSkinOrientation CHuiSkin::Orientation() const
    {
    TSize screenSize = HuiUtil::ScreenSize();
    if(screenSize.iWidth > screenSize.iHeight)
        {
        return EHuiSkinOrientationLandscape;
        }
    return EHuiSkinOrientationPortrait;
    }


EXPORT_C THuiSkinLocation CHuiSkin::Location(THuiSkinElement /*aElement*/)
    {
    // The default skin has no opinion on element locations.
    return EHuiSkinLocationNone;
    }


EXPORT_C void CHuiSkin::NotifyDisplaySizeChangedL()
    {
    for(TInt i=0;i<iEnv.DisplayCount();i++)
    	{
    	(iEnv.Display(i)).SetDirty();	
    	}
    }


EXPORT_C void CHuiSkin::SetContext(CHuiGc& aGc)
    {
    iGc = &aGc;
    }


EXPORT_C CHuiGc& CHuiSkin::Context() const
    {
    __ASSERT_ALWAYS(iGc != NULL,
                    THuiPanic::Panic(THuiPanic::ESkinHasNoGraphicsContext));
    return *iGc;
    }

EXPORT_C const CHuiTexture& CHuiSkin::TextureL(TInt aSkinTextureId)
    {
    return TextureL(aSkinTextureId, EFalse /* don't force prepare */);
    }


const CHuiTexture& CHuiSkin::TextureL(TInt aSkinTextureId, TBool aForcedPrepare)
    {
    TBool prepare = aForcedPrepare;
    
    switch(aSkinTextureId)
        {
        // Default background skin texture implementation only returns blank texture.
        case EHuiSkinBackgroundTexture:
            return iEnv.TextureManager().BlankTexture();

        // Return shadow texture. Create if necessary.
        case EHuiSkinShadowTexture:
            if(!iShadowTexture)
                {
                iShadowTexture = CHuiTexture::NewL();
                iShadowTexture->iContentObservers.AppendL(*this);
                prepare = ETrue;
                }
            if(prepare)
                {
                PrepareShadowTextureL(*iShadowTexture);
                }
            return *iShadowTexture;

        // Return glyph shadow texture. Create if necessary.
        case EHuiSkinGlyphShadowTexture:
            if(!iGlyphShadowTexture)
                {
                iGlyphShadowTexture = CHuiTexture::NewL();
                iGlyphShadowTexture->iContentObservers.AppendL(*this);
                prepare = ETrue;
                }
            if(prepare)
                {
                PrepareGlyphShadowTextureL(*iGlyphShadowTexture);
                }
            return *iGlyphShadowTexture;

        // Return line texture. Create if necessary.
        case EHuiSkinLineTexture:
            if(!iLineTexture)
                {
                iLineTexture = CHuiTexture::NewL();
                iLineTexture->iContentObservers.AppendL(*this);
                prepare = ETrue;
                }
            if(prepare)
                {
                PrepareLineTextureL(*iLineTexture);
                }
            return *iLineTexture;

        // Specified skin texture id is not supported.
        default:
            HUI_DEBUG1(_L("CHuiSkin::TextureL() - Skin texture id: %i not supported."), aSkinTextureId);
            User::Leave(KErrNotSupported);
            break;
        }

    // Never reached
    return iEnv.TextureManager().BlankTexture();
    }


EXPORT_C void CHuiSkin::ReleaseTexture(TInt aSkinTextureResource)
    {   
    switch(aSkinTextureResource)
        {
        case EHuiSkinShadowTexture:
            delete iShadowTexture;
            iShadowTexture = NULL;
            break;

        case EHuiSkinGlyphShadowTexture:
            delete iGlyphShadowTexture;
            iGlyphShadowTexture = NULL;
            break;

        case EHuiSkinLineTexture:
            delete iLineTexture;
            iLineTexture = NULL;
            break;

        default:
            break;
        }
    }


EXPORT_C TInt CHuiSkin::GetTexture(TInt aSkinTextureId, const CHuiTexture*& aOutTexture)
    {
    // Try to access the texture.
    TRAPD(err, aOutTexture = &TextureL(aSkinTextureId));

    if(err != KErrNone)
        {
        // If texture creation failed set out texture to blank texture
        aOutTexture = &iEnv.TextureManager().BlankTexture();
        }

    return err;
    }

void CHuiSkin::ReleaseCachedTextures()
    {
    SkinExtension(KHuiSkinReleaseCachedTextures, NULL);
    }

void CHuiSkin::PrepareShadowTextureL(CHuiTexture& aTexture)
    {
    aTexture.GenerateBlobL(TSize(16, 16));
    }


void CHuiSkin::PrepareGlyphShadowTextureL(CHuiTexture& /*aTexture*/) 
    {
    }


void CHuiSkin::PrepareLineTextureL(CHuiTexture& aTexture)
    {
    const TUint8 lineData[2 * 16 * 4] =
        {
          0,   0,   0, 0,          0,   0,   0,  0,
          0,   0, 128, 0*8/10,       0,   0, 128, 0*8/10,
          0,   0, 128, 8*8/10,       0,   0, 128, 8*8/10,
          0,   0, 128, 16*8/10,      0,   0, 128, 16*8/10,
          0,   0, 128, 32*8/10,      0,   0, 128, 32*8/10,
         64,  64, 128, 64*8/10,     64,  64, 128, 64*8/10,
        128, 128, 192, 128,      128, 128, 192, 128,
        255, 255, 255, 255,      255, 255, 255, 255,
        255, 255, 255, 255,      255, 255, 255, 255,
        128, 128, 192, 128,      128, 128, 192, 128,
         64,  64, 128, 64*8/10,     64,  64, 128, 64*8/10,
          0,   0, 128, 32*8/10,      0,   0, 128, 32*8/10,
          0,   0, 128, 16*8/10,      0,   0, 128, 16*8/10,
          0,   0, 128, 8*8/10,       0,   0, 128,  8*8/10,
          0,   0, 128, 0*8/10,       0,   0, 128,  0*8/10,
          0,   0,   0, 0,          0,   0,   0,  0

        };

    aTexture.UploadL(EHuiTextureFormatRgba8888, TSize(2, 16), lineData);
    }


EXPORT_C void CHuiSkin::TextureContentUploaded(CHuiTexture& /*aTexture*/)
    {
    HUI_DEBUG(_L("CHuiSkin::TextureContentUploaded() - Called."));
    }
    

EXPORT_C void CHuiSkin::TextureContentReleased(CHuiTexture& /*aTexture*/)
    {
    HUI_DEBUG(_L("CHuiSkin::TextureContentReleased() - Called."));
    }


EXPORT_C void CHuiSkin::RestoreTextureContentL(CHuiTexture& aTexture)
    {
    HUI_DEBUG(_L("CHuiSkin::RestoreTextureContentL() - Called."));
      
    // Restore the textures that existed prior to release.
    if(&aTexture == iShadowTexture)
        {
        TextureL(EHuiSkinShadowTexture, ETrue);
        }
    else if(&aTexture == iGlyphShadowTexture)
        {
        TextureL(EHuiSkinGlyphShadowTexture, ETrue);
        }
    else if(&aTexture == iLineTexture)
        {
        TextureL(EHuiSkinLineTexture, ETrue);
        }
    else
        {
        // for PC lint
        }
    }

void CHuiSkin::SkinExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParameters*/)
    {
    
    }

