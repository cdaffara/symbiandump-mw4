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
* Description:   Implementation of CHuiVg10RenderPlugin. Rendering plugin for OpenVG 1.0.
*
*/



#include "HuiVg10RenderPlugin.h"
#include "HuiVg10RenderSurface.h"
#include "HuiVg10PBufferSurface.h"
#include "HuiVg10Gc.h"
#include "HuiVg10Texture.h"
#include "HuiVg10TextureManager.h"
#include "HuiVg10CurvePath.h"
#include "huivg10canvasgc.h"
#include "HuiRasterizedTextMesh.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiDisplay.h"
#include "HuiFxVg10Engine.h"
#include "../../CommonInc/uiacceltkdomaincrkeys.h"

#ifdef __NVG
    #include <nvg.h>
    #include "HuiVg10VgImageBinder.h"
#endif

#include <AknUtils.h>
#include <implementationproxy.h>

#if !defined(EGL_VERSION_1_2)
#  error At least EGL version 1.2 is required by the OpenVG render plugin.
#endif



const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( 0x200184B8, CHuiVg10RenderPlugin::NewL )
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

    
CHuiVg10RenderPlugin* CHuiVg10RenderPlugin::NewL()
    {
    CHuiVg10RenderPlugin* self = CHuiVg10RenderPlugin::NewLC();
    CleanupStack::Pop(self);
    return self;
    }


CHuiVg10RenderPlugin* CHuiVg10RenderPlugin::NewLC()
    {
    CHuiVg10RenderPlugin* self = new (ELeave) CHuiVg10RenderPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiVg10RenderPlugin::CHuiVg10RenderPlugin(THuiRenderPluginId aId)
        : CHuiRenderPlugin(aId)
    {
    }


void CHuiVg10RenderPlugin::ConstructL()
    {
    HUI_DEBUG(_L("CHuiVg10RenderPlugin::ConstructL() - Initializing Rendering plugin.") );
    
	iIsHardwareAccelerated = EFalse;
	iEGLStateForTextureUpload.iContext = KErrNotFound; // this will be defined later by the first upload in CHuiVg10Texture::PushEGLContext
    iEglDisplay = NULL; 
    iDefaultRenderSurface = NULL;


    // Restore (Create the EGL surfaces)
    RestoreL();

// The disable flag is defined in coretkoptions.mmh
#ifndef VG10_DISABLE_DEFAULT_RENDER_SURFACE
    HUI_DEBUG(_L("CHuiVg10RenderPlugin::ConstructL() - Create default PBuffer rendering surface for texture preloads.") );
    // Create default PBuffer rendering surface. This will allow us to for instance preload
    // and create textures before actual rendering surface exists.
    iDefaultRenderSurface = CHuiVg10PBufferSurface::NewLC(*this, TSize(64, 64));
    CleanupStack::Pop(iDefaultRenderSurface);
    iDefaultRenderSurface->MakeCurrent();
    iIsHardwareAccelerated = IsHardwareAccelerated();
#endif
    HUI_DEBUG(_L("CHuiVg10RenderPlugin::ConstructL() - Rendering plugin for OpenVG 1.0 ready.") );      

    // Initialize some "allow flags" which depend on used HW
    ReadAllowsSwapBufferPreserved();
    ReadAllowsVisualPBufferSurfaces();
    
    // Cache the Hardware configuration
    iHwConf = HuiUtil::GetValueFromCentralRepository( KUIAccelTKHWConfigurationFlags, iHwConf );
    }

CHuiVg10RenderPlugin::~CHuiVg10RenderPlugin()
    {
    Release();

#ifndef VG10_DISABLE_DEFAULT_RENDER_SURFACE
    if (iDefaultRenderSurface)
        {
        HUI_DEBUG(_L("CHuiVg10RenderPlugin::~CHuiVg10RenderPlugin() - Destroying default rendering surface."));
        // Destroy default rendering surface
        delete iDefaultRenderSurface; iDefaultRenderSurface = NULL;
        }
#endif

    }

void CHuiVg10RenderPlugin::RestoreL()
    {
    HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - Restoring renderer..") );

    // Get the display for drawing graphics.
    if (CHuiStatic::Env().GlesRefCounter() == 0)
        {
        HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - Getting default EGL Display.") );
        iEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(iEglDisplay == EGL_NO_DISPLAY)
            {
            HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - ERROR! No connection to EGL display was available."));
            TInt eglerr = eglGetError();
            switch (eglerr)
                {
                case EGL_BAD_ALLOC:
                    HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - Cause: Out of memory. Leaving."));
                    User::Leave(KErrNoMemory);
                default:
                    HUI_DEBUG2(_L("CHuiVg10RenderPlugin::RestoreL() - EGL information for the error: %S (EGL error code: %i). Leaving."), &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
                case EGL_SUCCESS:
                    User::Leave(KErrNotSupported);
                }
            }
        EGLint majorVersion=0;
        EGLint minorVersion=0;
        
        // Initialize display.
        if(eglInitialize(iEglDisplay, &majorVersion, &minorVersion) == EGL_FALSE)
            {
            TInt eglerr = eglGetError();
            switch (eglerr)
                {
                case EGL_BAD_ALLOC:
                    HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - Cause: Out of memory. Leaving."));
                    User::Leave(KErrNoMemory);
                default:
                    HUI_DEBUG2(_L("CHuiVg10RenderPlugin::RestoreL() - ERROR! Failed to initialize EGL display. Cause: %S (EGL error code: %i). Leaving."), &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
                case EGL_SUCCESS: // indicates that no error was recorded, but still the eglInitialize call failed (=> unable to continue!)
                case EGL_NOT_INITIALIZED:
                    User::Leave(KErrNotSupported);
                }
            }
        HUI_DEBUG2(_L("CHuiVg10RenderPlugin::RestoreL() - EGL Initialized. EGL Version %i.%i"), majorVersion, minorVersion); 
        CHuiStatic::Env().GlesRefCounter()++;

        // Indicate that we are going to use OpenVG for our drawing
        eglBindAPI(EGL_OPENVG_API);
        }

#ifndef VG10_DISABLE_DEFAULT_RENDER_SURFACE
    if (iDefaultRenderSurface != NULL)
        {
        HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - Restoring default rendering surface..") );
        iDefaultRenderSurface->RestoreL();
        iDefaultRenderSurface->MakeCurrent();
        }
#endif

    iStateFlags = 0;
#ifdef __NVG
    iNvgEngine = CNvgEngine::NewL();
    // Set the image binder for NVG-TLV special case "Group opacity"
    iMVGImageBinder = CHuiVg10VgImageBinder::NewL(this);
    iNvgEngine->SetVGImageBinder(iMVGImageBinder);
#endif
    HUI_DEBUG(_L("CHuiVg10RenderPlugin::RestoreL() - Renderer restored!") );

    }

void CHuiVg10RenderPlugin::Release()
    {
#ifndef VG10_DISABLE_DEFAULT_RENDER_SURFACE
    if (iDefaultRenderSurface != NULL)
        {
        HUI_DEBUG(_L("CHuiVg10RenderPlugin::Release() - Releasing default rendering surface..") );
        iDefaultRenderSurface->Release();
        }
#endif
    // Uninitialize EGL.
    if (--CHuiStatic::Env().GlesRefCounter() == 0)
        {
        HUI_DEBUG(_L("CHuiVg10RenderPlugin::Release() - Uninitializing EGL."));
        eglMakeCurrent(iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    	eglTerminate(iEglDisplay);
    	eglReleaseThread();
        iEglDisplay = 0;
        }
#ifdef __NVG
    delete iNvgEngine;
    iNvgEngine = NULL;
    delete iMVGImageBinder;
    iMVGImageBinder = NULL;
#endif
    HUI_DEBUG(_L("CHuiVg10RenderPlugin::Release() - Renderer released."));
    }


TBool CHuiVg10RenderPlugin::Allows(THuiRenderPluginAllow aAllow) const
    {
    switch(aAllow)
        {
        case EHuiRenderPluginAllowTextureModeInterpolate:
            // OpenVG does not support texture interpolation.
            return EFalse;
#ifdef VG10_DISABLE_DEFAULT_RENDER_SURFACE
        case EHuiRenderPluginAllowPBufferSurfaces:
            return EFalse;            
#endif            
        case EHuiRenderPluginAllowSwapBufferPreserve:
            return iAllowsSwapBufferPreserved;
        case EHuiRenderPluginAllowVisualPBufferSurfaces:
            return iAllowsVisualPBufferSurfaces;           
        default:
            return ETrue;
        }
    }


MHuiRenderSurface* CHuiVg10RenderPlugin::CreateRenderSurfaceL(CHuiDisplay& aDisplay)
    {
    return CHuiVg10RenderSurface::NewL(aDisplay);
    }


CHuiVg10PBufferSurface* CHuiVg10RenderPlugin::CreatePBufferSurfaceL(TSize aSize, TInt aEglBufferType, TInt aBufferColorMode)
    {
    CHuiVg10PBufferSurface* surface = CHuiVg10PBufferSurface::NewLC(*this, aSize, 0, aEglBufferType, aBufferColorMode);
    CleanupStack::Pop(surface);
    return surface;
    }


CHuiGc* CHuiVg10RenderPlugin::CreateGcL()
    {
    return CHuiVg10Gc::NewL();
    }


CHuiTexture* CHuiVg10RenderPlugin::CreateTextureL(const THuiTextureHandle* aExistingTexture)
    {
    return CHuiVg10Texture::NewL(*this, aExistingTexture);
    }

CHuiTextureManager* CHuiVg10RenderPlugin::CreateTextureManagerL(CHuiEnv& aEnv)
    {
    return CHuiVg10TextureManager::NewL(aEnv);
    }


CHuiMesh* CHuiVg10RenderPlugin::CreateMeshL(THuiMeshType aMeshType)
    {
    CHuiMesh* ret = NULL;
    
    switch(aMeshType)
    	{
    	default:
    		User::Leave(KErrNotSupported);
    		break;
    	}
    	
	return ret;    	
    }


CHuiTextMesh* CHuiVg10RenderPlugin::CreateTextMeshL()
    {
    return CHuiRasterizedTextMesh::NewL();
    }


CHuiCurvePath* CHuiVg10RenderPlugin::CreateCurvePathL()
    {
    CHuiVg10CurvePath* curve = new (ELeave) CHuiVg10CurvePath();
    CleanupStack::PushL(curve);
    curve->ConstructL();
    CleanupStack::Pop(curve);
    return curve;
    }


void CHuiVg10RenderPlugin::DeleteNamedTexture(TUint aName)
    {
    (void)aName;
    }


#ifndef VG10_DISABLE_DEFAULT_RENDER_SURFACE
void CHuiVg10RenderPlugin::NotifyDisplayCountL(TUint aCount)
    {
    if( aCount == 0 )
        {
        // No current display available in the environment.
        // Make default surface current to be able to continue resource loading.
        iDefaultRenderSurface->MakeCurrent();
        }
    }
#else
void CHuiVg10RenderPlugin::NotifyDisplayCountL(TUint /*aCount*/)
    {
    }
#endif

EGLConfig CHuiVg10RenderPlugin::EglDisplay()
    {
    return iEglDisplay;
    }


EGLContext CHuiVg10RenderPlugin::EglSharedContext() const
    {
    return iSharedContext;
    }


void CHuiVg10RenderPlugin::EglSetSharedContext(EGLContext aContext)
    {
    iSharedContext = aContext;
    }


TInt CHuiVg10RenderPlugin::EglChooseConfig(const TInt* aAttribList)
    {
    EGLint numOfConfigs = 0;
    if(eglChooseConfig(iEglDisplay, aAttribList, iConfigs, KVg10ConfigsCount,
                       &numOfConfigs) == EGL_FALSE)
        {
        return 0;
        }
    return numOfConfigs;
    }


TInt CHuiVg10RenderPlugin::EglConfig(TInt aIndex)
    {
    if(aIndex < 0 || aIndex >= KVg10ConfigsCount)
        {
        THuiPanic::Panic(THuiPanic::EEnvConfigNotFound);
        }
    return iConfigs[aIndex];
    }


void CHuiVg10RenderPlugin::EglPrintConfig(TInt aIndex)
    {
    if(aIndex < 0 || aIndex >= KVg10ConfigsCount)
        {
        THuiPanic::Panic(THuiPanic::EEnvConfigNotFound);
        }

    EGLint value;
    EGLConfig cfg = iConfigs[aIndex];

    CHuiStatic::Printf(_L("EGL Configuration %i:"), aIndex);

#ifdef WIN32
#define HUI_PRINT_EGL_ATTRIB(aAttr) \
    eglGetConfigAttrib(iEglDisplay, cfg, aAttr, &value); \
    CHuiStatic::Printf(_L(#aAttr " = %i (%04x)"), value, value);
#else
#define HUI_PRINT_EGL_ATTRIB(aAttr) \
    eglGetConfigAttrib(iEglDisplay, cfg, aAttr, &value); \
    CHuiStatic::Printf(_L("Parameter (id %04x):"), aAttr); \
    CHuiStatic::Printf(_L8(#aAttr)); \
    CHuiStatic::Printf(_L("Value: %i (%04x)"), value, value);
#endif
    HUI_PRINT_EGL_ATTRIB(EGL_CONFIG_ID);
    HUI_PRINT_EGL_ATTRIB(EGL_CONFIG_CAVEAT);
    HUI_PRINT_EGL_ATTRIB(EGL_SURFACE_TYPE);
    HUI_PRINT_EGL_ATTRIB(EGL_BUFFER_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_RED_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_GREEN_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_BLUE_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_ALPHA_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_DEPTH_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_LEVEL);
    HUI_PRINT_EGL_ATTRIB(EGL_MAX_PBUFFER_WIDTH);
    HUI_PRINT_EGL_ATTRIB(EGL_MAX_PBUFFER_HEIGHT);
    HUI_PRINT_EGL_ATTRIB(EGL_MAX_PBUFFER_PIXELS);
    HUI_PRINT_EGL_ATTRIB(EGL_NATIVE_RENDERABLE);
    HUI_PRINT_EGL_ATTRIB(EGL_NATIVE_VISUAL_ID);
    HUI_PRINT_EGL_ATTRIB(EGL_NATIVE_VISUAL_TYPE);
    HUI_PRINT_EGL_ATTRIB(EGL_SAMPLE_BUFFERS);
    HUI_PRINT_EGL_ATTRIB(EGL_SAMPLES);
    HUI_PRINT_EGL_ATTRIB(EGL_STENCIL_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_TRANSPARENT_TYPE);
#ifdef EGL_VERSION_1_1
    HUI_PRINT_EGL_ATTRIB(EGL_BIND_TO_TEXTURE_RGB);
    HUI_PRINT_EGL_ATTRIB(EGL_BIND_TO_TEXTURE_RGBA);
    HUI_PRINT_EGL_ATTRIB(EGL_MAX_SWAP_INTERVAL);
    HUI_PRINT_EGL_ATTRIB(EGL_MIN_SWAP_INTERVAL);
#endif
#ifdef EGL_VERSION_1_2
    HUI_PRINT_EGL_ATTRIB(EGL_PRESERVED_RESOURCES);
    HUI_PRINT_EGL_ATTRIB(EGL_LUMINANCE_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_ALPHA_MASK_SIZE);
    HUI_PRINT_EGL_ATTRIB(EGL_COLOR_BUFFER_TYPE);
    HUI_PRINT_EGL_ATTRIB(EGL_RENDERABLE_TYPE);
#endif
    
#undef HUI_PRINT_EGL_ATTRIB
    }


TBool CHuiVg10RenderPlugin::IsReleased() const
    {
    if(CHuiStatic::Env().GlesRefCounter() == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

const TDesC& CHuiVg10RenderPlugin::VgErrorMessage(VGErrorCode aErrorCode)
    {
    // Error messages.
    _LIT(KGlesErrorUnknown, "Invalid or unknown error.");

    switch(aErrorCode)
        {
        default:
            return KGlesErrorUnknown;
        }
    }


VGErrorCode CHuiVg10RenderPlugin::VgError()
    {
    VGErrorCode error = vgGetError();
#ifdef _DEBUG
    // Print the vgError always in debug mode, not just when HUI_DEBUG is in use 
    if (error != VG_NO_ERROR)
        {
        RDebug::Print(_L("CHuiVg10RenderPlugin::VgError -- %04x"), error);
        }
#endif
    return error;
    }


TBool CHuiVg10RenderPlugin::IsHardwareAccelerated()
	{
	// consider vg10 plugin as HW accelerated until a better check

//	TBool HWFound = EFalse;
	iIsHardwareAccelerated = ETrue;
	return ETrue;		
	}

CHuiCanvasGc* CHuiVg10RenderPlugin::CreateCanvasGcL()
    {
    return new (ELeave) CHuiVg10CanvasGc();        
    }

CHuiFxEngine* CHuiVg10RenderPlugin::CreateEffectsEngineL()
    {
    return CHuiFxVg10Engine::NewL(*this);
    }

void CHuiVg10RenderPlugin::AddRestoreStateFlags(TInt aFlags)
    {
    iStateFlags |= aFlags;
    }

TInt CHuiVg10RenderPlugin::GetRestoreStateFlags()
    {
    return iStateFlags;
    }

void CHuiVg10RenderPlugin::ClearRestoreStateFlags()
    {
    iStateFlags &= 0x00000000;
    }

#ifdef __NVG
CNvgEngine& CHuiVg10RenderPlugin::NvgEngine() const
    {
    return *iNvgEngine;
    }
#endif

TInt CHuiVg10RenderPlugin::GetHwConfigurationFlags() const
    {
    return iHwConf;
    }

// TODO: This is only a temporary check until all HW platforms support feature
void CHuiVg10RenderPlugin::ReadAllowsSwapBufferPreserved()
    {    
#ifdef __WINS__ 
    iAllowsSwapBufferPreserved = ETrue;
#else
    const char* vendor = eglQueryString(iEglDisplay, EGL_VENDOR);    
    //const VGubyte* vendor = vgGetString(VG_VENDOR);    
    //RDebug::Print(_L("vendor = %s"), vendor);    

    TBool found = EFalse;
    if( vendor[0] != 0 && vendor[0] == 'B' && vendor[1] == 'r')
        {
        RDebug::Print(_L("CHuiVg10RenderPlugin::ReadAllowsSwapBufferPreserved() -- ETrue"));    
        found = ETrue;
        }
    else
        {
        RDebug::Print(_L("CHuiVg10RenderPlugin::ReadAllowsSwapBufferPreserved() -- EFalse"));            
        }
    iAllowsSwapBufferPreserved = found;
#endif
    }

// TODO: This is only a temporary check until all HW platforms support feature
void CHuiVg10RenderPlugin::ReadAllowsVisualPBufferSurfaces()
    {
#ifdef __WINS__ 
    iAllowsVisualPBufferSurfaces = ETrue;
#else
    const char* vendor = eglQueryString(iEglDisplay, EGL_VENDOR);    
    //const VGubyte* vendor = vgGetString(VG_VENDOR);    
    //RDebug::Print(_L("vendor = %s"), vendor);

    TBool found = EFalse;
    if( vendor[0] != 0 && vendor[0] == 'B' && vendor[1] == 'r')
        {
        RDebug::Print(_L("CHuiVg10RenderPlugin::ReadAllowsVisualPBufferSurfaces() -- ETrue"));    
        found = ETrue;
        }
    else
        {
        RDebug::Print(_L("CHuiVg10RenderPlugin::ReadAllowsVisualPBufferSurfaces() -- EFalse"));            
        }
    iAllowsVisualPBufferSurfaces = found;
#endif
    }

