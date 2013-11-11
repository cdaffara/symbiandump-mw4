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








#ifndef WSOvi_REGISTER_HANDLER_H
#define WSOvi_REGISTER_HANDLER_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "sensessionhandler.h"
#include "wsovihandlercontext.h"
#include "wsovisessioncontext.h"
#include "wsovicons.h"
#include "wsoviservicesession.h"
#include "SenServiceConnection.h"


/**
 * Class:CWSOviRegisterHandler
 *
 * An implementation of the CWSOviSessionHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 *
 * Class represent phase related to session, before discovering and validation.
 *      Main goal is to indirectly implement (by passing session context) SIF method:
 *          -registegr/unregister service description
 */
class CWSOviRegisterHandler : public CSenSessionHandler
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

        static CWSOviRegisterHandler* NewL(TAny* aHandlerCtx);
        virtual TInt InvokeL(MSenSessionContext& aCtx);
        ~CWSOviRegisterHandler();
        
        virtual SenHandler::THandlerDirection Direction() const;
        virtual SenHandler::THandlerPhase Phase();
        virtual TInt InitL(MSenHandlerContext& aCtx);
        
            
    protected:
        /**
         * Function:   CWSOviRegisterHandler
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSOviRegisterHandler(MSenHandlerContext& aCtx);
        
        
        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();


    private:
        /**
         * Attempt to register the ServiceDescription. 
         *   In fact session is created but not vaidated and added into CoreManager.
         *   So registering different to adding that it doesnt validate, just register.
         *   Common issue is increase core database by one session (validated or not)
         *
         * @param aServiceDescription A ServiceDescription
         *
         *  return TInt aError - indicates the error or KErrNone if successful
         */
        virtual TInt RegisterServiceDescriptionL(
                                    MSenServiceDescription& aServiceDescription);


        /**
         * Attempt to unregister the ServiceDescription from the ServiceManager
         * that owns the framework.
         * This is a callback used by the ServiceManager.
         *
         * @param aServiceDescription A ServiceDescription
         *
         *  return TInt aError - indicates the error or KErrNone if successful
         */
        virtual TInt UnregisterServiceDescriptionL(
                                    MSenServiceDescription& aServiceDescription);

        /**
         * Register specific description (NAUTH contract)
         */
        TInt RegisterAuthClientL(MSenServiceDescription *aServiceDescription);
        
        /**
         * Unregister specific description (NAUTH contract)
         */
        TInt UnRegisterAuthClientL(MSenServiceDescription* aServiceDescription);
    };


#endif // WSOvi_HANDLER_REGISTER_PLUGIN_H

