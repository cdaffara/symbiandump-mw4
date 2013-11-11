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
* Description:  test functions for alfstatic.h
*
*/



// [INCLUDE FILES]
#include <alf/alfstatic.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStaticEnv
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStaticEnv( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStaticEnv, "TestStaticEnv" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStaticEnv );
    // Print to log file
    iLog->Log( KTestStaticEnv );
    
    STIF_ASSERT_EQUALS( iAlfEnv, &CAlfStatic::Env() );
    
    return KErrNone;
    }


//  [End of File]
