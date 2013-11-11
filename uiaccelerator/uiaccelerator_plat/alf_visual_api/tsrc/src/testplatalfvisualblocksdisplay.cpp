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
// CTestPlatAlfVisual::TestAlfDisplayForBackgroundL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayForBackgroundL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayForBackgroundL, "TestAlfDisplayForBackgroundL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayForBackgroundL );
    // Print to log file
    iLog->Log( KTestAlfDisplayForBackgroundL );
    
    iAlfDisplay->SetClearBackgroundL( CAlfDisplay::EClearWithColor );
    RArray<TAlfDisplayBackgroundItem> vArray( 1 );
    iAlfDisplay->SetBackgroundItemsL( vArray );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayForRoster
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayForRoster( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayForRoster, "TestAlfDisplayForRoster" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayForRoster );
    // Print to log file
    iLog->Log( KTestAlfDisplayForRoster );
    
    iAlfDisplay->Roster();
    ( ( const CAlfDisplay*) iAlfDisplay )->Roster();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayForVisibleArea
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayForVisibleArea( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayForVisibleAreaL, "TestAlfDisplayForVisibleAreaL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayForVisibleAreaL );
    // Print to log file
    iLog->Log( KTestAlfDisplayForVisibleAreaL );
    TRect rect( 0, 0, 1, 1 );
    iAlfDisplay->SetVisibleArea( rect );
//    iAlfDisplay->ForceSetVisibleArea( rect );
    iAlfDisplay->VisibleArea();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayForSetAttributesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayForSetAttributesL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayForSetAttributesL, "TestAlfDisplayForSetAttributesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayForSetAttributesL );
    // Print to log file
    iLog->Log( KTestAlfDisplayForSetAttributesL );
    
//    iAlfDisplay->SetFocusedEditor( NULL );
    iAlfDisplay->SetQuality( EAlfQualityFast );
    iAlfDisplay->SetDirty();
    iAlfDisplay->SetUseDepth( EFalse );
    iAlfDisplay->SetUsageL( EAlfDisplayUsageGeneric );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayGetAttributes
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayGetAttributes( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayGetAttributes, "TestAlfDisplayGetAttributes" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayGetAttributes );
    // Print to log file
    iLog->Log( KTestAlfDisplayGetAttributes );
    
    iAlfDisplay->Env();
    iAlfDisplay->Quality();
    iAlfDisplay->ObjectProvider();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayFading
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayFading( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayFading, "TestAlfDisplayFading" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayFading );
    // Print to log file
    iLog->Log( KTestAlfDisplayFading );
    
    iAlfDisplay->SuppressAutomaticFading();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayHandlePointerEventL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayHandlePointerEventL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayHandlePointerEventL, "TestAlfDisplayHandlePointerEventL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayHandlePointerEventL );
    // Print to log file
    iLog->Log( KTestAlfDisplayHandlePointerEventL );
    
    TPointerEvent vEvent;
    iAlfDisplay->HandlePointerEventL( vEvent );
    
    return KErrNone;
    }


// End of file

