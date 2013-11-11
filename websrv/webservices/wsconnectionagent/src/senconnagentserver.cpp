/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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







#include "senconnagentserver.h"
#include "senconnagentsession.h"

_LIT(KSenConnServerName, "_SenConnSrv");
const TInt KMaxServerName = 80;


CSenConnAgentServer::CSenConnAgentServer(): CServer2(EPriorityMore)
    {
    
    }

CSenConnAgentServer::~CSenConnAgentServer()
    {
    
    }

CSession2* CSenConnAgentServer::NewSessionL(const TVersion& /*aVersion*/,
        const RMessage2& /*aMessage*/) const
    {
    return new (ELeave) CSenConnAgentSession;
    }

EXPORT_C const TDesC& CSenConnAgentServer::Open()
    {
    CSenConnAgentServer* server = reinterpret_cast<CSenConnAgentServer*>
            (Dll::Tls());
    if (server == NULL)
        {
        server = new CSenConnAgentServer;
        if (server == NULL)
            {
            return KNullDesC;
            }
        server->iName = RThread().FullName().Right(KMaxServerName -
                KSenConnServerName().Length());
        for (TInt i = 0; i < server->iName.Length(); i++)
            {
            TUint16& chr = (server->iName)[i];
            if (chr == ':')
                {
                chr = ';';
                }
            }
        server->iName.Append(KSenConnServerName);
        TInt err = server->Start(server->iName);
        if (err != KErrNone)
            {
            delete server;
            return KNullDesC;
            }
        Dll::SetTls(server);
        }
    server->iCount++;
    return server->iName;
    }

EXPORT_C void CSenConnAgentServer::Close()
    {
    CSenConnAgentServer* server = reinterpret_cast<CSenConnAgentServer*>
            (Dll::Tls());
    if (server == NULL)
        {
        return;
        }
    --server->iCount;
    if (!server->iCount)
        {
        delete server;
        Dll::SetTls(NULL);
        }
    }
