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
* Description: 
*      Password dialog
*
*
*/


// INCLUDE FILES

#include <eikseced.h>
#include <avkon.hrh>

#include "BrowserAuthenticationDialog.h"
#include "BrowserDialogsProvider.hrh"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrowserAuthenticationDialog::NewL
// 2 phase constructor
// ---------------------------------------------------------
//
CBrowserAuthenticationDialog*
    CBrowserAuthenticationDialog::NewL( TDes& aUsername, 
                                        TDes& aPassword )
    {
    CBrowserAuthenticationDialog* dlg = 
                                new (ELeave) CBrowserAuthenticationDialog();
    CleanupStack::PushL( dlg );

    dlg->ConstructL( &aUsername, &aPassword );
    dlg->SetDataL( aUsername, aPassword );

    CleanupStack::Pop( dlg );
    return dlg;
    }

// ---------------------------------------------------------
// CBrowserAuthenticationDialog::CBrowserAuthenticationDialog
//
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CBrowserAuthenticationDialog::CBrowserAuthenticationDialog()
    : CAknMultiLineDataQueryDialog( CAknQueryDialog::ENoTone )
    , iEmptyName( ETrue )
    , iEmptyPassword( ETrue )
    , iUsername( NULL )
    , iPassword( NULL )
    {
    }

void CBrowserAuthenticationDialog::ConstructL( TDes* aUsername, 
                                               TDes* aPassword )
    {
    iUsername = aUsername;
    iPassword = aPassword;
    }

// ---------------------------------------------------------
// CBrowserAuthenticationDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void  CBrowserAuthenticationDialog::PreLayoutDynInitL()
    {
    
    CAknMultiLineDataQueryDialog::PreLayoutDynInitL();

    SetEditableL( ETrue );

    FirstControl()->SetQueryControlObserver( this );
    SecondControl()->SetQueryControlObserver( this );
    
    iEmptyName = ( !FirstControl()->GetTextLength() );
    // password is always empty. Confirmation softkey depends on name.
    MakeLeftSoftkeyVisible( !iEmptyName );
    } 

// ---------------------------------------------------------
// CBrowserAuthenticationDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CBrowserAuthenticationDialog::OkToExitL( TInt aKeycode )
    {
    TBool ret = CAknMultiLineDataQueryDialog::OkToExitL( aKeycode );    
    if ( aKeycode == EAknSoftkeyOk )
        {
        FirstControl()->GetText( *iUsername );
        SecondControl()->GetText( *iPassword );
        }
    return ret;
    }

// ---------------------------------------------------------
// CBrowserAuthenticationDialog::SetInitialCurrentLineL
// ---------------------------------------------------------
//
void CBrowserAuthenticationDialog::SetInitialCurrentLineL()
    {   
    CAknMultiLineDataQueryDialog::SetInitialCurrentLine();

    if( FirstControl()->GetTextLength() > 0 )
        {
        TryChangeFocusToL( EMultilineSecondLine );
        }
    }

// ---------------------------------------------------------
// CBrowserAuthenticationDialog::HandleQueryEditorStateEventL
//
// This code is base on AknQueryDialog.cpp
// ---------------------------------------------------------
//
TBool CBrowserAuthenticationDialog::HandleQueryEditorStateEventL(
                CAknQueryControl* aQueryControl, 
                TQueryControlEvent /*aEventType*/, 
                TQueryValidationStatus aStatus )
    {
    if( aQueryControl == FirstControl() )
        {
        iEmptyName = aStatus == EEditorEmpty;
        }
    else
        {
        iEmptyPassword = aStatus == EEditorEmpty;
        }

    MakeLeftSoftkeyVisible( !iEmptyName  || !iEmptyPassword );

    return EFalse;
    }

//  End of File
