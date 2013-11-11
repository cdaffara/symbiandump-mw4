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
* Description:  test functions for alfvisual.h
*
*/



// [INCLUDE FILES]
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfMeshSetAndGetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMeshSetAndGetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMeshSetAndGetL, "TestAlfMeshSetAndGetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMeshSetAndGetL );
    // Print to log file
    iLog->Log( KTestAlfMeshSetAndGetL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
/* Not supported at the moment
    CAlfMeshVisual* vVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );
    vVisual->CreateMeshL( EAlfMeshTypeM3G );
    CAlfMesh& vMesh = vVisual->Mesh();
    vMesh.MeshType();
    vMesh.StretchUniformly( 0, 0, 0, 0, 0, 0 );
    vMesh.ScaleUniformly( 0, 0, 0 );
    vMesh.AnimationControllerCount();
    vMesh.SetAnimationPosition( iAlfCtl->Id(), 0, 0 );
    vMesh.StartAnimationController( iAlfCtl->Id() );
    vMesh.StopAnimationController( iAlfCtl->Id() );
    vMesh.Reset();
*/    
    return KErrNone;
    }

// End of file

