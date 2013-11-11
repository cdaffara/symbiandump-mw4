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











#include "senlocaltransportplugin.h"

#include <SenServiceConnection.h>   // errorcode constants, like KErrSenNoEndpoint

#include "msenprovider.h"
#include "senhostletrequest.h"      // internal Provider\inc
#include "senhostletresponse.h"     // internal Provider\inc

#include "senidentifier.h"          // internal Utils\inc
#include "senidentifier.h"          // internal Utils\inc
#include "sendebug.h"               // internal Utils\inc

#include "senprovider.h"            // internal Framework\inc
#include "sensyncrequester.h"       // internal Framework\inc
#include <SenTransportProperties.h> // internal Framework\inc

#include "msentransport.h"
#include "msencoreservicemanager.h" // internal: Core\inc

#include "senlayeredtransportproperties.h"
#include "senhostletrequest.h"      // internal Provider\inc
#include "senguidgen.h"
#include "senlogger.h"

#include <RSenDocument.h>
namespace
    {
//Deleted to fix for compiler warning  #177-D

    //const TInt maxThreadHeapSize(KMinHeapSize*256);

    void threadMainL(TAny* aPtr)
        {
        CSenLocalRequest* pRequest = (CSenLocalRequest*) aPtr;
        pRequest->ExecuteL();
        TRequestStatus* status = &pRequest->iStatus;
        RThread owner;
        TInt error=owner.Open(pRequest->OwnerThreadId());	//CodeScannerWarnings
        if(error != KErrNone)	//CodeScannerWarnings
        {
        }
        owner.RequestComplete(status, CSenLocalRequest::EDeliverResponse ); //iTransactionID );
        // Go safely out of scope (will destroy this thread)
        }

    TInt threadFunction(TAny* aPtr)
        {
        CTrapCleanup* pCleanup = CTrapCleanup::New();
        CActiveScheduler::Install(NULL); // remove one
        CActiveScheduler* pScheduler = new (ELeave) CActiveScheduler();
        CActiveScheduler::Install(pScheduler);

        RSenDocument::ManualXmlEngineTlsAttachL();

        TInt leaveCode(KErrNone);
        TRAP(leaveCode, threadMainL(aPtr));

        RSenDocument::ManualXmlEngineTlsCleanup();

        CActiveScheduler::Install(NULL); // uninstall scheduler
        delete pScheduler;
        delete pCleanup;

        return leaveCode;
        }
    }
CSenLocalTransportPlugin* CSenLocalTransportPlugin::NewL(CSenTransportContext* apCtx)
    {
    CSenLocalTransportPlugin* pNew = NewLC(apCtx);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenLocalTransportPlugin* CSenLocalTransportPlugin::NewLC(CSenTransportContext* apCtx)
    {
    CSenLocalTransportPlugin* pNew =
        new (ELeave) CSenLocalTransportPlugin(apCtx);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenLocalTransportPlugin::CSenLocalTransportPlugin(CSenTransportContext* apCtx)
:   CSenTransport(apCtx),
    ipRequester(NULL),
    iInvokedProvider(NULL),
    iStatusCode(0),
    iTransactionID(0),
    iRequestMap(ETrue, ETrue),
    ipTransportProperties(NULL)
    {
    }

CSenLocalTransportPlugin::~CSenLocalTransportPlugin()
    {
    TLSLOG(KSenLocalTransportLogChannelBase,KMinLogLevel,_L("Log file closed."));
    TLSLOG_CLOSE(KSenLocalTransportLogChannelBase);
    iRequestMap.Reset(); // Cleanup the request queue       
    delete ipRequester;
    delete iInvokedProvider;
    delete ipTransportProperties;
    }

void CSenLocalTransportPlugin::ConstructL()
    {
    // Open connection to the file logger server
    TLSLOG_OPEN(KSenLocalTransportLogChannelBase,KSenLocalTransportLogLevel,KSenLocalTransportLogDir,KSenLocalTransportLogFile);
    TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"CSenLocalTransportPlugin::ConstructL - Log file opened.");
    TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"CSenLocalTransportPlugin::ConstructL - Version 2 [2006-05-09]");    
    MSenCoreServiceManager& core = Context().GetCoreL();
    ipRequester = CSenSyncRequester::NewL(*this, core);
    ipTransportProperties = CSenLayeredTransportProperties::NewL();
    }

TInt CSenLocalTransportPlugin::SubmitL(const TDesC8& aEndpoint,
                                       const TDesC8& aMessage,
                                       const TDesC8& aTransportProperties,
                                       HBufC8*& apResponse,
                                       MSenRemoteServiceConsumer& /*aConsumer*/) 
    {
    TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"CSenLocalTransportPlugin::SubmitL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"- No endpoint!");
        return KErrSenNoEndpoint; // from SenServiceConnection.h (public API)
        }
    TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KMinLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
    
    TLSLOG_L(KSenLocalTransportLogChannelBase,KMaxLogLevel,"- Message:");
    TLSLOG_ALL(KSenLocalTransportLogChannelBase,KMaxLogLevel,( aMessage ));

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
    TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KMinLogLevel, _L8("CSenLocalTransportPlugin::SubmitL returned: (%d)"), statusCode));
    

    if(leaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KMinLogLevel, _L8("CSenLocalTransportPlugin::SubmitL leaved: (%d)"), leaveCode));
        if(statusCode==KErrNone)
            {
            statusCode = leaveCode;
            }
        }
    else // did not leave
        {
        if(!apResponse)
            {
            TLSLOG(KSenLocalTransportLogChannelBase,KMinLogLevel,(_L("After calling CSenLocalTransportPlugin::SubmitL => response==NULL => leaving!")));
            User::Leave(KErrCorrupt);
            }
        TLSLOG_L(KSenLocalTransportLogChannelBase,KMaxLogLevel,"CSenLocalTransportPlugin::SubmitL response:");
        TLSLOG_ALL(KSenLocalTransportLogChannelBase,KMaxLogLevel,(*apResponse));
        }
                                        
    return statusCode;
    }

TInt CSenLocalTransportPlugin::SendL(const TDesC8& aEndpoint,
                                     const TDesC8& aMessage,
                                     const TDesC8& aTransportProperties,
                                     MSenServiceSession& aReplyTo,
                                     MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                                     TInt& aTxnId)
    {
    TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"CSenLocalTransportPlugin::SendL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"- No endpoint!");
        return KErrSenNoEndpoint; // from SenServiceConnection.h (public API)
        }
    TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KMinLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
    
    TLSLOG_L(KSenLocalTransportLogChannelBase,KMaxLogLevel,"- Message:");
    TLSLOG_ALL(KSenLocalTransportLogChannelBase,KMaxLogLevel,( aMessage ));
    
    iTransactionID++;
    TInt* txnID = new (ELeave) TInt(iTransactionID);
    aTxnId = iTransactionID;

	CleanupStack::PushL(txnID);	// memory leaks if CSenLocalRequest::NewLC leaves...
	
    // CSenLocalTransportPlugin::ProviderL method ("local proxy" for single consumer
    // -- since each consumer always owns its own transport) has now been **replaced**
    // with "provider factory" methods implemented to WSF **core**:

    MSenCoreServiceManager& core = Context().GetCoreL();
    CSenLocalRequest* pRequest = CSenLocalRequest::NewLC(aEndpoint,
                                                         aMessage, 
                                                         aTransportProperties,
                                                         aReplyTo,
                                                         aConsumer,
                                                         core,
                                                         *txnID);

#ifdef _SENDEBUG
    //pRequest->SetLogger(iLogger);
#endif // _SENDEBUG

    TInt retVal = iRequestMap.Append(pRequest, txnID);

    if(retVal==KErrNone)
        {
        CleanupStack::Pop(pRequest);
        CleanupStack::Pop(txnID);        
        if(!pRequest->IsActive())
            {
            TLSLOG_L(KSenLocalTransportLogChannelBase,KNormalLogLevel,"- Activating request / async call to the provider.");
            if(!pRequest->IsActive())
            	pRequest->SetActive();
            }
        pRequest->iStatus = KRequestPending;
        TRequestStatus* status = &pRequest->iStatus;
        User::RequestComplete( status, CSenLocalRequest::EInvokeService ); //iTransactionID );
        TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KNormalLogLevel, _L8("- Request status: %d"), pRequest->iStatus.Int()));
        }
    else
        {
        CleanupStack::PopAndDestroy(pRequest); // orphanized request
        CleanupStack::Pop(txnID);
        }
    return retVal; // status code
    }


TInt CSenLocalTransportPlugin::SetPropertiesL(const TDesC8& /* aProperties */,
                                              MSenLayeredProperties::TSenPropertiesLayer /* aTargetLayer */,
											  MSenRemoteServiceConsumer* /*aConsumer*/)
    { 
    return KErrNotSupported; // not implemented
    }

TInt CSenLocalTransportPlugin::PropertiesL(HBufC8*& /* aProperties */)
    {
    return KErrNotSupported; // not implemented
    }

TInt CSenLocalTransportPlugin::CompleteTransaction(const TInt /*aTxnId*/,
                                                         const TInt /*aStatus*/)
    {
    return KErrNotSupported; // not supported by HTTP Channel
    }

TInt CSenLocalTransportPlugin::CancelTransaction(const TInt /* aTxnId */)
    {
    return KErrNotSupported; 
    }

TPtrC8 CSenLocalTransportPlugin::UriSchemeL()
    {
    return KSenLocalTransportUriScheme();
    }


MSenProperties& CSenLocalTransportPlugin::PropertiesL()
    {
    return *ipTransportProperties; 
    }    

/*
RFileLogger* CSenLocalTransportPlugin::Log() const
    {
    return (RFileLogger*) &iLogger;
    }
*/
CSenLocalRequest* CSenLocalRequest::NewLC(const TDesC8& aEndpoint, //MSenProvider& aRequestTo,
                                          const TDesC8& aMessage,
                                          const TDesC8& aTransportProperties,
                                          MSenServiceSession& aReplyTo,
                                          MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                                          MSenCoreServiceManager& aProviderProxy,
                                          TInt aTxnId)
    {
    //CSenLocalRequest* pNew = new (ELeave) CSenLocalRequest(aRequestTo, aReplyTo, aConsumer, aProviderProxy, aTxnId);
    CSenLocalRequest* pNew = new (ELeave) CSenLocalRequest(aEndpoint, aReplyTo, aConsumer, aProviderProxy, aTxnId);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aMessage, aTransportProperties);
    return pNew;
    }


CSenLocalRequest::CSenLocalRequest(const TDesC8& aEndpoint, //MSenProvider& aRequestTo, 
                                   MSenServiceSession& aReplyTo,
                                   MSenRemoteServiceConsumer& aConsumer,
                                   MSenCoreServiceManager& aProviderProxy,
                                   TInt aTxnId)
: 
    CActive(EPriorityStandard),
    ipProvider(NULL),
    iEndpoint(aEndpoint),
    iProviderProxy(aProviderProxy),
    ipRequestImpl(NULL),
    ipResponseImpl(NULL),
    ipTransportProperties(NULL),
    iReplyTo(aReplyTo),
    iConsumer(aConsumer),
    iTxnId(aTxnId),
    iLeaveCode(KErrNone),
    iRetVal(KErrNone),
    iDelivered(EFalse),
    ipResponse(NULL)
    {
    CActiveScheduler::Add(this);
    }

    
CSenLocalRequest::~CSenLocalRequest()
    {
    DoCancel();
    delete ipTransportProperties;
    delete ipRequestImpl;
    delete ipResponseImpl;
    iThread.Close();
    delete ipResponse;
    //    delete ipReqIdentifier;
    }
    
    
// Called when local request thread crashes due to wrong local provider code
// WS-Stack catches this crash
// Stack still remain stable and communicates the failure to the request or consumer
void CSenLocalRequest::LocalRequestExceptionHandlerL(TExcType aType)
	{
	// Crash is caused by local provider's ServiceL method 
	// So nothing to be handled here and just return request thread
	// died status or OOM
	switch(aType) 
		{
		case EExcStackFault: //stack overflow
		case EExcAccessViolation: //heap memory overflow
			User::Leave(KErrNoMemory);//OOM
			break;
		default:
			User::Leave(KErrDied);
			break;
		}
	}

void CSenLocalRequest::ExecuteL()
    {
    // Allow only single thread to run ServiceL at the same time,
    // this is mandatory, since the provider might be sharable,
    // and providers might then have some status (session data)
    // which could be violated in multi-threaded execution.
    // @see MSenProvider.h


    if(ipProvider->Threadsafe())
        {
        // Only threadsafe providers block access from other threads.
        // The non-threadsafe providers allow multiple threads to get
        // inside ServiceL() - in such case the provider typically
        // needs to implement critical section handling *itself*
        ipProvider->iCriticalSection.Wait(); 
        }
        
	User::SetExceptionHandler(CSenLocalRequest::LocalRequestExceptionHandlerL, KExceptionFault);
        
    // try processing the message through the provider - trap any leave
    TRAP(iLeaveCode, iRetVal = ipProvider->ServiceL(*ipRequestImpl, *ipResponseImpl));
    
    // If no crashes or leaves during ServiceL
    if (iLeaveCode == KErrNone) 
    	{
    	ipResponse = ipResponseImpl->ExtractResponseUtf8();    	
    	}
    
   	// ipRequestImpl and ipResponseImpl are created within local provider thread
   	// scope. Since these objects involve XmlEngineAttachL and XmlEngineCleanup 
   	// calls, the cleanup is done within the threads scope otherwise XmlEngineCleanup
   	// will happen in main thread's scope which move the xmlengine reference count
   	// below 0, XmlEngine data structures will cleanup. This is unintended behaviour. So
   	// to avoid this cleanup is done within local provider thread scope
   	delete ipRequestImpl;
   	ipRequestImpl = NULL;
    delete ipResponseImpl;
    ipResponseImpl = NULL;  

    if(ipProvider->Threadsafe())
        {
        // Notify next thread that it can proceed and run 
        // ServiceL (needed for sharable providers):
        ipProvider->iCriticalSection.Signal();
        }

    iStatus = KRequestPending;
    if(!IsActive())
    	SetActive();
    }

const TThreadId CSenLocalRequest::OwnerThreadId() const
    {
    return iOwnerThreadId;
    }

void CSenLocalRequest::RunL() 
    {
    TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KMinLogLevel, _L8("CSenLocalRequest::RunL(), status: %d"), iStatus.Int()));

    switch(iStatus.Int())
        {
        
        case EInvokeService: // executed in "main", server thread by the ActiveScheduler
            {
            TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"EInvokeService -- ");

#ifdef _SENDEBUG
            TName fullThreadName = iThread.FullName();
            TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KNormalLogLevel, _L8("'%S'"), &fullThreadName));
#endif // _SENDEBUG
            iThread.Resume(); // this will issue ExecuteL call from *another thread*!
            }
            break;

        case EDeliverResponse: // executed in "main", server thread by the ActiveScheduler
            {
            TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"EDeliverResponse");
            DeliverL();
            }
            break;
            
        default:
            {
            TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"- Unknown request. Processing not implemented.");
            }
            // unknown
            break;
        }
    } 

void CSenLocalRequest::DeliverL()
    {
    if(iLeaveCode)
        {
        // send error to consumer
        iRetVal = iLeaveCode;
        TLSLOG_FORMAT((KSenLocalTransportLogChannelBase,KMinLogLevel, _L8("CSenProvider::ExecuteL leaved: %d"), iLeaveCode));

        Session().SendErrorToConsumerL(iRetVal, 
                                       NULL, 
                                       TxnId(),
                                       Consumer());
        }
    else        
        {
//        TPtrC8 response = ipResponseImpl->ResponseUtf8();
        //HBufC8* pResponse = ipResponseImpl->ExtractResponseUtf8();
        TLSLOG_L(KSenLocalTransportLogChannelBase,KMaxLogLevel,"CSenLocalRequest::ExecuteL() hostlet response:");

#ifdef _SENDEBUG        
        if( ipResponse )
            {
            TLSLOG_ALL(KSenLocalTransportLogChannelBase,KMaxLogLevel,( *ipResponse ));
            }
        else
            {
            TLSLOG_L(KSenLocalTransportLogChannelBase,KMinLogLevel,"-Fatal(!): pResponse == NULL");
            }
#endif // _SENDEBUG

        if( iRetVal!=KErrNone ) 
            {
            // send error to the consumer
            Session().SendErrorToConsumerL(iRetVal, 
                                           ipResponse, 
                                           TxnId(),
                                           Consumer());
			// Ownership is transferred if response is delivered so no cleanup                                        
            ipResponse = NULL;
            }
        else
            {
            // send message to consumer
            Session().SendToConsumerL(ipResponse, 
                                      TxnId(),
                                      Consumer());
			// Ownership is transferred if response is delivered so no cleanup                                           
            ipResponse = NULL;
            }
        } 
    iProviderProxy.ReleaseHostletL(ipProvider, iThread.FullName(), iConsumer.Id());
    iThread.Close();
    }

void CSenLocalRequest::DoCancel() 
    {
    }

void CSenLocalRequest::ConstructL(const TDesC8& aMessage, 
                                  const TDesC8& aTransportProperties)

    {

    ipTransportProperties = aTransportProperties.AllocL();

    TInt consumerIdLength(iConsumer.Id().Length());
    
    HBufC* pTemp = HBufC::NewLC(consumerIdLength);
    // Consumer id (UCS8) does not contain any special,
    // "UTF-8 to Unicode" -convertible characters, so 
    // copying is OK.
    TPtr temp = pTemp->Des();
    temp.Copy(iConsumer.Id()); 
    HBufC* pConsumerIdUnicode = HBufC::NewLC(consumerIdLength);
    TPtr consumerIdUnicode = pConsumerIdUnicode->Des();
    TLSLOG_ALL(KSenLocalTransportLogChannelBase,KMaxLogLevel,(consumerIdUnicode));

    if(consumerIdLength>KSenUuidPrefixLength) 
        {
        consumerIdUnicode.Append(pTemp->Right(consumerIdLength-KSenUuidPrefixLength));
        }

    // Remove dashes
    _LIT(KDash, "-");
    TInt pos = consumerIdUnicode.Find(KDash);
    while (pos != KErrNotFound)
        {
        consumerIdUnicode.Replace(pos, KDash().Length(), KNullDesC);
        pos = consumerIdUnicode.Find(KDash);
        }
    
    TName threadName;
    _LIT(KNameFmt, "CSenLocalRequest%d.%S");
    threadName.Format(KNameFmt, iTxnId, pConsumerIdUnicode);

    CleanupStack::PopAndDestroy(2); // pConsumerIdUnicode, pTemp

#ifdef EKA2
    RAllocator& heap = User::Allocator(); // RHeap is deprecated in EKA2
    iThread.Create(threadName, // name
                   threadFunction,
                   KDefaultStackSize,
                   &heap,
                   this);
#else // EKA1
    RHeap& heap = User::Heap();
    iThread.Create(threadName, // name
                   threadFunction,
                   KDefaultStackSize,
                   &heap,
                   this);
#endif // EKA

    RThread thread;
    iOwnerThreadId = thread.Id();


    ipResponseImpl = CSenHostletResponse::NewL(iTxnId);

    CSenChunk* pChunk = NULL;
    
/*    TInt getChunkRetCode(KErrNotFound);
    getChunkRetCode = */
    iConsumer.ChunkByTxnIdL(iTxnId, pChunk);


    ipRequestImpl = CSenHostletRequest::NewL(iTxnId, aMessage, iThread, iConsumer.Identifier(), pChunk);
    
//    getChunkRetCode = 0; // not used atm (in release builds)
    
    //TBuf8<128> threadName;
    //_LIT8(KNameFmt, "CSenLocalRequest-%d");
    //threadName.Format(KNameFmt, iTxnId);
    //ipReqIdentifier = CSenIdentifier::NewL(SenIdentifier::EThread, threadName);

    ipProvider = & iProviderProxy.LookupHostletForL(iEndpoint, iThread.FullName(), iConsumer.Id());

    }


MSenServiceSession& CSenLocalRequest::Session() const
    {
    return iReplyTo;
    }

MSenRemoteServiceConsumer& CSenLocalRequest::Consumer() const
    {
    return iConsumer;
    }


TInt CSenLocalRequest::TxnId() const
    {
    return iTxnId;
    }


TBool CSenLocalRequest::Delivered() const
    {
    return iDelivered;
    }

CSenHostletResponse& CSenLocalRequest::Response()
    {
    return *ipResponseImpl;
    }

TPtrC8 CSenLocalRequest::TransportProperties() const
    {
    return *ipTransportProperties;
    }

/*void CSenLocalRequest::SetLogger(RFileLogger& aLogger)
    {
    iLogger = &aLogger;
    }

RFileLogger* CSenLocalRequest::Log() const
    {
    return (RFileLogger*) iLogger;
    }
*/    

// END OF FILE
