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








#ifndef SEN_APPLICATION_CONTEXT_H
#define SEN_APPLICATION_CONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "e32cmn.h"

//#include "SenContext.h"
#include "senpointermap.h"

#include "senclientcontext.h"

// FORWARD DECLARATIONS
class CSenServerContext;
//class CSenClientContext;

// TYPE DEFS
typedef RSenPointerMap<TInt, CSenClientContext> RClientCtxMap; 

// CLASS DECLARATION
class CSenApplicationContext : public CBase // CSenContext
    {
    friend class CSenServerContext;
        
    public:
        // Destructor:
        IMPORT_C virtual ~CSenApplicationContext();
        
        // New methods:
        
        virtual CSenServerContext& ServerContext() const;
        
        virtual TSecureId SecureId() const;
        virtual CSenClientContext& OpenClientContextL(TInt aClientId);
        virtual TInt CloseClientContext(const CSenClientContext& aCtx);
        
    protected:
        IMPORT_C  CSenApplicationContext(TSecureId aSecureId, CSenServerContext& aCtx);
                                     
        IMPORT_C void BaseConstructL();

    private: 
        /**
        * Standard 2-phased constructor
        * @return pointer to new context.
        */ 
        IMPORT_C static  CSenApplicationContext* NewL(TSecureId aSecureId, CSenServerContext& aCtx);
        
        /**
        * Standard 2-phased constructor 
        * @return pointer to new context,
        * which is left on cleanupstack.
        */ 
        IMPORT_C static  CSenApplicationContext* NewLC(TSecureId aSecureId, CSenServerContext& aCtx);
        
    private: // data
        TSecureId iSecureId;
        CSenServerContext& iServerContext; 
        RClientCtxMap iClients;       
    };

#endif // SEN_APPLICATION_CONTEXT_H

// End of File
