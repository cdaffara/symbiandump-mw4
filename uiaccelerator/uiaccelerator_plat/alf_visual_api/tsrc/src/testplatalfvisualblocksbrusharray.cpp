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
#include <alf/alfbrusharray.h>
#include <alf/alflayout.h>
#include <alf/alfborderbrush.h>

#include "testplatalfvisual.h"


// CONSTANTS
const TInt KBrushParam = 3;
const TInt KBrushCount = 4;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayResetL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayResetL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayResetL, "TestBrushArrayResetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayResetL );
    // Print to log file
    iLog->Log( KTestBrushArrayResetL );
    
    CAlfBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 0, 0, 0, 0 );
    CAlfBrush* brushOwn = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );

    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    STIF_ASSERT_EQUALS( 0, brushArray->Count() );
    
    brushArray->AppendL( brush, EAlfDoesNotHaveOwnership );
    brushArray->AppendL( brushOwn, EAlfHasOwnership );
    
    STIF_ASSERT_EQUALS( 2, brushArray->Count() );
    
    brushArray->Reset();
    STIF_ASSERT_EQUALS( 0, brushArray->Count() );
    
    CleanupStack::Pop( brushOwn );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayAppendL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayAppendL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayAppendL, "TestBrushArrayAppendL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayAppendL );
    // Print to log file
    iLog->Log( KTestBrushArrayAppendL );
    
    CAlfBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 0, 0, 0, 0 );
    CAlfBrush* brushOwn = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );
    
    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    
    STIF_ASSERT_EQUALS( 0, brushArray->Count() );
    brushArray->AppendL( brushOwn, EAlfHasOwnership );
    brushArray->AppendL( brush, EAlfDoesNotHaveOwnership );
    STIF_ASSERT_EQUALS( 2, brushArray->Count() );
    
    CleanupStack::Pop( brushOwn );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayInsertL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayInsertL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayInsertL, "TestBrushArrayInsertL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayInsertL );
    // Print to log file
    iLog->Log( KTestBrushArrayInsertL );
    
    CAlfBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 0, 0, 0, 0 );
    CAlfBrush* brushOwn = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );
    
    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    
    STIF_ASSERT_EQUALS( 0, brushArray->Count() );
    brushArray->AppendL( brush, EAlfDoesNotHaveOwnership );
    brushArray->InsertL( 0, brushOwn, EAlfHasOwnership );
    STIF_ASSERT_EQUALS( 2, brushArray->Count() );
    
    CleanupStack::Pop( brushOwn );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayRemoveL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayRemoveL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayRemoveL, "TestBrushArrayRemoveL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayRemoveL );
    // Print to log file
    iLog->Log( KTestBrushArrayRemoveL );
    
    CAlfBrush* brush = CAlfBorderBrush::NewLC( *iAlfEnv, 0, 0, 0, 0 );
    CAlfBrush* brushOwn = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );
    
    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    
    STIF_ASSERT_EQUALS( 0, brushArray->Count() );
    brushArray->AppendL( brushOwn, EAlfHasOwnership );
    brushArray->AppendL( brush, EAlfDoesNotHaveOwnership );
    STIF_ASSERT_EQUALS( 2, brushArray->Count() );
    CleanupStack::Pop( brushOwn );
    brushArray->Remove( 0 );
    STIF_ASSERT_EQUALS( 1, brushArray->Count() );
    brushArray->Remove( 0 );
    STIF_ASSERT_EQUALS( 0, brushArray->Count() );
    CleanupStack::PopAndDestroy( brush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayCountL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayCountL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayCountL, "TestBrushArrayCountL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayCountL );
    // Print to log file
    iLog->Log( KTestBrushArrayCountL );
    
    CAlfBrush* brush1 = CAlfBorderBrush::NewLC( *iAlfEnv, 0, 0, 0, 0 );
    CAlfBrush* brush2 = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );
    CAlfBrush* brushOwn1 = CAlfBorderBrush::NewLC( *iAlfEnv, 2, 2, 2, 2 );
    CAlfBrush* brushOwn2 = CAlfBorderBrush::NewLC( *iAlfEnv,
            KBrushParam, KBrushParam, KBrushParam, KBrushParam );
    
    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    
    TInt count = brushArray->Count();
    STIF_ASSERT_EQUALS( 0, count );
    
    brushArray->AppendL( brushOwn1, EAlfHasOwnership );
    brushArray->AppendL( brush1, EAlfDoesNotHaveOwnership );
    brushArray->AppendL( brushOwn2, EAlfHasOwnership );
    brushArray->AppendL( brush2, EAlfDoesNotHaveOwnership );
    
    count = brushArray->Count();
    STIF_ASSERT_EQUALS( KBrushCount, count );
    
    CleanupStack::Pop( brushOwn2 );
    CleanupStack::Pop( brushOwn1 );
    CleanupStack::PopAndDestroy( brush2 );
    CleanupStack::PopAndDestroy( brush1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayOperatorL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayOperatorL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayOperatorL, "TestBrushArrayOperatorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayOperatorL );
    // Print to log file
    iLog->Log( KTestBrushArrayOperatorL );
    
    CAlfBrush* brushOwn = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );
    
    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    
    brushArray->AppendL( brushOwn, EAlfHasOwnership );
    
    if ( brushArray->Count() > 0 )
        {
        CAlfBrush* brush = &(*brushArray)[0];
        STIF_ASSERT_EQUALS( brush, brushOwn );
        }
    
    CleanupStack::Pop( brushOwn );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestBrushArrayAtL
// -----------------------------------------------------------------------------
TInt CTestPlatAlfVisual::TestBrushArrayAtL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestBrushArrayAtL, "TestBrushArrayAtL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestBrushArrayAtL );
    // Print to log file
    iLog->Log( KTestBrushArrayAtL );
    
    CAlfBrush* brushOwn = CAlfBorderBrush::NewLC( *iAlfEnv, 1, 1, 1, 1 );
    
    CAlfVisual* visual = CAlfLayout::AddNewL( *iAlfCtl );
    visual->EnableBrushesL();
    CAlfBrushArray* brushArray = visual->Brushes();
    
    brushArray->AppendL( brushOwn, EAlfHasOwnership );
    
    if ( brushArray->Count() > 0 )
        {
        CAlfBrush* brush = &brushArray->At( 0 );
        STIF_ASSERT_EQUALS( brush, brushOwn );
        }
    
    CleanupStack::Pop( brushOwn );
    
    return KErrNone;
    }


//  [End of File]
