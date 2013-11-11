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








#ifndef SEN_SERVER_CONTEXT_H
#define SEN_SERVER_CONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
//#include "SenContext.h"
#include "senpointermap.h" // pointer map is internal/ado: Utils\inc

#include "sencoreservicemanager.h"

// FORWARD DECLARATIONS
class CSenApplicationContext;
class CSenCoreServiceManager;


// TYPE DEFS
typedef RSenPointerMap<TUint32, CSenApplicationContext> RApplicationCtxMap; 

// CLASS DECLARATION
class CSenServerContext : public CBase // CSenContext
    {
    public:
        /**
        * Standard 2-phased constructor
        * @return pointer to new context.
        */ 
        IMPORT_C static CSenServerContext* NewL(MSenCoreServiceManager& aCore);
        /**
        * Standard 2-phased constructor 
        * @return pointer to new context,
        * which is left on cleanupstack.
        */ 
        IMPORT_C static CSenServerContext* NewLC(MSenCoreServiceManager& aCore);

        // Destructor:
        IMPORT_C virtual ~CSenServerContext();
        
        // New methods:
        //TInt AddApplicationContext(CSenApplicationContext& aCtx); // takes ownership
        
        // This method either creates a new ctx or returns existing one.
        // When application context is created, it is added to server context.
        virtual CSenApplicationContext& OpenApplicationContextL(TSecureId aSecureId);
        
        // Closes the application context, but ONLY if it does not have any clients
        // When application context is closed, it is removed from server context.
        virtual TInt CloseApplicationContext(const CSenApplicationContext& aCtx);
        
        // Getter for core
        virtual MSenCoreServiceManager& Core();
        
    protected:
        IMPORT_C  CSenServerContext(MSenCoreServiceManager& aCore);
                                     
        IMPORT_C void BaseConstructL();

    private: // Data
        RApplicationCtxMap iApplications;
        MSenCoreServiceManager& iCore;
    
    };

#endif // SEN_SERVER_CONTEXT_H

// End of File
