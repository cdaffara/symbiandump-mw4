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
* Description:  Implementation of class CCodWaitDialog.   
*
*/


// INCLUDE FILES

#include "CodWaitDialog.h"
#include "CodUi.hrh"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodWaitDialog::CCodWaitDialog()
// ---------------------------------------------------------
//
CCodWaitDialog::CCodWaitDialog
( CEikDialog** aSelfPtr, MCodDialogCommandObserver& aObserver )
: CAknWaitDialog( aSelfPtr, /*aVisibilityDelayOff=*/ ETrue )
    {
    iObserver = &aObserver;
    }

// ---------------------------------------------------------
// CCodWaitDialog::SetTextL()
// ---------------------------------------------------------
//
void CCodWaitDialog::SetTextL( TInt aResourceId )
    {
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( aResourceId );
    CAknWaitDialog::SetTextL( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// ---------------------------------------------------------
// CCodWaitDialog::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CCodWaitDialog::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    if ( aKeyEvent.iCode == EKeyPhoneEnd )
        {
        // Red key press -> Cancel.
        TryExitL( ECodUiCmdCancel );
        response = EKeyWasConsumed;
        }
    else
        {
        response = CAknWaitDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------
// CCodWaitDialog::OkToExitL()
// ---------------------------------------------------------
//
TBool CCodWaitDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
        // CAknShutter simulates ESC key events; this causes Cancel command.
        return ETrue;
        }
    else
        {
        iObserver->HandleDialogCommandL( aButtonId );
        return EFalse;
        }
    }
