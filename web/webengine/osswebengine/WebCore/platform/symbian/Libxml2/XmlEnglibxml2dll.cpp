/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definitions and declarations that affect XML Engine Core
*
*/ 

#include <stdlib.h>
//#include "Libxml2_threads.h"
#include "Libxml2_globals.h"
#include <string.h>

#include <e32std.h>

xmlGlobalStatePtr
xmlGetGlobalState(void)
{
    xmlGlobalStatePtr gs = STATIC_CAST(xmlGlobalStatePtr, Dll::Tls());
    if(!gs)
        {
        gs =  (xmlGlobalStatePtr) malloc(sizeof(xmlGlobalState));
        if(gs)
            {
            memset(gs, 0, sizeof(xmlGlobalState));
           
		    TThreadStackInfo aInfo;
		    RThread().StackInfo(aInfo);
		    TUint32 stackLimit = aInfo.iLimit;
               
            xmlInitializeGlobalState(gs, stackLimit);
            xeSetTLS(gs);
            }
        }
    return gs; // NOTE: NULL can be returned only during first access to the function,
               //       which happens from XmlEngine::XmlEngineAttach() method called by XmlEngine::XmlEnginePushL() 
}

void xeSetTLS(void* ptr)
{
    Dll::SetTls(ptr);
}

xmlGlobalStatePtr xeGetTLS()
{
    return STATIC_CAST(xmlGlobalStatePtr, Dll::Tls());
}

void xecleanTLS()
{
     xmlGlobalStatePtr gs = xeGetTLS();
     delete gs;
     xeSetTLS(NULL);
}
