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
#include <alf/alfmappingfunctions.h>

#include "testplatalfvisual.h"

// CONSTANTS
const TInt KBufSize = 64;
const TInt KNumber = 1000000;

// FORWARD DECLARATION
/*This class test protected functions of CAlfLineVisual*/
class CTestCAlfLineVisual : public CAlfLineVisual
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfLineVisual();
    /**
    * Destructor.
    */
    ~CTestCAlfLineVisual();
    /**
    * TestProtectedFuncForVisual test protected functions of CTestCAlfLineVisual
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfLineVisual::CTestCAlfLineVisual
// -----------------------------------------------------------------------------
//
CTestCAlfLineVisual::CTestCAlfLineVisual(){}

// -----------------------------------------------------------------------------
// CTestCAlfLineVisual::~CTestCAlfLineVisual
// -----------------------------------------------------------------------------
//
CTestCAlfLineVisual::~CTestCAlfLineVisual(){}

// -----------------------------------------------------------------------------
// CTestCAlfLineVisual::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfLineVisual::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLineVisualFullConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLineVisualFullConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLineVisualFullConstructorL, "TestAlfLineVisualFullConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLineVisualFullConstructorL );
    // Print to log file
    iLog->Log( KTestAlfLineVisualFullConstructorL );
    
    CAlfLineVisual* vVisual = new ( ELeave ) CAlfLineVisual;
    CleanupStack::PushL( vVisual );
    STIF_ASSERT_NOT_NULL( vVisual );
    vVisual->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( vVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLineVisualAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLineVisualAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLineVisualAddNewL, "TestAlfLineVisualAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLineVisualAddNewL );
    // Print to log file
    iLog->Log( KTestAlfLineVisualAddNewL );
    
    CAlfLineVisual* vVisual = CAlfLineVisual::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( vVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLineVisualSetAndGetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLineVisualSetAndGetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLineVisualSetAndGetL, "TestAlfLineVisualSetAndGetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLineVisualSetAndGetL );
    // Print to log file
    iLog->Log( KTestAlfLineVisualSetAndGetL );
    
    CAlfLineVisual* vVisual = CAlfLineVisual::AddNewL( *iAlfCtl );
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    vVisual->SetPath( vPath, EAlfDoesNotHaveOwnership );
    CAlfCurvePath* pathResult = vVisual->Path();
    STIF_ASSERT_EQUALS( (TInt)vPath , (TInt)pathResult );
    const CAlfTexture* texture = iAlfEnv->TextureManager().Texture( 0 );
    TAlfImage vImage( *texture );
    vVisual->SetImage( vImage );
    vVisual->Image();
    CAlfAverageMappingFunction* mappingFunction = 
    CAlfAverageMappingFunction::NewL( *iAlfEnv );
    CleanupStack::PushL( mappingFunction );
    vVisual->SetAlphaFunction( mappingFunction );
    CleanupStack::PopAndDestroy( mappingFunction );
    mappingFunction = CAlfAverageMappingFunction::NewL( *iAlfEnv );
    CleanupStack::PushL( mappingFunction );
    vVisual->SetWidthFunction( mappingFunction );
    CleanupStack::PopAndDestroy( mappingFunction );
    
    TAlfTimedValue valueSet;
    valueSet.SetValueNow( 0 );
    valueSet.SetTarget( 1, KNumber );
    vVisual->SetThickness(valueSet);
    const TAlfTimedValue& valueGet = vVisual->Thickness();
    STIF_ASSERT_EQUALS( (float)0.0, valueGet.ValueNow() );
    STIF_ASSERT_EQUALS( (float)1.0, valueGet.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.TimeToTargetinMilliSeconds() );
 
    valueSet.SetValueNow( 0 );
    valueSet.SetTarget( 1, KNumber );
    vVisual->SetShadowThickness(valueSet);
    
    const TAlfTimedValue& valueGet1 = vVisual->ShadowThickness();
    STIF_ASSERT_EQUALS( (float)0.0, valueGet1.ValueNow() );
    STIF_ASSERT_EQUALS( (float)1.0, valueGet1.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet1.TimeToTargetinMilliSeconds() );
    
    valueSet.SetValueNow( 0 );
    valueSet.SetTarget( 1, KNumber );
    vVisual->SetStartPos( valueSet );
    
    const TAlfTimedValue& valueGet2 = vVisual->StartPos();
    STIF_ASSERT_EQUALS( (float)0.0, valueGet2.ValueNow() );
    STIF_ASSERT_EQUALS( (float)1.0, valueGet2.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet2.TimeToTargetinMilliSeconds() );
    
    valueSet.SetValueNow( 0 );
    valueSet.SetTarget( 1, KNumber );
    vVisual->SetEndPos(valueSet);
    
    const TAlfTimedValue& valueGet3 = vVisual->EndPos();
    STIF_ASSERT_EQUALS( (float)0.0, valueGet3.ValueNow() );
    STIF_ASSERT_EQUALS( (float)1.0, valueGet3.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet3.TimeToTargetinMilliSeconds() );
    
    TRgb vRgb( KRgbBlue );
    vVisual->SetColor( vRgb );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLineVisualForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLineVisualForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLineVisualForVisualL, "TestAlfLineVisualForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLineVisualForVisualL );
    // Print to log file
    iLog->Log( KTestAlfLineVisualForVisualL );
    
    CAlfLineVisual* vVisual = CAlfLineVisual::AddNewL( *iAlfCtl );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vVisual->FindTag( vBuf );
    vVisual->UpdateChildrenLayout();
    vVisual->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLineVisualProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLineVisualProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLineVisualProtectedFuncL, "TestAlfLineVisualProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLineVisualProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfLineVisualProtectedFuncL );
    
    CTestCAlfLineVisual* vLineVisual = new ( ELeave ) CTestCAlfLineVisual;
    CleanupStack::PushL( vLineVisual );
    vLineVisual->ConstructL( *iAlfCtl );
    vLineVisual->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vLineVisual );
    
    return KErrNone;
    }

// End of file

