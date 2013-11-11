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











#ifndef WSSTAR_PASSPORT_HANDLER_H
#define WSSTAR_PASSPORT_HANDLER_H

#include <e32std.h>
#include <flogger.h>
// FORWARD DECLARATIONS

// CONSTANTS


// INCLUDES

#include "senmessagehandler.h"
#include "wsstarmessagecontext.h"
#include "SenSoapMessage.h"
#include "sendebug.h"
#include "wsstarhandlercontext.h"
#include "wsstarmessagehandlerscons.h"

/**
 * Class:       
 *
 * Description: An implementation of the CWSStarHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler creates add passport speciifc headers.
 *  Also create multi batched request.
 */

class CWSStarPassportHandler : public CSenMessageHandler
    {
public:
   static CWSStarPassportHandler* NewL(TAny* aHandlerCtx);


    virtual TInt InvokeL(MSenMessageContext& aCtx);
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    virtual TInt InitL(MSenHandlerContext& aCtx);
 
public: // destructor
    virtual ~CWSStarPassportHandler();


protected:
    /**
     * Function:   CWSStarPassportHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSStarPassportHandler(MSenHandlerContext& aCtx);

    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();

private:
    TInt HandleInboundL(MSenMessageContext& aCtx, CSenSoapEnvelope* aMessage);

private:
    RFileLogger* Log() const;    
    };

#endif // WSSTAR_PASSPORT_HANDLER_H

