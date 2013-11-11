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
* Description:  test functions for alftimedvalue.h
*
*/



// [INCLUDE FILES]
#include <alf/alftimedvalue.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KGeneralNum = 100;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedValueConstructor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedValueConstructor( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedValueConstructor, "TestTimedValueConstructor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedValueConstructor );
    // Print to log file
    iLog->Log( KTestTimedValueConstructor );
    
    TAlfTimedValue value1;
    TAlfTimedValue value2( 1.0 );
    TAlfTimedValue value3( 1.0, 1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedValueValueNow
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedValueValueNow( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedValueValueNow, "TestTimedValueValueNow" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedValueValueNow );
    // Print to log file
    iLog->Log( KTestTimedValueValueNow );
    
    TAlfTimedValue value;
    value.SetValueNow( 1.0 );
    STIF_ASSERT_EQUALS( TReal32( 1.0 ), value.ValueNow() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedValueTargetAndSpeed
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedValueTargetAndSpeed( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedValueTargetAndSpeed, "TestTimedValueTargetAndSpeed" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedValueTargetAndSpeed );
    // Print to log file
    iLog->Log( KTestTimedValueTargetAndSpeed );
    
    TAlfTimedValue value;
    value.SetTarget( 1.0, KGeneralNum );
    STIF_ASSERT_EQUALS( TReal32( 1.0 ), value.Target() );
    value.SetTargetWithSpeed( 1.0, 2.0 );
    STIF_ASSERT_EQUALS( TReal32( 1.0 ), value.Target() );
    STIF_ASSERT_EQUALS( TReal32( 2.0 ), value.Speed() );
    value.TimeToTargetinMilliSeconds();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedValueStyle
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedValueStyle( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedValueStyle, "TestTimedValueStyle" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedValueStyle );
    // Print to log file
    iLog->Log( KTestTimedValueStyle );
    
    TAlfTimedValue value;
    value.SetStyle( EAlfTimedValueStyleUseSystemDefault );
    STIF_ASSERT_EQUALS( EAlfTimedValueStyleUseSystemDefault, value.Style() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedValueMappingFunctionIdentifier
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedValueMappingFunctionIdentifier( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedValueMappingFunctionIdentifier, "TestTimedValueMappingFunctionIdentifier" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedValueMappingFunctionIdentifier );
    // Print to log file
    iLog->Log( KTestTimedValueMappingFunctionIdentifier );
    
    TAlfTimedValue value;
    value.SetMappingFunctionIdentifier( 1 );
    STIF_ASSERT_EQUALS( 1, value.MappingFunctionIdentifier() );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedValueFlags
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedValueFlags( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedValueFlags, "TestTimedValueFlags" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedValueFlags );
    // Print to log file
    iLog->Log( KTestTimedValueFlags );
    
    TAlfTimedValue value;
    value.Flags();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRealPointMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRealPointMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRealPointMethods, "TestRealPointMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRealPointMethods );
    // Print to log file
    iLog->Log( KTestRealPointMethods );
    
    TAlfRealPoint realPoint1( 1.0, 2.0 );
    TAlfRealPoint realPoint2( 0.0, 1.0 );
    
    TPoint point;
    point += realPoint1;
    
    realPoint1.AsSize();
    
    TAlfRealPoint realPoint3 = realPoint1 - realPoint2;
    TAlfRealPoint realPoint4 = realPoint2 + realPoint3;
    realPoint4 += realPoint3;
    realPoint4 -= realPoint3;
    TBool value = realPoint1 == realPoint4;
    STIF_ASSERT_TRUE( value );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRealSizeMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRealSizeMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRealSizeMethods, "TestRealSizeMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRealSizeMethods );
    // Print to log file
    iLog->Log( KTestRealSizeMethods );
    
    TAlfRealSize realSize1( 2.0, 2.0 );
    
    TSize size( 2, 2 );
    TAlfRealSize realSize2( size );
    
    TAlfRealPoint realPoint( 2.0, 2.0 );
    TAlfRealSize realSize3( realPoint );
    
    TSize size2;
    size2 += realSize1;
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestRealRectMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestRealRectMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestRealRectMethods, "TestRealRectMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestRealRectMethods );
    // Print to log file
    iLog->Log( KTestRealRectMethods );
    
    TAlfRealRect realRect;
    
    TRect rect;
    rect == realRect; //operator TRect() const
    
    realRect.Width(); //TReal32 Width() const
    realRect.Height(); //TReal32 Height() const
    realRect.TopRight(); //TAlfRealPoint TopRight() const
    realRect.BottomLeft(); //TAlfRealPoint BottomLeft() const
    realRect.Size(); //TAlfRealSize Size() const
    realRect.Center(); //TAlfRealPoint Center() const
    realRect.Grow( 1.0, 1.0 ); //void Grow(TReal32 aX, TReal32 aY)
    realRect.Shrink( 1.0, 1.0 ); //void Shrink(TReal32 aX, TReal32 aY)
    
    TPoint point( 1, 1 );
    realRect.Shrink( point ); //void Shrink(const TPoint& aPoint)
    
    TAlfRealPoint realPoint( 1.0, 1.0 );
    realRect.Shrink( realPoint ); //void Shrink(const TAlfRealPoint& aPoint)
    
    realRect.Shrink( realRect ); //void Shrink(const TAlfRealRect& aRect)
    realRect.Move( 1.0, 1.0 ); //void Move(TReal32 aDx, TReal32 aDy)
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedPointConstructor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedPointConstructor( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedPointConstructor, "TestTimedPointConstructor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedPointConstructor );
    // Print to log file
    iLog->Log( KTestTimedPointConstructor );
    
    TAlfTimedPoint point1;
    TAlfTimedPoint point2( 1.0, 1.0 );
    TAlfTimedPoint point3( 1.0, 1.0, KGeneralNum );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedPointSetStyle
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedPointSetStyle( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedPointSetStyle, "TestTimedPointSetStyle" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedPointSetStyle );
    // Print to log file
    iLog->Log( KTestTimedPointSetStyle );
    
    TAlfTimedPoint point;
    point.SetStyle( EAlfTimedValueStyleUseSystemDefault );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedPointSetMapFuncIdentifier
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedPointSetMapFuncIdentifier( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedPointSetMapFuncIdentifier, "TestTimedPointSetMapFuncIdentifier" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedPointSetMapFuncIdentifier );
    // Print to log file
    iLog->Log( KTestTimedPointSetMapFuncIdentifier );
    
    TAlfTimedPoint point;
    point.SetMappingFunctionIdentifier( 1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedPointTargetMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedPointTargetMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedPointTargetMethods, "TestTimedPointTargetMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedPointTargetMethods );
    // Print to log file
    iLog->Log( KTestTimedPointTargetMethods );
    
    TAlfTimedPoint point;
    TAlfRealPoint realPoint( 1.0, 1.0 );
    point.SetTarget( realPoint, KGeneralNum );
    point.SetTarget( 1.0, KGeneralNum );
    point.SetTargetWithSpeed( realPoint, KGeneralNum );
    point.SetTargetWithSpeed( 1.0, KGeneralNum );
    point.Target();
    point.TimeToTargetinMilliSeconds();
    point.IntTarget();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTimedPointValueNowMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTimedPointValueNowMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTimedPointValueNowMethods, "TestTimedPointValueNowMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTimedPointValueNowMethods );
    // Print to log file
    iLog->Log( KTestTimedPointValueNowMethods );
    
    TAlfTimedPoint point;
    point.ValueNow();
    point.IntValueNow();
    
    return KErrNone;
    }


//  [End of File]
