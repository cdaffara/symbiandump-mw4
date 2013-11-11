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
/*This class test protected function of CAlfCurvePathLayout*/
class CTestCAlfCurvePathLayout : public CAlfCurvePathLayout
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfCurvePathLayout();
    /**
    * Destructor.
    */
    ~CTestCAlfCurvePathLayout();
    /**
    * TestProtectedFuncForVisual test protected functions of CAlfCurvePathLayout
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfCurvePathLayout::CTestCAlfCurvePathLayout
// -----------------------------------------------------------------------------
//
CTestCAlfCurvePathLayout::CTestCAlfCurvePathLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfCurvePathLayout::~CTestCAlfCurvePathLayout
// -----------------------------------------------------------------------------
//
CTestCAlfCurvePathLayout::~CTestCAlfCurvePathLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfCurvePathLayout::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfCurvePathLayout::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLayoutAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLayoutAddNewL, "TestAlfCurvePathLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLayoutAddNewL );
    
    CAlfCurvePathLayout* vLayout = CAlfCurvePathLayout::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLayoutFullConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLayoutFullConstructL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLayoutFullConstructL, "TestAlfCurvePathLayoutFullConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLayoutFullConstructL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLayoutFullConstructL );
    
    CAlfCurvePathLayout* vLayout = new ( ELeave ) CAlfCurvePathLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLayoutForLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLayoutForLayoutL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLayoutForLayoutL, "TestAlfCurvePathLayoutForLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLayoutForLayoutL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLayoutForLayoutL );
    
    CAlfCurvePathLayout* vLayout = CAlfCurvePathLayout::AddNewL( *iAlfCtl );
    vLayout->BaseUnit();
    vLayout->CurvePath();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLayoutForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLayoutForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLayoutForVisualL, "TestAlfCurvePathLayoutForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLayoutForVisualL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLayoutForVisualL );
    
    CAlfCurvePathLayout* vLayout = CAlfCurvePathLayout::AddNewL( *iAlfCtl );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vLayout->FindTag( vBuf );
    vLayout->UpdateChildrenLayout();
    vLayout->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLayoutForMapL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLayoutForMapL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLayoutForMapL, "TestAlfCurvePathLayoutForMapL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLayoutForMapL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLayoutForMapL );
    
    CAlfCurvePathLayout* vLayout = CAlfCurvePathLayout::AddNewL( *iAlfCtl );
    
    vLayout->MappingFunctionIdentifier();
    vLayout->MapValue( 1, 0 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCurvePathLayoutProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCurvePathLayoutProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCurvePathLayoutProtectedFuncL, "TestAlfCurvePathLayoutProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCurvePathLayoutProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfCurvePathLayoutProtectedFuncL );
    
    CTestCAlfCurvePathLayout* vLayout = new ( ELeave ) CTestCAlfCurvePathLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    vLayout->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// End of file
