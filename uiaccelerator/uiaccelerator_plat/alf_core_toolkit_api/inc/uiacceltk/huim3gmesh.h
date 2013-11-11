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
* Description:   Definition of CHuiM3GMesh, an interface enabling usage of M3G
* 				 scene graphs within Hitchcock UI Toolkit user interfaces.
*
*/


 
#ifndef __HUIM3GMESH_H__
#define __HUIM3GMESH_H__

#include <uiacceltk/HuiMesh.h>
#include <uiacceltk/HuiRealRect.h>

// Force softfp linkage until included header has its own definitions
#if defined(__ARMCC_VERSION)
#pragma push
#pragma softfp_linkage
#endif
#include <M3G/m3g_core.h>
#if defined(__ARMCC_VERSION)
#pragma pop
#endif

/* Constants */

// ID for Animating all controllers
const TInt KHuiM3GAnimateAllControllers = 100000;  

/* Forward declarations. */

/* Structs */

// Hitchcock M3G animation controller
struct THuiM3GMeshAnimationController
	{
	M3GAnimationController iController;  // M3G controller
	THuiTimedValue iPosition;            // position in timeline
	TBool iAnimating;                    // is controller animating
	TInt iTimeOffset;
	TInt iPreviousTime;
	};


/**
 * CHuiM3GMesh is a 3D mesh that can be used in Hitchcock UI Toolkit to incorporate
 * M3G (Mobile 3D Graphics) objects into the user interface.
 */
class CHuiM3GMesh : public CHuiMesh
    {
public: // New
    
    /** @beginAPI */
    
    /**
     * Loads an M3G scene graph from an M3G file. The M3G mesh scene graph is loaded 
     * from the file given as a parameter. If the filename string is malformed and does not adhere to
     * file path syntax this method leaves with KErrBadName error code. If the file
     * path is not found this method leaves with KErrPathNotFound error code.
     * If the specified file is not found this method leaves with KErrNotFound error code.
     * If the M3G file does not contain any M3G objects or is corrupted this method
     * leaves with KErrCorrupt error code. If this method leaves the whole scene graph content
     * is released. This means that no partial scene graph content can be accessed through this object.
     *
     * @param aFileName Path to the M3G file from which the M3G scene graph is loaded
     *                  into this M3G mesh. The texture image path is prepended to this
     *                  resource file name. @see CHuiTextureManager::ImagePath().
     */	
	IMPORT_C virtual void LoadSceneL(const TDesC &aFileName);

	/** @endAPI */

    /**
     * Default constructor.
     */
	IMPORT_C CHuiM3GMesh();

    /**
     * Destructor.
     */
	IMPORT_C virtual ~CHuiM3GMesh();

	/**
	 * Set viewport of the m3g mesh.
	 */
    IMPORT_C virtual void SetViewportRect( const THuiRealRect& aRect );

	/**
	 * Rotate world around Y axis.
	 */
	IMPORT_C virtual void RotateYaw(TReal32 aAngle) __SOFTFP;

	/**
	 * Rotate world around X axis.
	 */
	IMPORT_C virtual void RotatePitch(TReal32 aAngle) __SOFTFP;

	/**
	 * Store camera transformations of the M3G world so those can be restored later on and this way animations
	 * doesn't mess with camera manipulations.
	 */
	IMPORT_C virtual void StoreCamera();
	
	/**
	 * Restore camera transformations of the M3G world.
	 */
	IMPORT_C virtual void RestoreCamera();
	
	/**
	 * Goes through animation controllers and does animations.
	 */
	IMPORT_C virtual void AnimateControllers();
	
public: // From CHuiMesh
     /**
     * Draws the loaded M3G scene.
     */     
    IMPORT_C virtual void Draw(CHuiGc& aGc, const THuiImage* aImage = 0, 
                      const THuiImage* aSecondaryImage = 0,
                      TReal32 aSecondaryAlpha = 0.0) const __SOFTFP;


	/**
	 * Returns number of (hitchcock) animation controllers
	 */
    IMPORT_C virtual TInt AnimationControllerCount() const;

	/**
	 * Set position of animation in animation controller.
	 *
	 * @param aControllerId  ID of the controller.
	 * @param aTarget  Target position in the timeline (seconds)
	 * @param aTime  Time to be used in animation
	 */
	IMPORT_C virtual void SetAnimationPosition(TInt aControllerId, TReal32 aTarget, TInt aTime) __SOFTFP;

	/**
	 * Start animation of the controller
	 *
	 * @param aControllerId  ID of the animation controller.
	 */
	IMPORT_C virtual void StartAnimationController(TInt aControllerId);
	
	/**
	 * Stop animation of the controller
	 *
	 * @param aControllerId  ID of the animation controller.
	 */
    IMPORT_C virtual void StopAnimationController(TInt aControllerId);

protected: // todo: to private?

    /**
     * 2nd phase constructor. 
     */
    IMPORT_C virtual void ConstructL();    

    /**
     * Will release the loaded scene.
     */
    IMPORT_C virtual void ReleaseScene();
	
	/**
	 * Load scene with m3g interface parameter
	 */
	IMPORT_C void LoadSceneL(const TDesC& aFileName, M3GInterface aInterface);
	
	/**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void M3GMeshExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

	/**
	 * Finds the center point of the groups objects. Goes the groups
	 * inside the group recursively.
	 *
	 * @param aGroup  group thats center position is needed
	 * @param aTrans  translation values to be returned
	 * @param aFirst  indicates if aTrans has been initiliazed
	 */
	void FindObjectCenter(M3GGroup aGroup, TReal32 aTrans[4], TBool aFirst = ETrue);

	void RotateObjects(TReal32 aAngle, TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP;                      
	
	/**
	 * Populates the iControllers array with animation controllers found
	 * from below given object in the M3G object net.
	 */
	void PopulateAnimationControllerArrayL(const M3GObject aObject);
	
	/**
	 * Execute the actual scene graph loading.
	 */
	void DoLoadSceneL(const TDesC& aFileName, M3GInterface aInterface);	
	
protected:

	// Area where M3G mesh is rendered.
	THuiRealRect iViewportRect;

	// World
    M3GWorld iWorld;

    // Camera
    M3GCamera iCamera;
    TBool iUpdateCamera;

    // Objects
    TInt iNumObjects;
    M3GObject *iObjects;


	// Stored camera transformations    
    TReal32 iCamRotation[4];
    TReal32 iCamTranslation[4];

    // Animation controllers
    RArray<THuiM3GMeshAnimationController> iControllers;
    
    /** Spare member to help keeping binary compatibility, since HuiM3GMesh is
        now dll derivable **/
    TInt iSpare;

    };  

#endif  // __HUIM3GMESH_H__
