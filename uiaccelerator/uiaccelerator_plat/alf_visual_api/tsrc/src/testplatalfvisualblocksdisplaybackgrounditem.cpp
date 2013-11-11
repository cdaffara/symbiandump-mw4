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
// CTestPlatAlfVisual::TestAlfDisplayBgItemConstructor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayBgItemConstructor( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayBgItemConstructor, "TestAlfDisplayBgItemConstructor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayBgItemConstructor );
    // Print to log file
    iLog->Log( KTestAlfDisplayBgItemConstructor );
    
    TAlfDisplayBackgroundItem vItem1;
    
    TRect rect( 0, 0, 1, 1 );
    TAlfDisplayBackgroundItem vItem2( rect );
    
    TAknsItemID vId;
    TAlfDisplayBackgroundItem vItem3( rect, vId );
    
    TAlfDisplayBackgroundItem vItem4( rect, vId, 0 );
    
    TRgb vRgb( KRgbBlack );
    TAlfDisplayBackgroundItem vItem5( rect, vRgb );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfDisplayBgItemSetAttributes
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDisplayBgItemSetAttributes( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDisplayBgItemSetAttributes, "TestAlfDisplayBgItemSetAttributes" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDisplayBgItemSetAttributes );
    // Print to log file
    iLog->Log( KTestAlfDisplayBgItemSetAttributes );
    
    TRgb vRgb( KRgbBlack );
    TAknsItemID vId;
    TRect rect( 0, 0, 1, 1 );
    TAlfDisplayBackgroundItem vItem;
    vItem.SetRect( rect );
    vItem.SetColor( vRgb );
    vItem.SetColor( vId, 0 );
    vItem.SetSkinBackground( vId );
    return KErrNone;
    }

// End of file


