/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        
*
*/








// INCLUDE FILES
#include "sensecuritymechanismobserver.h"
#include "senbaseidentitymanager.h"

#include <flogger.h>
#include "sendebug.h"
#include "senlogger.h"

// CONSTANTS
namespace
    {
    }

CSenSenSecurityMechanismObserver* CSenSenSecurityMechanismObserver::NewL(
                                            CSenBaseIdentityManager& aOwner)
    {
    CSenSenSecurityMechanismObserver* pNew =
                        CSenSenSecurityMechanismObserver::NewLC(aOwner);
    CleanupStack::Pop();
    return pNew;
    }

CSenSenSecurityMechanismObserver* CSenSenSecurityMechanismObserver::NewLC(
                                            CSenBaseIdentityManager& aOwner)
    {
    CSenSenSecurityMechanismObserver* pNew =
                    new (ELeave) CSenSenSecurityMechanismObserver(aOwner);
    CleanupStack::PushL(pNew);

    pNew->ConstructL();
    return pNew;
    }

CSenSenSecurityMechanismObserver::CSenSenSecurityMechanismObserver(
                                            CSenBaseIdentityManager& aOwner)
: CActive(EPriorityStandard),
  iOwner(aOwner)
    {
    }

void CSenSenSecurityMechanismObserver::ConstructL()
    {
    CActiveScheduler::Add(this);
    iEcomSession = REComSession::OpenL();
    }

CSenSenSecurityMechanismObserver::~CSenSenSecurityMechanismObserver()
    {
    iEcomSession.CancelNotifyOnChange(iStatus);
    Cancel();
    iEcomSession.Close();
    }

void CSenSenSecurityMechanismObserver::RunL()
    {
    if (iStatus == KErrNone)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenSenSecurityMechanismObserver::RunL");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"..calling IdentityManager.ReloadSenSecurityMechanismsL");
        iOwner.ReloadSenSecurityMechanismsL();
        iEcomSession.NotifyOnChange(iStatus);
        if(!IsActive())
        	SetActive();
        }
    }
TInt CSenSenSecurityMechanismObserver::RunError(TInt /* aError */)
    {
//    if (aError != KErrNone)
    return KErrNone;
    }

void CSenSenSecurityMechanismObserver::DoCancel()
    {
    TRequestStatus status;
    iEcomSession.CancelNotifyOnChange(status);
    }

void CSenSenSecurityMechanismObserver::Start()
    {
    if(!IsActive())
    	SetActive();
    iEcomSession.NotifyOnChange(iStatus);
    }

RFileLogger* CSenSenSecurityMechanismObserver::Log()
    {
    return iOwner.Log();
    }

namespace
    {
    }

//  End of File
