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
* Description:   Definition of CHuiRenderPlugin, a base class for
*                all rendering plugins / renderers.
*
*/




#ifndef __HUIRENDERPLUGIN_H__
#define __HUIRENDERPLUGIN_H__


#include <e32base.h>
#include <bitdev.h>
#include <uiacceltk/HuiMesh.h>

#include "HuiRenderPluginBase.h"


/* Forward declarations. */
class MHuiRenderSurface;
class CHuiEnv;
class CHuiDisplay;
class CHuiGc;
class CHuiTexture;
class THuiTextureHandle;
class CHuiTextureManager;
class THuiFontManager;
class CHuiTextMesh;
class CHuiCurvePath;
class CHuiCanvasGc;

class CHuiFxEngine;

/**
 * Rendering plugin identifiers.
 */
enum THuiRenderPluginId
    {
    /** BitGdi renderer for legacy systems. */
    EHuiRenderPluginBitgdi,

    /** OpenVG 1.0 renderer. */
    EHuiRenderPluginVg10,

    /** OpenGL ES 1.0 renderer. */
    EHuiRenderPluginGles10,

    /** OpenGL ES 1.1 renderer. */
    EHuiRenderPluginGles11,

    /** OpenGL ES 2.0 renderer. */
    EHuiRenderPluginGles20
    };


/**
 * Specific rendering features allowed by a plugin.
 */
enum THuiRenderPluginAllow
    {
    /** The rendering plugin supports 3D projection and allows visuals to use
        the 3D projection mode. */
    EHuiRenderPluginAllow3DProjection,

    /** The rendering plugin supports bilinear filtering without considerable
        performance penalty. */
    EHuiRenderPluginAllowBilinearFilter,

    /** The rendering plugin supports a multitexturing mode where two textures
        are interpolated by an arbitrary factor. */
    EHuiRenderPluginAllowTextureModeInterpolate,
    
    /** The rendering plugin allows using pBuffer surfaces for texture preloading */
    EHuiRenderPluginAllowPBufferSurfaces,
    
    /** The rendering plugin allows to preserve buffer content after 
        eglswapbuffers has been called */
    EHuiRenderPluginAllowSwapBufferPreserve,
    
    /** The rendering plugin allows using pBuffer surfaces for cached visual drawing */
    EHuiRenderPluginAllowVisualPBufferSurfaces
    };


/**
 * The common base class for all Hitchcock rendering plugins (renderers).
 *
 * For example defines factory methods for creating render specific
 * implementations for toolkit's resources, such as meshes, graphics
 * context, textures and the texture manager.
 *
 * Rendering plugins are internal to the toolkit.
 */
class CHuiRenderPlugin : public CHuiRenderPluginBase
    {
public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHuiRenderPlugin();


    /* Methods. */

    /**
     * Determines the identifier of the renderer.
     */
    IMPORT_C THuiRenderPluginId Id() const;

    /**
     * Determines if the renderer allows the usage of a specific
     * rendering feature or technique.
     *
     * @param aAllow  Feature to check.
     *
     * @return True, if allowed.
     */
    IMPORT_C virtual TBool Allows(THuiRenderPluginAllow aAllow) const;

    /**
     * Creates a new rendering surface for a HUITK display.
     *
     * @param aDisplay  Display.
     */
    IMPORT_C virtual MHuiRenderSurface* CreateRenderSurfaceL(CHuiDisplay& aDisplay);

    /**
     * Creates a new instance of a HUITK graphics context.
     */
    IMPORT_C virtual CHuiGc* CreateGcL();

    /**
     * Creates a new instance of a texture manager.
     */
    IMPORT_C virtual CHuiTextureManager* CreateTextureManagerL(CHuiEnv& aEnv);

    /**
     * Creates a new instance of an effects engine.
     */
    IMPORT_C virtual CHuiFxEngine* CreateEffectsEngineL();

    /**
     * Creates a new instance of font manager. The created font manager instance has
     * to be freed by the caller. This is used to manage all font instances in Hitchcock UI Toolkit.
     */
    IMPORT_C virtual THuiFontManager* CreateFontManagerL() const;
    
    /**
     * Creates a new instance of a texture.
     *
     * @param aExistingTexture  Handle to an existing named texture object.
     *                          The returned CHuiTexture will take ownership
     *                          of the handle's texture object.
     */
    IMPORT_C virtual CHuiTexture* CreateTextureL(const THuiTextureHandle* aExistingTexture = 0);

    /**
     * Creates a new instance of a texture, and pushes it on the cleanup stack.
     */
    IMPORT_C virtual CHuiTexture* CreateTextureLC(const THuiTextureHandle* aExistingTexture = 0);

    /**
     * Creates a new instance of a mesh.
     *
     * @param aMeshType  CHuiMesh derived concrete mesh type.
     *
     * @return  Concrete implementation of CHuiMesh.
     */
    IMPORT_C virtual CHuiMesh* CreateMeshL(THuiMeshType aMeshType);

    /**
     * Creates a new instance of a text mesh.
     */
    IMPORT_C virtual CHuiTextMesh* CreateTextMeshL() = 0;

    /**
     * Creates a new instance of a curve path.
     */
    IMPORT_C virtual CHuiCurvePath* CreateCurvePathL();

    /**
     * Deletes a named texture object. Named texture objects are owned
     * by the graphics API itself.
     */
    IMPORT_C virtual void DeleteNamedTexture(TUint aName);

    /**
     * Notifies this render plugin of a change in current display count.
     *
     * @param aCount The current count of displays existing in the HuiToolkit environment.
     */
    IMPORT_C virtual void NotifyDisplayCountL(TUint aCount);

    /**
     * Releases render plugin resources. Release all resources that can restored
     * later independently by render plugin. Might release for example render surfaces.
     *
     * The actual behaviour depends on the inherited class.
     *
     * @see RestoreL()
     */
    IMPORT_C virtual void Release();

    /**
     * Restores render plugin resources. Restores all resources released in Release().
     *
     * The actual behaviour depends on the inherited class.
     *
     * @see Release()
     */
    IMPORT_C virtual void RestoreL();
    
    /**
     * Enables off screen bitmap mode 
     */
    IMPORT_C virtual void EnableOffScreenBitmapL(TInt aFlags);

    /**
     * Disables off screen bitmap mode 
     */
    IMPORT_C virtual void DisableOffScreenBitmap();

    /**
     * Gets the pointer to off screen bitmap 
     */
    IMPORT_C virtual CFbsBitmap* OffScreenBitmap() const;
	
    /**
     * Updates the latest frame to the off screen bitmap
     */
    IMPORT_C virtual void UpdateOffScreenBitmapL(CHuiDisplay& aDisplay);

    /**
     * Draws the off screen bitmap to the window
     */
    IMPORT_C virtual void DrawOffScreenBitmapToWindowL(CHuiDisplay& aDisplay);

    /**
     * Creates a new instance of a canvas graphics context.
     */
    IMPORT_C virtual CHuiCanvasGc* CreateCanvasGcL();
    
    /**
     * Adds flags to render plugin, which can be used e.g. in GC to determine
     * if the state has to be restored somehow. Each renderer has it's own flags 
     */
    IMPORT_C virtual void AddRestoreStateFlags(TInt aFlags);
    
    /**
     * Returns the changed state flags
     */
    IMPORT_C virtual TInt GetRestoreStateFlags();
    
    /**
     * Clears the state flags
     */
    IMPORT_C virtual void ClearRestoreStateFlags();
    
protected:

    /* Constructors. */

    /**
     * Default constructor.
     */
    IMPORT_C CHuiRenderPlugin(THuiRenderPluginId aId);

protected: // New methods
    
    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void RenderPluginExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Identifier of the plugin. */
    THuiRenderPluginId iId;

    /** Spare member to help keeping binary compatibility, since HuiRenderPlugin
        is now dll derivable **/
    TInt iSpare;
    };


#endif // __HUIENV_H__
