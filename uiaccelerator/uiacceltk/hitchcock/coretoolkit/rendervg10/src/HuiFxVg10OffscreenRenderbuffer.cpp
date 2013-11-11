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



#include "HuiFxVg10OffscreenRenderbuffer.h"
#include "HuiVg10RenderPlugin.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiDisplay.h"

CHuiFxVg10OffscreenRenderbuffer* CHuiFxVg10OffscreenRenderbuffer::NewL(CHuiVg10RenderPlugin& aPlugin, const TSize& aSize)
    {
    CHuiFxVg10OffscreenRenderbuffer* e = new (ELeave) CHuiFxVg10OffscreenRenderbuffer();
    CleanupStack::PushL(e);
    e->ConstructL(aPlugin, aSize);
    CleanupStack::Pop(e);
    return e;
    }

void CHuiFxVg10OffscreenRenderbuffer::ConstructL(CHuiVg10RenderPlugin& aPlugin, const TSize& aSize)
    {
    CHuiFxRenderbuffer::ConstructL(aSize, EBufferTypeOffscreen);
    iPlugin = &aPlugin;

    PushEGLContext();
    
#ifndef __WINS__ // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
#else
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
#endif

    iImage = vgCreateImage(imageInternalFormat, aSize.iWidth, aSize.iHeight, 
                           VG_IMAGE_QUALITY_NONANTIALIASED);

    HUIFX_VG_INVARIANT();

    const TInt BITS_PER_CHANNEL = 8;
    
    // Choose an EGL config
    const EGLint attrs[] =
        {
        EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE,    EGL_OPENVG_BIT,
        EGL_RED_SIZE,           BITS_PER_CHANNEL,
        EGL_GREEN_SIZE,         BITS_PER_CHANNEL,
        EGL_BLUE_SIZE,          BITS_PER_CHANNEL,
        EGL_ALPHA_SIZE,         BITS_PER_CHANNEL,
        EGL_NONE
        };
    TInt configCount = iPlugin->EglChooseConfig(attrs);
    ASSERT(configCount > 0);
    EGLConfig config = iPlugin->EglConfig(0);
    
    // Create a context
    iContext = eglCreateContext(iPlugin->EglDisplay(), config,
            iPlugin->EglSharedContext(), NULL);
    ASSERT(iContext);
    
    // Create a pbuffer surface
    iSurface = eglCreatePbufferFromClientBuffer(iPlugin->EglDisplay(), EGL_OPENVG_IMAGE,
                                                iImage, config, NULL);
    ASSERT(iSurface);   
    
    // Initialize the context
    iGc = iPlugin->CreateGcL();
    this->InitGc(aSize);

    PopEGLContext();
    }

CHuiFxVg10OffscreenRenderbuffer::~CHuiFxVg10OffscreenRenderbuffer()
    {  
    delete iGc;
    
    if(iPlugin)
        {
        eglDestroySurface(iPlugin->EglDisplay(), iSurface);
        eglDestroyContext(iPlugin->EglDisplay(), iContext); 
        }
    
    if (iRotatedImage != VG_INVALID_HANDLE)
        {
        vgDestroyImage(iRotatedImage);
        HUIFX_VG_INVARIANT();
        }
    vgDestroyImage(iImage);
    HUIFX_VG_INVARIANT();
    }

void CHuiFxVg10OffscreenRenderbuffer::InitGc(const TSize& aSize)
    {
    BindAsRenderTarget();
    iGc->InitState();
    VGfloat color[] = 
                {
                 .0f, .0f, .0f, .0f
                };
    vgSetfv(VG_CLEAR_COLOR, sizeof(color) / sizeof(VGfloat), color);
    vgClear(0, 0, aSize.iWidth, aSize.iHeight);
    UnbindAsRenderTarget();
    
    // Let renderer know that we have been fiddlling with OpenVg state directly
    // "iGc->InitState" confuses scissoring setting, so lets notify it.
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
    renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyScissor);    
    renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyBlendMode);    
    }

void CHuiFxVg10OffscreenRenderbuffer::PrepareForReuse(const TSize& aReusedSize)
    {
    if (iBackgroundEnabled)
        {
        ReadBackground();
        }
    else
        {
        const TInt COLOR_COMPONENTS = 4;
        VGfloat savedColor[COLOR_COMPONENTS];
        vgGetfv(VG_CLEAR_COLOR, COLOR_COMPONENTS, savedColor);
        
        VGfloat color[COLOR_COMPONENTS] = 
                    {
                     .0f, .0f, .0f, .0f
                    };
        vgSetfv(VG_CLEAR_COLOR, COLOR_COMPONENTS, color);
        vgClearImage(Image(), 0, 0, aReusedSize.iWidth, aReusedSize.iHeight);
        vgSetfv(VG_CLEAR_COLOR, COLOR_COMPONENTS, savedColor);        
        }
    
    HUIFX_VG_INVARIANT();
    }

CHuiGc& CHuiFxVg10OffscreenRenderbuffer::BindAsRenderTarget()
    {
    // Save current context and surfaces
    iSavedContext = eglGetCurrentContext();
    iSavedDrawSurface = eglGetCurrentSurface(EGL_DRAW);
    iSavedReadSurface = eglGetCurrentSurface(EGL_READ);
    ASSERT(iSavedContext != iContext);
    ASSERT(iSavedDrawSurface != iSurface);
    ASSERT(iSavedReadSurface != iSurface);

    // Bind our own surface
    eglMakeCurrent(iPlugin->EglDisplay(), iSurface, iSurface, iContext);
    ASSERT(eglGetError() == EGL_SUCCESS);

    return *iGc;
    }

void CHuiFxVg10OffscreenRenderbuffer::BindAsTexture(THuiFxRenderbufferUsage aUsage)
    {
    // Nothing to do
    }

void CHuiFxVg10OffscreenRenderbuffer::UnbindAsTexture()
    {
    // Nothing to do
    }

void CHuiFxVg10OffscreenRenderbuffer::UnbindAsRenderTarget()
    {
    // Restore original surface & context
    eglMakeCurrent(iPlugin->EglDisplay(), iSavedDrawSurface, iSavedReadSurface,
        iSavedContext);
    ASSERT(eglGetError() == EGL_SUCCESS);
    }

VGImage CHuiFxVg10OffscreenRenderbuffer::Image() const
    {
    return iImage;
    }

void CHuiFxVg10OffscreenRenderbuffer::ReadBackground()
    {
    if (iBackgroundEnabled)
        {
        TBool rotatedDisplay = EFalse;
        CHuiGc::TOrientation orientation = CHuiStatic::Env().PrimaryDisplay().Orientation(); 
        
        // The current context can be the screen context or, in case of nested effects, an "effect off-screen"
        // context. The latter uses VG image that is encapsulated inside PBuffer surface.
        // Only if we have the screen context, the surface content can be in rotated state.
        
        TBool isScreenContext = (iPlugin->EglSharedContext() == eglGetCurrentContext());
        if (isScreenContext)
            {
            rotatedDisplay = orientation == CHuiGc::EOrientationCCW90 || orientation == CHuiGc::EOrientationCW90;
            }


        // Read surface size from egl
        EGLDisplay eglDisp = eglGetCurrentDisplay();
        EGLSurface eglSurf = eglGetCurrentSurface(EGL_DRAW);
        EGLint sfWidth;
        EGLint sfHeight;
        eglQuerySurface(eglDisp, eglSurf, EGL_WIDTH, &sfWidth);
        eglQuerySurface(eglDisp, eglSurf, EGL_HEIGHT, &sfHeight);

        #ifdef HUIFX_TRACE  
        RDebug::Print(_L("CHuiFxVg10OffscreenRenderbuffer::ReadBackground - surface size: %i,%i "), sfWidth, sfHeight);
        #endif
        
        // Read pixels from surface        
        if (!rotatedDisplay)
            {
            // Much easier if no rotation !
            vgGetPixels(iImage, 0, 0, iPosition.iX, sfHeight - (iPosition.iY + iSize.iHeight), iSize.iWidth, iSize.iHeight);
            }
        else
            {
            // If screen is rotated but surface is not in native orientation, this gets difficult
            // because vgGetPixels is not affected by transformations.

            // display size = surface size rotated
            TInt displayHeight = sfWidth;
            TInt displayWidth = sfHeight;
       
            TSize newRotatedImageSize = TSize(iSize.iHeight, iSize.iWidth);
            if (iRotatedImage == VG_INVALID_HANDLE || iRotatedImageSize != newRotatedImageSize)
                {
                PushEGLContext();
                
                // *** Create new vg image
                
                // Remove existing vg image, if any
                if (iRotatedImage != VG_INVALID_HANDLE)
                    {
                    vgDestroyImage(iRotatedImage);
                    iRotatedImage = VG_INVALID_HANDLE;
                    HUIFX_VG_INVARIANT();
                    }
                
                #ifndef __WINS__ // Should possibly query the supported mode instead?
                    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
                #else
                    VGImageFormat imageInternalFormat = VG_sARGB_8888;
                #endif

                    iRotatedImageSize = newRotatedImageSize;
                    iRotatedImage = vgCreateImage(imageInternalFormat, iRotatedImageSize.iWidth, iRotatedImageSize.iHeight, 
                                       VG_IMAGE_QUALITY_NONANTIALIASED);
                    //iRotatedImage = vgCreateImage(imageInternalFormat, iSize.iWidth, iSize.iHeight, 
                    //                   VG_IMAGE_QUALITY_NONANTIALIASED);

                PopEGLContext();
                }
            
            
            // If we have rotation on CHuiGc level, we must manually take that into account when
            // accessing pixels directly                
            TSize rotatedSize = iSize;
            TPoint rotatedPos = iPosition;
            TRect renderBufferLocation = TRect(iPosition, iSize);
            if(orientation == CHuiGc::EOrientationCCW90)
                {
                // Rotate the buffer location relative to real surface coordinates
                rotatedSize = TSize(iSize.iHeight, iSize.iWidth);
                rotatedPos = TPoint(displayHeight - iPosition.iY - iSize.iHeight, iPosition.iX);                        
                renderBufferLocation = TRect(rotatedPos, rotatedSize);            
                }
            else if(orientation == CHuiGc::EOrientationCW90)
                {
                // Rotate the buffer location relative to real surface coordinates
                rotatedSize = TSize(iSize.iHeight, iSize.iWidth);
                rotatedPos = TPoint(iPosition.iY, displayWidth - iPosition.iX - iSize.iWidth);            
                renderBufferLocation = TRect(rotatedPos, rotatedSize);
                }
            else
                {
                // nothing
                }        
            
            #ifdef HUIFX_TRACE 
            RDebug::Print(_L("CHuiFxVg10OffscreenRenderbuffer::ReadBackground - renderBufferLocation: %i,%i, %i,%i "), 
                    renderBufferLocation.iTl.iX,
                    renderBufferLocation.iTl.iY,
                    renderBufferLocation.iBr.iX,
                    renderBufferLocation.iBr.iY);

            TRect vgRect(TPoint(renderBufferLocation.iTl.iX, sfHeight - renderBufferLocation.iTl.iY - rotatedSize.iHeight), rotatedSize);

            RDebug::Print(_L("CHuiFxVg10OffscreenRenderbuffer::ReadBackground - vgRect: %i,%i, %i,%i "), 
                    vgRect.iTl.iX,
                    vgRect.iTl.iY,
                    vgRect.iBr.iX,
                    vgRect.iBr.iY);
            #endif
            
            // So...first get pixels from surface into rotated image
            vgGetPixels(iRotatedImage, 0, 0, renderBufferLocation.iTl.iX, sfHeight - (renderBufferLocation.iTl.iY + rotatedSize.iHeight), rotatedSize.iWidth, rotatedSize.iHeight);
            
            // Draw rotated image into real buffer image, first bind it as render target...
            BindAsRenderTarget();            
            
            // ...store some states...
            const TInt VG_MATRIX_SIZE = 9;
            VGfloat oldMatrix[VG_MATRIX_SIZE];
            vgGetMatrix(oldMatrix);

            // ...set some vg states...
            vgLoadIdentity();
            vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
            vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
            vgSeti(VG_SCISSORING, VG_FALSE);

            TInt w = iSize.iWidth;
            TInt h = iSize.iHeight;
            
            // ...select right rotation...
            if (orientation == CHuiGc::EOrientationCW90)
                {
                // Rotate around origo and move back to displayarea
                vgRotate(-90);
                vgTranslate(-h, 0);
                }
            else if (orientation == CHuiGc::EOrientationCCW90)
                {
                // Rotate around origo and move back to displayarea
                vgRotate(90);
                vgTranslate(0, -w);
                }
            else if (orientation == CHuiGc::EOrientation180)
                {
                // Rotate around origo and move back to displayarea
                vgRotate(180);
                vgTranslate(-w, -h);            
                }
            else
                {
                }        
            
            // ...Draw...
            if (iRotatedImage != VG_INVALID_HANDLE)
                {
                vgDrawImage(iRotatedImage);
                }
            
            // ..and restore default VG states
            vgSeti(VG_SCISSORING, VG_TRUE);
            vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
            vgLoadMatrix(oldMatrix);

            // ...finally unbind image and we should have the content correctly.
            UnbindAsRenderTarget();
            }
        }    
    }

void CHuiFxVg10OffscreenRenderbuffer::PushEGLContext()
    {
    iPreviousEGLState.iContext= eglGetCurrentContext(); 
    TEGLState& state = iPlugin->GetUploadState();
    if (state.iContext == KErrNotFound)
        {
        TEGLState& state = iPlugin->GetUploadState();
        // the first context used for uploading will be used for all texture uploads
        state.iContext      = iPreviousEGLState.iContext; 
        state.iDrawSurface  = eglGetCurrentSurface(EGL_DRAW);
        state.iReadSurface  = eglGetCurrentSurface(EGL_READ);
        state.iDisplay      = eglGetCurrentDisplay();
        }
    else
        {
        // change context only if necessary
        if (iPreviousEGLState.iContext != state.iContext)
            {
            iPreviousEGLState.iDrawSurface  = eglGetCurrentSurface(EGL_DRAW);
            iPreviousEGLState.iReadSurface  = eglGetCurrentSurface(EGL_READ);
            iPreviousEGLState.iDisplay      = eglGetCurrentDisplay();
            eglMakeCurrent(state.iDisplay, state.iDrawSurface, state.iReadSurface, state.iContext);
            }
        }
    }

void CHuiFxVg10OffscreenRenderbuffer::PopEGLContext()
    {
    if (iPreviousEGLState.iContext != iPlugin->GetUploadState().iContext)
        {
        eglMakeCurrent(iPreviousEGLState.iDisplay,  iPreviousEGLState.iDrawSurface, iPreviousEGLState.iReadSurface,iPreviousEGLState.iContext);
        }
    }
