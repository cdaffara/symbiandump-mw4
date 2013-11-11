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
/*This class test protected functions of CAlfFlowLayout*/
class CTestCAlfFlowLayout : public CAlfFlowLayout
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfFlowLayout();
    /**
    * Destructor.
    */
    ~CTestCAlfFlowLayout();
    /**
    * TestProtectedFuncForVisual test protected functions of CAlfFlowLayout
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfFlowLayout::CTestCAlfFlowLayout
// -----------------------------------------------------------------------------
//
CTestCAlfFlowLayout::CTestCAlfFlowLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfFlowLayout::~CTestCAlfFlowLayout
// -----------------------------------------------------------------------------
//
CTestCAlfFlowLayout::~CTestCAlfFlowLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfFlowLayout::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfFlowLayout::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFlowLayoutAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFlowLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFlowLayoutAddNewL, "TestAlfFlowLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFlowLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestAlfFlowLayoutAddNewL );
    
    CAlfFlowLayout* vLayout = CAlfFlowLayout::AddNewL( *iAlfCtl );
    STIF_ASSERT_NOT_NULL( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFlowLayoutFullConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFlowLayoutFullConstructL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFlowLayoutFullConstructL, "TestAlfFlowLayoutFullConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFlowLayoutFullConstructL );
    // Print to log file
    iLog->Log( KTestAlfFlowLayoutFullConstructL );
    
    CAlfFlowLayout* vLayout = new ( ELeave ) CAlfFlowLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFlowLayoutForLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFlowLayoutForLayoutL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFlowLayoutForLayoutL, "TestAlfFlowLayoutForLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFlowLayoutForLayoutL );
    // Print to log file
    iLog->Log( KTestAlfFlowLayoutForLayoutL );
    
    CAlfFlowLayout* vLayout = CAlfFlowLayout::AddNewL( *iAlfCtl );
    vLayout->BaseUnit();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFlowLayoutForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFlowLayoutForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFlowLayoutForVisualL, "TestAlfFlowLayoutForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFlowLayoutForVisualL );
    // Print to log file
    iLog->Log( KTestAlfFlowLayoutForVisualL );
    
    CAlfFlowLayout* vLayout = CAlfFlowLayout::AddNewL( *iAlfCtl );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vLayout->FindTag( vBuf );
    vLayout->UpdateChildrenLayout();
    vLayout->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFlowLayoutProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFlowLayoutProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFlowLayoutProtectedFuncL, "TestAlfFlowLayoutProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFlowLayoutProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfFlowLayoutProtectedFuncL );
    
    CTestCAlfFlowLayout* vLayout = new ( ELeave ) CTestCAlfFlowLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    vLayout->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFlowLayoutSetParametersL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFlowLayoutSetParametersL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFlowLayoutSetParametersL, "TestAlfFlowLayoutSetParametersL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFlowLayoutSetParametersL );
    // Print to log file
    iLog->Log( KTestAlfFlowLayoutSetParametersL );
    
    CAlfFlowLayout* vLayout = CAlfFlowLayout::AddNewL( *iAlfCtl );
    vLayout->SetFlowDirection( CAlfFlowLayout::EFlowHorizontal );
    vLayout->SetCentering( ETrue );
    vLayout->SetMode( CAlfFlowLayout::EModeCenterPerpendicular );
    vLayout->Mode();
    
    return KErrNone;
    }

// End of file

