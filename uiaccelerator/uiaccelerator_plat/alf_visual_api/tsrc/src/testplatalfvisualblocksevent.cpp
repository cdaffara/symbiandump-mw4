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
// CTestPlatAlfVisual::TestAlfEventConstructor
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEventConstructor( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEventConstructor, "TestAlfEventConstructor" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEventConstructor );
    // Print to log file
    iLog->Log( KTestAlfEventConstructor );
    
    TAlfEvent vEvent1( 0 );
    TAlfEvent vEvent2( 0, 0 );
    TAlfScrollerEvent vScrollEvent( TAlfScrollerEvent::EMoveByItem,
            TAlfScrollerEvent::EHorizontal, 0 );
    TAlfEvent vEvent3( *iAlfDisplay, vScrollEvent );
    TKeyEvent vKey;
    TAlfEvent vEvent4( *iAlfDisplay, vKey, EEventKey );
    TPointerEvent vPointer;
    TAlfEvent vEvent5( *iAlfDisplay, vPointer );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEventParameters
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEventParameters( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEventParameters, "TestAlfEventParameters" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEventParameters );
    // Print to log file
    iLog->Log( KTestAlfEventParameters );
    
    TPointerEvent vPointer;
    TAlfEvent vPointerEvent( *iAlfDisplay, vPointer );
    vPointerEvent.PointerDown();
    vPointerEvent.PointerUp();
    vPointerEvent.PointerLongTap();
    vPointerEvent.IsPointerEvent();
    vPointerEvent.PointerEvent();
    vPointerEvent.Type();
    TAlfEvent vCustomEvent( 0, 0 );
    vCustomEvent.IsCustomEvent();
    TAlfScrollerEvent vScroll( TAlfScrollerEvent::EMoveByItem,
            TAlfScrollerEvent::EHorizontal, 0 );
    TAlfEvent vScrollEvent( *iAlfDisplay, vScroll );
    vScrollEvent.ScrollerEvent();
    vScrollEvent.IsScrollerEvent();
    TKeyEvent vKey;
    TAlfEvent vKeyEvent( *iAlfDisplay, vKey, EEventKey );
    vKeyEvent.IsKeyEvent();
    vKeyEvent.Code();
    vKeyEvent.KeyEvent();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEventCustomData
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEventCustomData( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEventCustomData, "TestAlfEventCustomData" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEventCustomData );
    // Print to log file
    iLog->Log( KTestAlfEventCustomData );
    
    TAlfEvent vCustomEvent( 0, 0 );
    vCustomEvent.CustomEventData();
    vCustomEvent.CustomParameter();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestAlfEventDisplayAndVisual
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfEventDisplayAndVisual( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfEventDisplayAndVisual, "TestAlfEventDisplayAndVisual" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfEventDisplayAndVisual );
    // Print to log file
    iLog->Log( KTestAlfEventDisplayAndVisual );
    
    TKeyEvent vKey;
    TAlfEvent vKeyEvent( *iAlfDisplay, vKey, EEventKey );
    vKeyEvent.Display();
    vKeyEvent.SetVisual( NULL );
    vKeyEvent.Visual();
    
    return KErrNone;
    }

// End of file


