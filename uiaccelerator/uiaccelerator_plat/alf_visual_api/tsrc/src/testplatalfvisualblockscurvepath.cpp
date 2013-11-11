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
// CTestPlatAlfVisual::TestAlfCtrlGroupResourceId
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathNewL, "TestAlfCurvePathNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathNewL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathNewL );
    
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    CleanupStack::PushL( vPath );
    STIF_ASSERT_NOT_NULL( vPath );
    CleanupStack::PopAndDestroy( vPath );
    
    vPath = NULL;
    vPath = CAlfCurvePath::NewLC( *iAlfEnv );
    STIF_ASSERT_NOT_NULL( vPath );
    CleanupStack::PopAndDestroy( vPath );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLoopL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLoopL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLoopL, "TestAlfCurvePathLoopL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLoopL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLoopL );
    
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    CleanupStack::PushL( vPath );
    vPath->EnableLoop( ETrue );
    STIF_ASSERT_TRUE( vPath->Loop() );
    vPath->EnableLoop( EFalse );
    STIF_ASSERT_FALSE( vPath->Loop() );
    CleanupStack::PopAndDestroy( vPath );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathOffsetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathOffsetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathOffsetL, "TestAlfCurvePathOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathOffsetL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathOffsetL );
    
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    CleanupStack::PushL( vPath );
    TAlfTimedPoint offset;
    offset.iX.SetValueNow( 0.5 );
    offset.iY.SetValueNow( 0.6 );
    vPath->SetOffset(offset);
    
    const TAlfTimedPoint& newOffSet = vPath->Offset();
    STIF_ASSERT_EQUALS( (float)0.5, newOffSet.iX.ValueNow() );
    STIF_ASSERT_EQUALS( (float)0.6, newOffSet.iY.ValueNow() );
    CleanupStack::PopAndDestroy( vPath );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathProcessPointsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathProcessPointsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathProcessPointsL, "TestAlfCurvePathProcessPointsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathProcessPointsL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathProcessPointsL );
    
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    CleanupStack::PushL( vPath );
    TReal32 vRealnum = 0;
    TPoint vStartPoiont( 0, 0 );
    TPoint vEndPoint( 1, 1 );
    TSize vSize( 1, 1 );
    TAlfRealSize vRealSize( 0.5, 0.5 );
    TAlfRealPoint vRealStartPoint;
    TAlfRealPoint vRealEndPoint( vEndPoint );
    vPath->SetOrigin( vRealnum );
    
    vPath->AppendLineL( vStartPoiont, vEndPoint );
    vPath->AppendLineL( vRealStartPoint, vRealEndPoint );
    
    vPath->AppendArcL( vStartPoiont, vSize, 0.5, 0.5 );
    vPath->AppendArcL( vRealStartPoint, vRealSize, 0.5, 0.5 );
    STIF_ASSERT_NOT_EQUALS( (float)0.0, vPath->Length() )
    vPath->MapValue( 0.5, 0 );
    vPath->Evaluate( 0.4, vRealStartPoint );
    CleanupStack::PopAndDestroy( vPath );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathResetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathResetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathResetL, "TestAlfCurvePathResetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathResetL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathResetL );
    
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    CleanupStack::PushL( vPath );
    vPath->Reset();
    CleanupStack::PopAndDestroy( vPath );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathMappingFunctionIdentifierL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathMappingFunctionIdentifierL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathMappingFunctionIdentifierL, "TestAlfCurvePathMappingFunctionIdentifierL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathMappingFunctionIdentifierL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathMappingFunctionIdentifierL );
    
    CAlfCurvePath* vPath = CAlfCurvePath::NewL( *iAlfEnv );
    CleanupStack::PushL( vPath );
    vPath->MappingFunctionIdentifier();
    CleanupStack::PopAndDestroy( vPath );
    
    return KErrNone;
    }

// End of file


