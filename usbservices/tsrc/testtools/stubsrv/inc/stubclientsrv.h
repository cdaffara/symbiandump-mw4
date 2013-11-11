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

#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

_LIT(KStubSrvName, "stubsrv");

const TUid KStubSrvUid3 = {0xE0000090};

const TInt KStubSrvMajorVersionNumber = 1;
const TInt KStubSrvMinorVersionNumber = 1;
const TInt KStubSrvBuildNumber = 0;

enum TIpcConstant
    {
    EStubSrvEnqueApiBehavior,
    EStubSrvDeleteApiBehaviors,
    EStubSrvInvokeApi,
    EStubSrvInvokeApiCancel,
    EStubSrvGetApiCompletionCode,
    EStubSrvInvalidIpc,
    };

#endif

