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
* Description:   OpenVG 1.0 render surface implementation.
*
*/



#include "HuiVg10RenderSurface.h"
#include "HuiVg10RenderPlugin.h"
#include "uiacceltk/HuiSegmentedTexture.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiUtil.h"
#include <coecntrl.h>
#include <w32std.h>
#include <gdi.h>
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiEnv.h"
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
#include <graphics/wstfxconst.h>
#endif

#include <M3G/m3g_core.h>


CHuiVg10RenderSurface* CHuiVg10RenderSurface::NewL(CHuiDisplay& aDisplay)
    {
    CHuiVg10RenderSurface* self = CHuiVg10RenderSurface::NewLC(aDisplay);
    CleanupStack::Pop(self);
    return self;
    }


CHuiVg10RenderSurface* CHuiVg10RenderSurface::NewLC(CHuiDisplay& aDisplay)
    {
    CHuiVg10RenderSurface* self = new (ELeave) CHuiVg10RenderSurface(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiVg10RenderSurface::CHuiVg10RenderSurface(CHuiDisplay& aDisplay)
        : iDisplay(&aDisplay)
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED
    }


void CHuiVg10RenderSurface::ConstructL()
    {
    HUI_DEBUG(_L("CHuiVg10RenderSurface::ConstructL() - Constructing OpenVG 1.0 Rendering surface."));

    // Size of the surface.
    iSize = iDisplay->Size();

    // Create egl context and rendering surface
    iEglContext = iEglSurface  = NULL;
    iBoundTexture = NULL;
    RestoreL();

    HUI_DEBUG(_L("CHuiVg10RenderSurface::ConstructL() - Construction succeeded."));
}


CHuiVg10RenderSurface::~CHuiVg10RenderSurface()
    {
    Release();
    iDisplay = NULL;
    HUI_PROBE_REPORT_DESTRUCTED
    }


CHuiDisplay& CHuiVg10RenderSurface::Display() const
    {
    return *iDisplay;
    }
    
    
TUint CHuiVg10RenderSurface::Flags() const
	{    
	TBool useDirtyAreas = CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowSwapBufferPreserve);    	
    if (useDirtyAreas)
	    {
	    return MHuiRenderSurface::EFlagUseDirtyRects;
	    }
	else
	    {
	    return MHuiRenderSurface::EFlagNone;
	    }
	}


TBool CHuiVg10RenderSurface::GetScreenOrigin(TPoint& aOrigin) const
    {
    if(iDisplay && iDisplay->NativeControl())
        {
        aOrigin = iDisplay->NativeControl()->PositionRelativeToScreen();
        return ETrue;
        }
    return EFalse;
    }

static void CreateConstEglAttribs(int& i, EGLint* attribList)
    {
    const TInt BITS_PER_CHANNEL = 8;
    
    attribList[i++] = EGL_RED_SIZE;
    attribList[i++] = BITS_PER_CHANNEL;
	
    attribList[i++] = EGL_GREEN_SIZE;
    attribList[i++] = BITS_PER_CHANNEL;
    
    attribList[i++] = EGL_BLUE_SIZE;
    attribList[i++] = BITS_PER_CHANNEL;
    
    attribList[i++] = EGL_ALPHA_SIZE;
    attribList[i++] = BITS_PER_CHANNEL;
    
    attribList[i++] = EGL_SURFACE_TYPE;

    TBool useDirtyAreas = CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowSwapBufferPreserve);       
    if (useDirtyAreas)
        {    
        attribList[i++] = EGL_WINDOW_BIT | EGL_PBUFFER_BIT | EGL_SWAP_BEHAVIOR_PRESERVED_BIT;
        }
    else
        {
        attribList[i++] = EGL_WINDOW_BIT | EGL_PBUFFER_BIT;
        }

    attribList[i++] = EGL_RENDERABLE_TYPE;
    attribList[i++] = EGL_OPENVG_BIT;
    }

EGLConfig CHuiVg10RenderSurface::DetermineConfig()
    {
	EGLint attribList[20];  // reserve enough space for configurations	
	int i = 0;
	
	// Get default properties for the wanted EGLSurface.
    CreateConstEglAttribs(i, attribList);

	// Terminate attribute list
	attribList[i++] = EGL_NONE;

    CHuiVg10RenderPlugin& render = CHuiStatic::Vg10Renderer();

    // Choose an EGLConfig that best matches to the properties in attribList.
    TInt numOfConfigs = render.EglChooseConfig(attribList);

    if(!numOfConfigs)
        {
        TInt eglerr = eglGetError();
        HUI_DEBUG2(_L("CHuiVg10RenderSurface::DetermineConfig() - eglChooseConfig failed, no required EGL configuration found. Cause: %S (error %i). Panicking."), &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
        HUI_PANIC(THuiPanic::ERenderSurfaceUnknownDisplayMode);
        }

    HUI_DEBUG1(_L("CHuiVg10RenderSurface::DetermineConfig() - Got %i configs."), numOfConfigs);

    // Print the attributes of the selected config to log file.
    render.EglPrintConfig(0);

    // Choose the best EGLConfig. EGLConfigs returned by eglChooseConfig
    // are sorted so that the best matching EGLConfig is first in the list.
    return render.EglConfig(0);
    }


EGLSurface CHuiVg10RenderSurface::CreateSurface(EGLConfig aConfig)
    {
	EGLint* attriblistPtr = NULL;
#ifndef __WINS__ // \todo instead of "#ifndef" should make a check if alpha mode is supported
    // Create a rendering surface.
    EGLint attriblist[] = 
        {
        EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE,
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
        EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER,
#endif
        EGL_NONE
        };
    attriblistPtr = &attriblist[0];
#endif
    
    if(iDisplay->NativeWindow())
        {
        HUI_DEBUG(_L("CHuiVg10RenderSurface::CreateSurface() - Attempting to create a real window surface."));
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
        RDebug::Printf("STFX: ALF_HACK CHuiVg10RenderSurface::CreateSurface()");
        iDisplay->NativeWindow()->SetPurpose(ETfxPurposeDesktopWindow);
#endif
        EGLSurface surf = eglCreateWindowSurface(iEglDisplay, aConfig,
                                      iDisplay->NativeWindow(), attriblistPtr);

        TBool useDirtyAreas = CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowSwapBufferPreserve);       
        if (useDirtyAreas)
            {    
            eglSurfaceAttrib(iEglDisplay, surf, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED );
            }
        return surf;
        }
    else
        {
        // Pbuffer attributes.
        EGLint pbufferAttribList[] =
            {
            EGL_WIDTH,      iSize.iWidth,
            EGL_HEIGHT,     iSize.iHeight,
            EGL_NONE
            };

        HUI_DEBUG(_L("CHuiVg10RenderSurface::CreateSurface() - Attempting to create a pbuffer surface."));
        return eglCreatePbufferSurface(iEglDisplay, aConfig, pbufferAttribList);
        }
    }


TSize CHuiVg10RenderSurface::Size() const
    {
    return iSize;
    }


void CHuiVg10RenderSurface::SetSizeL(const TSize& aSize)
    {
    iSize = aSize;

    if(!iDisplay->NativeWindow())
        {
        // Recreate pbuffer surfaces.
        eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        // Recreate the rendering surface.
        eglDestroySurface(iEglDisplay, iEglSurface);

        iEglSurface = CreateSurface(DetermineConfig());

        if(iEglSurface == EGL_NO_SURFACE)
            {
            TInt eglerr = eglGetError();
            switch (eglerr)
                {
                case EGL_BAD_ALLOC:
                    HUI_DEBUG2(_L("CHuiVg10RenderSurface::SetSizeL() - ERROR! Failed to (re)create surface having size (%ix%i). Out of memory."), 
                               aSize.iWidth, aSize.iHeight);
                    User::Leave(KErrNoMemory);
                default:
                    HUI_DEBUG4(_L("CHuiVg10RenderSurface::SetSizeL() - ERROR! Failed to (re)create surface having size (%ix%i).Cause: %S (EGL error code: %i). Leaving."), 
                                aSize.iWidth, aSize.iHeight, 
                                &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
                case EGL_SUCCESS: // indicates that no error was recorded, but still the CreateSurface call failed (=> unable to continue!)
                    User::Leave(KErrNotSupported);
                }
            }
        }
    }


void CHuiVg10RenderSurface::MakeCurrent()
	{
	// No need to change the current render surface if this render surface is current already.
	if(CHuiStatic::CurrentRenderSurface() != ((MHuiRenderSurface*)this))
		{
		CHuiEnv* env = CHuiEnv::Static();  
		if (env && env->iSwapObserver)
		    {    
            env->iSwapObserver->ReleaseWindowServer();
		    }
		
		eglMakeCurrent(iEglDisplay, iEglSurface, iEglSurface, iEglContext);
        
        if (env && env->iSwapObserver)
            {
            env->iSwapObserver->ReleaseWindowServer(EFalse);
            }

		TInt eglerr = eglGetError();
		if(eglerr != EGL_SUCCESS)
			{
			HUI_DEBUG3(_L("CHuiVg10RenderSurface::MakeCurrent() - Error in switching on the render surface, EGLDisplay: %i, EGLSurface: %i, EGLContext: %i."),
				iEglDisplay, iEglSurface, iEglContext);
			HUI_DEBUG2(_L("CHuiVg10RenderSurface::MakeCurrent() - Could not switch this rendering surface on. Cause: %S (EGL error code: %i). Panicking."),
				&CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
			HUI_PANIC(THuiPanic::ERenderSurfaceActivationFailed)
			}
		CHuiStatic::SetCurrentRenderSurface(this);               
		}
	}

void CHuiVg10RenderSurface::SwapBuffers()
    {
    eglSwapBuffers(iEglDisplay, iEglSurface);
    }

void CHuiVg10RenderSurface::Finish()
    {
    vgFinish();
    }

void CHuiVg10RenderSurface::BindTexture(TInt /*aTextureUnit*/,
                                        const MHuiSegmentedTexture& aTexture,
                                        TInt /*aSegment*/)
    {
    iBoundTexture = &aTexture;
    }


const MHuiSegmentedTexture* CHuiVg10RenderSurface::BoundTexture() const
    {
    return iBoundTexture;
    }

void CHuiVg10RenderSurface::HandleVisibilityEvent(TBool /* aIsVisible */ )
    {
    // Dummy method, does not handle the events in any way...
    }

void CHuiVg10RenderSurface::Release()
    {
    HUI_DEBUG( _L("CHuiVg10RenderSurface::Release() - Releasing EGL surface and context..") );
    if (iEglSurface != NULL)
        {
        eglDestroySurface(iEglDisplay, iEglSurface);
        TInt eglerr = eglGetError();
        if(eglerr != EGL_SUCCESS)
            {
            HUI_DEBUG2(_L("CHuiVg10RenderSurface::Release() - WARNING! Could not destroy EGL surface. Cause: %S (EGL error code: %i)."),
                       &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            }
        iEglSurface = NULL;
        }
    if (iEglContext != NULL)
        {
        // Unshare this context if it was
        // set as shared..
        CHuiVg10RenderPlugin& renderer = CHuiStatic::Vg10Renderer();
        if(renderer.EglSharedContext() == iEglContext)
            {
            HUI_DEBUG(_L("CHuiVg10RenderSurface::Release() - Unsharing this render context."));
            renderer.EglSetSharedContext(EGL_NO_CONTEXT);
            }

        eglDestroyContext(iEglDisplay, iEglContext);
        TInt eglerr = eglGetError();
        if(eglerr != EGL_SUCCESS)
            {
            HUI_DEBUG2(_L("CHuiVg10RenderSurface::Release() - WARNING! Could not destroy EGL context. Cause: %S (EGL error code: %i)."),
                       &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            }
        iEglContext = NULL;
        }
    }

void CHuiVg10RenderSurface::RestoreL()
    {
    HUI_DEBUG( _L("CHuiVg10RenderSurface::RestoreL() - Recreating render surface.") );

    if((iEglSurface != NULL) && (iEglContext != NULL))
        {
        HUI_DEBUG( _L("CHuiVg10RenderSurface::RestoreL() - Render surface and context already in place, no need to restore.") );
        }

    CHuiVg10RenderPlugin& render = CHuiStatic::Vg10Renderer();
    iEglDisplay = render.EglDisplay();

    // Create an EGL rendering surface.
    EGLConfig config = DetermineConfig();
    iEglSurface = CreateSurface(config);

    if(iEglSurface == EGL_NO_SURFACE)
        {
        TInt eglerr = eglGetError();
        switch (eglerr)
            {
            case EGL_BAD_ALLOC:
                HUI_DEBUG2(_L("CHuiVg10RenderSurface::RestoreL() - ERROR! Failed to create rendering surface having size (%ix%i). Out of memory."), 
                           iSize.iWidth, iSize.iHeight);
                User::Leave(KErrNoMemory);
            default:
                HUI_DEBUG4(_L("CHuiVg10RenderSurface::RestoreL() - ERROR! Failed to create rendering surface having size (%ix%i). Cause: %S (EGL error code: %i)."), 
                            iSize.iWidth, iSize.iHeight, 
                            &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
                User::Leave(KErrNotSupported);
            }
        }

    // There is only a single texture manager in the environment, so textures
    // need to shared between all contexts.
    iEglContext = eglCreateContext(iEglDisplay, config,
                                   render.EglSharedContext(), NULL);
    if(iEglContext == EGL_NO_CONTEXT)
        {
        TInt eglerr = eglGetError();
        switch (eglerr)
            {
            case EGL_BAD_ALLOC:
                HUI_DEBUG(_L("CHuiVg10RenderSurface::RestoreL() - ERROR! Failed to create rendering context. Out of memory."));
                User::Leave(KErrNoMemory);
            default:
                HUI_DEBUG2(_L("CHuiVg10RenderSurface::RestoreL() - ERROR! Failed to create rendering context. Cause: %S (EGL error code: %i)."),                            
                            &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
                User::Leave(KErrNotSupported);
            }
        }

    // Share the created context for texture management.
    if(render.EglSharedContext() == EGL_NO_CONTEXT)
        {
        // Now that there is a context, make sure the renderer knows of it.
        render.EglSetSharedContext(iEglContext);
        }
    }

const TDesC& CHuiVg10RenderSurface::EglErrorMessage(TInt aErrorCode)
    {
    // EGL Error messages.
    _LIT(KEglErrorSUCCESS, "The last function succeeded without error.");
    _LIT(KEglErrorNOT_INITIALIZED, "EGL is not initialized, or could not be initialized, for the specified EGL display connection.");
    _LIT(KEglErrorBAD_ACCESS, "EGL cannot access a requested resource (for example a context is bound in another thread).");
    _LIT(KEglErrorBAD_ALLOC, "EGL failed to allocate resources for the requested operation.");
    _LIT(KEglErrorBAD_ATTRIBUTE, "An unrecognized attribute or attribute value was passed in the attribute list.");
    _LIT(KEglErrorBAD_CONTEXT, "An EGLContext argument does not name a valid EGL rendering context.");
    _LIT(KEglErrorBAD_CONFIG, "An EGLConfig argument does not name a valid EGL frame buffer configuration.");
    _LIT(KEglErrorBAD_CURRENT_SURFACE, "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid.");
    _LIT(KEglErrorBAD_DISPLAY, "An EGLDisplay argument does not name a valid EGL display connection.");
    _LIT(KEglErrorBAD_SURFACE, "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering.");
    _LIT(KEglErrorBAD_MATCH, "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface).");
    _LIT(KEglErrorBAD_PARAMETER, "One or more argument values are invalid.");
    _LIT(KEglErrorBAD_NATIVE_PIXMAP, "A NativePixmapType argument does not refer to a valid native pixmap.");
    _LIT(KEglErrorBAD_NATIVE_WINDOW, "A NativeWindowType argument does not refer to a valid native window.");
    _LIT(KEglErrorUNKNOWN, "Unknown EGL Error.");

    // Define literals for the errors
    switch(aErrorCode)
        {
        case EGL_SUCCESS: return KEglErrorSUCCESS;
        case EGL_NOT_INITIALIZED: return KEglErrorNOT_INITIALIZED;
        case EGL_BAD_ACCESS: return KEglErrorBAD_ACCESS;
        case EGL_BAD_ALLOC: return KEglErrorBAD_ALLOC;
        case EGL_BAD_ATTRIBUTE: return KEglErrorBAD_ATTRIBUTE;
        case EGL_BAD_CONTEXT: return KEglErrorBAD_CONTEXT;
        case EGL_BAD_CONFIG: return KEglErrorBAD_CONFIG;
        case EGL_BAD_CURRENT_SURFACE: return KEglErrorBAD_CURRENT_SURFACE;
        case EGL_BAD_DISPLAY: return KEglErrorBAD_DISPLAY;
        case EGL_BAD_SURFACE: return KEglErrorBAD_SURFACE;
        case EGL_BAD_MATCH: return KEglErrorBAD_MATCH;
        case EGL_BAD_PARAMETER: return KEglErrorBAD_PARAMETER;
        case EGL_BAD_NATIVE_PIXMAP: return KEglErrorBAD_NATIVE_PIXMAP;
        case EGL_BAD_NATIVE_WINDOW: return KEglErrorBAD_NATIVE_WINDOW;
        default: return KEglErrorUNKNOWN;
        }
    }

void CHuiVg10RenderSurface::HandleDisplayUsageChangeL()
    {
    // Nothing to do
    }
    
void CHuiVg10RenderSurface::SetDirtyRect(const TRect& /*aRect*/)
    {
    // Nothing to do
    }
    
MHuiTargetBitmap* CHuiVg10RenderSurface::BackBuffer()
    {
    // Nothing to do
    return NULL;
    }

CFbsBitGc* CHuiVg10RenderSurface::Gc()
    {
    // Nothing to do
    return NULL;
    }
    
void CHuiVg10RenderSurface::RenderSurfaceExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    *aExtensionParams = NULL;
    }

HUI_SESSION_OBJECT_IMPL(CHuiVg10RenderSurface, ETypeRenderSurface)
