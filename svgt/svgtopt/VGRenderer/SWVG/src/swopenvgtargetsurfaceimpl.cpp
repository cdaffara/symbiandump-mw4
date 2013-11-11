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
 * Description:  CSWVGTargetSurfaceImpl source file
 *
*/

#include "swopenvgtargetSurfaceImpl.h"
    
#include <platform/vg/vgcontext.h>

#include <svgtbitmap.h>

CSWVGTargetSurfaceImpl::CSWVGTargetSurfaceImpl()
    {
    }

CSWVGTargetSurfaceImpl::~CSWVGTargetSurfaceImpl()
    {
    }

CSWVGTargetSurfaceImpl* CSWVGTargetSurfaceImpl::NewL()
    {
    CSWVGTargetSurfaceImpl* self = CSWVGTargetSurfaceImpl::NewLC();
    CleanupStack::Pop();
    return self;
    }

CSWVGTargetSurfaceImpl* CSWVGTargetSurfaceImpl::NewLC()
    {
    CSWVGTargetSurfaceImpl* self = new (ELeave) CSWVGTargetSurfaceImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

inline void CSWVGTargetSurfaceImpl::ConstructL()
    {
    }

TInt CSWVGTargetSurfaceImpl::InitializeSurface(TSize aSize, TInt aColorSpace)
    {
    iSize = aSize;
    TInt vgError = ::VGIInitialize(aSize.iWidth,aSize.iHeight, (VGIColorSpace)aColorSpace);
    return MapOpenVGErrorToSymbianError(vgError);
    }

void CSWVGTargetSurfaceImpl::SetConfiguration(TInt /*aOption*/, const TAny* /*aValue*/)
    {    
    }

TInt CSWVGTargetSurfaceImpl::CreateSurface(TInt /*aDisplayMode*/, RWindow */*aSurface*/, CFbsBitmap */*aBitmap*/)
    { 
    return KErrNotSupported;
    }

TInt CSWVGTargetSurfaceImpl::CreateSurface(TInt /*aDisplayMode*/, RWindow */*aSurface*/, CSvgtBitmap */*aBitmap*/)
    {
    return KErrNone;
    }

TInt CSWVGTargetSurfaceImpl::ResizeSurface(TSize aSize)
    {
    iSize = aSize;
    TInt vgError = ::VGIResize(aSize.iWidth,aSize.iHeight);
    return MapOpenVGErrorToSymbianError(vgError);
    }

TInt CSWVGTargetSurfaceImpl::CopyBitmap(TInt /*aDisplayMode*/, TInt /*aMaskMode*/, CFbsBitmap */*aBitmap*/, CFbsBitmap */*aMask*/, TSize /*aSize*/)
    {
    return KErrNotSupported;
    }
TInt CSWVGTargetSurfaceImpl::CopyBitmap(TInt /*aDisplayMode*/,TInt /*aMaskMode*/,CSvgtBitmap *aBitmap, CSvgtBitmap *aMask, TSize /*aSize*/)
    {
    VGIColorBufferFormat format = 
            (VGIColorBufferFormat)MapSymbianDisplayModeToOpenVG( (TDisplayMode)aBitmap->DisplayMode() );
    
    TInt vgError = KErrNone;
    if(aMask) 
        {
        vgError = ::VGICopyToTarget(format, 
                    aBitmap->Stride(), aBitmap->BitmapBuffer(), 
                    aMask->Stride(), aMask->BitmapBuffer(), 
                    VGI_SKIP_TRANSPARENT_PIXELS);
        }
    else
        {
        vgError = ::VGICopyToTarget(format, 
                    aBitmap->Stride(), aBitmap->BitmapBuffer(), 
                    0, NULL, 
                    VGI_COPY_TRANSPARENT_PIXELS);
        }
        
    return MapOpenVGErrorToSymbianError(vgError);
    }

TInt CSWVGTargetSurfaceImpl::PrepareToBindClientBuffer()
    {
    return KErrNone;
    }

TInt CSWVGTargetSurfaceImpl::BindClientBuffer(TInt buffer)
    {
    return MapOpenVGErrorToSymbianError(::VGIBindToImage(buffer));
    }
  
TInt CSWVGTargetSurfaceImpl::UnBindClientBuffer()
    {
    return MapOpenVGErrorToSymbianError(::VGIUnBindImage());
    }

void CSWVGTargetSurfaceImpl::TerminateSurface()
    {
    ::VGITerminate();
    }

TInt CSWVGTargetSurfaceImpl::MapSymbianDisplayModeToOpenVG( TDisplayMode aMode) const
    {
    VGIColorBufferFormat format;
    switch( aMode )
    {
    case EColor16MA:
        format = VGI_COLOR_BUFFER_FORMAT_ARGB8888;
        break;

    case EColor16MU:
        format = VGI_COLOR_BUFFER_FORMAT_XRGB8888;
        break;

    case EColor16MAP:
        format = VGI_COLOR_BUFFER_FORMAT_ARGB8888_PRE;
        break;
        
    case EColor16M:
        format = VGI_COLOR_BUFFER_FORMAT_RGB888;
        break;

    case EColor64K:
        format = VGI_COLOR_BUFFER_FORMAT_RGB565;
        break;

    case EColor4K:
        format = VGI_COLOR_BUFFER_FORMAT_XRGB4444;
        break;

    default:
        return KErrNotSupported;
    }
    return format;
    }

TInt CSWVGTargetSurfaceImpl::MapOpenVGErrorToSymbianError( TInt aError ) const
    {
    TInt error = KErrNone;
    switch (aError)
        {
        case VGI_OK:
            {
            error = KErrNone;
            break;
            }
        case VG_OUT_OF_MEMORY_ERROR:
        case VGI_ERROR_OUT_OF_MEMORY:
            {
            error = KErrNoMemory;
            break;
            }
        case VG_ILLEGAL_ARGUMENT_ERROR:
        case VGI_ERROR_INVALID_ARGUMENTS:
            {
            error = KErrArgument;
            break;
            }
        case VGI_ERROR_ALREADY_EXISTS:
            {
            error = KErrAlreadyExists;
            break;
            }
        case VG_UNSUPPORTED_PATH_FORMAT_ERROR:
        case VGI_ERROR_COLORSPACE_NOT_SUPPORTED:
        case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR:
        case VGI_ERROR_NOT_SUPPORTED:
            {
            error = KErrNotSupported;
            break;
            }
        case VGI_ERROR_ILLEGAL_IMAGE_HANDLE:
            {
            error = KErrBadHandle;
            break;
            }
        case VG_IMAGE_IN_USE_ERROR:
        case VGI_ERROR_IMAGE_IN_USE:
            {
            error = KErrInUse;
            break;
            }
        case VGI_ERROR_ILLEGAL_OPERATION:
            {
            error = KErrPermissionDenied;
            break;
            }
        case VG_BAD_HANDLE_ERROR:
        case VG_PATH_CAPABILITY_ERROR:
            {
            error = KErrUnknown;
            break;
            }
        default:
            {
            error = KErrUnknown;
            }
        }

    return error;
    }
