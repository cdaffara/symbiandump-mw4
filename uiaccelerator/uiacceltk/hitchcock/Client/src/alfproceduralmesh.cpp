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
* Description:   Procedural 3D Mesh
*
*/



#include "alf/alfproceduralmesh.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"
#include "alf/alfconstants.h"
#include "alf/alfenv.h"
#include "alfskinnableimage.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfProceduralMesh::TProceduralMeshPrivateData
    {
    CAlfGenComponent* iComms; // Not owned
    TAlfMaterial iMaterial;      
    CAlfSkinnableImage* iMaterialTextureImage;
    CAlfSkinnableImage* iMaterialSpecularImage;
    };

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfProceduralMesh::CAlfProceduralMesh()
    {
    }


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAlfProceduralMesh::ConstructL( CAlfGenComponent& aComms )
    {
    // Construct the base class
    CAlfMesh::ConstructL(aComms, EAlfMeshTypeProcedural);
    
    // Create object data.
    iData = new (ELeave) TProceduralMeshPrivateData;
    
    // Zero all object data
    iData->iComms = NULL;
    iData->iMaterialTextureImage = NULL;
    iData->iMaterialSpecularImage = NULL;
    
    // Fill data
    iData->iComms = &aComms;
    iData->iMaterial.iPreset = EAlfMaterialChrome;  
    
    CAlfEnv* env = CAlfEnv::Static();
    iData->iMaterialTextureImage = new (ELeave) CAlfSkinnableImage(env);
    iData->iMaterialSpecularImage = new (ELeave) CAlfSkinnableImage(env);  
    
    // Create input and output buffers for inter-process communication
    TBuf8<1> outDummy;
    TInt param = EAlfMeshTypeProcedural;
    TPckgC<TInt> buf(param);
        
    // Call a synchronous command to create an associated server-side procedural mesh.
    TInt err = iData->iComms->DoSynchronousCmd(EAlfMeshVisualCreateMesh, buf, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfProceduralMesh::ConstructL leave error %d", err )
        User::Leave( err );
        }   
    }


// ---------------------------------------------------------------------------
// Exposed constructor
// ---------------------------------------------------------------------------
//
CAlfProceduralMesh* CAlfProceduralMesh::NewL( CAlfGenComponent& aComms )
    {
    CAlfProceduralMesh* self = new( ELeave ) CAlfProceduralMesh;
    CleanupStack::PushL( self );
    self->ConstructL( aComms );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfProceduralMesh::~CAlfProceduralMesh()
    {
    if ( iData )
        {
        delete iData->iMaterialTextureImage;
        iData->iMaterialTextureImage = NULL;
        
        delete iData->iMaterialSpecularImage;
        iData->iMaterialSpecularImage = NULL;
        }
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Forms a cube mesh to this procedural mesh.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfProceduralMesh::MakeCubeL(
        TReal32 aSizeX, 
        TReal32 aSizeY, 
        TReal32 aSizeZ, 
        TReal32 aEdgeRadius) __SOFTFP
    {
    RArray<TReal32> array;
    CleanupClosePushL( array );
    
    array.AppendL( aSizeX );
    array.AppendL( aSizeY );
    array.AppendL( aSizeZ );
    array.AppendL( aEdgeRadius );
    
    HBufC8* buffer = ExternalizeLC(array);
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshMakeCube, *buffer, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfProceduralMesh::MakeCubeL leave error %d", err )
        User::Leave( err );
        }
        
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &array );
    }

// ---------------------------------------------------------------------------
// Forms a sphere mesh to this procedural mesh.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfProceduralMesh::MakeSphereL(
        TReal32 aRadius, 
        TInt aColumns, 
        TInt aRows) __SOFTFP
    {
    RArray<TReal32> array;
    CleanupClosePushL( array );
    
    array.AppendL( aRadius );
    array.AppendL( (TReal32)aColumns );
    array.AppendL( (TReal32)aRows );
    
    HBufC8* buffer = ExternalizeLC(array);
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshMakeSphere, *buffer, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfProceduralMesh::MakeSphereL leave error %d", err )
        User::Leave( err );
        }
        
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &array );
    }

// ---------------------------------------------------------------------------
// Forms a torus mesh to this procedural mesh.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfProceduralMesh::MakeTorusL(
        TReal32 aMainRadius, 
        TReal32 aOuterRadius, 
        TInt aMainSegments, 
        TInt aOuterSegments,
        TReal32 aSegmentAngleOffset ) __SOFTFP
    {
    RArray<TReal32> array;
    CleanupClosePushL( array );
    
    array.AppendL( aMainRadius );
    array.AppendL( aOuterRadius );
    array.AppendL( (TReal32)aMainSegments );
    array.AppendL( (TReal32)aOuterSegments );
    array.AppendL( aSegmentAngleOffset );
    
    HBufC8* buffer = ExternalizeLC(array);
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshMakeTorus, *buffer, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfProceduralMesh::MakeTorusL leave error %d", err )
        User::Leave( err );
        }
    
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &array );
    }
    
// ---------------------------------------------------------------------------
// Returns the material definition of this mesh.
// ---------------------------------------------------------------------------
//
EXPORT_C const TAlfMaterial& CAlfProceduralMesh::Material() const
    {
    TAlfMaterialParams params; 
    TPckg<TAlfMaterialParams> buf(params);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshMaterial, KNullDesC8, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfProceduralMesh::Material panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iMaterial.iSpecular = params.iSpecular;
    iData->iMaterial.iTextureImage = iData->iMaterialTextureImage->Image();
    iData->iMaterial.iSpecularImage = iData->iMaterialSpecularImage->Image();
    
    return iData->iMaterial;
    }
    
// ---------------------------------------------------------------------------
// Sets the material definition used with this mesh.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfProceduralMesh::SetMaterial( const TAlfMaterial& aMaterial )
    {
    iData->iMaterialTextureImage->SetImage( aMaterial.iTextureImage );
    iData->iMaterialSpecularImage->SetImage( aMaterial.iSpecularImage );

    TAlfMaterialParams params;
    TPckgC<TAlfMaterialParams> buf(params);
    
    params.iPreset = aMaterial.iPreset;
    params.iColor = aMaterial.iColor;
    params.iTextureImage.iTextureHandle = 
        iData->iMaterialTextureImage->Image().HasTexture() ? 
            iData->iMaterialTextureImage->Image().Texture().ServerHandle() 
            : 0 ;
    params.iTextureImage.iTl = iData->iMaterialTextureImage->Image().TimedTopLeft();
    params.iTextureImage.iBr = iData->iMaterialTextureImage->Image().TimedBottomRight();
    params.iSpecularImage.iTextureHandle = 
        iData->iMaterialSpecularImage->Image().HasTexture() ? 
            iData->iMaterialSpecularImage->Image().Texture().ServerHandle() 
            : 0 ;
    params.iSpecularImage.iTl = iData->iMaterialSpecularImage->Image().TimedTopLeft();
    params.iSpecularImage.iBr = iData->iMaterialSpecularImage->Image().TimedBottomRight();
    params.iSpecular = aMaterial.iSpecular;
    
    TInt err = iData->iComms->DoCmdNoReply( EAlfMeshSetMaterial, buf );

    if ( err == KErrNone )
        {
        iData->iMaterial = aMaterial;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfProceduralMesh::SetMaterial panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Sets specular image of the material.
// ---------------------------------------------------------------------------
//     
void CAlfProceduralMesh::SetSpecularImage(const TAlfImage& aImage)
    {
    iData->iMaterial.iSpecularImage = aImage;
    }    
