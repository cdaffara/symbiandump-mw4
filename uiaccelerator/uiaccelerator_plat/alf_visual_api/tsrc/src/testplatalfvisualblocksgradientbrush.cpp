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
// CTestPlatAlfVisual::TestAlfGradientBrushNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGradientBrushNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGradientBrushNewL, "TestAlfGradientBrushNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGradientBrushNewL );
    // Print to log file
    iLog->Log( KTestAlfGradientBrushNewL );
    
    CAlfGradientBrush* vBrush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfGradientBrush::NewLC( *iAlfEnv );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGradientBrushSetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGradientBrushSetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGradientBrushSetL, "TestAlfGradientBrushSetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGradientBrushSetL );
    // Print to log file
    iLog->Log( KTestAlfGradientBrushSetL );
    
    CAlfGradientBrush* vBrush = CAlfGradientBrush::NewLC( *iAlfEnv );
    vBrush->SetDirection( CAlfGradientBrush::EDirectionLeft );
    TRgb vRgb( KRgbBlue );
    vBrush->SetColor( vRgb );
    TAlfImage vImage;
    vBrush->SetImage( vImage );
    vBrush->AppendColorL( 1.0, vRgb);
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGradientBrushGetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGradientBrushGetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGradientBrushGetL, "TestAlfGradientBrushGetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGradientBrushGetL );
    // Print to log file
    iLog->Log( KTestAlfGradientBrushGetL );
    
    CAlfGradientBrush* vBrush = CAlfGradientBrush::NewLC( *iAlfEnv );
    vBrush->Color( 0 );
    vBrush->Opacity( 0 );
    vBrush->Image();
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// End of file


