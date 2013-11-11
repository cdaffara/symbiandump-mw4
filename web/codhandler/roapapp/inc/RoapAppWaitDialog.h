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
*      Declaration of class CRoapAppWaitDialog.   
*      
*
*/


#ifndef DD_VIEWER_WAIT_DIALOG_H
#define DD_VIEWER_WAIT_DIALOG_H

// INCLUDES

#include <AknWaitDialog.h>

// CLASS DECLARATION

/**
* Mixin protocol for handling commands for wait dialog.
*/
class MRoapDialogCommandObserver
    {
    public:     // new methods

        /**
        * Handle dialog command.
        * @param aCommand Dialog command id.
        */
        virtual void HandleDialogCommandL( TInt aCommand ) = 0;

    };

/**
* DD Viewer application's wait note. This dialog is passive; commands
* arriving here are forwarded to its observer.
*/
class CRoapAppWaitDialog: public CAknWaitDialog
    {
    public:     // construction

        /**
        * Constructor.
        * @param aSelfPtr Self pointer.
        * @param aObserver Commands arriving to the dialog are forwarded
        * to this observer.
        */
        CRoapAppWaitDialog
            ( CEikDialog** aSelfPtr, MRoapDialogCommandObserver& aObserver );

    protected:  // from CAknNoteDialog (CEikDialog)

        /**
        * Handle button press (forward to observer).
        * @param Id of button pressed.
        * @return Always EFalse - delete the dialog to dismiss it.
        */
        TBool OkToExitL( TInt aButtonId );

    private:    // data

        MRoapDialogCommandObserver* iObserver;  ///< Command observer.

    };

#endif /* def DD_VIEWER_WAIT_DIALOG_H */
