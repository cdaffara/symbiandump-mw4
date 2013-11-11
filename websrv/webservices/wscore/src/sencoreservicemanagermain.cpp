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








#include "sencoreservicemanager.h"

#if defined (__WINS__) && !defined(EKA2)
IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
    {
    return reinterpret_cast<TInt>(&CSenCoreServiceManager::ThreadFunction);
    }

GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#else  // __ARMI__ && EKA2
GLDEF_C TInt E32Main()
    {
    return CSenCoreServiceManager::ThreadFunction(NULL);
    }
#endif

//END OF FILE

