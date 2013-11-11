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
/*This class test protected functions of CAlfImageVisual*/
class CTestCAlfImageVisual : public CAlfImageVisual
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfImageVisual();
    /**
    * Destructor.
    */
    ~CTestCAlfImageVisual();
    /**
    * TestProtectedFuncForVisual test protected functions of CAlfImageVisual
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfImageVisual::CTestCAlfImageVisual
// -----------------------------------------------------------------------------
//
CTestCAlfImageVisual::CTestCAlfImageVisual(){}

// -----------------------------------------------------------------------------
// CTestCAlfImageVisual::~CTestCAlfImageVisual
// -----------------------------------------------------------------------------
//
CTestCAlfImageVisual::~CTestCAlfImageVisual(){}

// -----------------------------------------------------------------------------
// CTestCAlfFlowLayout::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfImageVisual::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgVisualFullConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgVisualFullConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgVisualFullConstructorL, "TestAlfImgVisualFullConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgVisualFullConstructorL );
    // Print to log file
    iLog->Log( KTestAlfImgVisualFullConstructorL );
    
    CAlfImageVisual* vVisual = new ( ELeave ) CAlfImageVisual;
    CleanupStack::PushL( vVisual );
    STIF_ASSERT_NOT_NULL( vVisual );
    vVisual->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( vVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgVisualAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgVisualAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgVisualAddNewL, "TestAlfImgVisualAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgVisualAddNewL );
    // Print to log file
    iLog->Log( KTestAlfImgVisualAddNewL );
    
    CAlfImageVisual* vVisual = CAlfImageVisual::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( vVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgVisualSetAndGetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgVisualSetAndGetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgVisualSetAndGetL, "TestAlfImgVisualSetAndGetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgVisualSetAndGetL );
    // Print to log file
    iLog->Log( KTestAlfImgVisualSetAndGetL );
    
    CAlfImageVisual* vVisual = CAlfImageVisual::AddNewL( *iAlfCtl );
    
    TRgb vRgb( KRgbBlue );
    vVisual->SetColor( vRgb );
    vVisual->SetColorMode( CAlfImageVisual::EColorModulate );
    vVisual->SetStretch( 0, 1 );
    vVisual->SetStretchMode( CAlfGc::EStretchHorizontal );
    vVisual->SetDropShadow( 0 );
    TAlfXYMetric vXYMetric;
    vVisual->SetDropShadow( vXYMetric );
    TAlfMetric vMetric;
    vVisual->SetDropShadow( vMetric );
    TAlfTimedValue vValue( 0 );
    vVisual->SetTurnAngle( vValue );
    vVisual->TurnAngle();
    vVisual->SetTurnAngle( vValue );
    vVisual->SetScale( vValue );
    vVisual->Scale();
    TAlfTimedPoint vPoint( 0, 0 );
    vVisual->SetOffset( vPoint );
    vVisual->Offset();
    vVisual->SetScaleMode( CAlfImageVisual::EScaleNormal );
    vVisual->ScaleMode();
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    const CAlfTexture* vText = vTextManager->Texture( 0 );
    TAlfImage vFirstImage( *vText );
    TAlfImage vSecImage( vUid, EAknsAppIconTypeList, 
            TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    
    vVisual->SetImage( vFirstImage );
    vVisual->SetSecondaryImage( vSecImage );
    TAlfTimedValue vAlpha( 1 );
    vVisual->SetSecondaryAlpha( vAlpha );
    vVisual->Image();
    vVisual->SecondaryImage();
    vVisual->SecondaryAlpha();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgVisualForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgVisualForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgVisualForVisualL, "TestAlfImgVisualForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgVisualForVisualL );
    // Print to log file
    iLog->Log( KTestAlfImgVisualForVisualL );
    
    CAlfImageVisual* vVisual = CAlfImageVisual::AddNewL( *iAlfCtl );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vVisual->FindTag( vBuf );
    vVisual->UpdateChildrenLayout();
    vVisual->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgVisualProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgVisualProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgVisualProtectedFuncL, "TestAlfImgVisualProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgVisualProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfImgVisualProtectedFuncL );
    
    CTestCAlfImageVisual* vImgVisual = new ( ELeave ) CTestCAlfImageVisual;
    CleanupStack::PushL( vImgVisual );
    vImgVisual->ConstructL( *iAlfCtl );
    vImgVisual->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vImgVisual );
    
    return KErrNone;
    }

// End of file

