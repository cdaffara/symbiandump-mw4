/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines constants
*
*/



#ifndef __EVENTMEDIATORDEFS_H__
#define __EVENTMEDIATORDEFS_H__

#include <e32base.h>

static const TUid KEventMediatorUid3 = {0x101FD288};

_LIT(KEventMediatorFile, "eventmed");
_LIT(KEventMediatorServer, "!EventMediatorServer");

// The server version. A version must be specified when 
// creating a session with the server
const TUint KEventMediatorMajorVersionNumber = 0;
const TUint KEventMediatorMinorVersionNumber = 1;
const TUint KEventMediatorBuildVersionNumber = 1;

/*// Panic codes
enum TEventMediatorPanic
    {
    ECreateTrapCleanup = 1,
    ECreateServer,
    EBadDescriptor,
    EBadRequest,
    };
*/
#endif // __EVENTMEDIATORDEFS_H__
