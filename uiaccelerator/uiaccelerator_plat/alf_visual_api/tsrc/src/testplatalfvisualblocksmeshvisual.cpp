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

// CONSTANTS
const TInt KBufSize = 64;

// FORWARD DECLARATION
/*This class test protected functions of CAlfMeshVisual*/
class CTestCAlfMeshVisual : public CAlfMeshVisual
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfMeshVisual();
    /**
    * Destructor.
    */
    ~CTestCAlfMeshVisual();
    /**
    * TestProtectedFuncForVisual test protected functions of CTestCAlfMeshVisual
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfMeshVisual::CTestCAlfMeshVisual
// -----------------------------------------------------------------------------
//
CTestCAlfMeshVisual::CTestCAlfMeshVisual(){}

// -----------------------------------------------------------------------------
// CTestCAlfMeshVisual::~CTestCAlfMeshVisual
// -----------------------------------------------------------------------------
//
CTestCAlfMeshVisual::~CTestCAlfMeshVisual(){}

// -----------------------------------------------------------------------------
// CTestCAlfMeshVisual::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfMeshVisual::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfMeshVisualFullConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMeshVisualFullConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMeshVisualFullConstructorL, "TestAlfMeshVisualFullConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMeshVisualFullConstructorL );
    // Print to log file
    iLog->Log( KTestAlfMeshVisualFullConstructorL );
    
    CAlfMeshVisual* vVisual = new ( ELeave ) CAlfMeshVisual;
    CleanupStack::PushL( vVisual );
    STIF_ASSERT_NOT_NULL( vVisual );
    vVisual->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( vVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfMeshVisualAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMeshVisualAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMeshVisualAddNewL, "TestAlfMeshVisualAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMeshVisualAddNewL );
    // Print to log file
    iLog->Log( KTestAlfMeshVisualAddNewL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
/* Not supported at the moment
    CAlfMeshVisual* vVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );    
    STIF_ASSERT_NOT_NULL( vVisual );
*/
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfMeshVisualSetAndGetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMeshVisualSetAndGetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMeshVisualSetAndGetL, "TestAlfMeshVisualSetAndGetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMeshVisualSetAndGetL );
    // Print to log file
    iLog->Log( KTestAlfMeshVisualSetAndGetL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
/* Not supported at the moment
    CAlfMeshVisual* vVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );
    vVisual->CreateMeshL( EAlfMeshTypeM3G );
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    const CAlfTexture* vText = vTextManager->Texture( 0 );
    TAlfImage vImage( *vText );
    vVisual->SetImage( vImage );
    vVisual->Image();
    TAlfImage vSecImage( vUid, EAknsAppIconTypeList, 
                TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    vVisual->SetSecondaryImage( vSecImage );
    vVisual->SecondaryImage();
    TUid vSecUid = { 0x00000001 };
    TAlfImage vNewImage( vSecUid, EAknsAppIconTypeList, 
                    TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    vVisual->SetSpecularImage( vNewImage );
    vVisual->SpecularImage();
    vVisual->Mesh();
    vVisual->ProceduralMesh(); 
    vVisual->M3GMesh();
    const TAlfTimedValue vValue( 1 );
    vVisual->SetSecondaryAlpha( vValue );
    vVisual->SecondaryAlpha();
    vVisual->SetYawAngle( vValue );
    vVisual->YawAngle();
    vVisual->SetPitchAngle( vValue );
    vVisual->PitchAngle();
    vVisual->SetScale( vValue );
    vVisual->Scale();
*/
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfMeshVisualForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMeshVisualForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMeshVisualForVisualL, "TestAlfMeshVisualForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMeshVisualForVisualL );
    // Print to log file
    iLog->Log( KTestAlfMeshVisualForVisualL );
    
    // Mesh not supported at the moment
    STIF_ASSERT_LEAVES_WITH( KErrNotSupported,
                             CAlfMeshVisual::AddNewL( *iAlfCtl ) );
/* Not supported at the moment
    CAlfMeshVisual* vVisual = CAlfMeshVisual::AddNewL( *iAlfCtl );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vVisual->FindTag( vBuf );
    vVisual->UpdateChildrenLayout();
    vVisual->RemoveAndDestroyAllD();
*/
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfMeshVisualProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMeshVisualProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMeshVisualProtectedFuncL, "TestAlfMeshVisualProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMeshVisualProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfMeshVisualProtectedFuncL );
    
    CTestCAlfMeshVisual* vMeshVisual = new ( ELeave ) CTestCAlfMeshVisual;
    CleanupStack::PushL( vMeshVisual );
    vMeshVisual->ConstructL( *iAlfCtl );
    vMeshVisual->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vMeshVisual );
    
    return KErrNone;
    }

// End of file

