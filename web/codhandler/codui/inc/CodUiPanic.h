/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic codes.
*
*/


#ifndef COD_UI_PANIC_H
#define COD_UI_PANIC_H

// TYPES

/**
* Panic reasons.
*/
enum TCodUiPanicReason
    {
    /// Unknown command.
    ECodUiUnknownCommand,
    /// Unknown status.
    ECodUiUnknownStatus,
    /// Internal error.
    ECodUiInternal,
    /// Callback function is invalid (returned unsupported ETrue value).
    ECodUiInvalidCallback,
    /// A request is pending.
    ECodUiRequestPending,
    /// Bad control id.
    ECodUiBadControl
    };

// FUNCTION DECLARATIONS

/**
* Panic.
* @param aReason Reason for the panic.
*/
void CodUiPanic( TCodUiPanicReason aReason );

#endif /* def COD_UI_PANIC_H */
