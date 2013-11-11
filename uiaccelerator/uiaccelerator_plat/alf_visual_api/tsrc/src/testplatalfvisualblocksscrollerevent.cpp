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
* Description:  test functions for alfscrollerevent.h
*
*/



// [INCLUDE FILES]
#include <alf/alfscrollerevent.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestScrollEventMethods
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestScrollEventMethods( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestScrollEventMethods, "TestScrollEventMethods" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestScrollEventMethods );
    // Print to log file
    iLog->Log( KTestScrollEventMethods );
    
    TAlfScrollerEvent scrollerEvent(
            TAlfScrollerEvent::EMoveByItem,
            TAlfScrollerEvent::EVertical, 1
            );
    scrollerEvent.Code();
    scrollerEvent.Orientation();
    scrollerEvent.Magnitude();
    
    return KErrNone;
    }


//  [End of File]
