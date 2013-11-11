/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Procedural mesh definition
*
*/



#ifndef C_ALFPROCEDURALMESH_H
#define C_ALFPROCEDURALMESH_H

#include <alf/alfmesh.h>

/**
 *  Procedural mesh support.
 *
 *  This is a CAlfMesh - derived mesh implementation that provides support
 *  for procedural meshes. There are collection of shapes that can be procedurally
 *  constructed using this object. This mesh can be used through CAlfMeshVisual
 *  and created with CAlfMeshVisual::CreateMeshL() method with argument EAlfMeshTypeProcedural.
 * Usage:
 * @code
 *  //Create Procedural Mesh and get it from Mesh Visual
 *  meshVisual->CreateMeshL( EAlfMeshTypeProcedural );                
 *  CAlfProceduralMesh* mesh = meshVisual->ProceduralMesh();
 * 
 *  //Make a cube
 *  int sizeX = 30000;
 *  int sizeY = 20000; 
 *  int sizeZ = 25000;
 *  int edgeRadius = 3000;
 *  mesh->MakeCubeL( sizeX, sizeY, sizeZ, edgeRadius );
 * 
 *  //Make a sphere
 *  int radius = 15000;
 *  int columns = 20;
 *  int rows = 20;
 *  mesh->MakeSphereL( radius, columns, rows );
 *           
 *  //Make a Torus
 *  int mainRadius = 15000;
 *  int outerRadius = 2000;
 *  int mainSegments = 20;
 *  int outerSegments = 20;
 *  float segmentAngleOffset = 0.0;
 *  mesh->MakeTorusL( mainRadius, outerRadius, mainSegments, outerSegments, segmentAngleOffset );
 *  
 *  //Material setting
 *  TAlfMaterial material;
 * 	material.iColor = Color( 128 );
 *  material.iPreset = EAlfMaterialShadow;
 *  const float KValueNow = 0.2;
 * 	const float KTargetValue = 0.5;
 * 	TAlfTimedValue tv( KValueNow );
 * 	tv.SetTarget( KTargetValue, 500 );    
 * 	material.iSpecular = tv;
 *  mesh->SetMaterial( material );               
 * 
 *  //Set glass preset with speculer image set.
 *  material.iSpecularImage = TAlfImage();
 *  material.iPreset = EAlfMaterialGlass;
 *  mesh->SetMaterial(material);  
 * 
 * @endcode
 * @see TAlfMaterial
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfProceduralMesh ): public CAlfMesh
    {

public:

    /**
     * Constructs a new CAlfProceduralMesh instance.
     */
    static CAlfProceduralMesh* NewL( CAlfGenComponent& aComms );

    /**
     * Destructor
     */
    ~CAlfProceduralMesh();

    /**
     * Creates a cube mesh.
     * @todo move.
     * @param aSizeX
     * @param aSizeY
     * @param aSizeZ
     * @param aEdgeRadius
     */
    IMPORT_C void MakeCubeL(
        TReal32 aSizeX, 
        TReal32 aSizeY, 
        TReal32 aSizeZ, 
        TReal32 aEdgeRadius) __SOFTFP;

    /**
     * Creates a sphere mesh.
     * @todo move
     * @param aRadius
     * @param aColumns
     * @param aRows
     */
    IMPORT_C void MakeSphereL(
        TReal32 aRadius, 
        TInt aColumns, 
        TInt aRows) __SOFTFP;

    /** 
     * Creates a torus mesh.
     * @todo move
     * @param aMainRadius
     * @param aOuterRadius
     * @param aMainSegments
     * @param aOuterSegments
     * @param aSegmentAngleOffset
     */
    IMPORT_C void MakeTorusL(
        TReal32 aMainRadius, 
        TReal32 aOuterRadius, 
        TInt aMainSegments, 
        TInt aOuterSegments,
        TReal32 aSegmentAngleOffset = 0) __SOFTFP;

    /**
     * Returns the material definition used by this procedural mesh.
     *
     * @return TAlfMaterial material definition.
     */
    IMPORT_C const TAlfMaterial& Material() const;
    
    /**
     * Sets the material definition used by this procedural mesh.
     *
     * @param aMaterial Material definition to be applied to this mesh.
     */    
    IMPORT_C void SetMaterial( const TAlfMaterial& aMaterial );

    /**
     * Sets the specular image of the mesh material.
     */
    void SetSpecularImage(const TAlfImage& aImage);

private:

    /**
     * Constructor
     */
    CAlfProceduralMesh();

    /**
     * Second phase constructor
     */
    void ConstructL( CAlfGenComponent& aComms );

private: // data

    struct TProceduralMeshPrivateData;
    TProceduralMeshPrivateData* iData;

    };


#endif // C_ALFPROCEDURALMESH_H
