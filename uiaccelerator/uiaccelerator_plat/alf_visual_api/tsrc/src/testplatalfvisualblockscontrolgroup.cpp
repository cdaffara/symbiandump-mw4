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
// CTestPlatAlfVisual::TestAlfCtrlGroupResourceId
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlGroupResourceId( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlGroupResourceIdL, "TestAlfCtrlGroupResourceIdL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlGroupResourceIdL );
    // Print to log file
    iLog->Log( KTestAlfCtrlGroupResourceIdL );
    
    TInt vId = iAlfCtrlGroup->ResourceId();
    STIF_ASSERT_EQUALS( 1, vId );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlGroupProcessCtrlL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlGroupProcessCtrlL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlGroupProcessCtrlL, "TestAlfCtrlGroupProcessCtrlL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlGroupProcessCtrlL );
    // Print to log file
    iLog->Log( KTestAlfCtrlGroupProcessCtrlL );
    
    CAlfControl* ctrl = new ( ELeave ) CAlfControl;
    CleanupStack::PushL( ctrl );
    ctrl->ConstructL( *iAlfEnv );
    const TInt num = 23;
    ctrl->SetId( num );
    iAlfCtrlGroup->AppendL( ctrl );
    STIF_ASSERT_EQUALS( 2, iAlfCtrlGroup->Count() );
    CAlfControl* foundControl = &iAlfCtrlGroup->Control( 1 );
    STIF_ASSERT_EQUALS( (TInt)ctrl, (TInt)foundControl );
    iAlfCtrlGroup->FindControl( num );
    foundControl = iAlfCtrlGroup->FindControl( num );
    STIF_ASSERT_NOT_EQUALS( 0, (TInt)foundControl );
    iAlfCtrlGroup->Remove( ctrl );
    CleanupStack::PopAndDestroy( ctrl );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlGroupProcessInput
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlGroupProcessInput( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlGroupProcessInput, "TestAlfCtrlGroupProcessInput" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlGroupProcessInput );
    // Print to log file
    iLog->Log( KTestAlfCtrlGroupProcessInput );
    
    iAlfCtrlGroup->SetAcceptInput( ETrue );
    STIF_ASSERT_TRUE( iAlfCtrlGroup->AcceptInput() );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfCtrlGroupProcessTransformL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfCtrlGroupProcessTransformL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfCtrlGroupProcessTransformL, "TestAlfCtrlGroupProcessTransformL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfCtrlGroupProcessTransformL );
    // Print to log file
    iLog->Log( KTestAlfCtrlGroupProcessTransformL );
    
    iAlfCtrlGroup->EnableTransformationL( ETrue );
    STIF_ASSERT_TRUE( iAlfCtrlGroup->IsTransformed() );
    iAlfCtrlGroup->Transformation();
    iAlfCtrlGroup->EnableTransformationL( EFalse );
    STIF_ASSERT_FALSE( iAlfCtrlGroup->IsTransformed() );
    
    return KErrNone;
    }

// End of file

