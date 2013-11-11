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
* Description:   OpenVG 1.0 render surface.
*
*/



#ifndef __HUIVG10RENDERSURFACE_H__
#define __HUIVG10RENDERSURFACE_H__


#include <e32base.h>

#include <EGL/egl.h>

#include "HuiRenderSurface.h"
#include "uiacceltk/HuiSessionObject.h"


/* Forward declarations. */
class CHuiDisplay;


/**
 * The render surface is a destination where rendered graphics will ultimately
 * end up. This may be, e.g., the display frame buffer or a bitmap in memory.
 */
NONSHARABLE_CLASS(CHuiVg10RenderSurface) : public CBase, public MHuiRenderSurface,
                                           public MHuiSessionObject
    {
public:

    /* Constructors and destructor. */

    static CHuiVg10RenderSurface* NewL(CHuiDisplay& aDisplay);

    static CHuiVg10RenderSurface* NewLC(CHuiDisplay& aDisplay);

    virtual ~CHuiVg10RenderSurface();


    /* Methods, implemets MHuiRenderSurface interface. */

    TUint Flags() const;
    
    TBool GetScreenOrigin(TPoint& aOrigin) const;

    TSize Size() const;

    void SetSizeL(const TSize& aSize);

    virtual void MakeCurrent();

    virtual void SwapBuffers();

    virtual void Finish();

    virtual void BindTexture(TInt aTextureUnit,
                             const MHuiSegmentedTexture& aTexture,
                             TInt aSegment);

    /**
     * Handles change in surface visibility. This method is called if surface is either
     * hidden or shown. This default implementation ignores visibility events.
     *
     * @param aIsVisible ETrue if surface becomes visible. EFalse if surface is hidden.
     */
    virtual void HandleVisibilityEvent(TBool aIsVisible);

    virtual void Release();

    virtual void RestoreL();

    virtual void HandleDisplayUsageChangeL();
    
    // These methods are not really used, since opengles renderer doesn't
    // support dirty region handling or symbian graphics context!
    virtual void SetDirtyRect(const TRect& aRect);
    virtual MHuiTargetBitmap* BackBuffer();
    virtual CFbsBitGc* Gc();

    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    virtual void RenderSurfaceExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

    /**
     * Static method for returning verbose error message strings according to the given
     * EGL error value. This can be used to create meaningful error messages in
     * HUIToolkit environments that use OpenVG rendering.
     *
     * @param aErrorCode EGL Error code retrieved by eglGetError - method.
     * @return Verbose error message as a TDesC reference object.
     */
    static const TDesC& EglErrorMessage(TInt aErrorCode);

    // Implementation of MHuiSessionObject
    
    /**
     * Gets the object type.
     */
    TType Type() const;

    /**
     * Gets the session id for object.
     */
    TInt SessionId() const;

    /**
     * Sets the session id for object.
     */
    void SetSessionId(TInt aSessionId);
    
    /**
     *  @return the currently bound texture image.
     */
    const MHuiSegmentedTexture* BoundTexture() const;

protected:

    /* Constructors. */

    CHuiVg10RenderSurface(CHuiDisplay& aDisplay);

    void ConstructL();

    /**
     * Determines the display to which this rendering surface is attached.
     *
     * @return  Display.
     */
    CHuiDisplay& Display() const;

    /**
     * Determine config for the owner display.
     */
    EGLConfig DetermineConfig();

    /**
     * Create an EGL rendering surface.
     */
    EGLSurface CreateSurface(EGLConfig aConfig);


protected:

    /** Display that owns the surface. */
    CHuiDisplay* iDisplay;

    /** The display. */
    EGLDisplay iEglDisplay;

    /** The rendering context. */
    EGLContext iEglContext;

    /** The window where the graphics are drawn. */
    EGLSurface iEglSurface;

    /** Size of the rendering surface. */
    TSize iSize;
    
    /** The currently bound texture */
    const MHuiSegmentedTexture* iBoundTexture;

private:
    /** Session id */
    TInt iSessionId;
    
    };


#endif // __HUIVG10RENDERSURFACE_H__
