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

// CONSTANT
const TInt KBufSize = 64;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfM3GMeshLoadSceneL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfM3GMeshLoadSceneL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfM3GMeshLoadSceneL, "TestAlfM3GMeshLoadSceneL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfM3GMeshLoadSceneL );
    // Print to log file
    iLog->Log( KTestAlfM3GMeshLoadSceneL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
    return KErrNone;    
/* Not supported at the moment
    CAlfMeshVisual* vVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );
    vVisual->CreateMeshL( EAlfMeshTypeM3G );
    CAlfM3GMesh* vMesh = vVisual->M3GMesh();
    _LIT( KM3gFile, "testm3g.m3g");
    TBuf<KBufSize> buf( KM3gFile );
    TRAPD( err, vMesh->LoadSceneL( buf ) );
    if ( err != KErrNone )
        {
        return KErrNone;
        }
    else
        {
        return KErrNone;
        }
*/
    }

// End of file


