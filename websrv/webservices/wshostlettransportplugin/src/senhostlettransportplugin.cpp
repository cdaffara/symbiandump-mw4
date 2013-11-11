/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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











#include "senhostlettransportplugin.h"

#include <SenServiceConnection.h>   // errorcode constants, like KErrSenNoEndpoint

#include "msenprovider.h"
#include "senhostletrequest.h"      // internal Provider\inc
#include "senhostletresponse.h"     // internal Provider\inc

#include "senidentifier.h"          // internal Utils\inc
#include "senidentifier.h"          // internal Utils\inc
#include "sendebug.h"               // internal Utils\inc - debugging MACROs

#include "senprovider.h"            // internal Framework\inc
#include "sensyncrequester.h"       // internal Framework\inc
#include <SenTransportProperties.h> // internal Framework\inc
#include "senservicesession.h"      // internal Framework\inc
#include "msentransport.h"          // internal Framework\inc
#include "msenremotehostlet.h"      // internal Framework\inc

#include "msencoreservicemanager.h" // internal: Core\inc

#include "senchunk.h"               // Internal Utils\inc
#include "senlogger.h"              // internal Logger\inc - logging MACROs

#include "msenservicesession.h"
#include "msenremoteserviceconsumer.h"

#include "senlayeredtransportproperties.h"


#include "senvtcptransportproperties.h"
#include "SenHttpTransportProperties.h"

namespace
    {
    }


CSenHostletTransportPlugin* CSenHostletTransportPlugin::NewL(CSenTransportContext* apCtx)
    {
    CSenHostletTransportPlugin* pNew = NewLC(apCtx);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenHostletTransportPlugin* CSenHostletTransportPlugin::NewLC(CSenTransportContext* apCtx)
    {

    const CSenServiceSession* pSession = apCtx->GetSession();
    if( pSession == NULL ||  pSession->Hostlet() == NULL) 
        {
        User::Leave( KErrSenHostNotAvailable );
        }
    CSenHostletTransportPlugin* pNew = new (ELeave) CSenHostletTransportPlugin(apCtx);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenHostletTransportPlugin::CSenHostletTransportPlugin(CSenTransportContext* apCtx)
:   CSenTransport(apCtx),
	ipRequester(NULL),
    iInvokedProvider(NULL),
    iStatusCode(0),
    iTransactionID(0),
//    iConsumerMap(EFalse, ETrue), 
//    iSessionMap(EFalse, EFalse), // iConsumerMap deletes the txnIds (using same "new reserved" TInt here!)
    iRequestMap(EFalse, ETrue), // CSenActiveHostletRequest (keys) objects are NOT owned!
    ipLayeredXmlProperties(NULL)
    {
    }

CSenHostletTransportPlugin::~CSenHostletTransportPlugin()
    {
    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "Log file closed.");
    TLSLOG_CLOSE( KHostletTransportLogChannel );

    TInt count( iRequestMap.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        CSenActiveHostletRequest* pReq = iRequestMap.KeyAt(i);
        if( pReq )
            {
            pReq->Deque();
            }
        }
    iRequestMap.Reset(); 
    delete ipRequester;
    delete iInvokedProvider;
    delete ipLayeredXmlProperties;
    }

void CSenHostletTransportPlugin::ConstructL()
    {
    // Open connection to the file logger server
    TLSLOG_OPEN(KHostletTransportLogChannel, KHostletTransportLogLevel, KLogDir, KLogFile);
    
    //LOG_CONNECT;
    // LOG_CREATELOG((KLogDir, KLogFile, EFileLoggingModeOverwrite));
    //TLSLOG_L(KHostletTransportLogChannel, KHostletTransportLogLevel,  "CSenHostletTransportPlugin::ConstructL(): log file opened.");
    MSenCoreServiceManager& core = Context().GetCoreL();
    ipRequester = CSenSyncRequester::NewL(*this, core);
    ipLayeredXmlProperties = CSenLayeredTransportProperties::NewL();    
    }

TInt CSenHostletTransportPlugin::SubmitL(const TDesC8& aEndpoint,
                                       const TDesC8& aMessage,
                                       const TDesC8& aTransportProperties,
                                       HBufC8*& apResponse,
                                       MSenRemoteServiceConsumer& /*aConsumer*/)
    {
    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "CSenHostletTransportPlugin::SubmitL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "- No endpoint!");
        return KErrSenNoEndpoint; // from SenServiceConnection.h (public API)
        }
    
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
//    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "- Message:");
//    TLSLOG_ALL((KHostletTransportLogChannel, KHostletTransportLogLevel,  aMessage ));
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMaxLogLevel, _L8("- Message: %S"), &aMessage));

    TInt leaveCode(KErrNone);
    TInt statusCode(KErrNone);

    /*
    statusCode = ipRequester->SubmitL(aEndpoint,            // endpoint
                                      aMessage,             // request body
                                      aTransportProperties, // request props
                                      apResponse);
    */
    
    TRAP( leaveCode, statusCode = ipRequester->SubmitL(aEndpoint,            // endpoint
                                                       aMessage,             // request body
                                                       aTransportProperties, // request props
                                                       apResponse); )         // response body
    
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L8("CSenHostletTransportPlugin::SubmitL returned: (%d)"), statusCode));

    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L8("CSenHostletTransportPlugin::SubmitL leaved: (%d)"), leaveCode));
        if(statusCode==KErrNone)
            {
            statusCode = leaveCode;
            }
        }
    else // did not leave
        {
        if(!apResponse)
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "After calling CSenHostletTransportPlugin::SubmitL => response==NULL => leaving!");
            User::Leave(KErrCorrupt);
            }
        TLSLOG_L(KHostletTransportLogChannel, KMaxLogLevel,  "CSenHostletTransportPlugin::SubmitL response:");
//        TLSLOG_ALL((KHostletTransportLogChannel, KHostletTransportLogLevel, *apResponse));
        TLSLOG_FORMAT((KHostletTransportLogChannel, KMaxLogLevel, _L8("- Response: %S"), apResponse));
        }
                                        
    return statusCode;
    }

/*
TInt CSenHostletTransportPlugin::SendL(const TDesC8& aEndpoint,
                                     const TDesC8& aMessage,
                                     const TDesC8& aTransportProperties,
                                     MSenServiceSession& aReplyTo,
                                     MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                                     TInt& aTxnId)
    {
    TLSLOG_L(KHostletTransportLogChannel, KHostletTransportLogLevel,  "CSenHostletTransportPlugin::SendL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KHostletTransportLogChannel, KHostletTransportLogLevel,  "- No endpoint!");
        return KErrSenNoEndpoint; // from SenServiceConnection.h (public API)
        }
    
    TLSLOG_FORMAT((KHostletTransportLogChannel, KHostletTransportLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
    TLSLOG_L(KHostletTransportLogChannel, KHostletTransportLogLevel,  "- Message:");
//    TLSLOG_ALL((KHostletTransportLogChannel, KHostletTransportLogLevel,  aMessage ));
    
//    MSenCoreServiceManager& core = Context().GetCoreL();
    //const CSenServiceSession* pSession = Context().GetSession();
    //if( pSession == NULL ||  pSession->Hostlet() == NULL) 

    // NOTE: this solution requires following line in CSenClientSession::SendMsgL:
///    iAllowShowDialogCount++;
///    TInt transactionId( KErrNotFound );
///    pSenChunk->ChunkHeader().SetTransactionId( transactionId ); 
///    TInt sendRetCode(KErrNone);


    if(aReplyTo.Hostlet() == NULL) 
        {
        User::Leave( KErrSenHostNotAvailable );
        }
        
    CSenChunk* pChunk = NULL;
    TInt retVal = aConsumer.ChunkByTxnIdL(aTxnId, pChunk);
    if ( retVal >= KErrNone )
        {
        iTransactionID++;
        TInt* txnID = new (ELeave) TInt(iTransactionID);
        aTxnId = iTransactionID;
        pChunk->ChunkHeader().SetTransactionId(aTxnId);

        MSenRemoteHostlet* pHostlet = aReplyTo.Hostlet();
        TInt retVal = pHostlet->ProcessRequestFromConsumerL(aMessage, aTxnId, aConsumer);

        TLSLOG_FORMAT((KHostletTransportLogChannel, KHostletTransportLogLevel, _L("- ProcessRequestFromConsumerL returned: %d"), retVal ));
        // No leave occurred

        if( retVal == KErrNone )
            {
            TInt retVal = iSessionMap.Append(&aReplyTo, txnID);
            if( retVal==KErrNone )
                {
                retVal = iConsumerMap.Append(&aConsumer, txnID);
                if(retVal!=KErrNone)
                    {
                    TLSLOG_FORMAT((KHostletTransportLogChannel, KHostletTransportLogLevel, 
                        _L("- iConsumerMap.Append failed: %d"), retVal ));

                    // Last entry to iSessionMap must be removed
                    TInt removeRetVal = iSessionMap.Remove(*txnID);
#ifdef _SENDEBUG
                    if(removeRetVal==KErrNotFound)
                        {
                        TLSLOG_FORMAT((KHostletTransportLogChannel, KHostletTransportLogLevel, 
                            _L("- MAJOR: Could not remove session by txnId(%d)"), *txnID));
                        }
#endif // _SENDEBUG
                    removeRetVal = 0; // not used in release builds
                    }
                }
            else
                {
                TLSLOG_FORMAT((KHostletTransportLogChannel, KHostletTransportLogLevel, _L("- FATAL: iConsumerMap.Append failed: %d"), retVal ));
                }
            }
        }

    return retVal;
    }
*/

/*
RConsumerMap CSenHostletTransportPlugin::ConsumerMap()
    {
    return iConsumerMap;    
    
    }
RSessionMap CSenHostletTransportPlugin::SessionMap()
    {
    return iSessionMap;
    }
*/

TInt CSenHostletTransportPlugin::SendL(const TDesC8& aEndpoint,
                                     const TDesC8& aMessage,
                                     const TDesC8& /* aTransportProperties */,
                                     MSenServiceSession& aReplyTo,
                                     MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                                     TInt& aTxnId)
    {
    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "CSenHostletTransportPlugin::SendL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "- No endpoint!");
        return KErrSenNoEndpoint; // from SenServiceConnection.h (public API)
        }

    if(aReplyTo.Hostlet() == NULL) 
        {
        TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "- Failure: no Hostlet for the session: leave with KErrSenHostNotAvailable");
        return KErrSenHostNotAvailable;
        }
    
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
//    TLSLOG_L(KHostletTransportLogChannel, KHostletTransportLogLevel,  "- Message:");
//    TLSLOG_ALL((KHostletTransportLogChannel, KHostletTransportLogLevel, aMessage));
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L8("- Message: %S"), &aMessage));

    
    iTransactionID++;
    TInt* txnID = new (ELeave) TInt(iTransactionID);
    aTxnId = iTransactionID;

//    MSenCoreServiceManager& core = Context().GetCoreL();
    CSenActiveHostletRequest* pRequest = CSenActiveHostletRequest::NewLC( aEndpoint,
                                                                          //aMessage, 
                                                                          //aTransportProperties,
                                                                          aReplyTo,
                                                                          aConsumer,
                                                                          *txnID,
                                                                          iRequestMap,
                                                                          *this,
                                                                          *ipCtx );

    TInt retVal = iRequestMap.Append(pRequest, txnID);

    if(retVal==KErrNone)
        {
        CleanupStack::Pop(pRequest);
        if(!pRequest->IsActive())
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "- Activating request / async call to the provider.");
            pRequest->SetActive();
            }
        pRequest->iStatus = KRequestPending;
        TRequestStatus* status = &pRequest->iStatus;
        User::RequestComplete( status, CSenActiveHostletRequest::EInvokeService ); //iTransactionID );
        TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, _L8("- Request status: %d"), pRequest->iStatus.Int()));
        }
    else
        {
        TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("- FATAL: iRequestMap.Append failed: %d"), retVal ));
        CleanupStack::PopAndDestroy(pRequest); // orphanized request
        }
    return retVal; // status code
    }

TInt CSenHostletTransportPlugin::SetPropertiesL(const TDesC8& /* aProperties */,
                                                MSenLayeredProperties::TSenPropertiesLayer /* aTargetLayer */,
												MSenRemoteServiceConsumer* /*aConsumer*/)
    {
    return KErrNotSupported; // not implemented
    }

TInt CSenHostletTransportPlugin::PropertiesL(HBufC8*& /* aProperties */)
    {
    return KErrNotSupported; // not implemented
    }

// ASYNC
TInt CSenHostletTransportPlugin::CompleteTransaction(const TInt aTxnId,
                                                     const TInt aCompletionCode)
    {
    TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, 
        _L("CSenHostletTransportPlugin::CompleteTransaction(%d, %d)"), aTxnId, aCompletionCode));

    TInt index = iRequestMap.FindValue(aTxnId);
    
    // complete the request (transaction)
    if(index != KErrNotFound)
        {
        // Consumer found
        TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, 
            _L("- Pending transaction at index %d found. Completing it."),
            index));

        CSenActiveHostletRequest* pRequest = iRequestMap.KeyAt(index);
        TRequestStatus status;
        TRequestStatus* pStatus = &status;
        index = pRequest->Complete(pStatus, aCompletionCode);
        TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, _L("- Request.Complete() returned: %d"), index));
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "- Cannot find pending transaction with given ID.");
        }
#endif
    return index;
    }


TInt CSenHostletTransportPlugin::CancelTransaction( const TInt aTxnId )
    {
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("CSenHostletTransportPlugin::CancelTransaction(%d)"), aTxnId));
    return CompleteTransaction( aTxnId, KErrSenCancelled );
    }

TPtrC8 CSenHostletTransportPlugin::UriSchemeL()
    {
    return KSenHostletTransportUriScheme();
    }
    
MSenProperties& CSenHostletTransportPlugin::PropertiesL()
    {
    return *ipLayeredXmlProperties;
    }


// ------------------------------------------------------------------------------- //

CSenActiveHostletRequest* CSenActiveHostletRequest::NewLC(const TDesC8& aEndpoint,
                                          MSenServiceSession& aReplyTo,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          TInt aTxnId,
                                          RLocalRequestMap& aRequestMap,
                                          MSenTransport& aParent,
                                          MSenTransportContext& aParentCtx)
    {
    CSenActiveHostletRequest* pNew = new (ELeave) CSenActiveHostletRequest(aEndpoint, aReplyTo, aConsumer, aTxnId, aRequestMap, aParent, aParentCtx);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }


CSenActiveHostletRequest::CSenActiveHostletRequest(const TDesC8& aEndpoint,
                                                   MSenServiceSession& aReplyTo,
                                                   MSenRemoteServiceConsumer& aConsumer,
                                                   TInt aTxnId,
                                                   RLocalRequestMap& aRequestMap,
                                                   MSenTransport& aParent,
                                                   MSenTransportContext& aParentContext)
:   CActive(EPriorityStandard),
    iEndpoint(aEndpoint),
    iCompletionCode(KErrNone),
    iReplyTo(aReplyTo),
    iConsumer(aConsumer),
    iTxnId(aTxnId),
    iRequestMap(aRequestMap),
    iParent(aParent),
    iParentContext(aParentContext),
    iMessage(KNullDesC8),
    iProperties(KNullDesC8)
    {
    CActiveScheduler::Add(this);
    }

    
CSenActiveHostletRequest::~CSenActiveHostletRequest()
    {
    if ( IsActive() )
        {
        Cancel(); // calls CSenActiveHostletRequest::DoCancel()
        }
    else
        {
        RequestMap().Remove(TxnId());
        }
    }

// returns chunk index or an error
CSenChunk* CSenActiveHostletRequest::FindChunkL(const TInt aTxnId, TInt& aRetVal)
    {
    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "CSenActiveHostletRequest::DataFromChunkL");
    CSenChunk* pChunk = NULL;
   
    // This lookup needs to be performed at every state of this AO,
    //  since the transaction (chunk) might have been removed..
    aRetVal = iConsumer.ChunkByTxnIdL(aTxnId, pChunk);
    
    TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, _L("- ChunkByTxnIdL() returned: %d"), aRetVal));
  
    if( aRetVal == KErrNone )
        {
        if ( pChunk )
            {
            iMessage.Set(KNullDesC8);
            iProperties.Set(KNullDesC8);
            TInt error(KErrNone);
            if (pChunk->ChunkHeader().PropertiesType() == MSenProperties::ENotInUse )
                {
                error = pChunk->DescFromChunk(iMessage);
                }
            else
                {
                error = pChunk->DescsFromChunk(iMessage, iProperties);
                }
                
            if( error )
                {
                aRetVal = error;
                iMessage.Set(KNullDesC8);
                iProperties.Set(KNullDesC8);
                }
            }
        else
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "- Chunk from consumer == NULL, eventhough getter returned KErrNone(!)");
            // No chunk, eventhough KErrNone was returned, should NEVER happen
            // ==> report "internal error"
            aRetVal = KErrSenInternal; // error code is from SenServiceConnection.h
            }
        }
    return pChunk;        
    }
    
TInt CSenActiveHostletRequest::RunError(TInt aError)
    {
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("CSenActiveHostletRequest::RunError(%d)"), aError));
    delete this;
    return KErrNone;
    }

CSenTransportProperties* CSenActiveHostletRequest::NewPropertiesByTypeL( MSenProperties::TSenPropertiesClassType aType, const TDesC8& aPropertiesAsXml )
    {
    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "CSenActiveHostletRequest::NewPropertiesByTypeL - begin");    
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L(" - aProperties: '%S'"), &aPropertiesAsXml ));

    CSenTransportProperties* pProps = NULL;
    CSenXmlReader& reader = *(iParentContext.GetCoreL().XMLReader());

    // Check whether the response carries properties and allocate properties object, if it does
    switch ( aType )
        {
        case MSenProperties::ENotInUse:
            {
            // pProps = NULL;
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "Properties type: MSenProperties::ENotInUse");    
            }
        break;

        case MSenProperties::ESenHttpTransportProperties:
            {
            pProps = CSenHttpTransportProperties::NewL( iProperties, reader );
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "Properties type: MSenProperties::ESenHttpTransportProperties");    
            }
        break;

        case MSenProperties::ESenVtcpTransportProperties:
            {
            pProps = CSenVtcpTransportProperties::NewL( iProperties, reader );
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "Properties type: MSenProperties::ESenVtcpTransportProperties");    
            }
        break;

        case MSenProperties::ESenTransportProperties:
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "Properties type: MSenProperties::ESenTransportProperties");    
            pProps = CSenTransportProperties::NewL( iProperties, reader );    
            }
        break;
        
        default:
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "Properties type is unknown, defaulting to MSenProperties::ESenTransportProperties");
            pProps = CSenTransportProperties::NewL( iProperties, reader );    
            }
        break;
        } 
    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "CSenActiveHostletRequest::NewPropertiesByTypeL - end");    
    return pProps;           
    }

void CSenActiveHostletRequest::RunL() 
    {
    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("CSenActiveHostletRequest::RunL(), status: %d"), iStatus.Int()));
    TInt leaveCode(KErrNone);
    TInt retVal(KErrNone); // for UDEB, logging
    CSenChunk* pChunk = NULL;
    switch(iStatus.Int())
        {
        
        case EInvokeService:
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "HostletTransport req AO::RunL -- EInvokeService -- ");
            MSenRemoteHostlet* pHostlet = iReplyTo.Hostlet();
            if( pHostlet )
                {
                TRAP( leaveCode, pChunk = FindChunkL(iTxnId, retVal) );
                MSenProperties::TSenPropertiesClassType type = pChunk->ChunkHeader().PropertiesType();

                if ( leaveCode != KErrNone ) 
                    {
                    retVal = leaveCode;
                    }
                    
                if ( retVal == KErrNone )
                    {
//                    CSenTransportProperties* pProps = NewPropertiesByTypeL( type, iProperties );
//                    if( pProps )
//                        {
//                        CleanupStack::PushL( pProps );
//                        retVal = iReplyTo.SendToHostletL( *pHostlet, iMessage, iTxnId, iConsumer, pProps );
//                        CleanupStack::PopAndDestroy( pProps );
//                        }
//                    else // no properties in this request
//                        {
                        retVal = iReplyTo.SendToHostletL( *pHostlet, iMessage, iTxnId, iConsumer );
//                        }
                    TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, _L8("- HostletTransport req AO::RunL: ProcessRequestFromConsumerL returned: %d "), retVal));
                    }
                }
            }
            break;

        case ECompleteTransaction:
            {
            TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "HostletTransport req AO::RunL -- ECompleteTransaction -- ");
            
            // This lookup needs to be performed (again), since the transaction (chunk) might have been removed..
            TRAP( leaveCode, pChunk = FindChunkL(iTxnId, retVal) );
            TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("- FindChunkL() returned: %d [%d]"), retVal, leaveCode));
            if( leaveCode != KErrNone ) 
                {
                retVal = leaveCode;
                }
            
            if( retVal == KErrNone )
                {
                // It is VERY important to change the chunk's (message's) direction to INBOUND. Among other things,
                // it will indicate that the descriptor(s) passed to session & consumer -- msg & props -- are in
                // chunk (no need to delete a copy from heap, etc)
                pChunk->ChunkHeader().SetMessageDirection(MSenMessage::EInbound);

                MSenProperties::TSenPropertiesClassType type = pChunk->ChunkHeader().PropertiesType();
                
                HBufC8* pMessage = (HBufC8*)&iMessage; // This is OK, since we have set the message direction to INBOUND, and message will NOT be de-allocated(!),
                                                       // Neither will it be copied to its own address (it is already in the chunk)
                                                       // NOTE(!): Remote consumer checks chunk header's message direction and does not delete the desc behind this pointer
                                                       // if the direction is already "inbound"
                                                       
                CSenTransportProperties* pProps = NewPropertiesByTypeL( type, iProperties );

                if( iCompletionCode < KErrNone ) // aCompletionCode == ERROR
                    {
                    if ( pProps )
                        {
                        CleanupStack::PushL( pProps );
                        TRAP( leaveCode, retVal = iReplyTo.SendErrorToConsumerL(iCompletionCode, pMessage, iTxnId, iConsumer, pProps); )
                        CleanupStack::PopAndDestroy( pProps );
                        }
                    else
                        {
                        TRAP( leaveCode, retVal = iReplyTo.SendErrorToConsumerL(iCompletionCode, pMessage, iTxnId, iConsumer); )
                        }
                    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("- SendErrorToConsumerL() returned: %d [%d]"), retVal, leaveCode));
                    }
                else // completionCode == KErrNone (all is fine)
                    {
                    if ( pProps )
                        {
                        CleanupStack::PushL( pProps );
                        TRAP( leaveCode, retVal = iReplyTo.SendToConsumerL(pMessage, iTxnId, iConsumer, pProps); )
                        CleanupStack::PopAndDestroy( pProps );
                        }
                    else
                        {
                        TRAP( leaveCode, retVal = iReplyTo.SendToConsumerL(pMessage, iTxnId, iConsumer); )
                        }
                    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L("- SendToConsumerL() returned: %d [%d]"), retVal, leaveCode));
                    }
                if ( retVal == KErrNone && leaveCode != KErrNone) 
                    {
                    retVal = leaveCode;
                    }

                MSenRemoteHostlet* pHostlet = iReplyTo.Hostlet();
                if( pHostlet )
                    {
                    TRAP( leaveCode, retVal = pHostlet->OnServiceCompleteL(iTxnId, retVal, KNullDesC8); ) 
                    TLSLOG_FORMAT((KHostletTransportLogChannel, KMinLogLevel, _L8("- OnServiceCompleteL returned: %d [%d]"), retVal, leaveCode));
                    retVal = 0; // not used in release builds
                    }
#ifdef _SENDEBUG                        
                else 
                    {
                    TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "- Remote Hostlet == NULL");
                    }
#endif                        
                }
                
            if(retVal == KErrNone)
                {
                retVal = RequestMap().Remove(iTxnId); // deletes the request (transaction)
                TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, _L("- Remove() returned: %d"), retVal));
                }
            // de-allocate this in EVERY CASE
            delete this; // (*) => this MUST ALWAYS BE PERFORMED!
            }
            break;
            
        default:
            {
            TLSLOG_FORMAT((KHostletTransportLogChannel, KNormalLogLevel, _L("- HostletTransport req AO::RunL: Unknown request: %d"), iStatus.Int()));
            }
            // unknown
            break;
        }
    TInt unused = leaveCode; leaveCode = unused; // not used in release builds
    unused = retVal; retVal = unused;       // not used in release builds
    } 

TInt CSenActiveHostletRequest::Complete(TRequestStatus*& aStatus, TInt aCompletionCode) 
    {
    iCompletionCode = aCompletionCode;
    if(!IsActive())
        {
        TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel,  "CSenActiveHostletRequest::Complete()");
        SetActive();
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KHostletTransportLogChannel, KMinLogLevel, "CSenActiveHostletRequest::Complete(): AO already active (?)");
        }
#endif // _SEDEBUG        
    iStatus = KRequestPending;
    aStatus = &iStatus;
    User::RequestComplete( aStatus, CSenActiveHostletRequest::ECompleteTransaction );
    return KErrNone; 
    }


void CSenActiveHostletRequest::DoCancel() 
    {
    RequestMap().Remove(TxnId()); 
    }

void CSenActiveHostletRequest::ConstructL()
    {
    }

RLocalRequestMap& CSenActiveHostletRequest::RequestMap()
    {
    return iRequestMap;
    }

MSenServiceSession& CSenActiveHostletRequest::Session() const
    {
    return iReplyTo;
    }

MSenRemoteServiceConsumer& CSenActiveHostletRequest::Consumer() const
    {
    return iConsumer;
    }


TInt CSenActiveHostletRequest::TxnId() const
    {
    return iTxnId;
    }

// END OF FILE
