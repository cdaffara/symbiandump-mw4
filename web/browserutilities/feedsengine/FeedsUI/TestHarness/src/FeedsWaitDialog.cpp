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


#include <AknWaitDialog.h>
#include <eikprogi.h>
#include <eikdialg.h>
#include <StringLoader.h>

#include <FeedsApp.rsg>

#include "FeedsWaitDialog.h"


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsWaitDialog* CFeedsWaitDialog::NewL(MFeedsWaitDialogListener& aListener)
    {
    CFeedsWaitDialog* self = new (ELeave) CFeedsWaitDialog(aListener);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::CFeedsWaitDialog
//
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CFeedsWaitDialog::CFeedsWaitDialog(MFeedsWaitDialogListener& aListener):
        iListener(aListener)
    {
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::~CFeedsWaitDialog
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeedsWaitDialog::~CFeedsWaitDialog()
    {
    delete iWaitDialog;
    delete iProgressDialog;
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::DialogDismissedL
//
// Called when the user presses the cancel button.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::DialogDismissedL(TInt /*aButtonId*/)
    {
    iListener.DialogDismissedL();
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::ShowWaitDialogL
//
// Displays a WaitDialog.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::ShowWaitDialogL(TInt aLabelId)
    {
    iWaitDialog = new (ELeave) CAknWaitDialog(
            reinterpret_cast<CEikDialog**>(&iWaitDialog));
    iWaitDialog->PrepareLC(R_FEEDS_WAIT_NOTE_DIALOG);
    
    UpdateLabelL(aLabelId);
    iWaitDialog->SetCallback(this);
    iWaitDialog->RunLD();
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::ShowProgressDialogL
//
// Displays a ProgressDialog.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::ShowProgressDialogL(TInt aLabelId)
    {
    iProgressDialog = new (ELeave) CAknProgressDialog(
            reinterpret_cast<CEikDialog**>(&iProgressDialog));
    iProgressDialog->PrepareLC(R_FEEDS_PROGRESS_NOTE_DIALOG);
    
    UpdateLabelL(aLabelId);
    iProgressDialog->SetCallback(this);
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressDialog->RunLD();
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::UpdateLabelL
//
// Updates the dialog's label.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::UpdateLabelL(TInt aLabelId)
    {
    HBufC*  label = NULL;

    // Load the label's resource.
    label = StringLoader::LoadLC(aLabelId);

    // Set the label.
    if (iWaitDialog != NULL)
        {
        iWaitDialog->SetTextL(*label);
        }
    else if (iProgressDialog != NULL)
        {
        iProgressDialog->SetTextL(*label);
        }
        
    CleanupStack::PopAndDestroy(label);
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::SetMaxProgressL
//
// For progress dialogs this method set the max progress value.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::SetMaxProgressL(TInt aMaxValue)
    {
    if (iProgressDialog != NULL)
        {
        iProgressInfo->SetFinalValue(aMaxValue);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::UpdateProgressL
//
// For progress dialogs this method updates the progress.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::UpdateProgressL(TInt aIncrement)
    {
    if (iProgressDialog != NULL)
        {
        iProgressInfo->IncrementAndDraw(aIncrement);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsWaitDialog::Close
//
// Closes the dialog.
// -----------------------------------------------------------------------------
//
void CFeedsWaitDialog::Close()
    {
    if (iWaitDialog != NULL)
        {
        //iWaitDialog->ProcessFinishedL();
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
    else if (iProgressDialog != NULL)
        {
        //iProgressDialog->ProcessFinishedL();
        delete iProgressDialog;
        iProgressDialog = NULL;
        }
    }
