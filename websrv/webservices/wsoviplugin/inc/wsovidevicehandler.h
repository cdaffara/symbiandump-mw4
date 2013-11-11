/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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









#ifndef WSOVI_ENVELOPER_HANDLER_H
#define WSOVI_ENVELOPER_HANDLER_H

#include <e32std.h>

// FORWARD DECLARATIONS

// CONSTANTS


// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "wsovimessagecontext.h"
#include "wsovihandlercontext.h"
#include "senmessagehandler.h"
#include "sendebug.h"
#include "senserviceinvocationframework.h"

/**
 * Class:       
 *
 * Description: An implementation of the CWSOviHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler creates proper Envelope based on provided policy (by message context)
 */
class CWSOviDeviceHandler : public CSenMessageHandler
    {

public:
    static CWSOviDeviceHandler* NewL(TAny* aHandlerCtx);
    virtual TInt InvokeL(MSenMessageContext& aCtx);
    ~CWSOviDeviceHandler();
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    virtual TInt InitL(MSenHandlerContext& aCtx);
    
        
protected:
    /**
     * Function:   CWSOviDeviceHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSOviDeviceHandler(MSenHandlerContext& aCtx);
    
    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();

private:

    HBufC8* iNAuthVersion;
    };

#endif // WSOVI_ENVELOPER_HANDLER_H

