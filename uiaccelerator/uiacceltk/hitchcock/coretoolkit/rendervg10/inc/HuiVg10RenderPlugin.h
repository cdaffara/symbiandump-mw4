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
* Description:   Definition of CHuiVg10RenderPlugin. Rendering plugin for OpenVG 1.0.
*
*/



#ifndef __HUIVG10RENDERPLUGIN_H__
#define __HUIVG10RENDERPLUGIN_H__


#include <e32base.h>

#include <EGL/egl.h>

#include <VG/openvg.h>

#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiMesh.h"

class TEGLState
    {
public:
    EGLContext iContext;
    EGLSurface iReadSurface;
    EGLSurface iDrawSurface;
    EGLDisplay iDisplay;
    };

/**
 * OpenVG renderer specific state change flags
 */
enum THuiVg10GcStateFlags
    {
    EHuiVg10GcStateFlagDirtyMatrixMode      = 1,
    EHuiVg10GcStateFlagDirtyMatrix          = 2,
    EHuiVg10GcStateFlagDirtyPaint           = 4,
    EHuiVg10GcStateFlagDirtyScissor         = 8,
    EHuiVg10GcStateFlagDirtyScissorRects    = 0x10,
    EHuiVg10GcStateFlagDirtyBlendMode       = 0x20
    };

/* Forward declarations. */
class MHuiRenderSurface;
class CHuiVg10PBufferSurface;
#ifdef __NVG
class CNvgEngine;
class MVGImageBinder;
#endif

/** Number of compatible EGL configurations queried. */
const TInt KVg10ConfigsCount = 2;

/**
 * Macro for asserting that the renderer's state is OK when entering
 * a function.
 */
#define HUI_VG_INVARIANT() __ASSERT_DEBUG(CHuiVg10RenderPlugin::VgError() == VG_NO_ERROR, THuiPanic::Panic(THuiPanic::EVg10Invariant))

/**
 * Macro for asserting that the latest OpenVG function call did not
 * fail. You may put this assert after any OpenVG function call.
 * Has no effect in release builds.
 *
 * @param aPanic  Panic code to use if the assertion fails.
 *
 * @see THuiPanic error codes, for at least the ERendererError.
 */
#define HUI_VG_ASSERT_SUCCESS(aPanic) __ASSERT_DEBUG(CHuiVg10RenderPlugin::VgError() == VG_NO_ERROR, THuiPanic::Panic(aPanic))


/**
 * Rendering plugin for OpenVG 1.0.
 *
 * @todo  GHuiVg10RenderPlugin should be derived from an abstract
 *        CHuiEglRenderPlugin, which would contain the common EGL-methods.
 *        OpenVG uses EGL, too.
 */
NONSHARABLE_CLASS(CHuiVg10RenderPlugin) : public CHuiRenderPlugin
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructs a new OpenVG 1.0 rendering plugin instance.
     */
    static CHuiVg10RenderPlugin* NewL();

    /**
     * Constructs a new OpenVG 1.0 rendering plugin instance and leaves it
     * on the cleanup stack.
     */
    static CHuiVg10RenderPlugin* NewLC();


    /**
     * Destructor.
     */
    virtual ~CHuiVg10RenderPlugin();


    /* Methods. */

    TBool Allows(THuiRenderPluginAllow aAllow) const;

    /**
     * Creates a new rendering surface.
     *
     * @param aParams  Parameters for the surface. Ends in EHuiRenderSurfaceNone.
     */
    virtual MHuiRenderSurface* CreateRenderSurfaceL(CHuiDisplay& aDisplay);

    /**
     * Creates a new PBuffer surface for off-screen rendering purposes.
     *
     * @param aSize Size of the created PBuffer surface.
     * @return Pointer to the just created PBuffer surface object.
     */
    virtual CHuiVg10PBufferSurface* CreatePBufferSurfaceL(TSize aSize, TInt aEglBufferType = EGL_PBUFFER_BIT, TInt aBufferColorMode = 0);

    /**
     * Creates a new instance of a HUITK graphics context.
     */
    virtual CHuiGc* CreateGcL();

    /**
     * Creates a new instance of a texture.
     */
    virtual CHuiTexture* CreateTextureL(const THuiTextureHandle* aExistingTexture = 0);

    /**
     * Creates a new instance of a texture manager.
     */
    virtual CHuiTextureManager* CreateTextureManagerL(CHuiEnv& aEnv);

    /**
     * Creates a new instance of an effects engine.
     */
    virtual CHuiFxEngine* CreateEffectsEngineL();


    /**
     * Creates a new instance of a mesh. Leaves with KErrNotSupported if this renderer
     * does not support the given mesh type
     *
     * @param aMeshType  CHuiMesh derived concrete mesh type
     * 
     * @return  Created concrete mesh implementation.
     */
    virtual CHuiMesh* CreateMeshL(THuiMeshType aMeshType);

    /**
     * Creates a new instance of a text mesh.
     */
    virtual CHuiTextMesh* CreateTextMeshL();

    /**
     * Creates a new instance of a curve path.
     */
    virtual CHuiCurvePath* CreateCurvePathL();

    virtual void DeleteNamedTexture(TUint aName);

    /**
     * Notifies this render plugin of a change in current display count.
     *
     * @param aCount The current count of displays existing in the HuiToolkit environment.
     */
    virtual void NotifyDisplayCountL(TUint aCount);

    /**
     * Returns the EGL display currently used by the plugin.
     */
    EGLConfig EglDisplay();

    /**
     * Returns the shared EGL context.
     *
     * @param aOtherContext  Context that wants to join the shared context.
     */
    EGLContext EglSharedContext() const;

    /**
     * Sets the shared EGL context.
     *
     * @param aContext  Context that is available for sharing with.
     */
    void EglSetSharedContext(EGLContext aContext);

    /**
     * EGL utility method. See eglChooseConfig.
     *
     * @return  Number of configurations found.
     */
    TInt EglChooseConfig(const TInt* aAttribList);

    /**
     * EGL utility method. Returns one of the configurations determined by
     * EglChooseConfig.
     */
    TInt EglConfig(TInt aIndex);

    /**
     * EGL debug method. Prints (using CHuiStatic::Printf) all the attributes of
     * the specified configuration.
     */
    void EglPrintConfig(TInt aIndex);

    /**
     * Check if the OpenVG context is released. If it is released,
     * any drawing commands to the context will cause a panic.
     *
     * @return ETrue if the OpenVG context has been released.
     */
     TBool IsReleased() const;

    /* Static methods. */

    /**
     * GlErrorMessage returns string description for an OpenVG
     * error code. Can be used to clarify OpenVG errors when debugging.
     *
     * Error codes are from from the OpenVG 1.0 specification, chapter 4.
     *
     * @param aErrorCode  OpenVG error code, usually requested by calling
     *                    vgGetError().
     * @return  Descriptor with the error message.
     */
    static const TDesC& VgErrorMessage(VGErrorCode aErrorCode);

    /**
     * Returns the latest OpenVG error code. Equivalent to calling vgGetError(),
     * but also prints the GL error code as a debug message.
     */
    static VGErrorCode VgError();
	
	/**
	 * IsHardwareAccelerated returns boolean value if hardware acceleration 
	 * is available. Also stores value to the iIsHardwareAccelerated variable.
	 *
	 * @return  ETrue if hardware accelartion is available
	 */
	TBool IsHardwareAccelerated();
	
    /**
     * Creates a new instance of a canvas graphics context.
     */
    virtual CHuiCanvasGc* CreateCanvasGcL();

    /**
     * Used to set flags that describe how the openvg state has changed.
     */
    virtual void AddRestoreStateFlags(TInt aFlags);
   
    /**
     * Return the flags, e.g. for vg10gc to reset its state
     */
    virtual TInt GetRestoreStateFlags();
   
    /**
     * Clear the stateflags
     */
    virtual void ClearRestoreStateFlags();
   
#ifdef __NVG
    /**
     * Getter for NVGEngine, to be used with NVG drawing inside OpenVG plugin
     * 
     * @return   CNvgEngine object
     */
    CNvgEngine& NvgEngine() const;
#endif

    /**
     * Getter for hardware configuration flags (rendering quality Accurate / Fast)
     */
    TInt GetHwConfigurationFlags() const;
    
	/**
	*  Getter for texture upload context. This is called by CHuiVg10Texture.
	*/
    TEGLState& GetUploadState()
	    {
	    return iEGLStateForTextureUpload;
	    }
	
protected:

    /* Constructors. */

    /**
     * Default constructor.
     */
    CHuiVg10RenderPlugin(THuiRenderPluginId aId = EHuiRenderPluginVg10);

    /**
     * Second-phase constructor. Basicly calls RestoreL().
     */
    void ConstructL();

    /**
     * Release resources associated with EGL and OpenVG.
     * Also, will destroy the default render surface used
     * for sharing resources.
     *
     * @see RestoreL()
     */
    virtual void Release();

    /**
     * Restores resources needed for EGL/OpenVG rendering.
     * Finds an EGL display, initializes it for rendering
     * and restores a default shared render surface that
     * enables resources to be loaded without an existing
     * CHuiDisplay.
     */
    virtual void RestoreL();

private:
    
    void ReadAllowsSwapBufferPreserved();
    void ReadAllowsVisualPBufferSurfaces();
        
private:

    /** The display where the graphics are drawn. */
    EGLDisplay iEglDisplay;

    /** Describes the format, type and size of the color buffers and
        ancillary buffers for EGLSurface. */
    EGLConfig iConfigs[KVg10ConfigsCount];

    /**
     * EGL Context for shared resources. This will allow us to
     * for instance preload and create textures before actual
     * rendering surface exists.
     */
    EGLContext iSharedContext;

    /**
     * Dummy PBuffer surface to be used with default context before any
     * other rendering surfaces exist.
     */
    CHuiVg10PBufferSurface* iDefaultRenderSurface;

    TBool iIsHardwareAccelerated;
    
    /**
     * Member that stores the changed flags
     */
	TInt iStateFlags;
    
#ifdef __NVG
    /** NVG decoder for plugin classes to use **/
    CNvgEngine* iNvgEngine;
    /** Image binder for NVG-TLV "Group opacity" special case */
    MVGImageBinder* iMVGImageBinder;
#endif
    
    // Hardware configuration, fetched from CenRep
    TInt iHwConf;
    
    TBool iAllowsSwapBufferPreserved;
    TBool iAllowsVisualPBufferSurfaces;
    
	// This is initialized by CHuiVg10Texture during first texture upload
    TEGLState iEGLStateForTextureUpload;
    
    };


#endif // __HUIVG10RENDERPLUGIN_H__
