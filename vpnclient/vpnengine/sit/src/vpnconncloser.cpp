/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Task handler for closing VPN connection.
*
*/



#include "vpnconncloser.h"
#include "kmdapi.h"
#include "log.h"

CVpnConnCloser* CVpnConnCloser::NewL(MTaskHandlerManager* aManager,
                                     const TTaskArrivedEventData& aTaskInfo)
    {
    CVpnConnCloser* self = new (ELeave) CVpnConnCloser(aManager, aTaskInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CVpnConnCloser::CVpnConnCloser(MTaskHandlerManager* aManager,
                               const TTaskArrivedEventData& aTaskInfo)
    : CTaskHandler(aManager, aTaskInfo, ECloseVpnConnCancelEvent, &iEventSpecDes) 
    {
    }

void CVpnConnCloser::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iEventMediator.Connect());
    User::LeaveIfError(iKmdServ.Connect());
    User::LeaveIfError(iPolicyServ.Connect());
    }
    
CVpnConnCloser::~CVpnConnCloser()
    {
    Cancel();
    iEventMediator.Close();
    iKmdServ.Close();
    iPolicyServ.Close();
    RELEASE_EVENT_LOGGER;
    }
    
void CVpnConnCloser::StartTaskHandling()
    {
	DEB(LOG(Log::Printf(_L("Starting VPN IAP deactivation\n")));)			
    INIT_EVENT_LOGGER(iEventMediator, iEventSpecDes().iVpnIapId, iEventSpecDes().iRealIapId);
    GotoState(KStateDeactivateKmd);
    }
    
void CVpnConnCloser::ChangeStateL()
    {
    switch (NextState())
        {
        case KStateDeactivateKmd:
            StateDeactivateKmdL();
            break;

        case KStateUnloadIpsecPolicy:
            StateUnloadIpsecPolicy();
            break;

        case KStateAfterUnloadIpsecPolicy:
            StateAfterUnloadIpsecPolicy();
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }

void CVpnConnCloser::CancelOngoingOperation()
    {
    switch (CurrState())
        {
        case KStateDeactivateKmd:
        case KStateAfterUnloadIpsecPolicy:
            // There's no ongoing external request to cancel
            break;

        case KStateUnloadIpsecPolicy:
			DEB(LOG(Log::Printf(_L("Canceling Ipsec policy unload\n")));)
            iPolicyServ.CancelUnload();
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }

void CVpnConnCloser::StateDeactivateKmdL()
    {
    SetCurrState(KStateDeactivateKmd);
    TInt err = iKmdServ.StopVpnConnection( iEventSpecDes().iVpnIapId,
                                        (TKmdStopConnection::TType)iEventSpecDes().iDeactivateType );
	DEB(LOG(Log::Printf(_L("VPN connection stopped, VPN IAP id=%d, err=%d\n"),
	        iEventSpecDes().iVpnIapId, err ));)	
    GotoState(KStateUnloadIpsecPolicy);
    }

void CVpnConnCloser::StateUnloadIpsecPolicy()
    {
    SetCurrState(KStateUnloadIpsecPolicy);

    iPolicyServ.UnloadPolicy(iEventSpecDes().iIpsecPolicyHandle, iStatus);
	DEB(LOG(Log::Printf(_L("Ipsec policy unloaded, handle = %d\n"), iEventSpecDes().iIpsecPolicyHandle));)	
    SetNextState(KStateAfterUnloadIpsecPolicy);
    SetActive();
    }

void CVpnConnCloser::StateAfterUnloadIpsecPolicy()
    {
    SetCurrState(KStateAfterUnloadIpsecPolicy);
	DEB(LOG(Log::Printf(_L("VPN IAP deactivation completed, Ipsec policy unload status = %d\n"), iStatus.Int()));)

    LOG_EVENT(R_VPN_MSG_VPN_IAP_DEACT, NULL, iStatus.Int(), iEventSpecDes().iDeactivateType);
    
    TaskComplete(iStatus.Int());
    }

void CVpnConnCloser::ReportResult(TInt aStatus)
    {
    // Report the close operation status
    // information to the Event Mediator

    TCloseVpnConnEventData eventData;
    
    eventData.iTaskStatus = aStatus;
    
    TPckg<TCloseVpnConnEventData> eventDataDes(eventData);
    ReportEvent(ECloseVpnConnEvent, iEventSpecDes, eventDataDes);
    iKmdServ.Close();
    }
