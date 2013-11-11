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
* Description: 
*
*/


#include "BackupObserver.h"

#include <connect/sbdefs.h>

#include "FavouritesUtil.h"
#include "FavouritesLogger.h"

CBackupObserver* CBackupObserver::NewL()
    {
    CBackupObserver* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

    
CBackupObserver* CBackupObserver::NewLC()
    {
    CBackupObserver* self = new (ELeave) CBackupObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CBackupObserver::CBackupObserver()
: CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }


CBackupObserver::~CBackupObserver()
    {
    Cancel();
    }


void CBackupObserver::ConstructL()
    {
    User::LeaveIfError(
        iBackupStatus.Attach(KUidSystemCategory, 
                             conn::KUidBackupRestoreKey));
    iStatus = KRequestPending;
    iBackupStatus.Subscribe(iStatus);
    SetActive();
    }


void CBackupObserver::DoCancel()
    {
    iBackupStatus.Cancel();
    }


void CBackupObserver::RunL()
    {
    User::LeaveIfError(iStatus.Int());

    TBool backupInProgress = FavouritesUtil::IsBackupInProgressL();

    if(backupInProgress)
        {
        FLOG(( _L("Backup is in progress, stopping server") ));
        CActiveScheduler::Stop();
        }
    }

