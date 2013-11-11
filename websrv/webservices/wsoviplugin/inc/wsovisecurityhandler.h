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








#ifndef WSOVI_SECURITY_HANDLER_H
#define WSOVI_SECURITY_HANDLER_H

#include <e32std.h>

#include "senmessagehandler.h"
#include "senserviceinvocationframework.h"


/**
 * Class:       
 *
 * Description: An implementation of the CWSOviHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler sign NAuth message.
 */
 
class CWSOviSecurityHandler : public CSenMessageHandler
    {

typedef RArray<TXmlEngElement> RArrayElements;

public:
    static CWSOviSecurityHandler* NewL(MSenHandlerContext* aHandlerCtx);
    virtual TInt InvokeL(MSenMessageContext& aCtx);
    virtual TInt InitL(MSenHandlerContext& aCtx);    
    ~CWSOviSecurityHandler();
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
private:
    /**
     * Function:   CWSOviSecurityHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSOviSecurityHandler(MSenHandlerContext& aCtx);
    TTime iDiff;
    };


#endif // WSOVI_SECURITY_HANDLER_H

