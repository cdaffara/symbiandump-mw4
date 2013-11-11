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
* Description:  Declaration of class CCodWaitDialog.   
*
*/


#ifndef COD_WAIT_DIALOG_H
#define COD_WAIT_DIALOG_H

// INCLUDES

#include <AknWaitDialog.h>

// CLASS DECLARATION

/**
* Mixin protocol for handling commands for wait dialog.
*/
class MCodDialogCommandObserver
    {
    public:     // new methods

        /**
        * Handle dialog command.
        * @param aCommand Dialog command id.
        */
        virtual void HandleDialogCommandL( TInt aCommand ) = 0;

    };

/**
* A dialog which forwards dialog commands to its observer.
*/
NONSHARABLE_CLASS( CCodWaitDialog ): public CAknWaitDialog
    {
    public:     // construction

        /**
        * Constructor.
        * @param aSelfPtr Self pointer.
        * @param aObserver Commands arriving to the dialog are forwarded
        * to this observer.
        */
        CCodWaitDialog
            ( CEikDialog** aSelfPtr, MCodDialogCommandObserver& aObserver );

    public:     // new methods

        /**
        * Set text from resource.
        * @param aResourceId Resource id (RESOURCE TBUF).
        */
        void SetTextL( TInt aResourceId );

    protected:  // from CAknNoteDialog (CEikDialog)

        /**
        * Handle key event.
        * @param aKeyEvent Key event.
        * @param aType Event type.
        * @return Key response.
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Handle button press (forward to observer).
        * @param Id of button pressed.
        * @return Always EFalse - delete the dialog to dismiss it.
        */
        TBool OkToExitL( TInt aButtonId );

    private:    // data

        MCodDialogCommandObserver* iObserver;   ///< Command observer.

    };

#endif /* def COD_WAIT_DIALOG_H */
