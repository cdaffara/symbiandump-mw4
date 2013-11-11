/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Task handler for closing VPN connection
*
*/



// @file vpnconncloser.h

#ifndef __VPN_CONN_CLOSER_H__
#define __VPN_CONN_CLOSER_H__

#include "sit.h"
#include "taskhandler.h"
#include "kmdapi.h"
#include "ipsecpolapi.h"
#include "eventlogger.h"
#include "eventdefssit.h"

// Task handling states
const TInt KStateDeactivateKmd          = 1;
const TInt KStateUnloadIpsecPolicy      = 2;
const TInt KStateAfterUnloadIpsecPolicy = 3;

/**
 * Protocol version getter task handler
 */
NONSHARABLE_CLASS(CVpnConnCloser) : public CTaskHandler
    {
public:
    static CVpnConnCloser* NewL(MTaskHandlerManager* aManager,
                                const TTaskArrivedEventData& aTaskInfo);
    ~CVpnConnCloser();

private: // From CTaskHandler
    void StartTaskHandling();
    void ChangeStateL();
    void CancelOngoingOperation();
    void ReportResult(TInt aStatus);

private:
    CVpnConnCloser(MTaskHandlerManager* aManager,
                   const TTaskArrivedEventData& aTaskInfo);
    void ConstructL();

    void StateDeactivateKmdL();
    void StateUnloadIpsecPolicy();
    void StateAfterUnloadIpsecPolicy();
    
private:
    TPckgBuf<TCloseVpnConnEventSpec> iEventSpecDes;

    RKMD iKmdServ;
    RIpsecPolicyServ iPolicyServ;

    DEFINE_EVENT_LOGGER
    };

#endif // __VPN_CONN_CLOSER_H__
