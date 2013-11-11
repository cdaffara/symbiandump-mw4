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
// CTestPlatAlfVisual::TestAlfConstantMapFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfConstantMapFuncL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfConstantMapFuncL, "TestAlfConstantMapFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfConstantMapFuncL );
    // Print to log file
    iLog->Log( KTestAlfConstantMapFuncL );
    
    CAlfConstantMappingFunction* vMapFunc = 
        CAlfConstantMappingFunction::NewL( *iAlfEnv );
    vMapFunc->SetValue( 0 );
    vMapFunc->Value();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLinearMapFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLinearMapFuncL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLinearMapFuncL, "TestAlfLinearMapFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLinearMapFuncL );
    // Print to log file
    iLog->Log( KTestAlfLinearMapFuncL );
    
    CAlfLinearMappingFunction* vMapFunc = 
        CAlfLinearMappingFunction::NewL( *iAlfEnv );
    vMapFunc->SetFactor( 0 );
    vMapFunc->Factor();
    vMapFunc->SetOffset( 0 );
    vMapFunc->Offset();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfSineMapFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfSineMapFuncL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfSineMapFuncL, "TestAlfSineMapFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfSineMapFuncL );
    // Print to log file
    iLog->Log( KTestAlfSineMapFuncL );
    
    CAlfSineMappingFunction* vMapFunc = 
        CAlfSineMappingFunction::NewL( *iAlfEnv );
    vMapFunc->SetFactor( 0 );
    vMapFunc->Factor();
    vMapFunc->SetOffset( 0 );
    vMapFunc->Offset();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCosineMapFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCosineMapFuncL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCosineMapFuncL, "TestAlfCosineMapFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCosineMapFuncL );
    // Print to log file
    iLog->Log( KTestAlfCosineMapFuncL );
    
    CAlfCosineMappingFunction* vMapFunc = 
        CAlfCosineMappingFunction::NewL( *iAlfEnv );
    vMapFunc->SetFactor( 0 );
    vMapFunc->Factor();
    vMapFunc->SetOffset( 0 );
    vMapFunc->Offset();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfAverageMapFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfAverageMapFuncL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfAverageMapFuncL, "TestAlfAverageMapFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfAverageMapFuncL );
    // Print to log file
    iLog->Log( KTestAlfAverageMapFuncL );
    
    CAlfAverageMappingFunction* vMapFunc = 
        CAlfAverageMappingFunction::NewL( *iAlfEnv );
    const TAlfTimedValue vValue( 0 );
    vMapFunc->SetWeight( vValue );
    vMapFunc->SetMappingFunction1( NULL );
    vMapFunc->SetMappingFunction2( NULL );
    vMapFunc->SetMappingFunctions( NULL, NULL );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfTableMapFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfTableMapFuncL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfTableMapFuncL, "TestAlfTableMapFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfTableMapFuncL );
    // Print to log file
    iLog->Log( KTestAlfTableMapFuncL );
    
    CAlfTableMappingFunction* vMapFunc = 
        CAlfTableMappingFunction::NewL( *iAlfEnv );
    const TAlfTimedValue vValue( 0 );
    vMapFunc->SetMappingTableValues( 0, 0, NULL );
    
    return KErrNone;
    }

// End of file

