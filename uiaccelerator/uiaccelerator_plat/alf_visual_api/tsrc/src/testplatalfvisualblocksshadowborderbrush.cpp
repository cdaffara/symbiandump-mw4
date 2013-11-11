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
* Description:  test functions for alfshadowborderbrush.h
*
*/



// [INCLUDE FILES]
#include <alf/alfshadowborderbrush.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KGeneralNum = 100;
const TInt KTime = 1000000;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestShadowBBNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestShadowBBNewL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestShadowBBNewL, "TestShadowBBNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestShadowBBNewL );
    // Print to log file
    iLog->Log( KTestShadowBBNewL );
    
    CAlfShadowBorderBrush* borderBrush = NULL;
    
    TAlfMetric metricWidth( KGeneralNum );
    borderBrush = CAlfShadowBorderBrush::NewL( *iAlfEnv, metricWidth );
    CleanupStack::PushL( borderBrush );
    STIF_ASSERT_NOT_NULL( borderBrush );
    CleanupStack::PopAndDestroy( borderBrush );
    
    borderBrush = CAlfShadowBorderBrush::NewL( *iAlfEnv, KGeneralNum );
    CleanupStack::PushL( borderBrush );
    STIF_ASSERT_NOT_NULL( borderBrush );
    CleanupStack::PopAndDestroy( borderBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestShadowBBNewLCL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestShadowBBNewLCL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestShadowBBNewLCL, "TestShadowBBNewLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestShadowBBNewLCL );
    // Print to log file
    iLog->Log( KTestShadowBBNewLCL );
    
    CAlfShadowBorderBrush* borderBrush = NULL;
    
    TAlfMetric metricWidth( KGeneralNum );
    borderBrush = CAlfShadowBorderBrush::NewLC( *iAlfEnv, metricWidth );
    STIF_ASSERT_NOT_NULL( borderBrush );
    CleanupStack::PopAndDestroy( borderBrush );
    
    borderBrush = CAlfShadowBorderBrush::NewLC( *iAlfEnv, KGeneralNum );
    STIF_ASSERT_NOT_NULL( borderBrush );
    CleanupStack::PopAndDestroy( borderBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestShadowBBWidthMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestShadowBBWidthMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestShadowBBWidthMethodsL, "TestShadowBBWidthMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestShadowBBWidthMethodsL );
    // Print to log file
    iLog->Log( KTestShadowBBWidthMethodsL );
    
    CAlfShadowBorderBrush* borderBrush = CAlfShadowBorderBrush::NewLC( *iAlfEnv, KGeneralNum );
    STIF_ASSERT_NOT_NULL( borderBrush );
    
    const TAlfTimedValue& valueGet = borderBrush->Width();
    STIF_ASSERT_EQUALS( 0, valueGet.TimeToTargetinMilliSeconds() );
    
    TAlfTimedValue valueSet;
    valueSet.SetValueNow( 0 );
    valueSet.SetTarget( 1, KTime );
    borderBrush->SetWidth( valueSet );
    borderBrush->Width();
    STIF_ASSERT_EQUALS( 0.f, valueGet.ValueNow() );
    STIF_ASSERT_EQUALS( 1.f, valueGet.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.TimeToTargetinMilliSeconds() );
    
    TAlfMetric valueSet2( 2.f );
    borderBrush->SetWidth( valueSet2 );
    borderBrush->Width();
    STIF_ASSERT_EQUALS( 2.f, valueGet.ValueNow() );
    STIF_ASSERT_EQUALS( 2.f, valueGet.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.TimeToTargetinMilliSeconds() );

    CleanupStack::PopAndDestroy( borderBrush );
    
    return KErrNone;
    }


//  [End of File]
