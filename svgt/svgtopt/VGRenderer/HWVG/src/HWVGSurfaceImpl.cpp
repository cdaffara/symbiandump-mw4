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
* Description:  CHWVGSurfaceImpl source file
 *
*/

#include <eikenv.h>

#ifdef DEBUG_RENDERER
#include <e32debug.h>
#endif

#include "HWVGSurfaceImpl.h"

CHWVGSurfaceImpl::CHWVGSurfaceImpl(TUint8 aOption)
    : iSurfaceType (aOption)
    {
#ifdef HWRENDERER
    iEglDisplay                 = EGL_NO_DISPLAY;
    iEglSurface                 = EGL_NO_SURFACE;
    iEglPBufferSurface_Client   = EGL_NO_SURFACE;
    iEglContext                 = EGL_NO_CONTEXT;
#endif
    }

CHWVGSurfaceImpl::~CHWVGSurfaceImpl()
    {
    }

inline void CHWVGSurfaceImpl::ConstructL()
    {
    }

CHWVGSurfaceImpl* CHWVGSurfaceImpl::NewL(TUint8 aOption)
    {
    CHWVGSurfaceImpl* self = CHWVGSurfaceImpl::NewLC(aOption);
    CleanupStack::Pop();
    return self;
    }

CHWVGSurfaceImpl* CHWVGSurfaceImpl::NewLC(TUint8 aOption)
    {
    CHWVGSurfaceImpl* self = new (ELeave)CHWVGSurfaceImpl(aOption);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

TInt CHWVGSurfaceImpl::MapEGLErrorCodeToSymbian(TInt aErrorCode)
    {
    switch (aErrorCode)
        {
        case EGL_SUCCESS:
            return KErrNone;
        case EGL_NOT_INITIALIZED:
            return KErrNotReady;
        case EGL_BAD_ACCESS:
            return KErrPermissionDenied;
        case EGL_BAD_ALLOC:
            return KErrNoMemory;
        case EGL_BAD_CONTEXT:
        case EGL_BAD_CURRENT_SURFACE:
        case EGL_BAD_DISPLAY:
        case EGL_BAD_SURFACE:
        case EGL_BAD_NATIVE_PIXMAP:
        case EGL_BAD_NATIVE_WINDOW:
        case EGL_CONTEXT_LOST:
            return KErrBadHandle;
        case EGL_BAD_CONFIG:
        case EGL_BAD_PARAMETER:
        case EGL_BAD_MATCH:
        case EGL_BAD_ATTRIBUTE:
            return KErrArgument;
        default:
            return KErrUnknown;
        }
    }

TInt CHWVGSurfaceImpl::InitializeSurface(TSize aSize, TInt /*aColorSpace*/)
    {
#ifdef HWRENDERER
     if((aSize.iWidth==iSize.iWidth) && (aSize.iHeight==iSize.iHeight))
         {
         return 0;
         }
     
    if (iSurfaceType == SURFACE_PBUFFER)
        {
        iSize = aSize;
        iEglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
        if ( iEglDisplay == EGL_NO_DISPLAY )
            {
            return MapEGLErrorCodeToSymbian(eglGetError());
            }
        if ( eglInitialize( iEglDisplay, 0, 0 ) == EGL_FALSE )
            {
            return MapEGLErrorCodeToSymbian(eglGetError());
            }
        if ( eglBindAPI(EGL_OPENVG_API) == EGL_FALSE)
            {
            return MapEGLErrorCodeToSymbian(eglGetError());
            }
        }
    else
        {
        return KErrNotSupported;
        }
    return KErrNone;
#else
    return KErrNotSupported;
#endif
    }

void CHWVGSurfaceImpl::SetConfiguration(TInt /*aOption*/, const TAny* /*aValue*/)
    {
    //TODO
    }

TInt CHWVGSurfaceImpl::CreateSurface(TInt aDisplayMode, RWindow */*aSurface*/, CFbsBitmap *aBitmap)
    {
#ifdef HWRENDERER
    if (iSurfaceType == SURFACE_PBUFFER)
        {
        EGLConfig *configList   = 0;
        EGLint numOfConfigs     = 0; 
        EGLint configSize       = 0;
        
        if ( eglGetConfigs( iEglDisplay, configList, configSize, &numOfConfigs ) == EGL_FALSE )
            {
            return MapEGLErrorCodeToSymbian(eglGetError());
            }
        
        configSize = numOfConfigs;
        
        configList = (EGLConfig*) User::Alloc( sizeof(EGLConfig)*configSize );
        if ( configList == NULL )
            {
            return MapEGLErrorCodeToSymbian(eglGetError());
            }
        

        TInt BufferSize = TDisplayModeUtils::NumDisplayModeBitsPerPixel(EColor16MA);// / 8;

/*        const EGLint attribList[] =
            {
            EGL_BUFFER_SIZE,            BufferSize,
            EGL_RED_SIZE,             5,
            EGL_GREEN_SIZE,           6,
            EGL_BLUE_SIZE,            5,
            EGL_ALPHA_SIZE,           0,
            EGL_RENDERABLE_TYPE,        EGL_OPENVG_BIT,
            EGL_SURFACE_TYPE,           EGL_PBUFFER_BIT,
            EGL_NONE
            };
*/

        const EGLint attribList[] = { 
                 EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
                 EGL_SURFACE_TYPE,         EGL_PBUFFER_BIT,
                 EGL_RED_SIZE, 8,
                 EGL_GREEN_SIZE, 8,
                 EGL_BLUE_SIZE, 8,
                 EGL_ALPHA_SIZE, 8,
                 EGL_NONE 
                };

        if ( eglChooseConfig( iEglDisplay, attribList, configList, configSize, 
                              &numOfConfigs ) == EGL_FALSE )
            {
            User::Free( configList );
            return MapEGLErrorCodeToSymbian(eglGetError());
            }

        iConfig = configList[0]; 
        User::Free( configList ); 

        if ( aBitmap != 0 )
            {
            iSize = aBitmap->SizeInPixels();
            }

        return CreatePBufferSurface();
        }
    else
        {
        return KErrNotSupported;
        }
#else
    return KErrNotSupported;
#endif
    }

TInt CHWVGSurfaceImpl::CreatePBufferSurface()
    {
#ifdef HWRENDERER
    const EGLint attribList[] = { EGL_WIDTH,
                                        iSize.iWidth,
                                        EGL_HEIGHT,
                                        iSize.iHeight,
                                        EGL_NONE };

    iEglSurface = eglCreatePbufferSurface( iEglDisplay, iConfig, attribList );
    if (iEglSurface == EGL_NO_SURFACE)
        {
        return MapEGLErrorCodeToSymbian(eglGetError());
        }

    if (iEglContext == EGL_NO_CONTEXT)
        {
        iEglContext = eglCreateContext( iEglDisplay, iConfig, EGL_NO_CONTEXT, NULL );
        }

    if (iEglContext == EGL_NO_CONTEXT)
        {
        return MapEGLErrorCodeToSymbian(eglGetError());
        }
    if ( eglMakeCurrent( iEglDisplay, iEglSurface, iEglSurface, iEglContext ) == EGL_FALSE )
        {
           return MapEGLErrorCodeToSymbian(eglGetError());
        }
    return KErrNone;
#else
    return KErrNotSupported;
#endif
    }

TInt CHWVGSurfaceImpl::ResizeSurface(TSize aSize)
    {
#ifdef HWRENDERER
    if((aSize.iWidth==iSize.iWidth) && (aSize.iHeight==iSize.iHeight))
        return KErrNone;

    if (iSurfaceType == SURFACE_PBUFFER)
        {
        if (iEglDisplay)
            {
            eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
            eglDestroySurface( iEglDisplay, iEglSurface ); 
            //eglDestroyContext( iEglDisplay, iEglContext );
            }

        iSize = aSize;
        return CreatePBufferSurface();
        }
    else
        {
        return KErrNotSupported;
        }
#else
    return KErrNotSupported;
#endif
    }

TInt CHWVGSurfaceImpl::CopyBitmap(TInt /*aDisplayMode*/,TInt aMaskMode, CFbsBitmap *aBitmap, CFbsBitmap */*aMask*/, TSize /*BmpSize*/)
    {
#ifdef HWRENDERER
    if (iSurfaceType == SURFACE_PBUFFER)
        {
        eglCopyBuffers( iEglDisplay, iEglSurface, aBitmap );
        }
#endif
    }

TInt CHWVGSurfaceImpl::PrepareToBindClientBuffer()
    {
    return KErrNone;
    }



TInt CHWVGSurfaceImpl::BindClientBuffer(TInt buffer)
    {
#ifdef HWRENDERER
    iEglPBufferSurface_Client = eglCreatePbufferFromClientBuffer(iEglDisplay, EGL_OPENVG_IMAGE, buffer, iConfig, 0);

    if (iEglPBufferSurface_Client == EGL_NO_SURFACE)
        {
#ifdef DEBUG_RENDERER        
        RDebug::Print(_L("eglCreatePbufferFromClientBuffer Failed %x "), eglGetError());
#endif
        return KErrGeneral;
        }

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    if ( eglMakeCurrent( iEglDisplay, iEglPBufferSurface_Client, iEglPBufferSurface_Client, iEglContext ) == EGL_FALSE )
        {

#ifdef DEBUG_RENDERER        
        RDebug::Print(_L("eglMakeCurrent Failed %x "), eglGetError());
#endif
        return KErrGeneral;
        }

#endif
    return KErrNone;
    }

  

TInt CHWVGSurfaceImpl::UnBindClientBuffer()
    {
#ifdef HWRENDERER

    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

    if (iEglPBufferSurface_Client)
        {
        eglDestroySurface( iEglDisplay, iEglPBufferSurface_Client );
        iEglPBufferSurface_Client = EGL_NO_SURFACE;
        }

    if ( eglMakeCurrent( iEglDisplay, iEglSurface, iEglSurface, iEglContext ) == EGL_FALSE )
        {
        return KErrGeneral;
        }
#endif
    return KErrNone;
    }



void CHWVGSurfaceImpl::TerminateSurface()
    {
#ifdef HWRENDERER
    if (iEglDisplay != EGL_NO_DISPLAY)
        {
        eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
        if (iEglSurface)
            {
            eglDestroySurface( iEglDisplay, iEglSurface );
            }

        if (iEglContext)
            {
            eglDestroyContext( iEglDisplay, iEglContext );
            }

        eglTerminate( iEglDisplay );
        iEglDisplay = EGL_NO_DISPLAY;
        }
#endif
    }
