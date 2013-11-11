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
* Description:   Implementation of CHuiVg10PBufferSurface. CHuiGles10PBufferSurface is
*                an OpenVG PBuffer surface object that implements the MHuiRenderSurface
*                interface.
*
*/



#include "HuiVg10RenderPlugin.h"
#include "HuiVg10PBufferSurface.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiSegmentedTexture.h"
#include <EGL/egl.h>

#include <coemain.h>

struct THuiColorModeData
	{
	TInt iColorMode;
	TInt iBitCount;
	TInt iRed;
	TInt iGreen;
	TInt iBlue;
	TInt iAlpha;
	};
	
const THuiColorModeData KHuiColorModeData[] =
		{ 
			{0, EGL_DONT_CARE, 0, 0, 0, 8}, // Do not use ENone as there might be many of those in headers
			{EColor4K, 	 12, 4, 4, 4, 0},
			{EColor64K,  16, 5, 6, 5, 0},
			{EColor16M,  24, 8, 8, 8, 0},
			{EColor16MU, 32, 8, 8, 8, 0},
#ifndef __NVG
			{EColor16MA, 32, 8, 8, 8, 8}
#else
			{EColor16MA, 32, 8, 8, 8, 8},	
			{EColor16MAP, 32, 8, 8, 8, 8}	
#endif			
		};


CHuiVg10PBufferSurface* CHuiVg10PBufferSurface::NewLC(CHuiVg10RenderPlugin& aRenderer, 
                                                          const TSize& aSize, 
                                                          TInt aAlphaBits, 
                                                          TInt aEglBufferType, 
                                                          TInt aBufferColorMode)
    {
    CHuiVg10PBufferSurface* self = new (ELeave) CHuiVg10PBufferSurface(
    	aRenderer, aSize, aAlphaBits, aEglBufferType, aBufferColorMode);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiVg10PBufferSurface* CHuiVg10PBufferSurface::NewLC(CHuiVg10RenderPlugin& aRenderer, 
                                                          const TSize& aSize, 
                                                          TInt aAlphaBits)
    {
    return NewLC(aRenderer, aSize, aAlphaBits, EGL_PBUFFER_BIT, 0);
    }


CHuiVg10PBufferSurface* CHuiVg10PBufferSurface::NewL(const TSize& aSize, TInt aAlphaBits)
    {
    CHuiVg10PBufferSurface* self = CHuiVg10PBufferSurface::NewLC(aSize, aAlphaBits);
    CleanupStack::Pop(self);
    return self;
    }


CHuiVg10PBufferSurface* CHuiVg10PBufferSurface::NewLC(const TSize& aSize, TInt aAlphaBits)
    {
    CHuiVg10RenderPlugin& render = CHuiStatic::Vg10Renderer();
    return NewLC(render, aSize, aAlphaBits);
    }


CHuiVg10PBufferSurface::CHuiVg10PBufferSurface(CHuiVg10RenderPlugin& aRenderer,
                                                   const TSize& aSize,
                                                   TInt aAlphaBits, 
                                                   TInt aEglBufferType, 
                                                   TInt aBufferColorMode)
        : iSize(aSize), iEglBufferType(aEglBufferType), iBufferColorMode(aBufferColorMode), 
          iAlphaBits(aAlphaBits),iRenderer(aRenderer)
    {
    }


void CHuiVg10PBufferSurface::ConstructL()
    {
    HUI_DEBUG(_L("CHuiVg10PBufferSurface::ConstructL() - Constructing OpenVG 1.0 PBuffer Rendering surface."));

    // Create egl context and rendering surface
    iEglContext = iEglSurface  = NULL;
    iIsHardwareAccelerated = EFalse;
    RestoreL();

    HUI_DEBUG(_L("CHuiVg10PBufferSurface::ConstructL() - Construction exited OK."));
    }


CHuiVg10PBufferSurface::~CHuiVg10PBufferSurface()
    {
    Release();
    }


TBool CHuiVg10PBufferSurface::GetScreenOrigin(TPoint& /*aOrigin*/) const
    {
    // This class does only off-screen rendering, so the on-screen origin
    // cannot be determined.
    return EFalse;
    }


EGLConfig CHuiVg10PBufferSurface::DetermineConfig()
    {    
    HUI_DEBUG1(_L("CHuiVg10PBufferSurface::DetermineConfig() enter. iBufferColorMode=%d"), iBufferColorMode);
    // Define properties for the wanted EGLSurface. To get the best possible
    // performance, choose an EGLConfig with a buffersize matching
    // the current window's display mode.

    // Determine the buffer size
#ifdef __WINSCW__
	if (iBufferColorMode == ENone)
		{
		
    	// Determine the natively supported display mode
        CWsScreenDevice* screenDevice = CHuiStatic::ScreenDevice();
        if(screenDevice)
           	{
           	screenDevice->DisplayMode();
           	}
        }
#endif

	// Find color mode data
    THuiColorModeData cmData = KHuiColorModeData[0];
    //Mem::Copy(&cmData, (&KHuiColorModeData[0]), sizeof(THuiColorModeData)); // default mode
    for (TInt colorModeIndex = 0; 
    	 colorModeIndex < (sizeof(KHuiColorModeData) / sizeof(THuiColorModeData)); 
    	 colorModeIndex++)
    	 	{
    	 	if (iBufferColorMode == KHuiColorModeData[colorModeIndex].iColorMode)
    	 		{
    	 		Mem::Copy(&cmData, &(KHuiColorModeData[colorModeIndex]), sizeof(THuiColorModeData));
    	 		}
    	 	}
    
	// Define properties for the wanted EGLSurface.
	EGLint attribListDefault[15];  // reserve enough space for configurations
	int tmp = 0;
	
    attribListDefault[tmp++] = EGL_BUFFER_SIZE;
    attribListDefault[tmp++] = cmData.iBitCount;
        
    attribListDefault[tmp++] = EGL_SURFACE_TYPE;
    attribListDefault[tmp++] = iEglBufferType;

    attribListDefault[tmp++] = EGL_RENDERABLE_TYPE;
    attribListDefault[tmp++] = EGL_OPENVG_BIT;
    
    attribListDefault[tmp++] = EGL_ALPHA_SIZE;
    attribListDefault[tmp++] = cmData.iAlpha;

    /*
#ifndef __WINSCW__
	if( iIsHardwareAccelerated )
		{		
        // Emulator renders in software, so it does not antialias.
        // MBX is efficient in supersampling.
		attribListDefault[tmp++] = EGL_SAMPLE_BUFFERS;
		attribListDefault[tmp++] = 1;

		attribListDefault[tmp++] = EGL_SAMPLES;
		attribListDefault[tmp++] = 4;	
		}
#endif // __WINSCW__
*/

	// Terminate attribute list
	attribListDefault[tmp++] = EGL_NONE;
         



	// Define properties for the wanted EGLSurface.
	EGLint attribListCustom[25];  // reserve enough space for configurations
	tmp = 0;

	attribListCustom[tmp++] = EGL_BUFFER_SIZE;
	attribListCustom[tmp++] = cmData.iBitCount;
	
	attribListCustom[tmp++] = EGL_RED_SIZE;
	attribListCustom[tmp++] = cmData.iRed;
	
    attribListCustom[tmp++] = EGL_GREEN_SIZE;
    attribListCustom[tmp++] = cmData.iGreen;
        
	attribListCustom[tmp++] = EGL_BLUE_SIZE;
	attribListCustom[tmp++] = cmData.iBlue;
	
	attribListCustom[tmp++] = EGL_RENDERABLE_TYPE;
	attribListCustom[tmp++] = EGL_OPENVG_BIT;
        
	attribListCustom[tmp++] = EGL_SURFACE_TYPE;
	attribListCustom[tmp++] = iEglBufferType;

    attribListCustom[tmp++] = EGL_ALPHA_SIZE;
    attribListCustom[tmp++] = cmData.iAlpha;
	
/*
	if( iIsHardwareAccelerated )
		{		
        // Emulator renders in software, so it does not antialias.
        // MBX is efficient in supersampling.
		attribListCustom[tmp++] = EGL_SAMPLE_BUFFERS;
		attribListCustom[tmp++] = 1;

		attribListCustom[tmp++] = EGL_SAMPLES;
		attribListCustom[tmp++] = 4;
		}
*/
	// Terminate attribute list
	attribListCustom[tmp++] = EGL_NONE;


    // Choose an EGLConfig that best matches to the properties in attribList.
    TInt numOfConfigs = 0;
	if (iBufferColorMode == 0)
		{
		// No explicit color mode. Vg chooses the best mode.
		numOfConfigs = iRenderer.EglChooseConfig(attribListDefault);
		}
	else
		{
		numOfConfigs = iRenderer.EglChooseConfig(attribListCustom);
		}
		
    if(!numOfConfigs)
        {
#ifdef _DEBUG
        TInt eglerr = eglGetError();
        HUI_DEBUG2(_L("CHuiVg10PBufferSurface::DetermineConfig() - eglChooseConfig failed, no required EGL configuration found. Cause: %S (error %i). Panicking."), &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
#endif
        HUI_PANIC(THuiPanic::ERenderSurfaceUnknownDisplayMode)
        }

    HUI_DEBUG1(_L("CHuiVg10PBufferSurface::DetermineConfig() - Got %i configs."), numOfConfigs);

    // Print the attributes of the selected config to log file.
    iRenderer.EglPrintConfig(0);

	if (iBufferColorMode == 0)
		{
		// Set color mode based on parameters
		
		// Update color mode struct
    	TInt cfg = iRenderer.EglConfig(0);
    	eglGetConfigAttrib(iEglDisplay, cfg, EGL_BUFFER_SIZE, 	&(cmData.iBitCount)); 
    	eglGetConfigAttrib(iEglDisplay, cfg, EGL_RED_SIZE, 		&(cmData.iRed)); 
    	eglGetConfigAttrib(iEglDisplay, cfg, EGL_GREEN_SIZE, 	&(cmData.iGreen)); 
    	eglGetConfigAttrib(iEglDisplay, cfg, EGL_BLUE_SIZE, 	&(cmData.iBlue)); 
    	eglGetConfigAttrib(iEglDisplay, cfg, EGL_ALPHA_SIZE, 	&(cmData.iAlpha)); 

		// Find color mode from array
    	for (TInt colorModeIndex = 0; 
    	 	colorModeIndex < (sizeof(KHuiColorModeData) / sizeof(THuiColorModeData)); 
    	 	colorModeIndex++)
    		{
    	 	if (
    	 		(cmData.iBitCount == KHuiColorModeData[colorModeIndex].iBitCount) &&
    	 		(cmData.iRed == KHuiColorModeData[colorModeIndex].iRed) &&
    	 		(cmData.iGreen == KHuiColorModeData[colorModeIndex].iGreen) &&
    	 		(cmData.iBlue == KHuiColorModeData[colorModeIndex].iBlue) &&
    	 		(cmData.iAlpha == KHuiColorModeData[colorModeIndex].iAlpha) )
    	 		{
    	 		iBufferColorMode = KHuiColorModeData[colorModeIndex].iColorMode;
    	 		}
    	 	}		
		}

    // Choose the best EGLConfig. EGLConfigs returned by eglChooseConfig
    // are sorted so that the best matching EGLConfig is first in the list.
    return iRenderer.EglConfig(0);
    }


EGLSurface CHuiVg10PBufferSurface::CreateSurface(EGLConfig aConfig)
    {
    EGLSurface surface = EGL_NO_SURFACE;
    if (iEglBufferType == EGL_PBUFFER_BIT)
    	{
    	// Create a Pbuffer surface.
    	EGLint pbufferAttribList[] =
        	{
        	EGL_WIDTH,      iSize.iWidth,
        	EGL_HEIGHT,     iSize.iHeight,
        	EGL_NONE
        	};

    	HUI_DEBUG(_L("CHuiVg10PBufferSurface::CreateSurface() - Attempting to create a PBuffer surface."));  
    	surface = eglCreatePbufferSurface(iEglDisplay, aConfig, pbufferAttribList);
    	}

    else if (iEglBufferType == EGL_PIXMAP_BIT)
    	{
    	delete(iBuffer);
    	iBuffer = NULL;    
    	iBuffer = new CWsBitmap( CHuiStatic::WsSession() );
    	if (iBuffer && (iBufferColorMode > 0))
    		{
    		iBuffer->Create( iSize, (TDisplayMode)iBufferColorMode);
    		HUI_DEBUG(_L("CHuiVg10PBufferSurface::CreateSurface() - Attempting to create a Pixmap surface."));
    		surface = eglCreatePixmapSurface(iEglDisplay, aConfig, iBuffer, NULL);
    		}
    	}
    else
        {
        // For PC-lint    
        }
    	
    return surface;
    }


TSize CHuiVg10PBufferSurface::Size() const
    {
    return iSize;
    }


void CHuiVg10PBufferSurface::SetSizeL(const TSize& aSize)
    {
    iSize = aSize;

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
                HUI_DEBUG2(_L("CHuiVg10PBufferSurface::SetSizeL() - ERROR! Failed to (re)create surface having size (%ix%i). Out of memory."), 
                           aSize.iWidth, aSize.iHeight);
                User::Leave(KErrNoMemory);
            default:
                HUI_DEBUG4(_L("CHuiVg10PBufferSurface::SetSizeL() - ERROR! Failed to (re)create surface having size (%ix%i).Cause: %S (EGL error code: %i). Leaving."), 
                            aSize.iWidth, aSize.iHeight, 
                            &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            case EGL_SUCCESS: // indicates that no error was recorded, but still the CreateSurface call failed (=> unable to continue!)
                User::Leave(KErrNotSupported);
            }
        }

    MakeCurrent();
    }


void CHuiVg10PBufferSurface::MakeCurrent()
    {
	if (iEglSurface == NULL || iEglContext == NULL)
	{
		// we can't makeCurrent on a released surface
		return;
	}
    
	// No need to change the current render surface if this render surface is current already.
	if(CHuiStatic::CurrentRenderSurface() != ((MHuiRenderSurface*)this))
	    {
        eglMakeCurrent(iEglDisplay, iEglSurface, iEglSurface, iEglContext);
        
        TInt eglerr = eglGetError();
        if(eglerr != EGL_SUCCESS)
            {
            HUI_DEBUG2(_L("CHuiVg10PBufferSurface::MakeCurrent() - Could not switch this rendering surface on. Cause: %S (EGL error code: %i). Panicking."),
                       &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            HUI_PANIC(THuiPanic::ERenderSurfaceActivationFailed)
            }
        CHuiStatic::SetCurrentRenderSurface(this);	    
	    }    
    }


void CHuiVg10PBufferSurface::SwapBuffers()
    {
    // Pbuffers consist of a single buffer only (no front/back buffers to swap).
    }

void CHuiVg10PBufferSurface::Finish()
    {
    vgFinish();
    }

void CHuiVg10PBufferSurface::BindTexture(TInt aTextureUnit,
                                           const MHuiSegmentedTexture& aTexture,
                                           TInt aSegment)
    {
    (void)aTextureUnit;
    (void)aTexture;
    (void)aSegment;
    }

void CHuiVg10PBufferSurface::HandleVisibilityEvent(TBool /* aIsVisible */ )
    {
    // dummy method, does not handle the events in any way..
    }

void CHuiVg10PBufferSurface::Release()
    {
 
    HUI_DEBUG( _L("CHuiVg10PBufferSurface::Release() - Releasing EGL surface and context..") );
    if (iEglSurface != NULL)
        {
        eglDestroySurface(iEglDisplay, iEglSurface);
        TInt eglerr = eglGetError();
        if(eglerr != EGL_SUCCESS)
            {
            HUI_DEBUG2(_L("CHuiVg10PBufferSurface::Release() - WARNING! Could not destroy EGL surface. Cause: %S (EGL error code: %i)."),
                       &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            }
        iEglSurface = NULL;
        }
     
    // Delete pixmap if exists    
    delete(iBuffer);
    iBuffer = NULL;    
        
    if (iEglContext != NULL)
        {
        // Unshare this context if it was
        // set as shared..
        if(iRenderer.EglSharedContext() == iEglContext)
            {
            HUI_DEBUG(_L("CHuiVg10PBufferSurface::Release() - Unsharing this render context."));
            iRenderer.EglSetSharedContext(EGL_NO_CONTEXT);
            }

        eglDestroyContext(iEglDisplay, iEglContext);
        TInt eglerr = eglGetError();
        if(eglerr != EGL_SUCCESS)
            {
            HUI_DEBUG2(_L("CHuiVg10PBufferSurface::Release() - WARNING! Could not destroy EGL context. Cause: %S (EGL error code: %i)."),
                       &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            }
        iEglContext = NULL;
        }

    }

void CHuiVg10PBufferSurface::RestoreL()
    {

    HUI_DEBUG( _L("CHuiVg10PBufferSurface::RestoreL() - Restoring render surface.") );

    if((iEglSurface != NULL) && (iEglContext != NULL))
        {
        HUI_DEBUG( _L("CHuiVg10RenderSurface::RestoreL() - Render surface and context already in place, no need to restore.") );
        }
    iEglDisplay = iRenderer.EglDisplay();

    // Create an EGL rendering surface.
    EGLConfig config = DetermineConfig();
    iEglSurface = CreateSurface(config);
    if(iEglSurface == EGL_NO_SURFACE)
        {
        TInt eglerr = eglGetError();
        switch (eglerr)
            {
            case EGL_BAD_ALLOC:
                HUI_DEBUG2(_L("CHuiVg10PBufferSurface::RestoreL() - ERROR! Failed to create pbuffer surface having size (%ix%i). Out of memory."), 
                           iSize.iWidth, iSize.iHeight);
                User::Leave(KErrNoMemory);
            default:
                HUI_DEBUG4(_L("CHuiVg10PBufferSurface::RestoreL() - ERROR! Failed to create pbuffer surface having size (%ix%i). Cause: %S (EGL error code: %i)."), 
                            iSize.iWidth, iSize.iHeight, 
                            &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            case EGL_SUCCESS: // indicates that no error was recorded, but still the CreateSurface call failed (=> unable to continue!)
                User::Leave(KErrNotSupported);
            }
        }

    // Create rendering context for PBuffer rendering.
    // There is only a single texture manager in the environment, so textures
    // need to shared between all contexts.
    iEglContext = eglCreateContext(iEglDisplay, config,
                                   iRenderer.EglSharedContext(), NULL);
    if(iEglContext == EGL_NO_CONTEXT)
        {
        TInt eglerr = eglGetError();
        switch (eglerr)
            {
            case EGL_BAD_ALLOC:
                HUI_DEBUG(_L("CHuiVg10PBufferSurface::RestoreL() - ERROR! Failed to create rendering context. Out of memory."));
                User::Leave(KErrNoMemory);
            default:
                HUI_DEBUG2(_L("CHuiVg10PBufferSurface::RestoreL() - ERROR! Failed to create rendering context. Cause: %S (EGL error code: %i)."),                            
                            &CHuiVg10RenderSurface::EglErrorMessage(eglerr), eglerr);
            case EGL_SUCCESS: // indicates that no error was recorded, but still the CreateSurface call failed (=> unable to continue!)
                User::Leave(KErrNotSupported);
            }
        }

    // Share this context if there was not already a shared context for texture
    // management
    if(iRenderer.EglSharedContext() == EGL_NO_CONTEXT)
        {
        HUI_DEBUG(_L("CHuiVg10PBufferSurface::ConstructL() - Making this surface shared."));
        // Now that there is a context, make sure the renderer knows of it.
        iRenderer.EglSetSharedContext(iEglContext);
        }
	eglMakeCurrent(iEglDisplay, iEglSurface, iEglSurface, iEglContext);
    }

EGLSurface CHuiVg10PBufferSurface::EglSurface()
    {
    return iEglSurface;
    }

void CHuiVg10PBufferSurface::BindToM3G()
	{
	}

void CHuiVg10PBufferSurface::ReleaseFromM3G()
	{
	}
	
CFbsBitmap* CHuiVg10PBufferSurface::Buffer()
	{
	return iBuffer;
	}
    
TInt CHuiVg10PBufferSurface::BufferColorMode() const
	{
	return iBufferColorMode;
	}

TUint CHuiVg10PBufferSurface::Flags() const
	{
	// ToDo: Is some flag needed?
	return MHuiRenderSurface::EFlagNone;
	}

void CHuiVg10PBufferSurface::HandleDisplayUsageChangeL()
    {
    // Nothing to do
    }
	
void CHuiVg10PBufferSurface::SetDirtyRect(const TRect& /*aRect*/)
    {
    // Nothing to do
    }
    
MHuiTargetBitmap* CHuiVg10PBufferSurface::BackBuffer()
    {
    // Nothing to do
    return NULL;
    }

CFbsBitGc* CHuiVg10PBufferSurface::Gc()
    {
    // Nothing to do
    return NULL;
    }

void CHuiVg10PBufferSurface::RenderSurfaceExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    *aExtensionParams = NULL;
    }
