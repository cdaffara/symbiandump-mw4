/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test functions for alfproceduralmesh.h
*
*/



// [INCLUDE FILES]
#include <alf/alfproceduralmesh.h>
#include <alf/alfmeshvisual.h>

#include "testplatalfvisual.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestProMeshMakeMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestProMeshMakeMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestProMeshMakeMethodsL, "TestProMeshMakeMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestProMeshMakeMethodsL );
    // Print to log file
    iLog->Log( KTestProMeshMakeMethodsL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
/* Not supported at the moment
    CAlfMeshVisual* meshVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );
    meshVisual->CreateMeshL( EAlfMeshTypeProcedural );
    CAlfProceduralMesh* proMesh = meshVisual->ProceduralMesh();
    STIF_ASSERT_NOT_NULL(  proMesh );
    proMesh->MakeCubeL( 1, 1, 1, 1 );
    proMesh->MakeSphereL( 1, 1, 1 );
    proMesh->MakeTorusL( 1, 1, 1, 1, 1 );
*/    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestProMeshMaterialMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestProMeshMaterialMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestProMeshMaterialMethodsL, "TestProMeshMaterialMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestProMeshMaterialMethodsL );
    // Print to log file
    iLog->Log( KTestProMeshMaterialMethodsL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
/* Not supported at the moment
    CAlfMeshVisual* meshVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );
    meshVisual->CreateMeshL( EAlfMeshTypeProcedural );
    CAlfProceduralMesh* proMesh = meshVisual->ProceduralMesh();
    STIF_ASSERT_NOT_NULL(  proMesh );
    TAlfMaterial material = proMesh->Material();
    proMesh->SetMaterial( material );
*/    
    return KErrNone;
    }


//  [End of File]
