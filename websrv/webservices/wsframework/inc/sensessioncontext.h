/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Each application that connects WS-stack through client-server
*                interface has an application context of it's own, which can
*                hold a list for active connections and service sessions, 
*                acting as a common place holder and bridge between the
*                smaller (contexts) that those contain. Class allows 
*                application-wide information to be shared efficiently
*                at proper layer.
*
*/








#ifndef SEN_SESSION_CONTEXT_H
#define SEN_SESSION_CONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
//#include "SenContext.h"
#include "senpointermap.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CSenSessionContext : public CBase // CSenContext
    {
    public:
        /**
        * Standard 2-phased constructor
        * @return pointer to new application context.
        */ 
        IMPORT_C static  CSenSessionContext* NewL();
        /**
        * Standard 2-phased constructor 
        * @return pointer to new application context,
        * which is left on cleanupstack.
        */ 
        IMPORT_C static  CSenSessionContext* NewLC();

        // Destructor:
        IMPORT_C virtual ~CSenSessionContext();
        
        // New methods:
        
    protected:
        IMPORT_C  CSenSessionContext();
                                     
        IMPORT_C void BaseConstructL();

    private: 
    };

#endif // SEN_SESSION_CONTEXT_H

// End of File
