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
* Description:  test functions for alfbrush.h
*
*/



// [INCLUDE FILES]
#include <alf/alfconstants.h>

#include "testplatalfvisual.h"


// FORWARD DECLARATION
/**
 * This class is used to test CAlfBrush protected function
 * 
 * @lib none
 * @since S60 v3.2
 */
class CTestAlfBrush : public CAlfBrush
    {
public:
    /**
     * Test constructL method of CAlfBrush.
     */
    void TestConstructL(
        CAlfEnv& aEnv,
        TInt aImplementationId, 
        TInt aImplementationUid, 
        const TDesC8& aConstructionParams )
        {
        CAlfBrush::ConstructL( aEnv, aImplementationId, aImplementationUid, aConstructionParams );
        }
    /**
     * Test constructL method of CAlfBrush.
     */
    void TestConstructL()
        {
        CAlfBrush::ConstructL();
        }
    /**
     * Test PropertyOwnerExtension method of CAlfBrush.
     */
    void TestPropertyOwnerExtension( const TUid& aExtensionUid, TAny** aExtensionParams )
        {
        CAlfBrush::PropertyOwnerExtension( aExtensionUid, aExtensionParams );
        }
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushOpacityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushOpacityL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushOpacityL, "TestAlfBrushOpacityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushOpacityL );
    // Print to log file
    iLog->Log( KTestAlfBrushOpacityL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->Opacity();
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushSetOpacityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushSetOpacityL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushSetOpacityL, "TestAlfBrushSetOpacityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushSetOpacityL );
    // Print to log file
    iLog->Log( KTestAlfBrushSetOpacityL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    TAlfTimedValue value( 0 );
    brush->SetOpacity( value );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushSetLayerL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushSetLayerL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushSetLayerL, "TestAlfBrushSetLayerL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushSetLayerL );
    // Print to log file
    iLog->Log( KTestAlfBrushSetLayerL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    TAlfBrushLayer layer = EAlfBrushLayerBackground;
    brush->SetLayer( layer );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushSetClipToVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushSetClipToVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushSetClipToVisualL, "TestAlfBrushSetClipToVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushSetClipToVisualL );
    // Print to log file
    iLog->Log( KTestAlfBrushSetClipToVisualL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->SetClipToVisual( EFalse );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushClipToVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushClipToVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushClipToVisualL, "TestAlfBrushClipToVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushClipToVisualL );
    // Print to log file
    iLog->Log( KTestAlfBrushClipToVisualL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->ClipToVisual();
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushIdentifierL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushIdentifierL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushIdentifierL, "TestAlfBrushIdentifierL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushIdentifierL );
    // Print to log file
    iLog->Log( KTestAlfBrushIdentifierL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->Identifier();
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushCommsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushCommsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushCommsL, "TestAlfBrushCommsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushCommsL );
    // Print to log file
    iLog->Log( KTestAlfBrushCommsL );
    
    CAlfBrush* brush = CAlfGradientBrush::NewL( *iAlfEnv );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->Comms();
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushConstructLWithParamL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushConstructLWithParamL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushConstructLWithParamL, "TestAlfBrushConstructLWithParamL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushConstructLWithParamL );
    // Print to log file
    iLog->Log( KTestAlfBrushConstructLWithParamL );
    
    CTestAlfBrush* brush = new ( ELeave ) CTestAlfBrush;
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    TAlfXYMetric aThickness, aEdgeOffset;
    TAlfCreateBorderBrushMetricParams params;
    params.iThickness = aThickness; 
    params.iEdgeOffset = aEdgeOffset;
    TPckgC<TAlfCreateBorderBrushMetricParams> paramsPckg(params);
    brush->TestConstructL( *iAlfEnv, 
            EAlfBorderBrushCreateMetric,
            0,
            paramsPckg );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushConstructL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushConstructL, "TestAlfBrushConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushConstructL );
    // Print to log file
    iLog->Log( KTestAlfBrushConstructL );
    
    CTestAlfBrush* brush = new ( ELeave ) CTestAlfBrush;
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->TestConstructL();
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfBrushPropertyOwnerExtL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfBrushPropertyOwnerExtL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfBrush, "TestPlatAlfBrush" );
    _LIT( KTestAlfBrushPropertyOwnerExtL, "TestAlfBrushPropertyOwnerExtL" );
    TestModuleIf().Printf( 0, KTestPlatAlfBrush, KTestAlfBrushPropertyOwnerExtL );
    // Print to log file
    iLog->Log( KTestAlfBrushPropertyOwnerExtL );
    
    CTestAlfBrush* brush = new ( ELeave ) CTestAlfBrush;
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    brush->TestPropertyOwnerExtension( TUid::Uid( 0 ), NULL );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    
    }


//  [End of File]
