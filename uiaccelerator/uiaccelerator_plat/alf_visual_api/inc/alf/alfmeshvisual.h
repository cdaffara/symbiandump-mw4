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



#ifndef C_ALFMESHVISUAL_H
#define C_ALFMESHVISUAL_H

#include <alf/alfvisual.h>
#include <alf/alfmesh.h>

class TAlfImage;
class CAlfControl;
class CAlfLayout;
class CAlfProceduralMesh;
class CAlfM3GMesh;

/**
 *  Mesh visual
 *
 *  Mesh visual allows integration of 3D shapes (meshes) with the user interface.
 *  The mesh visual always contains one CAlfMesh - derived concrete mesh implementation.
 *  By default the mesh visual contains a CAlfProceduralMesh object. The concrete
 *  mesh object can be queried through the Mesh() - interface and can be recreated
 *  using the CreateMeshL() - interface.
 *
 *  Usage:
 *  @code
 *  //Create Mesh Visual
 *   CAlfMeshVisual* meshVisual = CAlfMeshVisual::AddNewL( control );
 * 
 *  //Create M3GMesh and get it
 *  meshVisual->CreateMeshL( EAlfMeshTypeM3G  );
 *  CAlfM3GMesh* m3gmesh = mMeshVisual->M3GMesh();
 *  
 *  //Create ProceduralMesh and get it
 *  meshVisual->CreateMeshL( EAlfMeshTypeProcedural  );
 *  CAlfProceduralMesh* proceduralmesh = mMeshVisual->ProceduralMesh();
 * 
 *  //Set MeshVisual Yaw angle
 *  TAlfTimedValue tv( 2 );
 *  tv.setTarget( 20, 500 );
 *  meshVisual->SetYawAngle( tv );
    
 *  @endcode
 * 
 *  @see CAlfM3GMesh
 *  @see CAlfProceduralMesh
 * 
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfMeshVisual : public CAlfVisual
    {
public:
    
    /**
     * Constructor, which gives ownership to the control.
     *
     * @param aOwnerControl The control
     * @param aParentLayout If given, the parent layout.
     * @return New instance. Ownership NOT transreffed (owned by control)
     */
    IMPORT_C static CAlfMeshVisual* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfMeshVisual();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfMeshVisual();
    
    /* Methods. */
	/**
	 * Creates CAlfMesh - derived object and attaches it to this CAlfMeshVisual.
	 * This method will destroy the mesh object that was contained in this
	 * mesh visual before. The created mesh object can be queried through
	 * Mesh() - method.
	 *
	 * @param aMeshType Concrete mesh type to be created. Possible values are
	 * e.g EAlfMeshTypeProcedural and EAlfMeshTypeM3G.
	 * 
	 */
	IMPORT_C void CreateMeshL(TInt aMeshType);
		
    IMPORT_C void SetImage(const TAlfImage& aImage);
    IMPORT_C const TAlfImage& Image() const;

    /**
     * Secondary image is used for crossfading.
     */    
    IMPORT_C void SetSecondaryImage(const TAlfImage& aImage);
    IMPORT_C const TAlfImage& SecondaryImage() const;

    IMPORT_C void SetSpecularImage(const TAlfImage& aImage);
    IMPORT_C const TAlfImage& SpecularImage() const;

    /**
     * Returns the mesh object contained in this mesh visual.
     *
     * @return  Mesh.
     */
    IMPORT_C CAlfMesh& Mesh();
    
    /**
     * Returns pointer to the procedural mesh contained in this mesh visual.
     * If contained mesh isn't a procedural mesh, NULL is returned.
     * 
     * @return Pointer to the procedural mesh.
     */
 	IMPORT_C CAlfProceduralMesh* ProceduralMesh() const; 

    /**
     * Returns pointer to the M3G mesh contained in this mesh visual.
     * If contained mesh isn't an M3G mesh, NULL is returned.
     * 
     * @return Pointer to the M3G mesh.
     */
 	IMPORT_C CAlfM3GMesh* M3GMesh() const;     
    
    IMPORT_C TAlfTimedValue SecondaryAlpha() const;
    IMPORT_C void SetSecondaryAlpha( const TAlfTimedValue& aSecondaryAlpha );
    
    IMPORT_C TAlfTimedValue YawAngle() const;
    IMPORT_C void SetYawAngle( const TAlfTimedValue& aYawAngle );
    
    IMPORT_C TAlfTimedValue PitchAngle() const;
    IMPORT_C void SetPitchAngle( const TAlfTimedValue& aPitchAngle );
    
    IMPORT_C TAlfTimedValue Scale() const;
    IMPORT_C void SetScale( const TAlfTimedValue& aScale );
   
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);


protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

    
private:

    struct TMeshVisualPrivateData;
    TMeshVisualPrivateData* iMeshVisualData;

    };


#endif // C_ALFMESHVISUAL_H
