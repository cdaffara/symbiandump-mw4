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
* Description:    Header declaration
*
*/











#ifndef WSSTAR_ADDRESSING_HANDLER_H
#define WSSTAR_ADDRESSING_HANDLER_H

#include <e32std.h>
#include <flogger.h>
// FORWARD DECLARATIONS

// CONSTANTS


// INCLUDES

#include "senmessagehandler.h"
#include "wsstarmessagehandlerscons.h"
#include "senguidgen.h"
#include "wsstarmessagecontext.h"
#include "SenSoapMessage.h"
#include "wsstaraddressingheaderinbound.h"
#include "SenSoapFault.h"
#include "sendebug.h"
#include "senserviceinvocationframework.h"
#include "wsstarhandlercontext.h"
#include "wsstaraddressingepr.h"

/**
 * Class:       
 *
 * Description: An implementation of the CWSStarHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler is responsible for ws-addressing layes in ws* framework.
 *  Works in both direction:
 *       - in inbound case fill porperly message context, or recognize addressing faults
 *       - in outbound case create specific headers in envelope
 */
class CWSStarAddressingHandler : public CSenMessageHandler
    {
public:
    static CWSStarAddressingHandler* NewL(TAny* aHandlerCtx);


    virtual TInt InvokeL(MSenMessageContext& aCtx);
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    virtual TInt InitL(MSenHandlerContext& aCtx);
    virtual ~CWSStarAddressingHandler();
    
protected:
    /**
     * Function:   CWSStarAddressingHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSStarAddressingHandler(MSenHandlerContext& aCtx);

    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();

    CSenElement* CreateElementAddressingL(CSenElement& aParentElement, const TDesC8& aLocalName,
                                  const TDesC8& aContent, TBool aUseAddress); 
    void SetMessageIDL(MSenMessageContext& aCtx);
    void ShareMyLiterals(MSenMessageContext& aCtx);
    void AddAppInfoHeaderL(  MSenMessageContext& aCtx, 
                            CSenElement& aHeader);
private:
//methods
    virtual TInt HandleInboundL(MSenMessageContext& aCtx, CSenSoapEnvelope* aMessage);
    virtual TInt HandleOutboundL(MSenMessageContext& aCtx, CSenSoapEnvelope* aMessage);
    void InitVersionL(const TDesC8* aVersion);
    RFileLogger* Log() const;    
    HBufC8* iVersion;//owned
    };

#endif // WSSTAR_ADDRESSING_HANDLER_H

