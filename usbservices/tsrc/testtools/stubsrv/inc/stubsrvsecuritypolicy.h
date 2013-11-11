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

#ifndef SERVER_SECURITYPOLICY_H
#define SERVER_SECURITYPOLICY_H

#include "stubclientsrv.h"

const TInt KStubSrvIpcRanges[] = 
    {
    0,
    EStubSrvInvalidIpc,
    };

const TInt KStubSrvIpcRangeCount = sizeof(KStubSrvIpcRanges) / sizeof(TInt);

const TUint8 KStubSrvElementsIndex[KStubSrvIpcRangeCount] = 
    {
    0,
    CPolicyServer::ENotSupported
    };

const CPolicyServer::TPolicyElement KStubSrvElements[] = 
    {
    {TSecurityPolicy::EAlwaysPass, CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy KStubSrvPolicy =
    {
    CPolicyServer::EAlwaysPass,
    KStubSrvIpcRangeCount,                               
    KStubSrvIpcRanges,
    KStubSrvElementsIndex,
    KStubSrvElements
    };

#endif


