/** @file
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
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

#ifndef __MESSAGEHANDLER_PAN__
#define __MESSAGEHANDLER_PAN__

/** Panic Category */
_LIT(KMessageHandler, "MessageHandler");

/** MessageHandler panic codes */
enum TMessageHandlerPanic
    {
    EMessageHandlerBadRequest = 1,
    EMessageHandlerBadDescriptor = 2,
    EMessageHandlerSrvCreateServer = 3,
    EMessageHandlerMainSchedulerError = 4,
    EMessageHandlerCreateTrapCleanup = 5,
    EMessageHandlerSrvSessCreateTimer = 6,
    EMessageHandlerReqAlreadyPending = 7,		//	A request is already pending
    EMessageHandlerDifferencesInIap = 8,
    EMessageHandlerBadState = 9,
    EMessageHandlerRemoveNonExistingCp = 10
    };

#endif // __MESSAGEHANDLER_PAN__
