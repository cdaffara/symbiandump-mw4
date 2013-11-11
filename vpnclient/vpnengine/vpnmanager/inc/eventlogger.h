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
* Description: Event logging.
*
*/



#ifndef __EVENTLOGGER_H__
#define __EVENTLOGGER_H__

#include <e32base.h>
#include <in_sock.h>
#include <vpnlogmessages.rsg>

#include "eventmediatorapi.h"

class CPolicyStore;

class CEventLogger : CBase
  	{
public:
    static CEventLogger* NewL(CPolicyStore& aPolicyStore);
	void LogEvent(TUint aMsgId, const TDesC* aDes1, const TDesC8* aDes2, TInt aInt1, TInt aInt2);
    void LogEvent(TUint aMsgId, const TDesC8* aDes1, const TDesC8* aDes2, TInt aInt1, TInt aInt2);
    ~CEventLogger();

private:
    CEventLogger(CPolicyStore& aPolicyStore);
    void ConstructL();
    
    void LogEventL(TUint aMsgId, const TDesC* aDes1, const TDesC8* aDes2, TInt aInt1, TInt aInt2);
    HBufC8* CertSubjectNameL(const TDesC8& aCertData);
    
    
private:
    REventMediator iEventMediator;
    CPolicyStore& iPolicyStore;    
	};

#define DEFINE_EVENT_LOGGER         CEventLogger* iEventLogger;
#define INIT_EVENT_LOGGER(a)        TRAP_IGNORE(iEventLogger = CEventLogger::NewL(a););
#define RELEASE_EVENT_LOGGER        delete iEventLogger; iEventLogger = NULL;

#define LOG_EVENT(a, b, c, d, e)    if (iEventLogger) iEventLogger->LogEvent(a, b, c, d, e);
#define LOG_EVENT_2(a, b, c, d, e)  if (iVpnApiServant->iEventLogger) iVpnApiServant->iEventLogger->LogEvent(a, b, c, d, e);
#define LOG_EVENT_2B(a, b, c, d, e) if (iVpnApiServant.iEventLogger) iVpnApiServant.iEventLogger->LogEvent(a, b, c, d, e);
#define LOG_EVENT_3(a, b, c, d, e)  if (VpnApiServant()->iEventLogger) VpnApiServant()->iEventLogger->LogEvent(a, b, c, d, e);

#endif // __EVENTLOGGER_H__
