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








#ifndef WSOviPub_VALIDATE_HANDLER_H
#define WSOviPub_VALIDATE_HANDLER_H

#include <e32std.h>

// FORWARD DECLARATIONS

// CONSTANTS


// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "sensessionhandler.h"
#include "wsovihandlercontext.h"
#include "wsovisessioncontext.h"
#include "SenIdentityProvider.h"
#include "wsovioauthclient.h"
#include "wsovicons.h"
#include "wsoviservicesession.h"



/**
 * Class:       
 *
 * Description: An implementation of the CWSOviHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 */
class CWSOviValidateHandler : public CSenSessionHandler
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
    static CWSOviValidateHandler* NewL(TAny* aHandlerCtx);

    /**
    * InvokeL is the most emergent method in message handler interface.
    * Each sub-class must implement this method thus providing message
    * processing routine.
    * @param aCtx is the session context that will be accessed and 
    * typically altered by implemented handler, in order to provide
    * modifications on the layer of the session construction, validation,
    * or some other, focused area that the handler was intended to take
    * responsibility for. For example, certain handler can itself create
    * a session of its own, send message(s) to (authentication) service
    * (invoking other handlers), and finally access session context in 
    * order to update credentials. 
    */
    virtual TInt InvokeL(MSenSessionContext& aCtx);
    ~CWSOviValidateHandler();
    virtual SenHandler::THandlerDirection Direction() const;
    virtual SenHandler::THandlerPhase Phase();
    virtual TInt InitL(MSenHandlerContext& aCtx);
    RFileLogger* Log() const;
    MSenHandlerContext& HandlerContext();    
protected:
    /**
     * Function:   CWSOviValidateHandler
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CWSOviValidateHandler(MSenHandlerContext& aCtx);
    
    /**
    * Obtain or construct a valid SecurityTokenServiceClient. Ask
    * owned ServiceManager for ServiceDescriptions that match
    * Authentication server contract & ProviderID.
    *
    * @param providerID
    * @return SecurityTokenServiceClient
    */
    CWSOviOAuthClient* AuthClientL(CSenIdentityProvider*& aIdentityProvider);
    
    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();

private:
    /**
     * Function:   IdentityProviderFromCoreL
     *
     * Discussion: copmplex getter. It asking core for IdentitProvider by using a pattern.
     */
    
    CSenIdentityProvider* IdentityProviderFromCoreL(const TDesC8& aProviderID);
    const TDesC8* iVersion;
    };


#endif // WSOvi_VALIDATE_HANDLER_H

