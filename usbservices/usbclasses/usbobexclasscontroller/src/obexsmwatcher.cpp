/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors Obex ServiceMan errors.
*
*/

#include "obexsmwatcher.h"
#include <e32base.h>
#include <obexservicemanprop.h>
#include "debug.h"


/**
 * @since S60 V5.2
 */
CObexSMWatcher* CObexSMWatcher::NewL(MObexSMObserver& aObserver)
    {
    LOG_FUNC

    CObexSMWatcher* self = new (ELeave) CObexSMWatcher(aObserver);
    CleanupStack::PushL( self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 *
 */
CObexSMWatcher::~CObexSMWatcher()
    {
    LOG_FUNC
    Cancel();
    iObexSMPostInit.Close();
    }

/**
 * Performs 2nd phase construction.
 */
void CObexSMWatcher::ConstructL()
    {
    LOG_FUNC

    TInt err = iObexSMPostInit.Attach(KUidObexSMCategory, KObexSMPostInitErrorProperty);
    User::LeaveIfError( err);
    
    RunL();
    }

/**
 * 
 */

CObexSMWatcher::CObexSMWatcher(MObexSMObserver& aObserver) :
    CActive(CActive::EPriorityStandard), iObserver(aObserver)
    {
    LOG_FUNC    
    CActiveScheduler::Add(this);
    }

/**
 * 
 */
void CObexSMWatcher::RunL()
    {
    LOG_FUNC 
    
    LOG1("iStatus=%d", iStatus.Int());
    
    iObexSMPostInit.Subscribe( iStatus );
    SetActive();
    
    TInt value = KErrNone;
    TInt err = iObexSMPostInit.Get(value);
    if (err == KErrNone && value != KErrNone)
        {
        iObserver.MosmError(value);
        }    

    }

/**
 *
 */
void CObexSMWatcher::DoCancel()
    {
    LOG_FUNC
    iObexSMPostInit.Cancel();
    }
