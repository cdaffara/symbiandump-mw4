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
* Description:  CHWVGSurfaceImpl header file
 *
*/

#ifndef HWVGSURFACEIMPL_H_
#define HWVGSURFACEIMPL_H_

#include <e32base.h>

#include "MVGSurfaceImpl.h"

#ifdef HWRENDERER
#include <EGL\egl.h>
#endif

class CHWVGSurfaceImpl : public CBase, public MVGSurfaceImpl
{
public:
    enum HWSurfaceType
        {
        SURFACE_PBUFFER = 0,
        SURFACE_WINDOW = 1,
        SURFACE_PIXMAP = 2
        };
public:
    static CHWVGSurfaceImpl* NewL(TUint8 aOption);
    static CHWVGSurfaceImpl* NewLC(TUint8 aOption);
    
    virtual ~CHWVGSurfaceImpl();

    virtual TInt InitializeSurface(TSize aSize, TInt aColorSpace);
    
    virtual TInt CreateSurface(TInt aDisplayMode, RWindow *aSurface, CFbsBitmap *aBitmap);
    
    virtual TInt ResizeSurface(TSize aSize);
    
    virtual TInt CopyBitmap(TInt aDisplayMode,TInt aMaskMode, CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize aSize = TSize(0,0));

    virtual TInt PrepareToBindClientBuffer();
    virtual TInt BindClientBuffer(TInt buffer);
      
    virtual TInt UnBindClientBuffer();

    virtual void TerminateSurface();
    
    virtual void SetConfiguration(TInt aOption, const TAny* aValue);

    
private:
    CHWVGSurfaceImpl(TUint8 aOption);
    void ConstructL();
    TInt CreatePBufferSurface();
    TInt MapEGLErrorCodeToSymbian(TInt aErrorCode);
    
    TUint8          iSurfaceType;
    TSize           iSize;
#ifdef HWRENDERER
    EGLDisplay      iEglDisplay;
    EGLSurface      iEglSurface;
    EGLSurface      iEglPBufferSurface_Client;
    EGLContext      iEglContext;
    EGLConfig       iConfig; 
#endif
};

#endif /*HWVGSURFACEIMPL_H_*/
