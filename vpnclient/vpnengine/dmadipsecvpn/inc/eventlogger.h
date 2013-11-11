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
* Description: Class definition of CEventLogger.
*
*/



#ifndef __EVENTLOGGER_H__
#define __EVENTLOGGER_H__


#include <e32base.h>

#include "eventmediatorapi.h"
#include "vpnapi.h"

class CEventLogger : CBase
  	{
public:
    static CEventLogger* NewL();
    void LogEvent(RVpnServ& vpnApi, TUint aMsgId, const TDesC* aDes1, TInt aInt1, TInt aInt2);
    ~CEventLogger();

private:
    CEventLogger();
    void ConstructL();
    void LogEvent(TUint aMsgId, const TDesC8* aDes1, TInt aInt1, TInt aInt2);
    
private:
    REventMediator iEventMediator;
	};

#define DEFINE_EVENT_LOGGER         CEventLogger* iEventLogger;
#define INIT_EVENT_LOGGER           TRAP_IGNORE(iEventLogger = CEventLogger::NewL(););
#define RELEASE_EVENT_LOGGER        delete iEventLogger; iEventLogger = NULL;

#define LOG_EVENT(a, b, c, d, e)       if (iEventLogger) iEventLogger->LogEvent(a, b, c, d, e);

#endif // __EVENTLOGGER_H__
