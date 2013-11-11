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








#ifndef WSSTAR_SERVICEUPDATE_HANDLER_H
#define WSSTAR_SERVICEUPDATE_HANDLER_H

#include <e32std.h>

// FORWARD DECLARATIONS

// CONSTANTS


// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "sensessionhandler.h"
#include "wsstarhandlercontext.h"
#include "wsstarsessioncontext.h"
#include "wsstarcons.h"


/**
 * Class:CWSStarServiceUpdateHandler
 *
 * An implementation of the CWSStarSessionHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 * This handler is responsible to updated mobile ticket and use session context token.
 *
 */
class CWSStarServiceUpdateHandler : public CSenSessionHandler
    {

public:
    /**
     * Function:    NewL
     *
     * Description: Create instance of concrete implementation. Note that ECOM
     *              interface implementations can only have two signatures for
     *              NewL:
     *               - NewL without parameters (used here)
     *               - NewL with TAny* pointer, which may provide some client
     *                 data
     *
     * Returns:    Instance of this class.
     *
     * Note:       The interface, which is abstract base class of this
     *             implementation, also provides NewL method. Normally abstract
     *             classes do not provide NewL, because they cannot create
     *             instances of themselves.
     */

    static CWSStarServiceUpdateHandler* NewL(TAny* aHandlerCtx);
    virtual TInt InvokeL(MSenSessionContext& aCtx);
    ~CWSStarServiceUpdateHandler();
    
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    virtual TInt InitL(MSenHandlerContext& aCtx);
        
protected:
    /**
     * Function:   CWSStarServiceUpdateHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSStarServiceUpdateHandler(MSenHandlerContext& aCtx);
    
    
    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();
    CSenElement* GetSCTElementL(CSenElement& aParent);


private:
    RFileLogger* Log() const;
    const TDesC8* iVersion;
    };


#endif // WSSTAR_SERVICEUPDATE_HANDLER_H

