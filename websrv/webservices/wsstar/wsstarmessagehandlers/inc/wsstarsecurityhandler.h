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








#ifndef WSSTAR_SECURITY_HANDLER_H
#define WSSTAR_SECURITY_HANDLER_H

#include <e32std.h>

#include "SenDomFragmentBase.h"
#include "senmessagehandler.h"
#include "SenElement.h"


class RFileLogger;


/**
 * Class:       
 *
 * Description: An implementation of the CWSStarHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler sign elemetns in Envelope according to policy which is delivered.
 */
 
class CWSStarSecurityHandler : public CSenMessageHandler
    {

typedef RArray<TXmlEngElement> RArrayElements;

public:
    static CWSStarSecurityHandler* NewL(MSenHandlerContext* aHandlerCtx);
    virtual TInt InvokeL(MSenMessageContext& aCtx);
    virtual TInt InitL(MSenHandlerContext& aCtx);    
    ~CWSStarSecurityHandler();
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    
private:
    CSenDomFragmentBase* GetElementsToSignL(MSenMessageContext& aCtx,
            RArrayElements* aElements);
    void AttachElementL(MSenMessageContext& aCtx, TXmlEngElement& aSignature);

private:
    /**
     * Function:   CWSStarSecurityHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSStarSecurityHandler(MSenHandlerContext& aCtx);
    
private:

    RFileLogger* Log() const;
    };


#endif // WSSTAR_SECURITY_HANDLER_H

