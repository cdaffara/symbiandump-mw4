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
* Description:  KMD starter
*
*/


#include "srvstarter.h"
#include "kmdserver.h"

CServer2* Starter::CreateAndStartServerL()
    {
    return CKmdServer::NewL();
    }

TPtrC Starter::ServerName()
    {
    return KKmdServerName().Mid(0);
    }
