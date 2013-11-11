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
* Description:   ?Description
*
*/



#ifndef __HUIMESH_H__
#define __HUIMESH_H__


#include <e32base.h>
#include <gdi.h>

#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiMaterial.h>
#include <uiacceltk/HuiSessionObject.h>
#include <uiacceltk/HuiRealRect.h>


/* Forward declarations. */
class CHuiGc;

/**
 * Mesh types.
 * These types can be used in NewL to create a concrete
 * mesh object from the supported set.
 */
enum THuiMeshType
    {
    /** Procedural mesh. @see CHuiProceduralMesh */
    EHuiMeshTypeProcedural,
    
    /** M3G mesh. @see CHuiM3GMesh */
    EHuiMeshTypeM3G
    };


/**
 * CHuiMesh is the base class for all 3D meshes. It also definces few
 * simpliest 3D manipulation interfaces that applications can use to 
 * manipulate meshes. It is possible to instantiate CHuiMesh 
 * itself, but the resulting mesh instance will have no functionality. 
 * Thus it can be used as a dummy instance in rendering plugins that do 
 * not support drawing of 3D meshes.
 */
NONSHARABLE_CLASS(CHuiMesh) : public CBase, public MHuiSessionObject
    {
public:    

    /* Constructors and destructors. */

    /**
     * Constructs a new mesh instance. Applications call this to create new
     * mesh instances. The implementation of the returned mesh instance 
     * depends on the active renderer. This method does not return CHuiMesh
     * object but an object derived from CHuiMesh.
     *
     * @param  aMeshType  CHuiMesh derived concrete mesh type.
     *
     * @return  Concrete implementation of CHuiMesh.
     */
    IMPORT_C static CHuiMesh* NewL(THuiMeshType aMeshType);
    
    /**
     * Constructs a new mesh instance and leaves it on the cleanup stack.
     * Applications call this to create new mesh instances. The implementation 
     * of the returned mesh instance depends on the active renderer.This method 
     * does not return CHuiMesh object but an object derived from CHuiMesh.
     *
     * @param  aMeshType  CHuiMesh derived concrete mesh type.
     *
     * @return  Concrete implementation of CHuiMesh.
     */
    IMPORT_C static CHuiMesh* NewLC(THuiMeshType aMeshType);

    /**
     * Default constructor.
     */
    CHuiMesh(THuiMeshType aMeshType = EHuiMeshTypeProcedural);

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiMesh();           

    
    /* Methods. */

    /**
     * Determines the concrete mesh type this instance implements.
     *
     * @return THuiMeshType The mesh type implemented by this object.
     */
    IMPORT_C THuiMeshType MeshType() const;

    /**
     * Resets the mesh. All contents are destroyed.
     */
    IMPORT_C virtual void Reset();

    /**
     * Stretch the mesh uniformly along the X, Y, and Z axes. Negative side
     * of each axis is offseted negatively. Positive side of each side is 
     * offseted positively. Normals are not affected.
     *
     * @param aXLimit
     * @param aXOffset
     * @param aYLimit
     * @param aYOffset
     * @param aZLimit
     * @param aZOffset
     */
    IMPORT_C virtual void StretchUniformly(TReal32 aXLimit, TReal32 aXOffset, 
                                           TReal32 aYLimit, TReal32 aYOffset,
                                           TReal32 aZLimit, TReal32 aZOffset) __SOFTFP;
                                
    /**
     * Scale the mesh uniformly along each axis. Normals are also scaled and
     * renormalized.
     *
     * @param aX  X scaling factor.
     * @param aY  Y scaling factor.
     * @param aZ  Z scaling factor.
     */                                
    IMPORT_C virtual void ScaleUniformly(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    /**
     * Draw the mesh.
     */     
    virtual void Draw(CHuiGc& aGc, const THuiImage* aImage = 0, 
                      const THuiImage* aSecondaryImage = 0,
                      TReal32 aSecondaryAlpha = 0.0) const __SOFTFP;
        

    // Implementation of MHuiSessionObject
    
    /**
     * Gets the object type.
     */
    IMPORT_C TType Type() const;

    /**
     * Gets the session id for object.
     */
    IMPORT_C TInt SessionId() const;

    /**
     * Sets the session id for object.
     */
    IMPORT_C void SetSessionId(TInt aSessionId);

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

private:

    /** Mesh type implemented by this instance. */
    THuiMeshType iMeshType;

    /** Session id */
    TInt iSessionId;
    
    };

#endif // __HUIMESH_H__
