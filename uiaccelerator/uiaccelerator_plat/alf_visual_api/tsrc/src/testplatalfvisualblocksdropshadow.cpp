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
// CTestPlatAlfVisual::TestAlfDropShadowSetAttributesL
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::TestAlfDropShadowSetAttributesL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestPlatAlfVisual, "TestPlatAlfVisual" );
    _LIT( KTestAlfDropShadowSetAttributesL, "TestAlfDropShadowSetAttributesL" );
    TestModuleIf().Printf( 0, KTestPlatAlfVisual, KTestAlfDropShadowSetAttributesL );
    // Print to log file
    iLog->Log( KTestAlfDropShadowSetAttributesL );
    CAlfTextVisual* vVisual = CAlfTextVisual::AddNewL( *iAlfCtl );
    _LIT( KTest, "test" );
    TBuf<KBufSize> vBuf( KTest );
    vVisual->SetTextL( vBuf );
    vVisual->EnableDropShadowL();
    CAlfDropShadow* vDropShadow = vVisual->DropShadowHandler();
    TReal32 vReal = 0;
    TAlfMetric vMetric( 0 );
    TAlfTimedPoint vPoint;
    TRgb vRgb( KRgbBlack );
    TAknsItemID vId;
    TAlfTimedValue vValue( 0 );
    
    vDropShadow->SetOffset( vReal, vMetric );
    vDropShadow->SetOffset( vPoint );
    vDropShadow->SetColor( vRgb );
    vDropShadow->SetColor( vId, 0 );
    vDropShadow->SetOpacity( vValue);
    vDropShadow->SetRadius( vValue);
    vDropShadow->SetScale( vValue);
    
    return KErrNone;
    }

// End of file

