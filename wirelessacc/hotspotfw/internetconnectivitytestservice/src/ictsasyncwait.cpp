/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CIctsActiveWait
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "ictsasyncwait.h"

// ---------------------------------------------------------
// CIctsAsyncWait::CIctsAsyncWait()
// ---------------------------------------------------------
//
CIctsAsyncWait::CIctsAsyncWait() : CActive(CActive::EPriorityUserInput)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CIctsAsyncWait::~CIctsAsyncWait()
// ---------------------------------------------------------
//
CIctsAsyncWait::~CIctsAsyncWait() 
    {
    Cancel() ;
    }

// ---------------------------------------------------------
// CIctsAsyncWait::Wait() 
// ---------------------------------------------------------
//
void CIctsAsyncWait::Wait() 
{
    SetActive();
    iActiveWait.Start();
}

// ---------------------------------------------------------
// CIctsAsyncWait::RunL()
// ---------------------------------------------------------
//
void CIctsAsyncWait::RunL()
{
    iActiveWait.AsyncStop() ;
}

// ---------------------------------------------------------
// CIctsAsyncWait::DoCancel()
// ---------------------------------------------------------
//
void CIctsAsyncWait::DoCancel()
{
    if( iStatus == KRequestPending )
        {
        TRequestStatus * reqStat = &iStatus;
        User::RequestComplete(reqStat, KErrCancel);
        }
}

// End of File
