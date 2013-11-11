/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Panic codes for COD Handler.   
*      
*
*/


#ifndef COD_PANIC_H
#define COD_PANIC_H

// INCLUDES

#include <e32def.h>

// TYPES

/**
* Panic reasons for COD Handler.
*/
enum TCodHandlerPanicReason
    {
    /// Attempt to read beyond the end of a buffer.
    ECodBufferOverread = -300,
    /// A command / operation is not expected in the current state.
    ECodOffState,
    /// Invalid COD data found where only valid is expected.
    ECodInvalidData,
    /// Observer already set.
    ECodObserverAlreadySet,
    /// Invalid arguments.
    ECodInvalidArguments,
    /// Request pending.
    ECodRequestPending,
    /// Internal error.
    ECodInternal
    };

// FUNCTION DECLARATIONS

/**
* Panic the COD Handler.
* @param aReason Reason for the panic.
*/
void CodPanic( TCodHandlerPanicReason aReason );

#endif /* def COD_PANIC_H */
