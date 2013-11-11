/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/



#include "HuiFxVg10OnscreenRenderbuffer.h"
#include "HuiVg10RenderSurface.h"
#include "HuiVg10RenderPlugin.h"

CHuiFxVg10OnscreenRenderbuffer* CHuiFxVg10OnscreenRenderbuffer::NewL(CHuiVg10RenderPlugin& aPlugin, CHuiVg10RenderSurface& aSurface)
    {
    CHuiFxVg10OnscreenRenderbuffer* e = new (ELeave) CHuiFxVg10OnscreenRenderbuffer();
    CleanupStack::PushL(e);
    e->ConstructL(aPlugin, aSurface);
    CleanupStack::Pop(e);
    return e;
    }

void CHuiFxVg10OnscreenRenderbuffer::ConstructL(CHuiVg10RenderPlugin& aPlugin, CHuiVg10RenderSurface& aSurface)
    {
    CHuiFxRenderbuffer::ConstructL(aSurface.Size(), EBufferTypeOnscreen);
    iPlugin = &aPlugin;
    iSurface = &aSurface;
    iTextureUsage = ERenderbufferUsageReadWrite;
    }


CHuiFxVg10OnscreenRenderbuffer::~CHuiFxVg10OnscreenRenderbuffer()
    {
    ReleaseVgImage();
    
    delete iGc;
    }

CHuiGc& CHuiFxVg10OnscreenRenderbuffer::BindAsRenderTarget()
    {
    // Save current context and surfaces
    iSavedContext = eglGetCurrentContext();
    iSavedDrawSurface = eglGetCurrentSurface(EGL_DRAW);
    iSavedReadSurface = eglGetCurrentSurface(EGL_READ);

    // Bind our own surface
    iSurface->MakeCurrent();
    
    return *iGc;
    }

void CHuiFxVg10OnscreenRenderbuffer::BindAsTexture(THuiFxRenderbufferUsage aUsage)
    {
    // Copy the contents of the framebuffer into our backbuffer image
    // TODO: figure out a more efficient way of doing this
    iTextureUsage = aUsage;
    if (aUsage == ERenderbufferUsageReadWrite || aUsage == ERenderbufferUsageReadOnly)
        {
        CreateVgImage();
        BindAsRenderTarget();
        vgGetPixels(iImage, 0, 0, 0, 0, Size().iWidth, Size().iHeight);
        HUIFX_VG_INVARIANT();
        UnbindAsRenderTarget();
        }
    }

void CHuiFxVg10OnscreenRenderbuffer::UnbindAsTexture()
    {
    // Copy the contents from our backbuffer image to the framebuffer
    if (iTextureUsage == ERenderbufferUsageReadWrite || iTextureUsage == ERenderbufferUsageWriteOnly)
        {
        BindAsRenderTarget();
        vgSetPixels(0, 0, iImage, 0, 0, Size().iWidth, Size().iHeight);
        HUIFX_VG_INVARIANT();
        UnbindAsRenderTarget();
        ReleaseVgImage();
        }
    }

void CHuiFxVg10OnscreenRenderbuffer::UnbindAsRenderTarget()
    {
    // Restore original surface & context
    eglMakeCurrent(iPlugin->EglDisplay(), iSavedDrawSurface, iSavedReadSurface,
                   iSavedContext);
    }

VGImage CHuiFxVg10OnscreenRenderbuffer::Image() const
    {
    return iImage;
    }

void CHuiFxVg10OnscreenRenderbuffer::PrepareForReuse(const TSize& /* aReusedRect */)
    {
    // this should never happen.
    ASSERT(FALSE);
    }

void CHuiFxVg10OnscreenRenderbuffer::CreateVgImage()
    {
#ifndef __WINS__ // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
#else
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
#endif
    
    iImage = vgCreateImage(imageInternalFormat, Size().iWidth, Size().iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);
    HUIFX_VG_INVARIANT();
    
    // Initialize the context
    iGc = iPlugin->CreateGcL();
    BindAsRenderTarget();
    iGc->InitState();
    UnbindAsRenderTarget();
    
    // Let renderer know that we have been fiddlling with OpenVg state directly
    // "iGc->InitState" confuses scissoring setting, so lets notify it.
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
    renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyScissor);    
    renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyBlendMode);        
    }

void CHuiFxVg10OnscreenRenderbuffer::ReleaseVgImage()
    {
    vgDestroyImage(iImage);
    HUIFX_VG_INVARIANT();
    }

