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
// CTestPlatAlfVisual::TestAlfImageBrushNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImageBrushNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImageBrushNewL, "TestAlfImageBrushNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImageBrushNewL );
    // Print to log file
    iLog->Log( KTestAlfImageBrushNewL );
    
    TAlfImage vImage;
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    CAlfTexture* vText = &( vTextManager->BlankTexture() );
    vImage.SetTexture( *vText );
    CAlfImageBrush* vBrush = CAlfImageBrush::NewL( *iAlfEnv, vImage );
    CleanupStack::PushL( vBrush );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    vBrush = NULL;
    vBrush = CAlfImageBrush::NewLC( *iAlfEnv, vImage );
    STIF_ASSERT_NOT_NULL( vBrush );
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImageBrushSetAndGetParaL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImageBrushSetAndGetParaL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImageBrushSetAndGetParaL, "TestAlfImageBrushSetAndGetParaL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImageBrushSetAndGetParaL );
    // Print to log file
    iLog->Log( KTestAlfImageBrushSetAndGetParaL );
    
    TAlfImage vImage;
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    CAlfTexture* vText = &( vTextManager->BlankTexture() );
    vImage.SetTexture( *vText );
    CAlfImageBrush* vBrush = CAlfImageBrush::NewLC( *iAlfEnv, vImage );
    vBrush->SetScale( CAlfImageVisual::EScaleFitHeight );
    TAlfImage vNewImage;
    CAlfTexture* vNewText = &( vTextManager->BlankTexture() );
    vNewImage.SetTexture( *vNewText );
    vBrush->SetImage( vNewImage );
    vBrush->SetBorders( 1, 1, 1, 1 );
    TAlfTimedPoint vPoint( 1, 1 );
    vBrush->SetOffset( vPoint );
    TAlfTimedValue vValue( 1 );
    vBrush->SetScale( vValue );
    vBrush->Image();
    vBrush->BorderDrawingEnabled();
    vBrush->CenterDrawingEnabled();
    vBrush->Offset();
    vBrush->Scale();
    
    CleanupStack::PopAndDestroy( vBrush );
    
    return KErrNone;
    }


// End of file

