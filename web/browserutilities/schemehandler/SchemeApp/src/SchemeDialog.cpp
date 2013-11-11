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
*      Implementation of class CSchemeDialog.   
*      
*
*/


// INCLUDE FILES

#include <avkon.hrh>
#include <aknappui.h>
#include <aknlists.h>
#include <eiklbo.h>
#include <AknForm.h>
#include "SchemeDialog.h"
#include "Scheme.hrh"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSchemeDialog::~CSchemeDialog()
// ---------------------------------------------------------
//
CSchemeDialog::~CSchemeDialog()
    {
    }

// ---------------------------------------------------------
// CSchemeDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CSchemeDialog::PreLayoutDynInitL()
    {
    }

// ---------------------------------------------------------
// CSchemeDialog::OkToExitL()
// ---------------------------------------------------------
//
TBool CSchemeDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOptions )
        {
        iAvkonAppUi->ProcessCommandL( EAknSoftkeyOptions );
        }
    else if ( aButtonId == EAknSoftkeyBack )
        {
        iAvkonAppUi->ProcessCommandL( EEikCmdExit );
        }

    return EFalse;
    }
