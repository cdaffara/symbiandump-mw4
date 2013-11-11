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
* Description:   Task handler for getting IP version
*
*/



// @file protocolversiongetter.h

#ifndef __PROTOCOL_VERSION_GETTER_H__
#define __PROTOCOL_VERSION_GETTER_H__

#include "sit.h"
#include "taskhandler.h"
#include "vpnapi.h"
#include "eventdefssit.h"

// Task handling states
const TInt KStateGetProtocolVersion = 1;

/**
 * Protocol version getter task handler
 */
NONSHARABLE_CLASS(CProtocolVersionGetter) : public CTaskHandler
    {
public:
    static CProtocolVersionGetter* NewL(MTaskHandlerManager* aManager,
                                        const TTaskArrivedEventData& aTaskInfo);
    ~CProtocolVersionGetter();

private: // From CTaskHandler
    void StartTaskHandling();
    void ChangeStateL();
    void CancelOngoingOperation();
    void ReportResult(TInt aStatus);

private:
    CProtocolVersionGetter(MTaskHandlerManager* aManager,
                           const TTaskArrivedEventData& aTaskInfo);
    void ConstructL();

    void StateGetProtocolVersionL();

    void GetProtocolVersionL();
    void ParseProtocolVersionL(const TDesC8& aPolicyData);
    
private:
    TPckgBuf<TGetProtocolVersionEventSpec> iEventSpecDes;
    TGetProtocolVersionEventData iEventData;

    RVpnServ iVpnServ;
    };

#endif // __PROTOCOL_VERSION_GETTER_H__
