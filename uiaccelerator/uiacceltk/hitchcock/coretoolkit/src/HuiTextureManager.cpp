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
* Description:   Implementation of CHuiTextureManager, texture objects loading 
*                and management.
*
*/



#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>

#include "uiacceltk/HuiTextureManager.h"  // Class definition
#include "uiacceltk/HuiTextureProcessor.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiDisplay.h"
#include "huitextureanimationstate.h"

/**
 * Granularity of CHuiTextureManager::iAnimations and 
 * CHuiTextureManager::iAnimatedTextureGroupItems arrays.
 */
const TInt KHuiAnimationsGranularity = 4;

EXPORT_C MHuiBitmapProvider::~MHuiBitmapProvider()
    {
    }

EXPORT_C CHuiTextureManager::CHuiTextureManager(CHuiEnv& aEnv)
        : CActive(EPriorityHigh),
        iEnv(aEnv), iState(EIdle), iReleaseState(ENormal), iFS(CHuiStatic::FsSession()),
        iAnimatedTextureGroupItems( KHuiAnimationsGranularity ),
        iAnimations( KHuiAnimationsGranularity ),
        iEnableTexMemCalculation( EFalse )  //Texture memory calculation is disabled by default
    {}


EXPORT_C void CHuiTextureManager::BaseConstructL()
    {
    iEnv.SetTextureManager(*this);

    SetImagePathL(_L(""));

    // Create a small initial bitmaps for decoder output.
    iBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iBitmap->Create(TSize(4, 4), EColor64K) );
    iMaskBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iMaskBitmap->Create(TSize(4, 4), EGray256) );

    // Create blank texture
    iBlankTexture = CHuiStatic::Renderer().CreateTextureL();
    iBlankTexture->SetImageFileNameL(_L("blank"));

    CActiveScheduler::Add(this);
    
    // Enabling logging will cause diagnostics and debugging messages to be
    // printed to a file.
    CHuiStatic::EnableLogging(EFalse);
    }


EXPORT_C CHuiTextureManager::~CHuiTextureManager()
    {
    Cancel();

    delete iAnimationWithoutGroup;
    iAnimatedTextureGroupItems.Close();
    iAnimations.ResetAndDestroy();
    iAnimations.Close();

    // Delete the texture processor.
    delete iProcessor;

    // Clear any remaining entries in the load queue.
    for(TInt i = 0; i < iLoadQueue.Count(); ++i)
        {
        delete iLoadQueue[i].iDecoder;
        iLoadQueue[i].iDecoder = 0;
        }
    iLoadQueue.Reset();

    delete iBlankTexture;

    // Remove all texture entries (from last to first -order)
    /*while (iTextures.Count()>0)*/
    for(TInt i = iTextures.Count()-1; i >= 0; --i)
        {
        CTextureEntry * te = iTextures[i];
        
        if (te->iTexture)
            {
            delete te->iTexture;
            }
        else
            {
            // This texture was defined but never loaded.
            HUI_DEBUG2(_L("CHuiTextureManager::~CHuiTextureManager() - Notice, texture id %i (\"%S\") was never loaded!"), te->iId, te->iFileName);
            }
        }
        
    iTextures.ResetAndDestroy();
    iStateObservers.Close();
    iLoadObservers.Close();
    iAnimatedTextures.Close();
    iTextureAutoSizeObservers.Close();

    delete iBitmap;
    delete iMaskBitmap;
    delete iImagePath;
    }


EXPORT_C CHuiEnv& CHuiTextureManager::Env()
    {
    return iEnv;
    }


EXPORT_C CHuiTexture* CHuiTextureManager::Texture(TInt aId)
    {
    TInt index = CheckTexture(aId);
    if(index == -1)
        {
        HUI_DEBUG1(_L("CHuiTextureManager::Texture() - Texture id %i not found! Returning blank."), aId);
        // NOTE: there is a danger of editing the blank texture
        // instead of the real texture..
        return &BlankTexture();
        }
    CHuiTexture * texture = iTextures[index]->iTexture;
    if(texture)
        {
        return texture;
        }
    else
        {
        HUI_ASSERT(EFalse); // texture exists but is not loaded! (defined but not loaded)
        return &BlankTexture();
        }
    }


EXPORT_C const CHuiTexture* CHuiTextureManager::Texture(TInt aId) const
    {
    TInt id = CheckTexture(aId);
    if(id == -1)
        {
        return &BlankTexture();
        }
    const CHuiTexture * texture = iTextures[id]->iTexture;
    if(texture)
        {
        return texture;
        }
    else
        {
        HUI_ASSERT(EFalse); // texture exists but is not loaded! (defined but not loaded)
        return &BlankTexture();
        }
    }


EXPORT_C CHuiTexture* CHuiTextureManager::TextureL(TInt aId)
    {
    // try to get the index for the texture id
    TInt index = CheckTexture(aId);
    if(index != -1)
        {
        // If the texture has content and a filename, we can do a load-on-demand.       
        if(!iTextures[index]->iTexture->HasContent() && iTextures[index]->iFileName)
            {
            // we don't want to return textures without content
            // so load-on-demand      
                    
            // texture has an entry but the
            // actual texture object has not been loaded yet

            // we can load the texture if there's a filename
            // available for this id
            return &LoadTextureL(aId);
            }
            
        // texture found  
        return iTextures[index]->iTexture;
        }

    User::Leave(KErrNotFound);
    return NULL; // never
    }


EXPORT_C void CHuiTextureManager::SetImagePathL(const TDesC& aPath)
    {
    delete iImagePath;
    iImagePath = NULL;

    TParsePtrC parse(aPath);
    CEikonEnv* coe = CEikonEnv::Static();
    if (aPath.Length() && !parse.DrivePresent() && coe && coe->EikAppUi() && coe->EikAppUi()->Application())
        {
        iImagePath = HBufC::NewL(aPath.Size()+2); // two extra characters for drive
        TPtr ptr = iImagePath->Des();
        ptr.Append(coe->EikAppUi()->Application()->AppFullName().Left(2));
        ptr.Append(aPath);
        }
    else
        {
        iImagePath = aPath.AllocL();        
        }
    }

EXPORT_C const TDesC& CHuiTextureManager::ImagePath() const
    {
    return *iImagePath;
    }


EXPORT_C const CHuiTexture& CHuiTextureManager::BlankTexture() const
    {
    HUI_ASSERT(iBlankTexture!=NULL); // this MUST have a valid pointer otherwise access violation will follow..
    return *iBlankTexture;
    }


EXPORT_C CHuiTexture& CHuiTextureManager::BlankTexture()
    {
    HUI_ASSERT(iBlankTexture!=NULL); // this MUST have a valid pointer otherwise access violation will follow..
    return *iBlankTexture;
    }


EXPORT_C TBool CHuiTextureManager::SetTextureId(CHuiTexture* aTexture, TInt aId)
    {
    
    if(!aTexture)
        {
        // Cannot set texture ID for null texture.
        return EFalse;
        }
    
    // Iterate through all the managed textures to find the one we
    // are interested in, and then set its texture ID.
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        if(iTextures[i]->iTexture == aTexture)
            {
            // Texture was found in the list, so set the new identifier.
            iTextures[i]->iId = aId;
            return ETrue;
            }
        }

    // Texture was not found in the manager's list.
    return EFalse;
    }

EXPORT_C TBool CHuiTextureManager::IsLoaded(const CHuiTexture * texture) const
    {
    if(texture != NULL && texture->HasContent() && !IsInLoadQueue(texture))
        {
        return ETrue;
        }
    return EFalse;
    }


EXPORT_C TBool CHuiTextureManager::IsLoaded(const TDesC& aImageName, const TInt aFrameNumber) const
    {
    TFileName fileName = *iImagePath;
    fileName.Append(aImageName);
    TInt index = CheckTexture(fileName, aFrameNumber);
    if(index < 0)
        {
        HUI_DEBUG1(_L("CHuiTextureManager::IsLoaded() - Texture \"%S\" was not found. Assuming not loaded."), &aImageName);
        return EFalse;
        }
    return IsLoaded(iTextures[index]->iTexture);
    }


EXPORT_C TBool CHuiTextureManager::IsLoaded(TInt aId) const
    {
    HUI_ASSERT(aId!=0);
    TInt index = CheckTexture(aId);
    if(index < 0)
        {
        HUI_DEBUG1(_L("CHuiTextureManager::IsLoaded() - Texture having id %i was not found. Assuming not loaded."), aId);
        return EFalse;
        }
    return IsLoaded(iTextures[index]->iTexture);
    }


EXPORT_C void CHuiTextureManager::AppendTextureL(CHuiTexture* aTexture, TInt aId)
    {
    // Panic if the manager is currently restoring or releasing textures.
    if(iReleaseState == EReleasing)
        {
        THuiPanic::Panic(THuiPanic::ETextureManagerTextureDestroyedDuringRelease);
        }
    else if(iReleaseState == ERestoring)
        {
        THuiPanic::Panic(THuiPanic::ETextureManagerTextureConstructedDuringRestore);
        }
    else
        {
        // for PC lint
        }

    HUI_ASSERT(aTexture != NULL);

#ifdef _DEBUG
    // Look for an existing entry for this.
    // There must not be any duplicates present!
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        if(iTextures[i]->iTexture == aTexture)
            {
            User::Leave(KErrAlreadyExists);
            }
        }
#endif

    // Create a new texture entry
    CTextureEntry * te = new (ELeave) CTextureEntry(aId, aTexture);
    CleanupStack::PushL(te);
    // Add the new entry to the list of new textures
    User::LeaveIfError( iTextures.Append(te) );
    CleanupStack::Pop(); // pop the texture entry

    HUI_DEBUG1(_L("CHuiTextureManager::AppendTextureL() - Registered texture 0x%x."), aTexture);
    }


EXPORT_C void CHuiTextureManager::RemoveTexture(CHuiTexture& aTexture)
    {
    // Panic if the manager is currently restoring or releasing textures.
    if(iReleaseState == EReleasing)
        {
        THuiPanic::Panic(THuiPanic::ETextureManagerTextureDestroyedDuringRelease);
        }
    else if(iReleaseState == ERestoring)
        {
        THuiPanic::Panic(THuiPanic::ETextureManagerTextureDestroyedDuringRestore);
        }
    else
        {
        // for PC lint
        }
    
    // Cancel loading of the texture to be removed.
    CancelLoadingOfTexture(aTexture);

    // Manual sequential search.
    for(TInt index = iTextures.Count() - 1; index >= 0; --index)
        {
        if(iTextures[index]->iTexture == &aTexture)
            {
            CTextureEntry * entry = iTextures[index];
            if ( entry->iFileName )
                {
                HUI_DEBUG4(_L("CHuiTextureManager::RemoveTexture() - Removing texture 0x%x  (Name: \"%S\", Id: %i). Number of textures after deleting: %i."),
                              &aTexture, &(aTexture.ImageFileName()), entry->iId, iTextures.Count()-1);                     
                }
            else
                {
                HUI_DEBUG3(_L("CHuiTextureManager::RemoveTexture() - Removing unnamed texture 0x%x  ( Id: %i). Number of textures after deleting: %i."),
                              &aTexture, entry->iId, iTextures.Count()-1);                     
                }

            iTextures.Remove(index);
            delete entry;
            return;
            }
        }

    HUI_DEBUG2(_L("CHuiTextureManager::RemoveTexture() - Error! Failed to remove texture 0x%x \"%S\". Texture not found."), &aTexture, &aTexture.ImageFileName());
    // this is a programming error.. texturemanager is aware of all textures in the toolkit!!
    #ifdef _DEBUG
    HUI_PANIC(THuiPanic::ETextureNotValid)
    #endif
    }


EXPORT_C CHuiTexture& CHuiTextureManager::LoadTextureL(
    const TDesC& aImageName,
    THuiTextureUploadFlags aFlags,
    TInt aId,
    TInt aFrameNumber)
    {
    return LoadTextureL(aImageName, TSize(0, 0), aFlags, aId, aFrameNumber);
    }


EXPORT_C CHuiTexture& CHuiTextureManager::LoadTextureL(const TInt aId,
                                       const TSize& aTextureMaxSize,
                                       THuiTextureUploadFlags aFlags)
    {
    return LoadTextureL(KNullDesC(), aTextureMaxSize, aFlags, aId);
    }


EXPORT_C CHuiTexture& CHuiTextureManager::LoadTextureL(const TDesC& aImageName,
        const TSize& aTextureMaxSize,
        THuiTextureUploadFlags aFlags,
        TInt aId,
        TInt aFrameNumber)
    {
    // Ignore EHuiTextureFlagAllowDirectBitmapUsage flag
   	aFlags = (THuiTextureUploadFlags)(aFlags & ~EHuiTextureFlagAllowDirectBitmapUsage);		
    
    TFileName fileName;

    CTextureEntry * entry = NULL;  

    // If the name is invalid, and there was no filename available
    // based on id, return a dummy texture.
    if((aImageName.Length() == 0) && (aId == 0))
        {
        HUI_DEBUG(_L("CHuiTextureManager::LoadTextureL() - No filename or id provided for texture. Returning blank."));
        return BlankTexture();
        }

    // add path to filename if filename has been passed
    if(aImageName.Length() > 0)
        {
        // assume relative pathname and prepend the image path to get full filename
        fileName = aImageName;
        PrependImagePath(fileName);
        }

    // if no name has been passed as a parameter but
    // a nonzero id has been passed, we
    // can assume that there's a predefined name available
    if((aImageName.Length() == 0) && (aId != 0))
       {
        // search for a texture filename based on the id.
        TInt index = CheckTexture(aId);
        if(index >=0)
            {
            entry = iTextures[index];
            HUI_ASSERT(entry->iFileName);
            fileName = *(entry->iFileName);
            }
        else
            {
            HUI_DEBUG1(_L("CHuiTextureManager::LoadTextureL() - Unable to load texture without filename or pre-defined id (filename definition for id %i not found)."), aId);
            User::Leave(KErrNotFound);
            }
       }

    // a guarantee that there's a filename available
    HUI_ASSERT(	fileName.Length()>0 );

    // Reuse pre-existing entries:
    // try first finding an entry based on id
    if(entry == NULL && aId != 0)
        {
        TInt previouslyLoadedIndex = CheckTexture(aId);
        if (previouslyLoadedIndex >= 0)
            {
            entry = iTextures[previouslyLoadedIndex];
            }
        }

    // We are not allowed to change required texture id so we can't 
    // try again with the filename unless user does not care about id
    if(aId == 0 && entry == NULL)
        {
        TInt previouslyLoadedIndex = CheckTexture(fileName, aFrameNumber);
        if (previouslyLoadedIndex >= 0)
            {
            entry = iTextures[previouslyLoadedIndex];
            }
        }

    // create new texture entry if there's not one
    // available at this point
    if(entry == NULL)
        {
        HUI_DEBUG(_L("CHuiTextureManager::LoadTextureL() - Creating new texture."));

        // in the case that a previously loaded slot was not found,
        // create a new texture, which automatically registers a new
        // texture entry in the iTextures list.
        CHuiTexture::NewL();

        // we assume that the newest entry was added the last
        entry = iTextures[iTextures.Count()-1];

        entry->iId = aId;
        }
    else
        {
        // We have a valid texture name and ID.
        HUI_ASSERT(entry->iTexture);
      
        // we have a pre-existing texture,
        // but check that the texture is loaded ok, we can also
        // return unloaded textures that are in the load queue
        if (entry->iTexture->HasContent() || IsInLoadQueue(entry->iTexture))
            {
            // a previous texture exists!
            HUI_ASSERT(entry->iTexture!=NULL);            
            
            HUI_DEBUG3(_L("CHuiTextureManager::LoadTextureL() - Reusing texture 0x%x (\"%S\", id: %i) No need to load."),
                      entry->iTexture, &entry->iTexture->ImageFileName(), entry->iId);
            return *(entry->iTexture);
            }
        }

    HUI_ASSERT(entry!=NULL);
    HUI_ASSERT(entry->iTexture!=NULL);
       
    // replace filename
    // File names are relative to the image path.
    entry->SetFileName(fileName);
    entry->iTexture->SetImageFileNameL(fileName);
    entry->iTexture->EnableShadow((aFlags & EHuiTextureUploadFlagGenerateShadow) != 0);
    entry->iFrameNumber = aFrameNumber;

    // add a content observer so that we can reload the texture
    // during restoring
    if (entry->iTexture->iContentObservers.Count()==0)
        {
        HUI_DEBUG2(_L("CHuiTextureManager::LoadTextureL() - Adding content observer 0x%x for texture 0x%x."),  entry, entry->iTexture);
        entry->iTexture->iContentObservers.AppendL(*entry);
        }

    HUI_DEBUG3(_L("CHuiTextureManager::LoadTextureL() - Enqueuing texture 0x%x (Name: \"%S\", Id: %i)."),
               entry->iTexture, &entry->iTexture->ImageFileName(), entry->iId);

    // Prepare for loading by creating a load queue entry.
    SLoadQueueEntry loadqentry;
    Mem::FillZ(&loadqentry, sizeof(loadqentry));
    loadqentry.iLoading = entry;
    entry->iMaxTextureSize = aTextureMaxSize;
    entry->iFlags = aFlags;

    // Textures are loaded one at a time, in the order they were requested.
    User::LeaveIfError( iLoadQueue.Append(loadqentry) );

    // Start loading images.
    if(iState == EIdle)
        {
        StartLoading();
        
        if(iState == ELoading)
            {
            // Notify observers that loading has started.
            NotifyStateChange();
            }
        }

    return *entry->iTexture;
    }

EXPORT_C CHuiTexture&
CHuiTextureManager::CreateTextureL(TInt aId,
                                   MHuiBitmapProvider* aBitmapProvider,
                                   THuiTextureUploadFlags aFlags)
    {
    CHuiTexture* tex;
    
    // can't create texture without id (or filename)
    if (aId==0)
        {
        HUI_DEBUG(_L("CHuiTextureManager::CreateTextureL() - ERROR! Can't create texture without valid id. Please use nonzero ids."));
        User::Leave(KErrArgument);
        }

    // Provide an already created texture if such exists.
    CHuiTexture* texture = NULL;
    TRAPD(err, texture = TextureL(aId));
    if(err == KErrNone)
        {
        ASSERT( texture );

        // Update flags here because there is no other way to change flags. 
        CTextureEntry* entry = TextureEntry(CheckTexture(aId));
        if (entry)
            {
            entry->iFlags = aFlags;        
            }
        
        return *texture;
        }

    // Assert that the bitmap provider object exists.
    HUI_ASSERT(aBitmapProvider != NULL);

    HUI_DEBUG1(_L("CHuiTextureManager::CreateTextureL() - Requesting bitmap content from provider 0x%x .."), aBitmapProvider);

    // Initialize bitmap and mask bitmap pointers to zero.
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* maskBitmap = NULL;

    // Call the bitmapProvider method to load the bitmaps.
    ASSERT( aBitmapProvider );
    aBitmapProvider->ProvideBitmapL(aId, bitmap, maskBitmap);

    // Error if main bitmap could not be loaded
    User::LeaveIfNull(bitmap);
    CleanupStack::PushL(bitmap);
    
    UpdateFlags(*bitmap, maskBitmap, aFlags);
    
    if (maskBitmap)
        {
        // A mask was provided, so pass this to texture creation.
        CleanupStack::PushL(maskBitmap);
        // create textures right away for these bitmaps
        tex = &CreateTextureL(*bitmap, maskBitmap, aFlags, aId);
        // get rid of bitmap and mask
        CleanupStack::PopAndDestroy(maskBitmap);
        }
    else
        {
        // No mask, so create texture without one.
        // create textures right away for these bitmaps
        tex = &CreateTextureL(*bitmap, NULL, aFlags, aId);            
        }
        
    CleanupStack::PopAndDestroy(bitmap);        

    // store the provider with the texture for Restore operations
    HUI_ASSERT(CheckTexture(aId)>=0);
    CTextureEntry * entry = TextureEntry(CheckTexture(aId));
    entry->iBitmapProvider = aBitmapProvider;
    entry->iFlags = aFlags;

    HUI_ASSERT(entry->iTexture);

    // add a content observer so that we can reload the texture
    // during restoring
    if (entry->iTexture->iContentObservers.Count()==0)
        {
        HUI_DEBUG2(_L("CHuiTextureManager::CreateTextureL() - Adding content observer 0x%x for texture 0x%x."),  entry, entry->iTexture);
        entry->iTexture->iContentObservers.AppendL(*entry);
        }

    return *tex;
    }

EXPORT_C CHuiTexture& CHuiTextureManager::CreateTextureL(CFbsBitmap& aBitmap,
                                   const CFbsBitmap* aMask,
                                   THuiTextureUploadFlags aFlags,
                                   TInt aId)
    {
    TBool hasAlpha = (aMask != NULL);
    TSize bitmapSize = aBitmap.SizeInPixels();

    CHuiStatic::Tic(2);

    // Verify that the bitmap size is same as the mask size.
    if(hasAlpha && (bitmapSize != aMask->SizeInPixels()))
        {
        HUI_DEBUG(_L("CHuiTextureManager::CreateTextureL() - Bitmap size is not compatible with the mask bitmap!"));
        User::Leave(KErrArgument);
        }

    // Leave if the name and the id are empty.
    if(aId==0)
        {
        HUI_DEBUG(_L("CHuiTextureManager::CreateTextureL() - ERROR! Can't create texture without valid id or name."));
        User::Leave(KErrArgument);
        }

    // If the texture has already been loaded, just return a reference.
    TInt previouslyLoadedIndex = CheckTexture(aId);

    if((previouslyLoadedIndex != -1) && IsLoaded(aId))
        {
        CHuiTexture * tex = iTextures[previouslyLoadedIndex]->iTexture;
        HUI_ASSERT(tex);
        HUI_DEBUG3(_L("CHuiTextureManager::CreateTextureL() - Reusing previously loaded texture (0x%x, \"%S\", id: %i)."),
                     tex, &tex->ImageFileName(), iTextures[previouslyLoadedIndex]->iId);
        return *tex;
        }

    // create and append to the textures list (automatic)
    CHuiTexture* texture = CHuiTexture::NewL();
    // Set texture name
    texture->SetImageFileNameL(_L(""));
    // Set the texture id.
    HUI_ASSERT(TextureEntryCount()>0);
    CTextureEntry * te = TextureEntry(TextureEntryCount()-1);
    te->iId = aId;

    HUI_DEBUG3(_L("CHuiTextureManager::CreateTextureL() - Created new texture 0x%x (\"%S\", id: %i)."),
                     texture, &texture->ImageFileName(), aId);

    // Let the texture know its size as soon as possible.
    texture->SetSize(bitmapSize);

    HUI_DEBUG6(_L("CHuiTextureManager::CreateTextureL() - Image for texture 0x%x is \"%S\": w=%i, h=%i, dm=%i, mdm=%i"),
               texture,
               &texture->ImageFileName(),
               bitmapSize.iWidth,
               bitmapSize.iHeight,
               aBitmap.DisplayMode(),
               (aMask != NULL ? aMask->DisplayMode() : ENone));

    // Enable texture shadow.
    texture->EnableShadow((aFlags & EHuiTextureUploadFlagGenerateShadow) != 0);

	if (bitmapSize.iWidth != 0 && bitmapSize.iHeight != 0)
	    {
    	if (aFlags & EHuiTextureFlagAllowDirectBitmapUsage) 
    		{
       		texture->UploadDirectL(aBitmap, aMask, aFlags);		
    		}
    	else 
    		{
       		texture->UploadL(aBitmap, aMask, aFlags);		
    		}

	    }

    HUI_DEBUG1(_L("CHuiTextureManager::CreateTextureL() - Upload for texture 0x%x done, notifying possible observers.."), texture);

    // Store bits per pixel info to calculate texture memory usage
    te->iBitDepth = TDisplayModeUtils::NumDisplayModeBitsPerPixel( aBitmap.DisplayMode() );
    if( aMask )
    	{
    	 te->iBitDepth =  te->iBitDepth + TDisplayModeUtils::NumDisplayModeBitsPerPixel( aMask->DisplayMode() );
    	}

    // Notify the observers
    NotifyTextureLoaded(*texture, aId, KErrNone);

    HUI_DEBUG2(_L("CHuiTextureManager::CreateTextureL() - Finished with texture 0x%x, toc: %f seconds"), texture, CHuiStatic::Toc(2));
    return *texture;
    }


EXPORT_C void CHuiTextureManager::UpdateTextureFromBitmapL(TInt aId, MHuiBitmapProvider* aBitmapProvider)
    {
    TInt previouslyLoadedIndex = CheckTexture(aId);
    if (previouslyLoadedIndex == KErrNotFound)
    	{
    	User::Leave(KErrNotFound);
    	}

	// Unload
	
    UnloadTexture(aId);      

	// Upload with the new content
	
	CTextureEntry* entry = iTextures[previouslyLoadedIndex];
    
    HUI_DEBUG1(_L("CHuiTextureManager::UpdateTextureFromBitmapL() - Requesting bitmap content from provider 0x%x .."), aBitmapProvider);

    // Initialize bitmap and mask bitmap pointers to zero.
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* maskBitmap = NULL;

    // Call the bitmapProvider method to load the bitmaps.
    if (!aBitmapProvider)
    	{
    	aBitmapProvider = entry->iBitmapProvider;
    	}
    if (aBitmapProvider)
    	{
   	    aBitmapProvider->ProvideBitmapL(aId, bitmap, maskBitmap);
    	}

    // Error if main bitmap could not be loaded
    if (!bitmap)
    	{
    	User::Leave(KErrNotFound);
    	}
    CleanupStack::PushL(bitmap);
    
    if (maskBitmap)
        {
        CleanupStack::PushL(maskBitmap);
        }

    TBool hasAlpha = (maskBitmap != NULL);
    TSize bitmapSize = bitmap->SizeInPixels();

    CHuiStatic::Tic(2);

    // Verify that the bitmap size is same as the mask size.
    if(hasAlpha && (bitmapSize != maskBitmap->SizeInPixels()))
        {
        HUI_DEBUG(_L("CHuiTextureManager::UpdateTextureFromBitmapL() - Bitmap size is not compatible with the mask bitmap!"));
        User::Leave(KErrArgument);
        }

    CHuiTexture* texture = entry->iTexture;
    // Set texture name
    texture->SetImageFileNameL(_L(""));

    HUI_DEBUG6(_L("CHuiTextureManager::UpdateTextureFromBitmapL() - Image for texture 0x%x is \"%S\": w=%i, h=%i, dm=%i, mdm=%i"),
               texture,
               &texture->ImageFileName(),
               bitmapSize.iWidth,
               bitmapSize.iHeight,
               bitmap->DisplayMode(),
               (maskBitmap != NULL ? maskBitmap->DisplayMode() : ENone));

    UpdateFlags(*bitmap, maskBitmap, entry->iFlags);
    
	if (bitmapSize.iWidth != 0 && bitmapSize.iHeight != 0)
	    {
    	if (entry->iFlags & EHuiTextureFlagAllowDirectBitmapUsage) 
    		{
       		texture->UploadDirectL(*bitmap, maskBitmap, entry->iFlags);		
    		}
    	else 
    		{
       		texture->UploadL(*bitmap, maskBitmap, entry->iFlags);		
    		}
	    }
    HUI_DEBUG2(_L("CHuiTextureManager::UpdateTextureFromBitmapL() - Finished with texture 0x%x, toc: %f seconds"), texture, CHuiStatic::Toc(2));
     
    // Store bits per pixel info to calculate texture memory usage
    entry->iBitDepth = TDisplayModeUtils::NumDisplayModeBitsPerPixel( bitmap->DisplayMode() );
    if (maskBitmap)
    	{
    	entry->iBitDepth = entry->iBitDepth + TDisplayModeUtils::NumDisplayModeBitsPerPixel( maskBitmap->DisplayMode() );
    	}
    
    if (maskBitmap)    
    	{
    	CleanupStack::PopAndDestroy(maskBitmap);   	
    	}
    	
    CleanupStack::PopAndDestroy(bitmap);        
    
    // Store provider in case it has changed
    if (aBitmapProvider)
    	{
    	entry->iBitmapProvider = aBitmapProvider;
    	}
   }
    
 
void CHuiTextureManager::UpdateFlags(const CFbsBitmap& aBitmap, const CFbsBitmap* aMaskBitmap, 
	                                 THuiTextureUploadFlags& aFlags)
	{
	// Update EHuiTextureFlagUseBitmapDirectly flag if needed
	
	if (aFlags & EHuiTextureFlagAllowDirectBitmapUsage)
		{
		TBool isDirectBitmapOk = !aBitmap.IsCompressedInRAM() && 
		                     (!aMaskBitmap || !aMaskBitmap->IsCompressedInRAM());
		// Check texture formats from the display
		// Note: if there is no display the check cannot be done
		if (isDirectBitmapOk)
			{
			isDirectBitmapOk = EFalse;		
			if (CHuiStatic::Env().DisplayCount() > 0)
				{
				RPointerArray<CHuiDisplay::CTextureBitmapFormat> formatArray;
    			TInt err = CHuiStatic::Env().PrimaryDisplay().GetPreferredTextureFormats(formatArray);
    			if (!err)
    				{
    				for (TInt i = 0; i < formatArray.Count(); i++)
    					{
   			   			if ((aBitmap.DisplayMode() == formatArray[i]->iImage) &&
   			   				((!aMaskBitmap) || (aMaskBitmap->DisplayMode() == formatArray[i]->iMask)))
   			   				{
   			   				isDirectBitmapOk = ETrue;
   			   				}
    					}
					}
				formatArray.ResetAndDestroy();
				}
			}
		// Update	
		if (!isDirectBitmapOk)
			{
   			aFlags = (THuiTextureUploadFlags)(aFlags & ~EHuiTextureFlagAllowDirectBitmapUsage);		
			}
		}
	}

    
RFs& CHuiTextureManager::FsSession() const
    {
    return iFS;
    }


EXPORT_C void CHuiTextureManager::PrependImagePath(TDes& aFileName) const
    {
    TFileName buf;

    //allow app to load textures from different drive with complete path
    TParse p1;
    p1.Set(aFileName,0,0);

    if (p1.DrivePresent())
        {
        return;
        }

    if(aFileName.Find(*iImagePath) == KErrNotFound)
        {
        buf = *iImagePath;
        buf.Append(aFileName);
        aFileName = buf;
        }
    }


EXPORT_C void CHuiTextureManager::UnloadTexture(const TDesC& aImageName, const TInt aFrameNumber)
    {
    TFileName fileName = aImageName;
    PrependImagePath(fileName);
    TInt index = CheckTexture(fileName, aFrameNumber);
    if (index >= 0)
        {
        HUI_DEBUG2(_L("CHuiTextureManager::UnloadTexture() - Unloading texture 0x%x (id: %i)."),
                   iTextures[index]->iTexture,
                   iTextures[index]->iId);
        DoUnload(index);
        return;
        }
    HUI_DEBUG1(_L("CHuiTextureManager::UnloadTexture() - Warning! Unable to unload \"%S\" - No such texture(s) found. Already unloaded?"), &fileName);
    }


EXPORT_C void CHuiTextureManager::UnloadTexture(TInt aId)
    {
    TInt index = CheckTexture(aId);
    if (index >= 0)
        {
        HUI_DEBUG2(_L("CHuiTextureManager::UnloadTexture() - Unloading texture 0x%x (id: %i)."),
                   iTextures[index]->iTexture,
                   iTextures[index]->iId);
        DoUnload(index);
        return;
        }
    HUI_DEBUG1(_L("CHuiTextureManager::UnloadTexture() - Warning! Unable to unload id %i. No texture with such id. Already unloaded?"), aId);
    }


EXPORT_C void CHuiTextureManager::DefineFileNameL(TInt aId,
                                                  const TDesC& aImageName)
    {
    if (aId==0)
        {
        HUI_DEBUG(_L("CHuiTextureManager::DefineFileNameL() - ERROR! Defining filename for texture id 0 is not allowed. Please use nonzero ids."));
        User::Leave(KErrArgument); // can't specify filename for "no id"
        }

    // Look for an existing entry for the id.
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        if(iTextures[i]->iId == aId)
            {
            // Just set the new image name.
            iTextures[i]->SetFileName(aImageName);
            return;
            }
        }

    // create a new texture, which automatically registers a new
    // texture entry in the iTextures list.
    CHuiTexture::NewL();
    // we assume that the newest entry was added last
    CTextureEntry* entry = iTextures[iTextures.Count()-1];
    entry->iId = aId;
    entry->SetFileName(aImageName);
    }


EXPORT_C TInt CHuiTextureManager::CheckTexture(const TDesC& aImageName, TInt aFrameNumber) const
    {
    if(aImageName.Length()==0)
        {
        // name empty, can't check
        return -1;
        }
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        CTextureEntry * te = iTextures[i];
        // compare against texture manager entry filename (iFileName)
        if((te->iFileName != NULL)
            && (aImageName.Compare(*(te->iFileName)) == 0)
            && te->iFrameNumber == aFrameNumber)
            {
            return i;
            }

        // compare against texture object imagefilename
        if( te->iTexture 
            && te->iTexture->ImageFileName().Compare(aImageName) == 0
            && te->iTexture->iFrameNumber == aFrameNumber)
            {
            return i;
            }
        }
    // not found
    return -1;
    }


EXPORT_C TInt CHuiTextureManager::CheckTexture(TInt aId) const
    {
    if(aId == 0)
        {
        // id not defined, can't search
        return -1;
        }
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        if(iTextures[i]->iId == aId)
            {
            return i;
            }
        }
    // not found:
    return -1;
    }


TBool CHuiTextureManager::IsInLoadQueue(const CHuiTexture * texture) const
    {

    if (texture == NULL)
        {
        return EFalse;
        }

    // textures in the load queue are not loaded yet,
    // the others are assumed to be loaded..
    for(TInt i = 0; i < iLoadQueue.Count(); ++i)
        {
        CTextureEntry* textureEntry = iLoadQueue[i].iLoading;
        if(textureEntry->iTexture == texture)
            {
            // Still in the queue.
            return ETrue;
            }
        }
    return EFalse;
    }

TBool CHuiTextureManager::IsInLoadQueue(TInt id) const
    {
    // textures in the load queue are not loaded yet,
    // the others are assumed to be loaded..
    for(TInt i = 0; i < iLoadQueue.Count(); ++i)
        {
        if(iLoadQueue[i].iLoading->iId == id)
            {
            // Still in the queue.
            return ETrue;
            }
        }
    return EFalse;
    }


void CHuiTextureManager::StartLoading()
    {
    // Start the texture load active object if we're not loading
    if(iState == EIdle)
        {
        HUI_DEBUG(_L("CHuiTextureManager::StartLoading() - Starting load queue."));

        // loop that finds next entry to load
        while (1)
            {            
            HUI_DEBUG(_L("CHuiTextureManager::StartLoading() - Find next to be loaded."));
            
            // try to schedule next image for loading..
            TRAPD(err, DoLoadNextL());
            // ok?
                
            if(err != KErrNone)
                {
                HUI_DEBUG(_L("CHuiTextureManager::StartLoading() - Found an error."));
                
                HUI_ASSERT(iLoadQueue.Count()>0);
                // remove the entry from the queue
                SLoadQueueEntry entry = PopLoadedQueueEntry();
                switch (err)
                    {
                    case KErrNoMemory:
                        {
                        HUI_DEBUG3( _L("CHuiTextureManager::StartLoading() - Unable to start loading \"%S\" (id %i): Out of memory. Trying to continue with %i items in the queue.."), &(entry.iLoading->iTexture->ImageFileName()), entry.iLoading->iId, iLoadQueue.Count());
                        break;
                        }
                    default:
                        {
                        HUI_DEBUG4( _L("CHuiTextureManager::StartLoading() - Unable to start loading \"%S\" (id %i): Leave error code %i. Trying to continue with %i items in the queue.."), &(entry.iLoading->iTexture->ImageFileName()), entry.iLoading->iId, err, iLoadQueue.Count());
                        break;
                        }
                    }
                // Notify observers about the image loading error
                NotifyTextureLoaded(*entry.iLoading->iTexture, entry.iLoading->iId, err);
                // Image loading has been completed.
                iState = EIdle;
                continue;
                }
            else
                {
                HUI_DEBUG(_L("CHuiTextureManager::StartLoading() - Found next texture to load."));
                if(iLoadQueue.Count() == 0 && iLoadQueueHadValidTexture)
                    {
                    // During the load loop that has passed we have succesfully started to load
                    // at least one texture. So the state has been changed to ELoading, thus
                    // it is now changed back to EIdle.
                    NotifyStateChange();
                    iLoadQueueHadValidTexture = EFalse;
                    }
                }
                
            // leave the loop if we had no trouble scheduling the next
            // image decode
            break;
            }

        HUI_DEBUG(_L("CHuiTextureManager::StartLoading() - Load queue started."));
        }                     
    }


CHuiTextureManager::SLoadQueueEntry CHuiTextureManager::PopLoadedQueueEntry()
    {
    SLoadQueueEntry entry = iLoadQueue[0];
    HUI_DEBUG2( _L("CHuiTextureManager::PopLoadedQueueEntry() - Cleaning up load queue entry for \"%S\" (id %i)."),
                &(entry.iLoading->iTexture->ImageFileName()), entry.iLoading->iId);
    iLoadQueue.Remove(0);
    // Delete the decoder.
    delete entry.iDecoder;
    entry.iDecoder = 0;
    return entry;
    }


void CHuiTextureManager::DoLoadNextL()
    {
	HUI_DEBUG(_L("CHuiTextureManager::DoLoadNextL() - Going into assert."));    
    __ASSERT_ALWAYS(iState == EIdle, THuiPanic::Panic(THuiPanic::EInternal));
    HUI_DEBUG(_L("CHuiTextureManager::DoLoadNextL() - Survived assert."));

    // Any loading tasks left?
    if(iLoadQueue.Count() == 0)
        {
        HUI_DEBUG(_L("CHuiTextureManager::DoLoadNextL() - No more items to load. Changing state to idle."));
        return; // nothing else to be loaded.
        }

    // Start timing
    CHuiStatic::Tic(2);

    // Manager is now busy.
    iState = ELoading;

    HUI_DEBUG(_L("CHuiTextureManager::DoLoadNextL() - Fetching next load queue entry."));

    // Fetch a load queue entry
    SLoadQueueEntry& entry = iLoadQueue[0];
    CHuiTexture* texture = entry.iLoading->iTexture;
    HUI_ASSERT(texture != NULL);
    HUI_DEBUG1(_L("CHuiTextureManager::DoLoadNext() - Starting to decode %S"), &texture->ImageFileName());

    // Create a new image decoder for loading the image.
    TFileName imageFileName = texture->ImageFileName(); // does not include image path
    PrependImagePath(imageFileName);
    
    delete entry.iDecoder;
    entry.iDecoder = NULL;
    
    TRAPD( err,  entry.iDecoder = CImageDecoder::FileNewL(FsSession(), imageFileName, CImageDecoder::EOptionAlwaysThread) );
    
    // check for errors..
    if(err != KErrNone)
        {
        // Try to cancel the decoding (if possible)
        if(entry.iDecoder)
            {
            entry.iDecoder->Cancel();
            }
        switch (err)
            {
            case KEComErrNoInterfaceIdentified:
                {
                HUI_DEBUG1( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". No plugin decoders were found."), &texture->ImageFileName() );
                User::Leave(err); // re-leave with the error
                }
            case KErrPathNotFound:
                {
                HUI_DEBUG1( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". Path not found."), &texture->ImageFileName() );
                User::Leave(err); // re-leave with the error
                }
            case KErrAccessDenied:
            case KErrPermissionDenied:
                {
                HUI_DEBUG1( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". Access denied."), &texture->ImageFileName() );
                User::Leave(err); // re-leave with the error
                }
            case KErrNotFound:
                {
                HUI_DEBUG1( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". Resource not found."), &texture->ImageFileName() );
                User::Leave(err); // re-leave with the error
                }
            case KErrUnderflow:
                {
                HUI_DEBUG1( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". Not enough data in file to identify which plugin decoder to use."), &texture->ImageFileName() );
                User::Leave(err); // re-leave with the error
                }
            case KErrNotSupported:
                {
                HUI_DEBUG1( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". Format not supported."), &texture->ImageFileName() );
                User::Leave(err); // re-leave with the error
                }
            default:
                {
                HUI_DEBUG2( _L("CHuiTextureManager::DoLoadNext() - CImageDecoder failed to decode \"%S\". Unknown error (code %i)."), &texture->ImageFileName(), err );
                User::Leave(err); // re-leave with the error
                }
            }
        }
     
    entry.iDecoder->SetDecoderThreadPriority(EPriorityAbsoluteBackgroundNormal);
        
    // There is a valid texture in the load queue that we now start to load.
    iLoadQueueHadValidTexture = ETrue;        
        
    // from decoder's frame info retrieve the framesize
    const TInt frameCount = entry.iDecoder->FrameCount();
    entry.iLoading->iFrameCount = frameCount;
    if (frameCount && entry.iLoading->iFrameNumber>frameCount)
        {
        entry.iLoading->iFrameNumber = frameCount-1;
        }

    CHuiTextureAnimationState* textureAnimationState = NULL;
    
    TInt groupId = 0;
    TBool foundGroup = FindGroupIdByTextureId( entry.iLoading->iId, groupId );
    
    if ( frameCount > 1 && NeedsAnimationState( *entry.iDecoder ) )
        {
        // It's animated texture which needs to be produced sequentially.
        if ( !foundGroup )
            {
            HUI_DEBUG(_L("CHuiTextureManager::DoLoadNextL() - Animation: Group not found."));

            if ( iAnimationWithoutGroup && 
                 iAnimationWithoutGroup->CheckIfCanProduce( 
                    imageFileName,
                    entry.iLoading->iFrameNumber,
                    frameCount ) )
                {
                // use iAnimationWithoutGroup
                }
            else
                {
                delete iAnimationWithoutGroup;
                iAnimationWithoutGroup = NULL;
                
                iAnimationWithoutGroup = 
                    CHuiTextureAnimationState::NewL( 
                        0, imageFileName, frameCount );
                }
            textureAnimationState = iAnimationWithoutGroup;
            }
        else
            {
            HUI_DEBUG(_L("CHuiTextureManager::DoLoadNextL() - Animation: Group found."));
            textureAnimationState = CreateAnimationStateL( 
                groupId, imageFileName, 
                entry.iLoading->iFrameNumber, frameCount );
            }
        }
    else
        {
        // No need to produce sequentially.
        if ( foundGroup )
            {
            RemoveAnimationState( FindAnimationState( groupId ) );
            }
        }

    if ( textureAnimationState != iAnimationWithoutGroup )
        {
        delete iAnimationWithoutGroup;
        iAnimationWithoutGroup = NULL;
        }

    const TInt frameToBeLoaded = 
        !textureAnimationState ? 
        entry.iLoading->iFrameNumber : 
        textureAnimationState->GetNextFrameNumber();
        
    TFrameInfo frameInfo(entry.iDecoder->FrameInfo(frameToBeLoaded));
    entry.iLoading->iFrameInterval = frameInfo.iDelay.Int64();

    if ( textureAnimationState )
        {
        textureAnimationState->OfferNextFrameInfo( frameInfo );
        }
    
    // check for alpha channel
    // Gifs don't have alpha but support tranparency so the alpha flag is ignored currently
    if(frameInfo.iFlags & TFrameInfo::ETransparencyPossible/* &&
            frameInfo.iFlags & TFrameInfo::EAlphaChannel*/)
        {
        entry.iHasAlpha = ETrue;
        }
    else
        {
        entry.iHasAlpha = EFalse;
        }

    // Get the image original size
    TRect bitmapSize = frameInfo.iFrameCoordsInPixels;
    TSize overallSize = 
        !textureAnimationState ? 
            bitmapSize.Size() : 
            textureAnimationState->OverallSize();
    
    entry.iOriginalSize = overallSize;

    HUI_DEBUG6(_L("CHuiTextureManager::DoLoadNext() - (sub)Image %S (id %i): w=%i, h=%i, bpp=%i, dm=%i"),
               &texture->ImageFileName(),
               entry.iLoading->iId,
               bitmapSize.Size().iWidth,
               bitmapSize.Size().iHeight,
               frameInfo.iBitsPerPixel,
               frameInfo.iFrameDisplayMode);

    HUI_DEBUG(_L("CHuiTextureManager::DoLoadNext() - Setting logical size of the bitmap."));

    // Try to assign the original size to the image
    texture->SetSize(overallSize);

    // target resolution for texture, initially equal to bitmap size
    TInt widthResolutionTarget = overallSize.iWidth;
    TInt heightResolutionTarget = overallSize.iHeight;

    TSize maxTexSize = entry.iLoading->iMaxTextureSize;

    // Assign new texture resolution target dimensions
    // if we have explicitly requested them
    if( (entry.iLoading->iFlags & EHuiTextureUploadFlagDoNotRetainResolution)
            && maxTexSize.iWidth > 0
            && maxTexSize.iHeight > 0)
        {
        HUI_DEBUG3(_L("CHuiTextureManager::DoLoadNext() - Custom texture size for image %S: w=%i, h=%i"),
                   &texture->ImageFileName(),
                   maxTexSize.iWidth,
                   maxTexSize.iHeight);

        // assign new target resolution for decoder-based scaling
        if(maxTexSize.iWidth < widthResolutionTarget)
            {
            widthResolutionTarget = maxTexSize.iWidth;
            }
        if(maxTexSize.iHeight < heightResolutionTarget)
            {
            heightResolutionTarget = maxTexSize.iHeight;
            }
        }

    // Set up segmentation so we know what kind of texture (and texture
    // size) we are dealing with
    texture->SetupSegmentsL(overallSize,
                           TSize(widthResolutionTarget, heightResolutionTarget),
                           entry.iLoading->iFlags);

    // we can do direct decoder-based scaling,
    // if only a single segment texture is used
    if( !textureAnimationState &&
        texture->SegmentCount() == 1 &&
        (texture->SegmentTextureSize(0).iWidth < bitmapSize.Size().iWidth ||
         texture->SegmentTextureSize(0).iHeight < bitmapSize.Size().iHeight))
        {

        // assign new texture size target dimensions from the calculated segment 0
        widthResolutionTarget = texture->SegmentTextureSize(0).iWidth;
        heightResolutionTarget = texture->SegmentTextureSize(0).iHeight;

        // we need to do some downscaling, but can we do arbitrary
        // scaling as well?
        if(frameInfo.iFlags & TFrameInfo::EFullyScaleable)
            {
            // .. yes, arbitrary scaling is possible
            // just assign the new size to the bitmap
            // so that it will be scaled accordingly during
            // conversion
            HUI_DEBUG3(_L("CHuiTextureManager::DoLoadNext() - Downscaling image %S to size %ix%i."),
                       &texture->ImageFileName(),
                       widthResolutionTarget,
                       heightResolutionTarget);
            }
        else
            {
            // all decoders should be able to do 1/2, 1/4, 1/8 DCT Scaling
            // calculate nearest half size for the decoder-downscaled bitmap
            // halve image width&height to size which is closest larger match
            // of the bitmap size
            TInt halvedWidth = bitmapSize.Size().iWidth;
            TInt halvedHeight = bitmapSize.Size().iHeight;
            TInt halveFactor = 1; // this limits the halving to 1/8 max
            while ( ((halvedWidth >> 1) > widthResolutionTarget) &&
                    ((halvedHeight >> 1) > heightResolutionTarget)
                    && (halveFactor << 1) <= 8)
                {
                halveFactor <<= 1;
                }
            halvedWidth = halvedWidth / halveFactor;
            halvedHeight = halvedHeight / halveFactor;
            // .. the bitmap will be downscaled further to the correct
            // dimensions by ImageLoadingCompleteL after the bitmap has been decoded
            HUI_DEBUG4(_L("CHuiTextureManager::DoLoadNext() - Halving image %S size to %ix%i (factor %i)."),
                       &texture->ImageFileName(),
                       halvedWidth,
                       halvedHeight, halveFactor);
            widthResolutionTarget = halvedWidth;
            heightResolutionTarget = halvedHeight;
            }
        }
    else
        {
        // either multi-segmented or no need for scaling..
        HUI_DEBUG(_L("CHuiTextureManager::DoLoadNext() - Decoder-based downscaling not required/possible."));
        }

    if ( textureAnimationState )
        {
        // we convert subframe completely
        widthResolutionTarget = bitmapSize.Size().iWidth;
        heightResolutionTarget = bitmapSize.Size().iHeight;
        }

    if (!(entry.iLoading->iFlags & EHuiTextureUploadFlagRetainColorDepth) && iBitmap->DisplayMode()!=EColor64K)
        {
        // (Re)Create the bitmap in EColor64K (16bit) mode to save memory
        iBitmap->Create(TSize(widthResolutionTarget, heightResolutionTarget), EColor64K);
        }
    else if ((entry.iLoading->iFlags & EHuiTextureUploadFlagRetainColorDepth) && iBitmap->DisplayMode()==EColor64K)
        {
        // (Re)Create the bitmap in EColor16MU (24bit) mode retain the color information
        iBitmap->Create(TSize(widthResolutionTarget, heightResolutionTarget), EColor16MU);
        }
    else
        {
        // no need to recreate the bitmap, but assign the new size!
        iBitmap->Resize(TSize(widthResolutionTarget, heightResolutionTarget));
        }

    if (iBitmap->DisplayMode()==EColor64K)
        {
        HUI_DEBUG(_L("CHuiTextureManager::DoLoadNext() - Decoding to 16bit image to conserve memory."));
        }
    // Decode ( and rescale ) to bitmap.
    if(entry.iHasAlpha)
        {
        // set the alpha channel bitmap to the same size than the color bitmap
        iMaskBitmap->Resize(TSize(widthResolutionTarget, heightResolutionTarget));
        entry.iDecoder->Convert(&iStatus, *iBitmap, *iMaskBitmap, frameToBeLoaded);
        }
    else
        {
        // Save some memory by making the unused alpha channel bitmap
        // very small.
        iMaskBitmap->Resize(TSize(4, 4));
        entry.iDecoder->Convert(&iStatus, *iBitmap, frameToBeLoaded);
        }

    // Wait for completion.
    SetActive();

    HUI_DEBUG1(_L("CHuiTextureManager::DoLoadNext() - Exiting, toc %.2f sec"), CHuiStatic::Toc(2));
    }

CHuiTextureAnimationState* CHuiTextureManager::CreateAnimationStateL( 
        TInt aGroupId, const TDesC& aImageFile, 
        TInt aFrameNumber, TInt aFrameCount )
    {
    CHuiTextureAnimationState* result = FindAnimationState( aGroupId );
    
    TBool recreate = !result || !aFrameNumber;
    if ( !recreate )
        {
        // result != NULL => check if result is animation state of some
        // previous frame
        recreate = 
            !result->CheckIfCanProduce( aImageFile, aFrameNumber, aFrameCount );
        }
        
    if ( recreate )
        {
        // First frame, let's just recreate animation state.
        RemoveAnimationState( result );
        result = NULL;
        
        result = CHuiTextureAnimationState::NewL( 
            aGroupId, aImageFile, aFrameCount );
        CleanupStack::PushL( result );
        iAnimations.AppendL( result );
        CleanupStack::Pop( result );
        }
        
    return result;
    }

CHuiTextureAnimationState* CHuiTextureManager::FindAnimationState( 
        TInt aGroupId )
    {
    CHuiTextureAnimationState* result = NULL;
    const TInt count = iAnimations.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CHuiTextureAnimationState* state = iAnimations[ i ];
        if ( state->OwnerTextureGroupId() == aGroupId )
            {
            result = state;
            break;
            }
        }
    return result;
    }

void CHuiTextureManager::RemoveAnimationState( 
        CHuiTextureAnimationState* aState )
    {
    if ( aState )
        {        
        TInt pos = iAnimations.Find( aState );
        if ( pos != KErrNotFound )
            {
            iAnimations.Remove( pos );
            }
        delete aState;
        }
    }

TBool CHuiTextureManager::FindGroupIdByTextureId( 
        TInt aTextureId, TInt& aGroupId ) const
    {
    TBool found = EFalse;
    const TInt count = iAnimatedTextureGroupItems.Count();
    TInt i = 0;
    while ( !found && ( i < count ) )
        {
        const TAnimatedTextureGroupItem& item = iAnimatedTextureGroupItems[ i ];
        if ( item.iTextureId == aTextureId )
            {
            found = ETrue;
            aGroupId = item.iGroupId;
            }
        i++;
        }
    return found;
    }

TBool CHuiTextureManager::NeedsAnimationState( const CImageDecoder& aDecoder )
    {
    // If this image contains any special image disposal methods, then
    // use animation state.
    
    const TInt frames = aDecoder.FrameCount();
    TBool result = EFalse;
    
    for ( TInt i = 0; !result && i < frames; ++i )
        {
        const TFrameInfo& info = aDecoder.FrameInfo( i );
        const TUint32 disposalFlags =
            info.iFlags & 
                ( TFrameInfo::ELeaveInPlace |
                  TFrameInfo::ERestoreToBackground |
                  TFrameInfo::ERestoreToPrevious );
                  
        result = result || disposalFlags;
        }
    
    return result;
    }

void CHuiTextureManager::DoUnload(TInt index)
    {
    HUI_ASSERT(index>=0 && index < iTextures.Count());
    CHuiTexture * t = iTextures[index]->iTexture;
    if (t == NULL)
        {
        HUI_DEBUG1(_L("CHuiTextureManager::UnloadTexture() - Not unloading a NULL texture at index %i."), index);
        return;
        }

    HUI_DEBUG3(_L("CHuiTextureManager::UnloadTexture() - Unloading texture \"%S\" id %i at index %i."), &t->ImageFileName(), iTextures[index]->iId, index);
    // Is this in the queue?
    if(IsInLoadQueue(t))
        {
        // Not loaded yet, can't unload. Mark it as unloaded, though,
        // so it will be discarded when the loading completes.
        for(TInt i = 0; i < iLoadQueue.Count(); ++i)
            {
            if(iLoadQueue[i].iLoading->iTexture == t)
                {
                HUI_DEBUG1(_L("CHuiTextureManager::UnloadTexture() - Marking \"%S\" as unloaded (still in the load queue). "), &t->ImageFileName());
                iLoadQueue[i].iUnloaded = ETrue;
                return;
                }
            }
        }

    // Just delete the contents of the texture
    t->Reset();
    HUI_ASSERT(!t->HasContent());
    }


EXPORT_C TInt CHuiTextureManager::RunError( TInt /*aError*/ )
    {
    // Should never be called, RunL is responsible for handling all error cases
    HUI_ASSERT(EFalse);
    return KErrNone;
    }


EXPORT_C void CHuiTextureManager::RunL()
    {
    if( iReleaseState == EReleased || 
        iReleaseState == EPartiallyReleased ||
        iReleaseState == EReleasing )
        {
        // Set the loading state to idle, but do not notify observers of this,
        // since we are released.
        // This has to be done so that unfinished loading can be restarted in
        // the RestoreL().
        iState = EIdle;        
        // Leave the texture to the load queue and load it again after restore.
        return;
        }
    
    TInt status = iStatus.Int();
    
    if ( status == KErrNone )
        {
        status = ImageLoadingContinue();
        if ( status > KErrNone )
            {
            HUI_DEBUG(_L("CHuiTextureManager::RunL() - Animation: Continue loading."));
            return;
            }
        }
    
    // remove the loaded entry from the queue
    SLoadQueueEntry entry = PopLoadedQueueEntry();

    // Image loading has been completed.
    iState = EIdle;

    // check status
    if( status == KErrNone )
        {
        // ok, we have a loaded image, but
        // we still need to do texture uploads
        // and possibly some image conversions
        TRAPD( err, ImageLoadingCompleteL(entry) );

        if(err != KErrNone)
            {
            delete iAnimationWithoutGroup;
            iAnimationWithoutGroup = NULL;
            
            switch (err)
                {
                case KErrNoMemory:
                    {
                    HUI_DEBUG3( _L("CHuiTextureManager::RunL() - ERROR! Failed to convert image to texture. Out of memory in ImageLoadingCompleteL(). Source image was  \"%S\" (id %i). Trying to continue with %i items in the queue.."),
                                &(entry.iLoading->iTexture->ImageFileName()), entry.iLoading->iId, iLoadQueue.Count() );
                    break;                                
                    }
                default:
                    {
                    HUI_DEBUG4( _L("CHuiTextureManager::RunL() - Error %i occurred during image upload/conversion to texture. Image filename was: \"%S\" (id %i). Trying to continue with %i items in the queue.."),
                                err, &(entry.iLoading->iTexture->ImageFileName()), entry.iLoading->iId, iLoadQueue.Count() );
                    break;
                    }
                }
            // Notify observers about the image upload error
            NotifyTextureLoaded(*entry.iLoading->iTexture, entry.iLoading->iId, err);
            }
        }
    else
        {
        HUI_DEBUG2( _L("CHuiTextureManager::RunL() - Image decoding error while decoding \"%S\" (id %i). Bypassing image.."), &(entry.iLoading->iTexture->ImageFileName()), entry.iLoading->iId);

        delete iAnimationWithoutGroup;
        iAnimationWithoutGroup = NULL;

        // notify sb of failed image load!
        NotifyTextureLoaded(*entry.iLoading->iTexture, entry.iLoading->iId, status);
        }

    // Continue loading entries.
    StartLoading();

    if(iLoadQueue.Count() == 0)
        {
        HUI_DEBUG(_L("CHuiTextureManager::RunL() - No more images to load, exiting.."));
        }
    }


EXPORT_C void CHuiTextureManager::DoCancel()
    {
    if(iLoadQueue.Count() > 0)
        {
        iLoadQueue[0].iDecoder->Cancel();
        }
        
    iState = EIdle;
    }

TInt CHuiTextureManager::ImageLoadingContinue()
    {
    TInt status = KErrNone;
    
    const SLoadQueueEntry& entry = iLoadQueue[0];
    
    TInt groupId = 0;
    CHuiTextureAnimationState* state = NULL;
    if ( iAnimationWithoutGroup )
        {
        state = iAnimationWithoutGroup;
        }
    else if ( FindGroupIdByTextureId( entry.iLoading->iId, groupId ) )
        {
        state = FindAnimationState( groupId );
        }
    else
        {
        // state = NULL;
        }
        
    if ( state && 
         ( state->GetNextFrameNumber() != entry.iLoading->iFrameNumber ) )
        {
        // Someone has asked animated frames not in sequence - 
        // very inefficient!
        HUI_DEBUG(_L("CHuiTextureManager::ImageLoadingContinue: Frames not requested in sequence."));
        TRAP( status, 
            state->ProceedWithoutNextFrameL( 
                iBitmap, 
                entry.iHasAlpha ? iMaskBitmap : NULL ) );

        if ( status == KErrNone )
            {
            // Image loading has been completed.
            iState = EIdle;            
            StartLoading();
            status = 1; // indicate that started loading.
            }
        }
    
    return status;
    }

void CHuiTextureManager::ImageLoadingCompleteL(SLoadQueueEntry& aEntry)
    {
    CHuiTexture* tex = aEntry.iLoading->iTexture;
    HUI_ASSERT(tex);
    tex->iFrameNumber = aEntry.iLoading->iFrameNumber;
    tex->iFrameCount = aEntry.iLoading->iFrameCount;
    tex->iFrameInterval = aEntry.iLoading->iFrameInterval;

    if(iReleaseState != ENormal)
        {
        HUI_DEBUG1(_L("CHuiTextureManager::ImageLoadingCompleteL() - Unable to upload bitmap data for \"%S\". Texture manager has been released."), &tex->ImageFileName());
        return;
        }
       
    if ( tex->iFrameCount > 1 )
        {
        ProcessAnimatedFrameL( aEntry );
        }
    TBool hasAlpha = aEntry.iHasAlpha;
    
    delete iAnimationWithoutGroup;
    iAnimationWithoutGroup = NULL;

    TSize* maxTexSize = &aEntry.iLoading->iMaxTextureSize;    
    if(((maxTexSize->iWidth != 0) && (maxTexSize->iHeight != 0)) && iBitmap->SizeInPixels() != aEntry.iLoading->iMaxTextureSize )
        {
        // The decoder failed to constrain the texture dimensions properly, due to its internal limitations.
        // So we need to scale the texture(s) down further to the correct size.      
        
        CFbsBitmap* dest = new (ELeave) CFbsBitmap();
        CleanupStack::PushL( dest );
        User::LeaveIfError( dest->Create(*maxTexSize, iBitmap->DisplayMode()) );
        HuiUtil::ScaleFbsBitmapL(*iBitmap, *dest);
        CleanupStack::Pop( dest );
        delete iBitmap;
        iBitmap = dest;

        if( hasAlpha )
            {
            // Scale the alpha as well.
            dest = new (ELeave) CFbsBitmap();   
            CleanupStack::PushL( dest );
            User::LeaveIfError(
                dest->Create(*maxTexSize, iMaskBitmap->DisplayMode()) );     
            HuiUtil::ScaleFbsBitmapL(*iMaskBitmap, *dest);
            CleanupStack::Pop( dest );
            delete iMaskBitmap;
            iMaskBitmap = dest;
            }                      
        }        

    HUI_DEBUG3(_L("CHuiTextureManager::ImageLoadingCompleteL() - Loading complete for \"%S\". Uploading %ix%i FBS bitmap to texture.."),
               &tex->ImageFileName(),
               iBitmap->SizeInPixels().iWidth,
               iBitmap->SizeInPixels().iHeight);

    // Upload without compression.
    tex->UploadL(*iBitmap,
                (hasAlpha? iMaskBitmap: NULL),
                 aEntry.iLoading->iFlags);

    // Store bits per pixel info to calculate texture memory usage
    aEntry.iLoading->iBitDepth = TDisplayModeUtils::NumDisplayModeBitsPerPixel( iBitmap->DisplayMode() );
    if( iMaskBitmap )
        {
        aEntry.iLoading->iBitDepth = aEntry.iLoading->iBitDepth + 
        TDisplayModeUtils::NumDisplayModeBitsPerPixel( iMaskBitmap->DisplayMode() );
        }
    
    // Now we're setting up the real texture, no more placeholder needed
    tex->SetPlaceholder(NULL);

    HUI_DEBUG2(_L("CHuiTextureManager::ImageLoadingCompleteL() - ... Upload done. Setting original size of %ix%i pixels."),
               aEntry.iOriginalSize.iWidth,
               aEntry.iOriginalSize.iHeight);

    // (Re)set the original size of the image as the logical size
    tex->SetSize(aEntry.iOriginalSize);

    // Notify the observers of loaded texture
    NotifyTextureLoaded(*tex, aEntry.iLoading->iId, KErrNone);

    // Unload, if the unloading was already requested while loading!
    if(aEntry.iUnloaded)
        {
        UnloadTexture(tex->ImageFileName(),tex->iFrameNumber);
        }

    HUI_DEBUG(_L("CHuiTextureManager::ImageLoadingCompleteL() - Done. "));
    }

void CHuiTextureManager::ProcessAnimatedFrameL(SLoadQueueEntry& aEntry)
    {
    // Animated file - produce next frame.
    CFbsBitmap* frameBitmap = NULL;
    CFbsBitmap* frameMask = NULL;

    // ImageFileName() does not include image path
    CHuiTextureAnimationState* state = NULL;
    TInt groupId = 0;
    if ( iAnimationWithoutGroup )
        {
        state = iAnimationWithoutGroup;
        }
    else if ( FindGroupIdByTextureId( aEntry.iLoading->iId, groupId ) )
        {
        state = FindAnimationState( groupId );
        }

    if ( !state )
        {
        return;
        }

    HUI_DEBUG1(_L("CHuiTextureManager::ProcessAnimatedFrameL: Producing next frame (%d)."), 
        state->GetNextFrameNumber() );

    state->ProduceNextFrameL( 
        frameBitmap, frameMask, 
        iBitmap, aEntry.iHasAlpha ? iMaskBitmap : NULL );

    HUI_DEBUG2(_L("CHuiTextureManager::ProcessAnimatedFrameL: Next frame produced (%x, %x)."),
        frameBitmap, frameMask );
    
    aEntry.iHasAlpha = ( frameMask != NULL );
    if ( frameBitmap != iBitmap )
        {
        delete iBitmap;
        iBitmap = frameBitmap;
        }
        
    if ( frameMask && (frameMask != iMaskBitmap) )
        {
        delete iMaskBitmap;
        iMaskBitmap = frameMask;
        }        
    }

EXPORT_C TInt CHuiTextureManager::LoadQueueCount() const
    {
    return iLoadQueue.Count();
    }


EXPORT_C void CHuiTextureManager::AdvanceTime(TReal32 aElapsedTime) const
    {
    for(TInt i = 0; i < iAnimatedTextures.Count(); ++i)
        {
        iAnimatedTextures[i].AdvanceTime(aElapsedTime);
        }
    NotifyTextureAutoSizeObservers();
    // Write total memory consumed on rendering textures as an info message,
    // if feature is enabled from central repository Rnd flag
    if ( iEnableTexMemCalculation )
    	{
    	TextureMemUsage(); 
    	}    
    }


EXPORT_C void CHuiTextureManager::SetProcessor(CHuiTextureProcessor* aProcessor)
    {
    // Since we have ownership of the processor, delete the previous one if
    // there is one.
    if(iProcessor)
        {
        delete iProcessor;
        iProcessor = NULL;
        }

    iProcessor = aProcessor;
    }


EXPORT_C CHuiTextureProcessor& CHuiTextureManager::Processor()
    {
    if(!iProcessor)
        {
        THuiPanic::Panic(THuiPanic::ETextureManagerNoProcessor);
        }
    return *iProcessor;
    }


void CHuiTextureManager::CancelLoadingOfTexture(CHuiTexture& aTexture)
    {
    for(TInt i = 0; i < iLoadQueue.Count(); ++i)
        {
        if(iLoadQueue[i].iLoading->iTexture == &aTexture)
            {
            // Texture found from the load queue.
            if(i == 0 && iState == ELoading)
                {
                // Texture is currently loading
                Cancel();
                PopLoadedQueueEntry();
                StartLoading();                
                }
            else
                {
                // Delete the decoder and remove entry.
                delete iLoadQueue[i].iDecoder;
                iLoadQueue.Remove(i);
                }
            break;
            }
        }
    }


void CHuiTextureManager::NotifyStateChange() const
    {
    for(TInt index = 0; index < iStateObservers.Count(); ++index)
        {
        iStateObservers[index].TextureManagerStateChanged(*this);
        }
    }


EXPORT_C void CHuiTextureManager::NotifyTextureLoaded(CHuiTexture& aTexture,
                             TInt aTextureId,
                             TInt aErrorCode) const
    {
    aTexture.SetTextureChanged(ETrue);
    for(TInt index = 0; index < iLoadObservers.Count(); ++index)
        {
        iLoadObservers[index].TextureLoadingCompleted(aTexture, aTextureId, aErrorCode);
        }
    }

EXPORT_C CHuiTextureManager::CTextureEntry * CHuiTextureManager::TextureEntry(TInt aIndex)
    {
    // check limits
    if ( aIndex < iTextures.Count() )
        {
        return iTextures[aIndex];
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C TInt CHuiTextureManager::TextureEntryCount() const
    {
    return iTextures.Count();
    }

EXPORT_C TBool CHuiTextureManager::Release()
    {
    HUI_DEBUG(_L("CHuiTextureManager::Release() - Called."));

    HUI_ASSERT(iReleaseState == ENormal);
    iReleaseState = EReleasing;
    
    TBool allReleased = ETrue; // by default, all texures are released.

    // Release all textures. Use an order inverse to creation order.
    for(TInt i = iTextures.Count() - 1; i >= 0; --i)
        {
        CHuiTexture * tex = iTextures[i]->iTexture;
        if (tex)
            {
            // This is called when application goes into the backgrounf
            // If release priority is set to 0, the texture should not be released.
            if ( tex->Priority() == 0 )
                {
                HUI_DEBUG5(_L("CHuiTextureManager::Release() - Not releasing priority texture 0x%x (\"%S\", id: %i) [%i/%i]"),
                          tex, &(tex->ImageFileName()), iTextures[i]->iId, i+1, iTextures.Count());
                allReleased = EFalse;
                }
            else
                {     
                HUI_DEBUG5(_L("CHuiTextureManager::Release() - Releasing texture 0x%x (\"%S\", id: %i) [%i/%i]"),
                          tex, &(tex->ImageFileName()), iTextures[i]->iId, i+1, iTextures.Count());
                tex->Release();
                }
            }
        else
            {
            HUI_DEBUG4(_L("CHuiTextureManager::Release() - Not releasing unloaded texture 0x%x, id: %i) [%i/%i]"),
                          tex, iTextures[i]->iId, i+1, iTextures.Count());                
            }
        }

    HUI_DEBUG(_L("CHuiTextureManager::Release() - Release processor."));

    // Release texture processor.
    if(iProcessor)
        {
        iProcessor->Release();
        }

    HUI_ASSERT(iReleaseState == EReleasing);
    if ( allReleased )
        {
        HUI_DEBUG(_L("CHuiTextureManager::Release() - Exiting (EReleased)."));
        iReleaseState = EReleased;
        }
    else
        {
        HUI_DEBUG(_L("CHuiTextureManager::Release() - Exiting (EPartiallyReleased)."));
        iReleaseState = EPartiallyReleased;
        }
    return allReleased;
    }


EXPORT_C void CHuiTextureManager::RestoreL()
    {
    HUI_DEBUG(_L("CHuiTextureManager::RestoreL() - Called."));

    HUI_ASSERT(iReleaseState == EReleased || iReleaseState == EPartiallyReleased );
    iReleaseState = ERestoring;

    // Restore texture processor.
    if(iProcessor)
        {
        iProcessor->RestoreL();
        }

    // Restore all textures. Use same order as in which the textures were created.
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        CHuiTexture * tex = iTextures[i]->iTexture;
        if (tex)
            {
            if ( tex->HasContent() )
                {
                HUI_DEBUG5(_L("CHuiTextureManager::RestoreL() - Alredy has content texture 0x%x (\"%S\", id: %i) [%i/%i]"),
                          tex, &(tex->ImageFileName()), iTextures[i]->iId, i+1, iTextures.Count());
                }
            else
                {    
                HUI_DEBUG5(_L("CHuiTextureManager::RestoreL() - Restoring texture 0x%x (\"%S\", id: %i) [%i/%i]"),
                          tex, &(tex->ImageFileName()), iTextures[i]->iId, i+1, iTextures.Count());
                // Restore the texture.
                TRAP_IGNORE( tex->RestoreL() );
                }
            }
        else
            {
            HUI_DEBUG4(_L("CHuiTextureManager::RestoreL() - Not restoring unloaded texture 0x%x, id: %i) [%i/%i]"),
                          tex, iTextures[i]->iId, i+1, iTextures.Count());                
            }
        }

    // Restart loading of textures that still might be in the load queue.
    StartLoading();

    HUI_ASSERT(iReleaseState == ERestoring);
    iReleaseState = ENormal;
    }

EXPORT_C void CHuiTextureManager::NotifySkinChangedL()
    {
    for(TInt i = iTextures.Count()-1; i >= 0; i--)
        {
        CHuiTexture * tex = iTextures[i]->iTexture;
        if (tex &&
            tex->HasContent() &&
            tex->IsSkinContent())
            {
            tex->Release();
            tex->RestoreL();    
            }
        }            
    }

EXPORT_C TInt CHuiTextureManager::AddAnimatedTextureGroupL( 
        const RArray<TInt>& aTextureIds )
    {
    const TInt count = aTextureIds.Count();
    
    // Zero ids are not allowed here
    for ( TInt j = 0; j < count; j++ )
        {
        if ( !aTextureIds[ j ] ) 
            {
            User::Leave( KErrNotSupported );
            }
        }
        
    TInt result = 0;
    if ( count )
        {
        result = aTextureIds[ 0 ]; 
    
        // Reserve space so next loop cannot fail.
        iAnimatedTextureGroupItems.ReserveL( 
            iAnimatedTextureGroupItems.Count() + aTextureIds.Count() );
    
        TAnimatedTextureGroupItem item;
        item.iGroupId = result;
        
        for ( TInt i = 0; i < count; i++ )
            {
            item.iTextureId = aTextureIds[ i ];
            
            (void)iAnimatedTextureGroupItems.
                Append( item ); // ought not to fail due to Reserve.
            }
        }
        
    return result;
    }
    
EXPORT_C void CHuiTextureManager::RemoveAnimatedTextureGroup( TInt aGroupId )
    {
    // Remove animated texture group items.
    const TInt count = iAnimatedTextureGroupItems.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        if ( iAnimatedTextureGroupItems[ i ].iGroupId == aGroupId )
            {
            iAnimatedTextureGroupItems.Remove( i );
            }
        }
    
    RemoveAnimationState( FindAnimationState( aGroupId ) );
    }

// CHuiTextureManager::CTextureEntry

CHuiTextureManager::CTextureEntry::CTextureEntry(
    TInt aId,
    const TFileName & aFileName,
    const TSize& aMaxTextureSize,
    THuiTextureUploadFlags aFlags,
    TInt aFrameNumber )
    : iId(aId), iFileName(NULL),
      iTexture(NULL),
      iMaxTextureSize(aMaxTextureSize), iFlags(aFlags),
      iBitmapProvider(NULL),
      iFrameNumber(aFrameNumber),
      iFrameCount(0),
      iFrameInterval(0)
    {
    SetFileName(aFileName);
    }
    
CHuiTextureManager::CTextureEntry::CTextureEntry(TInt aId, CHuiTexture * aTexture)
                      : iId(aId),
                        iFileName(NULL),
                        iTexture(aTexture),
                        iMaxTextureSize(TSize(0,0)),
                        iFlags(EHuiTextureUploadFlagDefault),
                        iBitmapProvider(NULL),
                        iFrameNumber(0),
                        iFrameCount(0),
                        iFrameInterval(0)
    {
    }
            
CHuiTextureManager::CTextureEntry::CTextureEntry(const CTextureEntry &aSrc)
                      : iId(aSrc.iId),
                        iFileName(NULL),
                        iTexture(aSrc.iTexture),
                        iMaxTextureSize(aSrc.iMaxTextureSize),
                        iFlags(aSrc.iFlags),
                        iBitmapProvider(NULL),
                        iFrameNumber(aSrc.iFrameNumber),
                        iFrameCount(0),
                        iFrameInterval(0)
    {
    SetFileName(*aSrc.iFileName);
    }
            
CHuiTextureManager::CTextureEntry & CHuiTextureManager::CTextureEntry::operator=(const CTextureEntry &aSrc)
    {
    if ( this == &aSrc )
        {
        return *this;
        }
            
    iId = aSrc.iId;
    if (aSrc.iFileName != NULL)
        {
        SetFileName(*aSrc.iFileName);
        }
    iMaxTextureSize = aSrc.iMaxTextureSize;
    iFlags = aSrc.iFlags;
    iBitmapProvider = aSrc.iBitmapProvider;
    iFrameNumber = aSrc.iFrameNumber;
    iFrameCount = aSrc.iFrameCount;
    iFrameInterval = aSrc.iFrameInterval;
    return *this;
    }
            
EXPORT_C CHuiTextureManager::CTextureEntry::~CTextureEntry()
    {
    delete iFileName; 
    iFileName = NULL;
    // CHuiTextureManager deletes the iTexture
    }
    
void CHuiTextureManager::CTextureEntry::SetFileName(const TFileName & aFileName)
    {
    delete iFileName;
    iFileName = NULL;
    if (aFileName.Length()>0)
        {
        // this might fail and therefore the iFileName is left as NULL.
        iFileName = aFileName.Alloc();
        }
    }

void CHuiTextureManager::CTextureEntry::TextureContentUploaded(CHuiTexture& /*aTexture*/)
    {
    }
void CHuiTextureManager::CTextureEntry::TextureContentReleased(CHuiTexture& /*aTexture*/)
    {
    }
void CHuiTextureManager::CTextureEntry::RestoreTextureContentL(CHuiTexture& aTexture)
    {
    HUI_DEBUG(_L("CHuiTextureManager::CTextureEntry::RestoreTextureContentL() - Called for entry 0x%x (id: %i)."));
    if (iBitmapProvider != NULL)
        {
        HUI_DEBUG3(_L("CHuiTextureManager::CTextureEntry::RestoreTextureContentL() - Using bitmap provider 0x%x to recreate content for texture 0x%x (id: %i).."), iBitmapProvider, &aTexture, iId);
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* maskBitmap = NULL;

        // call the bitmapProvider method to load the bitmaps
        iBitmapProvider->ProvideBitmapL(iId, bitmap, maskBitmap);

        if (maskBitmap)
            {
            CleanupStack::PushL(maskBitmap);
            }
        
        if (!bitmap)
            {
            User::Leave(KErrArgument);
            }
        else
            {
            CleanupStack::PushL(bitmap);
            }
        
        // recreate texture content..

        // Let the texture know its size as soon as possible.
        aTexture.SetSize(bitmap->SizeInPixels());

        // Enable texture shadow.
        aTexture.EnableShadow((iFlags & EHuiTextureUploadFlagGenerateShadow) != 0);

        // Upload!    
        TSize bitmapSize = bitmap->SizeInPixels();
    	if (bitmapSize.iWidth != 0 && bitmapSize.iHeight != 0)
    	    {
    		if (iFlags & EHuiTextureFlagAllowDirectBitmapUsage) 
    			{
       			aTexture.UploadDirectL(*bitmap, maskBitmap, iFlags);		
    			}
    		else 
    			{
            	aTexture.UploadL(*bitmap, maskBitmap, iFlags);
    			}
    	    }
        // Clean up owned bitmaps
        CleanupStack::PopAndDestroy(bitmap);        
        if (maskBitmap)
            {
            CleanupStack::PopAndDestroy(maskBitmap);        
            }       
        }
    // check for file-based restore
    else if (iFileName != NULL)
        {
        HUI_DEBUG2(_L("CHuiTextureManager::CTextureEntry::RestoreTextureContentL() - Reloading texture 0x%x from \"%S\".."), &aTexture, iFileName);
        CHuiStatic::Env().TextureManager().LoadTextureL(*iFileName, iMaxTextureSize, iFlags, iId);
        return;
        }
    else
        {
        // for PC lint
        }
    }

void CHuiTextureManager::NotifyTextureAutoSizeObservers() const
    {
    if (!iTextureAutoSizingEnabled)
        {
        return;
        }
    
    TBool somethingWasReported = EFalse;
    TBool anyAutoSizedTexturesExist = EFalse;
    
    for(TInt index = iTextures.Count() - 1; index >= 0; --index)
        {
        CTextureEntry * entry = iTextures[index];
        if (entry->iTexture)
            {
            if (entry->iTexture->IsAutoSizeCalculationEnabled())
                {
                anyAutoSizedTexturesExist = ETrue;
                
                // Calculate preferred size from the previous frame data
                THuiRealSize preferredSize = entry->iTexture->CalculateAutoSize();
                
                // Notify observers if preferred size has changed
                if (entry->iTexture->CalculatedAutoSizeChanged())
                    {
                    HUI_DEBUG3(_L("CHuiTextureManager::NotifyTexturePreferredSizeObservers - To be notified id: %i, width: %f , height %f"), entry->iId, preferredSize.iWidth, preferredSize.iHeight);                    

                    // Notify observers. For now ignore return values.
                    for(TInt index = 0; index < iTextureAutoSizeObservers.Count(); ++index)
                        {
                        iTextureAutoSizeObservers[index].PreferredSizeChanged(*entry->iTexture, entry->iId, preferredSize);
                        somethingWasReported = ETrue;
                        }            
                    }
                
                // Restart preferred size calculations for next frame   
                entry->iTexture->ResetAutoSizeCalculation();        
                }
            }
        }
    
    if (somethingWasReported)
        {
        for(TInt index = 0; index < iTextureAutoSizeObservers.Count(); ++index)
            {
            iTextureAutoSizeObservers[index].PreferredSizeReportCompleted();                
            }                        
        }

    // Keep checking autosized textures only if those exist. 
    // New textures will enable flag when needed.
    iTextureAutoSizingEnabled = anyAutoSizedTexturesExist;
    }

EXPORT_C void CHuiTextureManager::ClearChangedTextures()
    {
    if (iHasChangedTextures)
        {
        for(TInt index = iTextures.Count() - 1; index >= 0; --index)
            {
            CTextureEntry * entry = iTextures[index];
            if (entry->iTexture)
                {
                entry->iTexture->TextureClearChanged();
                }
            }
        iHasChangedTextures = EFalse;        
        }
    }
    
EXPORT_C void CHuiTextureManager::SetHasChangedTextures()
    {
    iHasChangedTextures = ETrue;    
    }

/*
 * Enables/Disables memory usage calculation.
 */
EXPORT_C void CHuiTextureManager::EnableTexMemoryCalculation( TBool aEnableTeMemCal )
	{
	iEnableTexMemCalculation = aEnableTeMemCal;
	}

/*
 * Calculates memory used by texturemanager to store all the textures.
 * Texture memory is calculated as ( Texture area ) * ( bits per pixel )
 * 
 * This routine prints info message stating toal no of textures and memory consuption by them.
 */
void CHuiTextureManager::TextureMemUsage() const
	{
	TReal32 totalMemUsage = 0;
	TReal32 avgBitDepth = 0;
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        if( iTextures[i]->iTexture )
            {            
            TSize size = iTextures[i]->iTexture->Size();         
            totalMemUsage = totalMemUsage + size.iWidth * size.iHeight * iTextures[i]->iBitDepth;   
            avgBitDepth = avgBitDepth + iTextures[i]->iBitDepth;
            }        
        }
    
    avgBitDepth = avgBitDepth/iTextures.Count();
    totalMemUsage = totalMemUsage/8000000;
    TBuf<300> buf;
    buf.FillZ();
    buf.AppendNum( totalMemUsage, TRealFormat( 8, 4 ) );
    buf.Append(_L( "MB;By:" ) );
    buf.AppendNum( iTextures.Count() );
    buf.Append(_L( ";BD:" ) );
    buf.AppendNum( avgBitDepth );
    User::InfoPrint( buf );      
	}
TInt CHuiTextureManager::EstimatedTextureMemUsage(TInt aAverageBitsPerPixel) const
    {
    TReal32 averageBytesPerPixel =  TReal32(aAverageBitsPerPixel)/8.f;
	TReal32 totalMemUsage = 0;
    TInt pixels = 0;
    for(TInt i = 0; i < iTextures.Count(); ++i)
        {
        // TODO: Exclude NVG textures !
        if( iTextures[i]->iTexture /*&& 
            iTextures[i]->iTexture->HasContent()*/)
            {            
            TSize size = iTextures[i]->iTexture->Size();         
            pixels += size.iWidth * size.iHeight;                        
            }        
        }
        
    totalMemUsage = TReal32(pixels) * averageBytesPerPixel;        
    return totalMemUsage;
    }



EXPORT_C void CHuiTextureManager::TextureManagerExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }

void CHuiTextureManager::EnableAutoSizeCalculation(TBool aEnable)
    {
    iTextureAutoSizingEnabled = aEnable;
    }

