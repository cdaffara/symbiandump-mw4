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
*      Error codes for COD Handler.   
*      
*
*/


#ifndef COD_ERROR_H
#define COD_ERROR_H

#include <e32def.h>

// CONSTANTS

/// Error in COD (syntax error, missing mandatory attributes etc.).
LOCAL_D const TInt KErrCodInvalidDescriptor = -1000;
/// Content is too large to be saved.
LOCAL_D const TInt KErrCodInsufficientSpace = -1001;
/// No Access Point available.
LOCAL_D const TInt KErrCodNoAccessPoint = -1002;
/// Not Supported version.
LOCAL_D const TInt KErrCodUnsupportedVersion = -1003;
/// Attribute mismatch.
LOCAL_D const TInt KErrCodAttributeMismatch = -1004;
/// Invalid type.
LOCAL_D const TInt KErrCodInvalidType = -1005;

// "Borrowed" error codes!
// This means that we are using error codes from other modules as they already
// have localised error strings known by the error resolver.

/// "Message not supported".
LOCAL_D const TInt KErrCodProfileNotSupported = -4604;
/// "Message not supported".
LOCAL_D const TInt KErrCodConfigNotSupported = -4604;
/// "Could not connect to Internet. Make sure that Network Settings is closed."
LOCAL_D const TInt KErrCodCannotOpenConnection = -181;
/// "Could not find specified Internet server"
LOCAL_D const TInt KErrCodDnsLookupFailed = -3058;
/// "Could not connect to the specified server"
LOCAL_D const TInt KErrCodCannotConnect = -191;
/// "Operation aborted by server."
LOCAL_D const TInt KErrCodHttpAbort = -20042;
/// "Authentication failed"
LOCAL_D const TInt KErrCodHttpAuthFailed = -4157;
/// "Authentication failed"
LOCAL_D const TInt KErrCodHttpAuthAborted = -4157;
/// "Check gateway IP address setting."
LOCAL_D const TInt KErrCodHttpInvalidGateway = -20040;
/// "Page not found. Check URL address."
LOCAL_D const TInt KErrCodHttpBadUrl = -20028;
/// "Response unknown."
LOCAL_D const TInt KErrCodHttpBadResponse = -20019;
/// "Connection not available."
LOCAL_D const TInt KErrCodHttpCommsFailed = -20037;
/// "Connection time-out."
LOCAL_D const TInt KErrCodHttpNoResponse = -20015;
/// "Page not found."
LOCAL_D const TInt KErrCodHttpNotFound = -20005;
/// "Operation aborted by server."
LOCAL_D const TInt KErrCodHttpServerError = -20025;
/// "Connection time-out"
LOCAL_D const TInt KErrCodHttpTimedOut = -20015;
/// "Page not available."
LOCAL_D const TInt KErrCodHttpUnavailable = -20029;
/// "Connection to WAP server dropped."
LOCAL_D const TInt KErrCodWapConnectionDropped = -20041;
/// "License Failed."
LOCAL_D const TInt KErrCodHttpLicenseFailed = -20027;
/// "Precondition Failed error code received from HTTP stack"
LOCAL_D const TInt KErrCodHttpRequestedRangeNotSatisfiable = -20043;
/// "Precondition Failed error code received from HTTP stack"
LOCAL_D const TInt KErrCodHttpPreconditionFailed = -20044;
/// "COD Download paused"
LOCAL_D const TInt KErrCodHttpDownloadPaused = -20045;
/// "Multiple Object Download Failed
LOCAL_D const TInt KErrMultipeObjectDownloadFailed = -20046;

#endif /* def COD_ERROR_H */
