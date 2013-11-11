/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef C_ALFMESH_H
#define C_ALFMESH_H

#include <e32base.h>
#include <alf/alfmaterial.h>

class CAlfGenComponent;
class TAlfImage;

/**
 * Mesh types.
 * These types identify concrete mesh - derived object types.
 */
enum TAlfMeshType
    {
    /** Procedural mesh. @see CAlfProceduralMesh */
    EAlfMeshTypeProcedural,
    
    /** M3G mesh. @see CAlfM3GMesh */
    EAlfMeshTypeM3G
    };


/**
 * Mesh class.
 * Base class for M3G and Procedural Mesh
 * 
 * Usage:
 * @code
 * //Create procedural Mesh
 * meshVisual->CreateMeshL( EAlfMeshTypeProcedural ); 
 * CAlfProceduralMesh* mesh = meshVisual->ProceduralMesh();
 * 
 * //Make it a cube
 *  mesh->MakeCubeL( 30000, 20000, 25000, 3000 );
 * 
 * //Stretch Mesh
 *  mesh->StretchUniformly( xLimit, xOffset, yLimit, yOffset, zLimit, zOffset );
 *  
 * @endcode
 * 
 * @see CAlfMeshVisual::CreateMeshL()
 * @see CAlfProceduralMesh
 * @see CAlfM3GMesh
 *
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfMesh ): public CBase
    {

public:

    static CAlfMesh* NewL( CAlfGenComponent& aComms );

    ~CAlfMesh();

    /**
     * Determines the concrete mesh type this CAlfMesh instance implements.
     *
     * @return TAlfMeshType The mesh type implemented by this object,  
     * possible values are e.g EAlfMeshTypeProcedural and EAlfMeshTypeM3G, but
     * in the future also other types may be supported.
     */
    IMPORT_C TInt MeshType() const;

    /**
     * Resets the mesh. All contents are destroyed.
     */
    IMPORT_C void Reset();
    
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
    IMPORT_C void StretchUniformly(
        TReal32 aXLimit, 
        TReal32 aXOffset, 
        TReal32 aYLimit, 
        TReal32 aYOffset,
        TReal32 aZLimit, 
        TReal32 aZOffset) __SOFTFP;
                                
    /**
     * Scale the mesh uniformly along each axis. Normals are also scaled and
     * renormalized.
     *
     * @param aX  X scaling factor.
     * @param aY  Y scaling factor.
     * @param aZ  Z scaling factor.
     */                                
    IMPORT_C void ScaleUniformly(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;
    
	/**
	 * Returns number of ALF animation controllers
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
    
    
protected:

    CAlfMesh();

    /**
     * The second phase constructor. CAlfMesh - derived objects
     * have to pass the mesh type they represent in a call to
     * this method.
     */
    void ConstructL( CAlfGenComponent& aComms, TInt aMeshType = EAlfMeshTypeProcedural );

private:

    void StretchUniformlyL(
        TReal32 aXLimit, 
        TReal32 aXOffset, 
        TReal32 aYLimit, 
        TReal32 aYOffset,
        TReal32 aZLimit, 
        TReal32 aZOffset);
                                
    void ScaleUniformlyL(TReal32 aX, TReal32 aY, TReal32 aZ);

	virtual TInt DoAnimationControllerCount() const;
	
	virtual void SetAnimationPositionL(TInt aControllerId, TReal32 aTarget, TInt aTime);

	virtual void StartAnimationControllerL(TInt aControllerId);
	
	virtual void StopAnimationControllerL(TInt aControllerId);


private: // data

    struct TMeshPrivateData;
    TMeshPrivateData* iData;

    };


#endif // C_ALFMESH_H
