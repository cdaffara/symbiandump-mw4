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
* Description:   Defines interface for HUITK render surfaces. The rendering 
*                surface is a destination where rendered graphics will 
*                ultimately end up.
*
*/



#ifndef __HUIRENDERSURFACE_H__
#define __HUIRENDERSURFACE_H__


#include <e32base.h>


/* Forward declarations. */
class MHuiSegmentedTexture;
class MHuiTargetBitmap;
class CFbsBitGc;


/**
 * The render surface is a destination where rendered graphics will ultimately
 * end up. This may be, e.g., the display frame buffer or a bitmap in memory.
 */
class MHuiRenderSurface
    {
public:

	/** Render surface flags */
	enum TFlags
    	{
    	EFlagNone =                            0x0,
		/** Surface uses double buffering. */    	
		EFlagUseDoubleBuffering =              0x1,
		/** Surface uses dirty rects. */    	
    	EFlagUseDirtyRects =                   0x2,
    	/** Surface uses transformed clipping rects. */
    	EFlagUsetransformedClippingRects =     0x4,
    	/** Surface uses Symbian graphics context rects. */
    	EFlagUseSymbianGC =                    0x8
    	};

public:

    /**
     * Virtual destructor.
     */
    IMPORT_C virtual ~MHuiRenderSurface();
    
    /**
     * Returns the flags of the render surface.
     * @return Flags.
     */
    virtual TUint Flags() const = 0;
    
    /**
     * Determines the location of the rendering surface on the screen.
     *
     * @param aOrigin  Top left corner coordinates returned here, in absolute screen
     *                 coordinates. The top left corner of the screen is (0, 0).
     *
     * @return  If location successfully determined, returns <code>ETrue</code>.
     *          If the surface is an off-screen surface, returns <code>EFalse</code>.
     */
    virtual TBool GetScreenOrigin(TPoint& aOrigin) const = 0;

    /**
     * Determines the size of the surface.
     *
     * @return  Size of the surface in pixels.
     */
    virtual TSize Size() const = 0;

    /**
     * Changes the size of the surface.
     *
     * @param aSize  Size of surface in pixels.
     */
    virtual void SetSizeL(const TSize& aSize) = 0;

    /**
     * Makes this the current rendering surface for any drawing operations.
     * Call CHuiStatic::SetCurrentRenderSurface().
     */
    virtual void MakeCurrent() = 0;

    /**
     * Swaps the back buffer to the front.
     */
    virtual void SwapBuffers() = 0;

    /**
     * Synchronizes the context. All rendering calls for the currently bound context are guaranteed 
     * to be executed, when thsi method returns.
     */
    virtual void Finish() = 0;

    /**
     * Binds a texture to the surface to be used during subsequent drawing
     * operations.
     *
     * In practice, the texture is bound to the rendering context
     * of this render surface. When something is drawn in the surface's 
     * rendering context, the texture specified here is used.
     *
     * @param aTextureUnit Texture unit number to bind the texture to. Binding
     * to different units enables multitexturing, but using multiple texture
     * units (multitexturing) may not be supported.
     * @param aTexture The texture to be bound.
     * @param activeTextureSegment Index of the texture segment to be bound.
     * operations. 
     */
    virtual void BindTexture(TInt aTextureUnit,
                             const MHuiSegmentedTexture& aTexture,
                             TInt activeTextureSegment) = 0;

    /**
     * Handles change in surface visibility. This method is called if surface is either
     * hidden or shown.
     *
     * @param aIsVisible ETrue if surface becomes visible. EFalse if surface is hidden.
     */
    virtual void HandleVisibilityEvent(TBool aIsVisible) = 0;

    /**
    * Temporarily release the resources of this rendering surface. You
    * can call this method to free up resources related to the rendering surface,
    * for example when the display goes to background (ie. render surface is not needed).
    * @see RestoreL() 
    */
    virtual void Release() = 0;
    
    /**
    * Recreate any released resources of this rendering surface. You
    * can call this method to recreate resources related to rendering surface,
    * for example, when the display comes back to foreground (ie. becomes visible).
    * @see Release() 
    */
    virtual void RestoreL() = 0;

    /**
    * Handles change in the intended usage of display. Most render surfaces do not
    * have any special functionality for different display usage scenarios.
    */
    virtual void HandleDisplayUsageChangeL() = 0;


    /**
    * Sets the dirty region i.e. the rectangular area that needs to be updated.
    * 
    * Writing a real implementation for this might not be needed if the renderer
    * does not support dirty region handling.
    * 
    * @param aRect The rectangular area that is dirty
    */
    virtual void SetDirtyRect(const TRect& aRect) = 0;
    
    /**
     * Returns the back buffer bitmap.
     * 
     * Writing a real implementation for this might not be needed if the renderer
     * does not support symbian graphics context.
     *
     * @return  Back buffer bitmap.
     */
    virtual MHuiTargetBitmap* BackBuffer() = 0;

    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    virtual void RenderSurfaceExtension(const TUid& aExtensionUid, TAny** aExtensionParams) = 0;
    };


#endif
