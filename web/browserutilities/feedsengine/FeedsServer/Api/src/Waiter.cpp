/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Waits for request to complete
*
*/


#include "Waiter.h"

// -----------------------------------------------------------------------------
// CWaiter::NewL
//
// Two-phased constructor. 
// -----------------------------------------------------------------------------
//
CWaiter* CWaiter::NewL()
    {
    CWaiter* self = new (ELeave) CWaiter();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
// -----------------------------------------------------------------------------
// CWaiter::CWaiter
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWaiter::CWaiter()
        :CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CWaiter::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWaiter::ConstructL()
    {    
    }
    
// -----------------------------------------------------------------------------
// CWaiter::~CWaiter
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CWaiter::~CWaiter()
    {
    Cancel();
    }
    
// -----------------------------------------------------------------------------
// CWaiter::Wait
//
// Waits till request gets completed
// -----------------------------------------------------------------------------
//
TInt CWaiter::Wait()
    {
    iStatus = KRequestPending;
    iIsWaiting = ETrue;
    SetActive();
    iSyncWait.Start();
    return iStatus.Int();
    }
    
// -----------------------------------------------------------------------------
// CWaiter::RunL
//
// RunL
// -----------------------------------------------------------------------------
//
void CWaiter::RunL()
    {
    if( iSyncWait.IsStarted() )
        {
        iSyncWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// CWaiter::DoCancel
//
// DoCancel
// -----------------------------------------------------------------------------
//
void CWaiter::DoCancel()
    {
    RequestComplete(KErrCancel);
  	}

// -----------------------------------------------------------------------------
// CWaiter::RequestComplete
//
// RequestComplete
// -----------------------------------------------------------------------------
//
TBool CWaiter::RequestComplete(TInt aStatus)
{
	if(iIsWaiting)
	{
		TRequestStatus  *status = &iStatus;
		User::RequestComplete(status, aStatus);
		iIsWaiting = EFalse;
		return ETrue;
	}
	return EFalse;
}
