/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:           
*
*/











// INCLUDE FILES
#include "sensyncrequester.h"
#include "msentransport.h"
#include "msencoreservicemanager.h" // internal: Core\inc
#include "senidentifier.h"
#include "senguidgen.h"             // internal: Utils\inc
#include "MSenHostlet.h"
#include "MSenProperties.h"

EXPORT_C CSenSyncRequester* CSenSyncRequester::NewL(MSenTransport& aTransport,
                                                    MSenCoreServiceManager& aManager)
    {
    CSenSyncRequester* pNew = CSenSyncRequester::NewLC(aTransport, aManager);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenSyncRequester* CSenSyncRequester::NewLC(MSenTransport& aTransport,
                                                     MSenCoreServiceManager& aManager)
    {
    CSenSyncRequester* pNew = new (ELeave) CSenSyncRequester(aTransport, aManager);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenSyncRequester::~CSenSyncRequester()
    {
    delete ipResponse;
    delete ipResponseProperties;
    delete ipConsumerId;
    delete ipIdentifier;
    }

EXPORT_C CSenSyncRequester::CSenSyncRequester(MSenTransport& aTransport,
                                     MSenCoreServiceManager& aManager)

:   iTransport(aTransport),
    iServiceManager(aManager),
    ipIdentifier(NULL),
    ipConsumerId(NULL),
    ipResponse(NULL),
    ipResponseProperties(NULL),
    iResponsePropertiesType(MSenProperties::ESenXmlProperties),
    iStatusCode(KErrNone)
    {
    }

EXPORT_C void CSenSyncRequester::BaseConstructL()
    {
    ipConsumerId =  iServiceManager.RandomGuidL();
    ipIdentifier = CSenIdentifier::NewL(SenIdentifier::ESession, *ipConsumerId);
    }

TInt CSenSyncRequester::SubmitL(const TDesC8& aEndpointUri,
                                const TDesC8& aMessage,
                                const TDesC8& aRequestTransportProperties,
                                HBufC8*& apResponseTo)
    {
    iStatusCode = KErrNone;
    TInt txnId(KErrNone);
    TInt retCode = iTransport.SendL(aEndpointUri,
                                    aMessage,
                                    aRequestTransportProperties,
                                    *this,
                                    *this,
                                    txnId);
                                    
    if(retCode==KErrNone)
        {
        // Wait here until a callback from transport is received:
        iSchedulerWait.Start();
    
        // Now read the response and return code from member variables,
        // and assign them to the references passed by the SubmitL caller:

        apResponseTo = ipResponse;
        ipResponse = NULL;
        //apResponseProperties = ipResponseProperties; // transport properties of the response.
        //ipResponseProperties = NULL;

        // On success, always return the transaction ID received from the transport:
        return txnId;
        }                                    
    else // error occurred
        {
        // SendL failed, return the error immediately:
        return retCode;
        }
    }


TInt CSenSyncRequester::SendToConsumerL(HBufC8* apMessage,
                                        const TInt aTxnId,
                                        MSenRemoteServiceConsumer& aConsumer,
                                        MSenProperties* aResponseTransportProperties)
    {
    // SendToConsumerL method will be changed to provide reference to 
    // the transport properties which describe the RESPONSE, not request..
    return aConsumer.HandleMessageL(apMessage, aTxnId, aResponseTransportProperties);
    }

TInt CSenSyncRequester::SendErrorToConsumerL(const TInt aErrorCode,
                                             HBufC8* apError,
                                             const TInt aTxnId,
                                             MSenRemoteServiceConsumer& aConsumer,
                                             MSenProperties* aResponseTransportProperties)
    {
    // SendErrorToConsumerL method will be changed to provide reference to 
    // the transport properties which describe the RESPONSE, not request..
    return aConsumer.HandleErrorL(apError, aErrorCode, aTxnId, aResponseTransportProperties);
    }

// Note: this method is meant for those SESSION implementations, which
// actually are capable of handling certain errors between transport
// and session.
TInt CSenSyncRequester::HandleErrorL(const TInt aErrorCode,
                                     HBufC8* apError,
                                     const TInt /*aTxnId*/,
                                     MSenRemoteServiceConsumer& /*aConsumer*/,
                                     MSenProperties* /*aResponseTransportProperties*/)
    {
    // NONE of the errors between transport and session are handled in this level:
    if(apError)
    	{
    	delete apError;
    	apError = NULL;
    	}
    return aErrorCode; // simply bounce the error back to the caller
    }

const TDesC8& CSenSyncRequester::Id() const
    {
    return *ipConsumerId;
    }

TInt CSenSyncRequester::SetSessionL(MSenRemoteServiceSession& /*aServiceSession*/)
    {
    return KErrNone;
    }

// From MSenRemoteServiceConsumer
TInt CSenSyncRequester::HandleMessageL(HBufC8* apMessage,
                                       const TInt aTxnId,
                                       MSenProperties* aResponseTransportProperties)
    {
    iStatusCode = KErrNone;
    ipResponse = apMessage;
    if( aResponseTransportProperties ) 
        {
        iResponsePropertiesType = aResponseTransportProperties->PropertiesClassType();
        ipResponseProperties = aResponseTransportProperties->AsUtf8L();
        }
    // Now stop the SYNC wait in the SubmitL, so that the main thread 
    // of the Serene Core Active Scheduler can continue it's execution
    if(iSchedulerWait.IsStarted())
    	iSchedulerWait.AsyncStop();
    return aTxnId;
    }

// From MSenServiceSession
TInt CSenSyncRequester::HandleErrorL(HBufC8* apError,
                                     const TInt aErrorCode,
                                     const TInt aTxnId,
                                     MSenProperties* aResponseTransportProperties)
    {
    iStatusCode = aErrorCode;
    ipResponse = apError;
    if( aResponseTransportProperties ) 
        {
        iResponsePropertiesType = aResponseTransportProperties->PropertiesClassType();
        ipResponseProperties = aResponseTransportProperties->AsUtf8L();
        }
    // Now stop the SYNC wait in the SubmitL, so that the main thread 
    // of the Serene Core Active Scheduler can continue it's execution
    if(iSchedulerWait.IsStarted())
    	iSchedulerWait.AsyncStop();
    return aTxnId;
    }

void CSenSyncRequester::SetStatusL(const TInt /*aStatus*/)
    {
    // not implemented
    }

MSenTransport& CSenSyncRequester::TransportL()
    {
    return  (MSenTransport&)iTransport;
    }

CSenIdentifier& CSenSyncRequester::Identifier() const
    {
    return *ipIdentifier;
    }
  

TInt CSenSyncRequester::ChunkByTxnIdL(TInt /* aTxnId */, CSenChunk*& /* aChunk */)
    {
    return KErrNotSupported;
    }

MSenRemoteHostlet* CSenSyncRequester::Hostlet() const
    {
    return NULL;
    }

TInt CSenSyncRequester::SendToHostletL(MSenRemoteHostlet& /* aReceiver */,
                                       const TDesC8& /* aMessage */,
                                       const TInt /* aTxnId */,
                                       MSenRemoteServiceConsumer& /* aFrom */,
                                       MSenProperties* /* aProperties */)
    {
    return KErrNotSupported;
    }

void CSenSyncRequester::FileProgress(TInt /*aTxnId*/, TBool /*aIncoming*/, TBool /*aIsSoap*/,
                const TDesC8& /*aSoapOrCid*/, TInt /*aProgress*/) 
    {
    }
void CSenSyncRequester::SetDataTrafficDetails( TSenDataTrafficDetails& /*aDetails*/)		
	{
	}
//  End of File

