/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Skinnable image
*
*/



#include <alf/alfconstants.h>
#include "alfskinnableimage.h"
#include "alf/alfimageloaderutil.h"
#include "alf/alftexturemanager.h"
#include "alf/alftexture.h"
#include "alf/alfenv.h"
#include "alfclient.h"

#include <uiacceltk/HuiUtil.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSkinnableImage::CAlfSkinnableImage(CAlfEnv* aEnv)
    {
    iEnv = aEnv;    
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CAlfSkinnableImage::~CAlfSkinnableImage()
    {
    if (Image().HasTexture())
        {
        CAlfTextureManager& oldManager = ImageTextureManager(Image());
        oldManager.DecRefcount(iId);        
        }
    
    DeleteTexture();

    // Remove size observer if needed
    if (iSkinnedImageUtil)
       {
       CAlfTextureManager& manager = ImageTextureManager(iSkinnedImageTextureManagerId);        
       manager.RemoveAutoSizeObserver(iSkinnedImageUtil);
       }
       
    delete iSkinnedImageUtil; 
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    

void CAlfSkinnableImage::SetImage(const TAlfImage& aImage)
    {    
    CAlfTextureManager& oldManager = ImageTextureManager(Image());
    CAlfTextureManager& newManager = ImageTextureManager(aImage);
    
    if (Image().HasTexture())
        {
        oldManager.DecRefcount(iId);        
        }
        
    if ( iSkinnedImageUtil )
        {
        oldManager.RemoveAutoSizeObserver(iSkinnedImageUtil);   
        delete iSkinnedImageUtil;
        iSkinnedImageUtil = NULL;
        } 
    
    iImage = aImage;
    TRAP_IGNORE(CreateTextureL()); // Creates texture from skin id if needed           

    if (Image().HasTexture())
        {
        CAlfTexture* texture = (CAlfTexture*)&Image().Texture();
        iId = texture->Id();        
        newManager.IncRefcount(iId);        
        
        // If texture does not have content and it has been released because of
        // ref counting try to restore content.
        if (!texture->HasContent())        
            {
            TInt releaseFlags = texture->ReleaseFlags();  
            if (releaseFlags & CAlfTexture::EReleaseFromRefCounting)
                {
                // Clear release flag 
                releaseFlags &= ~CAlfTexture::EReleaseFromRefCounting;    
                texture->SetReleaseFlags(releaseFlags);
            
                // If there are no more release flags, try to restore content.
                if (!releaseFlags)
                    {                                        
                     if (texture->BitmapProvider())
                        {
                        TRAP_IGNORE(newManager.UpdateTextureFromBitmapL(iId))    
                        }
                    else
                        {
                        TRAP_IGNORE(newManager.UpdateTextureFromFileL(iId))                        
                        }    
                    }
                }
            }
        }
    else
        {
        iId = 0;
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TAlfImage& CAlfSkinnableImage::Image()
    {
    return iImage;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CAlfSkinnableImage::CreateTextureL()
    {
    if (iImage.SkinnableImage())
        {      
        // old iSkinnedImageUtil should be freed already
        __ASSERT_DEBUG( !iSkinnedImageUtil, USER_INVARIANT() );
        iSkinnedImageUtil = new (ELeave) CAlfAutoSizeImageLoaderUtil;
        
        CAlfTextureManager& tm = ImageTextureManager(iImage);
        
        // Add size observer if needed
        if (iImage.TextureFlags() & EAlfTextureFlagAutoSize)
            {
            tm.AddAutoSizeObserverL(iSkinnedImageUtil);        
            }        

        CAlfTexture* texture = NULL;
        iSkinnedImageUtil->SetSize(iImage.PreferredSize(), iImage.PreferredScaleMode());
        
        MAlfBitmapProvider* imageprovider = NULL;
        if (iImage.AppIconUid() != TUid::Uid(0))
            {
            imageprovider = iSkinnedImageUtil->CreateImageLoaderL(iImage.AppIconUid(), 
                iImage.AppIconType());                
            }
        else
            {
            imageprovider = iSkinnedImageUtil->CreateImageLoaderL(iImage.SkinID(), 
                *iImage.FallbackFileName(),
                iImage.FallbackBitmapId(),
                iImage.FallbackMaskId());                
            }    
        
        texture = &tm.CreateTextureL(iImage.PreferredTextureId(), imageprovider, iImage.TextureFlags());                     
        iImage.SetTexture(*texture);
        
        // Delete old texture if we managed to create new one 
        if (texture)
            {
            // Check that the new one is really a new one, not just reusing old id.
            if (texture->Id() != iSkinnedImageTextureId || tm.ManagerUid() != iSkinnedImageTextureManagerId)
                {
                DeleteTexture();
                iSkinnedImageTextureId = texture->Id();
                iSkinnedImageTextureManagerId = tm.ManagerUid();                    
                }            
            }                    
        }
    }

// ---------------------------------------------------------------------------
// This function deletes texture associated to image, but does not delete
// the bitmapprovider (it is left to support release/restore).
// ---------------------------------------------------------------------------
//    
void CAlfSkinnableImage::DeleteTexture()
    {
    if (iSkinnedImageTextureId != 0)
        {
        CAlfTextureManager& manager = ImageTextureManager(iSkinnedImageTextureManagerId);        
        const CAlfTexture* oldtexture = manager.Texture(iSkinnedImageTextureId);
        if (oldtexture && oldtexture->Id() == iSkinnedImageTextureId)
            {
            delete oldtexture;    
            iSkinnedImageTextureId = 0;
            iSkinnedImageTextureManagerId = TUid::Uid(0);                               
            }                                        
        }                
    }
    
// ---------------------------------------------------------------------------
// Gets the texturemanager for the texture inside given image. Defaults
// to default texture manager.
// ---------------------------------------------------------------------------
//    
CAlfTextureManager& CAlfSkinnableImage::ImageTextureManager(const TAlfImage& aImage)
    {
    CAlfTextureManager* tm = aImage.PreferredTextureManager();
    if (!tm)
        {
        tm = &iEnv->TextureManager();                    
        }
    return *tm;        
    }
    
// ---------------------------------------------------------------------------
// Gets the texturemanager based on texturemanager uid. Defaults
// to default texture manager.
// ---------------------------------------------------------------------------
//    
CAlfTextureManager& CAlfSkinnableImage::ImageTextureManager(TUid aManagerUid)
    {
    CAlfTextureManager* tm = &iEnv->TextureManager();
    if (iSkinnedImageTextureManagerId != TUid::Uid(0))
        {
        tm = iEnv->SharedTextureManager(aManagerUid);
        }
    return *tm;             
    }
    
