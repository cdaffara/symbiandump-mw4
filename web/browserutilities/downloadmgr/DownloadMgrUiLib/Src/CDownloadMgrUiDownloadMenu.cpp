/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Supports Download Menu
*
*/



// INCLUDE FILES
#include    "CDownloadMgrUiDownloadMenu.h"
#include    "UiLibLogger.h"
#include    <e32std.h>
#include    <e32def.h>
#include    <DownloadMgrUiLib.rsg>
#include    <coemain.h>
#include    <eikmenup.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadMenu::CDownloadMgrUiDownloadMenu
// -----------------------------------------------------------------------------
//
CDownloadMgrUiDownloadMenu::CDownloadMgrUiDownloadMenu( CCoeEnv& aCoeEnv )
:   iCoeEnv( aCoeEnv )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadMenu::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadMenu::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadMenu::NewL
// -----------------------------------------------------------------------------
//
CDownloadMgrUiDownloadMenu* CDownloadMgrUiDownloadMenu::NewL( CCoeEnv& aCoeEnv )
    {
    CDownloadMgrUiDownloadMenu* self = 
        new ( ELeave ) CDownloadMgrUiDownloadMenu( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadMgrUiDownloadMenu::~CDownloadMgrUiDownloadMenu()
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadMenu::~CDownloadMgrUiDownloadMenu");
    //delete iExtension;
    iExtension = 0;
    CLOG_WRITE(" iExtension");
    CLOG_LEAVEFN("CDownloadMgrUiDownloadMenu::~CDownloadMgrUiDownloadMenu");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadMenu::AddMenuL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiDownloadMenu::AddMenuL( TInt aCommandId, 
                                           CEikMenuPane& aMenuPane, 
                                           TInt aPreviousId )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadMenu::AddMenuL");

    CEikMenuPaneItem::SData menuItemData;
    menuItemData.iCommandId = aCommandId;
    menuItemData.iFlags = 0;
    menuItemData.iCascadeId = 0;
    HBufC* menuText = iCoeEnv.AllocReadResourceLC( R_DMUL_DOWNLOADMENU_ITEM_TEXT );
    // Max length of iText is ENominalTextLength!
    menuItemData.iText = menuText->Left( 
        menuText->Length()>CEikMenuPaneItem::SData::ENominalTextLength ? 
        CEikMenuPaneItem::SData::ENominalTextLength : menuText->Length() );
    CleanupStack::PopAndDestroy( menuText ); // menuText
    if ( aPreviousId == KErrNotFound )
        {
        aMenuPane.AddMenuItemL( menuItemData );
        }
    else
        {
        aMenuPane.AddMenuItemL( menuItemData, aPreviousId );
        }

    CLOG_LEAVEFN("CDownloadMgrUiDownloadMenu::AddMenuL");
    }

// End of file.
