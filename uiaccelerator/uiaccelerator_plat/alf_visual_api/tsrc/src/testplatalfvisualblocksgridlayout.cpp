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
/*This class test protected functions of CAlfGridLayout*/
class CTestCAlfGridLayout : public CAlfGridLayout
    {
public:
    /**
    * C++ default constructor.
    */
    CTestCAlfGridLayout();
    /**
    * Destructor.
    */
    ~CTestCAlfGridLayout();
    /**
    * TestProtectedFuncForVisual test protected functions of CAlfGridLayout
    * @since S60 5.0
    * @param aItem is not used.
    * @return Symbian OS error code.
    */
    TInt TestProtectedFuncForVisual( CStifItemParser& aItem );
    };

// -----------------------------------------------------------------------------
// CTestCAlfGridLayout::CTestCAlfGridLayout
// -----------------------------------------------------------------------------
//
CTestCAlfGridLayout::CTestCAlfGridLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfGridLayout::~CTestCAlfGridLayout
// -----------------------------------------------------------------------------
//
CTestCAlfGridLayout::~CTestCAlfGridLayout(){}

// -----------------------------------------------------------------------------
// CTestCAlfGridLayout::TestProtectedFuncForVisual
// -----------------------------------------------------------------------------
//
TInt CTestCAlfGridLayout::TestProtectedFuncForVisual( CStifItemParser& /*aItem*/ )
    {
    TUid vUid = { 0x00000000 };
    PropertyOwnerExtension( vUid, NULL );
    DoRemoveAndDestroyAllD();
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutAddNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutAddNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutAddNewL, "TestAlfGridLayoutAddNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutAddNewL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutAddNewL );
    
    CAlfGridLayout* vLayout = CAlfGridLayout::AddNewL( *iAlfCtl, 1, 1 );
    STIF_ASSERT_NOT_NULL( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutFullConstructL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutFullConstructL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutFullConstructL, "TestAlfGridLayoutFullConstructL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutFullConstructL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutFullConstructL );
    
    CAlfGridLayout* vLayout = new ( ELeave ) CAlfGridLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutForLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutForLayoutL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutForLayoutL, "TestAlfGridLayoutForLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutForLayoutL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutForLayoutL );
    
    CAlfGridLayout* vLayout = CAlfGridLayout::AddNewL( *iAlfCtl, 1, 1 );
    vLayout->BaseUnit();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutForVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutForVisualL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutForVisualL, "TestAlfGridLayoutForVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutForVisualL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutForVisualL );
    
    CAlfGridLayout* vLayout = CAlfGridLayout::AddNewL( *iAlfCtl, 1, 1 );
    _LIT8( KTest, "tset" );
    TBuf8<KBufSize> vBuf( KTest );
    vLayout->FindTag( vBuf );
    vLayout->UpdateChildrenLayout();
    vLayout->RemoveAndDestroyAllD();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutProtectedFuncL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutProtectedFuncL( CStifItemParser& aItem )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutProtectedFuncL, "TestAlfGridLayoutProtectedFuncL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutProtectedFuncL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutProtectedFuncL );
    
    CTestCAlfGridLayout* vLayout = new ( ELeave ) CTestCAlfGridLayout;
    CleanupStack::PushL( vLayout );
    vLayout->ConstructL( *iAlfCtl );
    vLayout->TestProtectedFuncForVisual( aItem );
    CleanupStack::Pop( vLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutSetParametersL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutSetParametersL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutSetParametersL, "TestAlfGridLayoutSetParametersL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutSetParametersL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutSetParametersL );
    
    CAlfGridLayout* vLayout = CAlfGridLayout::AddNewL( *iAlfCtl, 1, 1 );
    vLayout->SetColumnsL( 1 );
    vLayout->SetRowsL( 1 );
    RArray<TInt> vWeights(2);
    vWeights.Append( 1 );
    vWeights.Append( 1 );
    vLayout->SetRowsL( vWeights );
    vLayout->SetColumnsL( vWeights );
    TAlfMetric vMetric( 1 );
    vLayout->FillWeightsL( EAlfGridColumn, 1, vMetric );
    vLayout->InsertWeightL( EAlfGridColumn, vMetric, 0 );
    vLayout->ReplaceWeightL( EAlfGridColumn, vMetric, 1 );
    vLayout->DimensionCount( EAlfGridColumn );
    vLayout->RemoveWeightL( EAlfGridColumn, 1 );
    vLayout->SetExpanding( 0 );
    vLayout->OrdinalToBlock( 0 );
    vLayout->SetLayoutModeFlags( EAlfGridColumn, 0 );
    vLayout->ClearLayoutModeFlags( EAlfGridColumn, 0 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGridLayoutGetParaL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGridLayoutGetParaL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGridLayoutGetParaL, "TestAlfGridLayoutGetParaL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGridLayoutGetParaL );
    // Print to log file
    iLog->Log( KTestAlfGridLayoutGetParaL );
    
    CAlfGridLayout* vLayout = CAlfGridLayout::AddNewL( *iAlfCtl, 1, 1 );
    TAlfMetric vMetric( 1 );
    vLayout->InsertWeightL( EAlfGridColumn, vMetric, 0 );
    vLayout->Weight( EAlfGridColumn, 1 );
    vLayout->LayoutModeFlags( EAlfGridColumn );
    vLayout->RowCount();
    vLayout->ColumnCount();
    
    return KErrNone;
    }

// End of file

