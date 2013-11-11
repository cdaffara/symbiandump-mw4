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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgLoadUtilConstructorL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgLoadUtilConstructorL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgLoadUtilConstructorL, "TestAlfImgLoadUtilConstructorL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgLoadUtilConstructorL );
    // Print to log file
    iLog->Log( KTestAlfImgLoadUtilConstructorL );
    
    CAlfImageLoaderUtil* vUtil = new ( ELeave ) CAlfImageLoaderUtil;
    CleanupStack::PushL( vUtil );
    STIF_ASSERT_NOT_NULL( vUtil );
    CleanupStack::PopAndDestroy( vUtil );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgLoadUtilCreateImageLoaderL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgLoadUtilCreateImageLoaderL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgLoadUtilCreateImageLoaderL, "TestAlfImgLoadUtilCreateImageLoaderL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgLoadUtilCreateImageLoaderL );
    // Print to log file
    iLog->Log( KTestAlfImgLoadUtilCreateImageLoaderL );
    
    CAlfImageLoaderUtil* vUtil = new ( ELeave ) CAlfImageLoaderUtil;
    CleanupStack::PushL( vUtil );
    const TInt KIconIdMessaging = 268458181; 
    TUid uid; 
    uid.iUid = KIconIdMessaging;
    vUtil->CreateImageLoaderL( uid, EAknsAppIconTypeContext );
    TAknsItemID vID;
    TRect vInnerRect( 0, 0, 1, 1 );
    TRect vOutRect( 0, 0, 1, 1 );
    vUtil->CreateImageLoaderL( vID, vInnerRect, vOutRect );
    _LIT( KBitmapFile, "Z:\\resource\\apps\\avkon2.mbm" );
    TBuf<KBufSize> vBuf( KBitmapFile );
    const TInt number = 6000;
    vUtil->CreateImageLoaderL( vID, vBuf, number, number );
    vUtil->CreateSVGImageLoaderL( vBuf );
    CleanupStack::PopAndDestroy( vUtil );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfImgLoadUtilSetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfImgLoadUtilSetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfImgLoadUtilSetL, "TestAlfImgLoadUtilSetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfImgLoadUtilSetL );
    // Print to log file
    iLog->Log( KTestAlfImgLoadUtilSetL );
    
    CAlfImageLoaderUtil* vUtil = new ( ELeave ) CAlfImageLoaderUtil;
    CleanupStack::PushL( vUtil );
    TUid vUid = { 0x00000001 };
    vUtil->CreateImageLoaderL( vUid, EAknsAppIconTypeContext );
    vUtil->SetSize( TSize( 1, 1 ) );
    vUtil->SetSkinInstance( NULL );
    CleanupStack::PopAndDestroy( vUtil );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfAutoSizeImgLoadUtilSetL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfAutoSizeImgLoadUtilSetL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfAutoSizeImgLoadUtilSetL, "TestAlfAutoSizeImgLoadUtilSetL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfAutoSizeImgLoadUtilSetL );
    // Print to log file
    iLog->Log( KTestAlfAutoSizeImgLoadUtilSetL );
    
    CAlfAutoSizeImageLoaderUtil* vUtil = new ( ELeave ) CAlfAutoSizeImageLoaderUtil;
    CleanupStack::PushL( vUtil );
    STIF_ASSERT_NOT_NULL( vUtil );
    CleanupStack::PopAndDestroy( vUtil );
    
    return KErrNone;
    }

// End of file


