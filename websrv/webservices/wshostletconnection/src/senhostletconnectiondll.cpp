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
* Description:        
*
*/








// INCLUDE FILES
#include <e32std.h>

#include "senhostletconnectionimpl.h"
//#include "SenHostletConnectionLog.h"

// ============================= LOCAL FUNCTIONS ===============================

EXPORT_C CSenHostletConnection* CSenHostletConnection::NewL(MSenHostlet& aProvider)
    {
    return CSenHostletConnectionImpl::NewL(aProvider);
    }

EXPORT_C CSenHostletConnection* CSenHostletConnection::NewLC(MSenHostlet& aProvider)
    {
    return CSenHostletConnectionImpl::NewLC(aProvider);
    }

CSenHostletConnection::CSenHostletConnection()
    : CActive(EPriorityStandard)
    {
    }

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return(KErrNone);
    }
#endif

#ifdef _SENDEBUG

// static logging functions
/*
void CSenHostletConnectionLogger::WriteFormat(TRefByValue<const TDesC16> aFmt,...)
    {
    CSenLogger::WriteFormat(KSenHostletConnectionLogChannel, KHostletConnectionLogLevel, aFmt);
    }
    
void CSenHostletConnectionLogger::WriteFormat(TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
    {
    CSenLogger::WriteFormat(KSenHostletConnectionLogChannel, KHostletConnectionLogLevel, aFmt, aList);
    }

void CSenHostletConnectionLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
    {
    CSenLogger::WriteFormat(KSenHostletConnectionLogChannel, KHostletConnectionLogLevel, aFmt);
    }

void CSenHostletConnectionLogger::WriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    CSenLogger::WriteFormat(KSenHostletConnectionLogChannel, KHostletConnectionLogLevel, aFmt, aList);
    }
*/
#endif // _SENDEBUG

// End of file


