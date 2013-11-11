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
// CTestPlatAlfVisual::TestAlfDropShadowBrushNewFuncsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDropShadowBrushNewFuncsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDropShadowBrushNewFuncsL, "TestAlfDropShadowBrushNewFuncsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDropShadowBrushNewFuncsL );
    // Print to log file
    iLog->Log( KTestAlfDropShadowBrushNewFuncsL );
    
    CAlfDropShadowBrush* vBrush = CAlfDropShadowBrush::NewL( *iAlfEnv, 0 );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    TAlfMetric vMetric( 0 );
    vBrush = CAlfDropShadowBrush::NewL( *iAlfEnv, vMetric );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfDropShadowBrush::NewLC( *iAlfEnv, 0 );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfDropShadowBrush::NewLC( *iAlfEnv, vMetric );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDropShadowBrushSoftnessL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDropShadowBrushSoftnessL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDropShadowBrushSoftnessL, "TestAlfDropShadowBrushSoftnessL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDropShadowBrushSoftnessL );
    // Print to log file
    iLog->Log( KTestAlfDropShadowBrushSoftnessL );
    
    CAlfDropShadowBrush* vBrush = CAlfDropShadowBrush::NewLC( *iAlfEnv, 0 );
    TAlfMetric vMetric( 0 );
    vBrush->SetSoftness( vMetric );
    TAlfTimedValue valueSet;
    valueSet.SetValueNow( 0 );
    const TInt num = 1000000;
    valueSet.SetTarget( 1, num );
    vBrush->SetSoftness( valueSet );
    
    const TAlfTimedValue& valueGet = vBrush->Softness();
    STIF_ASSERT_EQUALS( (float)0.0, valueGet.ValueNow() );
    STIF_ASSERT_EQUALS( (float)1.0, valueGet.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDropShadowBrushDepthL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDropShadowBrushDepthL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDropShadowBrushDepthL, "TestAlfDropShadowBrushDepthL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDropShadowBrushDepthL );
    // Print to log file
    iLog->Log( KTestAlfDropShadowBrushDepthL );
    
    CAlfDropShadowBrush* vBrush = CAlfDropShadowBrush::NewLC( *iAlfEnv, 0 );
    TAlfMetric vMetric( 0 );
    vBrush->SetDepth( vMetric );
    
    TAlfTimedValue valueSet;
    valueSet.SetValueNow( 0 );
    const TInt num = 1000000;
    valueSet.SetTarget( 1, num );
    vBrush->SetDepth( valueSet );
    
    const TAlfTimedValue& valueGet = vBrush->Depth();
    STIF_ASSERT_EQUALS( (float)0.0, valueGet.ValueNow() );
    STIF_ASSERT_EQUALS( (float)1.0, valueGet.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.TimeToTargetinMilliSeconds() );
    
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDropShadowBrushSetColorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDropShadowBrushSetColorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDropShadowBrushSetColorL, "TestAlfDropShadowBrushSetColorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDropShadowBrushSetColorL );
    // Print to log file
    iLog->Log( KTestAlfDropShadowBrushSetColorL );
    
    CAlfDropShadowBrush* vBrush = CAlfDropShadowBrush::NewLC( *iAlfEnv, 0 );
    TRgb vRgb( KRgbBlack );
    vBrush->SetColor( vRgb );
    
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// End of file

