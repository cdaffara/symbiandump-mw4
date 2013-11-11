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
* Description:   Event logger
*
*/



// @file eventlogger.h

#ifndef __EVENTLOGGER_H__
#define __EVENTLOGGER_H__

#include <e32base.h>
#include <in_sock.h>
#include <vpnlogmessages.rsg>

#include "eventmediatorapi.h"
#include "eventviewer.h"

const TInt KMaxIpAddressLength = 64;

_LIT8(KUnknownVpnAp, "Unknown VPN access point");
_LIT8(KUnknownIap, "Unknown IAP");

const TInt R_VPN_MSG_VPN_IAP_ACT_START  = KMaxTInt;
const TInt R_VPN_MSG_VPN_IAP_ACT_END    = KMaxTInt - 1;
const TInt R_VPN_MSG_VPN_IAP_ACT_CANCEL = KMaxTInt - 2;


NONSHARABLE_CLASS(CEventLogger) : CBase
  	{
public:
    static CEventLogger* NewL(REventMediator& aEventMediator, TUint32 aVpnIapId, TUint32 aRealIapId);
	void LogEvent(TUint aMsgId, TAny* aAnyPtr, TInt aStatusCode, TInt aReasonCode);
    ~CEventLogger();

private:
    CEventLogger(REventMediator& aEventMediator, TUint32 aVpnIapId, TUint32 aRealIapId);
    void ConstructL();
    
    TInt DoLogEvent(TUint aMsgId, TAny* aAnyPtr, TInt aStatusCode, TInt aReasonCode);
    
private:
    TIapName iVpnApName;
    TIapName iRealApName;
    REventMediator& iEventMediator;
    TUint32 iVpnIapId;
    TUint32 iRealIapId;
	};

#define DEFINE_EVENT_LOGGER         CEventLogger* iEventLogger;
#define INIT_EVENT_LOGGER(a, b, c)  TRAP_IGNORE(iEventLogger = CEventLogger::NewL(a, b, c););
#define RELEASE_EVENT_LOGGER        delete iEventLogger; iEventLogger = NULL;

#define LOG_EVENT(a, b, c, d)       if (iEventLogger) iEventLogger->LogEvent(a, b, c, d);

#endif // __EVENTLOGGER_H__
