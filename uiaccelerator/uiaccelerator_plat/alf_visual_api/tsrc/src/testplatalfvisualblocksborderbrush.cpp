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
* Description:  test function implement of CTestPlatAlfVisual
*
*/


// [INCLUDE FILES]
#include <alf/alfborderbrush.h>
#include <alf/alfbitmapprovider.h>
#include <avkon.mbg>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KImageWidth = 60;
const TInt KImageHeight = 670;
const TInt KGeneralTime = 1000000;


// FORWARD DECLARATION
// class CTestBitmapProvider for testing CAlfBorderBrush
class CTestBitmapProvider : public CBase, public MAlfBitmapProvider
    {
public:
    /**
     * Constructor
     */
    CTestBitmapProvider() : iImageNumber( 0 ) {}
    
    /**
     * From MAlfBitmapProvider
     * 
     * Provide a bitmap and mask from the given UID.
     */
    void ProvideBitmapL( TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask )
        {
        TBool done = EFalse;
        
        while ( !done )
            {
            AknIconUtils::CreateIconL( aBitmap, aBitmapMask, 
                AknIconUtils::AvkonIconFileName(), 
                EMbmAvkonQgn_indi_find_glass + iImageNumber,
                EMbmAvkonQgn_indi_find_glass_mask + iImageNumber );

            AknIconUtils::SetSize( aBitmap, TSize( KImageWidth, KImageHeight ) );
            
            // Alf does not support all displaymodes, so continue until we find supported image
            if ( !aBitmap || aBitmap->DisplayMode() < EColor64K
                 || !aBitmapMask || aBitmapMask->DisplayMode() != EGray256 )
                {
                delete aBitmap;
                delete aBitmapMask;
                aBitmap = NULL;
                aBitmapMask = NULL;
                iImageNumber += 2;
                }
            else
                {
                done = ETrue;
                }
            }
        }
    
private:
    /**
     * The number of image
     */
    TInt iImageNumber;
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushNewL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushNewL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushNewL, "TestBorderBrushNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushNewL );
    // Print to log file
    iLog->Log( KTestBorderBrushNewL );
    
    TAlfXYMetric thickness;
    TAlfXYMetric edgeOffset;
    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, thickness, edgeOffset );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushNewLCL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushNewLCL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushNewLCL, "TestBorderBrushNewLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushNewLCL );
    // Print to log file
    iLog->Log( KTestBorderBrushNewLCL );
    
    TAlfXYMetric thickness;
    TAlfXYMetric edgeOffset;
    CAlfBorderBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, thickness, edgeOffset );
    STIF_ASSERT_NOT_NULL( brush );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushOffsetNewL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushOffsetNewL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushOffsetNewL, "TestBorderBrushOffsetNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushOffsetNewL );
    // Print to log file
    iLog->Log( KTestBorderBrushOffsetNewL );
    
    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0,0,0,0 ); 
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    CleanupStack::PopAndDestroy( brush );
    
    brush = CAlfBorderBrush::NewL( *iAlfEnv, -1, 11231230, -04234234234, 0 );
    CleanupStack::PushL( brush );
    STIF_ASSERT_NOT_NULL( brush );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushOffsetNewLCL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushOffsetNewLCL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushOffsetNewLCL, "TestBorderBrushOffsetNewLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushOffsetNewLCL );
    // Print to log file
    iLog->Log( KTestBorderBrushOffsetNewLCL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 0, 0, 0, 0 );
    STIF_ASSERT_NOT_NULL( brush );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetImageL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetImageL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetImageL, "TestBorderBrushSetImageL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetImageL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetImageL );

    CTestBitmapProvider* bmpProvider = new ( ELeave ) CTestBitmapProvider;
    CleanupStack::PushL( bmpProvider );
    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    CAlfTexture& texture = iAlfEnv->TextureManager().CreateTextureL( 2,
                                         bmpProvider,
                                         EAlfTextureFlagGenerateShadow );
                                         
    brush->SetImage( TAlfImage( texture ) );
    CleanupStack::PopAndDestroy( brush );
    CleanupStack::PopAndDestroy( bmpProvider );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushImageL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushImageL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushImageL, "TestBorderBrushImageL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushImageL );
    // Print to log file
    iLog->Log( KTestBorderBrushImageL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    brush->Image();
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetColorL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetColorL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetColorL, "TestBorderBrushSetColorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetColorL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetColorL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    brush->SetColor( KRgbDarkGray );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushThicknessL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushThicknessL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushThicknessL, "TestBorderBrushThicknessL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushThicknessL );
    // Print to log file
    iLog->Log( KTestBorderBrushThicknessL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    const TAlfTimedPoint& valueGet = brush->Thickness();
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.ValueNow() );
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.iX.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetThicknessL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetThicknessL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetThicknessL, "TestBorderBrushSetThicknessL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetThicknessL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetThicknessL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    TAlfTimedPoint valueSet;
    valueSet.iY.SetValueNow( 0 );
    valueSet.iY.SetTarget( 1, KGeneralTime );
    brush->SetThickness( valueSet );
    
    const TAlfTimedPoint& valueGet = brush->Thickness();
    STIF_ASSERT_EQUALS( 0.f, valueGet.iY.ValueNow() );
    STIF_ASSERT_EQUALS( 1.f, valueGet.iY.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.iY.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetThicknessMetricL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetThicknessMetricL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetThicknessMetricL, "TestBorderBrushSetThicknessMetricL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetThicknessMetricL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetThicknessMetricL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    TAlfMetric valueSet( 2.f );
    brush->SetThickness( valueSet );
    
    const TAlfTimedPoint& valueGet = brush->Thickness();
    STIF_ASSERT_EQUALS( 2.f, valueGet.iY.ValueNow() );
    STIF_ASSERT_EQUALS( 2.f, valueGet.iY.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.iY.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushEdgeOffsetL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushEdgeOffsetL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushEdgeOffsetL, "TestBorderBrushEdgeOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushEdgeOffsetL );
    // Print to log file
    iLog->Log( KTestBorderBrushEdgeOffsetL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    const TAlfTimedPoint& valueGet = brush->EdgeOffset();
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.ValueNow() );
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.iX.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetEdgeOffsetL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetEdgeOffsetL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetEdgeOffsetL, "TestBorderBrushSetEdgeOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetEdgeOffsetL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetEdgeOffsetL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    TAlfTimedPoint valueSet;
    valueSet.iX.SetValueNow( 0 );
    valueSet.iX.SetTarget( 1, KGeneralTime );
    brush->SetEdgeOffset( valueSet );
    
    const TAlfTimedPoint& valueGet = brush->EdgeOffset();
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.ValueNow() );
    STIF_ASSERT_EQUALS( 1.f, valueGet.iX.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.iX.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetEdgeOffsetMetricL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetEdgeOffsetMetricL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetEdgeOffsetMetricL, "TestBorderBrushSetEdgeOffsetMetricL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetEdgeOffsetMetricL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetEdgeOffsetMetricL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    TAlfMetric valueSet( 2.f );
    brush->SetEdgeOffset( valueSet );
    
    const TAlfTimedPoint& valueGet = brush->EdgeOffset();
    STIF_ASSERT_EQUALS( 2.f, valueGet.iX.ValueNow() );
    STIF_ASSERT_EQUALS( 2.f, valueGet.iX.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.iX.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushImageOffsetL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushImageOffsetL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushImageOffsetL, "TestBorderBrushImageOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushImageOffsetL );
    // Print to log file
    iLog->Log( KTestBorderBrushImageOffsetL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    const TAlfTimedPoint& valueGet = brush->ImageOffset();
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.ValueNow() );
    STIF_ASSERT_EQUALS( 0.f, valueGet.iX.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.iX.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetImageOffsetL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetImageOffsetL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetImageOffsetL, "TestBorderBrushSetImageOffsetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetImageOffsetL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetImageOffsetL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    TAlfTimedPoint valueSet;
    valueSet.iY.SetValueNow( 0 );
    valueSet.iY.SetTarget( 1, KGeneralTime );
    brush->SetImageOffset( valueSet );
    
    const TAlfTimedPoint& valueGet = brush->ImageOffset();
    STIF_ASSERT_EQUALS( 0.f, valueGet.iY.ValueNow() );
    STIF_ASSERT_EQUALS( 1.f, valueGet.iY.Target() );
    STIF_ASSERT_NOT_EQUALS( 0, valueGet.iY.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBorderBrushSetImageOffsetMetricL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBorderBrushSetImageOffsetMetricL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBorderBrushSetImageOffsetMetricL, "TestBorderBrushSetImageOffsetMetricL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBorderBrushSetImageOffsetMetricL );
    // Print to log file
    iLog->Log( KTestBorderBrushSetImageOffsetMetricL );

    CAlfBorderBrush* brush = CAlfBorderBrush::NewL( *iAlfEnv, 0, 0, 0, 0 );
    CleanupStack::PushL( brush );
    TAlfMetric valueSet( 2.f );
    brush->SetImageOffset( valueSet );
    
    const TAlfTimedPoint& valueGet = brush->ImageOffset();
    STIF_ASSERT_EQUALS( 2.f, valueGet.iY.ValueNow() );
    STIF_ASSERT_EQUALS( 2.f, valueGet.iY.Target() );
    STIF_ASSERT_EQUALS( 0, valueGet.iY.TimeToTargetinMilliSeconds() );
    CleanupStack::PopAndDestroy( brush );

    return KErrNone;
    }


//  [End of File]
