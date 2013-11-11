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
* Description:   Definition of THuiPanic. Utility class for panicing (macros 
*                and panic codes).
*
*/



#ifndef __HUIPANIC_H__
#define __HUIPANIC_H__


#include <e32base.h>
#include <uiacceltk/HuiStatic.h>


#define HUI_PANIC(reason) \
    { \
    CHuiStatic::Printf(_L8("HUI_PANIC -- Panic %i in "__FILE__" on line %i"), reason, __LINE__); \
    THuiPanic::Panic(reason); \
    }


/**
 * Panic utility class.
 */
NONSHARABLE_CLASS(THuiPanic)
    {
public:

    /**
     * Panic numbers.
     * Note: some of these match with the KErrXY -error codes in
     * e32err.h
     */
    enum TReason
        {
        ENone = 0, ///< No error has occurred

        //
        // Panics that are caused by invalid user actions.
        //

        // Generic.
        EDuplicateObserver = 1,
        EObserverNotFound,

        // Environment.
        EEnvConfigNotFound = 100,

        // Static.
        EStaticInvalidRenderer = 200,
        EStaticDataNotCreated,
        EStaticInvalidClock,
        EStaticCurrentRenderSurfaceNotSpecified, ///< Current render surface is not specified (e.g. MakeCurrent is not called for any render surface)

        // Scheduler.
        ESchedulerUnknownCommandType = 300,

        // Roster.
        ERosterGroupNotFound = 400, ///< Visual's control group is not in the roster
        ERosterUnknownInputObserverType = 401,

        // Skin.
        ESkinHasNoGraphicsContext = 500,
        ESkinInvalidTextStyle,
        ESkinFontLoadingFailed,
        ESkinInvalidFontId,
        ESkinFontSpecCachingFailed,
        ESkinFontSpecCacheInvalid,

        // Control groups.
        EControlGroupTransformNotEnabled = 600,

        // Controls.

        // Visuals.
        EVisualBrushArrayNotEnabled = 700,
        EVisualTransformNotEnabled,
        EVisualHasNoDisplayDuringRefresh,

        /** 
         * Error occurred while preparing a visual for drawing. 
         * The error most probably was propagated to the owner
         * of the visual, but no custom handlers for the errors
         * were implemented. 
         * 
         * @see MHuiVisualOwner::VisualPrepareDrawFailed() - The
         * default implementation in CHuiControl raises this panic. 
         * Override this method in your huicontrols to avoid this 
         * panic.
         */
		EVisualPrepareDrawFailed, 
		
        // Layouts.
        ELayoutDuplicateVisual = 800,
        ELayoutNotScrolling,
        ELayoutChildVisualNotFound,
        ELayoutInvalidChildPosition,

        // Brushes.
        EBrushArrayInvalidIndex = 900,

        // Renderer (generic).
        ERendererError = 1000, ///< glGetError() returned an error.

        // Render surfaces.
        ERenderSurfaceUnknownDisplayMode = 1100, ///< Unknown display mode encountered. No rendering surface could be created that matches the given display mode.
        ERenderSurfaceConfigurationNotSupported, ///< Required render surface configuration is not supported by the system. No required rendering surface could be created.
        ERenderSurfaceActivationFailed, ///< Render surface could not be activated for rendering. MakeCurrent has failed.

        // Texture manager.
        ETextureManagerNoProcessor = 1200,
        ETextureManagerTextureDestroyedDuringRelease,
        ETextureManagerTextureConstructedDuringRelease,
        ETextureManagerTextureDestroyedDuringRestore,
        ETextureManagerTextureConstructedDuringRestore,

        // Display.
        EDisplayClipStackUnpopped = 1300, ///< Rectangles remain on the stack
        EDisplayClipStackEarlyPop, ///< Too many rectangles popped from the stack

        // Image.
        EImageNoTexture = 1400,
        EImageNotSegmentedTexture, ///< Texture used by image must implement MHuiSegmentedTexture.

        // Texture.
        ETextureInvalidFormat = 1500, ///< Tried to use undefined format type
        /**
         * An attempt to use a texture that is not known by the toolkit.
         * Also the texture reference used may be invalid or the texture
         * may have been removed from the toolkit.
         */
        ETextureNotValid,

        // Graphics context.
        EGcClipStackPopFromEmpty = 1600, ///< Tried to pop from empty stack
        EMatrixStackPopFromEmpty,
        EGcInvalidMatrixStack,

        // Text mesh.
        ETextMeshNoText = 1700,

        // OpenGL ES 1.0.
        EGles10Invariant = 1900, ///< Gles10RenderPlugin encounters a fatal error (invariant fails).

        // OpenGL ES 1.1.
        EGles11IncompatibleGlesProfile = 2000,
        EGles11IncompatibleGlesVersion,
        EGles11UnknownGlesVersion,
        
        // OpenVG 1.0
        EVg10Invariant = 3000, ///< Vg10RenderPlugin encounters a fatal error (invariant fails).

        //
        // Panics that are caused by the HUITK implementation.
        // These should not occur even if user is doing something illegal.
        //

        ENotImplemented = 6000, ///< Feature not implemented at this time.
        EMethodDeprecated,      ///< Feature has been deprecated and no longer functions. Should not be used as it will be will be removed.
        EFontTextureNoVacancy, ///< Font texture has been filled with glyphs

        EInternalBitmapFileNameNotFound = 6100,
        EInternalBitmapNotFound,
        EInternalBitmapLoadTextureFailure,


        //
        // Other panics.
        //

        EOutOfMemory = KErrNoMemory, ///< Not enough memory to perform the requested operation.
        EUnknown = KErrGeneral, ///< Unknown (uncategorized) error.

        /// @todo get rid of EInternal panic code (more specific error code has to be used instead!)
        EInternal = -1 ///< Internal error (most probably a bug in the toolkit)
        };


public:

    /**
     * Panics the current thread with the HUI toolkit panic category
     * and the specified panic number.
     *
     * @param aReason  panic number
     * @todo Add description literals to panic codes, match code to description and
     * debug output the error to clarify the situation..
     */
    IMPORT_C static void Panic(TReason aReason);
    };


#endif  // __HUIPANIC_H__
