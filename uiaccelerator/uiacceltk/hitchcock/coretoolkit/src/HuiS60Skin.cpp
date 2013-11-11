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
* Description:   Implementation of CHuiS60Skin. CHuiS60Skin is a HUITK skin 
*                that uses resources from the current S60 skin.
*
*/



#include "uiacceltk/HuiS60Skin.h"  // Class definition
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiTextureManager.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiDisplay.h"

#include <AknsDrawUtils.h>
#include <AknUtils.h>

#include "huiskinbackroundlayout.h"
#include "huiextension.h"

struct TBackgroundTexture
    {
    TAknsItemID iID;
    CHuiTexture* iBackgroundTexture;    //not owned
    };
    
struct TPrivData
    {
    RArray<TBackgroundTexture> iBackgrounds;
    };


    
EXPORT_C CHuiS60Skin::CHuiS60Skin(CHuiEnv& aEnv)
    : CHuiSkin(aEnv)
    {
    iSpare = NULL;
    }


EXPORT_C void CHuiS60Skin::ConstructL()
    {
    iSkinControlContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgScreen, TRect(TPoint(0, 0), HuiUtil::ScreenSize()), ETrue);
    iSpare = new (ELeave) TPrivData;
    }


EXPORT_C CHuiS60Skin::~CHuiS60Skin()
    {
    FreeBackgrounds();
    delete (TPrivData*)(iSpare);
    delete iSkinControlContext;
    delete iBackgroundTexture;
    if (iSkinSrvConnected)
        iSkinSrvSession.Close();
        
    delete iBackgroundBitmap;
    iCachedSkinItems.ResetAndDestroy();
    iCachedSkinItems.Close();
    }


EXPORT_C THuiSkinLocation CHuiS60Skin::Location(THuiSkinElement aElement)
    {
    // Changed so that correct cba location is asked from AknLayouUtils
    switch(aElement)
         {
         case EHuiSkinElementSoftKeys:
            {
            THuiSkinLocation retValue;
            AknLayoutUtils::TAknCbaLocation skLocation; 
            skLocation = (AknLayoutUtils::TAknCbaLocation)CHuiStatic::CbaLocation();
            switch ( skLocation )
                {
                default:
                    // falltrough
                case AknLayoutUtils::EAknCbaLocationBottom:
                    {
                    retValue = EHuiSkinLocationBottom;
                    break;
                    }
                case AknLayoutUtils::EAknCbaLocationRight:
                    {
                    retValue = EHuiSkinLocationRight;
                    break;
                    }
                case AknLayoutUtils::EAknCbaLocationLeft:
                    {
                    retValue = EHuiSkinLocationLeft;
                    break;
                    }
                }
            return retValue;
            }

        default:
            {
            return EHuiSkinLocationNone;
            }
        }
   }


MAknsSkinInstance* CHuiS60Skin::SkinInstance() const
    {
    MAknsSkinInstance* skin = 0;
    if ( CCoeEnv::Static())
        {
        skin = AknsUtils::SkinInstance();
        if(!skin )
            {
            TRAP_IGNORE( AknsUtils::InitSkinSupportL() )
            skin = AknsUtils::SkinInstance();
            }
        }
    return skin;
    }


void CHuiS60Skin::UpdateBackgroundL()
    {
    if(!iBackgroundTexture)
        {
        iBackgroundTexture = CreateSkinBackgroundL(KAknsIIDQsnBgScreen);
        iBackgroundTexture->SetSkinContent(ETrue);
        iBackgroundTexture->iContentObservers.AppendL(*this);
        }
    else if(iReloadBackground || !iBackgroundTexture->HasContent() )
        {
        iBackgroundTexture->Reset();
        delete iBackgroundBitmap;
        iBackgroundBitmap = NULL;
        iBackgroundBitmap = CHuiStatic::GetBgBitmapLC(KAknsIIDQsnBgScreen); 
        CleanupStack::Pop(iBackgroundBitmap);
        // update iBackgroundRect as well 
        TRect dummy;
        GetRectForItem(KAknsIIDQsnBgScreen, dummy, iBackgroundRect);
        iBackgroundTexture->UploadL(*iBackgroundBitmap,NULL,EHuiTextureUploadFlagRetainResolution );                
        iBackgroundTexture->SetSkinContent(ETrue);
        }
    else
        {
        // there should be already up-to-date background texture
        }
    HUI_DEBUG1(_L("CHuiS60Skin::UpdateBackgroundL - Free memory at exit: %i"), HuiUtil::FreeMemory());
    }

void CHuiS60Skin::FreeAllBackgroundTextureResources()
    {
    // delete old contents

    TBackgroundTexture bgTexture;
    TInt itemCount = ((TPrivData*)(iSpare))->iBackgrounds.Count(); 
    for (TInt index = 0; index < itemCount; index++)
        {
        bgTexture = ((TPrivData*)(iSpare))->iBackgrounds[index];
        if( bgTexture.iBackgroundTexture )
            {
            bgTexture.iBackgroundTexture->Reset();
            }
        }
    
    delete iBackgroundTexture;
    iBackgroundTexture = NULL;
    delete iBackgroundBitmap;
    iBackgroundBitmap = NULL;
    iBackgroundRect = TRect(0,0,0,0);
    iCachedSkinItems.ResetAndDestroy();      
    }


EXPORT_C void CHuiS60Skin::SkinContentChanged()
    {
    
    }

EXPORT_C void CHuiS60Skin::SkinConfigurationChanged(
    const TAknsSkinStatusConfigurationChangeReason aReason )
    {
    if (aReason == EAknsSkinStatusConfigurationDeployed)
        {
        Env().Display(0).SetSkinSizeChangePending();
        }
    }

EXPORT_C void CHuiS60Skin::SkinPackageChanged(
    const TAknsSkinStatusPackageChangeReason aReason )
    {
    
    }


EXPORT_C const CHuiTexture& CHuiS60Skin::TextureL(TInt aSkinTextureId)
    {
    // Create S60 theme specific background texture

    // If Bitgdi renderer is used, do not create a copy of background 
    // image, but return the default dummy texture.
    if(aSkinTextureId == EHuiSkinBackgroundTexture && 
    	CHuiStatic::Renderer().Id () != EHuiRenderPluginBitgdi) 
        {
        if(!iBackgroundTexture || ( iBackgroundTexture && !iBackgroundTexture->HasContent() ) ||  
           iReloadBackground )            
            {
            UpdateBackgroundL();
            iReloadBackground = EFalse;
            }
        return *iBackgroundTexture;
        }
        
    // Otherwise use the default textures        
    return CHuiSkin::TextureL(aSkinTextureId);
    }


EXPORT_C void CHuiS60Skin::ReleaseTexture(TInt aSkinTextureResource)
    {
    HUI_DEBUG(_L("CHuiS60Skin::ReleaseTexture() - Called."));
    
    // Release background texture
    if(aSkinTextureResource == EHuiSkinBackgroundTexture)
        {
        HUI_DEBUG(_L("CHuiS60Skin::ReleaseTexture() - Destroying background texture."));
        
        delete iBackgroundTexture;
        iBackgroundTexture = NULL;
        }
    else
        {
        // Otherwise use the default method
        return CHuiSkin::ReleaseTexture(aSkinTextureResource);
        }                
    }


EXPORT_C void CHuiS60Skin::NotifyDisplaySizeChangedL()
    {
    // Reload all the skin bitmaps
     FreeAllBackgroundTextureResources();
     Env().NotifySkinChangedL();
     Env().TextStyleManager().NotifyDisplaySizeChangedL();
     ReloadBgTexturesL();
    }


EXPORT_C TRgb CHuiS60Skin::StyleTextColor(THuiPreconfiguredTextStyle aStyle,
                                          THuiBackgroundType aBackgroundType) const
    {
    // Default text color depends directly on the background type.
    TRgb defaultColor = (aBackgroundType == EHuiBackgroundTypeLight? KRgbBlack : KRgbWhite);

    switch(aStyle)
        {
        default:
            break;

        case EHuiTextStyleSupplement:
            return TRgb(128, 128, 128);
        }

    return defaultColor;
    }


EXPORT_C void CHuiS60Skin::RestoreTextureContentL(CHuiTexture& aTexture)
    {
    if(&aTexture == iBackgroundTexture)
        {
        iReloadBackground = ETrue;
        TextureL(EHuiSkinBackgroundTexture);
        }
    else
        {
        CHuiSkin::RestoreTextureContentL(aTexture);
        }
    }

EXPORT_C void CHuiS60Skin::SkinExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    if (aExtensionUid == KHuiSkinReleaseCachedTextures)
        {
        FreeAllBackgroundTextureResources();
        }
    else
        {        
        CHuiSkin::SkinExtension(aExtensionUid, aExtensionParameters);
        }
    }
    
EXPORT_C THuiSkinOrientation CHuiS60Skin::Orientation() const
    {
    return CHuiSkin::Orientation();
    }
         
EXPORT_C void CHuiS60Skin::SetContext(CHuiGc& aGc)
    {
    CHuiSkin::SetContext(aGc);
    }

EXPORT_C CHuiGc& CHuiS60Skin::Context() const
    {
    return CHuiSkin::Context();
    }

EXPORT_C TInt CHuiS60Skin::GetTexture(TInt aSkinTextureResource, const CHuiTexture*& aOutTexture)
    {
    return CHuiSkin::GetTexture(aSkinTextureResource, aOutTexture);
    }


void CHuiS60Skin::FreeBackgrounds()
    {
    if (CHuiStatic::Renderer().Id () == EHuiRenderPluginBitgdi)
        {
        // no need to render the skin backgrounds separately on shitgdi
        return;
        }

    if (!iSpare)
        {
        return;
        }

    TBackgroundTexture bgTexture;
    TInt itemCount = ((TPrivData*)(iSpare))->iBackgrounds.Count(); 
    for (TInt index = 0; index < itemCount; index++)
        {
        bgTexture = ((TPrivData*)(iSpare))->iBackgrounds[index];
        delete bgTexture.iBackgroundTexture;
        bgTexture.iBackgroundTexture = NULL;
        }
    ((TPrivData*)(iSpare))->iBackgrounds.Reset();
    
    iCachedSkinItems.ResetAndDestroy(); // reset cached bg images & rects
    }
    
    
CHuiTexture* CHuiS60Skin::CreateSkinBackgroundL(const TAknsItemID& aID)
    {
    // Update the background image.
    if (!iSkinSrvConnected)
        {
        User::LeaveIfError(iSkinSrvSession.Connect(this));
        iSkinSrvConnected = ETrue;
        }
    
    MAknsSkinInstance* skin = SkinInstance();
    CHuiTexture* texture = CHuiTexture::NewL();
    
    CleanupStack::PushL(texture);
    
    if (skin)
        {
        TRect skinrect;
        TRect dummy;
        GetRectForItem(aID, dummy, skinrect);

        CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
        CleanupStack::PushL(bitmap);
        User::LeaveIfError( bitmap->Create(skinrect.Size(), EColor16MAP) );        

        CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(bitmap);
        CleanupStack::PushL(device);

        CFbsBitGc* gc = 0;
        User::LeaveIfError( device->CreateContext(gc) );
        CleanupStack::PushL(gc);
        iSkinControlContext->SetRect(skinrect);
        iSkinControlContext->SetBitmap(aID);

        AknsDrawUtils::DrawBackground(skin, iSkinControlContext, NULL, *gc, TPoint(0,0), skinrect,
                          KAknsDrawParamDefault);

        CleanupStack::PopAndDestroy(gc);
        CleanupStack::PopAndDestroy(device);

        texture->UploadL(*bitmap,NULL,EHuiTextureUploadFlagRetainResolution );            
        CleanupStack::PopAndDestroy(bitmap);
        }
    else
        {
        CFbsBitmap* bitmap = SearchCachedSkinItemBitmap(aID);
        if(!bitmap)
            {
            TRect skinrect;
            TRect dummy;
            GetRectForItem(aID, dummy, skinrect);

            if( aID == KAknsIIDQsnBgScreen) // handle normal background id differently
                {
                delete iBackgroundBitmap; 
                iBackgroundBitmap = NULL;
                bitmap = CHuiStatic::GetBgBitmapLC(aID);
                CleanupStack::Pop(bitmap);
                iBackgroundBitmap = bitmap;
                iBackgroundRect = skinrect;
                }
            else // others are cached in skin item array
                {
                bitmap = CHuiStatic::GetBgBitmapLC(aID);
                TInt index = SearchCachedSkinItemIndex(aID);
                if( index == KErrNotFound ) // add new
                    {
                    CSkinItem* newSkinItem = new (ELeave) CHuiS60Skin::CSkinItem();
                    CleanupStack::PushL(newSkinItem);
                    newSkinItem->iId = aID;
                    newSkinItem->iSkinRect = skinrect;
                    newSkinItem->iBitmap = bitmap;
                    User::LeaveIfError(iCachedSkinItems.Append(newSkinItem));
                    CleanupStack::Pop(newSkinItem);
                    }
                else // modify existing
                    {
                    iCachedSkinItems[index]->iSkinRect = skinrect;
                    delete iCachedSkinItems[index]->iBitmap;
                    iCachedSkinItems[index]->iBitmap = NULL; 
                    iCachedSkinItems[index]->iBitmap = bitmap;
                    }
                CleanupStack::Pop(bitmap);                
                }
            }
        texture->UploadL(*bitmap,NULL,EHuiTextureUploadFlagRetainResolution );            
        }

    CleanupStack::Pop(texture);
    return texture;
    }

void CHuiS60Skin::ReloadBgTexturesL()
    {
    if (CHuiStatic::Renderer().Id () == EHuiRenderPluginBitgdi )
        {
        // no need to render the skin backgrounds separately on bitgdi
        return;
        }
    iCachedSkinItems.ResetAndDestroy(); // reset cached bg images & rects
    
    TBackgroundTexture bgTexture;
    TInt itemCount = ((TPrivData*)(iSpare))->iBackgrounds.Count(); 
    for (TInt index = 0; index < itemCount; index++)
        {
        bgTexture = ((TPrivData*)(iSpare))->iBackgrounds[index];
        delete bgTexture.iBackgroundTexture;
        bgTexture.iBackgroundTexture = NULL;
        // put back. In case of a leave NULL texture is stored and we can try to re-create it later
        ((TPrivData*)(iSpare))->iBackgrounds[index] = bgTexture; 
        bgTexture.iBackgroundTexture = CreateSkinBackgroundL(bgTexture.iID);
        // texture created succesfully
        ((TPrivData*)(iSpare))->iBackgrounds[index] = bgTexture;
        }
    }

void CHuiS60Skin::UpdateBackgroundsL(const RArray<THuiDisplayBackgroundItem>& aItems)
    {
    if (CHuiStatic::Renderer().Id () == EHuiRenderPluginBitgdi)
        {
        // no need to render the skin backgrounds separately on shitgdi
        return;
        }
    FreeBackgrounds();    
    
    THuiDisplayBackgroundItem bgItem;
    TBackgroundTexture bgTexture;
    for (TInt index = 0; index < aItems.Count(); index++)
        {
        bgItem = aItems[index];
        if (bgItem.ClearMode() == CHuiDisplay::EClearWithSkinBackground)
            {
            bgTexture.iID = bgItem.SkinBackground();
            bgTexture.iBackgroundTexture = NULL;
            TRAPD(err, bgTexture.iBackgroundTexture = CreateSkinBackgroundL(bgItem.SkinBackground()));
            // if texture creation failed because our own internal services were busy
            // still append NULL texture so we know to try it again later
            if(err == KErrNone || err == KErrTimedOut )
                { 
                ((TPrivData*)(iSpare))->iBackgrounds.Append(bgTexture);
                }
            else
                {
                User::Leave(err);
                }
            }
        }           
    }

EXPORT_C CHuiTexture* CHuiS60Skin::BackgroundTexture(const TAknsItemID& aID)
    {
    if (CHuiStatic::Renderer().Id () == EHuiRenderPluginBitgdi)
        {
        // only opengl has separate skin bg textures.
        return NULL;
        }
    if (!iSpare)
        {
        return NULL;
        }
    TBackgroundTexture bgTexture;
    TInt itemCount = ((TPrivData*)(iSpare))->iBackgrounds.Count(); 
    for (TInt index = 0; index < itemCount; index++)
        {
        bgTexture = ((TPrivData*)(iSpare))->iBackgrounds[index];
        if (bgTexture.iID == aID)
            {
            if(!bgTexture.iBackgroundTexture || !bgTexture.iBackgroundTexture->HasContent())
                {
                delete bgTexture.iBackgroundTexture;
                bgTexture.iBackgroundTexture = NULL;
                TRAPD(err, bgTexture.iBackgroundTexture = CreateSkinBackgroundL(bgTexture.iID));
                // put bgTexture back to the array also in leave cases
                // otherwise we could leave invalid pointer in array
                ((TPrivData*)(iSpare))->iBackgrounds[index] = bgTexture;
                if(err)
                    {
                    return NULL; // creating failed for some reason
                    }
                }
            return bgTexture.iBackgroundTexture;
            }
        }
    return NULL;
    }


TInt CHuiS60Skin::SearchCachedSkinItemIndex(const TAknsItemID& aId)
    {
    TInt cacheditemsCount = iCachedSkinItems.Count();
    for(TInt i = 0; i < cacheditemsCount; i++ )
        {
        if( iCachedSkinItems[i]->iId == aId )
            {
            return i;
            }
        }

    HUI_DEBUG2(_L("CHuiS60Skin::SeachCachedSkinItemIndex - cached TAknsItemID %i %i (iMajor, iMinor) not found"), aId.iMajor, aId.iMinor );

    return KErrNotFound;
    }

TRect CHuiS60Skin::SearchCachedSkinItemRect(const TAknsItemID& aId)
    {
    TRect returnRect = TRect();
    if(aId == KAknsIIDQsnBgScreen)
        {
        returnRect = iBackgroundRect;
        }
    else
        {
        TInt index = SearchCachedSkinItemIndex(aId);
        if(index != KErrNotFound )
            {
            returnRect = iCachedSkinItems[index]->iSkinRect;
            }
        }   
    return returnRect;
    }

CFbsBitmap* CHuiS60Skin::SearchCachedSkinItemBitmap(const TAknsItemID& aId)
    {
    CFbsBitmap* bitmap = NULL;
    if(aId == KAknsIIDQsnBgScreen)
        {
        bitmap = iBackgroundBitmap;
        }
    else
        {
        TInt index = SearchCachedSkinItemIndex(aId);
        if(index != KErrNotFound )
            {
            bitmap = iCachedSkinItems[index]->iBitmap;
            }
        }   
    return bitmap;    
    }


TRect CHuiS60Skin::SkinRect(const TAknsItemID& aID)
    {
    return SearchCachedSkinItemRect(aID);
    }
