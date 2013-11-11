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

// CONSTANTS
const TInt KBufSize = 64;

// FORWARD DECLARATION
/*This class test protected function of CAlfDeckLayout*/
class CTestCAlfDeckLayout : public CAlfDeckLayout
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfDeckLayout();
    /**
    * Destructor.
    */
    ~CTestCAlfDeckLayout();
    /**
    * TestProtectedFuncForVisual test protected functions of CAlfDeckLayout
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfDeckLayout::CTestCAlfDeckLayout
// -----------------------------------------------------------------------------
//
CTestCAlfDeckLayout::CTestCAlfDeckLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfDeckLayout::~CTestCAlfDeckLayout
// -----------------------------------------------------------------------------
//
CTestCAlfDeckLayout::~CTestCAlfDeckLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfDeckLayout::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfDeckLayout::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDeckLayoutAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDeckLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDeckLayoutAddNewL, "TestAlfDeckLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDeckLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestAlfDeckLayoutAddNewL );
    
    CAlfDeckLayout* vLayout = CAlfDeckLayout::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDeckLayoutFullConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDeckLayoutFullConstructL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDeckLayoutFullConstructL, "TestAlfDeckLayoutFullConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDeckLayoutFullConstructL );
    // Print to log file
    iLog->Log( KTestAlfDeckLayoutFullConstructL );
    
    CAlfDeckLayout* vLayout = new ( ELeave ) CAlfDeckLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDeckLayoutForLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDeckLayoutForLayoutL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDeckLayoutForLayoutL, "TestAlfDeckLayoutForLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDeckLayoutForLayoutL );
    // Print to log file
    iLog->Log( KTestAlfDeckLayoutForLayoutL );
    
    CAlfDeckLayout* vLayout = CAlfDeckLayout::AddNewL( *iAlfCtl );
    vLayout->BaseUnit();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDeckLayoutForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDeckLayoutForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDeckLayoutForVisualL, "TestAlfDeckLayoutForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDeckLayoutForVisualL );
    // Print to log file
    iLog->Log( KTestAlfDeckLayoutForVisualL );
    
    CAlfDeckLayout* vLayout = CAlfDeckLayout::AddNewL( *iAlfCtl );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vLayout->FindTag( vBuf );
    vLayout->UpdateChildrenLayout();
    vLayout->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDeckLayoutProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDeckLayoutProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDeckLayoutProtectedFuncL, "TestAlfDeckLayoutProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDeckLayoutProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfDeckLayoutProtectedFuncL );
    
    CTestCAlfDeckLayout* vLayout = new ( ELeave ) CTestCAlfDeckLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    vLayout->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// End of file
