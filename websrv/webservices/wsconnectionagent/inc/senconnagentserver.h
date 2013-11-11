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
* Description: Header declaration
*
*/







#include <e32base.h>

NONSHARABLE_CLASS(CSenConnAgentServer): public CServer2
    {
    public:
    IMPORT_C static const TDesC& Open();
    IMPORT_C static void Close();
    inline void DestroyIfInactive();
    
    private:
    CSenConnAgentServer();
   ~CSenConnAgentServer();
    CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
    
    private:
    TInt iCount;
    TFullName iName;
    };

void CSenConnAgentServer::DestroyIfInactive()
    {
    iSessionIter.SetToFirst();
    if (iSessionIter++ == NULL)
        {
        delete this;
        Dll::SetTls(NULL);
        }
    }
