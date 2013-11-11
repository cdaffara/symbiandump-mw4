/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Enables the destruction of the Select Element Popup Dialog
*               asynchronously
*
*
*/

#include "BrowserDialogsProviderAsyncExit.h"

// ---------------------------------------------------------------------------
// staticCBrowserDialogsProviderAsyncExit* 
//                CBrowserDialogsProviderAsyncExit::NewL( 
//                                          CBrowserSelectElementDlg& aDlg )
// Create a CBrowserDialogsProviderAsyncExit object. Leaves on failure.
// ---------------------------------------------------------------------------
//
CBrowserDialogsProviderAsyncExit* 
                CBrowserDialogsProviderAsyncExit::NewL( CBrowserSelectElementDlg& aDlg )
    {
    CBrowserDialogsProviderAsyncExit* self = NewLC( aDlg );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
//	static CBrowserDialogsProviderAsyncExit* 
//                                  CBrowserDialogsProviderAsyncExit::NewLC( 
//                                          CBrowserSelectElementDlg& aPopup)
//  Create a CBrowserDialogsProviderAsyncExit object. Leaves on failure.
// ----------------------------------------------------------------------------
//    
CBrowserDialogsProviderAsyncExit* CBrowserDialogsProviderAsyncExit::NewLC(
                                            CBrowserSelectElementDlg& aDlg )
    {
    CBrowserDialogsProviderAsyncExit* self = 
                    new (ELeave) CBrowserDialogsProviderAsyncExit( aDlg );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
//	CBrowserDialogsProviderAsyncExit::CBrowserDialogsProviderAsyncExit( 
//                                          CBrowserSelectElementDlg& aDlg )
//  Constructs this object
// ----------------------------------------------------------------------------
//    
CBrowserDialogsProviderAsyncExit::CBrowserDialogsProviderAsyncExit( 
                                            CBrowserSelectElementDlg& aDlg ) 
        : CActive( CActive::EPriorityStandard ),
        iDlg ( aDlg )

    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//	CBrowserDialogsProviderAsyncExit::~CBrowserDialogsProviderAsyncExit()
//  Destroy the object and release all memory objects
// ---------------------------------------------------------------------------
//    
CBrowserDialogsProviderAsyncExit::~CBrowserDialogsProviderAsyncExit()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
//	void CBrowserDialogsProviderAsyncExit::ConstructL()
//  Two-pase constructor
// ---------------------------------------------------------------------------
//    
void CBrowserDialogsProviderAsyncExit::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//	void CBrowserDialogsProviderAsyncExit::Start()
//  Complete an asynchronous request.
// ---------------------------------------------------------------------------
//    
void CBrowserDialogsProviderAsyncExit::Start()
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
//	void CBrowserDialogsProviderAsyncExit::DoCancel()
//  Cancel any outstanding requests.
// ---------------------------------------------------------------------------
//    
void CBrowserDialogsProviderAsyncExit::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
//	void CBrowserDialogsProviderAsyncExit::RunL()
//  Handles object`s request completion event.
// ---------------------------------------------------------------------------
//    
void CBrowserDialogsProviderAsyncExit::RunL()
    {
    iDlg.DestroyPopupL(); // Exit the popup
    }

// End of file
