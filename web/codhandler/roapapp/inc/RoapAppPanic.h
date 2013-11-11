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
* Description: 
*      Error and panic codes.   
*      
*
*/


#ifndef ROAP_APP_PANIC_H
#define ROAP_APP_PANIC_H

// TYPES

/**
* Panic reasons for DD Handler.
*/
enum TRoapAppPanicReason
    {
    /// Unknown command.
    ERoapAppUnknownCommand,
    /// Unknown status.
    ERoapAppUnknownStatus,
    /// Internal error.
    ERoapAppInternal,
    /// Callback function is invalid (returned unsupported ETrue value).
    ERoapAppInvalidCallback,
    /// A request is pending.
    ERoapAppRequestPending
    };

// FUNCTION DECLARATIONS

/**
* Panic the application.
* @param aReason Reason for the panic.
*/
void RoapAppPanic( TRoapAppPanicReason aReason );

#endif /* def ROAP_APP_PANIC_H */
