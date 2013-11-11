/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Uid definitions for server and client 
*
*/



#ifndef __ALFSTREAMERUIDS_H__
#define __ALFSTREAMERUIDS_H__

#define KDLLUID 0xabbadaba
#define KDLLUID2 0xdabaabba

#define KServerUid3 TUid::Uid(KSERVERUID)

#define ALFSTREAMER_SERVER_NAME _L("alfstreamerserver")
_LIT(KAlfStreamerServerThreadName,"AlfStreamerServerThread");
_LIT(KAlfStreamerServerSemaphoreName,"AlfStreamerServerSemaphore");

#endif