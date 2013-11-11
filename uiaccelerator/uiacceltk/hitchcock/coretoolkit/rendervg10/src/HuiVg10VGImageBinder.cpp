/*
 * Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   Class HuiVg10VGImageBinder
 *
 */

#include "HuiVg10VgImageBinder.h"
#include "HuiVg10RenderPlugin.h"
#include "uiacceltk/HuiUtil.h"

CHuiVg10VgImageBinder::CHuiVg10VgImageBinder(CHuiVg10RenderPlugin* aRenderPlugin):
iRenderPlugin(aRenderPlugin)
        {
        }

void CHuiVg10VgImageBinder::ConstructL()
    {
    }

CHuiVg10VgImageBinder::~CHuiVg10VgImageBinder()
    {
    if (iEglPBufferSurface_Client)
        {
        eglDestroySurface( iRenderPlugin->EglDisplay(), iEglPBufferSurface_Client );
        iEglPBufferSurface_Client = EGL_NO_SURFACE;
        }
    // Not owned, don't delete.
    iRenderPlugin = NULL;
    }

CHuiVg10VgImageBinder* CHuiVg10VgImageBinder::NewL(CHuiVg10RenderPlugin* aRenderPlugin)
    {   
    CHuiVg10VgImageBinder* self = new (ELeave) CHuiVg10VgImageBinder(aRenderPlugin);
    self->ConstructL();
    return self;
    }

TInt CHuiVg10VgImageBinder::BindClientBuffer(TUint aBuffer)
    {

    // This returns the index of the corresponding aBuffer stored in the array. 
    // If KErrNotFound is returned,it indicates that this is the first BindClientBuffer
    // call for aBuffer and hence eglPbufferfromclient has to be created for this buffer
    TInt bufferIndex = iGroupOpacityImages.Find(aBuffer);
    
    
    // This check mandates that iSavedDraw/Read Surfaces are stored only for the first time
    // (i.e., before any pbufferfromclient surfaces are created).This is because when there are concurrent 
    // BindToImageL calls,we would eventually be losing track of the base window surface on
    // top of which the vgImage has to be drawn. 
    if(iGroupOpacityImages.Count() == 0)							
        {
        // Save current context and surfaces
        iSavedContext = eglGetCurrentContext();
        iSavedDrawSurface = eglGetCurrentSurface(EGL_DRAW);
        iSavedReadSurface = eglGetCurrentSurface(EGL_READ);
        }

    // Buffer Index would be KErrNotFound if this is the first BindClientBuffer call for aBuffer
    // (there would be multiple BindClientBuffer calls for an aBuffer) and hence corresponding
    // pbufferfromclient surface has to be created for that aBuffer.    
    if(bufferIndex == KErrNotFound)
        {
        // Check whether we should use the Alpha format bit
        VGImageFormat imageFormat = (VGImageFormat)vgGetParameteri(aBuffer, VG_IMAGE_FORMAT);
        TInt maskBit = 0;
        if (imageFormat == VG_sRGBA_8888_PRE)
            {
            maskBit = EGL_VG_ALPHA_FORMAT_PRE_BIT;
            }

        const TInt BITS_PER_CHANNEL = 8;
        // Choose an EGL config
        const EGLint attrs[] =
            {
            EGL_RENDERABLE_TYPE,    EGL_OPENVG_BIT,
            EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT | maskBit,
            EGL_RED_SIZE,           BITS_PER_CHANNEL,
            EGL_GREEN_SIZE,         BITS_PER_CHANNEL,
            EGL_BLUE_SIZE,          BITS_PER_CHANNEL,
            EGL_ALPHA_SIZE,         BITS_PER_CHANNEL,
            EGL_NONE
            };

        // Create a context
        TInt configCount = iRenderPlugin->EglChooseConfig(attrs);
        EGLConfig config = iRenderPlugin->EglConfig(0);

        // Create a pbuffer surface
        iEglPBufferSurface_Client = eglCreatePbufferFromClientBuffer(iRenderPlugin->EglDisplay(),
                EGL_OPENVG_IMAGE, 
                static_cast<EGLClientBuffer>(aBuffer),    // Use the param image as buffer
                config, NULL);
        if (iEglPBufferSurface_Client == EGL_NO_SURFACE)
            {
            HUI_DEBUG1(_L("CHuiVg10VgImageBinder::BindClientBuffer() - EGL Surface could not be created, eglErr: %04x"), eglGetError() );
            return KErrGeneral;
            }
        iGroupOpacitySurfaces.Append(iEglPBufferSurface_Client);
        iGroupOpacityImages.Append(aBuffer);
        } 
        // Control would go to else part indicating that this is not the first BindClientBuffer for aBuffer 
        // and hence the corresponding eglPBufferfromClient surface could be retrieved with the bufferIndex
    else				
        {
        iEglPBufferSurface_Client = iGroupOpacitySurfaces[bufferIndex];
        }

    EGLContext context = iRenderPlugin->EglSharedContext();
    
    // eglMakeCurrent with EGL_NO_SURFACE de-couples vgImage from an old eglpbufferfromclient surface.
    // Otherwise in a multiple BindClientBuffer scenario for the same vgImage, eglMakeCurrent
    // fails with an EGL_BAD_ACCESS error (vgimage already inuse error)
    eglMakeCurrent(iRenderPlugin->EglDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, context);      
    
    // Bind our own PBuffer surface (from VGImage)
    if ( eglMakeCurrent(iRenderPlugin->EglDisplay(), iEglPBufferSurface_Client, iEglPBufferSurface_Client, context ) == EGL_FALSE )
        {
        HUI_DEBUG1(_L("CHuiVg10VgImageBinder::BindClientBuffer() - EGL Surface could not be made current, eglErr: %04x"), eglGetError());
        TInt eglError = eglGetError();
        return KErrGeneral;
        }

    // Alles in Ordnung!
    return KErrNone;
    }

TInt CHuiVg10VgImageBinder::UnBindClientBuffer()
    {
    EGLContext context = iRenderPlugin->EglSharedContext();
    
    // eglMakeCurrent with EGL_NO_SURFACE de-couples vgImage from an old eglpbufferfromclient surface.
    // Otherwise in a multiple BindClientBuffer scenario for the same vgImage, eglMakeCurrent
    // fails with an EGL_BAD_ACCESS error (vgimage already inuse error)
    eglMakeCurrent(iRenderPlugin->EglDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, context);
	
    // iSavedDrawSurface and iSavedReadSurface would be the window surface on top of which the
    // group opacity vgImages(aBuffers) would have to be drawn. This is the reason why we store
    // iSavedDrawSurface only once at the start of BindClientBuffer routine.
    if ( eglMakeCurrent(iRenderPlugin->EglDisplay(), iSavedDrawSurface, iSavedReadSurface, context) == EGL_FALSE )
        {
        HUI_DEBUG1(_L("CHuiVg10VgImageBinder::BindClientBuffer() - EGL Surface could not be made current, eglErr: %04x"), eglGetError());
        return KErrGeneral;
        }

    // When the final UnBindClientBuffer is called and the vgimage has to be drawn on to the window surface.
    // We would lose the handles if these iSaved surfaces are set to zero. 
//    iSavedDrawSurface = 0;
//    iSavedReadSurface = 0;
//    iSavedContext = 0;

    if (iEglPBufferSurface_Client)
        {
        TInt bufferIndex = iGroupOpacitySurfaces.Find(iEglPBufferSurface_Client);
        iGroupOpacitySurfaces.Remove(bufferIndex);
        iGroupOpacityImages.Remove(bufferIndex);
        eglDestroySurface( iRenderPlugin->EglDisplay(), iEglPBufferSurface_Client );
        iEglPBufferSurface_Client = EGL_NO_SURFACE;
        }

    // Everything went fine
    return KErrNone;
    }

// End of file

