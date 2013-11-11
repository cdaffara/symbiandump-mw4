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
#include <alf/alfvisual.h>

#include "testplatalfvisual.h"

// CONSTANTS
const TInt KBufSize = 64;

// FORWARD DECLARATION
/*This class test protected functions of CAlfLayout*/
class CTestCAlfLayout : public CAlfLayout
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfLayout();
    /**
    * Destructor.
    */
    ~CTestCAlfLayout();
    /**
    * TestProtectedFuncForVisual test protected functions of CTestCAlfLayout
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfLayout::CTestCAlfLayout
// -----------------------------------------------------------------------------
//
CTestCAlfLayout::CTestCAlfLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfLayout::~CTestCAlfLayout
// -----------------------------------------------------------------------------
//
CTestCAlfLayout::~CTestCAlfLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfFlowLayout::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfLayout::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLayoutFullConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLayoutFullConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLayoutFullConstructorL, "TestAlfLayoutFullConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLayoutFullConstructorL );
    // Print to log file
    iLog->Log( KTestAlfLayoutFullConstructorL );
    
    CAlfLayout* vLayout = new ( ELeave ) CAlfLayout;
    CleanupStack::PushL( vLayout );
    STIF_ASSERT_NOT_NULL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    CleanupStack::PopAndDestroy( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLayoutAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLayoutAddNewL, "TestAlfLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestAlfLayoutAddNewL );
    
    CAlfLayout* vLayout = CAlfLayout::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLayoutSetAndGetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLayoutSetAndGetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLayoutSetAndGetL, "TestAlfLayoutSetAndGetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLayoutSetAndGetL );
    // Print to log file
    iLog->Log( KTestAlfLayoutSetAndGetL );
    
    CAlfLayout* vLayout = CAlfLayout::AddNewL( *iAlfCtl );
    CAlfImageVisual* vVisualOne = CAlfImageVisual::AddNewL( *iAlfCtl );
    TUid vUid = { 0x00000000 };
    TAlfImage vFirstImage( vUid, EAknsAppIconTypeList, 
            TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    vVisualOne->SetImage( vFirstImage );
    
    CAlfImageVisual* vVisualTwo = CAlfImageVisual::AddNewL( *iAlfCtl );
    TUid vSecUid = { 0x00000001 };
    TAlfImage vSecImage( vSecUid, EAknsAppIconTypeList, 
            TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    vVisualTwo->SetImage( vSecImage );
    
    vLayout->Append( vVisualOne );
    vLayout->Append( vVisualTwo );
    
    vLayout->EnableScrollingL();
    vLayout->Scrolling();
    TAlfTimedPoint vPoint( 0, 0 );
    vLayout->SetScrollOffset( vPoint );
    vLayout->ScrollOffset();
    TPoint vPos( 0, 1);
    TSize vSize( 1, 1 );
    vLayout->ChildOrdinal( 1 );
    vLayout->ChildPos( 0, vPos );
    vLayout->ChildSize( 0, vSize );
    
    TAlfXYMetric vXYMetric;
    vLayout->SetInnerPadding( vXYMetric );
    vLayout->InnerPaddingAsMetric();
    const TPoint vConstPos( 1, 1);
    vLayout->SetInnerPadding( vConstPos );
    vLayout->InnerPadding();
    vLayout->SetTransitionTime( 1 );
    
    vLayout->TransitionTime();
    vLayout->HorizontalInnerPadding();
    vLayout->VerticalInnerPadding();
    vLayout->InnerPaddingInBaseUnits();
    vLayout->EffectiveLayoutOrdinal( *vVisualTwo );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLayoutForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLayoutForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLayoutForVisualL, "TestAlfLayoutForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLayoutForVisualL );
    // Print to log file
    iLog->Log( KTestAlfLayoutForVisualL );
    
    CAlfLayout* vLayout = CAlfLayout::AddNewL( *iAlfCtl );
    CAlfImageVisual* vVisualOne = CAlfImageVisual::AddNewL( *iAlfCtl );
    TUid vUid = { 0x00000000 };
    TAlfImage vFirstImage( vUid, EAknsAppIconTypeList, 
            TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    vVisualOne->SetImage( vFirstImage );
    
    CAlfImageVisual* vVisualTwo = CAlfImageVisual::AddNewL( *iAlfCtl );
    TUid vSecUid = { 0x00000001 };
    TAlfImage vSecImage( vSecUid, EAknsAppIconTypeList, 
            TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    vVisualTwo->SetImage( vSecImage );
    vLayout->Append( vVisualOne );
    vLayout->Remove( vVisualOne );
    vLayout->Append( vVisualOne );
    vLayout->Append( vVisualTwo );
    vLayout->Reorder( *vVisualOne, 0 );
    vLayout->MoveVisualToFront( *vVisualTwo );
    vLayout->MoveVisualToBack( *vVisualTwo );
//    vLayout->VirtualSize();//error
    vLayout->FindVisual( vVisualOne );
    vLayout->Remove( vVisualOne );
    vLayout->Insert( vVisualOne, 0 );
    vLayout->Count();
    vLayout->Visual( 1 );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vLayout->FindTag( vBuf );
    vLayout->UpdateChildrenLayout();
    vLayout->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLayoutBaseUnitL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLayoutBaseUnitL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLayoutBaseUnitL, "TestAlfLayoutBaseUnitL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLayoutBaseUnitL );
    // Print to log file
    iLog->Log( KTestAlfLayoutBaseUnitL );
    
    CAlfLayout* vLayout = CAlfLayout::AddNewL( *iAlfCtl );
    TAlfMetric metricX(1.0, EAlfUnitNormalized);
    TAlfMetric metricY(2.0, EAlfUnitNormalized);
    vLayout->SetBaseUnit(metricX);
    
    TAlfXYMetric metricXY( metricX, metricY );
    vLayout->SetBaseUnit( metricXY );
    TAlfXYMetric resultMetricXY = vLayout->BaseUnit();
    TAlfMetric resultMetricX = resultMetricXY.iX;
    TAlfMetric resultMetricY = resultMetricXY.iY;
    
    STIF_ASSERT_EQUALS( metricX.iMagnitude, resultMetricX.iMagnitude );
    STIF_ASSERT_EQUALS( metricX.iUnit, resultMetricX.iUnit );
    STIF_ASSERT_EQUALS( metricX.iReferenceTextStyleId, resultMetricX.iReferenceTextStyleId );

    STIF_ASSERT_EQUALS( metricY.iMagnitude, resultMetricY.iMagnitude );
    STIF_ASSERT_EQUALS( metricY.iUnit, resultMetricY.iUnit );
    STIF_ASSERT_EQUALS( metricY.iReferenceTextStyleId, resultMetricY.iReferenceTextStyleId );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfLayoutProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfLayoutProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfLayoutProtectedFuncL, "TestAlfLayoutProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfLayoutProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfLayoutProtectedFuncL );
    
    CTestCAlfLayout* vLayout = new ( ELeave ) CTestCAlfLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    vLayout->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// End of file

