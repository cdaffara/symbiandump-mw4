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
* Description:   ?Description
*
*/



#include "HuiVg10TextureManager.h"  // Class definition
#include "HuiVg10TextureProcessor.h"


CHuiVg10TextureManager* CHuiVg10TextureManager::NewL(CHuiEnv& aEnv)
    {
    CHuiVg10TextureManager* self = CHuiVg10TextureManager::NewLC(aEnv);
    CleanupStack::Pop(self);
    return self;
    }
    

CHuiVg10TextureManager* CHuiVg10TextureManager::NewLC(CHuiEnv& aEnv)
    {
    CHuiVg10TextureManager* self = new (ELeave) CHuiVg10TextureManager(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiVg10TextureManager::CHuiVg10TextureManager(CHuiEnv& aEnv)
        : CHuiTextureManager(aEnv)
    {
    }


void CHuiVg10TextureManager::ConstructL()
    {
    CHuiTextureManager::BaseConstructL();
    
    // Create a texture processor.
    CHuiVg10TextureProcessor* proc = CHuiVg10TextureProcessor::NewL(Env());
    SetProcessor(proc); // ownership to base class
    
    iBitmap16MA = new (ELeave) CFbsBitmap;
    iBitmap16MA->Create(TSize(1,1), EColor16MA);
    iBitmap16MU = new (ELeave) CFbsBitmap;
    iBitmap16MU->Create(TSize(1,1), EColor16MU);
    iBitmap64K = new (ELeave) CFbsBitmap;
    iBitmap64K->Create(TSize(1,1), EColor64K);
    iBitmap256Gray = new (ELeave) CFbsBitmap;
    iBitmap256Gray->Create(TSize(1,1), EGray256);
    
    iBitmapDevice16MA = CFbsBitmapDevice::NewL(iBitmap16MA);
    iBitmapDevice16MU = CFbsBitmapDevice::NewL(iBitmap16MU);
    iBitmapDevice64K = CFbsBitmapDevice::NewL(iBitmap64K);
    iBitmapDevice256Gray = CFbsBitmapDevice::NewL(iBitmap256Gray);
    }


CHuiVg10TextureManager::~CHuiVg10TextureManager()
    {
    delete iBitmap16MA;
    delete iBitmap16MU;
    delete iBitmap64K;
    delete iBitmap256Gray;
    delete iBitmapDevice16MA;
    delete iBitmapDevice16MU;
    delete iBitmapDevice64K;
    delete iBitmapDevice256Gray;
    delete iBitGc16MA;
    delete iBitGc16MU;
    delete iBitGc64K;
    delete iBitGc256Gray;
    }

CFbsBitmap& CHuiVg10TextureManager::ConvertBitmapL(const CFbsBitmap& aBitmap, TDisplayMode aDisplayMode, TBitmapCopyMode aCopyMode, TSize aNewSize)
    {
    TSize newSize = aBitmap.SizeInPixels();
    if (aNewSize != TSize(0,0))
        {
        newSize = aNewSize;
        }
    
    // Select the correct Colormode bitmap & device
    CFbsBitmap* bitmap = NULL;
    CFbsBitmapDevice* bitmapDevice = NULL;
    CFbsBitGc* bitGc = NULL;
    // This is needed for storing the possibly recreated bitGc to a member variable
    CFbsBitGc** bitGcMemberPtr = NULL;
    switch(aDisplayMode)
        {
        case EColor16MA:
            bitmap = iBitmap16MA;
            bitmapDevice = iBitmapDevice16MA;
            bitGc = iBitGc16MA;
            bitGcMemberPtr = &iBitGc16MA;
            break;
        case EColor16MU:
            bitmap = iBitmap16MU;
            bitmapDevice = iBitmapDevice16MU;
            bitGc = iBitGc16MU;
            bitGcMemberPtr = &iBitGc16MU;
            break;
        case EColor64K:
            bitmap = iBitmap64K;
            bitmapDevice = iBitmapDevice64K;
            bitGc = iBitGc64K;
            bitGcMemberPtr = &iBitGc64K;
            break;
        case EGray256:
            bitmap = iBitmap256Gray;
            bitmapDevice = iBitmapDevice256Gray;
            bitGc = iBitGc256Gray;
            bitGcMemberPtr = &iBitGc256Gray;
            break;
        default:
            // Not supported! Implement more cases if needed, otherwise this'll leave
            User::Leave(KErrNotSupported);
            break;
        }
    
    // No conversion required?
    if ( aCopyMode == EAllowDuplication && aBitmap.DisplayMode() == aDisplayMode && 
         !aBitmap.IsCompressedInRAM() && newSize == aBitmap.SizeInPixels() )
        {
        // If the bitmap's color mode was already correct & sizes matched,
        // and bmp isn't rombitmap, no conversion was required and we can return
        // the same bitmap that was given as a parameter
        CFbsBitmap& bmp = const_cast<CFbsBitmap&>(aBitmap);
        return bmp;
        }
    
    // If needed, Resize the bitmap & bitmapDevice to correct dimensions,
    // instead of recreating them always
    if (newSize != bitmap->SizeInPixels() || !bitGc)
        {
        // Try to resize the bitmap & bmpDevice, leave, if e.g. out of memory
        User::LeaveIfError(bitmap->Resize(newSize));

        delete bitGc;
		bitGc = 0;
        *bitGcMemberPtr = 0;

        User::LeaveIfError(bitmapDevice->Resize(newSize));
        
        // The BitGc has to be created every time separately
        User::LeaveIfError(bitmapDevice->CreateContext(bitGc));
        // Store the newly created context to a correct member variable
        *bitGcMemberPtr = bitGc;
        }
    
    if (aDisplayMode == EColor16MA && bitGc)
        {
        bitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        }
    
    if (bitGc)
		{
		bitGc->BitBlt(TPoint(0, 0), &aBitmap);
		}
    return *bitmap;
    }

// End of file

