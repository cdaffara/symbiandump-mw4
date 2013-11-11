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
* Description:   Definition of CHuiVg10PBufferSurface class. 
*                CHuiVg10PBufferSurface is an OpenVG PBuffer surface 
*                object that implements the MHuiRenderSurface interface.
*
*/



#ifndef __HUIVG10PBUFFERSURFACE_H__
#define __HUIVG10PBUFFERSURFACE_H__


#include "HuiVg10RenderSurface.h"

class CHuiVg10RenderPlugin;
class CFbsBitmap;

/**
 * Off-screen rendering surface for OpenGL ES content. A pbuffer (pixel buffer)
 * surface can be used for instance for rendering temporary graphical content 
 * onto. This content can then be reused in the current application. Because 
 * the class has been inherited from MHuiRenderSurface, it can also be used 
 * for rendering from displays. Doesn't output rendered content on the 
 * framebuffer.
 *
 * @lib hitchcock.lib
 */
NONSHARABLE_CLASS(CHuiVg10PBufferSurface) : public CBase, public MHuiRenderSurface
    {
public: // Constructors and destructor

    /**
	 * \todo     
	 */
    static CHuiVg10PBufferSurface* NewLC(CHuiVg10RenderPlugin& aRenderer, const TSize& aSize, TInt aAlphaBits, TInt aEglBufferType, TInt aBufferColorMode);

    /**
     * Constructs a new example CHuiVg10PBufferSurface instance. A static factory method that
     * leaves the object to the cleanup stack.
     * @param aRenderer CHuiVg10RenderPlugin object that is used to access the shared context
     * and determine surface configurations.
     * @param aSize Size in pixels of the created PBuffer surface object.
     * @return Newly created instance of CHuiVg10PBufferSurface.
     */
    static CHuiVg10PBufferSurface* NewLC(CHuiVg10RenderPlugin& aRenderer, const TSize& aSize, TInt aAlphaBits = 0);

    /**
     * Constructs a new example CHuiVg10PBufferSurface instance. A static factory method.
     * @param aSize Size in pixels of the created PBuffer surface object.
     * @return Newly created instance of CHuiVg10PBufferSurface.
     */
    static CHuiVg10PBufferSurface* NewL(const TSize& aSize, TInt aAlphaBits = 0);

    /**
     * Constructs a new CHuiVg10PBufferSurface class instance and leaves it
     * on the cleanup stack.
     * @param aSize Size in pixels of the created PBuffer surface object.
     * @return Newly created instance of CHuiVg10PBufferSurface.
     */
    static CHuiVg10PBufferSurface* NewLC(const TSize& aSize, TInt aAlphaBits = 0);

    /**
     * Destructor.
     */
    virtual ~CHuiVg10PBufferSurface();

public: // From MHuiRenderSurface

    TUint Flags() const;
    
    /**
     * Determines the location of the rendering surface on the screen. But since
     * this is an off-screen surface, will return EFalse and will not set the
     * origin.
     *
     * @return  If location successfully determined, returns <code>ETrue</code>.
     *          If the surface is an off-screen surface, returns <code>EFalse</code>.
     */
    TBool GetScreenOrigin(TPoint& aOrigin) const;

    /**
     * Determines the size of the surface.
     *
     * @return  Size of the surface in pixels.
     */
    TSize Size() const;

    /**
     * Changes the size of the surface.
     *
     * @param aSize  Size of surface in pixels.
     */
    void SetSizeL(const TSize& aSize);

    /**
     * Makes this the current rendering surface for any drawing operations.
     * Call CHuiStatic::SetCurrentRenderSurface().
     */
    void MakeCurrent();

    /**
     * Swaps the front and back buffers of the surface.
     * Pbuffers consist of a single buffer only, so swapping has no effect.
     */
    void SwapBuffers();

    /**
     * Synchronizes the context. All rendering calls for the currently bound context are guaranteed 
     * to be executed, when thsi method returns.
     */
    void Finish();

    /**
     * Binds a texture to the surface to be used during subsequent drawing
     * operations.
     */
    void BindTexture(TInt aTextureUnit,
                             const MHuiSegmentedTexture& aTexture,
                             TInt activeTextureSegment);

    /**
     * Handles change in surface visibility. This method is called if surface is either
     * hidden or shown.
     *
     * @param aIsVisible ETrue if surface becomes visible. EFalse if surface is hidden.
     */
    void HandleVisibilityEvent(TBool aIsVisible);

    void Release();
    
    void RestoreL();
    
    void HandleDisplayUsageChangeL();    

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

public: // New methods

    /**
     * Returns the associated EGL surface object.
     */
    EGLSurface EglSurface();
    
	void BindToM3G();
	
	void ReleaseFromM3G();
    
    /**
     * \todo
     */
    CFbsBitmap* Buffer();
    
    /**
     * \todo
     */
    TInt BufferColorMode() const;


protected:

    /**
     * Creates a new PBuffer surface object.
     */
    EGLSurface CreateSurface(EGLConfig aConfig);

    /**
     * Determine config for the owner display.
     */
    EGLConfig DetermineConfig();


private:

    /* Constructors. */

    /**
     * Constructs an instance of CHuiClassName.
     * @param aRenderer    CHuiVg10RenderPlugin object that is used to access 
     *                     the shared context and determine surface configurations.
     * @param aSize        Size of the created PBuffer surface.
     * @note               Protected because this constructor only needs to be 
     *                     called by derived classes. Use the provided static 
     *                     factory methods instead.
     * @see NewL()
     * @see NewLC()
     */
    CHuiVg10PBufferSurface(CHuiVg10RenderPlugin& aRenderer, const TSize& aSize, TInt aAlphaBits, TInt aEglBufferType, TInt aBufferColorMode);

    /**
     * Second-phase constructor.
     */
    void ConstructL();


private:

    /** The display. */
    EGLDisplay iEglDisplay;

    /** The rendering context. */
    EGLContext iEglContext;

    /** The pbuffer surface where the graphics are drawn. */
    EGLSurface iEglSurface;

    /** Current surface size. */
    TSize iSize;

    /** \todo */
	TInt iEglBufferType;
	
    /** \todo */
	TInt iBufferColorMode;
	
    /** Alpha channel bits to allocate. */
    TInt iAlphaBits;
    
    CFbsBitmap* iBuffer;
    
    /** OpenGL ES Render plugin that provides the display configurations and the display
    for this pbuffer surface. */
    CHuiVg10RenderPlugin& iRenderer;

    TBool iIsHardwareAccelerated;
    };


#endif // __HUIVG10PBUFFERSURFACE_H__

