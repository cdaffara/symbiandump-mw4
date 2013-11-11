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
* Description:  test functions for alftextstylemanager.h
*
*/



// [INCLUDE FILES]
#include <alf/alftextstylemanager.h>
#include <alf/alftextstyle.h>

#include "testplatalfvisual.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::TestStyleMgrTextStyleMethodsL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestStyleMgrTextStyleMethodsL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestStyleMgrTextStyleMethodsL, "TestStyleMgrTextStyleMethodsL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestStyleMgrTextStyleMethodsL );
    // Print to log file
    iLog->Log( KTestStyleMgrTextStyleMethodsL );
    
    CAlfTextStyleManager* styleMgr = &( iAlfEnv->TextStyleManager() );
    
    // create a new TextStyle A
    TInt styleAId = styleMgr->CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont, EAlfTextStyleNormal );
    
    // create a new style B having A as parent
    TInt styleBId = styleMgr->CreatePlatformTextStyleL( styleAId );
    
    // clone TextStyle C from B
    TInt styleCId = styleMgr->CopyTextStyleL( styleBId );
       
    styleMgr->TextStyle( styleAId );
    styleMgr->TextStyle( styleBId );
    styleMgr->TextStyle( styleCId );
    
    styleMgr->DeleteTextStyle( styleAId );
    styleMgr->DeleteTextStyle( styleBId );
    styleMgr->DeleteTextStyle( styleCId );
    
    return KErrNone;
    }


//  [End of File]
