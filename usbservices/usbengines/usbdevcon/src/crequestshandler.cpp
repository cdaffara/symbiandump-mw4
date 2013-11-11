/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Requests handler
*
*/



#include <ecom/ecom.h>
#include <cusbdevicecontrolplugin.h>

#include "cusbdevcon.h"
#include "crequestshandler.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CRequestsHandler* CRequestsHandler::NewL(RDevUsbcClient& aLdd,
                                         RUsbWatcher& aUsbWatcher,
                                         RUsb& aUsbManager)
    {
    
    FLOG( _L( "[USBDEVCON]\tCRequestsHandler::NewL" ) );

    CRequestsHandler* self = new (ELeave) CRequestsHandler(aLdd, aUsbWatcher, aUsbManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
// -------------------------------------Handle--------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//  
void CRequestsHandler::ConstructL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCRequestsHandler::ConstructL" ) );
    
    // read, construct and attach handlers
    
    RImplInfoPtrArray implementations;
    const TEComResolverParams noResolverParams;
    REComSession::ListImplementationsL(KUsbCMHandlerInterface, noResolverParams, KRomOnlyResolverUid, implementations);
    
    for (TUint i(0); i < implementations.Count(); ++i)
        {
        TUid uid(implementations[i]->ImplementationUid());
        CUsbCMHandler* handler = (reinterpret_cast<CUsbCMHandler*>(REComSession::CreateImplementationL
                                                                                                    (uid, _FOFF(CUsbCMHandler, iPrivateEComUID))));
        iHandlers.AppendL(handler);
        }
       
   implementations.Close(); // cleanup
        
   FTRACE(FPrint(
           _L("[USBDEVCON]\tCRequestsHandler::ConstructL: Amount of handlers attached = %d" ), iHandlers.Count()));

    // initialize handlers, pass them iLdd, iUsbWatcher, iUsbManager
    for(TInt i(0); i < iHandlers.Count(); ++i)
        {
        iHandlers[i]->Initialize(iLdd, iUsbWatcher, iUsbManager);
        }
    }
    
// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//
CRequestsHandler::CRequestsHandler( RDevUsbcClient& aLdd, 
                                    RUsbWatcher& aUsbWatcher,
                                    RUsb& aUsbManager) : 
                            iLdd (aLdd),
                            iUsbWatcher(aUsbWatcher),
                            iUsbManager(aUsbManager)
    {
    }

// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CRequestsHandler::~CRequestsHandler()
    {

    FLOG( _L( "[USBDEVCON]\tCRequestsHandler::~CRequestsHandler()" ) );
    // destroy handlers  
    iHandlers.ResetAndDestroy();
    
    REComSession::FinalClose(); // plug-ins final cleanup
    
    }   


// ---------------------------------------------------------------------------
// Search plugin which can handle the request
// ---------------------------------------------------------------------------
//
TInt CRequestsHandler::Handle(const RBuf8& aSetupPacket, RBuf8& aData)
    {
    
    FLOG( _L( "[USBDEVCON]\tCRequestsHandler::Handle" ) );
    
    TInt err(KErrNotSupported);
    TUint counter(0); // handlers counter
    
    // loop through handlers, call handle() while error == KErrNotSupported
    while((err == KErrNotSupported) && (counter < iHandlers.Count()))               
        {
        err = iHandlers[counter]->Handle(aSetupPacket, aData);
        ++counter;
        }
        
    return err;
    
    }
    


