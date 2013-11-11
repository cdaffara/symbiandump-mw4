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
* Description:
*
*/

#include <e32cmn.h>
#include "stubclient.h"
#include "stubclientsrv.h"

_LIT(KStubSrvExe, "stubsrv.exe");
static TInt ClientStart()
	{
	RProcess server;
	TInt r = server.Create(KStubSrvExe, KNullDesC, TUidType(KNullUid, KNullUid, KStubSrvUid3));
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
	    server.Kill(0);
	else
	    server.Resume();
	User::WaitForRequest(stat);
	r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

void RStubClient::EnqueBehavior(const TApiBehavior& aBeh, TInt aExeDuration)
    {
    CheckSession();
    TApiBehaviorPckgBuf buf(aBeh);
    (void) SendReceive(EStubSrvEnqueApiBehavior, TIpcArgs(&buf, aExeDuration));    
    }

void RStubClient::DeleteBehaviors(TUint aLib, TUint aApi)
    {
    CheckSession();
    (void) SendReceive(EStubSrvDeleteApiBehaviors, TIpcArgs(aLib, aApi)); 
    }

void RStubClient::InvokeApi(TApiBehaviorPckg& aBehPckg, TApiInvokeMode aMode)
    {
    CheckSession();
    (void) SendReceive(EStubSrvInvokeApi, TIpcArgs(&aBehPckg, aMode, 0));    
    }

void RStubClient::InvokeApiL(TApiBehaviorPckg& aBehPckg, TRequestStatus& aStatus, TApiInvokeMode aMode)
    {
    CheckSession();
    TInt transctionId = 0;
    TPckg<TInt> transpckg(transctionId);
    TInt err = SendReceive(EStubSrvGetApiCompletionCode, TIpcArgs(aBehPckg().iLib, aBehPckg().iApi, &transpckg));
    if (err)
        {
        User::Leave(err);
        }
    SendReceive(EStubSrvInvokeApi, TIpcArgs(&aBehPckg, aMode, transctionId), aStatus);
    }

void RStubClient::InvokeApiCancel(TUint aLib, TUint aApi)
    {
    CheckSession();
    (void) SendReceive(EStubSrvInvokeApiCancel, TIpcArgs(aLib, aApi));
    }

TInt RStubClient::Connect()
    {
	TInt retVal = CreateSession(KStubSrvName, Version());
	if (retVal)
        {
        retVal = ClientStart();
        if (!retVal)
            retVal = CreateSession(KStubSrvName, Version());
        }
    return retVal;
    }

TVersion RStubClient::Version() const
    {
    return(TVersion(KStubSrvMajorVersionNumber, KStubSrvMinorVersionNumber, KStubSrvBuildNumber));
    }

_LIT(KPanicCat, "!Stubsrv");

void RStubClient::CheckSession()
    {
    if (!Handle())
        User::Panic(KPanicCat, KErrBadHandle);
    }
