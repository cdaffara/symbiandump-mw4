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
* Description:  test functions for alfutil.h
*
*/



// [INCLUDE FILES]
#include <alf/alfutil.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KArrayLen = 3;
const TInt KLightPosArrayLen = 4;
const TInt KDestMatArrayLen = 16;

const TInt KGeneralNum = 256;

const TInt KComponentsNum = 4;

const TInt KRed = 16; // red
const TInt KGreen = 24; // green
const TInt KBlue = 32; // blue
const TInt KAlpha = 255; // alpha

const TInt KScreenW = 320;
const TInt KScreenH = 200;
const TInt KGeneralSize = 16;
const TInt KScrBufSizeW = 160;
const TInt KScrBufSizeH = 100;

const TInt KSrcBufLen1 = KComponentsNum * KScreenW * KScreenH;
const TInt KSrcBufLen2 = KComponentsNum * KScrBufSizeW * KScrBufSizeH;
const TInt KDesBufLen = KComponentsNum * KGeneralSize * KGeneralSize;

const TInt KTempSrcNum1 = KGeneralSize * KScrBufSizeW * KComponentsNum;
const TInt KTempSrcNum2 = 2 * KGeneralSize * KScrBufSizeW * KComponentsNum;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilPower2
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilPower2( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilPower2, "TestUtilPower2" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilPower2 );
    // Print to log file
    iLog->Log( KTestUtilPower2 );
    
    AlfUtil::Power2( 1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilPower2RoundDown
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilPower2RoundDown( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilPower2RoundDown, "TestUtilPower2RoundDown" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilPower2RoundDown );
    // Print to log file
    iLog->Log( KTestUtilPower2RoundDown );
    
    AlfUtil::Power2RoundDown( 1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilInterpolate
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilInterpolate( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilInterpolate, "TestUtilInterpolate" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilInterpolate );
    // Print to log file
    iLog->Log( KTestUtilInterpolate );
    
    AlfUtil::Interpolate( 1.0, 1.0, 2.0 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilWrapValue
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilWrapValue( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilWrapValue, "TestUtilWrapValue" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilWrapValue );
    // Print to log file
    iLog->Log( KTestUtilWrapValue );
    
    TReal32 value = 0.0;
    AlfUtil::WrapValue( value, 1.0, 1.0 );//aHigh - aLow <= 0
    AlfUtil::WrapValue( value, 1.0, 2.0 );//aHigh - aLow > 0 && aValue < aLow
    value = 2.0;
    AlfUtil::WrapValue( value, 1.0, 2.0 );//aHigh - aLow > 0 && aValue >= aHigh
    value = 1.0;
    AlfUtil::WrapValue( value, 1.0, 2.0 );//aHigh - aLow > 0 && aValue >= aLow && aValue < aHigh
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilRandomInt
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilRandomInt( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilRandomInt, "TestUtilRandomInt" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilRandomInt );
    // Print to log file
    iLog->Log( KTestUtilRandomInt );
    
    AlfUtil::RandomInt( 1, 2 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilRandomReal
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilRandomReal( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilRandomReal, "TestUtilRandomReal" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilRandomReal );
    // Print to log file
    iLog->Log( KTestUtilRandomReal );
    
    AlfUtil::RandomReal( 1.0, 3.0 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilQuickLengthWithPoint
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilQuickLengthWithPoint( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilQuickLengthWithPoint, "TestUtilQuickLengthWithPoint" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilQuickLengthWithPoint );
    // Print to log file
    iLog->Log( KTestUtilQuickLengthWithPoint );
    
    TAlfRealPoint vector( 0.f, 1.f );
    AlfUtil::QuickLength( vector );//Abs( aVector.iX ) < Abs( aVector.iY )
    vector.iX = 2.0;
    AlfUtil::QuickLength( vector );//Abs( aVector.iX ) >= Abs( aVector.iY )
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilQuickLength
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilQuickLength( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilQuickLength, "TestUtilQuickLength" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilQuickLength );
    // Print to log file
    iLog->Log( KTestUtilQuickLength );
    
    AlfUtil::QuickLength( 1.0, 2.0 );//Abs( aDx ) < Abs( aDy )
    AlfUtil::QuickLength( 2.0, 1.0 );//Abs( aDx ) >= Abs( aDy )
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilQuickNormalize
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilQuickNormalize( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilQuickNormalize, "TestUtilQuickNormalize" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilQuickNormalize );
    // Print to log file
    iLog->Log( KTestUtilQuickNormalize );
    
    TAlfRealPoint normal;
    AlfUtil::QuickNormalize( normal );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilQuickNormalizeWithVector
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilQuickNormalizeWithVector( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilQuickNormalizeWithVector, "TestUtilQuickNormalizeWithVector" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilQuickNormalizeWithVector );
    // Print to log file
    iLog->Log( KTestUtilQuickNormalizeWithVector );
    
    TReal32 vector[3] = { 1.0, 2.0, 3.0 };
    AlfUtil::QuickNormalize( vector );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilCrossProduct
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilCrossProduct( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilCrossProduct, "TestUtilCrossProduct" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilCrossProduct );
    // Print to log file
    iLog->Log( KTestUtilCrossProduct );
    
    TReal32 vectorA[ KArrayLen ] = { 1.0, 2.0, 3.0 };
    TReal32 vectorB[ KArrayLen ] = { 1.0, 2.0, 3.0 };
    TReal32 product[ KArrayLen ];
    AlfUtil::CrossProduct( vectorA, vectorB, product );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilNormalFromPoints
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilNormalFromPoints( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilNormalFromPoints, "TestUtilNormalFromPoints" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilNormalFromPoints );
    // Print to log file
    iLog->Log( KTestUtilNormalFromPoints );
    
    TReal32 points[ KArrayLen ][ KArrayLen ] =
        { 
        { 1.0, 2.0, 3.0 },
        { 1.0, 2.0, 3.0 },
        { 1.0, 2.0, 3.0 }
        };
    TReal32 normal[ KArrayLen ];
    
    AlfUtil::NormalFromPoints( points, normal );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilShadowMatrix
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilShadowMatrix( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilShadowMatrix, "TestUtilShadowMatrix" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilShadowMatrix );
    // Print to log file
    iLog->Log( KTestUtilShadowMatrix );
    
    TReal32 planePoint[ KArrayLen ] = { 1.0, 2.0, 3.0 };
    TReal32 planeNormal[ KArrayLen ] = { 1.0, 2.0, 3.0 };
    TReal32 lightPos[ KLightPosArrayLen ] = { 1.0, 2.0, 3.0, 4.0 };
    TReal32 destMat[ KDestMatArrayLen ];
    
    AlfUtil::ShadowMatrix( planePoint, planeNormal, lightPos, destMat );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilFreeMemory
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilFreeMemory( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilFreeMemory, "TestUtilFreeMemory" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilFreeMemory );
    // Print to log file
    iLog->Log( KTestUtilFreeMemory );
    
    TUint totalMemo;
    AlfUtil::FreeMemory( &totalMemo );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilScreenSize
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilScreenSize( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilScreenSize, "TestUtilScreenSize" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilScreenSize );
    // Print to log file
    iLog->Log( KTestUtilScreenSize );
    
    AlfUtil::ScreenSize();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilNearestFontInTwipsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilNearestFontInTwipsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilNearestFontInTwipsL, "TestUtilNearestFontInTwipsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilNearestFontInTwipsL );
    // Print to log file
    iLog->Log( KTestUtilNearestFontInTwipsL );
    
    TFontSpec fontSpec;
    AlfUtil::NearestFontInTwipsL( fontSpec );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilReleaseFont
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilReleaseFont( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilReleaseFont, "TestUtilReleaseFont" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilReleaseFont );
    // Print to log file
    iLog->Log( KTestUtilReleaseFont );

    CFont* font = NULL;
    AlfUtil::ReleaseFont( font );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilLengthUnit
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilLengthUnit( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilLengthUnit, "TestUtilLengthUnit" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilLengthUnit );
    // Print to log file
    iLog->Log( KTestUtilLengthUnit );

    AlfUtil::LengthUnit();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilColorLightness
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilColorLightness( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilColorLightness, "TestUtilColorLightness" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilColorLightness );
    // Print to log file
    iLog->Log( KTestUtilColorLightness );

    TRgb color;
    AlfUtil::ColorLightness( color );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilScaleFbsBitmapL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilScaleFbsBitmapL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilScaleFbsBitmapL, "TestUtilScaleFbsBitmapL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilScaleFbsBitmapL );
    // Print to log file
    iLog->Log( KTestUtilScaleFbsBitmapL );

    TSize sizeInPixels = TSize( KGeneralNum, KGeneralNum );
    CFbsBitmap *srcBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( srcBmp );
    User::LeaveIfError( srcBmp->Create( sizeInPixels, EColor16M ) );
    CFbsBitmap *scaledBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( scaledBmp );
    User::LeaveIfError( scaledBmp->Create( sizeInPixels, EColor16MA ) );
    AlfUtil::ScaleFbsBitmapL( *srcBmp, *scaledBmp );
    CleanupStack::PopAndDestroy( scaledBmp );
    CleanupStack::PopAndDestroy( srcBmp );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilCombineMaskFbsBitmapL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilCombineMaskFbsBitmapL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilCombineMaskFbsBitmapL, "TestUtilCombineMaskFbsBitmapL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilCombineMaskFbsBitmapL );
    // Print to log file
    iLog->Log( KTestUtilCombineMaskFbsBitmapL );

    TSize sizeInPixels = TSize( KGeneralNum, KGeneralNum );
    CFbsBitmap *srcBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( srcBmp );
    User::LeaveIfError( srcBmp->Create( sizeInPixels, EColor16M ) );
    CFbsBitmap *srcMaskBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( srcMaskBmp );
    User::LeaveIfError( srcMaskBmp->Create( sizeInPixels, EGray2 ) );
    CFbsBitmap *combinedBmp = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( combinedBmp );
    User::LeaveIfError( combinedBmp->Create( sizeInPixels, EColor16MA ) );
    
    AlfUtil::CombineMaskFbsBitmapL( *srcBmp, *srcMaskBmp, *combinedBmp );
    CleanupStack::PopAndDestroy( combinedBmp );
    CleanupStack::PopAndDestroy( srcMaskBmp );
    CleanupStack::PopAndDestroy( srcBmp );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilCropFbsBitmapL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilCropFbsBitmapL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilCropFbsBitmapL, "TestUtilCropFbsBitmapL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilCropFbsBitmapL );
    // Print to log file
    iLog->Log( KTestUtilCropFbsBitmapL );

    TSize sizeInPixels = TSize( KGeneralNum, KGeneralNum );
    CFbsBitmap *srcBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( srcBmp );
    User::LeaveIfError( srcBmp->Create( sizeInPixels, EColor16M ) );
    CFbsBitmap *cropedBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( cropedBmp );
    User::LeaveIfError( cropedBmp->Create( sizeInPixels, EGray2 ) );
    TPoint cropPos;
    AlfUtil::CropFbsBitmapL( *srcBmp, *cropedBmp, cropPos );
    CleanupStack::PopAndDestroy( cropedBmp );
    CleanupStack::PopAndDestroy( srcBmp );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilScaleImageL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilScaleImageL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilScaleImageL, "TestUtilScaleImageL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilScaleImageL );
    // Print to log file
    iLog->Log( KTestUtilScaleImageL );

    TUint8* srcBuf = new TUint8[ KSrcBufLen1 ];
    STIF_ASSERT_NOT_NULL( srcBuf );
    CleanupStack::PushL( srcBuf );
    // fill with
    for ( TInt t = 0; t < KSrcBufLen1; )
        {
        srcBuf[t++] = KRed; // red
        srcBuf[t++] = KGreen; // green
        srcBuf[t++] = KBlue; // blue
        srcBuf[t++] = KAlpha; // alpha
        }

    // try to allocate dummy image buffer for 320x200 image
    TUint8* desBuf = new TUint8[ KDesBufLen ];
    STIF_ASSERT_NOT_NULL( desBuf );
    CleanupStack::PushL( desBuf );
    // fill with zero pixels
    for ( TInt t = 0; t < KDesBufLen; t++ )
        {
        desBuf[t] = 0;
        }
    TSize srcSize = TSize( KScreenW, KScreenH );
    TSize desSize = TSize( KGeneralSize, KGeneralSize );
    AlfUtil::ScaleImage( KComponentsNum, srcSize, srcBuf, desSize, desBuf );
    CleanupStack::PopAndDestroy( desBuf );
    CleanupStack::PopAndDestroy( srcBuf );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilCropImageL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilCropImageL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilCropImageL, "TestUtilCropImageL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilCropImageL );
    // Print to log file
    iLog->Log( KTestUtilCropImageL );

    TUint8* srcBuf = new TUint8[ KSrcBufLen2 ];
    STIF_ASSERT_NOT_NULL( srcBuf );
    CleanupStack::PushL( srcBuf );
    // fill with
    for ( TInt t = 0; t < KSrcBufLen2; )
        {
        // fill the target crop area a bit differently
        if ( ( t >= KTempSrcNum1 ) && ( t <= KTempSrcNum2 ) )
            {
            srcBuf[t++] = KRed; // red
            srcBuf[t++] = KGreen; // green
            srcBuf[t++] = KBlue; // blue
            srcBuf[t++] = KAlpha; // alpha
            }
        else
            {
            srcBuf[t++] = 1; // red
            srcBuf[t++] = 1; // green
            srcBuf[t++] = 1; // blue
            srcBuf[t++] = 1; // alpha
            }
    }

    // try to allocate dummy image buffer for 160x100 image
    TUint8* desBuf = new TUint8[ KDesBufLen ];
    STIF_ASSERT_NOT_NULL( desBuf );
    CleanupStack::PushL( desBuf );
    // fill with zero pixels
    for (TInt t = 0; t< KDesBufLen; t++)
        {
        desBuf[t] = 0;
        }
    TSize srcBufSize = TSize( KScrBufSizeW, KScrBufSizeH );
    TPoint cropOffset = TPoint( 0, KGeneralSize );
    TSize croppedSize = TSize( KGeneralSize, KGeneralSize );
    AlfUtil::CropImage( KComponentsNum, srcBufSize, srcBuf, cropOffset, croppedSize, desBuf );
    CleanupStack::PopAndDestroy( desBuf );
    CleanupStack::PopAndDestroy( srcBuf );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilTagMatches
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilTagMatches( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilTagMatches, "TestUtilTagMatches" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilTagMatches );
    // Print to log file
    iLog->Log( KTestUtilTagMatches );

    _LIT8( KTagTxt, "Caption" );
    _LIT8( KTagsColonSeparatedTxt, "Test:Caption" );
    AlfUtil::TagMatches( KTagsColonSeparatedTxt, KTagTxt );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilConvertBitmapToDisplayModeLCL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilConvertBitmapToDisplayModeLCL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilConvertBitmapToDisplayModeLCL, "TestUtilConvertBitmapToDisplayModeLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilConvertBitmapToDisplayModeLCL );
    // Print to log file
    iLog->Log( KTestUtilConvertBitmapToDisplayModeLCL );

    TSize sizeInPixels = TSize( KGeneralNum, KGeneralNum );
    CFbsBitmap *srcBmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( srcBmp );
    User::LeaveIfError( srcBmp->Create( sizeInPixels, EColor16M ) );
    CFbsBitmap* desBmp = AlfUtil::ConvertBitmapToDisplayModeLC( *srcBmp, EGray2 );
    CleanupStack::PopAndDestroy( desBmp );
    CleanupStack::PopAndDestroy( srcBmp );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestUtilThemeItemIdL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestUtilThemeItemIdL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestUtilThemeItemIdL, "TestUtilThemeItemIdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestUtilThemeItemIdL );
    // Print to log file
    iLog->Log( KTestUtilThemeItemIdL );

    _LIT( KTxtNone, "None" );
    AlfUtil::ThemeItemIdL( *iAlfEnv, KTxtNone );
    
    return KErrNone;
    }


//  [End of File]
