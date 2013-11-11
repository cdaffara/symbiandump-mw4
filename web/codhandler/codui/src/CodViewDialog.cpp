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
* Description:  Implementation of class CCodViewDialog.   
*
*/


// INCLUDE FILES

#include "CodViewDialog.h"
#include "CodView.h"
#include "CodUi.hrh"
#include "CodUiPanic.h"
#include <aknappui.h>
#include <CodUi.rsg>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodViewDialog::CCodViewDialog
// ---------------------------------------------------------
//
CCodViewDialog::CCodViewDialog( const CCodData& aData, TBool aCod )
: CAknDialog(), iData( aData ), iCod( aCod )
    {
    }

// ---------------------------------------------------------
// CCodViewDialog::ExecuteLD
// ---------------------------------------------------------
//
TInt CCodViewDialog::ExecuteLD()
    {
    return CAknDialog::ExecuteLD( R_COD_UI_COD_VIEW_DIALOG );
    }

// ---------------------------------------------------------
// CCodViewDialog::CreateCustomControlL
// ---------------------------------------------------------
//
SEikControlInfo CCodViewDialog::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    switch ( aControlType )
        {
        case ECodUiCodView:
            {
            controlInfo.iControl = new (ELeave) CCodView();
            break;
            }

        default:
            {
            __ASSERT_DEBUG( EFalse, CodUiPanic( ECodUiBadControl ) );
            break;
            }
        }
    return controlInfo;
    }

// ---------------------------------------------------------
// CCodViewDialog::ProcessCommandL
// ---------------------------------------------------------
//
void CCodViewDialog::ProcessCommandL( TInt aCommandId )
    {
    switch( aCommandId )
        {
        case ECodUiCmdAccept:
        case ECodUiCmdReject:
            {
            TryExitL( aCommandId );
            break;
            }

        case EAknCmdExit:
        case EEikCmdExit:
            {
            ((CAknAppUi*)iEikonEnv->EikAppUi())->RunAppShutter();
            break;
            }

        default:
            {
            CAknDialog::ProcessCommandL( aCommandId );
            }
        }
    }

// ----------------------------------------------------------
// CCodViewDialog::OfferKeyEventL()
// ----------------------------------------------------------
//
TKeyResponse CCodViewDialog::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result = EKeyWasNotConsumed;
    if (
        ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter ) &&
        ( !(aKeyEvent.iModifiers & EModifierShift) )
       )
        {
        // Selection key press is same as Accept softkey.
        ProcessCommandL( ECodUiCmdAccept );
        result = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyPhoneEnd )
        {
        // Red key press -> Reject.
        // When incoming voice call ended the screen of download descriptor
        // information should not be closed.

        result = EKeyWasConsumed;
        }
    else
        {
        result = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    return result;
    }

// ---------------------------------------------------------
// CCodViewDialog::PostLayoutDynInitL
// ---------------------------------------------------------
//
void CCodViewDialog::PostLayoutDynInitL()
    {
    CAknDialog::PostLayoutDynInitL();
    }
    
// ---------------------------------------------------------
// CCodViewDialog::ActivateL
// ---------------------------------------------------------
//
void CCodViewDialog::ActivateL()
    {
    CAknDialog::ActivateL();
    STATIC_CAST( CCodView*, Control( ECodUiCtlCodViewRtxEd ) )->
        SetDataL( iData, iCod );
    }
