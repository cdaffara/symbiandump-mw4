/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates a wait or progress dialog.
*
*/

#ifndef FEEDS_WAIT_DIALOG_H
#define FEEDS_WAIT_DIALOG_H


// INCLUDES
#include <aknwaitnotewrapper.h> 
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Observer interface for the WaitDialog
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class MFeedsWaitDialogListener
    {
    public:
        /**
        * Called when the user presses the cancel button.
        *
        * @since 3.0
        * @return void.
        */
        virtual void DialogDismissedL() = 0;        
    };


/**
*  Encapsulates a wait or progress dialog.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFeedsWaitDialog: public CBase, public MProgressDialogCallback
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CFeedsWaitDialog* NewL(MFeedsWaitDialogListener& aListener);
        
        /**
        * Destructor.
        */        
        virtual ~CFeedsWaitDialog();


	public: // From MProgressDialogCallback
        /**
        * Called when the user presses the cancel button.
        *
        * @since ?
        * @param aButtonId The id of the pressed button.
        * @return void.
        */
        virtual void DialogDismissedL(TInt aButtonId);


    public: // New methods
        /**
        * Displays a WaitDialog.
        *
        * @since 3.0
        * @param aLabelId The label's resource id.
        * @return void.
        */
        void ShowWaitDialogL(TInt aLabelId);

        /**
        * Displays a ProgressDialog.
        *
        * @since 3.0
        * @param aLabelId The label's resource id.
        * @return void.
        */
        void ShowProgressDialogL(TInt aLabelId);

        /**
        * Updates the dialog's label.
        *
        * @since 3.0
        * @param aLabelId The label's resource id.
        * @return void.
        */
        void UpdateLabelL(TInt aLabelId);

        /**
        * For progress dialogs this method set the max progress value.
        *
        * @since 3.0
        * @param aMaxValue The progress dialog's max value.
        * @return void.
        */
        void SetMaxProgressL(TInt aMaxValue);

        /**
        * For progress dialogs this method updates the progress.
        *
        * @since 3.0
        * @param aIncrement The amount the progress changed.
        * @return void.
        */
        void UpdateProgressL(TInt aIncrement);

        /**
        * Closes the dialog.
        *
        * @since 3.0
        * @return void.
        */
        void Close();


    private:
        /**
        * C++ default constructor.
        */
        CFeedsWaitDialog(MFeedsWaitDialogListener& aListener);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        
    private:
        MFeedsWaitDialogListener&  iListener;

	    CAknWaitDialog*       iWaitDialog;
	    CAknProgressDialog*   iProgressDialog;
	    CEikProgressInfo*     iProgressInfo;
    };

#endif      // FEEDS_WAIT_DIALOG_H
            
// End of File

