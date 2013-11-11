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
* Description:   Class canvas renderering target
*
*/



#include "huivg10canvasrenderbuffer.h"
#include "HuiVg10RenderPlugin.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiEnv.h"
#include "HuiVg10RenderPlugin.h"
#include "uiacceltk/HuiDisplay.h"

//#define HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL


CHuiVg10CanvasRenderBuffer::CHuiVg10CanvasRenderBuffer()
    {       
    }

CHuiVg10CanvasRenderBuffer::~CHuiVg10CanvasRenderBuffer()
    {
    UnInitialize();
    }

void CHuiVg10CanvasRenderBuffer::UnInitialize()
    {
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();

    UnBind();

    if (iGc)
        {
        delete iGc;
        iGc = 0;
        }
    
    if (iRotatedImage)
        {
        vgDestroyImage(iRotatedImage);
        iRotatedImage = VG_INVALID_HANDLE;
        }

    if (iImage)
        {
        vgDestroyImage(iImage);
        iImage = VG_INVALID_HANDLE;
        }
    
    if (iSurface)
        {
        eglDestroySurface(renderer.EglDisplay(), iSurface);
        iSurface = 0;
        }
    
    if (iContext)
        {
        eglDestroyContext(renderer.EglDisplay(), iContext);      
        iContext = 0;
        }

    iInitialized = EFalse;
    }

void CHuiVg10CanvasRenderBuffer::InitializeL(const TSize& aSize)
    {    
    if (iSize == aSize && iImage && iContext && iSurface)
        {
        // Already initalized
        return;
        }

    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
    
    // Just in case...
    UnInitialize();

    PushEGLContext();
    
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime startTime;
        startTime.UniversalTime();
#endif
    
    iSize = aSize;        

#ifndef __WINS__ // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
#else
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
#endif
    
    iImage = vgCreateImage(imageInternalFormat, iSize.iWidth, iSize.iHeight, 
                           VG_IMAGE_QUALITY_NONANTIALIASED);

    if (iImage == VG_INVALID_HANDLE)
        {
        PopEGLContext();

        VGErrorCode err = vgGetError();
        RDebug::Print(_L("CHuiVg10CanvasRenderBuffer::InitializeL() - vgCreateImage failed: VgError -- %04x"), err);
        User::Leave(KErrGeneral);
        }
        
    const TInt BITS_PER_CHANNEL = 8;
    
    // Choose an EGL config
    const EGLint attrs[] =
        {
        EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
        EGL_RENDERABLE_TYPE,    EGL_OPENVG_BIT,
        EGL_RED_SIZE,           BITS_PER_CHANNEL,
        EGL_GREEN_SIZE,         BITS_PER_CHANNEL,
        EGL_BLUE_SIZE,          BITS_PER_CHANNEL,
        EGL_ALPHA_SIZE,         BITS_PER_CHANNEL,
        EGL_NONE
        };
    TInt configCount = renderer.EglChooseConfig(attrs);
    ASSERT(configCount > 0);

    if (configCount <= 0)
        {
        PopEGLContext();

        RDebug::Print(_L("CHuiVg10CanvasRenderBuffer::InitializeL() - no EGL config found !"));
        User::Leave(KErrGeneral);
        }    
    
    EGLConfig config = renderer.EglConfig(0);
    
    // Create a context
    iContext = eglCreateContext(renderer.EglDisplay(), config,
            renderer.EglSharedContext(), NULL);

    if(!iContext)
        {
        PopEGLContext();

        EGLint err = eglGetError();
        RDebug::Print(_L("CHuiVg10CanvasRenderBuffer::InitializeL() - EGLContext creation failed: EglError -- %04x"), err);
        User::Leave(KErrGeneral);
        }

    // Create a pbuffer surface
    iSurface = eglCreatePbufferFromClientBuffer(renderer.EglDisplay(), EGL_OPENVG_IMAGE,
                                                iImage, config, NULL);
    if(!iSurface)
        {
        PopEGLContext();

        EGLint err = eglGetError();
        RDebug::Print(_L("CHuiVg10CanvasRenderBuffer::InitializeL() - EGLSurface creation failed: EglError -- %04x"), err);
        User::Leave(KErrGeneral);
        }
    
    // Initialize the context (TODO: We perhaps do not really need to do this by creating Cg 
    iGc = renderer.CreateGcL();
    Bind();
    iGc->InitState();
        
    VGfloat color[] = 
                {
                0.f, 0.f, 0.f, 0.f
                };
    vgSetfv(VG_CLEAR_COLOR, sizeof(color) / sizeof(VGfloat), color);
    vgClear(0, 0, iSize.iWidth, iSize.iHeight);
    UnBind();

    // Let renderer know that we have been fiddlling with OpenVg state directly
    // "iGc->InitState" confuses scissoring setting, so lets notify it.
    renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyScissor);    
    renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyBlendMode);    
    
 #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
        RDebug::Print(_L("> CHuiVg10CanvasRenderBuffer::InitializeL took %i ms"), timeInMs);
#endif

    iInitialized = ETrue;
    
    PopEGLContext();
    }

void CHuiVg10CanvasRenderBuffer::Bind()
    {
    // Make sure it is safe to call even if "UnBind" has not been called or this function
    // has already been called.
    if (!iSurface || !iContext || iSavedContext || iSavedDrawSurface || iSavedReadSurface)
        {
        return;
        }
    
    // Save current context and surfaces
    iSavedContext = eglGetCurrentContext();
    iSavedDrawSurface = eglGetCurrentSurface(EGL_DRAW);
    iSavedReadSurface = eglGetCurrentSurface(EGL_READ);
    
    // Bind our own surface
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
    eglMakeCurrent(renderer.EglDisplay(), iSurface, iSurface, iContext);
    }

void CHuiVg10CanvasRenderBuffer::UnBind()
    {
    // Make sure it is safe to call even if "Bind" has not been called or this
    // function has already been called.
    if (!iSavedDrawSurface || !iSavedReadSurface || !iSavedContext)
        {
        return;
        }        
    
    // Restore original surface & context
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
	
	// Todo: Should check whether the saved surface was UI surface 
	// and call CHuiVg10RenderSurface::MakeCurrent() for that instead
	CHuiEnv* env = CHuiEnv::Static();  
	if (env && env->iSwapObserver)
	    {    
        env->iSwapObserver->ReleaseWindowServer();
	    }

    eglMakeCurrent(renderer.EglDisplay(), iSavedDrawSurface, iSavedReadSurface,
        iSavedContext);
        
    if (env && env->iSwapObserver)
        {
        env->iSwapObserver->ReleaseWindowServer(EFalse);
        }
    
    iSavedDrawSurface = 0;
    iSavedReadSurface = 0;
    iSavedContext = 0;    
    }

VGImage CHuiVg10CanvasRenderBuffer::Image() const
    {
    return iImage;
    }


void CHuiVg10CanvasRenderBuffer::PushEGLContext()
    {
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
    iPreviousEGLState.iContext= eglGetCurrentContext(); 
    TEGLState& state = renderer.GetUploadState();
    if (state.iContext == KErrNotFound)
        {
        TEGLState& state = renderer.GetUploadState();
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

void CHuiVg10CanvasRenderBuffer::PopEGLContext()
    {
    CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
    if (iPreviousEGLState.iContext != renderer.GetUploadState().iContext)
        {
        eglMakeCurrent(iPreviousEGLState.iDisplay,  iPreviousEGLState.iDrawSurface, iPreviousEGLState.iReadSurface,iPreviousEGLState.iContext);
        }
    }

void CHuiVg10CanvasRenderBuffer::Copy(const CHuiCanvasRenderBuffer& aSourceBuffer)
    {
    if (iSize != aSourceBuffer.Size())
        {
        return;
        }
    
    CHuiVg10CanvasRenderBuffer* sourcebuffer = (CHuiVg10CanvasRenderBuffer*) &aSourceBuffer;    
    vgCopyImage(iImage, 0, 0, 
            sourcebuffer->Image(), 0, 0, 
            iSize.iWidth, iSize.iHeight, VG_FALSE);
    }

void CHuiVg10CanvasRenderBuffer::Copy(TPoint aPoint)
    {
    ReadBackground(aPoint);
    }

void CHuiVg10CanvasRenderBuffer::ReadBackground(TPoint aPosition)
    {
    CHuiDisplay& display = CHuiStatic::Env().PrimaryDisplay();        
    TBool rotatedDisplay = display.Orientation() == CHuiGc::EOrientationCCW90 || display.Orientation() == CHuiGc::EOrientationCW90;
    TRect renderBufferLocation = TRect(aPosition, iSize);
    
    TRect displayArea = display.VisibleArea();
    
    TInt displayHeight = displayArea.Height();
    TInt displayWidth = displayArea.Width();
   
    TSize rotatedSize = iSize;
    TPoint rotatedPos = aPosition;
    
    // Read pixels from surface        
    if (rotatedDisplay)
        {
        if (iRotatedImage == VG_INVALID_HANDLE)
            {
            PushEGLContext();
            
            #ifndef __WINS__ // Should possibly query the supported mode instead?
                VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
            #else
                VGImageFormat imageInternalFormat = VG_sARGB_8888;
            #endif

            TSize rotatedImageSize = TSize(iSize.iHeight, iSize.iWidth);
            iRotatedImage = vgCreateImage(imageInternalFormat, rotatedImageSize.iWidth, rotatedImageSize.iHeight, 
                               VG_IMAGE_QUALITY_NONANTIALIASED);

            PopEGLContext();
            }
        
        
        // If we have rotation on CHuiGc level, we must manually take that into account when
        // accessing pixels directly                
        if(display.Orientation() == CHuiGc::EOrientationCCW90)
            {
            // Rotate the buffer location relative to real surface coordinates
            rotatedSize = TSize(iSize.iHeight, iSize.iWidth);
            rotatedPos = TPoint(displayHeight - aPosition.iY - iSize.iHeight, aPosition.iX);                        
            renderBufferLocation = TRect(rotatedPos, rotatedSize);            
            }
        else if(display.Orientation() == CHuiGc::EOrientationCW90)
            {
            // Rotate the buffer location relative to real surface coordinates
            rotatedSize = TSize(iSize.iHeight, iSize.iWidth);
            rotatedPos = TPoint(aPosition.iY, displayWidth - aPosition.iX - iSize.iWidth);            
            renderBufferLocation = TRect(rotatedPos, rotatedSize);
            }
        else
            {
            // nothing
            }        

        // If screen is rotated but surface is not in native orientation, this gets difficult
        // because vgGetPixels is not affected by transformations.

        // Swap h and w so that those are the "real" values from surface point of view.
        displayHeight = displayWidth;

        #ifdef HUIFX_TRACE    
        RDebug::Print(_L("CHuiFxVg10OffscreenRenderbuffer::PrepareForReuse - renderBufferLocation: %i,%i, %i,%i "), 
                renderBufferLocation.iTl.iX,
                renderBufferLocation.iTl.iY,
                renderBufferLocation.iBr.iX,
                renderBufferLocation.iBr.iY);

        TRect vgRect(TPoint(renderBufferLocation.iTl.iX, displayHeight - renderBufferLocation.iTl.iY - rotatedSize.iHeight), rotatedSize);

        RDebug::Print(_L("CHuiFxVg10OffscreenRenderbuffer::PrepareForReuse - vgRect: %i,%i, %i,%i "), 
                vgRect.iTl.iX,
                vgRect.iTl.iY,
                vgRect.iBr.iX,
                vgRect.iBr.iY);
        #endif
        
        // So...first get pixels from surface into rotated image
        vgGetPixels(iRotatedImage, 0, 0, renderBufferLocation.iTl.iX, displayHeight - renderBufferLocation.iTl.iY - rotatedSize.iHeight, rotatedSize.iWidth, rotatedSize.iHeight);
        
        // Draw rotated image into real buffer image, first bind it as render target...
        Bind();            
        
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
        if (display.Orientation() == CHuiGc::EOrientationCW90)
            {
            // Rotate around origo and move back to displayarea
            vgRotate(-90);
            vgTranslate(-h, 0);
            }
        else if (display.Orientation() == CHuiGc::EOrientationCCW90)
            {
            // Rotate around origo and move back to displayarea
            vgRotate(90);
            vgTranslate(0, -w);
            }
        else if (display.Orientation() == CHuiGc::EOrientation180)
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
        UnBind();
        }
    else
        {
        // Much easier if no rotation !
        vgGetPixels(iImage, 0, 0, renderBufferLocation.iTl.iX, displayHeight - renderBufferLocation.iTl.iY - rotatedSize.iHeight, rotatedSize.iWidth, rotatedSize.iHeight);
        }
    }

void CHuiVg10CanvasRenderBuffer::InitializeL(const TSize& aSize, TBool aForceClear)
    {
    if (!IsInitialized())
        {
        InitializeL(aSize);
        }
    else
        {
        if (aForceClear)
            {
            CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();

            // Reinitialize the context
            PushEGLContext();
        
            Bind();
            iGc->InitState();
        
            VGfloat color[] = 
                {
                0.f, 0.f, 0.f, 0.f
                };
            vgSetfv(VG_CLEAR_COLOR, sizeof(color) / sizeof(VGfloat), color);
            vgClear(0, 0, iSize.iWidth, iSize.iHeight);
            UnBind();

            // Let renderer know that we have been fiddlling with OpenVg state directly
            // "iGc->InitState" confuses scissoring setting, so lets notify it.
            renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyScissor);    
            renderer.AddRestoreStateFlags(EHuiVg10GcStateFlagDirtyBlendMode);    
    
            PopEGLContext();
            }
        }
    }

