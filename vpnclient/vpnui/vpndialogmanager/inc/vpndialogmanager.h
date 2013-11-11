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
* Description:   Declaration of the CVpnDialogManager class.
*
*/



#ifndef VPNDIALOGMANAGER_H
#define VPNDIALOGMANAGER_H

#include "vpnnotifierdefs.h"

class MVpnDialog;
class CVpnDialogShower;
class MVpnNotifierCallback;

class CVpnDialogManager : public CBase, public MVpnDialogManager
    {
public: // From CVpnDialogManagerBase
    virtual MVpnDialog* LaunchDialogL(MVpnNotifierCallback* aNotifier, TInt aDialogId,
                                TInt aNoteDialogId, const TDesC8& aInput);
    virtual TInt GetNoteTextId(TInt aNoteDialogId);
    virtual CVpnDialogShower* GetDialogShowerL();
    virtual MVpnDialog* ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aNoteDialogId);
    virtual ~CVpnDialogManager();
    
private:
    CVpnDialogShower* iDialogShower;
    };

#endif  // VPNDIALOGMANAGER_H
