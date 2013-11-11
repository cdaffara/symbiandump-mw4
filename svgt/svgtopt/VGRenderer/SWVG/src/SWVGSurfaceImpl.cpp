/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSWVGSurfaceImpl source file
 *
*/

#include "SWVGSurfaceImpl.h"
    
#include <platform/vg/vgcontext_symbian.h>


CSWVGSurfaceImpl::CSWVGSurfaceImpl()
    {
    }

CSWVGSurfaceImpl::~CSWVGSurfaceImpl()
    {
    }

CSWVGSurfaceImpl* CSWVGSurfaceImpl::NewL()
    {
    CSWVGSurfaceImpl* self = CSWVGSurfaceImpl::NewLC();
    CleanupStack::Pop();
    return self;
    }

CSWVGSurfaceImpl* CSWVGSurfaceImpl::NewLC()
    {
    CSWVGSurfaceImpl* self = new (ELeave)CSWVGSurfaceImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

inline void CSWVGSurfaceImpl::ConstructL()
    {
    }

TInt CSWVGSurfaceImpl::InitializeSurface(TSize aSize, TInt aColorSpace)
    {
    iSize = aSize;
    return ::VGISymbianInitialize(aSize, (VGIColorSpace)aColorSpace);
    }

void CSWVGSurfaceImpl::SetConfiguration(TInt /*aOption*/, const TAny* /*aValue*/)
    {    
    }

TInt CSWVGSurfaceImpl::CreateSurface(TInt /*aDisplayMode*/, RWindow */*aSurface*/, CFbsBitmap */*aBitmap*/)
    {
    return KErrNone;
    }

TInt CSWVGSurfaceImpl::ResizeSurface(TSize aSize)
    {
    iSize = aSize;
    return ::VGISymbianResize(aSize);
    }

TInt CSWVGSurfaceImpl::CopyBitmap(TInt aDisplayMode,TInt aMaskMode,CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize aSize)
    {
    int ret = KErrNone;
    
    if (aBitmap->Handle() == 0)
        {
        ret = aBitmap->Create(aSize, (TDisplayMode)aDisplayMode);
        }
    
    if (ret != KErrNone)
        {
        return ret;
        }
    
    if (aMask)
        {
        if (aMask->Handle() == 0)
            {
            ret = aMask->Create(aSize, (TDisplayMode)aMaskMode);
            }
        }

    if (ret != KErrNone)
        {
        return ret;
        }

    if(aMask) 
        {
        ret = ::VGISymbianCopyToBitmap(aBitmap, aMask, VGI_SKIP_TRANSPARENT_PIXELS);
        }
    else
        {
        ret = ::VGISymbianCopyToBitmap(aBitmap, aMask, VGI_COPY_TRANSPARENT_PIXELS);
        }
        
    return ret;
    }

TInt CSWVGSurfaceImpl::PrepareToBindClientBuffer()
    {
    return KErrNone;
    }

TInt CSWVGSurfaceImpl::BindClientBuffer(TInt buffer)
    {
    return ::VGISymbianBindToImage(buffer);
    }
  
TInt CSWVGSurfaceImpl::UnBindClientBuffer()
    {
    return ::VGISymbianUnBindImage();
    }

void CSWVGSurfaceImpl::TerminateSurface()
    {
    ::VGISymbianTerminate();
    }
