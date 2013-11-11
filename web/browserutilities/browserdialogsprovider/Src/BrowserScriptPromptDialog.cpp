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
*
*
*/


// INCLUDE FILES

#include <avkon.hrh>

#include "BrowserScriptPromptDialog.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CBrowserScriptPromptDialog::CBrowserScriptPromptDialog
//
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CBrowserScriptPromptDialog::CBrowserScriptPromptDialog( TDes& aDefInput,
                                                        HBufC*& aRetBuf )
    : CAknTextQueryDialog( aDefInput )
    , iRetBuf( aRetBuf )
    {
    }

// ---------------------------------------------------------
// CBrowserScriptPromptDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void  CBrowserScriptPromptDialog::PreLayoutDynInitL()
    {
    CAknTextQueryDialog::PreLayoutDynInitL();

    MakeLeftSoftkeyVisible( ETrue );
    } 

    // ---------------------------------------------------------
// CBrowserScriptPromptDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CBrowserScriptPromptDialog::OkToExitL( TInt aKeycode )
    {
    TBool ret( EFalse );

    if ( aKeycode == EAknSoftkeyOk )
        {
        CAknQueryControl* control = QueryControl();

        __ASSERT_DEBUG( control, User::Panic( KNullDesC, KErrGeneral ) );
        
        iRetBuf = HBufC::NewL( control->GetTextLength() );
        TPtr temp( iRetBuf->Des() );
        control->GetText( temp );
            
        ret = ETrue;
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CBrowserScriptPromptDialog::HandleQueryEditorStateEventL
//
// This code is base on AknQueryDialog.cpp
// ----------------------------------------------------------------------------
//
TBool CBrowserScriptPromptDialog::HandleQueryEditorStateEventL(
                                        CAknQueryControl* /*aQueryControl*/,
                                        TQueryControlEvent /*aEventType*/,
                                        TQueryValidationStatus /*aStatus*/ )
    {
    return EFalse;
    }

//  End of File
