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
// CTestPlatAlfVisual::TestAlfGenComponentNewL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGenComponentNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGenComponentNewL, "TestAlfGenComponentNewL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGenComponentNewL );
    // Print to log file
    iLog->Log( KTestAlfGenComponentNewL );
    _LIT8( KTest, "test" );
    TBuf8<KBufSize> vBuf( KTest );
    CAlfGenComponent* vComponent = CAlfGenComponent::NewL( *iAlfEnv, 0, 0x0DDDDDDE,
            vBuf );
    CleanupStack::PushL( vComponent );
    STIF_ASSERT_NOT_NULL( vComponent );
    CleanupStack::PopAndDestroy( vComponent );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfGenComponentProcessL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfGenComponentProcessL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfGenComponentProcessL, "TestAlfGenComponentProcessL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfGenComponentProcessL );
    // Print to log file
    iLog->Log( KTestAlfGenComponentProcessL );
    _LIT8( KTest, "test" );
    TBuf8<KBufSize> vBuf( KTest );
    CAlfGenComponent* vComponent = CAlfGenComponent::NewL( *iAlfEnv, 0, 0x0DDDDDDE,
            vBuf );
    CleanupStack::PushL( vComponent );
    vComponent->Identifier();
    _LIT8( KInput, "in" );
    TBuf8<KBufSize> vInputBuf( KInput );
    _LIT8( KOut, "out" );
    TBuf8<KBufSize> vOutBuf( KOut );
    vComponent->DoSynchronousCmd( 0, vInputBuf, vOutBuf );
    vComponent->CancelAsynchCmd( 0 );
    vComponent->DoAsynchronousCmdL( 0, vInputBuf, vOutBuf, NULL );
    vComponent->DoCmdNoReply( 0 , vInputBuf );
    
    CleanupStack::PopAndDestroy( vComponent );
    
    return KErrNone;
    }

// End of file


