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








#ifndef SEN_CLIENT_CONTEXT_H
#define SEN_CLIENT_CONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
//#include "SenContext.h"
#include "senpointermap.h"

// FORWARD DECLARATIONS
class CSenSessionContext;
class CSenApplicationContext;
class CSenClientSession;

// TYPE DEFS
typedef RSenPointerMap<TInt, CSenSessionContext> RSessionCtxMap; 


// CLASS DECLARATION
class CSenClientContext : public CBase // CSenContext
    {
    friend class CSenApplicationContext;
    friend class CSenClientSession;
    public:
        // Destructor:
        IMPORT_C virtual ~CSenClientContext();
        
        // New methods:
        virtual TInt ClientId() const;
        
    protected:
        IMPORT_C  CSenClientContext(TInt aClientId, CSenApplicationContext& aCtx);
                                     
        IMPORT_C void BaseConstructL();

    private: 
        /**
        * Standard 2-phased constructor
        * @return pointer to new context.
        */ 
        IMPORT_C static  CSenClientContext* NewL(TInt aClientId, CSenApplicationContext& aCtx);
        /**
        * Standard 2-phased constructor 
        * @return pointer to new context,
        * which is left on cleanupstack.
        */ 
        IMPORT_C static  CSenClientContext* NewLC(TInt aClientId, CSenApplicationContext& aCtx);

    private: // Data
        TInt iClientId;
        CSenApplicationContext& iApplicationContext;
        RSessionCtxMap iSessions;
    };

#endif // SEN_CLIENT_CONTEXT_H

// End of File
