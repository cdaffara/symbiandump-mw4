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



#ifndef HUIFXVG10OFFSCREENRENDERBUFFER_H_
#define HUIFXVG10OFFSCREENRENDERBUFFER_H_

#include "HuiFxVg10RenderbufferBase.h"
#include "HuiVg10RenderPlugin.h"

//FORWARD DECLARATIONS
class CHuiVg10RenderPlugin;

class CHuiFxVg10OffscreenRenderbuffer: public CHuiFxVg10RenderbufferBase
    {
public:
    static CHuiFxVg10OffscreenRenderbuffer* NewL(CHuiVg10RenderPlugin& aPlugin, const TSize& aSize);
    ~CHuiFxVg10OffscreenRenderbuffer();

    void InitGc(const TSize& aSize);
    CHuiGc& BindAsRenderTarget();
    void BindAsTexture(THuiFxRenderbufferUsage aUsage);
    void UnbindAsRenderTarget();
    void UnbindAsTexture();

    void PrepareForReuse(const TSize& aReusedSize);
    
    VGImage Image() const;
protected:
    void ConstructL(CHuiVg10RenderPlugin& aPlugin, const TSize& aSize);
private:
    void ReadBackground();
    void PushEGLContext();
    void PopEGLContext();
    
private:
    CHuiVg10RenderPlugin*   iPlugin;
    CHuiGc*                 iGc;
    VGImage                 iImage;
    EGLContext              iContext;
    EGLSurface              iSurface;
    EGLContext              iSavedContext;
    EGLSurface              iSavedReadSurface;
    EGLSurface              iSavedDrawSurface;
    VGImage                 iRotatedImage;  
    TSize                   iRotatedImageSize;
    TEGLState               iPreviousEGLState;
    };

#endif /*HUIFXVG10OFFSCREENRENDERBUFFER_H_*/
