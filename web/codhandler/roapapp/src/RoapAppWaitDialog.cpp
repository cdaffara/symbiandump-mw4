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
*      Implementation of class CRoapAppWaitDialog.   
*      
*
*/


// INCLUDE FILES

#include "RoapAppWaitDialog.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapAppWaitDialog::CRoapAppWaitDialog()
// ---------------------------------------------------------
//
CRoapAppWaitDialog::CRoapAppWaitDialog
( CEikDialog** aSelfPtr, MRoapDialogCommandObserver& aObserver )
: CAknWaitDialog( aSelfPtr, /*aVisibilityDelayOff=*/ ETrue )
    {
    iObserver = &aObserver;
    }

// ---------------------------------------------------------
// CRoapAppWaitDialog::OkToExitL()
// ---------------------------------------------------------
//
TBool CRoapAppWaitDialog::OkToExitL( TInt aButtonId )
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
