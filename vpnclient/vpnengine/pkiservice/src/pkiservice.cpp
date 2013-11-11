/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI server main module
*
*/

#include <ecom/ecom.h>

#include "pkiservice.h"
#include "pkisession.h"
#include "PKIMapper.h"
#include "pkiserviceclientservercommon.h"
#include "certificaterequeststore.h"
#include "keyoperationqueue.h"
#include "log_r6.h"
#include "pkiserviceassert.h"

const TUint CPKIService::iRangeCount = 2;

const TInt CPKIService::iRanges[iRangeCount] = 
    {
    PkiService::ELogon,
    PkiService::ESetInformational+1
    };

const TUint8 CPKIService::iElementIndex[iRangeCount] = 
    {
    0,
    CPolicyServer::ENotSupported
    };

const CPolicyServer::TPolicyElement CPKIService::iElements[] =
    {
	{_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy CPKIService::iPolicy =
    {
    0, 						// All connect attempts are checked
    iRangeCount,            // Count of ranges
    iRanges,                // 0-25, 26...
    iElementIndex,          // Only range 0-25§ are checked
    iElements               // The list of policy elements
    };
    
CPKIService::CPKIService(void):CPolicyServer(EPriorityStandard,iPolicy)
    {
    iSessionCount = 0;
    }


CPKIService::~CPKIService(void)
    {    
    delete iKeyOperationQueue;
    delete iMapper;    
    delete iShutdown;
    delete iCertificateRequestStore;
    
    REComSession::FinalClose();
    }


CPKIService* CPKIService::NewL(void)
    {
    CPKIService* self = new (ELeave) CPKIService;
    CleanupStack::PushL(self);
    
    self->ConstructL();
    
    CleanupStack::Pop(self);
    return self;
    }


void CPKIService::ConstructL()
    {
    //Makes sure private path exists
    RFs fileServer;
    User::LeaveIfError(fileServer.Connect());
    CleanupClosePushL(fileServer);
    TInt err = fileServer.CreatePrivatePath(EDriveC);
    if (err != KErrNone &&
        err != KErrAlreadyExists)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(); //fileServer    
    
    iCertificateRequestStore = CCertificateRequestStore::NewL();    
    iShutdown = new (ELeave) CSuspendedShutdown();
    iShutdown->Construct();    
    
    iMapper = CPKIMapper::NewL();
    iKeyOperationQueue = CKeyOperationQueue::NewL(*iMapper);
    
        
    StartL(KPkiServerName);
    }
        

CSession2* CPKIService::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
    {
    iShutdown->Cancel();
    CSession2* session = CPKISession::NewL(*const_cast<CPKIService*>(this), *iMapper, *iKeyOperationQueue);        
    iSessionCount++;
    return session;
    }

void CPKIService::SessionDeleted()
    {
    LOG_("-> CPKIService::SessionDeleted()");    
    iSessionCount--;
    
    LOG_1("iSessionCount (%d)", iSessionCount);    
    PKISERVICE_ASSERT(iSessionCount >= 0);
    
    if (iSessionCount == 0)
        {
        if (iStopImmediately)
            {
            CActiveScheduler::Stop();
            }
        else
            {
            iShutdown->ArmShutdown();
            }
        }
    LOG_("<- CPKIService::SessionDeleted()");    
    }

CCertificateRequestStore& CPKIService::CertificateRequestStore()
    {
    return *iCertificateRequestStore;
    }


const TInt KSuspendTime = 240000000;  // 240 sec
//const TInt KSuspendTime = 5000000;  // 5 sec

CSuspendedShutdown::CSuspendedShutdown() : CActive(EPriorityStandard)
{
}

void CSuspendedShutdown::Construct()
{
    iTimer.CreateLocal();
    CActiveScheduler::Add(this);
}

CSuspendedShutdown::~CSuspendedShutdown()
{
    iTimer.Close();
}

void CSuspendedShutdown::ArmShutdown()
{
    iTimer.After(iStatus, KSuspendTime);
    SetActive();
}

void CSuspendedShutdown::DoCancel()
{
    iTimer.Cancel();
}

void CSuspendedShutdown::RunL()
{
    CActiveScheduler::Stop();
}
