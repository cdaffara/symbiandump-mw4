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
#include <aknsconstants.h>

#include "testplatalfvisual.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImageConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImageConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImageConstructorL, "TestAlfImageConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImageConstructorL );
    // Print to log file
    iLog->Log( KTestAlfImageConstructorL );
    
    TAlfImage vImage1;
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    const CAlfTexture* vText = vTextManager->Texture( 0 );
    TAlfImage vImage2( *vText );
    TAlfImage vImage3( KAknsIIDNone, TSize( 1, 1) );
    TAlfImage vImage4( NULL, TSize( 1, 1), EAspectRatioPreservedAndUnusedSpaceRemoved,
           NULL, 0, 0, 0, 0, 0, 0, EAlfTextureFlagDefault, 0, NULL );
    TAlfImage vImage5( vUid, EAknsAppIconTypeList, 
            TSize( 1, 1), EAspectRatioPreserved, 0, 0, 0, 0 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImageSetAttributesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImageSetAttributesL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImageSetAttributesL, "TestAlfImageSetAttributesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImageSetAttributesL );
    // Print to log file
    iLog->Log( KTestAlfImageSetAttributesL );
    
    TAlfImage vImage;
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    CAlfTexture* vText = &( vTextManager->BlankTexture() );
    vImage.SetTexture( *vText );
    vImage.Translate( 0, 0 );
    vImage.Scale( 1, 1 );

    vImage.SetTexCoords();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImageGetAttributesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImageGetAttributesL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImageGetAttributesL, "TestAlfImageGetAttributesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImageGetAttributesL );
    // Print to log file
    iLog->Log( KTestAlfImageGetAttributesL );
    
    TAlfImage vImage;
    TUid vUid = { 0x00000000 };
    CAlfTextureManager* vTextManager = iAlfEnv->CreateSharedTextureManagerL( vUid );
    CAlfTexture* vText = &( vTextManager->BlankTexture() );
    vImage.SetTexture( *vText );
    vImage.HasTexture();
    vImage.BottomRight();
    vImage.TopLeft();
    vImage.Texture();
    
    return KErrNone;
    }


// End of file

