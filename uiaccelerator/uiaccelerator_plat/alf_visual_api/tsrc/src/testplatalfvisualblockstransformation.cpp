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
* Description:  test functions for alftransformation.h
*
*/



// [INCLUDE FILES]
#include <alf/alftransformation.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransNewL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransNewL, "TestTransNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransNewL );
    // Print to log file
    iLog->Log( KTestTransNewL );
    
    CAlfTransformation* alfTrans = CAlfTransformation::NewL( *iAlfEnv );
    CleanupStack::PushL( alfTrans );
    STIF_ASSERT_NOT_NULL( alfTrans );
    CleanupStack::PopAndDestroy( alfTrans );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransNewLCL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransNewLCL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransNewLCL, "TestTransNewLCL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransNewLCL );
    // Print to log file
    iLog->Log( KTestTransNewLCL );
    
    CAlfTransformation* alfTrans = CAlfTransformation::NewLC( *iAlfEnv );
    STIF_ASSERT_NOT_NULL( alfTrans );
    CleanupStack::PopAndDestroy( alfTrans );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransCountL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransCountL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransCountL, "TestTransCountL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransCountL );
    // Print to log file
    iLog->Log( KTestTransCountL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    alfTrans.Count();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransNonIdentityCountL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransNonIdentityCountL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransNonIdentityCountL, "TestTransNonIdentityCountL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransNonIdentityCountL );
    // Print to log file
    iLog->Log( KTestTransNonIdentityCountL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    alfTrans.NonIdentityCount();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransStepL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransStepL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransStepL, "TestTransStepL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransStepL );
    // Print to log file
    iLog->Log( KTestTransStepL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    alfTrans.Rotate( 0, 0, 1, 0 );
    alfTrans.Rotate( 0, 1, 0, 0 );
    alfTrans.Step( 1 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransReplaceStepL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransReplaceStepL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransReplaceStepL, "TestTransReplaceStepL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransReplaceStepL );
    // Print to log file
    iLog->Log( KTestTransReplaceStepL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    alfTrans.Rotate( 0, 0, 1, 0 );
    alfTrans.Rotate( 0, 1, 0, 0 );
    CAlfTransformation::TTransform transStep = alfTrans.Step( 1 );;
    alfTrans.ReplaceStep( 1, transStep );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransLoadIdentityL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransLoadIdentityL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransLoadIdentityL, "TestTransLoadIdentityL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransLoadIdentityL );
    // Print to log file
    iLog->Log( KTestTransLoadIdentityL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    alfTrans.LoadIdentity();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransTranslateL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransTranslateL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransTranslateL, "TestTransTranslateL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransTranslateL );
    // Print to log file
    iLog->Log( KTestTransTranslateL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    
    alfTrans.Translate( 1.0, 1.0 );
    TAlfTimedValue x, y;
    alfTrans.Translate( x, y );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransScaleL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransScaleL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransScaleL, "TestTransScaleL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransScaleL );
    // Print to log file
    iLog->Log( KTestTransScaleL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    
    alfTrans.Scale( 1.0, 2.0 );
    TAlfTimedValue x, y;
    alfTrans.Scale( x, y );
    alfTrans.Scale( 1.0, 2.0, 3.0 );
    TAlfTimedValue z;
    alfTrans.Scale( x, y, z );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestTransRotateL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestTransRotateL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestTransRotateL, "TestTransRotateL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestTransRotateL );
    // Print to log file
    iLog->Log( KTestTransRotateL );
    
    iAlfCtrlGroup->EnableTransformationL();
    CAlfTransformation& alfTrans = iAlfCtrlGroup->Transformation();
    
    TAlfTimedValue angle;
    alfTrans.Rotate( 1.0 );
    alfTrans.Rotate( angle );
    alfTrans.Rotate( 1.0, 2.0, 3.0, 4.0 );
    alfTrans.Rotate( angle, 1.0, 2.0, 3.0 );
    
    return KErrNone;
    }


//  [End of File]
