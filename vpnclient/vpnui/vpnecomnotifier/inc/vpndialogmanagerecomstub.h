/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN Ecom Notifier's dialog manager stub definition file
*
*/




#ifndef __VPNDIALOGMANAGERECOMSTUB_H__
#define __VPNDIALOGMANAGERECOMSTUB_H__

#include <e32base.h>

class CVpnNotifier;
class MVpnDialog;
class MVpnDialogManager;
class MVpnNotifierCallback;

class CVpnDialogManagerEcomStub : public CBase
    {
public: // From CVpnDialogManagerBase
    MVpnDialog* LaunchDialogL(MVpnNotifierCallback* aNotifier, TInt aDialogId,
                                TInt aNoteDialogId, const TDesC8& aInput);
    TInt GetNoteTextId(TInt aNoteDialogId);
    MVpnDialog* ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aNoteDialogId);
    ~CVpnDialogManagerEcomStub();
    void SetDialogManager(MVpnDialogManager* aDialogManager);
    
private:
    MVpnDialogManager*  iDialogManager; // ref, not owned by this component
    };

#endif  // __ACUDIALOGMANAGER_H__
