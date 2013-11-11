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
#ifndef __DISPATCHER_PAN__
#define __DISPATCHER_PAN__

/** Panic Category */
_LIT(KDispatcher, "Dispatcher");

/** Dispatcher panic codes */
enum TDispatcherPanics 
    {
    ECSAsyncBasicUi = 1,
	ECSAsyncProgram,
	EMessageHandlerBadState
    };

#endif // __DISPATCHER_PAN__
