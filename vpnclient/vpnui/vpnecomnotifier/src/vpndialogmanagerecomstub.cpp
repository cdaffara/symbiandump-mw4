/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   VPN Ecom Notifier's dialog manager stub implementation file
*
*/




#include "vpndialogmanagerecomstub.h"
#include "vpnnotifierdefs.h"

#include <vpnecomnotifier.rsg>

#include "logvpncommon.h"

MVpnDialog* CVpnDialogManagerEcomStub::LaunchDialogL(MVpnNotifierCallback* aNotifier, TInt aDialogId,
                                               TInt aNoteDialogId, const TDesC8& aInput)
    {
    LOG_1("-> CVpnDialogManagerEcomStub::LaunchDialogL() dialog ID is: %d", aDialogId);

    ASSERT(iDialogManager);
    LOG_(" Launching the real McDialogManager");
    MVpnDialog* dialog = iDialogManager->LaunchDialogL(aNotifier, aDialogId, aNoteDialogId, aInput);

    LOG_("<- CVpnDialogManagerEcomStub::LaunchDialogL()");
    return dialog;
    }

void CVpnDialogManagerEcomStub::SetDialogManager(MVpnDialogManager* aDialogManager) 
    {
    LOG_("-> CVpnDialogManagerEcomStub::SetDialogManager()");
    iDialogManager = aDialogManager;
    LOG_("<- CVpnDialogManagerEcomStub::SetDialogManager()");
    }

TInt CVpnDialogManagerEcomStub::GetNoteTextId(TInt aNoteDialogId)
    {
    LOG_1("-> CVpnDialogManagerEcomStub::GetNoteTextId(), note dialog id: %d", aNoteDialogId);
    
    ASSERT(iDialogManager);
    TInt ret = iDialogManager->GetNoteTextId(aNoteDialogId);
    
    LOG_1("<- CVpnDialogManagerEcomStub::GetNoteTextId() return: %d", ret);
    return ret;
    }

MVpnDialog* CVpnDialogManagerEcomStub::ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aNoteDialogId)
    {
    LOG_("-> CAcuEcomDialogManager::ShowNoteDialogL()");

    ASSERT(iDialogManager);
    LOG_(" Launching the real McDialogManager");

    MVpnDialog* ret = iDialogManager->ShowNoteDialogL(aNotifier, aNoteDialogId);

    LOG_("<- CAcuEcomDialogManager::ShowNoteDialogL()");

    return ret;
    }

CVpnDialogManagerEcomStub::~CVpnDialogManagerEcomStub()
    {
    LOG_("-> CAcuEcomDialogManager::~CVpnDialogManagerEcomStub()");
    LOG_("<- CAcuEcomDialogManager::~CVpnDialogManagerEcomStub()");
    }

