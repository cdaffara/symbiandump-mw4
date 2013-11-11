/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef WSSTAR_CREDENTIAL_COLLECTOR_HANDLER_H
#define WSSTAR_CREDENTIAL_COLLECTOR_HANDLER_H

#include <e32std.h>

// FORWARD DECLARATIONS

// CONSTANTS


// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "wsstarmessagehandlerscons.h"
#include "wsstarmessagecontext.h"
#include "wsstarhandlercontext.h"
#include "senmessagehandler.h"
#include "SenSoapMessage.h"
#include "sendebug.h"
#include "senserviceinvocationframework.h"

/**
 * Class:       
 *
 * Description: An implementation of the CWSStarHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler set mobile ticket in security section in header.
 */
class CWSStarCredentialCollectorHandler : public CSenMessageHandler
    {

public:
    static CWSStarCredentialCollectorHandler* NewL(TAny* aHandlerCtx);
    virtual TInt InvokeL(MSenMessageContext& aCtx);
    virtual TInt InitL(MSenHandlerContext& aCtx);    
    ~CWSStarCredentialCollectorHandler();
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    
protected:
    /**
     * Function:   CWSStarCredentialCollectorHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSStarCredentialCollectorHandler(MSenHandlerContext& aCtx);
    
    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();

private:

    RFileLogger* Log() const; 
    };


#endif // WSSTAR_CREDENTIAL_COLLECTOR_HANDLER_H

