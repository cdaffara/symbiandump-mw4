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



#ifndef HUIFXVG10ONSCREENRENDERBUFFER_H_
#define HUIFXVG10ONSCREENRENDERBUFFER_H_

#include "HuiFxVg10RenderbufferBase.h"

//FORWARD DECLARATIONS
class CHuiVg10RenderPlugin;
class CHuiVg10RenderSurface;
class CHuiVg10PBufferSurface;

/**
 *  A renderbuffer that uses an EGL surface for storage.
 */

class CHuiFxVg10OnscreenRenderbuffer: public CHuiFxVg10RenderbufferBase
    {
public:
    static CHuiFxVg10OnscreenRenderbuffer* NewL(CHuiVg10RenderPlugin& aPlugin, CHuiVg10RenderSurface& aSurface);
    ~CHuiFxVg10OnscreenRenderbuffer();

    CHuiGc& BindAsRenderTarget();
    void BindAsTexture(THuiFxRenderbufferUsage aUsage);
    void UnbindAsRenderTarget();
    void UnbindAsTexture();

    VGImage Image() const;
protected:
    void ConstructL(CHuiVg10RenderPlugin& aPlugin, CHuiVg10RenderSurface& aSurface);
    void PrepareForReuse(const TSize& /* aReusedRect */);
    
    void CreateVgImage();
    void ReleaseVgImage();
    
private:
    CHuiVg10RenderPlugin*   iPlugin;
    CHuiVg10RenderSurface*  iSurface;
    CHuiGc*                 iGc;
    VGImage                 iImage;
    EGLContext              iSavedContext;
    EGLSurface              iSavedReadSurface;
    EGLSurface              iSavedDrawSurface;
    THuiFxRenderbufferUsage iTextureUsage;
    };

#endif /*HUIFXVG10ONSCREENRENDERBUFFER_H_*/
