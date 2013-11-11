/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef __LOGGER_H_____
#define __LOGGER_H_____

#ifdef __CUSTOM_LOG_ENABLED__

    #define DOLOG(a) DoLog(a)

    // INCLUDES
    #include <e32std.h>

    // CONSTANTS
    _LIT(KLogFileName, "c:\\SenNotPluginDlgImpl.txt");

    // panics
    _LIT(KLogPanicCategory, "CustNotifyLog");
    const TInt KPanicFsConnectFailed = 1;
    const TInt KPanicFileCreateFailed = 2;
    const TInt KPanicFileWriteFailed = 3;

    void DoLog(const TDesC& aText);

#else  //__CUSTOM_LOG_ENABLED__
    #define DOLOG(a)
#endif //__CUSTOM_LOG_ENABLED__

#endif //__LOGGER_H_____

// End of File
