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
// CTestPlatAlfVisual::TestAlfMaterialConstructor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfMaterialConstructor( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfMaterialConstructor, "TestAlfMaterialConstructor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfMaterialConstructor );
    // Print to log file
    iLog->Log( KTestAlfMaterialConstructor );
    
    TAlfMaterial vMaterial;
    
    return KErrNone;
    }

// End of file

