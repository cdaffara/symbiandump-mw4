/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*	  Definition of class TDialerDialogs.
*
*
*/

 
// INCLUDE FILES

#include <eikenv.h>
#include <eikon.rsg>
#include <aknnotewrappers.h>

#include <BrowserTelService.rsg>
#include "BrowserTelServiceDlgs.h"


// ================= MEMBER FUNCTIONS =======================

//---------------------------------------------------------------------------------------
// TDialerDialogs::ErrorNoteL()
//---------------------------------------------------------------------------------------
//
void TDialerDialogs::ErrorNoteL( const TDesC& aPrompt )
	{
	CAknErrorNote* note = new( ELeave ) CAknErrorNote();
	note->ExecuteLD( aPrompt );	
	}

//---------------------------------------------------------------------------------------
// TDialerDialogs::ConfirmationDlgL()
//---------------------------------------------------------------------------------------
//
TInt TDialerDialogs::ConfirmationDlgL( const TDesC& aMsg, TInt aResourceId )
	{
	TPtrC msg( aMsg );
	CAknQueryDialog* dialog = new( ELeave ) CAknQueryDialog( msg );
	return dialog->ExecuteLD( aResourceId );
	}

//---------------------------------------------------------------------------------------
// TDialerDialogs::ConfirmationDlgL()
//---------------------------------------------------------------------------------------
//
TInt TDialerDialogs::WaitDialogL( const TDesC& aMsg, TInt aResourceId )
	{
	TPtrC msg(aMsg);
	CAknWaitDialog *dialog = NULL;
	dialog = new ( ELeave ) CAknWaitDialog( 
		reinterpret_cast<CEikDialog**>( dialog ) );
	dialog->PrepareLC( aResourceId );
	dialog->SetTextL( msg );
	return dialog->RunLD();	
	}
