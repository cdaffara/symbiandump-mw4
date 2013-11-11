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
*      Implementation of class CRequestCompleteCallback.   
*      
*
*/


// INCLUDE FILES

#include "RequestCompleteCallback.h"
#include "RoapAppPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRequestCompleteCallback::CRequestCompleteCallback()
// ---------------------------------------------------------
//
CRequestCompleteCallback::CRequestCompleteCallback()
: CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CRequestCompleteCallback::~CRequestCompleteCallback()
// ---------------------------------------------------------
//
CRequestCompleteCallback::~CRequestCompleteCallback()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CRequestCompleteCallback::CallbackOnCompletion()
// ---------------------------------------------------------
//
void CRequestCompleteCallback::CallbackOnCompletion
( const TCallBack& aCallback )
    {
    __ASSERT_DEBUG( !IsActive(), RoapAppPanic( ERoapAppRequestPending ) );
    iCallback = aCallback;
    SetActive();
    }

// ---------------------------------------------------------
// CRequestCompleteCallback::DoCancel()
// ---------------------------------------------------------
//
void CRequestCompleteCallback::DoCancel()
    {
    // The request should have been completed/Cancelled already, this object
    // cannot cancel it. If still pending, panic now (rather than hang).
    __ASSERT_ALWAYS( iStatus != KRequestPending, \
        RoapAppPanic( ERoapAppRequestPending ) );
    }

// ---------------------------------------------------------
// CRequestCompleteCallback::RunL()
// ---------------------------------------------------------
//
void CRequestCompleteCallback::RunL()
    {
#ifdef _DEBUG
    // Debug-only variable.
    TBool ret =
#endif
    iCallback.CallBack();
    // This object cannot renew the request itself. Therefore, callbacks
    // must return EFalse.
    __ASSERT_DEBUG( !ret, RoapAppPanic( ERoapAppInvalidCallback ) );
    }
