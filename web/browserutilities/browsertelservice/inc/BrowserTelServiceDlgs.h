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
*	  Declaration of class TDialerDialogs.
*
*
*/


// INCLUDE FILES

#ifndef __BROWSERTELSERVICE_DLGS_H
#define __BROWSERTELSERVICE_DLGS_H

#include <eikdialg.h>
#include <eikedwin.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>


// CLASS DECLARATION

/**
*  Implements the dialogs needed by browsertelservice
*/
class TDialerDialogs
	{
	public:
		/**
		* Displayes an error note
        * @param aPrompt The desired text to be shown
		*/
		static void ErrorNoteL( const TDesC& aPrompt );
		
		/**
		* Displayes a confirmation dialog
        * @param aMsg The desired text to be shown
		* @param aResourceId Resource for the dialog
        * @return Non zero if accepted
		*/
		static TInt ConfirmationDlgL( const TDesC& aMsg, TInt aResourceId );

		/**
		* Displayes a wait dialog
        * @param aMsg The desired text to be shown
		* @param aResourceId Resource for the dialog
        * @return Non zero if accepted
		*/
		static TInt WaitDialogL( const TDesC& aMsg, TInt aResourceId );
	};


#endif

