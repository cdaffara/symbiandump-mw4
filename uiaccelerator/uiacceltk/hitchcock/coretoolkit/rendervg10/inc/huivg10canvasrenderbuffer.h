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
* Description:   Definition of CHuiVg10CanvasRenderBuffer
*
*/



#ifndef HUIVG10CANVASRENDERBUFFER_H_
#define HUIVG10CANVASRENDERBUFFER_H_


#include <e32base.h>
#include <VG/openvg.h>
#include <EGL/egl.h>
#include "huicanvasrenderbuffer.h"
#include "uiacceltk/HuiGc.h"
#include "HuiVg10RenderPlugin.h"
    
/* Forward declarations. */    
class CHuiRenderPlugin;

NONSHARABLE_CLASS (CHuiVg10CanvasRenderBuffer) : public CHuiCanvasRenderBuffer
    {
public:

    /**
     * Constructor.
     */
    CHuiVg10CanvasRenderBuffer();

    /**
     * Destructor.
     */ 
    ~CHuiVg10CanvasRenderBuffer();
    
    
public: // From CHuiCanvasRenderBuffer    

    /**
     * Initializes render buffer.
     */
    void InitializeL(const TSize& aSize);
    
    /**
     * Releases render target resources.
     */
    void UnInitialize();
    
    /**
     * Binds as render buffer.
     */ 
    void Bind();
    
    /**
     * Unbinds as render target and restores the previous target.
     */ 
    void UnBind();
    
    /**
     * Copies content from the given buffer
     */ 
    void Copy(const CHuiCanvasRenderBuffer& aSourceBuffer);    

    /**
     * Copies content from the active surface.
     */ 
    void Copy(TPoint aPoint);    
    
    /**
     * Initializes render buffer, with clear option.
     */
    void InitializeL(const TSize& aSize, TBool aForceClear);
    
public:
    
    VGImage Image() const;
    
private:
    
    void PushEGLContext();
    void PopEGLContext();
    void ReadBackground(TPoint aPosition);
        
private:
 
    CHuiGc*                 iGc;
    VGImage                 iImage;
    VGImage                 iRotatedImage;
    EGLContext              iContext;
    EGLSurface              iSurface;    
    EGLContext              iSavedContext;
    EGLSurface              iSavedReadSurface;
    EGLSurface              iSavedDrawSurface;
    TEGLState               iPreviousEGLState;
    };

#endif /* HUIVG10CANVASRENDERBUFFER_H_ */
