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
* Description:  test functions for alftextureautosizeparams.h
*
*/



// [INCLUDE FILES]
#include <alf/alftextureautosizeparams.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtAutoSizeParamsConstructor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtAutoSizeParamsConstructor( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtAutoSizeParamsConstructor, "TestTxtAutoSizeParamsConstructor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtAutoSizeParamsConstructor );
    // Print to log file
    iLog->Log( KTestTxtAutoSizeParamsConstructor );
    
    TAlfTextureAutoSizeParams params;
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtAutoSizeParamsGetMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtAutoSizeParamsGetMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtAutoSizeParamsGetMethods, "TestTxtAutoSizeParamsGetMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtAutoSizeParamsGetMethods );
    // Print to log file
    iLog->Log( KTestTxtAutoSizeParamsGetMethods );
    
    TAlfTextureAutoSizeParams params;
    
    TInt target = TAlfTextureAutoSizeParams::EHigh;
    params.SetSizeLowerThreshold( target );
    TInt result = params.SizeLowerThreshold();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetSizeLowerThreshold( target );
    result = params.SizeLowerThreshold();
    STIF_ASSERT_EQUALS( target, result );
    
    target = TAlfTextureAutoSizeParams::EHigh;
    params.SetSizeUpperThreshold( target );
    result = params.SizeUpperThreshold();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetSizeUpperThreshold( target );
    result = params.SizeUpperThreshold();
    STIF_ASSERT_EQUALS( target, result );
    
    target = TAlfTextureAutoSizeParams::EHigh;
    params.SetMinSizeChange( target );
    result = params.MinSizeChange();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetMinSizeChange( target );
    result = params.MinSizeChange();
    STIF_ASSERT_EQUALS( target, result );
    
    target = TAlfTextureAutoSizeParams::EHigh;
    params.SetDownsizeSettleThreshold( target );
    result = params.DownsizeSettleThreshold();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetDownsizeSettleThreshold( target );
    result = params.DownsizeSettleThreshold();
    STIF_ASSERT_EQUALS( target, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTxtAutoSizeParamsSetMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTxtAutoSizeParamsSetMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTxtAutoSizeParamsSetMethods, "TestTxtAutoSizeParamsSetMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTxtAutoSizeParamsSetMethods );
    // Print to log file
    iLog->Log( KTestTxtAutoSizeParamsSetMethods );
    
    TAlfTextureAutoSizeParams params;
    
    TInt target = TAlfTextureAutoSizeParams::EHigh;
    params.SetSizeLowerThreshold( target );
    TInt result = params.SizeLowerThreshold();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetSizeLowerThreshold( target );
    result = params.SizeLowerThreshold();
    STIF_ASSERT_EQUALS( target, result );
    
    target = TAlfTextureAutoSizeParams::EHigh;
    params.SetSizeUpperThreshold( target );
    result = params.SizeUpperThreshold();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetSizeUpperThreshold( target );
    result = params.SizeUpperThreshold();
    STIF_ASSERT_EQUALS( target, result );
    
    target = TAlfTextureAutoSizeParams::EHigh;
    params.SetMinSizeChange( target );
    result = params.MinSizeChange();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetMinSizeChange( target );
    result = params.MinSizeChange();
    STIF_ASSERT_EQUALS( target, result );
    
    target = TAlfTextureAutoSizeParams::EHigh;
    params.SetDownsizeSettleThreshold( target );
    result = params.DownsizeSettleThreshold();
    STIF_ASSERT_EQUALS( target, result );
    target = TAlfTextureAutoSizeParams::ELow;
    params.SetDownsizeSettleThreshold( target );
    result = params.DownsizeSettleThreshold();
    STIF_ASSERT_EQUALS( target, result );
    
    return KErrNone;
    }


//  [End of File]
