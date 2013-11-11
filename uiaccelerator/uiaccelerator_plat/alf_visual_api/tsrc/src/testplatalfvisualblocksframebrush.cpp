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
// CTestPlatAlfVisual::TestAlfFrameBrushNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFrameBrushNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFrameBrushNewL, "TestAlfFrameBrushNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFrameBrushNewL );
    // Print to log file
    iLog->Log( KTestAlfFrameBrushNewL );
    
    TAlfImage vImage;
    TUid vUid;
    vUid.Null();
    CAlfFrameBrush* vBrush = CAlfFrameBrush::NewL( *iAlfEnv, vImage, 
            TAlfXYMetric( 0, 0) );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfFrameBrush::NewLC( *iAlfEnv, vImage, TAlfXYMetric( 0, 0 ) );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfFrameBrush::NewL( *iAlfEnv, KAknsIIDQsnFrList, 
            TAlfXYMetric( 0, 0 ) );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfFrameBrush::NewLC( *iAlfEnv, KAknsIIDQsnFrList, TAlfXYMetric( 0, 0 ) );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfFrameBrush::NewL( *iAlfEnv, KAknsIIDQsnFrList );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfFrameBrush::NewLC( *iAlfEnv, KAknsIIDQsnFrList );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfFrameBrushSetParaL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfFrameBrushSetParaL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfFrameBrushSetParaL, "TestAlfFrameBrushSetParaL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfFrameBrushSetParaL );
    // Print to log file
    iLog->Log( KTestAlfFrameBrushSetParaL );
    
    TAlfImage vImage;
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    CAlfTexture* vText = &( vTextManager->BlankTexture() );
    vImage.SetTexture( *vText );
    TAlfXYMetric vMetric;
    CAlfFrameBrush* vBrush = CAlfFrameBrush::NewL( *iAlfEnv, vImage, vMetric );
    CleanupStack::PushL( vBrush );
    
    TAlfImage vNewImage;
    TRect vInner( 0, 0, 100, 100 );
    TRect vOuter( 10, 10, 90, 90 );
    vBrush->SetFrameRectsL( vInner, vOuter );
    TAlfMetric valueSet(2.f);
    vBrush->SetEdgeOffset( valueSet );
    vBrush->SetImage( vNewImage );
    TAlfImage emptyImage;
    const TInt KMaxEnumValue = 9;
    for ( TInt i = 0;i < KMaxEnumValue; i++ )
        {
        vBrush->SetImage(CAlfFrameBrush::TFrameParts(i), emptyImage);
        }
    
    CleanupStack::PopAndDestroy( vBrush );
    
    
    return KErrNone;
    }


// End of file

