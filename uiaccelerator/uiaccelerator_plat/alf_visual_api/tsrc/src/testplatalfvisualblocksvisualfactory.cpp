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
* Description:  test functions for alfvisualfactory.h
*
*/



// [INCLUDE FILES]
#include <alf/alflayout.h>
#include <alf/alfvisual.h>
#include <alf/alfvisualfactory.h>

#include "testplatalfvisual.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfVFNewLayoutL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfVFNewLayoutL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfVFNewLayoutL, "TestAlfVFNewLayoutL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfVFNewLayoutL );
    // Print to log file
    iLog->Log( KTestAlfVFNewLayoutL );
    
    CAlfLayout* alfLayout = AlfVisualFactory::NewLayoutL( 
            EAlfLayoutTypeLayout, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfLayout );
    STIF_ASSERT_NOT_NULL( alfLayout );
    CleanupStack::PopAndDestroy( alfLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfVFNewLayoutLCL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfVFNewLayoutLCL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfVFNewLayoutLCL, "TestAlfVFNewLayoutLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfVFNewLayoutLCL );
    // Print to log file
    iLog->Log( KTestAlfVFNewLayoutLCL );
    
    CAlfLayout* alfLayout = AlfVisualFactory::NewLayoutLC(
            EAlfLayoutTypeLayout, NULL, *iAlfCtl, *iAlfEnv );
    STIF_ASSERT_NOT_NULL( alfLayout );
    CleanupStack::PopAndDestroy( alfLayout );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfVFNewVisualL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfVFNewVisualL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfVFNewVisualL, "TestAlfVFNewVisualL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfVFNewVisualL );
    // Print to log file
    iLog->Log( KTestAlfVFNewVisualL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualL(
            EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    CleanupStack::PushL( alfVisual );
    STIF_ASSERT_NOT_NULL( alfVisual );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfVFNewVisualLCL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfVFNewVisualLCL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfVFNewVisualLCL, "TestAlfVFNewVisualLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfVFNewVisualLCL );
    // Print to log file
    iLog->Log( KTestAlfVFNewVisualLCL );
    
    CAlfVisual* alfVisual = AlfVisualFactory::NewVisualLC(
            EAlfVisualTypeVisual, NULL, *iAlfCtl, *iAlfEnv );
    STIF_ASSERT_NOT_NULL( alfVisual );
    CleanupStack::PopAndDestroy( alfVisual );
    
    return KErrNone;
    }


//  [End of File]
