/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of class CTimeout.   
*      
*
*/


// INCLUDE FILES

#include "Timeout.h"
#include "CodLogger.h"
#include "CodPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTimeout::NewL()
// ---------------------------------------------------------
//
CTimeout* CTimeout::NewL( TInt aPriority, TCallBack aCallBack )
    {
    CTimeout* timeout = new (ELeave) CTimeout( aPriority, aCallBack );
    CleanupStack::PushL( timeout );
    timeout->ConstructL();
    CleanupStack::Pop( timeout );
    return timeout;
    }

// ---------------------------------------------------------
// CTimeout::~CTimeout()
// ---------------------------------------------------------
//
CTimeout::~CTimeout()
    {
    CLOG(( ECodEng, 2, _L("CTimeout::~CTimeout") ));
    Cancel();
    }

// ---------------------------------------------------------
// CTimeout::RunL()
// ---------------------------------------------------------
//
void CTimeout::RunL()
    {
    CLOG(( ECodEng, 2, _L("CTimeout::RunL iStatus(%d)"), iStatus.Int() ));
#ifdef _DEBUG
    TBool again =
#endif /* def _DEBUG */
    iCallBack.CallBack();
    // Assert that this is a "once-only" callback.
    __ASSERT_DEBUG( !again, CodPanic( ECodInternal ) );
    }

// ---------------------------------------------------------
// CTimeout::CTimeout()
// ---------------------------------------------------------
//
CTimeout::CTimeout( TInt aPriority, TCallBack aCallBack )
: CTimer( aPriority ), iCallBack( aCallBack )
    {
    CLOG(( ECodEng, 2, _L("CTimeout::CTimeout") ));
    CActiveScheduler::Add( this );
    }

