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

#include "senservicemanagerimpl.h"

// ============================ LOCAL FUNCTIONS ===============================

EXPORT_C CSenServiceManager* CSenServiceManager::NewL()
    {
    return CSenServiceManagerImpl::NewL();
    }

EXPORT_C CSenServiceManager* CSenServiceManager::NewLC()
    {
    return CSenServiceManagerImpl::NewLC();
    }

EXPORT_C CSenServiceManager* CSenServiceManager::NewL(MSenAuthenticationProvider& aAuthProvider)
    {
    return CSenServiceManagerImpl::NewL(aAuthProvider);
    }

EXPORT_C CSenServiceManager* CSenServiceManager::NewLC(MSenAuthenticationProvider& aAuthProvider)
    {
    return CSenServiceManagerImpl::NewLC(aAuthProvider);
    }

CSenServiceManager::CSenServiceManager()
    : CActive(EPriorityStandard)
    {
    }

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return(KErrNone);
    }
#endif

// End of file
