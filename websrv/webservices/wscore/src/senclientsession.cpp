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
#include <s32mem.h>                         // RChunk
#include <e32property.h>                    // RProperty

#include <SenServiceConnection.h>           // error codes and framework ids
#include <SenHostletConnection.h>           // error code(s)
#include "senwsdescription.h"
#include "senwspattern.h"
#include <SenIdentityProvider.h>
#include <SenXmlUtils.h>
#include <SenXmlProperties.h>

#include <xml/dom/xmlengnodelist.h> 
#include <defaultcaps.hrh>

#include "senclientsession.h"
#include "senservicemanagerdefines.h"       // IPC enumerations
#include "msencoreservicemanager.h"

#include "sendebug.h"                       // internal Utils\inc
#include "sennotplugindlg.h"                // internal NotifierPlugin\inc
#include "senidentifier.h"

#include "senserviceinvocationframework.h"  // internal Framework\inc
#include "msenremoteservicesession.h"       // internal Framework\inc
#include "sentransport.h"                   // internal Framework\inc
#include "sentransportcontext.h"            // internal Framework\inc
//#include "SenServiceSession.h"              // internal Framework\inc
#include "senwebservicesession.h"                // internal Framework\inc
#include "senlayeredtransportproperties.h"  // internal Framework\inc
#include "senmessagecontext.h"              // internal Framework\inc

#include "SenSoapEnvelope2.h"
#include "sensoapmessagedom2.h"
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengdeserializer.h>
#include "SenParser.h"
#include "seninternalcredential.h"
#include "senservercontext.h"
#include "senapplicationcontext.h"
#include "senclientcontext.h"
#include "senxmldebug.h"
#include "senlogger.h"
#include "SenServiceConnection.h"  
#include <SenTransportProperties.h>

#ifdef __TEST_RETRY_TTL
#include <SenVtcpTransportProperties.h>//internal for lonlived test
#endif //__TEST_RETRY_TTL
namespace
    {
    const TInt KFlatBufSize = 128;
	const TInt KMaxAuthenticationRetries = 3;
    _LIT8(KTab, "\t");
    _LIT8(KNewline, "\n");
    _LIT8(KProviderPolicyLocalName,     "ProviderPolicy");
    _LIT8(KServicePolicyLocalName,      "ServicePolicy");
    _LIT8(KProviderIdLocalName,         "ProviderID");
    
    // _LIT8( KSenCidPostfix, "@example.org" );
    #ifdef RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS
    _LIT8( KSenCidPostfix, "@" );
    const TInt KSenMaxCidLength = 10;
    #endif //RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS
    
    }


CSenMessageResourceHandler::CSenMessageResourceHandler(TInt aTxnId,
        TBool aIncoming, TBool aIsSoap, TInt aProgress,
        RSenConnectionServerSession* aOwner): CActive(EPriorityStandard),
        iOwner(aOwner)
    {
    TTransferProgress& data = iProgressData();
    data.iTxnId = aTxnId;
    data.iIncoming = aIncoming;
    data.iSoap = aIsSoap;
    data.iProgress = aProgress;
    }

CSenMessageResourceHandler::~CSenMessageResourceHandler()
    {
    Cancel();
    delete iSoapOrCid;
    }

void CSenMessageResourceHandler::DoCancel()
    {
    
    }

void CSenMessageResourceHandler::RunL()
    {
    iOwner->Remove(this);
    delete this;
    }

TInt CSenMessageResourceHandler::SetBuffer(const TDesC8& aSoapOrCid)
    {
    iSoapOrCid = aSoapOrCid.Alloc();
    if (iSoapOrCid)
        {
        return KErrNone;
        }
    return KErrNoMemory;
    }

TInt RSenConnectionServerSession::CreateSession(const RMessage2& aMessage)
    {
    TFullName name;
    TInt err = aMessage.Read(0, name);
    if (err != KErrNone)
        {
        return err;
        }
    err = RSessionBase::CreateSession(name, TVersion(), KAsyncMessageSlots);
    return err;
    }

void RSenConnectionServerSession::SendFileProgress(TInt aTxnId, TBool aIncoming,
        TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress)
    {
    CSenMessageResourceHandler* resHandler = new CSenMessageResourceHandler(
            aTxnId, aIncoming, aIsSoap, aProgress, this);
    if (!resHandler)
        {
        return;
        }
    if (resHandler->SetBuffer(aSoapOrCid) != KErrNone)
        {
        delete resHandler;
        return;
        }

    if (iFirst)
        {
        iFirst->iPrevious = resHandler;
        }
    resHandler->iNext = iFirst;
    iFirst = resHandler;

    CActiveScheduler::Add(resHandler);
    if(!resHandler->IsActive())
    	resHandler->SetActive();
    resHandler->iStatus = KRequestPending;

    TIpcArgs args(&resHandler->iProgressData, resHandler->iSoapOrCid);
    SendReceive(ESenCliServTransferProgress, args, resHandler->iStatus);
    }
TBool RSenConnectionServerSession::OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId)
    {
    
    HBufC8* cbXmlSnippet = aBrandIdListAsXmlSnippet.Alloc();
    CleanupStack::PushL(cbXmlSnippet);    
	TInt retVal(KErrNone);
	TIpcArgs args;
	TRequestStatus  status = KRequestPending; 	
	TBool goNogo = EFalse;
	args.Set(0, &goNogo);
	args.Set(1, cbXmlSnippet);
	args.Set(2, &aSelectedBrandId);
	SendReceive(ESenCliServGoNoGo, args, status);
   	User::WaitForRequest(status);
   	retVal = status.Int();
    CleanupStack::PopAndDestroy(cbXmlSnippet);    
   
    if(retVal == KErrNone)
	    return goNogo;        
	else
	    return EFalse;
	    
    }
void RSenConnectionServerSession::Remove(
        CSenMessageResourceHandler* aResourceHandler)
    {
    if (aResourceHandler == iFirst)
        {
        iFirst = aResourceHandler->iNext;
        if (iFirst)
            {
            iFirst->iPrevious = NULL;
            }
        }
    else
        {
        aResourceHandler->iPrevious->iNext = aResourceHandler->iNext;
        }
    }


void RSenConnectionServerSession::Close()
    {
    CSenMessageResourceHandler* resHandler = iFirst;
    while (resHandler)
        {
        resHandler->Cancel();
        CSenMessageResourceHandler* tmp = resHandler;
        resHandler = resHandler->iNext;
        delete tmp;
        tmp = NULL;
        }
    RSessionBase::Close();
    }

TInt RSenConnectionServerSession::InitProgressObserver(const RMessage2& aMessage)
	{
	TIpcArgs args(aMessage.Ptr1(), aMessage.Ptr2());
    TInt err = Send(ESenCliServInitialize, args);
    return err;
	}
#ifdef __ENABLE_ALR__
TInt RSenConnectionServerSession::InitMobilityObserver(const RMessage2& aMessage)
	{
	TIpcArgs args(aMessage.Ptr1(), aMessage.Ptr2());
    TInt err = Send(ESenCliServMobilityInitialize, args);
    return err;
	}

TInt RSenConnectionServerSession::PreferredCarrierAvailable( TAccessPointInfo& aOldAPInfo,
                                        TAccessPointInfo& aNewAPInfo, TBool& aIsUpgrade, TBool& aIsSeemLess)
	{
        // Note : Using TPtr8 since aIsSeamless is binary information
	 TPtr8 upGradeDescriptor(reinterpret_cast<TUint8*>(&aIsUpgrade),sizeof(aIsUpgrade),
        sizeof(aIsUpgrade));
	 TPtr8 seemLessdescriptor(reinterpret_cast<TUint8*>(&aIsSeemLess),sizeof(aIsSeemLess),
        sizeof(aIsSeemLess));
        
    TInt retVal(KErrNone);
    TIpcArgs args(aOldAPInfo.AccessPoint(), aNewAPInfo.AccessPoint(), &upGradeDescriptor, &seemLessdescriptor);    
	CSenConnAgentSync* caSync = new CSenConnAgentSync();
	if(caSync)
		{
		caSync->Start();
		SendReceive(ESenCliServPrefferedCarrierAvailable, args, caSync->iStatus);
        retVal = caSync->iStatus.Int();
		delete caSync;
		caSync = NULL;
		}
	return retVal;        
	
	}

TInt RSenConnectionServerSession::NewCarrierActive( TAccessPointInfo& aNewAPInfo, TBool& aIsSeamless )
    {
    // Note : Using TPtr8 since aIsSeamless is binary information
    TPtr8 descriptor(reinterpret_cast<TUint8*>(&aIsSeamless),sizeof(aIsSeamless),
    sizeof(aIsSeamless));
    TInt retVal(KErrNone);
    TIpcArgs args(aNewAPInfo.AccessPoint(), &descriptor);    
    CSenConnAgentSync* caSync = new CSenConnAgentSync();
    if(caSync)
        {
        caSync->Start();		
        SendReceive(ESenCliServNewCarrierActive, args, caSync->iStatus);
        retVal = caSync->iStatus.Int();
        delete caSync;
        caSync = NULL;
        }
    return retVal;
    }

TInt RSenConnectionServerSession::MobilityError( TInt& aError )
    {
    TInt retVal(KErrNone);
    TIpcArgs args(aError);
    CSenConnAgentSync* caSync = new CSenConnAgentSync();
    if(caSync)
        {
        caSync->Start();		
        SendReceive(ESenCliServMobilityError, args, caSync->iStatus);
        retVal = caSync->iStatus.Int();
        delete caSync;
        caSync = NULL;
        }
    return retVal;        
    }
#endif //__ENABLE_ALR__


TInt RSenConnectionServerSession::InitAuthObserver(const RMessage2& aMessage)
	{
	TIpcArgs args(aMessage.Ptr1(), aMessage.Ptr2());
    TInt err = Send(ESenCliServAuthInitialize, args);
    return err;
	}
TInt RSenConnectionServerSession::InitCoBrandingObserver(const RMessage2& aMessage)
	{
	TIpcArgs args(aMessage.Ptr1(), aMessage.Ptr2());
    TInt err = Send(ESenCliServCoBrandingInitialize, args);
    return err;
	}
TInt RSenConnectionServerSession::ReauthenticationNeeded(CSenChunk& aSenChunk)
	{
	TInt retVal(KErrNoMemory);
	TIpcArgs args;
	CActiveSchedulerWait asWait;
	aSenChunk.ChunkToArgs(args, 0);
	args.Set(1, &asWait);
	CSenConnAgentSync* caSync = new CSenConnAgentSync();
	if(caSync)
		{
		SendReceive(ESenCliServReAuthNeeded, args, caSync->iStatus);
		caSync->Start();
		Mem::FillZ(&asWait, sizeof(asWait));
		asWait.Start();
		retVal = caSync->iStatus.Int();
		delete caSync;
		caSync = NULL;
		}
	return retVal;
	}

CSenConnAgentSync::CSenConnAgentSync(): CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	iStatus = KRequestPending;
	}

CSenConnAgentSync::~CSenConnAgentSync()
	{
	if(IsActive())
		{
		Cancel();
		}
	}

void CSenConnAgentSync::DoCancel()
	{	
	}

void CSenConnAgentSync::RunL()
	{
	}

void CSenConnAgentSync::Start()
	{
	if(!IsActive())
		SetActive();
	}

CSenClientSession* CSenClientSession::NewL(MSenServiceManager& aServer, CSenServerContext& aCtx)
    {
    CSenClientSession* self = CSenClientSession::NewLC(aServer, aCtx);
    CleanupStack::Pop(self) ;
    return self ;
    }

CSenClientSession* CSenClientSession::NewLC(MSenServiceManager& aServer, CSenServerContext& aCtx)
    {
    CSenClientSession* self = new (ELeave) CSenClientSession(aServer, aCtx);
    CleanupStack::PushL(self) ;
    self->ConstructL() ;
    return self;
    }


CSenClientSession::CSenClientSession(MSenServiceManager& aServer, CSenServerContext& aCtx)
    : CSession2(),
    iManager(aServer),
    iStatus(0),
    iConsumerID(NULL),
    iSendBuf(NULL),
    iInitializer(NULL),
    iAllowWSDataReg(ENotChecked),
    iAllowWSDataUnReg(ENotChecked),
    ipTransport(NULL),
    ipIdentifier(NULL),
    ipHostletAwaitOp(NULL),
    iHostletRequests( ETrue, EFalse ),
    iAllowShowDialogCount(0),
    iConnectionID(KErrNotFound),
    iAuthenticationRetries(0),
    iServerContext( aCtx ),
    iConnectionHasCoBrandingCallback(EFalse),
    iReauthResendNeeded(EFalse)
#ifdef __ENABLE_ALR__
    ,iMobiltyObserver(NULL),
    iALRObserver(NULL),
    iMobilityCallBackForwarded(EFalse),
    iMobilityChoice(EFalse)
#endif
    {
    }

void CSenClientSession::ConstructL()
    {
    iConnectionID = iManager.NextConnectionID();
    #ifdef _SENDEBUG
    
    
    TFileName logFile;
    logFile.Append( KSenCoreServiceManagerLogFile().Left(KSenCoreServiceManagerLogFile().Length()-4) ); // exclude ".log" file extension
    logFile.AppendNum( iConnectionID );
    logFile.Append( KSenUnderline );
    
    logFile.Append( KSenCoreServiceManagerLogFile().Right(4) ); // postfix with ".log" file extension
    TLSLOG_OPEN_TO_MODE(KSenClientSessionLogChannelBase+iConnectionID, KSenClientSessionLogLevel, KSenCoreServiceManagerLogDir, logFile, EFileLoggingModeAppend);
#endif
    
    CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::ConstructL");
    // Generate unique id for this consumer
    iConsumerID = iManager.RandomGuidL();
    ipIdentifier = CSenIdentifier::NewL(SenIdentifier::ESession, *iConsumerID);
    __ASSERT_ALWAYS(iConsumerID!=NULL, User::Leave(KErrConsumerIdIsNull));
    CSLOG(iConnectionID, KNormalLogLevel ,(iConsumerID->Des()));
    iInitializer = CSenWSDescription::NewL();
    iManager.IncrementConnections();
    }

CSenClientSession::~CSenClientSession()
    {
    CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::~CSenClientSession - START");
#ifdef __ENABLE_ALR__    
    iMobiltyObserver = NULL ;
#endif
    switch( iSessionType )
        {
        case ERemoteConsumer_SC:
            {
            CSLOG_L(iConnectionID, KMinLogLevel ,"- session type: ERemoteConsumer_SC");
            //TTL maangement
            if( iServiceSession )
                {
                MSenTransport* pTransport = iServiceSession->Transport();
                if ( pTransport )
                    {
                    TRAP_IGNORE( pTransport->UnsubscribeEventListenerL(*this); )
                    }
	                              
                // Notify the framework plug-in about consumer being removed
                if( iFwNotified )
                    {
                    TRAP_IGNORE( iManager.NotifyFrameworksL(
                            iServiceSession->FrameworkId(),
                            KSenEventConsumerRemoved,
                            this); )        
                    }
                }
            }
        break;
        case ERemoteHostlet_HC:
            {
            CSLOG_L(iConnectionID, KMinLogLevel ,"- session type: ERemoteHostlet_HC");
            // Remove this hostlet from the session
            if( iServiceSession )
                {
                TRAP_IGNORE( iServiceSession->SetHostletL(NULL); )
                }
            }
        break;
        case ENotInitialized:
            {
            CSLOG_L(iConnectionID, KMinLogLevel ,"- session type: ENotInitialized");
            }
        break;
        default:
            {
            CSLOG_L(iConnectionID, KMinLogLevel ,"- unknown session type.");
            }
        }
    
    //Loop and complete orphan RMessage2s
    CSLOG_L(iConnectionID, KMinLogLevel ,"- Loop and complete orphan RMessage2s");

    CSenClientSession::CompletePendingMessages();

    TInt count(iHostletRequests.Count());
    if( count > 0)
        {
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d hostlet request(s) waiting. Completing due this host is freed."), count));
        for ( TInt hr=(count-1); hr>=0; hr-- )
            {
            CSenClientMessage* pMessage = (CSenClientMessage*)iHostletRequests.ValueAt( hr ); 
            TInt* pKey = iHostletRequests.KeyAt( hr );

            if( pMessage && pKey )
                {
                MSenRemoteServiceConsumer* pConsumer = pMessage->Consumer();
                if( pConsumer )
                    {
                    // Notifying consumer
                    TPtrC8 id = pConsumer->Id();
                    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Notifying consumer (ID: '%S') that it's host is gone."), &id));
                    
                    TRAP_IGNORE( pConsumer->HandleErrorL(NULL, KErrSenCancelled, *pKey, NULL); ) 
                    }
                }
            }
        iHostletRequests.Reset(); // de-allocates the keys (pKey -pointers)
        }

    iPendingTransactions.ResetAndDestroy();
    iPendingTxnIds.ResetAndDestroy();

    if(ipHostletAwaitOp)
        {
        RMessage2& await = ipHostletAwaitOp->RMessage();
        await.Complete(ESenServRequestCancelled);
        }

    if(iServiceSession)
        {
        // Notify responsible framework, so that it 
        // can do required cleanup for the session
        CSenClientSession::ReleaseServiceSession();
        }
    delete iSendBuf;
    delete iConsumerID;
    delete ipIdentifier;
    delete iInitializer;

    delete ipTransport;
    delete ipHostletAwaitOp;
    ipHostletAwaitOp = NULL;
    iConnectionSession.Close();

    /*    
    if( iSecureId > 0 )
        {
        CSenApplicationContext& appCtx = iServerContext.OpenApplicationContextL( iSecureId );
        if( iConnectionID > KErrNotFound )
            {
            CSenClientContext& clientCtx = appCtx.OpenClientContextL( iConnectionID );
            
            
            appCtx.CloseClientContext( clientCtx );
            iServerContext.CloseApplicationContext( appCtx );
            }
        }
    */
#ifdef __ENABLE_ALR__    
    if (iSessionType == ERemoteConsumer_SC && iALRObserver != NULL)
        {
        delete iALRObserver ;
        CSLOG_L(iConnectionID, KMinLogLevel , "-iALRObserver deleted ");
        }       
#endif        //__ENABLE_ALR__

    iManager.DecrementConnections();
    CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::~CSenClientSession - END");
    #ifdef _SENDEBUG
    TLSLOG_CLOSE(KSenClientSessionLogChannelBase+iConnectionID);
    #endif
    }


RTransactionArray& CSenClientSession::Transactions()
    {
    return iPendingTransactions;
    }

RTransactionIdArray& CSenClientSession::TxnIds()
    {
    return iPendingTxnIds;
    }
    
RHostletRequestMap& CSenClientSession::HostletRequestMap()
    {
    return iHostletRequests;
    }
        
// All server side session objects have a ServiceL loop which
// will be called from CS Framework
void CSenClientSession::ServiceL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::ServiceL")));
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- RMessage2.Handle(): %d"), aMessage.Handle()));

    switch (aMessage.Function())
        {
        case ESenServInitialize:
            {
           
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServInitialize")));
            delete iSendBuf;
            iSendBuf = NULL;
            InitializeL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServInitialize Completed")));
            break;
            }
        case ESenServTransaction:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServTransaction")));
            delete iSendBuf;
            iSendBuf = NULL;
            TransactionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServTransaction Completed")));
            break;
            }
        case ESenServSendMsgAndGetTxnId:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServSendMsgAndGetTxnId")));
            delete iSendBuf;
            iSendBuf = NULL;
            SendMsg(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServSendMsgAndGetTxnId Completed")));
            break;
            }
        case ESenServInstallFramework:
            {
           
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServInstallFramework")));
            delete iSendBuf;
            iSendBuf = NULL;
            InstallFrameworkL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServInstallFramework Completed")));
            break;
            }
        case ESenServAssociateService:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServAssociateService")));
            delete iSendBuf;
            iSendBuf = NULL;
            AssociateServiceL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServAssociateService Completed")));
            break;
            }
        case ESenServDissociateService:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServDissociateService")));
            delete iSendBuf;
            iSendBuf = NULL;
            DissociateServiceL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServDissociateService Completed")));
            break;
            }
        case ESenServRegisterIdentityProvider:
            {
           
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRegisterIdentityProvider")));
            delete iSendBuf;
            iSendBuf = NULL;
            RegisterIdentityProviderL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRegisterIdentityProvider Completed")));
            break;
            }
        case ESenServUnregisterIdentityProvider:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServUnregisterIdentityProvider")));
            delete iSendBuf;
            iSendBuf = NULL;
            UnregisterIdentityProviderL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServUnregisterIdentityProvider Completed")));
            break;
            }
        case ESenServRegisterServiceDescription:
            {
           
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRegisterServiceDescription")));
            delete iSendBuf;
            iSendBuf = NULL;
            RegisterServiceDescriptionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRegisterServiceDescription Completed")));
            break;
            }
        case ESenServUnregisterServiceDescription:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServUnregisterServiceDescription")));
            delete iSendBuf;
            iSendBuf = NULL;
            UnregisterServiceDescriptionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServUnregisterServiceDescription Completed")));
            break;
            }
        case ESenServGetServiceDescriptionsByPattern:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetServiceDescriptionsByPattern")));
            ServiceDescriptionsByPatternL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetServiceDescriptionsByPattern Completed")));
            break;
            }
        case ESenServGetLengthOfServiceDescriptionByPattern:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetLengthOfServiceDescriptionByPattern")));
            delete iSendBuf;
            iSendBuf = NULL;
            ServiceDescriptionsByPatternL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetLengthOfServiceDescriptionByPattern Completed")));
            break;
            }
        case ESenServGetServiceDescriptionsByUri:
            {
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetServiceDescriptionsByUri")));
            ServiceDescriptionsByUriL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetServiceDescriptionsByUri Completed")));
            break;
            }
        case ESenServGetLengthOfServiceDescriptionByUri:
            {
           
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetLengthOfServiceDescriptionByUri")));
            delete iSendBuf;
            iSendBuf = NULL;
            ServiceDescriptionsByUriL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetLengthOfServiceDescriptionByUri Completed")));
            break;
            }
        case ESenServIsReady:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServIsReady")));
            
            delete iSendBuf;
            iSendBuf = NULL;
            IsReadyL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServIsReady Completed")));
            break;
            }
        case ESenServHasFacet:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServHasFacet")));
            
            delete iSendBuf;
            iSendBuf = NULL;
            HasFacetL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServHasFacet Completed")));
            break;
            }
        case ESenServServerMessagesOnOff:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServServerMessagesOnOff")));
            
            delete iSendBuf;
            iSendBuf = NULL;
            CompleteServerMessagesOnOffL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServServerMessagesOnOff Completed")));
            break;
            }
        case ESenServRequestServiceDescription:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRequestServiceDescription")));
            
            delete iSendBuf;
            iSendBuf = NULL;
            RequestServiceDescriptionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRequestServiceDescription Completed")));
            break;
            }
        case ESenServReceiveServiceDescription:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServReceiveServiceDescription")));
            
            ReceiveServiceDescriptionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServReceiveServiceDescription Completed")));
            break;
            }
        case ESenServStartTransaction:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServStartTransaction")));
            
            delete iSendBuf;
            iSendBuf = NULL;
            StartTransactionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServStartTransaction Completed")));
            break;
            }
        case ESenServTransactionCompleted:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServTransactionCompleted")));
            
            delete iSendBuf;
            iSendBuf = NULL;
            TransactionCompletedL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServTransactionCompleted Completed")));
            break;
            }
        case ESenServCancelSession:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServCancelSession")));
            
            CancelSessionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServCancelSession Completed")));
            break;
            }
        case ESenServSetTransportProperties:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServSetTransportProperties")));
            
            SetTransportPropertiesL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServSetTransportProperties Completed")));
            break;
            }
        case ESenServTransportProperties:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServTransportProperties")));
            
            TransportPropertiesL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServTransportProperties Completed")));
            break;
            }

        // HOSTLET OPERATIONS
        case ESenServEstablishHostletConnection:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServEstablishHostletConnection")));
            
            EstablishHostletConnectionL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServEstablishHostletConnection Completed")));
            break;
            }
        case ESenServAwaitHostletRequest:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServAwaitHostletRequest")));
            
            AwaitHostletRequestL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServAwaitHostletRequest Completed")));
            break;
            }
/*
        case ESenServAcquireRequestHandle:
            {
            LOG_WRITE_L("ESenServAcquireRequestHandle");
            //AcquireHostletRequestHandleL();
            break;
            }
*/
        case ESenServProvideHostletResponse:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServHostletResponse")));
            
            ProvideHostletResponseL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServHostletResponse Completed")));
            break;
            }
        case ESenServMoveChunk:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServMoveChunk")));
            
            MoveChunkL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServMoveChunk Completed")));
            break;
            }
        case ESenServMoveFile:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServMoveFile")));
            
            MoveFileL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServMoveFile Completed")));
            break;
            }
		case ESenServSendFileHandle:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServSendFileHandle")));
            SendFileHandleL(aMessage);
			CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServSendFileHandle Completed")));
            break;
            }            
        case ESenServGetGredentials:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetGredentials")));
            
            CredentialsL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServGetGredentials Completed")));
            break;
            }
        case ESenServAddCredential:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServAddCredential")));
            
            AddCredentialL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServAddCredential Completed")));
            break;
            }
        case ESenServRemoveCredential:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRemoveCredential")));
            
            RemoveCredentialsL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServRemoveCredential Completed")));
            break;
            }
        case ESenServCancelRequest:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServCancelRequest")));
            
            CancelRequestL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServCancelRequest Completed")));
            break;
            }
        case ESenServConnectionID:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServConnectionID")));
            
            ConnectionID(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServConnectionID Completed")));
            break;
            }
        case ESenServConnectionIdentityProvider:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServConnectionIdentityProvider")));
            
            ConnectionIdentityProviderL(aMessage);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenServConnectionIdentityProvider Completed")));
            break;
            }
        case ESenObserveCoBranding:
            {
    		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenObserveCoBranding");
    		ObserveCoBranding(aMessage);
    		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenObserveCoBranding Completed");
            break;
            }
        case ESenObserveTransfer:
            {
    		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenObserveTransfer");
    		ObserveTransfer(aMessage);
    		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenObserveTransfer Completed");
            break;
            }
        case ESenObserveAuthCallback:
		{
		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenObserveAuthCallback");
		ObserveAuthCallback(aMessage);
		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenObserveAuthCallback Completed");
		break;
		}
#ifdef __ENABLE_ALR__		
		case ESenServRegisterMobiltyObserver:
			{
			CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServRegisterMobiltyObserver");
			ObserveMobiltyService(aMessage);
			CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServRegisterMobiltyObserver Completed");
			break;
			}
		case ESenServMigrateToPreffredCarrier:
			{
			CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServMigrateToPreffredCarrier");
			MigrateToPrefferedCarrierL(aMessage);
			CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServMigrateToPreffredCarrier Completed");
			break;
			}		
		case ESenServNewCarrierAccepted:
    	    {
    		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServNewCarrierAccepted");
    		NewCarrierAcceptedL(aMessage);
    		CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServNewCarrierAccepted Completed");
    	    break;
    	    }		
#endif    	    
        case ESenTransferProgress:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenTransferProgress")));
            aMessage.Complete(SendProgressToHostlet(aMessage));
            CSLOG(iConnectionID, KMinLogLevel ,(_L("ESenTransferProgress Completed")));
            break;
            }
        case ESenServSearchIdentityProvider:
        	{
        	CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServReauthenticationNeeded");
        	SearchIdentityProviderL(aMessage);
        	CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServReauthenticationNeeded Completed");
        	break;
        	}
        case ESenTrafficDetails: 	 
            {
        	CSLOG_L(iConnectionID,KMinLogLevel ,"ESenTrafficDetails");
            DataTrafficDetails(aMessage); 
            CSLOG_L(iConnectionID,KMinLogLevel ,"ESenTrafficDetails Completed");
            break;
            }
        case ESenServGetIdentityProviders:
        	{
        	CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServGetIdentityProviders");
        	IdentityProviders(aMessage);
	       	CSLOG_L(iConnectionID,KMinLogLevel ,"ESenServGetIdentityProviders Completed");
        	break;
        	}
        default:
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("default")));
            
            CSLOG(iConnectionID, KMinLogLevel ,(_L("Calling PanicClient( EBadRequest )")));
            
            PanicClient(aMessage, EBadRequest);
            CSLOG(iConnectionID, KMinLogLevel ,(_L("default Completed")));
            break;
            }
        }
    }

TInt CSenClientSession::ParseSessionInitializerL( CSenWSDescription*& apInitializer,
                                                  const TDesC8& aSdAsXml,
                                                  const TDesC8* apConsumerPolicy )
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ParseSessionInitializerL");
    TInt retVal(KErrNone);
    CSenWSDescription* pSD = NULL;
    delete apInitializer;
    apInitializer = NULL;
    
    if(apConsumerPolicy && apConsumerPolicy->Length()>0)
        {
        CSLOG_L(iConnectionID, KMinLogLevel, "- Service Pattern received");
        pSD = CSenWSPattern::NewLC();
        pSD->SetReader(*iManager.XMLReader());
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, pSD->ParseL(aSdAsXml));
        if(leaveCode!=KErrNone)
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ParseSessionInitializerL:");
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- ParseL( SD_AS_XML ) leaved: %d"), leaveCode));
//            retVal = leaveCode; // known issue: leave code 991 from SAX framework
            }
        CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::ParseSessionInitializerL - Consumer policy before parsing:");
        CSLOG_ALL(iConnectionID, KMinLogLevel ,( *apConsumerPolicy ));

        // add the content into internal ConsumerPolicy member variable of SD..
        leaveCode = KErrNone;
        TRAP(leaveCode, pSD->ParseL(*apConsumerPolicy));
        if(retVal == KErrNone && leaveCode!=KErrNone)
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- ParseL(policy) leaved: %d"), leaveCode));
//            retVal = leaveCode; // known issue: leave code 991 from SAX framework
            }
        }
    else
        {
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::ParseSessionInitializerL: parsing XML (%d bytes) into service description"), aSdAsXml.Length()));
        pSD = CSenWSDescription::NewLC();
        pSD->SetReader(*iManager.XMLReader());
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, pSD->ParseL(aSdAsXml));
        if(leaveCode!=KErrNone)
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::ParseSessionInitializerL - ParseL(SD) leaved: %d"), leaveCode));
            retVal = leaveCode;
            }
        }

#ifdef _SENDEBUG
    HBufC8* pAsXml = pSD->AsXmlL();
    if(pAsXml)
        {
        CleanupStack::PushL(pAsXml);
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ParseSessionInitializerL");
        CSLOG_L(iConnectionID,KMinLogLevel ,"- Service description after parsing:");
        CSLOG_ALL(iConnectionID,KMinLogLevel ,(*pAsXml));
        CleanupStack::PopAndDestroy(); // pAsXml
        }
#endif // _SENDEBUG

    apInitializer = pSD;
    CleanupStack::Pop(); // pSD
    return retVal;
    }

void CSenClientSession::InitializeL(const RMessage2& aMessage)
    {
    // To cache secure id of the client
    // This secure id will be used to notify client in case RProperty updates
    iClientSecureID = aMessage.SecureId();
	
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::InitializeL");
    iSessionType = ERemoteConsumer_SC;

	CSenChunk* pChunk = CSenChunk::NewLC(KNullDesC);
	TInt retVal = pChunk->OpenChunkFromRMsgL(aMessage, 2);
	
	TPckgBuf<TFourInts> fourInts;
	aMessage.ReadL( 3, fourInts );
	
    TInt sdLength = fourInts().iInt1;
    iConnectionHasAuthenticationCallback = fourInts().iInt2;
	TPtrC8 serviceDescriptionAsXml;
    TPtrC8 consumerPolicyAsXml;
	if ( retVal == KErrNone )
        {
    	retVal = pChunk->DescFromChunk(serviceDescriptionAsXml, 0);
    	
    	if ( retVal == KErrNone )
    	    {
            TInt servDescFromChunkLength = serviceDescriptionAsXml.Length();
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- XML SD from chunk length: %d bytes"), servDescFromChunkLength));
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- XML SD from IPC args length: %d bytes"), sdLength));
            if( sdLength != servDescFromChunkLength )
                {
                serviceDescriptionAsXml.Set( serviceDescriptionAsXml.Left( sdLength ) );
                // serviceDescriptionAsXml.SetLength( sdLength );
                }

    	    // Check if consumer policy was provided
        	TInt getPolicyRetCode = pChunk->DescFromChunk(consumerPolicyAsXml, 1);
                	
        	if( getPolicyRetCode != KErrNone )
        	    {
        	    CSLOG_L(iConnectionID,KMinLogLevel ,"- Consumer policy not used.");
                consumerPolicyAsXml.Set(KNullDesC8);
        	    }
            getPolicyRetCode = 0; // not used in release builds      	    

    	    }
        }
    if ( retVal == KErrNone )
        {
        // At least XML SD was read correctly from the chunk

        // Parse the XML string into a XML service description object
        delete iInitializer;
        iInitializer = NULL;
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- Service description as XML:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,( serviceDescriptionAsXml ));
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- Consumer policy as XML as XML:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,( consumerPolicyAsXml ));
        
        retVal = ParseSessionInitializerL(iInitializer, serviceDescriptionAsXml, &consumerPolicyAsXml);
        // Attempt to initialize service session
        if ( retVal == KErrNone )
            {
            // Sanity check
            if( !iInitializer )
                {
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Fatal: initializer is NULL after succeeded parsing!");
                aMessage.Complete(ESenInternalError);
                return;
                }
                

            // Notify the framework plug-in about consumer being added
            TPtrC8 frameworkID = iInitializer->FrameworkId();
            if( frameworkID.Length() > 0 )
                {
                iFwNotified = ETrue;
                TRAP_IGNORE( iManager.NotifyFrameworksL(
                        frameworkID,
                        KSenEventConsumerAdded,
                        this); )                 
                }

            // Initialize the service session
            HBufC8* pErrorMsg = NULL;
            retVal = InitializeServiceSessionL(*iInitializer, pErrorMsg);
            CleanupStack::PushL( pErrorMsg );
            if( pErrorMsg )
                {
                TPtrC8 error = pErrorMsg->Des();
                CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- InitializeL: retVal: (%d), error msg from InitializeServiceSessionL:"), retVal));
                CSLOG_ALL(iConnectionID,KMinLogLevel ,( error ));
                pChunk->DescsToChunk( error, KNullDesC8 );
                }
            else
                {
                pChunk->DescsToChunk( KNullDesC8, KNullDesC8 );
                }
            CleanupStack::PopAndDestroy( pErrorMsg );

            // Ensure that transport plug-in will be loaded:
            if( retVal == KErrNone ) 
                {
                TRAP( retVal, TransportL(); ) 
                
                MSenTransport* pTransport = iServiceSession->Transport();
                if ( pTransport )
                    {
                    TInt ret = pTransport->SubscribeEventListenerL(*this);
                    }
                }
            }
        }
        
    CleanupStack::PopAndDestroy( pChunk );

    // Write return value to pre-reserved client descriptor
    TPtr8 returnValue(reinterpret_cast<TUint8*>(&retVal),
                      sizeof(retVal), sizeof(retVal));

    TInt leaveCode(KErrNone);
    TRAP(leaveCode, aMessage.WriteL(0, returnValue)); //IPC V2
    if ( leaveCode != KErrNone )
        {
        aMessage.Complete(ESenInternalError);
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::InitializeL:");
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- RMessage2.WriteL(0, retVal) failed, leaveCode: %d. Completing with ESenInternalError"), leaveCode));
        return;
        }

    HBufC* pId16 = HBufC::NewLC(Id().Length());
    TPtr16 id16 = pId16->Des();
    id16.Copy(Id()); // Id does not containt UCS2 chars, so direct copying is ok!
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Writing id16: %S"), &id16));
    TRAP(leaveCode, aMessage.WriteL(1, id16)); //IPC V2
    CleanupStack::PopAndDestroy(); // pId16
    if( leaveCode != KErrNone )
        {
        aMessage.Complete(ESenInternalError);
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::InitializeL:");
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- RMessage2.WriteL(1, sessionId) failed, leaveCode: %d. Completing with ESenInternalError"), leaveCode));
        return;
        }

    if (retVal == KErrSenFailedAuthentication && iAuthenticationRetries < KMaxAuthenticationRetries && iConnectionHasAuthenticationCallback)
        {
        // Authentication failed.
        iAuthenticationRetries++;
        
        if ( iConnectionHasAuthenticationCallback )
            {
            // Ask authentication information from client
            aMessage.Complete(ESenReAuthNeeded);
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("Completed with ESenReAuthNeeded iAuthenticationRetries = [%d]"), iAuthenticationRetries));		
            }
        else
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"Still we need to handle KErrSenAuthenticationFault");
            retVal = KErrSenSoapFault;
            }
        }
    else
        {
        // Complete. Either init was ok, or some error occurred; client now knows the result
        iAuthenticationRetries = 0;
        aMessage.Complete(ESenServInitialized);
        CSLOG_FORMAT((iConnectionID,KMinLogLevel ,_L8("Completed with ESenServInitialized errrcode written [%d]"), retVal));
        }
    }

TInt CSenClientSession::InitializeServiceSessionL(CSenWSDescription& aInitializer, HBufC8*& aErrorMsg)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::InitializeServiceSessionL");

    // Parsing was successful, perform the actual service connection lookup
    iManager.SetShowPasswordDialog(ETrue);
    TInt retVal = iManager.InitServiceConnectionL(*this, aInitializer, aErrorMsg);
    iManager.SetShowPasswordDialog(EFalse);

    // Sanity check that connection is valid & ready
    if(retVal == KErrNone && (!iServiceSession || iStatus != KSenConnectionStatusReady))
        {
        CSLOG_L(iConnectionID,KMinLogLevel ,"FATAL: InitServiceConnectionL() == KErrNone but ServiceSession is NOT ready!");
        retVal = KErrSenInternal; 
        }
    return retVal;
    }



void CSenClientSession::TransactionL( const RMessage2& aMessage )
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::TransactionL");
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::TransactionL - RMessage2(%d)"), aMessage.Handle()));

    MSenTransport& transport = TransportL();
//    CSenClientMessage* pMessage = CSenClientMessage::NewLC( iManager.NextTransactionId(), aMessage, this, transport );
    CSenClientMessage* pMessage = CSenClientMessage::NewLC( aMessage, this, transport );
    pMessage->SetLogger( Log() );
    MSenMessageContext& context = pMessage->MessageContext();
    context.SetChunk(pMessage, EFalse);
    
    TInt retVal = pMessage->OpenChunkFromRMsgL( pMessage->RMessage(),2 );
    if(retVal==KErrNone)
        {
//        pMessage->ChunkHeader().SetContextId( pMessage->CtxId() );
        
        // Start waiting for HandleMessageL, HandleErrorL, cancel
        // or destruction of this session...
        retVal = Transactions().Append(pMessage);
        if(retVal!=KErrNone)
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Transactions().Append failed (OOM)!");
            }
        }
#ifdef _SENDEBUG
    else
        {
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- pMessage->OpenChunkL(2) failed: %d"), retVal));
        }
#endif

    if(retVal!=KErrNone)
        {
        CleanupStack::PopAndDestroy(); // pMessage
        aMessage.Complete(ESenInternalError);
        }
    else
        {
        CleanupStack::Pop(); // pMessage
        }
    }
    
TInt CSenClientSession::CompleteTransaction(TInt aErrorCode, CSenChunk& aChunk)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CompleteTransaction");
    TInt retVal(KErrNone);
	iReauthResendNeeded = EFalse ;

    TInt previousTxnId = aChunk.ChunkHeader().TransactionId();
    aChunk.ChunkHeader().SetTransactionId(KErrNotFound);
    TInt transactionIndex = TransactionIndex(KErrNotFound);
    aChunk.ChunkHeader().SetTransactionId(previousTxnId);
    
    if ( transactionIndex != KErrNotFound )
        {
        retVal = CompleteTransactionByIndex(aErrorCode, transactionIndex);
        }
    else
        {
        retVal = KErrNotFound;
        }
    
    return retVal;
    }

TInt CSenClientSession::CompleteTransactionByCtxId(TInt aErrorCode, TInt aCtxId)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CompleteTransactionByCtxId");
    TInt retVal(KErrNone);

    TInt transactionIndex = ContextIndex(aCtxId);
    
    if ( transactionIndex != KErrNotFound )
        {
        retVal = CompleteTransactionByIndex(aErrorCode, transactionIndex);
        }
    else
        {
        retVal = KErrNotFound;
        }
    
    return retVal;
    }

TInt CSenClientSession::CompleteTransactionByIndex(TInt aErrorCode, TInt aTransactionIndex)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CompleteTransactionByIndex");

    TInt retVal(KErrNone);
    if ( aTransactionIndex == KErrNotFound ) 
        {
        return KErrNotFound;
        }

    CSenClientMessage* pClientMessage = iPendingTransactions[aTransactionIndex];
    RMessage2& message = pClientMessage->RMessage();
    TPtr8 ptr(reinterpret_cast<TUint8*>(&aErrorCode), sizeof(aErrorCode), sizeof(aErrorCode));

    TInt leaveCode(KErrNone);
    TRAP(leaveCode, message.WriteL(0, ptr)); //IPC V2
    if ( leaveCode != KErrNone )
        {
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Leave code: %d"), leaveCode));
        message.Complete(ESenInternalError); 
        CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - ErrorCode");
        retVal = leaveCode;
        }
    else
        {
        TInt txnId = pClientMessage->ChunkHeader().TransactionId();
        
        TPtr8 ptr(reinterpret_cast<TUint8*>(&txnId), 
                  sizeof(txnId), 
                  sizeof(txnId)); 

        TInt leaveCode(KErrNone);
        TRAP(leaveCode, message.WriteL(1, ptr)); //IPC V2
        if ( leaveCode != KErrNone )
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Leave code: %d"), leaveCode));
            message.Complete(ESenInternalError);
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - TxnId");
            retVal = leaveCode;
            }
        else
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- ErrCode: %d"), aErrorCode));
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- txnId  : %d"), txnId));
            message.Complete(ESenServRequestResponsed);
            CSLOG_L(iConnectionID,KMinLogLevel ,"Completed with ESenServRequestResponsed");
            }
        }

    iPendingTransactions.Remove( aTransactionIndex );
    delete pClientMessage;

    return retVal;
    }
    
TInt CSenClientSession::TransactionIndex(TInt aTxnId)
    {
    TInt count = iPendingTransactions.Count();
    TInt index(0);
    for (; index < count; index++)
        {
        CSenClientMessage* pMsg = iPendingTransactions[index];
        if (pMsg && pMsg->ChunkHeader().TransactionId() == aTxnId)
            {
            return index;    
            }
        }
    return KErrNotFound;
    }

TInt CSenClientSession::ContextIndex(TInt aCtxId)
    {
    TInt count = iPendingTransactions.Count();
    TInt index(0);
    for (; index < count; index++)
        {
        CSenClientMessage* pContext = iPendingTransactions[index];
        if (pContext && pContext->CtxId() == aCtxId)
            {
            return index;    
            }
        }
    return KErrNotFound;
    }


const TDesC8& CSenClientSession::Id() const
    {
    if(iConsumerID)
        {
        return *iConsumerID;
        }
    else
        {
        return KNullDesC8;
        }
    }

CSenIdentifier& CSenClientSession::Identifier() const
    {
    return *ipIdentifier;
    }
TInt CSenClientSession::ListBinaryElementsL(RArray<TXmlEngElement>& aElementArray,
                                            TXmlEngElement& aElement)
    {
    CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::ListBinaryElementsL");
	_LIT8(KInclude,         "Include");
	_LIT8(KXopIncludeUri,   "http://www.w3.org/2004/08/xop/include");
//	_LIT8(KHref,            "href");      // not in use atm
//	_LIT8(KHrefUriEmpty,    "");  // not in use atm
	
	TInt count(0);
	RXmlEngNodeList<TXmlEngElement> list;
	CleanupClosePushL(list);
	aElement.GetChildElements(list);
	TInt c = list.Count();

#ifdef _SENDEBUG
    TPtrC8 root = aElement.Name();
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Processing element: '%S', num of child elements: %d"), &root, c));
    TPtrC8 content = aElement.Text();
    CSLOG_L(iConnectionID, KMaxLogLevel , "----  Content START ----");
    CSLOG_ALL(iConnectionID, KMaxLogLevel , (content));
    CSLOG_L(iConnectionID, KMaxLogLevel , "----  Content END ----");
#endif // _SENDEBUG

	

    while ( list.HasNext() )
        {
        TXmlEngElement element = list.Next();
        
        TPtrC8 tag = element.Name();
        CSLOG_FORMAT((iConnectionID, KMaxLogLevel , _L8("- child: '%S'"), &tag));
        if ( tag == KInclude )
            {
            CSLOG_L(iConnectionID, KMaxLogLevel , "- <xop:Include> element found.");
            if ( element.NamespaceUri() == KXopIncludeUri )
                {
                CSLOG_L(iConnectionID, KMaxLogLevel , "- namespace == http://www.w3.org/2004/08/xop/include");
                aElementArray.AppendL(element);
                count = count + 1;
                }
            }
        count = count + ListBinaryElementsL(aElementArray, element);
        }

    CleanupStack::PopAndDestroy(&list);
    return count;
    }
TInt CSenClientSession::ParseMessageL(TInt aTransactionId,
                                      const TDesC8& aRequest,
                                      CSenAtomEntry& aAtomEntry)
    {
	CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::ParseMessageL(aTransactionId, aRequest, aAtomEntry)");
    CSenParser* pParser = CSenParser::NewLC();
    pParser->EnableFeature(EReportNamespaceMapping);
	pParser->ParseL(aRequest, aAtomEntry);
	
	// handle binary containers (BLOBs / attachments)
    //TInt retVal = TransactionIndex(aTransactionId);
    TInt retVal = ContextIndex(aTransactionId);
    if ( retVal != KErrNotFound )
        {
        RTransactionArray& transactions = Transactions();   
        CSenClientMessage* pClientMessage = transactions[retVal];
    	
		RPointerArray<CSenBinaryData>& binaryDataArray = pClientMessage->BinaryDataArray();

        if ( binaryDataArray.Count() > 0 )
            {
        	TXmlEngElement messageRootElement = aAtomEntry.AsElementL();
        	TInt count2 = binaryDataArray.Count();
            RSenDocument document = aAtomEntry.AsDocumentL();
        	
        	TXmlEngElement element;
        	CSenBinaryData* pBinaryData;
			for ( TInt j = 0; j < count2; j++ )
			    {
			    pBinaryData = binaryDataArray[j];
            	if ( pBinaryData->Type() == CSenBinaryData::EChunk )
        			{
            	    TXmlEngChunkContainer binData =
            	            document.CreateChunkContainerL(
            	                        pBinaryData->Cid(),
            	                        pBinaryData->Chunk(),
            	                        pBinaryData->Offset(),
            	                        pBinaryData->Size() );
                    document.AsElement().AppendChildL(binData);
        			}
        		else if ( pBinaryData->Type() == CSenBinaryData::EFile )
        			{
            	    TXmlEngFileContainer binData = 
            	            document.CreateFileContainerL(
            	                        pBinaryData->Cid(),
	                                    pBinaryData->File() );
        			
                    document.AsElement().AppendChildL(binData);
        			}

                }
            }
        }
	
    CleanupStack::PopAndDestroy(pParser);
    CSLOG_L(iConnectionID, KMinLogLevel ,"CSenClientSession::ParseMessageL(aTransactionId, aRequest, aAtomEntry) Completed");
    return KErrNone;
    }    
TInt CSenClientSession::ParseMessageL(TInt aTransactionId,
                                      const TDesC8& aRequest,
                                      CSenSoapEnvelope2& aSoapEnvelope)
    {
    CSLOG_L(iConnectionID, KMinLogLevel , "CSenClientSession::ParseMessageL(aTransactionId, aRequest, aSoapEnvelope)");
    CSenParser* pParser = CSenParser::NewLC();
    pParser->EnableFeature(EReportNamespaceMapping);
	pParser->ParseL(aRequest, aSoapEnvelope);


    // handle binary containers (BLOBs / attachments)
    //TInt retVal = TransactionIndex(aTransactionId);
    TInt retVal = ContextIndex(aTransactionId);
    if ( retVal != KErrNotFound )
        {
        RTransactionArray& transactions = Transactions();   
        CSenClientMessage* pClientMessage = transactions[retVal];
    	
		RPointerArray<CSenBinaryData>& binaryDataArray = pClientMessage->BinaryDataArray();

        if ( binaryDataArray.Count() > 0 )
            {
    	
        	_LIT8(KCidPrefix, "cid:*");
        	TXmlEngElement messageRootElement = aSoapEnvelope.AsElementL();
        	
        	RArray<TXmlEngElement> elementArray;
        	RXmlEngNodeList<TXmlEngAttr> attrArray;
        	CleanupClosePushL(elementArray);
            TInt binElementCount = ListBinaryElementsL(elementArray,messageRootElement);
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Count of <xop:Include elements> in DOM tree: %d"), binElementCount));
            
            TXmlEngAttr attr;
            TPtrC8 cid;
        	TInt count = elementArray.Count();
        	TInt count2 = binaryDataArray.Count();
            RSenDocument document = aSoapEnvelope.AsDocumentL();
        	
        	TXmlEngElement element;
        	CSenBinaryData* pBinaryData;
        	if ( count > 0 && count2 > 0 )
        	    {
        		for ( TInt i = 0; i < count; i++ )
        		    {
        			element = elementArray[i];
        			element.GetAttributes(attrArray);
        			while ( attrArray.HasNext() )
        				{
        				attr = attrArray.Next();
        				TPtrC8 value = attr.Value();
                    	TInt position = value.Match(KCidPrefix);
                    	if ( position < 0 )
                    		{
                    		position = 0;
                    		}
                    	cid.Set(value.Mid( position + KCidPrefix().Size() - 1)); // minus star character (*)
                    	if ( cid.Length() > 0 )
                    	    {
                    	    break;
                    	    }
        				}
        		    attrArray.Close();
        			
        			for ( TInt j = 0; j < count2; j++ )
        			    {
        			    pBinaryData = binaryDataArray[j];
        			    
                        if ( pBinaryData->Cid() == cid )
                            {
                    		if ( pBinaryData->Type() == CSenBinaryData::EChunk )
                    			{
                        	    TXmlEngChunkContainer binData =
                        	            document.CreateChunkContainerL(
                        	                        pBinaryData->Cid(),
                        	                        pBinaryData->Chunk(),
                        	                        pBinaryData->Offset(),
                        	                        pBinaryData->Size() );
                                element.ReplaceWith(binData);
                    			}
                    		else if ( pBinaryData->Type() == CSenBinaryData::EFile )
                    			{
                        	    TXmlEngFileContainer binData = 
                        	            document.CreateFileContainerL(
                        	                        pBinaryData->Cid(),
            	                                    pBinaryData->File() );
                    			
                                element.ReplaceWith(binData);
                    			}

                            break;
                            }
        			    }
        		    }
        		}        
            CleanupStack::PopAndDestroy(&elementArray);
            }
        }
    CleanupStack::PopAndDestroy(pParser);
    CSLOG_L(iConnectionID, KMinLogLevel , "CSenClientSession::ParseMessageL(aTransactionId, aRequest, aSoapEnvelope) Completed");
    return retVal;
    }
    
TInt CSenClientSession::SearchMsgChunk( const RMessage2& aMessage, CSenChunk*& aHit )
    {
    TInt ctxId = aMessage.Int1();
    TInt ctxIndex = ContextIndex( ctxId );
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("SearchMsgChunk - using Context ID: %d"), ctxId));
    if ( ctxIndex == KErrNotFound ) 
        {
        return ctxIndex;
        }
    aHit = iPendingTransactions[ctxIndex];
    return KErrNone;
    }
    
void CSenClientSession::SendMsg( const RMessage2& aMessage )
    {
    CSLOG_L(iConnectionID, KMinLogLevel , "CSenClientSession::SendMsg");
    CSenChunk* pSenChunk = NULL;
    TInt err = SearchMsgChunk( aMessage, pSenChunk );
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- SearchMsgChunk returned: %d"), err));
    
    if ( err || !pSenChunk )
        {
        CSLOG_L(iConnectionID, KMinLogLevel , "CSenClientSession::SendMsg - FATAL(!): transaction not found by msg ctx ID!");
        aMessage.Complete( KErrSenInternal );
        }
    else
        {
        TRAP( err, SendMsgL( aMessage, *pSenChunk );  )
        
        if ( err != KErrNone )
            {
            aMessage.Complete( err );
            // Complete pending transaction, too
            CompleteTransaction( err, *pSenChunk );
            }
        }
    }

void CSenClientSession::SendMsgL(const RMessage2& aMessage, CSenChunk& aSenChunk)
    {
   CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SendMsgL");
    
    TPtrC8 request;
    TInt errorCode(KErrNone);
    TPtrC8 transportProperties(KNullDesC8);

    HBufC8* pErrorMsg = NULL;
	TBool initReqd = EFalse ;
	TBool sessionValidityError = SessionValidity() ;	
   if ( !aSenChunk.ChunkHeader().DoInitBeforeSending() )
	{
	if (KErrNone != sessionValidityError)
		{
		initReqd  = ETrue ;
		CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SendMsgL - init required ??? ");
		}
   	}
    if ( aSenChunk.ChunkHeader().DoInitBeforeSending() || initReqd)
        {
        iManager.SetShowPasswordDialog(ETrue);
        errorCode = iManager.InitServiceConnectionL(*this, *iInitializer, pErrorMsg);
        CleanupStack::PushL( pErrorMsg );
        if ( errorCode )
            {
			// Write error message to chunk only if there will be no more reauthentication loops
            // otherwise resend after successful reauthentication will fail
            if( errorCode != KErrSenFailedAuthentication || !(iAuthenticationRetries < KMaxAuthenticationRetries) )
            	{
	            if( pErrorMsg )
	                {
	                TPtrC8 errorMsg = pErrorMsg->Des();
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- SendMsgL - re-auth error from InitializeL: retVal: (%d), error msg from InitializeServiceSessionL:"), errorCode));
                    CSLOG_ALL(iConnectionID,KMinLogLevel ,( errorMsg ));
	                aSenChunk.DescToChunk( errorMsg ); // 2007-02-03: no longer silenced..
	                aSenChunk.ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
	                }
	            else 
	                {
	                // Replace sent message (request) with error message
	                // (in this case there is no error message => return KNullDesC8)
	                // NOTE(!): do not zero-length the request if there was no error, 
	                // otherwise re-send will fail(!)
	                aSenChunk.DescToChunk( KNullDesC8 );
	                aSenChunk.ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
	                }
				}
            }
        CleanupStack::PopAndDestroy( pErrorMsg );
        pErrorMsg = NULL; // pointer is re-used
        iManager.SetShowPasswordDialog(EFalse);                
        
        // Sanity check that connection is valid & ready
        if ( errorCode == KErrNone && (!iServiceSession || iStatus != KSenConnectionStatusReady) )
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"FATAL: InitServiceConnectionL() == KErrNone but ServiceSession is NOT ready!");
            errorCode = KErrSenInternal;
            }
        }
        
    if ( errorCode == KErrNone )
        {
		if (!initReqd && !iReauthResendNeeded)
	    {
	    errorCode = sessionValidityError ;
	    }
        if ( errorCode == KErrNone )
            {
            //if (aSenChunk.ChunkHeader().HasTransactionProperties() )
                            
            if ( aSenChunk.ChunkHeader().PropertiesType() != MSenProperties::ENotInUse )
                {
                errorCode = aSenChunk.DescsFromChunk(request, transportProperties);
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Has transport properties (%d bytes."), transportProperties.Length()));
                }
            else
                {
                if (aSenChunk.RequestFileHandle().SubSessionHandle()) //request is null Now !!
	                {
	                errorCode = KErrNone ;	
	                }
                else
		            {
                	errorCode = aSenChunk.DescFromChunk(request);
					}
                }
            }
        }

    if ( errorCode != KErrNone )
        {
	CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::SendMsgL() Fail with Error: = [%d], sessionValidityError = [%d]"), errorCode, sessionValidityError));
        //when there is a soapfault and session is expired, it is auth-fault
        if (KErrConnectionExpired == sessionValidityError && (KErrSenSoapFault == errorCode || KErrConnectionExpired == errorCode))
			{
			CSLOG_L(iConnectionID,KMaxLogLevel ,"-session is expired/soapfault, translating the error to  KErrSenAuthenticationFault");
			errorCode = KErrSenAuthenticationFault ;
			}
			
    	aMessage.Complete(errorCode);
    	CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Completed with errorCode: %d"), errorCode));
        
	    if( (KErrSenAuthenticationFault == errorCode || KErrSenFailedAuthentication  == errorCode
	     || KErrConnectionExpired == errorCode) && iAuthenticationRetries < KMaxAuthenticationRetries)
        	{
        	iAuthenticationRetries++;
			CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::SendMsgL() iAuthenticationRetries = [%d]"), iAuthenticationRetries));
        	CompleteReauthLoop(aSenChunk);
        	return;
        	}
        else
        	{
        	iAuthenticationRetries = 0;
	        CompleteTransaction(errorCode, aSenChunk);
	        return;
        	}
        }

    iAuthenticationRetries = 0; // reauthentication loop ends

    delete pErrorMsg; // In all cases, pErrorMsg should always NULL in here..
    pErrorMsg = NULL;


#ifdef _SENDEBUG
    CSLOG_FORMAT((iConnectionID, KMaxLogLevel , _L8("=================== SendL(%d bytes):"), request.Length()));
    CSLOG_ALL(iConnectionID, KMaxLogLevel ,(request));
    if( transportProperties.Length()>0 )    
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- Message layer transport properties:");
        CSLOG_ALL(iConnectionID, KMaxLogLevel ,(transportProperties));
        }
    else
        {
        CSLOG_L(iConnectionID,KMinLogLevel ,"- No message layer transport properties.");
        }
//    FILELOGALL(_L("WsLog"), _L("service_body_req.xml"), request);
#endif

    if(iAllowShowDialogCount<1)
        {
        // flag will stay up as long as count > 0

        iManager.SetShowPasswordDialog(ETrue); 
        }
    iAllowShowDialogCount++;
	//Proxy URL CR
	CSenTransportProperties * pTransportProperties = CSenTransportProperties::NewL(transportProperties, *iManager.XMLReader());
	
	HBufC8* pTransportPropertiesProxyUrl = NULL ;	
	TPtrC8 aProxyUrl;

	MSenTransport& transport = TransportL() ;
	MSenProperties& property = transport.PropertiesL() ;
	switch (property.PropertiesClassType())
    	{
		case MSenProperties::ESenLayeredTransportProperties:
		case MSenProperties::ESenLayeredHttpTransportProperties:
		case MSenProperties::ESenLayeredVtcpTransportProperties:
			{
			CSenLayeredTransportProperties& layerdPropery = (CSenLayeredTransportProperties&)property ;
			TInt error = layerdPropery.PropertyL(KSenConnectionProxyUrl,aProxyUrl) ;
			if(!error) 
    			{
        		CSLOG_L(iConnectionID,KMinLogLevel ,"- Found New Proxy URL");
            	CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- New proxy url('%S'."), &aProxyUrl));
    			//pTransportProperties->SetProxyHostL(aProxyHost) ; 
    			pTransportProperties->SetPropertyL(KSenConnectionProxyUrl,aProxyUrl);
		    	pTransportPropertiesProxyUrl = pTransportProperties->AsUtf8LC() ;    			
    			}
			}    			
    		break;
    	}
		
		delete pTransportProperties;		    	
		pTransportProperties = NULL;			
		
	    if ( pTransportPropertiesProxyUrl )
	    	{
        	CSLOG_L(iConnectionID,KMinLogLevel ,"- New Proxy URL has been set.");
	    	transportProperties.Set( pTransportPropertiesProxyUrl->Des() ) ;
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Transport properties with new proxy url(%d bytes."), transportProperties.Length()));
 	    	
	    	}

//    TInt transactionId( KErrNotFound );
    
    // Use the Core generated transaction ID when talking to transports
    // Using this ID, the transports can find message context associated
    // to this request / transaction.
    TInt transactionId( aSenChunk.ChunkHeader().ContextId() );
#ifdef __ENABLE_ALR__	
	HBufC8* pNewTransportProperties = NULL ;
	if ( iSessionType != ERemoteHostlet_HC )
		{
		//Only real consumers will monitor Mobility service
		if ( !iALRObserver )
			{
        	CSLOG_L(iConnectionID, KMinLogLevel , "SendMsgL - creating new ALR observer");
		  	iALRObserver = CALRObserver::NewL ( *this, *iManager.XMLReader() ) ;
		  	}	
		MSenTransport& transport = TransportL() ;
		//before opening connection we need to check the request is coming from Internal Consumers 
		//or real service consumer
        CSLOG_L(iConnectionID, KMinLogLevel , "SendMsgL - calling OpenConnectionL");
	  	TInt error = iALRObserver->OpenConnectionL( transportProperties, transport, *iInitializer, pNewTransportProperties ) ;		
	    if ( pNewTransportProperties )
	    	{
	        CSLOG_L(iConnectionID, KMinLogLevel , "SendMsgL - OpenConnectionL new transport properties set");
	    	transportProperties.Set( pNewTransportProperties->Des() ) ;
     	    CleanupStack::PushL( pNewTransportProperties );
	    	}
		if (error != KErrNone)
			{
			//ALR boot up failed , still iap can be collected in plugin 
	        CSLOG_FORMAT( (iConnectionID, KMinLogLevel , _L8("SendMsgL - OpenConnectionL returned, error: %d ALR boot up failed"), error) );
			}
		//else
		//	{	
		}
#endif	//__ENABLE_ALR__
    TInt sendRetCode(KErrNone);
    if ( aSenChunk.ChunkHeader().MessageType() == MSenMessage::ESoapMessage2 )
        {
        CSenSoapMessageDom2* pSoapMessage = CSenSoapMessageDom2::NewL(); 
        CleanupStack::PushL(pSoapMessage);
        ParseMessageL(transactionId, request, *pSoapMessage);
        TInt ctxLookupErr(KErrNone);
        MSenMessageContext* pCtx = MessageContextByTxnIdL( transactionId, ctxLookupErr );
        if( ctxLookupErr == KErrNone && pCtx )
            {
            sendRetCode = pCtx->SetMessage( pSoapMessage, ETrue );
            
            if( sendRetCode == KErrNone )
                {
                // Ownerships successfully transferred to ctx:
                CleanupStack::Pop(pSoapMessage);

                // Send like a "legacy SOAP message"
                sendRetCode = iServiceSession->SendSoapL( request,
                                                          transportProperties,
                                                          *this,
                                                          transactionId,
                                                          pErrorMsg );
                if ( pErrorMsg )
                    {
                    CleanupStack::PushL( pErrorMsg );
                    }
                aSenChunk.ChunkHeader().SetContextId(transactionId); // temporary
                CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SendMsgL - SetContextId: %d"), transactionId));
                }
            else 
                {
                // "Add to ctx" -setter method failed, delete orphan msg:
                CleanupStack::PopAndDestroy(pSoapMessage);
                CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SendMsgL - Add msg to ctx failed, error: %d"), sendRetCode));
                }
            }
        else 
            {
            // Message ctx not found, delete orphan msg:
            CleanupStack::PopAndDestroy(pSoapMessage);

            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SendMsgL - MsgCtx lookup failed, error: %d"), ctxLookupErr));
            if( ctxLookupErr == KErrNone && pCtx == NULL)
                {
                // lookup retCode == OK, but pCtx == NULL
                ctxLookupErr = KErrSenInternal;
                }
            sendRetCode = ctxLookupErr; // here one could generalize to: KErrSenInternal; 
            }
        }
    else if ( aSenChunk.ChunkHeader().MessageType() == MSenMessage::EAtomMessage )
        {
        CSenAtomEntry* pAtomEntry = CSenAtomEntry::NewL(); 
        CleanupStack::PushL(pAtomEntry);
        ParseMessageL(transactionId, request, *pAtomEntry);
        TInt ctxLookupErr(KErrNone);
        MSenMessageContext* pCtx = MessageContextByTxnIdL( transactionId, ctxLookupErr );
        if( ctxLookupErr == KErrNone && pCtx )
            {
            sendRetCode = pCtx->SetMessage( pAtomEntry, ETrue );
            if( sendRetCode == KErrNone )
                {
                // Ownerships successfully transferred to ctx:
                CleanupStack::Pop(pAtomEntry);

                // Send like a "legacy SOAP message"
                sendRetCode = iServiceSession->SendL( request,
                                                          transportProperties,
                                                          *this,
                                                          transactionId,
                                                          pErrorMsg );
                if ( pErrorMsg )
                    {
                    CleanupStack::PushL( pErrorMsg );
                    }
                aSenChunk.ChunkHeader().SetContextId(transactionId); // temporary
				CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SendMsgL - SetContextId: %d"), transactionId));
                }
            else 
                {
                // "Add to ctx" -setter method failed, delete orphan msg:
                CleanupStack::PopAndDestroy(pAtomEntry);
				CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SendMsgL - Add msg to ctx failed, error: %d"), sendRetCode));
                }
            }
        else 
            {
            // Message ctx not found, delete orphan msg:
            CleanupStack::PopAndDestroy(pAtomEntry);
			CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SendMsgL - MsgCtx lookup failed, error: %d"), ctxLookupErr));
            if( ctxLookupErr == KErrNone && pCtx == NULL)
                {
                // lookup retCode == OK, but pCtx == NULL
                ctxLookupErr = KErrSenInternal;
                }
            sendRetCode = ctxLookupErr; // here one could generalize to: KErrSenInternal; 
            }
        }
    else if ( aSenChunk.ChunkHeader().MessageType() == MSenMessage::ESoapMessage )
        {
        sendRetCode = iServiceSession->SendSoapL( request,
                                                  transportProperties,
                                                  *this,
                                                  transactionId,
                                                  pErrorMsg );
                                                  
        if ( pErrorMsg )
            {
            CleanupStack::PushL( pErrorMsg );
            }
        aSenChunk.ChunkHeader().SetContextId(transactionId); // temporary
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("SendMsgL - SetContextId: %d"), transactionId));
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("SendMsgL - TxnId: %d, sendRetCode: %d"), transactionId, sendRetCode));
        }
    else
        {
        sendRetCode = iServiceSession->SendL( request, 
                                              transportProperties, 
                                              *this, 
                                              transactionId,
                                              pErrorMsg );
                                              
        if ( pErrorMsg )
            {
            CleanupStack::PushL( pErrorMsg );
            }
        aSenChunk.ChunkHeader().SetContextId(transactionId); // temporary
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("SendMsgL - SetContextId: %d"), transactionId));
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- TxnId from SendL: %d, sendRetCode: %d"), transactionId, sendRetCode));
        }

    if ( sendRetCode == KErrNone )
        {
        if ( pErrorMsg )
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Minor (internal error): sendRetCode == KErrNone but pErrorMsg != NULL(!)");
            CleanupStack::PopAndDestroy( pErrorMsg );
            }
        TInt* txnId = new (ELeave) TInt(transactionId);
        CleanupStack::PushL(txnId);

        TInt appendRetCode = TxnIds().Append(txnId);
        if(appendRetCode==KErrNone)
            {
            CleanupStack::Pop(txnId); // now owned by pointer array

            TInt previousTxnId = aSenChunk.ChunkHeader().TransactionId();
            if (previousTxnId != 0)
                {
                TInt pendingTxnId = aSenChunk.ChunkHeader().PendingTransactionId();
                if (pendingTxnId == 0)
                    {
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("*** PendingTransactionId: %d"), previousTxnId));
                    aSenChunk.ChunkHeader().SetPendingTransactionId(previousTxnId);
                    }
                }
            aSenChunk.ChunkHeader().SetTransactionId( transactionId );
            CSLOG_L(iConnectionID,KMinLogLevel ,"SendL() OK, attempt to complete with TxnID:");
            
            // OK! Complete with TxnID:
            aMessage.Complete(transactionId); // OK!
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Completed with TxnId: %d"), transactionId));
            // NOW TRANSACTION RMessage2 is pending and waiting for HandleMessageL
            // or HandleErrorL callback, or Cancel() or destruction of this session!
            }
        else
            {
            CleanupStack::PopAndDestroy(txnId);
            // OOM(?), Append failed
            CSLOG_L(iConnectionID,KNormalLogLevel ," - TxnIds().Append(txnId)");
            aMessage.Complete(appendRetCode);
            // Complete on pending transaction, too
            CompleteTransaction(appendRetCode, aSenChunk);
            }
        }
    else // an error -- or signal about one-way message -- was received from transport
        { 
        if( pErrorMsg )
            {
            aSenChunk.DescToChunk( *pErrorMsg );
            aSenChunk.ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
            CleanupStack::PopAndDestroy(pErrorMsg); // delete pErrorMsg;
            }

        // SendL() failed, complete with error
        CSLOG_L(iConnectionID,KMinLogLevel ,"SendL() failed, complete this plus TXN (second RMessage2) immediately:");
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Error code: %d"), sendRetCode));

        if ( sendRetCode == KErrSenTransactionComplete )
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Transport plug-in interprets sent request to be a ONE-WAY MESSAGE, completing with KErrNone & KNullDesC8");
            aSenChunk.DescToChunk( KNullDesC8 );
            aSenChunk.ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
            sendRetCode = KErrNone; // Consider: updating API documentation, 
                                    // publishing KErrSenTransactionComplete,
                                    //  returning it instead of KErrNone
            }

        aMessage.Complete(sendRetCode);
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Completed with errorCode: %d"), sendRetCode));
        // Complete one pending transaction, too
        CompleteTransaction(sendRetCode, aSenChunk);
        } 
        //Proxy URL CR
        if (pTransportPropertiesProxyUrl != NULL)
            {
    	        CleanupStack::PopAndDestroy(pTransportPropertiesProxyUrl);  
            }
            
#ifdef __ENABLE_ALR__	
        if (pNewTransportProperties != NULL)
            {
    	        CleanupStack::PopAndDestroy(pNewTransportProperties);  
            }
#endif	//__ENABLE_ALR__
    }

void CSenClientSession::InstallFrameworkL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::InstallFrameworkL")));
    TInt retVal(KErrNone);
    TInt length = aMessage.Int1();
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::InstallFrameworkL framework length %d"), length));
    if (length < 1)
        {
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(length);
    TPtr8 inBuffer = inBuf->Des();

    //get message data into buf (via ptr)
    aMessage.ReadL(0, inBuffer);
    TRAPD(err, iManager.InstallFrameworkL(inBuffer));
    if (err != KErrNone)
        {
        retVal = KErrNotFound;
        }
    CleanupStack::PopAndDestroy(); // inBuf;
    aMessage.Complete(retVal);
    }

void CSenClientSession::AssociateServiceL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KNormalLogLevel ,(_L("CSenClientSession::AssociateServiceL")));

    TInt servLength = aMessage.Int1();
    TInt provLength = aMessage.Int3();

    if (servLength < 1 || provLength < 1)
        {
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    HBufC8* pServiceID = HBufC8::NewLC(servLength);
    HBufC8* pProviderID = HBufC8::NewLC(provLength);
    TPtr8 serviceID = pServiceID->Des();
    TPtr8 providerID = pProviderID->Des();

    // Read IPC arguments from RMessage2:
    aMessage.ReadL(0, serviceID);
    aMessage.ReadL(2, providerID);


    CSenIdentityProvider* pIdp = CSenIdentityProvider::NewLC(KNullDesC8);
    pIdp->SetProviderID(providerID); // id for this IDP
    pIdp->SetServiceID(serviceID);   // associated web service

    TInt error(KErrNone);
    TBool idpAlreadyExists;
    error = iManager.ContainsIdentityProviderL(idpAlreadyExists, *pIdp);
    

    if (error == KErrNone && idpAlreadyExists)
        {
        CSLOG(iConnectionID, KMaxLogLevel ,(_L("     IDP data already existed")));
        CleanupStack::PopAndDestroy(3); // pIdp, pProviderID, pServiceID 
        aMessage.Complete(error);
        return;
        }

    if (!CheckAllowRegisterL(error, aMessage, pIdp->FrameworkId()))
        {
        CleanupStack::PopAndDestroy(3); // pIdp, pProviderID, pServiceID
        aMessage.Complete(error);
        return;
        }
    
    TInt retVal(KErrNotFound);
    if (iManager.AssociateServiceL(serviceID, providerID))
        {
        retVal = KErrNone;
        }
    CleanupStack::PopAndDestroy(3); // pIdp, pProviderID, pServiceID
    aMessage.Complete(retVal);
    }

void CSenClientSession::DissociateServiceL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::DissociateServiceL")));
    TInt servLength = aMessage.Int1();
    TInt provLength = aMessage.Int3();

    if (servLength < 1 || provLength < 1)
        {
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    TInt error(KErrNone);
    if (!CheckAllowUnRegisterL(error, aMessage))
        {
        aMessage.Complete(error);
        return;
        }

    HBufC8* serviceID = HBufC8::NewLC(servLength);
    HBufC8* providerID = HBufC8::NewLC(provLength);
    TPtr8 ptrServiceID = serviceID->Des();
    TPtr8 ptrProviderID = providerID->Des();

    //get message data into buffers (via ptr)
    aMessage.ReadL(0, ptrServiceID);
    aMessage.ReadL(2, ptrProviderID);

    TInt retVal(KErrNotFound);
    if (iManager.DissociateServiceL(ptrServiceID, ptrProviderID))
        {
        retVal = KErrNone;
        }
    CleanupStack::PopAndDestroy(2); // providerID, serviceID
    aMessage.Complete(retVal);
    }

TBool CSenClientSession::CheckAllowRegisterL(TInt& aError, const RMessage2& aMessage, const TDesC8& aFrameworkId)
    {
    if(aFrameworkId == KDefaultOviFrameworkID)
    	{
    	if (!IsTrustedClient(aMessage))
    	    {
    	    aError = KErrPermissionDenied;
    	    return EFalse;
    	    }
    	}
#if defined ( RD_SEN_DISABLE_USER_PERMISSION_DIALOGS )
    iAllowWSDataReg = EAccepted;
#endif
        
    if (iAllowWSDataReg == ENotChecked)
        {
        aError = KErrNone;
        RNotifier notifier;
        User::LeaveIfError(notifier.Connect());
        CleanupClosePushL(notifier);

        TRequestStatus reqStatus;
        TPckgBuf<TAllowRegisterDlgRequest>* request = NULL;
        TPckgBuf<TAllowRegisterDlgResponse>* response = NULL;

        request = new(ELeave)TPckgBuf<TAllowRegisterDlgRequest>();
        CleanupStack::PushL(request);
        response = new(ELeave)TPckgBuf<TAllowRegisterDlgResponse>();
        CleanupStack::PushL(response);

        notifier.StartNotifierAndGetResponse(reqStatus, 
            KSenNotifierPluginUID, *request, *response);

        User::WaitForRequest(reqStatus);

        
        aError = reqStatus.Int();

        if(reqStatus.Int() == KErrNone)
            {
            if((*response)().OkButtonPressed())
                {
                iAllowWSDataReg = EAccepted;
                }
            else
                {
                iAllowWSDataReg = ENotAccepted; 
                // Overwrite KErrNone to indicate
                // that end-user denies permission

                aError = KErrSenNoEndUserPermission;
                }
            }
        else if(reqStatus.Int() == KErrNotFound)
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("Plugin notifier impl. was not found.")));


#ifdef _SENDEBUG
            // To allow console tests (debugging) to complete,
            // always DO grant permission if plug-in is not found.
            // THIS MUST BE USED ONLY IN DEBUG BUILDS, NEVER
            // for RELEASE (UREL) BUILDS == H/W!
            CleanupStack::PopAndDestroy(2); // request, response;
            CleanupStack::PopAndDestroy(); // close the notifier
            CSLOG(iConnectionID, KMinLogLevel ,(_L("** NOTE ** permission to manipulate WS data given!.")));

            aError = KErrNone; // for test driver compatibility
            return ETrue; // we can now allow WS data manipulation
                          // in debugging builds.

#else
    #ifndef EKA2 // EKA1 / S60 2nd edition devices -> temporary solution, since notifier plugin does not install!
            CleanupStack::PopAndDestroy(2); // request, response;
            CleanupStack::PopAndDestroy(); // close the notifier
            CSLOG(iConnectionID, KMinLogLevel ,(_L("** NOTE ** permission to manipulate WS data given!.")));

            aError = KErrNone; 
            return ETrue; 
    #endif
#endif
            }
        else
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("NotifierPluginImpl error: (%d)"), reqStatus.Int()));
            }

        CleanupStack::PopAndDestroy(2); // request, response;
        CleanupStack::PopAndDestroy(); // close the notifier
        }

    if (iAllowWSDataReg == EAccepted) 
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
    }

TBool CSenClientSession::CheckAllowUnRegisterL(TInt& aError, const RMessage2& aMessage)
	{
	return CheckAllowUnRegisterL(aError, aMessage, KNullDesC8);
	}
TBool CSenClientSession::CheckAllowUnRegisterL(TInt& aError, const RMessage2& aMessage, const TDesC8& aFrameworkId)
    {
    if(aFrameworkId == KDefaultOviFrameworkID)
    	{
    	if (!IsTrustedClient(aMessage))
    	    {
    	    aError = KErrPermissionDenied;
    	    return EFalse;
    	    }
    	}
#if defined ( RD_SEN_DISABLE_USER_PERMISSION_DIALOGS )
    iAllowWSDataUnReg = EAccepted;
#endif

    if (iAllowWSDataUnReg == ENotChecked)
        {
        aError = KErrNone;
        RNotifier notifier;
        User::LeaveIfError(notifier.Connect());
        CleanupClosePushL(notifier);

        TRequestStatus reqStatus;
        TPckgBuf<TAllowUnRegisterDlgRequest>* request = NULL;
        TPckgBuf<TAllowUnRegisterDlgResponse>* response = NULL; 

        request = new(ELeave)TPckgBuf<TAllowUnRegisterDlgRequest>();
        CleanupStack::PushL(request);
        response = new(ELeave)TPckgBuf<TAllowUnRegisterDlgResponse>();
        CleanupStack::PushL(response);

        notifier.StartNotifierAndGetResponse(reqStatus, 
            KSenNotifierPluginUID, *request, *response);

        User::WaitForRequest(reqStatus);

        aError = reqStatus.Int();

        
        if(reqStatus.Int() == KErrNone)
            {
            if((*response)().OkButtonPressed())
                {
                iAllowWSDataUnReg = EAccepted;
                }
            else
                {
                iAllowWSDataUnReg = ENotAccepted;
                // Overwrite KErrNone to indicate
                // that end-user denies permission

                aError = KErrSenNoEndUserPermission;
                }
            }
        else if(reqStatus.Int() == KErrNotFound)
            {
            CSLOG(iConnectionID, KMinLogLevel ,(_L("Plugin notifier impl. was not found")));

#ifdef _SENDEBUG
            // To allow console tests (debugging) to complete,
            // always DO grant permission if plugin is not found.
            // THIS MUST BE USED ONLY IN DEBUG BUILDS, NEVER
            // for RELEASE (UREL) BUILDS!
            CleanupStack::PopAndDestroy(2); // request, response;
            CleanupStack::PopAndDestroy(); // close the notifier
            CSLOG(iConnectionID, KMinLogLevel ,(_L("** NOTE ** permission to manipulate WS data given!.")));

            aError = KErrNone; // for test driver compatibility
            return ETrue; // we can now allow WS data manipulation
                          // in debugging builds.
            
#endif
            }
        else
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("Plugin notifier impl., Error: %d"), reqStatus.Int()));
            }

        CleanupStack::PopAndDestroy(2); // request, response;
        CleanupStack::PopAndDestroy(); // close the notifier
        }

    if (iAllowWSDataUnReg == EAccepted) 
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
    }

void CSenClientSession::RegisterIdentityProviderL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::RegisterIdentityProviderL")));

    TInt length = aMessage.Int1();
    if (length < 1)
        {
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(length);
    TPtr8 inBuffer = inBuf->Des();
    aMessage.ReadL(0, inBuffer);

#ifdef _SENDEBUG
    if( inBuf )
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- IdentityProvider description (as XML):");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(inBuffer));
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        }
#endif    


    CSenIdentityProvider* pdp = CSenIdentityProvider::NewLC(KNullDesC8);

    // parse into XML object.
    pdp->SetReader(*iManager.XMLReader());
    pdp->ParseL(inBuffer);

    TInt error(KErrNone);
    TBool idpAlreadyExists;
    error = iManager.ContainsIdentityProviderL(idpAlreadyExists, *pdp);

    if (error == KErrNone && idpAlreadyExists)
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"     IDP data already existed");
        CleanupStack::PopAndDestroy(2); // pdp, inBuf
        aMessage.Complete(error);
        return;
        }

    if (!CheckAllowRegisterL(error, aMessage, pdp->FrameworkId()))
        {
        CleanupStack::PopAndDestroy(2); // pdp, inBuf (fix from wk44)
        aMessage.Complete(error);
        return;
        }

    // PLEASE NOTE(!): ownerships is always transferred to manager!
    TInt retVal = iManager.RegisterIdentityProviderL(pdp);
    CleanupStack::Pop(); //pdp -> ownership is now elsewhere...
    CleanupStack::PopAndDestroy(); // inBuf
    aMessage.Complete(retVal);
    }

void CSenClientSession::UnregisterIdentityProviderL(const RMessage2& aMessage)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::UnregisterIdentityProviderL");
    TInt length = aMessage.Int1();
    if (length < 1)
        {
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    TInt error(KErrNone);
    if (!CheckAllowUnRegisterL(error, aMessage))
        {
        aMessage.Complete(error);
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(length);
    TPtr8 inBuffer = inBuf->Des();
    aMessage.ReadL(0, inBuffer);

#ifdef _SENDEBUG
    if( inBuf )
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- IdentityProvider description (as XML):");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(inBuffer));
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        }
#endif    
    
    CSenIdentityProvider* pdp = CSenIdentityProvider::NewLC(KNullDesC8);

    pdp->SetReader(*iManager.XMLReader());
    pdp->ParseL(inBuffer);
    if (!CheckAllowUnRegisterL(error, aMessage, pdp->FrameworkId()))
        {
        CleanupStack::PopAndDestroy(); // pdp
        aMessage.Complete(error);
        return;
        }
    TInt retVal(KErrNotFound);
    retVal = iManager.UnregisterIdentityProviderL(*pdp);


// Note: after "protected credentials" -change => no need to remove credential(s) anymore, since they are protected by account details (userinfo from IDP)
//   if ( retVal == KErrNone )
//        {
//        iManager.RemoveCredentialsL(idp->ProviderID());
//        }

    CleanupStack::PopAndDestroy(); // pdp // in the future ownership will be
                                          // transferred -> Pop is enough

    CleanupStack::PopAndDestroy(); // inBuf
    aMessage.Complete(retVal);
    }

void CSenClientSession::RegisterServiceDescriptionL(const RMessage2& aMessage)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::RegisterServiceDescriptionL");

    TInt length = aMessage.Int1();
    if(length < 1)
        {
        CSLOG_L(iConnectionID,KMinLogLevel ,"Tried to register ZERO length SD.");
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(length);
    TPtr8 inBuffer = inBuf->Des();
    aMessage.ReadL(0, inBuffer);

#ifdef _SENDEBUG
    if( inBuf )
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- XML SD description (as XML) before parsing:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*inBuf));
        
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        }
#endif 

    CSenWSDescription* pSd = CSenWSDescription::NewLC();
    pSd->SetReader(*iManager.XMLReader());
    pSd->ParseL(inBuffer);

#ifdef _SENDEBUG
    HBufC8* pAsXml = NULL;
    pAsXml = pSd->AsXmlL();
    if(pAsXml)
        {
        CleanupStack::PushL(pAsXml);
        CSLOG_L(iConnectionID,KMaxLogLevel ,"  SD after parsing:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*pAsXml));
        CleanupStack::PopAndDestroy(); // pAsXml
        }
    else
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- SD as XML was NULL after parsing(!)");
        }
#endif // _SENDEBUG

    TInt error(KErrNone);
    TBool dataAlreadyExists;
    error = iManager.ContainsServiceDescriptionL(dataAlreadyExists, *pSd);

    if (error == KErrNone && dataAlreadyExists)
        {
        CleanupStack::PopAndDestroy(2); // pSd, inBuf
        aMessage.Complete(error);
        CSLOG_L(iConnectionID,KMaxLogLevel ,"  SD was already found, no need to register.");
        return;
        }

    if (!CheckAllowRegisterL(error, aMessage, pSd->FrameworkId()))
        {
        CleanupStack::PopAndDestroy(2); // pSd, inBuf
        aMessage.Complete(error);
        return;
        }

    // NOTE(!): RegisterServiceDescriptionL() always takes the ownership!
    TInt retVal(iManager.RegisterServiceDescriptionL(pSd));
    CleanupStack::Pop(); // pSd, ownership was transferred
    CleanupStack::PopAndDestroy(); // inBuf
    aMessage.Complete(retVal);
    }

void CSenClientSession::UnregisterServiceDescriptionL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::UnregisterServiceDescriptionL")));
    TInt length = aMessage.Int1();
    if(length < 1)
        {
        CSLOG(iConnectionID, KMaxLogLevel ,(_L("Tried to unregister ZERO length SD.")));
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    TInt error(KErrNone);
    if (!CheckAllowUnRegisterL(error, aMessage))
        {
        aMessage.Complete(error);
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(length);
    TPtr8 inBuffer = inBuf->Des();
    aMessage.ReadL(0, inBuffer);

#ifdef _SENDEBUG
    if( inBuf )
        {
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- XML SD description (as XML) before parsing:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(inBuffer));
        CSLOG_L(iConnectionID,KMaxLogLevel ,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        }
#endif 

    CSenWSDescription* sd = CSenWSDescription::NewLC();
    sd->SetReader(*iManager.XMLReader());
    sd->ParseL(inBuffer);
    if (!CheckAllowUnRegisterL(error, aMessage, sd->FrameworkId()))
        {
        CleanupStack::PopAndDestroy(2); // sd, inBuf
        aMessage.Complete(error);
        return;
        }
#ifdef _SENDEBUG
        HBufC8* pAsXml = NULL;
        pAsXml = sd->AsXmlL();
        if(pAsXml)
            {
            CleanupStack::PushL(pAsXml);
            CSLOG_L(iConnectionID,KMaxLogLevel ,"  SD after parsing:");
            CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*pAsXml));
            CleanupStack::PopAndDestroy(); // 1
            }
        else
            {
            CSLOG_L(iConnectionID,KMaxLogLevel ,"  SD as XML was NULL after parsing(!)");
            }
#endif // _SENDEBUG


    TInt retVal(iManager.UnregisterServiceDescriptionL(*sd));
    CleanupStack::PopAndDestroy(2); // sd, inBuf
    aMessage.Complete(retVal);
    }

void CSenClientSession::ServiceDescriptionsByPatternL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::ServiceDescriptionsByPatternL")));
    
    CSenChunk* pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);

    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }
    
    TPtrC8 pattern;
    retVal = pSenChunk->DescFromChunk(pattern);
    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    RWSDescriptionArray arr; // the elements will NOT be owned..

    CSenWSDescription* pSD = CSenWSDescription::NewLC();
    pSD->SetReader(*iManager.XMLReader());
    pSD->ParseL(pattern);

    retVal = iManager.ServiceDescriptionsL(arr, *pSD);
    CleanupStack::PopAndDestroy(pSD); // pSD
    CleanupClosePushL(arr);

    if (retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // arr.Close() is enough
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(retVal);  
        }

    TInt count = arr.Count();
    
    // Calculate required heap allocation size:
    TInt size(0);
    if(count>0)
        {
        size += (count-1) * KTab().Length();
        }

    TInt leaveCode(KErrNone);
    TRAP(leaveCode, size += iManager.SizeOfServiceDescriptionsL(arr); )
    if(leaveCode!=KErrNone)
        {
        // the elements of array are not owned
        CleanupStack::PopAndDestroy(); // arr.Close() is enough
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(leaveCode); 
        }
        
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ServiceDescriptionsByPatternL");

    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Size of SD(s) + delimiter(s): %d bytes"), size));

    // Adjust RChunk to accomodate found SDs using known hnalde
    TPtr8* pDescriptions = NULL;
    retVal = pSenChunk->AllocDescToChunk(size, pDescriptions); 

    if(retVal==KErrNone && pDescriptions)
        {
        CBufFlat* pBuf = NULL;
        if(count>0)
            {
            pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            RBufWriteStream bufWs(*pBuf);
            CleanupClosePushL(bufWs);
            arr[0]->WriteAsXMLToL(bufWs);
            // Add this SD as XML into the "big heap buffer":
            TPtr8 p8 = pBuf->Ptr(0);
            pDescriptions->Append(p8); 
            // Destroy the temporary, flat stream buffer
            CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
            pBuf = NULL;
            }
        for(TInt x=1; x<count; x++)
            {
            // Add delimiter
            pDescriptions->Append(KTab); 

            pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            RBufWriteStream bufWs(*pBuf);
            CleanupClosePushL(bufWs);
            arr[x]->WriteAsXMLToL(bufWs);
            // Add this SD as XML into the "big heap buffer":
            TPtr8 p8 = pBuf->Ptr(0);
            pDescriptions->Append(p8); 
            CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
            pBuf = NULL;
            }
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- Requested descriptions:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*pDescriptions));
        }
#ifdef _SENDEBUG
    else
        {
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- AllocDescToRMsgL failed: %d"), retVal));
        }
#endif // _SENDEBUG

    CleanupStack::PopAndDestroy(); // arr.Close() is enough
    CleanupStack::PopAndDestroy(pSenChunk); // Close SenChunk

    aMessage.Complete(retVal);
    }

void CSenClientSession::ServiceDescriptionsByUriL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ServiceDescriptionsByUriL (contract)");
    
    CSenChunk* pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }
    
    TPtrC8 pattern;
    retVal = pSenChunk->DescFromChunk(pattern);
    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    RWSDescriptionArray arr;
    retVal = iManager.ServiceDescriptionsL(arr, pattern);
    CleanupClosePushL(arr);

    if (retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // arr.Close() is enough
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(retVal);
        return;
        }


    TInt count = arr.Count();
    
    // Calculate required heap allocation size:
    TInt size(0);
    if(count>0)
        {
        size += (count-1) * KTab().Length();
        }

    TInt leaveCode(KErrNone);
    TRAP(leaveCode, size += iManager.SizeOfServiceDescriptionsL(arr); )
    if(leaveCode!=KErrNone)
        {
        // the elements of array are not owned, reset is enough
        //arr.Reset();
        CleanupStack::PopAndDestroy(); // arr.Close() is enough
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(leaveCode); 
        }
        
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ServiceDescriptionsByPatternL");

    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Size of SD(s) + delimiter(s): %d bytes"), size));

    TPtr8* pDescriptions = NULL;
    retVal = pSenChunk->AllocDescToChunk(size, pDescriptions); 

    if(retVal==KErrNone && pDescriptions)
        {

        CBufFlat* pBuf = NULL;
        if(count>0)
            {
            pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            RBufWriteStream bufWs(*pBuf);
            CleanupClosePushL(bufWs);
            arr[0]->WriteAsXMLToL(bufWs);
            // Add this SD as XML into the "big heap buffer":
            TPtr8 p8 = pBuf->Ptr(0);
            pDescriptions->Append(p8); 
            // Destroy the temporary, flat stream buffer
            CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
            pBuf = NULL;
            }

        for(TInt x=1; x<count; x++)
            {
            // Add delimiter
            pDescriptions->Append(KTab); 

            pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            RBufWriteStream bufWs(*pBuf);
            CleanupClosePushL(bufWs);
            arr[x]->WriteAsXMLToL(bufWs);
            // Add this SD as XML into the "big heap buffer":
            TPtr8 p8 = pBuf->Ptr(0);
            pDescriptions->Append(p8); 
            CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
            pBuf = NULL;
            }
            
        CSLOG_L(iConnectionID,KMaxLogLevel ,"- Requested descriptions:");
        CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*pDescriptions));
        }
#ifdef _SENDEBUG
    else
        {
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- AllocDescToRMsgL failed: %d"), retVal));
        }
#endif // _SENDEBUG
    CleanupStack::PopAndDestroy(); // arr.Close() is enough
    CleanupStack::PopAndDestroy(pSenChunk); // Close SenChunk
    aMessage.Complete(retVal);
    }

void CSenClientSession::IsReadyL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::IsReadyL")));

    TBool isReady(EFalse);

    if (iStatus == KSenConnectionStatusReady)
        isReady = ETrue;

    TPtr8 ptr(reinterpret_cast<TUint8*>(&isReady), sizeof(isReady),
                sizeof(isReady));
    aMessage.WriteL(0, ptr); //IPC V2
    aMessage.Complete(KErrNone);
    }

// note: currently this function does not leave
void CSenClientSession::StartTransactionL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::StartTransactionL")));

    if (!CheckValidity(aMessage))
        {
        return;
        }
    TInt retVal(KErrNone);
    iServiceSession->StartTransaction();
    aMessage.Complete(retVal);
    }

// note: currently this function does not leave
void CSenClientSession::TransactionCompletedL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::TransactionCompletedL")));

    if (!CheckValidity(aMessage))
        {
        return;
        }
    TInt retVal(KErrNone);
    iServiceSession->TransactionCompleted();
    aMessage.Complete(retVal);
    }

void CSenClientSession::CancelSessionL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::CancelSessionL")));
    // Complete pending msgs via this call - or one could call non-leaving 
    // version and complete with (possible) leavecode it returns..
    CompletePendingMessagesL(); 
    
    // Check if this session is remote hostlet
    if ( iSessionType == ERemoteHostlet_HC && ipHostletAwaitOp)
        {
        ipHostletAwaitOp->RMessage().Complete(KErrSenCancelled);
        delete ipHostletAwaitOp;
        ipHostletAwaitOp = NULL; 
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CancelSessionL - await op completed with KErrSenCancelled");
        
        }
    
    iManager.SetShowPasswordDialog(EFalse); // not mandatory
    aMessage.Complete(KErrNone);
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CancelSessionL - operation complete.");
    }

void CSenClientSession::HasFacetL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::HasFacetL")));

    if (!CheckValidity(aMessage))
        {
        return;
        }

    TInt length = aMessage.Int2();
    if (length < 1)
        {
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(length);
    TPtr8 inBuffer = inBuf->Des();
    //inBuffer.Zero();

    TBool hasFacet;

    aMessage.ReadL(0, inBuffer); // URI

    TInt retVal = iServiceSession->HasFacetL(inBuffer, hasFacet);

    TPtr8 ptr(reinterpret_cast<TUint8*>(&hasFacet), sizeof(hasFacet),
        sizeof(hasFacet));
    aMessage.WriteL(1, ptr); //IPC V2
    CleanupStack::PopAndDestroy(); // inBuf
    aMessage.Complete(retVal);
    }

void CSenClientSession::CompleteServerMessagesOnOffL(const RMessage2& aMessage)
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::CompleteServerMessagesOnOffL")));

    if (!CheckValidity(aMessage))
        {
        return;
        }

    HBufC8* inBuf = HBufC8::NewLC(10);
    TPtr8 inBuffer = inBuf->Des();
    aMessage.ReadL(0, inBuffer); // boolean
    TBool onOrOff = (TBool)(*inBuffer.Ptr()) ;

    TInt retVal = iServiceSession->CompleteServerMessagesOnOffL(onOrOff);
    CleanupStack::PopAndDestroy(); // inBuf
    aMessage.Complete(retVal);
    }

void CSenClientSession::RequestServiceDescriptionL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::RequestServiceDescriptionL")));
    if (!CheckValidity(aMessage))
        {
        return;
        }

    iSendBuf = CBufFlat::NewL(KFlatBufSize);

    RBufWriteStream bufWs(*iSendBuf);
    CleanupClosePushL(bufWs); // push
    iServiceSession->AsServiceDescription().WriteAsXMLToL(bufWs);
    //iServiceSession->WriteAsXMLToL(bufWs); 


    // write the data length to client process descriptor..
    TInt neededLength = iSendBuf->Ptr(0).Length();
    TPtr8 ptr(reinterpret_cast<TUint8*>(&neededLength), sizeof(neededLength), sizeof(neededLength));
    TRAP( retVal, aMessage.WriteL(0, ptr); ) //IPC V2

    CleanupStack::PopAndDestroy(); // close bufWs
    aMessage.Complete(retVal);
    }

void CSenClientSession::ReceiveServiceDescriptionL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::ReceiveServiceDescriptionL")));
    if (!CheckValidity(aMessage))
        {
        return;
        }
    if(iSendBuf)
        {
        TRAP( retVal, aMessage.WriteL(0, iSendBuf->Ptr(0)); )  //IPC V2
        }
    else 
        {
        retVal = KErrSenInternal; 
        }

    aMessage.Complete(retVal);
    }
    
void CSenClientSession::SetTransportPropertiesL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::SetTransportPropertiesL")));

    if ( !CheckValidity(aMessage) )
        {
        return;
        }
    
    CSenChunk* pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);

    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }
    
    TPtrC8 transportProperties;
    retVal = pSenChunk->DescFromChunk(transportProperties);
    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Transport Properties (%d bytes):"), transportProperties.Length()));
    CSLOG_ALL(iConnectionID, KMaxLogLevel ,(transportProperties));
    if ( iServiceSession )
        {
#ifdef __ENABLE_ALR__
#if  defined ( RD_SEN_ENABLE_VTCP_BY_TRANSPORTPROPERTIES )
	HBufC8* pNewTransportProperties = NULL ;
	    if (iSessionType != ERemoteHostlet_HC)
            {
            
            //Only Real Consumer will monitor Mobility service
            if (!iALRObserver)
                {
	            CSLOG_L(iConnectionID, KMinLogLevel , "SetTransportPropertiesL - creating new ALR observer");
                iALRObserver = CALRObserver::NewL (*this, *iManager.XMLReader()) ;
                }
            MSenTransport& transport = TransportL() ;
            MSenProperties& property = transport.PropertiesL() ;
            //TPtrC8 transportCue = (*iInitializer).TransportCue();
            if (property.PropertiesClassType() == MSenProperties::ESenLayeredVtcpTransportProperties)
              	{
            	if (iALRObserver->GetActiveIap() >= 0 )//&& (transportCue == KSenTransportCueVirtualTCP)
            		{
            		//VTCP start a connection while setting transport itself so lets start ALR mobility observer
            		//collect the iap id and propagate it to VTCP
      		        CSLOG_L(iConnectionID, KMinLogLevel , "-SetTransportPropertiesL calling OpenConnectionL ");
            		TInt error = iALRObserver->OpenConnectionL(transportProperties, transport, *iInitializer, pNewTransportProperties) ;
        		if (error != KErrNone)
            			{
            			//ALR boot up failed , still iap can be collected in plugin 
            		    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SetTransportPropertiesL - OpenConnectionL returned, error: %d"), error));
            		  	}
            		if (pNewTransportProperties)
            			{
             		    CSLOG_L(iConnectionID, KMinLogLevel , "-SetTransportPropertiesL setting new transport properties from ALR observer");
            			transportProperties.Set(pNewTransportProperties->Des()) ;
                		CleanupStack::PushL(pNewTransportProperties);                        
            			}
            		}
              	}
            }
#endif //RD_SEN_ENABLE_VTCP_BY_TRANSPORTPROPERTIES
#endif //__ENABLE_ALR__
#ifdef __TEST_RETRY_TTL
			HBufC8* pNewTranProp = NULL;
			CSenVtcpTransportProperties* pVtcpProperties=CSenVtcpTransportProperties::NewL(transportProperties, *iManager.XMLReader());
			pVtcpProperties->SetMaxRetryTTLL(240);
			pVtcpProperties->SetMinRetryTTLL(30);
			pVtcpProperties->SetRetryDeltaTimeoutL(30);
			pNewTranProp=pVtcpProperties->AsUtf8LC();
			transportProperties.Set(pNewTranProp->Des()) ;
			delete pVtcpProperties;
			pVtcpProperties = NULL;	
			
#endif //__TEST_RETRY_TTL
	        retVal = iServiceSession->SetTransportPropertiesL(transportProperties, *this);
#ifdef __TEST_RETRY_TTL	        
	       CleanupStack::PopAndDestroy(pNewTranProp);
#endif //__TEST_RETRY_TTL	       
#ifdef __ENABLE_ALR__
#if  defined ( RD_SEN_ENABLE_VTCP_BY_TRANSPORTPROPERTIES )
                if (pNewTransportProperties != NULL)
                    {
            	        CleanupStack::PopAndDestroy(pNewTransportProperties);  
                    }
#endif //RD_SEN_ENABLE_VTCP_BY_TRANSPORTPROPERTIES	     	        
#endif //__ENABLE_ALR__
        }
    else
        {
        retVal = KErrSenNotInitialized;
        }

    CleanupStack::PopAndDestroy(pSenChunk);
    aMessage.Complete(retVal);
    }

void CSenClientSession::TransportPropertiesL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::TransportPropertiesL")));
    
    if ( !CheckValidity(aMessage) )
        {
        return;
        }

    CSenChunk* pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);

    if ( retVal != KErrNone )
        {
        CleanupStack::PopAndDestroy(pSenChunk);
        aMessage.Complete(KErrBadDescriptor);
        return;
        }

    HBufC8* pTransportProperties = NULL;
    iServiceSession->TransportPropertiesL(pTransportProperties, *this);
    
    retVal = pSenChunk->DescToChunk(*pTransportProperties);
    delete pTransportProperties;
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- DescToRChunkL returned: %d"), retVal));
    
    if ( retVal != KErrNone )
        {
        aMessage.Complete(ESenInternalError);
        CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - HandleBase");
        }
        
    CleanupStack::PopAndDestroy(pSenChunk);
    aMessage.Complete(retVal);
    }

void CSenClientSession::EstablishHostletConnectionL(const RMessage2& aMessage)
    {
    // To cache secure id of the client
    // This secure id will be used to notify client in case RProperty updates
    iClientSecureID = aMessage.SecureId();

    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::EstablishHostletConnectionL( %d )"), aMessage.Handle()));

    iSessionType = ERemoteHostlet_HC;
    TInt retVal(KErrNone);
    // Read hostlet connection properties from the rmessage


    // Read service description buffer length
    HBufC8* pServiceDescriptionBuf = NULL;
    retVal = ReadBufferFromRMessageL(aMessage, 1, 0, pServiceDescriptionBuf);
    CleanupStack::PushL( pServiceDescriptionBuf );

    if( retVal == KErrNone )
        {
        // Parse the XML string into a XML service description object
        TPtr8 sdAsXml = pServiceDescriptionBuf->Des();
        delete iInitializer;
        iInitializer = NULL;
        retVal = ParseSessionInitializerL(iInitializer, sdAsXml);

        // Attempt to initialize service session
        if ( retVal == KErrNone )
            {
            // Sanity check
            if( !iInitializer )
                {
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Fatal: initializer is NULL after succeeded parsing!");
                retVal = KErrSenInternal;
                }
            else
                {
                // Initialize the service session
                HBufC8* pErrorMsg = NULL;
                retVal = InitializeServiceSessionL(*iInitializer, pErrorMsg);
                CleanupStack::PushL( pErrorMsg );
                if( pErrorMsg )
                    {
                    TPtrC8 error = pErrorMsg->Des();
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- EstablishHostletConnectionL: retVal: (%d), error msg from InitializeServiceSessionL:"), retVal));
                    CSLOG_ALL(iConnectionID, KMinLogLevel ,( error ));
                    }
                CleanupStack::PopAndDestroy( pErrorMsg );
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Hostlet session INIT returned: %d"), retVal));


                if( retVal == KErrNone && iServiceSession )
                    {
                    if( iServiceSession->Hostlet() )
                        {
                        // Some other HC is already serving this endpoint,
                        // => return "endpoint reserved" code:
                        retVal = KErrSenEndpointReserved;
                        }
                    else
                        {
                        RFacetArray hostFacets;
                        iInitializer->FacetsL( hostFacets );
                        CleanupClosePushL( hostFacets );
                        TInt count(hostFacets.Count());
                        for (TInt i=0; i<count; i++)
                            {
                            ((CSenServiceSession*)iServiceSession)->SetFacetL(*hostFacets[i]);
                            }

                        hostFacets.ResetAndDestroy();
                        CleanupStack::Pop();
                        // Set this Remote Hostlet for the acquired session.
                        iServiceSession->SetHostletL(this);

                        // Remove this client session from the list of session consumers:
                        iServiceSession->RemoveConsumerL(*this);

                        // Create new hostlet connection transport plug-in instance:
                        TransportL(); 
                        
                        }

                    }
                }
            }
        }
        
    CleanupStack::PopAndDestroy( pServiceDescriptionBuf );
    // Complete with: KErrNone or KErrAlreadyExists, KErrSenNoEndpoint or KErrSenNoContract 
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- EstablishHostletConnectionL completes with: %d"), retVal));
    aMessage.Complete(retVal); 
    }

// Helper
TInt CSenClientSession::ReadBufferFromRMessageL(const RMessage2& aMessage, 
                                                TInt aSdLengthIdx, 
                                                TInt aSdBufIdx,
                                                HBufC8*& aSdBuf)
    {

    if(aSdBufIdx<0 || aSdBufIdx>3 || aSdLengthIdx<0 || aSdLengthIdx>3)
        {
        return KErrArgument;
        }

    TInt retVal(KErrNone);

    // Read service description buffer length
    TInt sdLength(KErrNotFound);

    switch(aSdLengthIdx)
        {
        case 0:
            {
            sdLength = aMessage.Int0(); // 1st argument
            break;
            }
        case 1:
            {
            sdLength = aMessage.Int1(); // 2nd argument
            break;
            }
        case 2:
            {
            sdLength = aMessage.Int2(); // 3rd argument
            break;
            }
        case 3:
            {
            sdLength = aMessage.Int3(); // 4th argument
            break;
            }
        default:
            {
            return KErrArgument;
            }
        }

    // Free and allocate buffer for service description    
    delete aSdBuf;
    aSdBuf = NULL;

    if(sdLength>0)
        {
        aSdBuf = HBufC8::NewL(sdLength);
        TPtr8 sdBuffer = aSdBuf->Des();
        // Read sd data into buf 
        switch(aSdBufIdx)
            {
            case 0:
                {
                aMessage.ReadL(0, sdBuffer);
                break;
                }
            case 1:
                {
                aMessage.ReadL(1, sdBuffer);
                break;
                }
            case 2:
                {
                aMessage.ReadL(2, sdBuffer);
                break;
                }
            case 3:
                {
                aMessage.ReadL(3, sdBuffer);
                break;
                }
            default:
                {
                return KErrArgument;
                }
            }
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8(" - ServiceDescription (%d bytes):"), sdBuffer.Length()));
        CSLOG_ALL(iConnectionID, KMaxLogLevel ,(sdBuffer));
        }
    else
        {
        CSLOG_L(iConnectionID,KNormalLogLevel ," - error: SD length is zero!");
        retVal = KErrArgument;
        }
    return retVal;
    }

void CSenClientSession::AwaitHostletRequestL(const RMessage2& aMessage)
    {
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::AwaitHostletRequestL( %d )"), aMessage.Handle()));

    // ALWAYS first check, if there already is some pending request, in which 
    // case aMessage can be completed immediately with request's chunk handle.
    RHostletRequestMap& requests = HostletRequestMap();
    TInt count(requests.Count());
    if( count > 0 )
        {
        CSenClientMessage* pMessage = (CSenClientMessage*)requests.ValueAt( 0 );
        const TInt* pKey = requests.KeyAt( 0 );
        if( pMessage && pKey )
            {
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d request(s) already waiting. Completing (serving) first in queue immediately."), count));
            delete ipHostletAwaitOp;
            ipHostletAwaitOp = NULL; // busy
            RHandleBase& handle = pMessage->Chunk();
#ifdef EKA2
            aMessage.Complete( handle );
#else // EKA1
            aMessage.Complete( handle.Handle() );
#endif // EKA2:EKA1
            // Queue is processed in simple, FIFO manner:
            requests.RemoveByKey( *pKey ); // de-allocates the txn ID
            }
        }
    else // start waiting for request..
        {
        // De-allocate last await op (wrapper)
        delete ipHostletAwaitOp;
        ipHostletAwaitOp = NULL;
        // Create wrapper for wait operation (RMessage2)
        ipHostletAwaitOp = CSenClientMessage::NewL( aMessage );
        CSLOG_L(iConnectionID,KMinLogLevel ,"- Waiting for Hostlet request from consumer(s).");
        }
    }




void CSenClientSession::ProvideHostletResponseL( const RMessage2& aMessage )
    {
    TInt responseCode = aMessage.Int3();
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::ProvideHostletResponseL( handle: %d, responseCode: %d )"), aMessage.Handle(), responseCode));
    TInt retVal(KErrNone);
    
    TInt txnId(KErrNotFound);
    CSenChunk* pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage, 2);
    if ( retVal == KErrNone )
        {
        txnId = pSenChunk->ChunkHeader().TransactionId();
        retVal = iServiceSession->ProvideHostletResponseL(*this, txnId, responseCode, KNullDesC8, *pSenChunk);
        }
    CleanupStack::PopAndDestroy( pSenChunk );
        
    if( retVal != KErrNone )  
        {
        // Some error occured, complete ASYNC "provide" -operation immediately
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::ProvideHostletResponseL - ERROR: Complete(%d)"), retVal));    
        aMessage.Complete(retVal);
        }
    else
        {
        // OK, append this ASYNC op to array of pending REMOTE HOSTLET transactions         
        // NOTE: TransactionL() method is quite similart to below code (re-factor later on)
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ProvideHostletResponseL - transport's CompleteTransaction() returned OK");
            
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::ProvideHostletResponseL - RMessage2(%d)"), aMessage.Handle()));

        CSenClientMessage* pMessage = CSenClientMessage::NewLC( aMessage );
        pMessage->SetLogger(Log());
        TInt retVal = pMessage->OpenChunkFromRMsgL(pMessage->RMessage(),2);
        
        if ( retVal == KErrNone )
            {
            TInt* pTxnId = new (ELeave) TInt(txnId);
            CleanupStack::PushL(pTxnId);

            retVal = TxnIds().Append(pTxnId);
            if ( retVal == KErrNone )
                {
                CleanupStack::Pop(pTxnId); // now owned by pointer array
            
                retVal = Transactions().Append(pMessage);
                if(retVal==KErrNone)
                    {
                    CleanupStack::Pop(pMessage); // now owned by transaction array of this Remote Hostlet
                    // Respond operation (ASYNC) is now safely pending
                    // Start waiting for MSenRemoteHostlet::OnServiceCompleteL OR cancel OR destruction 
                    // of this client session (remote hostlet)...
                    CSLOG_L(iConnectionID,KMinLogLevel ,"- ProvideResponse IPC operation now pending: OK.");
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(pMessage);
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(pTxnId);
                CleanupStack::PopAndDestroy(pMessage);
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(pMessage);
            }
            
        if ( retVal != KErrNone )
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Append failed (OOM): de-allocate client message:");
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Completeting this asynchronous IPC operation with ESenInternalError");
            aMessage.Complete(retVal);
            }
        }
    }
// Constantness on the TInt return type has no meaning since you are returning by value
TInt CSenClientSession::ConnectionId()
    {
    	return (iConnectionID);
    }

void CSenClientSession::MoveChunkL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        retVal = TransactionIndex(pSenChunk->ChunkHeader().TransactionId());
        if ( retVal != KErrNotFound )
            {
            RTransactionArray& transactions = Transactions();   
            CSenClientMessage* pClientMessage = transactions[retVal];
    
            TPtrC8 cid;
            retVal = pSenChunk->DescFromChunk(cid,2);
            if ( retVal == KErrNone )
                {
                RChunk chunk;
                CleanupClosePushL(chunk);
                retVal = chunk.Open(aMessage, 1, EOwnerProcess);
                if ( retVal == KErrNone )
                    {
                    CSenBinaryData* pBinaryData = CSenBinaryData::NewLC( chunk, cid, aMessage.Int2(), aMessage.Int3() );
            		RPointerArray<CSenBinaryData>& bArray = pClientMessage->BinaryDataArray();
            		bArray.AppendL(pBinaryData);
            		CleanupStack::Pop(pBinaryData);
                    }
                CleanupStack::Pop(&chunk);
                }
            }
        }
        
    CleanupStack::PopAndDestroy(pSenChunk);
    
    aMessage.Complete(retVal);
    }

void CSenClientSession::MoveFileL( const RMessage2& aMessage )
    {
    TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        retVal = TransactionIndex(pSenChunk->ChunkHeader().TransactionId());
        if ( retVal != KErrNotFound )
            {
            RTransactionArray& transactions = Transactions();   
            CSenClientMessage* pClientMessage = transactions[retVal];

            TPtrC8 cid;
            retVal = pSenChunk->DescFromChunk(cid,2);
            if (retVal == KErrNone)
                {
                RFile binaryDataFile;
                CleanupClosePushL(binaryDataFile);
                retVal = binaryDataFile.AdoptFromClient(aMessage, 1, 2);
                if ( retVal == KErrNone )
                    {
                    RFs fs;
            		// Get the Session handle
            		User::LeaveIfError(fs.Open(aMessage, 1));
            		CleanupClosePushL(fs);
                    
                    RFile file;
            		// Adopt the File handle;
            		User::LeaveIfError(file.Adopt(fs, binaryDataFile.SubSessionHandle()));
            		CleanupClosePushL(file);
            		
            		CSenBinaryData* pBinaryData = CSenBinaryData::NewLC(fs, file, cid);
            		RPointerArray<CSenBinaryData>& bArray = pClientMessage->BinaryDataArray();
            		bArray.AppendL(pBinaryData);
            		CleanupStack::Pop(pBinaryData);
            		
            		CleanupStack::Pop(&file);
            		CleanupStack::Pop(&fs);
                    }
          	    CleanupStack::PopAndDestroy(&binaryDataFile);
                }
            }
        }

    CleanupStack::PopAndDestroy(pSenChunk);
        
    aMessage.Complete(retVal);
    }
    
    
void CSenClientSession::SendFileHandleL( const RMessage2& aMessage )
    {
    TInt retVal(KErrNone);
    TInt txnId(KErrNotFound);
    CSenChunk* pSenChunk = NULL;
    pSenChunk = CSenChunk::NewLC(KNullDesC);
    
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);

    if ( retVal == KErrNone )
        {
        txnId = pSenChunk->ChunkHeader().TransactionId();
        if ( txnId != KErrNotFound )
            { 
            RFile fileHandle;
            CleanupClosePushL(fileHandle);
            retVal = fileHandle.AdoptFromClient(aMessage, 1, 2);
            if ( retVal == KErrNone )
                {
		         if (fileHandle.SubSessionHandle())
			         { 
	                
		                TInt fileSize(0);
		                fileHandle.Size(fileSize);
		                
		                if (fileSize > 0)
		                {
			               CSenClientMessage* pContext = iPendingTransactions[txnId];
			                if (pContext)
			                {
			                	pContext->RequestFileHandle(fileHandle);		                
			                }			                
						    
		                }
		                else
		                {
		                	retVal = KErrArgument;		                	
		                }
		                
	                }
	                else
	                {
	                	retVal=KErrArgument; // NULL file handle;
	         
	                }
               }
            CleanupStack::Pop(&fileHandle);
            }
        }

    CleanupStack::PopAndDestroy(pSenChunk);

    aMessage.Complete(retVal);
    }
TInt CSenClientSession::SendProgressToHostlet(const RMessage2& aMessage)
    {
    MSenRemoteHostlet* hostlet = iServiceSession->Hostlet();
    if (!hostlet)
        {
        return KErrNotSupported;
        }
    TTransferProgress progressData;
    TPckg<TTransferProgress> dataPtr = progressData;
    TInt err = aMessage.Read(0, dataPtr);
    if (err != KErrNone)
        {
        return err;
        }
    err = aMessage.GetDesLength(1);
    if (err < 0)
        {
        return err;
        }
    HBufC8* msg = HBufC8::New(err);
    if (!msg)
        {
        return KErrNoMemory;
        }
    TPtr8 msgPtr = msg->Des();
    err = aMessage.Read(1, msgPtr);
    if (err == KErrNone)
        {
        hostlet->TransferProgressForHostlet(progressData.iTxnId,
                progressData.iIncoming, progressData.iSoap, *msg,
                progressData.iProgress);
        }
    delete msg;
    return err;
    }
void CSenClientSession::AddCredentialL( const RMessage2& aMessage )
    {
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::AddCredentialL()")));
    TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        TPtrC8 patternAsXml;
        TPtrC8 idpAsXml;
        TPtrC8 credentialAsXml;
        
        retVal = pSenChunk->DescFromChunk(patternAsXml, 0);
        if ( retVal == KErrNone )
            {
            retVal = pSenChunk->DescFromChunk(idpAsXml, 1);
            if ( retVal == KErrNone )
                {
                retVal = pSenChunk->DescFromChunk(credentialAsXml, 2);
                if ( retVal == KErrNone )
                    {
                    CSenWSDescription* pSD = CSenWSDescription::NewLC();
                    pSD->SetReader(*iManager.XMLReader());
                    pSD->ParseL(patternAsXml);

                    CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KNullDesC8);
                    pIdP->SetReader(*iManager.XMLReader());
                    pIdP->ParseL(idpAsXml);
                    
                    CSenInternalCredential* pCredential = CSenInternalCredential::NewLC();
                    CSenParser* pParser = CSenParser::NewLC();
                	pParser->ParseL(credentialAsXml, *pCredential);
                	
                	CSenElement& element = pSD->AsElement();
                	RPointerArray<CSenElement>& elements = element.ElementsL();
                	if ( elements.Count() > 0)
                	    {
                	    CSenCredentialIdentifier& identifier = pCredential->IdentifierL();	//codescannerwarnings
                	    
                        TInt elementCount(elements.Count());
                        for (TInt i=0; i<elementCount; i++)
                            {
                            if ( ( elements[i]->LocalName() != KProviderPolicyLocalName ) &&
                                 ( elements[i]->LocalName() != KServicePolicyLocalName ) )
                                {
                                if ( elements[i]->LocalName() != KSenFacet )
                                    {
                                    identifier.SetPropertyL(elements[i]->LocalName(),
                                                            elements[i]->Content());
                                    }
                                }
                            }
                        RFacetArray facets;
                        CleanupClosePushL(facets);
                        pSD->FacetsL(facets);
                            {
                            TInt count(facets.Count());
                            for (TInt i=0; i<count; i++)
                                {
                                identifier.SetFacetL(*facets[i]);
                                }
                            }
                        facets.ResetAndDestroy();
                        CleanupStack::PopAndDestroy(&facets);
                	    }

                	CSenCredentialIdentifier& identifier = pCredential->IdentifierL();
                	TPtrC8 username;
                	identifier.PropertyL(KSenIdpAuthzIDLocalname,username);
                	TPtrC8 password;
                    identifier.PropertyL(KSenIdpPasswordLocalname,password);
                    TPtrC8 usernameIdp = pIdP->UserName();
                    TPtrC8 passwordIdp = pIdP->Password();
                    if (!username.Length())
                        {
                        if (usernameIdp.Length())
                            {
                            identifier.SetPropertyL(KSenIdpAuthzIDLocalname,usernameIdp);
                            }
                        if (!password.Length() && passwordIdp.Length())
                            {
                            identifier.SetPropertyL(KSenIdpPasswordLocalname, passwordIdp);
                            }
                        }
                    else
                        {
                        if (usernameIdp.Length() && username.Compare(usernameIdp) == 0)
                            {
                            if (!password.Length() && passwordIdp.Length())
                                {
                                identifier.SetPropertyL(KSenIdpPasswordLocalname, passwordIdp);
                                }
                            }
                        }
                	
                    if ( pSenChunk->ChunkHeader().DescriptorCount() == 4 )
                        {
                        TPtrC8 credentialPropertiesAsXml;
                        retVal = pSenChunk->DescFromChunk(credentialPropertiesAsXml, 3);
                        if ( retVal == KErrNone )
                            {
                            CSenCredentialProperties& properties = pCredential->PropertiesL();	//codescannerwarnings
                            pParser->ParseL(credentialPropertiesAsXml, properties);
                            }
                        }
                	CleanupStack::PopAndDestroy(pParser);
                	
                	// Following takes the ownership of pIdp and pCredential
                    iManager.AddCredentialL(pIdP, pCredential, retVal);
                    
                  	RWSDescriptionArray aMatches;
                    iManager.ServiceDescriptionsL(aMatches,*pSD);
               		CleanupClosePushL(aMatches);

                  	for(TInt i = 0; i < aMatches.Count(); i++)
                  		{
                      	if(((CSenWebServiceSession*)aMatches[i]) && pCredential)
                          {
	                      CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("SD Type is = %d"), aMatches[i]->DescriptionClassType()));						  
                          if( aMatches[i]->HasSuperClass( MSenServiceDescription::EServiceSession ) )
                          	{
                            CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::AddCredentialL() - Calling AddCredentialObserverL()")));
                            ((CSenWebServiceSession*)aMatches[i])->AddCredentialObserverL(*pCredential);
                            CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::AddCredentialL() - Completed AddCredentialObserverL()")));
                          	}
                          else
                          	{
                          	CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::AddCredentialL() - SD is not the session object !!! ")));
                          	}	
                          }
                  		}
                  		
                     CleanupStack::PopAndDestroy(&aMatches);
                    
                    CleanupStack::Pop(pCredential);
                    CleanupStack::Pop(pIdP);
                                        
                    CleanupStack::PopAndDestroy(pSD);
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(pSenChunk);
        
    aMessage.Complete(retVal);
    CSLOG(iConnectionID, KMinLogLevel ,(_L("CSenClientSession::AddCredentialL() Completed")));
    }

void CSenClientSession::CredentialsL( const RMessage2& aMessage )
    {
    _LIT8(KNoProperties, "-");
    
    TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        TPtrC8 patternAsXml;
        TPtrC8 idpAsXml;
        TPtrC8 credentialAsXml;
        retVal = pSenChunk->DescFromChunk(patternAsXml, 0);
        if ( retVal == KErrNone )
            {
            retVal = pSenChunk->DescFromChunk(idpAsXml, 1);
            if ( retVal == KErrNone )
                {
                CSenWSDescription* pSD = CSenWSDescription::NewLC();
                pSD->SetReader(*iManager.XMLReader());
                pSD->ParseL(patternAsXml);

                CSenIdentityProvider* pIdP = NULL;
                if ( idpAsXml != KNullDesC8 )
                    {
                    pIdP = CSenIdentityProvider::NewLC(KNullDesC8);
                    pIdP->SetReader(*iManager.XMLReader());
                    pIdP->ParseL(idpAsXml);
                    }
                
                RSenCredentialArray credentialsArray;
                CleanupClosePushL(credentialsArray);
                if ( pIdP )
                    {
                    retVal = iManager.CredentialsL(*pSD, *pIdP, credentialsArray);
                    }
                else
                    {
                    retVal = iManager.CredentialsL(*pSD, credentialsArray);
                    }
                    
                if ( retVal == KErrNone )
                    {
                    TInt count = credentialsArray.Count();
                    
                    // Calculate required heap allocation size:
                    TInt size(0);
                    if ( count > 0 )
                        {
                        size += (count-1) * KNewline().Length();
                        }
                    size += count * KTab().Length();

                    TInt leaveCode(KErrNone);
                    TRAP( leaveCode, size += iManager.SizeOfCredentialsL(credentialsArray); )
                    if ( leaveCode == KErrNone )
                        {
                        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Size of Credential(s) + delimiter(s): %d bytes"), size));

                        // Adjust RChunk to accomodate found Credentials using known handle
                        TPtr8* pCredentials = NULL;
                        retVal = pSenChunk->AllocDescToChunk(size, pCredentials); 

                        if(retVal==KErrNone && pCredentials)
                            {
                            CBufFlat* pBuf = NULL;
                            if(count>0)
                                {
                                pBuf = CBufFlat::NewL(KFlatBufSize);
                                CleanupStack::PushL(pBuf);
                                RBufWriteStream bufWs(*pBuf);
                                CleanupClosePushL(bufWs);
                                credentialsArray[0]->WriteAsXMLToL(bufWs);
                                bufWs.WriteL(KTab);
                                if ( credentialsArray[0]->HasProperties() )
                                    {
                                    credentialsArray[0]->PropertiesL().WriteAsXMLToL(bufWs);	//codescannerwarnings
                                    }
                                else
                                    {
                                    bufWs.WriteL(KNoProperties);
                                    }
                                
                                // Add this Credential as XML into the "big heap buffer":
                                TPtr8 p8 = pBuf->Ptr(0);
                                
                                TInt p8size = p8.Length();
                                
                                pCredentials->Append(p8); 
                                // Destroy the temporary, flat stream buffer
                                CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
                                pBuf = NULL;
                                }
                            for(TInt x=1; x<count; x++)
                                {
                                // Add delimiter
                                pCredentials->Append(KNewline); 

                                pBuf = CBufFlat::NewL(KFlatBufSize);
                                CleanupStack::PushL(pBuf);
                                RBufWriteStream bufWs(*pBuf);
                                CleanupClosePushL(bufWs);
                                credentialsArray[x]->WriteAsXMLToL(bufWs);
                                bufWs.WriteL(KTab);
                                if ( credentialsArray[x]->HasProperties() )
                                    {
                                    credentialsArray[x]->PropertiesL().WriteAsXMLToL(bufWs);	//codescannerwarnings
                                    }
                                else
                                    {
                                    bufWs.WriteL(KNoProperties);
                                    }
                                // Add this Credential as XML into the "big heap buffer":
                                TPtr8 p8 = pBuf->Ptr(0);
                                pCredentials->Append(p8); 
                                CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
                                pBuf = NULL;
                                }
                            CSLOG_L(iConnectionID,KMaxLogLevel ,"- Requested Credentials:");
                            CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*pCredentials));
                            }
#ifdef _SENDEBUG
                        else
                            {
                            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- AllocDescToRMsgL failed: %d"), retVal));
                            }
#endif // _SENDEBUG
                        }
                    }
                    
                CleanupStack::PopAndDestroy(&credentialsArray); // Close() is enough
                if ( idpAsXml != KNullDesC8 )
                    {
                    CleanupStack::PopAndDestroy(pIdP);
                    }
                CleanupStack::PopAndDestroy(pSD);
                }
            }
        }

    CleanupStack::PopAndDestroy(pSenChunk);
        
    aMessage.Complete(retVal);    
    }

void CSenClientSession::RemoveCredentialsL( const RMessage2& aMessage )
    {
    TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        TPtrC8 patternAsXml;
        TPtrC8 idpAsXml;
        TPtrC8 credentialAsXml;
        retVal = pSenChunk->DescFromChunk(patternAsXml, 0);
        if ( retVal == KErrNone )
            {
            retVal = pSenChunk->DescFromChunk(idpAsXml, 1);
            if ( retVal == KErrNone )
                {
                CSenWSDescription* pSD(NULL);
                if (patternAsXml.Length())
                    {
                    pSD = CSenWSDescription::NewLC();
                    pSD->SetReader(*iManager.XMLReader());
                    pSD->ParseL(patternAsXml);
                    }
                
                CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KNullDesC8);
                pIdP->SetReader(*iManager.XMLReader());
                pIdP->ParseL(idpAsXml);
                
                if(pIdP->FrameworkId() == KDefaultOviFrameworkID && !IsTrustedClient(aMessage))
                    {
                    retVal = KErrPermissionDenied;
                    }
                else
                    {
                    if (pSD)
                        {
                        retVal = iManager.RemoveCredentialsL(*pSD, *pIdP);
                        }
                    else
                        {
                        retVal = iManager.RemoveCredentialsL(pIdP->ProviderID());
                        }
                    }
                
                CleanupStack::PopAndDestroy(pIdP);
                if (pSD)
                    {
                    CleanupStack::PopAndDestroy(pSD);
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(pSenChunk);
        
    aMessage.Complete(retVal);    
    }

void CSenClientSession::ConnectionID(const RMessage2& aMessage)
    {
    if ( iConnectionID == KErrNotFound )
        {
        iConnectionID = iManager.NextConnectionID();
       // iSecureId = aMessage.SecureId();

        /*
        iServerContext.OpenApplicationContextL(iSecureId);
        CSenApplicationContext& appCtx = iServerContext.OpenApplicationContextL( aMessage.SecureId() );
        appCtx.OpenClientContextL( iConnectionID );
        */
        }
        iSecureId = aMessage.SecureId();
        iVendorId = aMessage.VendorId();
    aMessage.Complete(iConnectionID);
    }
    
void CSenClientSession::ConnectionIdentityProviderL(const RMessage2& aMessage)
    {
    TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        CSenWSDescription* pDesc = CSenWSDescription::NewLC();
        CBufFlat* pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs); // push
        iServiceSession->AsServiceDescription().WriteAsXMLToL(bufWs);
        
        pDesc->SetReader(*iManager.XMLReader());
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, pDesc->ParseL(pBuf->Ptr(0)));
        if ( leaveCode == KErrNone )
            {
            CleanupStack::PopAndDestroy(&bufWs);
            CleanupStack::PopAndDestroy(pBuf);
            MSenElement& xmlSdAsElement = pDesc->AsElement();
            MSenElement* pElement = xmlSdAsElement.Element(KProviderIdLocalName);
            if ( pElement )
                {
                TPtrC8 providerId = pElement->Content();
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pIdp = ((MSenCoreServiceManager&)iManager).IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                if ( pIdp )
                    {
                    HBufC8* pIdpAsXml = pIdp->AsXmlL();
                    CleanupStack::PushL(pIdpAsXml);
                    retVal = pSenChunk->DescToChunk(*pIdpAsXml);
                    CleanupStack::PopAndDestroy(pIdpAsXml);
                    }
                else
                    {
                    retVal = KErrNotFound;
                    }
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(&bufWs);
            CleanupStack::PopAndDestroy(pBuf);
            }
        CleanupStack::PopAndDestroy(pDesc);
        }

    CleanupStack::PopAndDestroy(pSenChunk);
    
    aMessage.Complete(retVal);
    }
/*    
CSenIdentityProvider* CSenClientSession::ConnectionIdentityProviderL()
    {
    CSenIdentityProvider* pIdp = NULL;
    
    if ( iServiceSession )
        {
        CSenWSDescription* pDesc = CSenWSDescription::NewLC();
        CBufFlat* pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs); // push
        iServiceSession->AsServiceDescription().WriteAsXMLToL(bufWs);
        
        pDesc->SetReader(*iManager.XMLReader());
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, pDesc->ParseL(pBuf->Ptr(0)));
        if ( leaveCode == KErrNone )
            {
            CleanupStack::PopAndDestroy(&bufWs);
            CleanupStack::PopAndDestroy(pBuf);
            MSenElement& xmlSdAsElement = pDesc->AsElement();
            MSenElement* pElement = xmlSdAsElement.Element(KProviderIdLocalName);
            if ( pElement )
                {
                TPtrC8 providerId = pElement->Content();
                CSenWSDescription* pPattern = CSenWSDescription::NewLC();
                pPattern->SetEndPointL(providerId);
                CSenIdentityProvider* pIdp = ((MSenCoreServiceManager&)iManager).IdentityProviderL(*pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(&bufWs);
            CleanupStack::PopAndDestroy(pBuf);
            }
        CleanupStack::PopAndDestroy(pDesc);
        }
    
    return pIdp;
    }
*/
void CSenClientSession::DataTrafficDetails(const RMessage2& aMessage) 
	{
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::DataTrafficDetails");
	TSenDataTrafficDetails trafficDetails; 
	TBool reset = aMessage.Int1();
 	if(reset == 1)
		{
		iDetails.iTotalBytesSent = 0;
		iDetails.iTotalBytesRecieved = 0; 
		}
	trafficDetails = iDetails;
	TPtr8 trafficDetailsDesc(reinterpret_cast<TUint8*>(&trafficDetails),sizeof(trafficDetails),
        					 sizeof(trafficDetails));
	TInt retVal(-1);     				
	TRAP(retVal,aMessage.WriteL(0,trafficDetailsDesc));
	CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Bytes send  : '%d')."), trafficDetails.iTotalBytesSent ));
	CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Bytes Received : '%d' ."), trafficDetails.iTotalBytesRecieved ));
    aMessage.Complete(retVal);
	}
void CSenClientSession::CancelRequestL(const RMessage2& aMessage)
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CancelRequestL");
    
    TInt transactionIDtoCancel = aMessage.Int0();

    RTransactionIdArray& txnIds = TxnIds();

    TInt pendingCount(txnIds.Count());
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d txnIds(s) pending."), pendingCount));
    for(TInt i=pendingCount-1; i>=0; i--)
        {
        if ( *txnIds[i] == transactionIDtoCancel )
            {
            TInt* pTxnId = txnIds[i];
            TInt cancelledTxnId(*pTxnId);
            txnIds.Remove(i);
            delete pTxnId;
            pTxnId = NULL;
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Now processing txn with id %d"), cancelledTxnId));


            // Attempt to cancel the actual activity on the transport layer.
            // Note that even if transport will not cancel this transaction,
            // the response from the transport (later on) won't anymore be 
            // delivered to consumer, since this RMessage2 is removed from 
            // iMessageMap few lines below. This means, that from the WS-stack
            // client point-of-view, cancel works and no "stray" responses
            // will follow (eventhough some data is received from transport 
            // stack).
            MSenTransport* pTransport = ipTransport;
            if(iServiceSession)
                {
                CSLOG_L(iConnectionID,KNormalLogLevel ,"-- has session.");
                MSenTransport* pSessionTransport = iServiceSession->Transport();
                if(pSessionTransport)
                    {
                    CSLOG_L(iConnectionID,KNormalLogLevel ,"--- session has transport.");
                    // If session owns a transport, always utilize that one
                    pTransport = pSessionTransport;
                    }
                }
            // If session does not own a transport, check if this remote consumer does..

            if(pTransport)
                {
                CSLOG_L(iConnectionID,KMinLogLevel ,"---- About to call CancelTransaction");
                TInt retCode = pTransport->CancelTransaction(cancelledTxnId);
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Transport's  CancelTransaction(%d) returned: %d"), cancelledTxnId, retCode));
                retCode = 0; // not used in release builds atm. 
                }
                
            break;
            }
        }
        
    RTransactionArray& txnMsgs = Transactions();
    pendingCount = txnMsgs.Count();
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d RMessage2(s) pending."), pendingCount));
    TInt transactionID;
    TInt pendingTxnId;
    for(TInt t=pendingCount-1; t>=0; t--)
        {
        if(txnMsgs[t])
            {
            CSenClientMessage* pClientMessage = Transactions()[t];
            pendingTxnId = pClientMessage->ChunkHeader().PendingTransactionId();
            transactionID = pClientMessage->ChunkHeader().TransactionId();

            if ( ( pendingTxnId && ( pendingTxnId == transactionIDtoCancel ) ) ||
                 ( !pendingTxnId && ( transactionID == transactionIDtoCancel ) ) )
                {
                RMessage2& message = pClientMessage->RMessage();
                
                pendingTxnId = pClientMessage->ChunkHeader().PendingTransactionId();
                if (pendingTxnId)
                    {
                    transactionID = pendingTxnId;
                    pClientMessage->ChunkHeader().SetPendingTransactionId(0);
                    pClientMessage->ChunkHeader().SetTransactionId(pendingTxnId);
                    }
                else
                    {
                    transactionID = pClientMessage->ChunkHeader().TransactionId();
                    }
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Completing Txn ID: %d"),transactionID)); 
                
                TPtr8 ptr(reinterpret_cast<TUint8*>(&transactionID), 
                          sizeof(transactionID), 
                          sizeof(transactionID)); 

                TInt leaveCode(KErrNone);
                TRAP(leaveCode, message.WriteL(1, ptr)); //IPC V2
                if(leaveCode!=KErrNone)
                    {
                    message.Complete(ESenInternalError);
                    CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - TxnId");
                    }
                else
                    {
                    message.Complete(ESenServRequestCancelled);
                    CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenServRequestCancelled - TxnId");
                    }

                delete pClientMessage;
                txnMsgs.Remove(t);
                
                break;
                }
            }
        }
        
    aMessage.Complete(KErrNone);
    }

void CSenClientSession::PanicClient( const RMessagePtr2& aMessage, TInt aPanic ) const
    {
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::PanicClient( %d )"), aPanic));
    // Note: this panics the client thread, not server
    aMessage.Panic(KSenServiceManagerPanic, aPanic) ; //IPC V2 
    }

RFileLogger* CSenClientSession::Log() const
    {
    return iManager.Log();
    }

TInt CSenClientSession::SetSessionL(MSenRemoteServiceSession& aServiceSession)
    {
    iServiceSession = &aServiceSession; // not owned
    SetStatusL(iServiceSession->StatusL());
    return KErrNone;
    }


TInt CSenClientSession::HandleMessageL( HBufC8* apMessage,
                                        const TInt aTxnId,
                                        MSenProperties* aResponseTransportProperties )
    {
    iReauthResendNeeded = EFalse ; //resetting the flag if already true
    if ( !apMessage )
        {
        apMessage = KNullDesC8().AllocL();
        }
    CleanupStack::PushL(apMessage);        
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::HandleMessageL");
    
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Txn ID: %d, message length: %d bytes"),  aTxnId, apMessage->Length() ));
    CSLOG_ALL(iConnectionID, KMaxLogLevel ,( *apMessage ));

    if( iAllowShowDialogCount>0 )
        {
        iAllowShowDialogCount--;
        }

    if( iAllowShowDialogCount==0 )
        {
        iManager.SetShowPasswordDialog(EFalse); // turn the flag down
        }

    TInt retCode(KErrNone);
    TInt index = TransactionIndex(aTxnId);
    if(index != KErrNotFound)
        {
        RTransactionArray& transactions = Transactions();   
        CSenClientMessage* pClientMessage = transactions[index];
        RMessage2& message = pClientMessage->RMessage();
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::HandleMessageL - RMessage2(%d)"),  message.Handle() ));

        // Remove the txnId from the list of pending ones:
        TInt transactionIdCount(TxnIds().Count());
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Number of pending TxnIds: %d"), transactionIdCount));
        
        for(TInt tx=0; tx<transactionIdCount; tx++)
            {
            TInt* pTxnId = TxnIds()[tx];
            if(*pTxnId == aTxnId)
                {
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Pending txnID(%d) found from index %d. Removing & deleting it."), *pTxnId,tx ));
                TxnIds().Remove(tx);
                delete pTxnId;
                break;
                }
            }

        TInt transactionID(aTxnId);
        TInt pendingTxnId = pClientMessage->ChunkHeader().PendingTransactionId();
        if(pendingTxnId)
            {
            transactionID = pendingTxnId;
            pClientMessage->ChunkHeader().SetPendingTransactionId(0);
            pClientMessage->ChunkHeader().SetTransactionId(pendingTxnId);
            }
        
        TPtr8 ptr(reinterpret_cast<TUint8*>(&transactionID), 
                  sizeof(transactionID), 
                  sizeof(transactionID)); 

        TInt leaveCode(KErrNone);
        TRAP(leaveCode, message.WriteL(1, ptr)); //IPC V2
        if(leaveCode!=KErrNone)
            {
            message.Complete(ESenInternalError);
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - TxnId");
            retCode = leaveCode;
            }
        else
            {
            // OK!
            if( pClientMessage->ChunkHeader().MessageDirection() == MSenMessage::EOutbound )
                {
                // SetResponseL and SetRequestL to CSenChunk (or it's subclass)
                // Methods should always check that TP is set to correct when MSG
                // direction changes; for e.g. when copying response, the TP type
                // MUST be set to ENotInUse, or SC will fail(!). Furthermore, the
                // SetResponseL method could ignore message data (and not copy it),
                // if direction is already inbound, etc. And, methods should check
                // the PTR so that copy-to-self is ignored/skipped/avoided. Finally,
                // the methods SHOULD support DOUBLE ENDED chunks, so that descs 
                // could be nicely re-allocated separately (MSG first, TP by some
                // other component etc...)
                pClientMessage->ChunkHeader().SetMessageDirection( MSenMessage::EInbound ); 
                HBufC8* pSerializedProperties = NULL;
                if( aResponseTransportProperties )
                    {
                    pClientMessage->ChunkHeader().SetPropertiesType(aResponseTransportProperties->PropertiesClassType());
                    pSerializedProperties = aResponseTransportProperties->AsUtf8LC();
                    retCode = pClientMessage->DescsToChunk(*apMessage, *pSerializedProperties);
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- DescsToChunk return code: %d"), retCode));
                    CleanupStack::PopAndDestroy(pSerializedProperties);
                    }
                else
                    {
                    pClientMessage->ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
                    retCode = pClientMessage->DescToChunk(*apMessage);
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- DescToChunk return code: %d"), retCode));
                    }
                }
            else
                {
                CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Reply message (%d) direction is already MSenMessage::EInbound, and data is already copied to chunk."), aTxnId));
                apMessage = NULL; 
                }

            if ( retCode == KErrNone && leaveCode != KErrNone )
                {
                retCode = leaveCode;
                }
            if( retCode != KErrNone)
                {
                message.Complete(ESenInternalError);
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - HandleBase");
                retCode = leaveCode;
                }
            else // OK!
                {   
                message.Complete(ESenServRequestResponsed);
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenServRequestResponsed");
                }
            }
        transactions.Remove(index);
        delete pClientMessage;
        }
    else // pending RMessage2 was not found
        {   
        CSLOG_L(iConnectionID,KMinLogLevel ,"- No pending RMessage2 found - cancelled?");
        retCode = KErrNotFound; 
        }
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::HandleMessageL returns: %d"), retCode));

    if( apMessage )
        {
        CleanupStack::PopAndDestroy(apMessage);        
        }
    else
        {
        CleanupStack::Pop(); // apMessage
        }

    return retCode;
    }

TInt CSenClientSession::HandleErrorL( HBufC8* apError,
                                      const TInt aErrorCode,
                                      const TInt aTxnId,
                                      MSenProperties* aResponseTransportProperties )
    {
    if ( !apError )
        {
        apError = KNullDesC8().AllocL();
        }
    CleanupStack::PushL(apError);        
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::HandleErrorL");
    
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Txn ID: %d, error (%d) length: %d bytes"),  aTxnId, aErrorCode, apError->Length() ));

    if(iAllowShowDialogCount>0)
        {
        iAllowShowDialogCount--;
        }

    if(iAllowShowDialogCount==0)
        {
        iManager.SetShowPasswordDialog(EFalse); // turn the flag down
        }

    TInt retCode(KErrNone);
    TInt index = TransactionIndex(aTxnId);
    if(index != KErrNotFound)
        {
        RTransactionArray& transactions = Transactions();   
        CSenClientMessage* pClientMessage = transactions[index];
        RMessage2& message = pClientMessage->RMessage();
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::HandleErrorL - RMessage2(%d)"),  message.Handle() ));

        // Remove the txnId from the list of pending ones:
        TInt transactionIdCount(TxnIds().Count());
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Number of pending TxnIds: %d"), transactionIdCount));
        for(TInt tx=0; tx<transactionIdCount; tx++)
            {
            TInt* pTxnId = TxnIds()[tx];
            if(*pTxnId == aTxnId)
                {
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Pending txnID(%d) found from index %d. Removing & deleting it."), *pTxnId, tx));
                TxnIds().Remove(tx);
                delete pTxnId;
                break;
                }
            }

        TInt errorCode(aErrorCode);
        TPtr8 ptrErr(reinterpret_cast<TUint8*>(&errorCode), 
                     sizeof(errorCode), 
                     sizeof(errorCode)); 

        TInt leaveCode(KErrNone);
        TRAP(leaveCode, message.WriteL(0, ptrErr)); //IPC V2
        if(leaveCode!=KErrNone)
            {
            message.Complete(ESenInternalError);
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError");
            retCode = leaveCode;
            }
        else // no internal error in writing error code
            {
            // Write back the transaction ID, so that client knows which
            // request was responded (use slot 1, slot 0 is reserved for 
            // errorcodes of handle error atm..)
            TInt transactionID(aTxnId);
            TInt pendingTxnId = pClientMessage->ChunkHeader().PendingTransactionId();
            if (pendingTxnId)
                {
                transactionID = pendingTxnId;
                pClientMessage->ChunkHeader().SetPendingTransactionId(0);
                pClientMessage->ChunkHeader().SetTransactionId(pendingTxnId);
                }
                
            TPtr8 ptr(reinterpret_cast<TUint8*>(&transactionID), 
                      sizeof(transactionID), 
                      sizeof(transactionID)); 

            TRAP(leaveCode, message.WriteL(1, ptr)); //IPC V2
            if(leaveCode!=KErrNone)
                {
                message.Complete(ESenInternalError);
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError");
                retCode = leaveCode;
                }
            else // no internal error in writing error message
                {
                switch(aErrorCode)
                    {
                    case KErrSenReinitRequired: // ESenReAuthAndResendNeeded
                        {
                        pClientMessage->ChunkHeader().SetMessageDirection( MSenMessage::EOutbound );
                        iReauthResendNeeded = ETrue ;
                        message.Complete(ESenReAuthAndResendNeeded);
                        CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenReAuthAndResendNeeded");
                        }
                    break;
                    case KErrSenResendRequired: // ESenResendNeeded
                        {
                        pClientMessage->ChunkHeader().SetMessageDirection( MSenMessage::EOutbound );
                        iReauthResendNeeded = EFalse ;
                        message.Complete(ESenResendNeeded);
                        CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenResendNeeded");
                        }
                    break;
                    default: // almost always: ESenServRequestResponsed 
                        {
                        iReauthResendNeeded = EFalse ;
                        // Some other error, which can freely be propagated back to service consumer
                        
                        if( pClientMessage->ChunkHeader().MessageDirection() == MSenMessage::EOutbound )
                            {
                            // SetResponseL and SetRequestL to CSenChunk (or it's subclass)
                            // Methods should always check that TP is set to correct when MSG
                            // direction changes; for e.g. when copying response, the TP type
                            // MUST be set to ENotInUse, or SC will fail(!). Furthermore, the
                            // SetResponseL method could ignore message data (and not copy it),
                            // if direction is already inbound, etc. And, methods should check
                            // the PTR so that copy-to-self is ignored/skipped/avoided. Finally,
                            // the methods SHOULD support DOUBLE ENDED chunks, so that descs 
                            // could be nicely re-allocated separately (MSG first, TP by some
                            // other component etc...)
                            pClientMessage->ChunkHeader().SetMessageDirection( MSenMessage::EInbound ); 
                            HBufC8* pSerializedProperties = NULL;
                            if( aResponseTransportProperties )
                                {
                                pClientMessage->ChunkHeader().SetPropertiesType(aResponseTransportProperties->PropertiesClassType());
                                pSerializedProperties = aResponseTransportProperties->AsUtf8LC();
                                retCode = pClientMessage->DescsToChunk(*apError, *pSerializedProperties);
                                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- DescsToChunk returned: %d"), retCode));
                                CleanupStack::PopAndDestroy(pSerializedProperties);
                                }
                            else
                                {
                                pClientMessage->ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
                                retCode = pClientMessage->DescToChunk(*apError);
                                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- DescToChunk returned: %d"), retCode));
                                }
                            }
                        else
                            {
                            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Error message (%d) direction is already MSenMessage::EInbound, and data is already copied to chunk."), aTxnId));
                            apError = NULL; // will NOT leak, since the pointer was pointing to global chunk!
                            }
                            
                        if ( retCode == KErrNone && leaveCode != KErrNone )
                            {
                            retCode = leaveCode;
                            }
    
                        if( retCode!=KErrNone )
                            {
                            message.Complete(ESenInternalError);
                            CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - HandleBase");
                            retCode = leaveCode;
                            }
                        else // OK!
                            {   
                            message.Complete(ESenServRequestResponsed);
                            CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenServRequestResponsed");
                            }
                        }
                    } // switch
                } // no internal error in writing error message
            } // no internal error in writing error code
            
        transactions.Remove(index);
        delete pClientMessage;
        }
    else // pending RMessage2 was not found
        {   
        CSLOG_L(iConnectionID,KMinLogLevel ,"- No pending RMessage2 found - cancelled?");
        retCode = KErrNotFound; 
        }
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("CSenClientSession::HandleErrorL returns: %d"), retCode));

    
    if( apError )
        {
        CleanupStack::PopAndDestroy(apError);    
        }
    else
        {
        CleanupStack::Pop(); // apError
        }
    
    return retCode;
    }

void CSenClientSession::SetStatusL(const TInt aStatus)
    {
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::SetStatusL(%d)"), aStatus));
    iStatus = aStatus;

    // Currently: publish ONLY "credentials expired" status.
    // One MUST NOT publish "credentials ready" status,
    // since publish&subscibe has higher priority
    // than "Initialize" IPC op that is fied by SC constructor
    // and this causes pre-mature SendL (internal boolean of SC
    // impl about "readyness" is still false (=> this could be
    // fixed, but it is better to wait for Async IPC "Initialize"
    // to complete.
    if( aStatus != KSenConnectionStatusReady )
        {
        // It is ok to "push" (==publish) "credentials expired" status
        RProperty::Set(iClientSecureID,(KSenRPropertyKeyBaseNumber+iConnectionID), aStatus);
        }
    }

void CSenClientSession::FileProgress( TInt aTxnId, TBool aIsIncoming, TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress ) 
    {
    CSenClientMessage* pTransaction = NULL;
    
    TInt index = TransactionIndex(aTxnId);
    if( index > KErrNotFound )
        {
        RTransactionArray& transactions = Transactions();
        if( index < transactions.Count() )
            {
            pTransaction = transactions[index];
            }
        }
    
    if( pTransaction ) ////// NOT needed  ( note about this code & TransferProgressForHostlet(): -> hostlet connection's client sessions does not have transaction in here..
        {
        if( !aIsIncoming && pTransaction->ChunkHeader().MessageDirection() != MSenMessage::EOutbound )
            {
            // the transaction has ALREADY been completed, so there is no need to inform about "file progress"
            return;
            }
        } // end of 'if( pTransaction )'
#ifdef _SENDEBUG        
        else
            {
        	CSLOG_FORMAT(( iConnectionID, KMinLogLevel, _L8("CSenClientSession::FileProgress() - ChunkByTxnIdL(%d) returned NULL"), aTxnId ));
            }
#endif // _SENDEBUG

    TRAP_IGNORE ( DeliverProgressToClient( aTxnId, aIsIncoming, aIsSoap, aSoapOrCid, aProgress ); ) // should NOT leave!
    }
void CSenClientSession::DeliverProgressToClient( TInt aTxnId, TBool aIsIncoming, TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress ) 
    {
    // NOTE: for OUTGOING messages with BLOBs (file attachements etc), aIncoming is EFalse, and aSoapOrCid is ALWAYS A CID(!)
	CSLOG_L( iConnectionID,KMinLogLevel,"" );
	CSLOG_FORMAT(( iConnectionID, KMinLogLevel, _L8("CSenClientSession::DeliverProgressToClient(txn: %d, progress: %d, incoming: %d, soap: %d) - begin"), aTxnId, aProgress, aIsIncoming, aIsSoap ));
	CSLOG_L( iConnectionID,KMinLogLevel, "- SOAP/CID:" );
    CSLOG_ALL( iConnectionID, KMinLogLevel, (aSoapOrCid ));    
	
#ifndef RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS

    // THE "LEGACY CODE": START =>
	CSLOG_L( iConnectionID, KMinLogLevel,"CSenClientSession::DeliverProgressToClient() - RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == FALSE" );
    if (iConnectionSession.Handle())
        {
        iConnectionSession.SendFileProgress( aTxnId, aIsIncoming, aIsSoap, aSoapOrCid, aProgress );
        }
        return;
    // <= THE "LEGACY CODE": END
    
#else // == RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE 

	CSLOG_L( iConnectionID, KMinLogLevel, "CSenClientSession::DeliverProgressToClient() - RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE" );
	
	// CHECK IF CODE WAS COMPILED WITH "MIXED" MODE (conn agent for SOAP progress, pub&sub for outgoing BLOB progress):
	 
  #ifdef RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS
    // Use connection agent for any SOAP progress signalizing:
	CSLOG_L( iConnectionID, KMinLogLevel, "CSenClientSession::DeliverProgressToClient() - RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == TRUE" );
	if( aIsSoap )
	    {
    	CSLOG_L( iConnectionID, KMinLogLevel,"- aIsSoap == TRUE => try to use connection agent for SOAP progress.." );
        if ( iConnectionSession.Handle() )
            {
        	CSLOG_L( iConnectionID, KMinLogLevel,"- iConnectionSession.Handle() is valid [OK]" );
            iConnectionSession.SendFileProgress( aTxnId, aIsIncoming, aIsSoap, aSoapOrCid, aProgress );
            return;
            }
  #ifdef _SENDEBUG            
        else
            {
        	CSLOG_L( iConnectionID, KMinLogLevel,"DeliverProgressToClient(): - iConnectionSession.Handle() is _invalid_ [NOT OK]: cannot deliver SOAP progress" );
            }
  #endif // _SENDEBUG            
	    } // end of: if ( aIsSoap )

  #ifdef _SENDEBUG            
    else
        {
        // the progress is not about SOAP
    	CSLOG_L( iConnectionID, KMinLogLevel,"DeliverProgressToClient(): - aIsSoap == FALSE => Use pub&sub for BLOB related progress signalizing." );
        }
  #endif // _SENDEBUG            
        
  #endif // RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS END
	
    // publish the new value from Serene Core server to the client (SC impl) 
    
    TInt cid(0);
    TInt cid2( KErrNotFound );
    
    TBool hasCidPostfix( EFalse );
    TInt lexingError(KErrNone);
    if ( !aIsSoap ) // NOTE: WHEN RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == TRUE, aIsSoap is ALWAYS FALSE when execution gets to this point(!)
        {
        // Current implementation "trusts" that CID is a numeric value!
        TPtrC8 numericCid( aSoapOrCid );
        
        // Look for "@example.org" and rip if off, if found
        TInt length = aSoapOrCid.Length();
        if( length )
            {
            TInt index = aSoapOrCid.Find( KSenCidPostfix );
            if( index != KErrNotFound )
                {
                hasCidPostfix = ETrue;
                numericCid.Set( aSoapOrCid.Left( index ) ); // take all the data prior the @ -char..
                CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("CSenClientSession::DeliverProgressToClient() - numeric cid: '%S', hasCidPostfix = ETrue"), &numericCid ));
                }
                
            }
        TPtrC8 numericCid2(KNullDesC8);
        if ( numericCid.Length() > KSenMaxCidLength )
            {
            numericCid2.Set(numericCid.Right(KSenMaxCidLength));
            numericCid.Set(numericCid.Left(KSenMaxCidLength-1));
            }
        TLex8 lex( numericCid );
        lexingError = lex.Val( cid );
        if( !lexingError && numericCid2.Length() )
            {
            TLex8 lex2( numericCid2 );
            lexingError = lex2.Val( cid2 );
            }
            
        if( !lexingError )
            {
            CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8(" - cid: '%S', cid2: '%S'"), &numericCid, &numericCid2 ));
            TFileOutgoingTransferProgress progress( aTxnId, aIsIncoming, aProgress, hasCidPostfix, cid, cid2 ); // note(!): ELeave is NOT used here, instead KErrNoMemory is returned if this method fails!
            TPckgBuf<TFileOutgoingTransferProgressBase> buffer( progress );
            RProperty::Set( KSenInterfaceUidFilesObserver, iConnectionID, buffer ); // this is serializing the property, so it is ok to let the T-classes to go out of scope
            }
        else
            {
            CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("CSenClientSession::DeliverProgressToClient() - TLex:Val('cid') returned error: %d, cid: '%S', cid2: '%S'"), lexingError, &numericCid, &numericCid2 ));
#ifdef RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS
        	CSLOG_L( iConnectionID, KMinLogLevel, "=> Going to UTILIZE Connection Agent for non-numeric CIDs!" );
            if (iConnectionSession.Handle())
                {
                iConnectionSession.SendFileProgress( aTxnId, aIsIncoming, aIsSoap, aSoapOrCid, aProgress );
                }
    
#endif // RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS
            }
        } // end of if ( !aIsSoap )
#ifdef _SENDEBUG        
    else
        {
        // this callback is carrying a SOAP (should not, that happens only with "incoming" file progress callbacks, actually, not when sending a msg OUT
        CSLOG_L( iConnectionID, KMinLogLevel, "CSenClientSession::DeliverProgressToClient() - SOAP of this 'progress' discarded (no ConnAgent session available)" );
        }
#endif // _SENDEBUG


#endif // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS  

    // The end of the method
	//CSLOG_L( iConnectionID,KMinLogLevel,"CSenClientSession::DeliverProgressToClient() - end" );
    }

TBool CSenClientSession::OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId)
    {
    if ( iConnectionSession.Handle() )
        {
        return iConnectionSession.OnGetBrandIdL( aBrandIdListAsXmlSnippet, aSelectedBrandId );
        }
    else
        {
        return EFalse;
        }
		}       
   
void CSenClientSession::TransferProgressForHostlet( TInt aTxnId, TBool aIsIncoming, TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress)
    {
#ifndef RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC    
    TRAP_IGNORE ( DeliverProgressToClient( aTxnId, aIsIncoming, aIsSoap, aSoapOrCid, aProgress ); ) // should NOT leave!
#else // RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC == TRUE
    CSLOG_L( iConnectionID, KMinLogLevel, "CSenClientSession::TransferProgressForHostlet: RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC == ETrue" );
#endif // end of RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC    
    }
    
void CSenClientSession::SetDataTrafficDetails( TSenDataTrafficDetails& aDetails ) 	
	{
	CSLOG_L( iConnectionID, KSenCoreServiceManagerLogLevel, "CSenClientSession::SetDataTrafficDetails");
	iDetails += aDetails;
	}

/**
* SessionValidity method checks the validity of this session
* @return KErrNone if session is ready and its credentials (if any) are valid
*         KErrSenNotInitialized if session is not yet set (iServiceSession is NULL)
*         KErrConnectionInitializing if session's state  is new 
*                                    (iStatus is KSenConnectionStatusNew)
*         KSenConnectionStatusExpired if session's state is initializing 
*                                    (iStatus is KSenConnectionStatusNew)
*         KSenConnectionStatusExpired if session's state is expired 
*                                    (iStatus is KSenConnectionStatusExpired)
*/
TInt CSenClientSession::SessionValidity()
    {
    TInt errorCode(KErrNone);

    if( !iServiceSession )
        {
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SessionValidity:");
        CSLOG_L(iConnectionID,KMinLogLevel ," - KErrSenNotInitialized");
        errorCode = KErrSenNotInitialized;
        }
    else if (iStatus == KSenConnectionStatusNew )
        {
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SessionValidity:");
        CSLOG_L(iConnectionID,KMinLogLevel ," - KErrConnectionInitializing");
        errorCode = KErrConnectionInitializing;
        }
    else if(iStatus == KSenConnectionStatusExpired)
        {
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SessionValidity:");
        CSLOG_L(iConnectionID,KMinLogLevel ," - KErrConnectionExpired");
        errorCode = KErrConnectionExpired;
        }
    else 
        {
        TBool sessionReady(EFalse);
        TInt leaveCode(KErrNone);
        TRAP( leaveCode, sessionReady = iServiceSession->IsReadyL(); )

        if(leaveCode!=KErrNone)
            {
            CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SessionValidity - FATAL:");
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8(" - IsReadyL() leaved: %d"), leaveCode));
            // Specific error *could* be defined and passed to actual WSC, 
            // but what it could possibly do with this info(?). Therefore,
            // currently this method will just signal that "session has
            // expired" by completing with KErrConnectionExpired.
            
            // Invalidate the session, since validity could not be determined
            sessionReady = EFalse;
            }

        if(!sessionReady)
            {
            // This session is not ready - its credentials are now invalid:
            CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::SessionValidity session is not ready");
            CSLOG_L(iConnectionID,KMinLogLevel ," - KErrConnectionExpired");
            errorCode = KErrConnectionExpired;
            }
        }
    return errorCode; // KErrNone if session (iServiceSession) is valid 
    }

// If this function returns EFalse, the caller should return
// because message has already been completed
TBool CSenClientSession::CheckValidity(const RMessage2& aMessage)
    {
    TInt errorCode(SessionValidity());
    if(errorCode!=KErrNone)
        {
        // Signal the client by competing this RMessage2:
        aMessage.Complete(errorCode);
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CheckValidity returns EFalse");
        // Caller should return, since message was already completed
        return EFalse;  
        }
    else
        {
        // Session is ready and credentials (if any) are valid.
		CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CheckValidity returns ETrue");        
        return ETrue;
        }
    }


MSenTransport& CSenClientSession::TransportL()
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::TransportL()");

    // Check if session exists:
    if(iServiceSession)
        {
        // Check if consumed session already owns a transport:
        MSenTransport* pSessionTransport = iServiceSession->Transport();
        if(pSessionTransport)
            {
            // Return the transport that is owned by the consumed session!
            return *pSessionTransport;
            }
        }
    // else, if NO session: utilize the transport owned by this consumer
    //       OR: create new transport for the session



    // Core is capable of working as transport factory, too.
    // The returned transport instancies may or may not be
    // ECOM plugins:
    MSenTransportFactory& transportFactory = iManager;
    CSenTransportBase* pTransport = NULL;
    
    // Check if service session has been initilized
    if(iServiceSession) // && iServiceSession.Transport() == NULL
        {
        // Session does not yet own any transport
        // Check if this consumer owns some transport:
        if(ipTransport)
            {
            // Transfer the ownership of current transport to the session
            // (internal service consumers have initiated this transport)
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Transport's ownership transferred from consumer to session.");
            pTransport = ipTransport;
            ipTransport = NULL;
            }
        else // there is no transport
            {
            /*
            // Figure out the XML SD to utilize FOR transport FACTORY:
            CSenWSDescription* pDesc = NULL;
            MSenServiceDescription& desc = iServiceSession->AsServiceDescription();
            pDesc = (CSenWSDescription*)&desc;

            if(pDesc->Endpoint().Length()==0 && iInitializer && iInitializer->Endpoint().Length()>0)
                {
                LOG_WRITE_L("- Due session does not specify endpoint, initilizer is utilized instead.");
                // Only, and only if session does NOT know it's endpoint AND
                // initializer (SD) does specify endpoint, rely on initilizer SD
                pDesc = iInitializer;
                }
            LOG_WRITE_L("- Instantiating new transport for the consumed service session.");
            pTransport = transportFactory.CreateL(*pDesc);
            */
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Instantiating new transport for the service session");
            CSenWSDescription& description =  (CSenWSDescription&)iServiceSession->AsServiceDescription();

            if( description.HasSuperClass( MSenServiceDescription::EServiceSession ) )
                {
                CSenServiceSession& session = (CSenServiceSession&)description;
                pTransport = transportFactory.CreateL(*iInitializer, &session);
                }
            else
                {
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Fatal: session does not derive from CSenServiceSession(!)");
                pTransport = transportFactory.CreateL(*iInitializer, NULL);
                }

            }
        // Transfer the ownership of transport to the session
        iServiceSession->SetTransportL(pTransport);
        }
    else // no service session
        {
        // Check if this consumer already owns a transport or not:
        if(!ipTransport)
            {
            // Check if ther is initilizer 
            if(!iInitializer)
                {
                // This should never happen:
                CSLOG_L(iConnectionID,KMinLogLevel ,"FATAL - CSenClientSession::TransportL():");
                CSLOG_L(iConnectionID,KMinLogLevel ," - iInitializer == NULL!");
                User::Leave(KErrNotFound);
                }
            CSLOG_L(iConnectionID,KMinLogLevel ,"- Instantiating new transport for the service consumer (no session yet)");
            pTransport = transportFactory.CreateL(*iInitializer, NULL);
            ipTransport = pTransport;
            }
        else
            {
            // Utilize the transport owned by this consumer as long as session does not own one:
            pTransport = ipTransport;
            }
        }
    return *pTransport;
    }



TInt CSenClientSession::ReleaseServiceSession()
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ReleaseServiceSession");

    // Notify responsible framework, so that it 
    // can do required cleanup for the session
    TInt leaveCode(KErrNone);
    if(iServiceSession)
        {
        // Cease to consumer this session:
        TInt removeRetVal(KErrNone);
        TRAP( leaveCode, removeRetVal = iServiceSession->RemoveConsumerL(*this); )
#ifdef _SENDEBUG
        if(leaveCode!=KErrNone)
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- RemoveConsumerL leaved: %d"), leaveCode));
            removeRetVal = leaveCode;
            leaveCode=KErrNone;
            }
        else
            {
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- RemoveConsumerL returned: %d"), removeRetVal));
            }
#endif

        // Ask framework(s) to release this session: de-allocation is framework specific
        TRAP( leaveCode, iManager.NotifyFrameworksL(
                iServiceSession->FrameworkId(),
                KSenEventConsumerReleasesSession,
                iServiceSession); )
#ifdef _SENDEBUG
        if(leaveCode!=KErrNone)
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- NotifyFrameworksL leaved %d"), leaveCode));
            }
        else
            {
            CSLOG_L(iConnectionID,KMinLogLevel ," - NotifyFrameworksL OK.");
            }
#endif // _SENDEBUG

        if(leaveCode==KErrNone)
            {
            leaveCode=removeRetVal;
            }
        }


    return leaveCode;
    }

TInt CSenClientSession::CompletePendingMessages()
    {
    TInt leaveCode(KErrNone);
    TRAP( leaveCode, CompletePendingMessagesL(); )
    return leaveCode;
    }

void CSenClientSession::CompletePendingMessagesL()
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::CompletePendingMessagesL");

    // New: first call cancel from transport using pending txnIds 

    RTransactionIdArray& txnIds = TxnIds();

    TInt pendingCount(txnIds.Count());
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d txnIds(s) pending."), pendingCount));
    for(TInt i=pendingCount-1; i>=0; i--)
        {
        if(txnIds[i])
            {
            TInt* pTxnId = txnIds[i];
            TInt cancelledTxnId(*pTxnId);
            txnIds.Remove(i);
            delete pTxnId;
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Now processing txn with id %d, after which %d are left.."), cancelledTxnId, i));


            // Attempt to cancel the actual activity on the transport layer.
            // Note that even if transport will not cancel this transaction,
            // the response from the transport (later on) won't anymore be 
            // delivered to consumer, since this RMessage2 is removed from 
            // iMessageMap few lines below. This means, that from the WS-stack
            // client point-of-view, cancel works and no "stray" responses
            // will follow (eventhough some data is received from transport 
            // stack).
            MSenTransport* pTransport = ipTransport;
            if(iServiceSession)
                {
                CSLOG_L(iConnectionID,KMaxLogLevel ,"-- has session.");
                MSenTransport* pSessionTransport = iServiceSession->Transport();
                if(pSessionTransport)
                    {
                    CSLOG_L(iConnectionID,KMaxLogLevel ,"--- session has transport.");
                    // If session owns a transport, always utilize that one
                    pTransport = pSessionTransport;
                    }
                }
            // If session does not own a transport, check if this remote consumer does..

            if(pTransport)
                {
                CSLOG_L(iConnectionID,KMinLogLevel ,"---- About to call CancelTransaction");
                TInt retCode = pTransport->CancelTransaction(cancelledTxnId);
                CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Transport's  CancelTransaction(%d) returned: %d"), cancelledTxnId, retCode));
                retCode = 0; // not used in release builds atm. 
                }
            }
        }
    txnIds.ResetAndDestroy(); // deallocate TInt heap allocations


    // New: complete pending transaction RMessage2 objects:
    RTransactionArray& txnMsgs = Transactions();
    pendingCount = txnMsgs.Count();
    CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d RMessage2(s) pending."), pendingCount));
    TInt transactionID;
    TInt pendingTxnId;
    for(TInt t=pendingCount-1; t>=0; t--)
        {
        if(txnMsgs[t])
            {
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- %d left to complete."), t+1));
            CSenClientMessage* pClientMessage = Transactions()[t];
            RMessage2& message = pClientMessage->RMessage();
            
            pendingTxnId = pClientMessage->ChunkHeader().PendingTransactionId();
            if (pendingTxnId)
                {
                transactionID = pendingTxnId;
                pClientMessage->ChunkHeader().SetPendingTransactionId(0);
                pClientMessage->ChunkHeader().SetTransactionId(pendingTxnId);
                }
            else
                {
                transactionID = pClientMessage->ChunkHeader().TransactionId();
                }
            CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Completing Txn ID: %d"),transactionID)); 
            
            TPtr8 ptr(reinterpret_cast<TUint8*>(&transactionID), 
                      sizeof(transactionID), 
                      sizeof(transactionID)); 

            TInt leaveCode(KErrNone);
            TRAP(leaveCode, message.WriteL(1, ptr)); //IPC V2
            if(leaveCode!=KErrNone)
                {
                message.Complete(ESenInternalError);
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError - TxnId");
                }
            else
                {
                message.Complete(ESenServRequestCancelled);
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenServRequestCancelled - TxnId");
                }

            delete pClientMessage;
            txnMsgs.Remove(t);
            }
        }
    txnMsgs.ResetAndDestroy(); // deallocate RMessage2 heap alloacations
    }
    

TInt CSenClientSession::ProcessRequestFromConsumerL( const TDesC8& aMessage,
                                                     const TInt aTxnId,
                                                     MSenRemoteServiceConsumer& aConsumer )
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::ProcessRequestFromConsumerL");
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Consumer id: (%S)"), &aConsumer.Id() ));
    CSLOG_L(iConnectionID,KMaxLogLevel ,"##############################################");
    CSLOG_ALL(iConnectionID, KMaxLogLevel, ( aMessage ));
    CSLOG_L(iConnectionID,KMaxLogLevel ,"##############################################");
    CSenChunk* pChunk = NULL;
    TInt retVal = aConsumer.ChunkByTxnIdL( aTxnId, pChunk );

    if( retVal == KErrNone ) 
        {
        CSenClientMessage* pMessage = (CSenClientMessage*) pChunk; // this holds all the data written by SC
    
        if( ipHostletAwaitOp )
            {
            RHandleBase& handle = pMessage->Chunk();
#ifdef EKA2
            ipHostletAwaitOp->RMessage().Complete( handle );
#else // EKA1
            ipHostletAwaitOp->RMessage().Complete( handle.Handle() );
#endif // EKA2:EKA1
            delete ipHostletAwaitOp;
            ipHostletAwaitOp = NULL;
            }
        else // await op is in-progress / model is "busy"
            {
            CSLOG_L(iConnectionID,KMaxLogLevel ,"- Request has to be queued, await operation is already in progress (busy)");
            // 
            TInt* pTxnId = new (ELeave) TInt(aTxnId); //CodeScannerWarnings
            CleanupStack::PushL(pTxnId);
            RHostletRequestMap& map = HostletRequestMap();
            // Put this consumer request in queue and start waiting 
            // for next "await hostlet request" -operation to come.
            retVal = map.Append(pTxnId, pMessage);
            if( retVal == KErrNone )
                {
                CleanupStack::Pop( pTxnId );
                }
            else
                {
                CleanupStack::PopAndDestroy( pTxnId ); 
                }
            }
        }
    return retVal;
    }


// NOTE: closely similar to transactioncompletel(!) 
TInt CSenClientSession::OnServiceCompleteL( const TInt aTxnId, TInt aCompletionCode, const TDesC8& /* aDesc */ )
    {
    CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::OnServiceCompleteL (part of remote hostlet interface)");
    TInt retVal(KErrNotFound);
    RTransactionIdArray& ids = TxnIds();
    TInt index(0);
    TInt count(ids.Count());
    TInt* pTxnId = NULL;
    for(; index < count; index++)
        {
        pTxnId = ids[index];
        if(pTxnId && *pTxnId == aTxnId)
            {
            break;
            }
        }
        
    if(index < count)
        {
        RTransactionArray& transactions = Transactions();
        TInt count(transactions.Count());
        if( index < count)
            {
            CSenClientMessage* pClientMessage = transactions[index];
            RMessage2& message = pClientMessage->RMessage();

            TPtr8 ptr(reinterpret_cast<TUint8*>(&aCompletionCode),
                        sizeof(aCompletionCode), sizeof(aCompletionCode));

            TInt leaveCode(KErrNone);
            TRAP(leaveCode, message.WriteL(0, ptr)); //IPC V2
            if(leaveCode!=KErrNone)
                {
                // LEAVE
                CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Leave code from RMessage2::WriteL(completionCode): %d"), leaveCode));
                message.Complete(ESenInternalError);
                retVal = leaveCode;
                CSLOG_L(iConnectionID,KMinLogLevel ,"Completed with ESenInternalError");
                }
            else
                {
                // OK
                TInt transactionId(aTxnId);
                TPtr8 txnId(reinterpret_cast<TUint8*>(&transactionId),
                            sizeof(transactionId), sizeof(transactionId));
                TRAP(leaveCode, message.WriteL(1, txnId)); //IPC V2
                if(leaveCode!=KErrNone)
                    {
                    // LEAVE
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Leave code from RMessage2::WriteL(txnID): %d"), leaveCode));
                    message.Complete(ESenInternalError);
                    retVal = leaveCode;
                    CSLOG_L(iConnectionID,KMinLogLevel ,"Completed with ESenInternalError");
                    }
                else
                    {
                    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- Completion code: %d"), aCompletionCode));
                    message.Complete(ESenOnServiceComplete);
                    CSLOG_L(iConnectionID,KMinLogLevel ,"Completed with ESenOnServiceComplete");
                    }
                }
            transactions.Remove(index);
            delete pClientMessage;
            retVal = KErrNone; // all went ok
            }
        else
            {
            retVal = KErrNotFound;
            }
        ids.Remove(index); // de-allocate TInt which was allocated from heap
        delete pTxnId;
        }
    else
        {
        retVal = KErrNotFound;
        }
        
    return retVal;    
    }


TInt CSenClientSession::ChunkByTxnIdL( TInt aTxnId, CSenChunk*& aChunk )
    {
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::ChunkByTxnIdL(%d)"), aTxnId));
    aChunk = NULL;

    TInt retVal = TransactionIndex(aTxnId);
    if(retVal > KErrNotFound)
        {
        RTransactionArray& transactions = Transactions();
        if( retVal < transactions.Count() )
            {
            CSenClientMessage* pClientMessage = transactions[retVal];
            aChunk = pClientMessage;
            retVal = KErrNone;
            }
        else           
            {
            return KErrNotFound; // "KErrIndexOutOfBounds"
            }
        }
     return retVal; // always KErrNone or KErrNotFound
     }
     
MSenMessageContext* CSenClientSession::MessageContextByTxnIdL( TInt aCtxId, TInt& aError )
    {
    CSenClientMessage* pClientMessage = NULL;
    TInt retVal = ContextIndex(aCtxId); // retVal == "context index" or ERROR
    if(retVal > KErrNotFound) 
        {
        RTransactionArray& transactions = Transactions();        
        if( retVal < transactions.Count() )
            {
            pClientMessage = transactions[retVal];
            if( pClientMessage )
                {
                aError = KErrNone;
                return &pClientMessage->MessageContext(); 
                }
            }
        else  
            {
            // "KErrIndexOutOfBounds"          
            aError = KErrNotFound;
            }
        }
    else        
        {
        aError = retVal;
        }
    return NULL;
    }
    
void CSenClientSession::StateChanged(TInt aState)
    {
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("CSenClientSession::StateChanged(%d)"), aState));
    RProperty::Set(iClientSecureID,(KSenRPropertyKeyBaseNumber+iConnectionID), aState);
    }
    
TBool CSenClientSession::HasAuthenticationCallback()
    {
    return iConnectionHasAuthenticationCallback;
    }
TBool CSenClientSession::HasCoBrandingCallback()
    {
    return iConnectionHasCoBrandingCallback;
    }

void CSenClientSession::SearchIdentityProviderL(const RMessage2& aMessage)
	{
	TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;

    pSenChunk = CSenChunk::NewLC(KNullDesC);
    pSenChunk->SetLogger(Log());
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        TInt length = aMessage.Int2();
        if(length < 1)
            {
            CSLOG_L(iConnectionID,KMaxLogLevel ,"- Tried search Identity Provide with null provider Id descriptor.");
            aMessage.Complete(KErrBadDescriptor);
            return;
            }
        
        HBufC8* inBuf = HBufC8::NewLC(length);
        TPtr8 providerId = inBuf->Des();
        aMessage.ReadL(1, providerId);
        
	    CSenWSDescription* pPattern = CSenWSDescription::NewLC();
	    pPattern->SetEndPointL(providerId);
	    CSenIdentityProvider* pIdp = ((MSenCoreServiceManager&)iManager).IdentityProviderL(*pPattern);
	    CleanupStack::PopAndDestroy(pPattern);
	    CleanupStack::PopAndDestroy(inBuf);
	    if ( pIdp )
	        {
	        HBufC8* pIdpAsXml = pIdp->AsXmlL();
	        CleanupStack::PushL(pIdpAsXml);
	        retVal = pSenChunk->DescToChunk(*pIdpAsXml);
	        CleanupStack::PopAndDestroy(pIdpAsXml);
	        }
	    else
	    	{
	        retVal = KErrNotFound;
	        }
        }

    CleanupStack::PopAndDestroy(pSenChunk);
    aMessage.Complete(retVal);
	
	}
TBool CSenClientSession::IsTrustedClient(const RMessage2& aMessage)
    {
    TVendorId vendor = aMessage.VendorId();
	_LIT_VENDOR_ID(trustedVendor,VID_DEFAULT);
    
    if (vendor.iId == trustedVendor.iId )
        return ETrue;
    else
        return EFalse;

    }
TBool CSenClientSession::CleanPassword(const RMessage2& aMessage)
	{
    TSecureId sid = aMessage.SecureId();
    _LIT_SECURE_ID(nokia_chateventmonitorserver, 0x2001B2C5 );
    _LIT_SECURE_ID(nokia_xmppnaui, 0x2001A95B);
    if ((sid.iId == nokia_chateventmonitorserver.iId) || (sid.iId == nokia_xmppnaui.iId))
        return EFalse;
    else
        return ETrue;
    }
TSecureId CSenClientSession::SecureId()
	{
	return iSecureId;
	}

TVendorId CSenClientSession::VendorId()
	{
	return iVendorId;
	}
void CSenClientSession::IdentityProviders(const RMessage2& aMessage)
	{
	
	TInt retVal(KErrNone);
    CSenChunk* pSenChunk = NULL;
    pSenChunk = CSenChunk::NewLC(KNullDesC);
    retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
    
    if ( retVal == KErrNone )
        {
        const RPointerArray<CSenIdentityProvider> pIdps = ((MSenCoreServiceManager&)iManager).IdentityProvidersL();
        
        const TInt count = pIdps.Count();
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- TOTAL IDP Found: %d"), count));		
  
        // Calculate required heap allocation size:
        TInt size(0);
        if(count>0)
            {
            size += (count-1) * KTab().Length();
            }

        TInt leaveCode(KErrNone);
        TRAP(leaveCode, size += iManager.SizeOfIdentityProvidersL(pIdps); )
        if(leaveCode!=KErrNone)
            {
            CleanupStack::PopAndDestroy(pSenChunk);
            aMessage.Complete(leaveCode);
            }
        CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::IdentityProviders");
        CSLOG_FORMAT((iConnectionID, KNormalLogLevel , _L8("- Size of SD(s) + delimiter(s): %d bytes"), size));

        // Adjust RChunk to accomodate found IdP using known hnalde
        TPtr8* pIdPrs = NULL;
        retVal = pSenChunk->AllocDescToChunk(size, pIdPrs); 
        const TBool trusted = IsTrustedClient( aMessage );
        const TBool cleanPass = CleanPassword( aMessage );
        if(retVal==KErrNone && pIdPrs)
            {
            for ( TInt i = 0; i < count; ++i )
                {
                if ( i >= 1 )
                	{
                    pIdPrs->Append( KTab );
                	}
                
                CBufFlat* pBuf= CBufFlat::NewL( KFlatBufSize );
                CleanupStack::PushL( pBuf );
                
                RBufWriteStream bufWs( *pBuf );
                CleanupClosePushL( bufWs );
                
                HBufC8* passDec = SenXmlUtils::DecodeHttpCharactersLC( pIdps[i]->Password() );
                RBuf8 password;
                password.Create( *passDec );
                CleanupStack::PopAndDestroy( passDec );                
                
                if ( !trusted )
                    {
                    // Clean AutoSignIn
                    TBool rollbackAutoMode = EFalse;
                    CSenElement* autoSignInEl( NULL );
                    
                    if ( pIdps[i]->FrameworkId() == KDefaultOviFrameworkID )
                        {
                        CSenElement* el( NULL );
                        pIdps[i]->AccountExtensions( el );
                
                        if ( el )
                            {
                            autoSignInEl = el->Element( KSenAccAutoSignIn );
                    
                            if ( autoSignInEl && autoSignInEl->Content() == KSenPropertyTrue )
                                {
                                autoSignInEl->SetContentL( KSenPropertyFalse );
                                rollbackAutoMode = ETrue;
                                }
                            }         
                        }
                    
                    pIdps[i]->SetUserInfoL( pIdps[i]->AuthzID(),
                                            pIdps[i]->AdvisoryAuthnID(),
                                            KNullDesC8() );
                    
                    pIdps[i]->WriteAsXMLToL( bufWs );
                    
                    if ( rollbackAutoMode ) //autoSignInEl has to exists, otherwise rollbackAutoMode is false
                    	{
                    	autoSignInEl->SetContentL( KSenPropertyTrue );
                    	}
                    TRAP(retVal,
			            	    HBufC8* encPassword = SenXmlUtils::EncodeHttpCharactersLC(password);
		                    pIdps[i]->SetUserInfoL( pIdps[i]->AuthzID(),
		                                            pIdps[i]->AdvisoryAuthnID(),
		                                            *encPassword );
		  	          	    CleanupStack::PopAndDestroy(encPassword);
  	          	    		);
                    }
                else//trusted application
                    {
                    if (cleanPass)//special case for Yukon
                    	{
                    	pIdps[i]->SetUserInfoL( pIdps[i]->AuthzID(),
	                                            pIdps[i]->AdvisoryAuthnID(),
	                                            KNullDesC8() );
	                    pIdps[i]->WriteAsXMLToL( bufWs );
	                    HBufC8* encPassword = SenXmlUtils::EncodeHttpCharactersLC(password);
	                    pIdps[i]->SetUserInfoL( pIdps[i]->AuthzID(),
	                                            pIdps[i]->AdvisoryAuthnID(),
	                                            *encPassword );
	            	    CleanupStack::PopAndDestroy(encPassword);
                    	}
                    else
                    	{
                    	pIdps[i]->WriteAsXMLToL( bufWs );
                    	}
                    }
                password.Close();
                // Add this SD as XML into the "big heap buffer":
                TPtr8 p8 = pBuf->Ptr(0);
                pIdPrs->Append(p8); 
                // Destroy the temporary, flat stream buffer
                CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
                
                pBuf = NULL;
                }
            CSLOG_L(iConnectionID,KMaxLogLevel ,"- Requested descriptions:");
            CSLOG_ALL(iConnectionID,KMaxLogLevel ,(*pIdPrs));
		     CSLOG_L(iConnectionID,KMinLogLevel ,"CSenClientSession::IdentityProviders completed");			
            }
    #ifdef _SENDEBUG
        else
            {
            CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- AllocDescToRMsgL failed: %d"), retVal));
            }
    #endif // _SENDEBUG

        }
    CleanupStack::PopAndDestroy(pSenChunk); // Close SenChunk

    aMessage.Complete(retVal);
	
	}

void CSenClientSession::ObserveTransfer(const RMessage2& aMessage)
	{
    CSLOG_L( iConnectionID,KMinLogLevel, "CSenClientSession::ObserveTransfer" );	
	TInt retVal(KErrNone);
	if(!iConnectionSession.Handle())
		{
		CSLOG_L( iConnectionID,KMinLogLevel, " iConnectionSession is not open. Creating it now." );	
		retVal = iConnectionSession.CreateSession(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- CreateSession returned: %d" ), retVal ));	
		}
	if(retVal == KErrNone)
		{
		CSLOG_L( iConnectionID,KMinLogLevel, " - Calling InitProgressObserver()." );	
		retVal = iConnectionSession.InitProgressObserver(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- InitProgressObserver() returned: %d" ), retVal ));	
		if(retVal != KErrNone)
			{
    		CSLOG_L( iConnectionID,KMinLogLevel, " => Forced to close iConnectionSession." );	
			iConnectionSession.Close();
			}
		}
	aMessage.Complete(retVal);
	}
void CSenClientSession::ObserveCoBranding(const RMessage2& aMessage)
	{
	TInt retVal(KErrNone);
	if(!iConnectionSession.Handle())
		{
		CSLOG_L( iConnectionID,KMinLogLevel, " iConnectionSession is not open. Creating it now." );	
		retVal = iConnectionSession.CreateSession(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- CreateSession returned: %d" ), retVal ));	
		}
	if(retVal == KErrNone)
		{
		CSLOG_L( iConnectionID,KMinLogLevel, " - Calling InitCoBrandingObserver()." );	
		retVal = iConnectionSession.InitCoBrandingObserver(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- InitCoBrandingObserver() returned: %d" ), retVal ));	
		if(retVal != KErrNone)
			{
    		CSLOG_L( iConnectionID,KMinLogLevel, " => Forced to close iConnectionSession." );	
			iConnectionSession.Close();
			}
		else
    		{
    		iConnectionHasCoBrandingCallback = ETrue;
    		}
		}
	aMessage.Complete(retVal);
	}
void CSenClientSession::ObserveAuthCallback(const RMessage2& aMessage)
	{
	TInt retVal(KErrNone);
	if(!iConnectionSession.Handle())
		{
		CSLOG_L( iConnectionID,KMinLogLevel, " iConnectionSession is not open. Creating it now." );	
		retVal = iConnectionSession.CreateSession(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- CreateSession returned: %d" ), retVal ));	
		}
	if(retVal == KErrNone)
		{
		CSLOG_L( iConnectionID,KMinLogLevel, " - Calling InitAuthObserver()." );	
		retVal = iConnectionSession.InitAuthObserver(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- InitAuthObserver() returned: %d" ), retVal ));	
		if(retVal != KErrNone)
			{
    		CSLOG_L( iConnectionID,KMinLogLevel, " => Forced to close iConnectionSession." );	
			iConnectionSession.Close();
			}
		}
	aMessage.Complete(retVal);
	}

#ifdef __ENABLE_ALR__    
void CSenClientSession::ObserveMobiltyService(const RMessage2& aMessage)
   {
   CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::ObserveMobiltyService registered ");
   TInt retVal(KErrNone);
   if(!iConnectionSession.Handle())
        {
        CSLOG_L( iConnectionID,KMinLogLevel, " iConnectionSession is not open. Creating it now." );	
        retVal = iConnectionSession.CreateSession(aMessage);
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- CreateSession returned: %d" ), retVal ));	
        }
   if(retVal == KErrNone)
        {
		CSLOG_L( iConnectionID,KMinLogLevel, " - Calling InitMobilityObserver()." );	
        retVal = iConnectionSession.InitMobilityObserver(aMessage);        
		CSLOG_FORMAT(( iConnectionID,KMinLogLevel, _L8("- InitMobilityObserver() returned: %d" ), retVal ));	
        if(retVal != KErrNone)
            {
    		CSLOG_L( iConnectionID,KMinLogLevel, " => Forced to close iConnectionSession." );	
            iConnectionSession.Close();
            }
        }
   aMessage.Complete(retVal);
   }

void CSenClientSession::MigrateToPrefferedCarrierL(const RMessage2& aMessage)
   {
   CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::MigrateToPrefferedCarrier ");			
   HBufC8* inBuf = HBufC8::NewLC(10);
   TPtr8 inBuffer = inBuf->Des();
   aMessage.ReadL(0, inBuffer); // boolean
   TBool userChoice = (TBool)(*inBuffer.Ptr()) ;
   CleanupStack::PopAndDestroy(); // inBuf			
   aMessage.Complete(KErrNone);
   iMobilityChoice = userChoice ;   
   MigrateToPrefferedCarrier(userChoice) ;
   }

void CSenClientSession::MigrateToPrefferedCarrier(TBool aUserChoice)
   {
   CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::MigrateToPrefferedCarrier() ");
    //if pending transaction are there we need to wait or cancel all transaction and then migrate //CompletePendingMessages()
    RTransactionIdArray& txnIds = TxnIds();
    TInt pendingCount(txnIds.Count());
    if (pendingCount == 0)
        {
        if (aUserChoice != EFalse)
            {
            CSLOG_L(iConnectionID, KMinLogLevel , "-calling iALRObserver->MigrateToPreferredCarrier ");		
            iALRObserver->MigrateToPreferredCarrier() ;
            }
        else
            {
            CSLOG_L(iConnectionID, KMinLogLevel , "-calling iALRObserver->IgnorePreferredCarrier ");		
            iALRObserver->IgnorePreferredCarrier() ;				
            }
        }
    else
        {
        CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("-call to iALRObserver->MigrateToPrefferedCarrier cant be performed as %d txnIds(s) pending."), pendingCount));
        }    
    }

void CSenClientSession::NewCarrierAcceptedL(const RMessage2& aMessage)
    {
    CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::NewCarrierAccepted ");				
    HBufC8* inBuf = HBufC8::NewLC(10);
    TPtr8 inBuffer = inBuf->Des();
    aMessage.ReadL(0, inBuffer); // boolean
    TBool userChoice = (TBool)(*inBuffer.Ptr()) ;
    CleanupStack::PopAndDestroy(); // inBuf	
    aMessage.Complete(KErrNone);

    NewCarrierAcceptedL(userChoice) ;        
    }

void CSenClientSession::NewCarrierAcceptedL(TBool aUserChoice)
   {
   CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::NewCarrierAccepted() ");
    if (aUserChoice != EFalse)
        {
        TInt retVal(KErrNone);
        CSLOG_L(iConnectionID, KMinLogLevel , "-calling iALRObserver->NewCarrierAccepted ");
        iALRObserver->NewCarrierAccepted() ;
        //now we have new iap id and we can even propagate to transport plugin
        //set transport properties
        CSLOG_L(iConnectionID, KMinLogLevel , "-calling iALRObserver->GetNewIapAsTransportPropertyL ");
        HBufC8* transportProperties = iALRObserver->GetNewIapAsTransportPropertyL() ;
        if(transportProperties != NULL)
            {
            CleanupStack::PushL (transportProperties) ;
            }
        if ( iServiceSession && transportProperties )
            {
            CSLOG_L(iConnectionID, KMinLogLevel , "-setting up new transport properties with new iap id received from iALRObserver ");
            retVal = iServiceSession->SetTransportPropertiesL(*transportProperties, *this);
            }
        else
            {
            retVal = KErrSenNotInitialized;
                 CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("-CSenClientSession::NewCarrierActive SetTransportPropertiesL failed with %d "), retVal)) ;
            //HandleErrorL(KErrSenNotInitialized, KNullDesC8) ;
            }
        if(transportProperties != NULL)
            {
             	CleanupStack::PopAndDestroy(transportProperties); // transportProperties	        
            }
        }
    else
        {
        CSLOG_L(iConnectionID, KMinLogLevel , "-calling iALRObserver->NewCarrierRejected ");		
        iALRObserver->NewCarrierRejected() ;			
        }
   }

void CSenClientSession::PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                        TAccessPointInfo aNewAPInfo,
                                        TBool aIsUpgrade,
                                        TBool aIsSeamless )
	{
    CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::PreferredCarrierAvailable ");
    
    RTransactionIdArray& txnIds = TxnIds();
    TInt pendingCount(txnIds.Count());
      
    if (iConnectionSession.Handle() && pendingCount == 0)
        {
        if (aIsSeamless)
            {
              // in S60 3.2, this situation cannot occur.
            CSLOG_L(iConnectionID, KMinLogLevel , "-it is seemless in S60 3.2, this situation cannot occur. ");
            }
        else if (iMobilityCallBackForwarded == EFalse)
            {
            CSLOG_L(iConnectionID, KMinLogLevel , "-Calling iConnectionagentSession PreferredCarrierAvailable() ");
    		iConnectionSession.PreferredCarrierAvailable(aOldAPInfo, aNewAPInfo, aIsUpgrade, aIsSeamless) ;
            iMobilityCallBackForwarded = ETrue ;
            }
        else if(iMobilityCallBackForwarded != EFalse)
            {
            CSLOG_L(iConnectionID, KMinLogLevel , "-PreferredCarrierAvailable callback is already forwared to client ");
            CSLOG_L(iConnectionID, KMinLogLevel , "-Client session has user choice by now and we can reuse it");
            MigrateToPrefferedCarrier (iMobilityChoice) ;
            }
        }	
	else
		{
        CSLOG_L(iConnectionID, KMinLogLevel , "MobiltyObserver is not registered ");
        CSLOG_L(iConnectionID, KMinLogLevel , "switching to new carrier automaticaly");
        MigrateToPrefferedCarrier (ETrue) ;
	    }
    }

void CSenClientSession::Error( TInt aError )
	{
    CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("-CSenClientSession::Error aError = %d"), aError));
    iMobilityCallBackForwarded = EFalse ; //reset the flag
    iMobilityChoice = EFalse ;       
    if (iConnectionSession.Handle())
        {
        CSLOG_L(iConnectionID, KMinLogLevel , "-Calling iConnectionagentSession MobilityError() ");
        iConnectionSession.MobilityError(aError) ;        
        }		
    else
        {
        CSLOG_L(iConnectionID, KMinLogLevel , "MobiltyObserver is not registered ");
        CSLOG_L(iConnectionID, KMinLogLevel , "-iMobiltyObserver->Error not called ");
        }
	}

void CSenClientSession::NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless )
    {
    CSLOG_L(iConnectionID, KMinLogLevel , "-CSenClientSession::NewCarrierActive ");
    if (iConnectionSession.Handle())
        {
        CSLOG_L(iConnectionID, KMinLogLevel , "-Calling iConnectionagentSession NewCarrierActive() ");
        iConnectionSession.NewCarrierActive(aNewAPInfo, aIsSeamless) ;
        iMobilityCallBackForwarded = EFalse ; //reset the flag
        iMobilityChoice = EFalse ;        
        }	
    else
        {
        CSLOG_L(iConnectionID, KMinLogLevel , "MobiltyObserver is not registered ");
        CSLOG_L(iConnectionID, KMinLogLevel , "activating new carrier automaticaly");
        TRAPD(retVal, NewCarrierAcceptedL(ETrue));
        }
    }
#endif // __ENABLE_ALR__    



TInt CSenClientSession::CompleteReauthLoop(CSenChunk& aSenChunk) //also completes transaction
	{
	CSLOG_L(iConnectionID,KMinLogLevel ,"- CompleteReauthLoop");
	TInt retVal(KErrNone);

	TInt previousTxnId = aSenChunk.ChunkHeader().TransactionId();
	aSenChunk.ChunkHeader().SetTransactionId(KErrNotFound);
	TInt transactionIndex = TransactionIndex(KErrNotFound);
	aSenChunk.ChunkHeader().SetTransactionId(previousTxnId);
	
	if ( transactionIndex != KErrNotFound )
		{
		CSenClientMessage* pClientMessage = iPendingTransactions[transactionIndex];
		RMessage2& message = pClientMessage->RMessage();
		

            TPtr8 ptr(reinterpret_cast<TUint8*>(&previousTxnId), 
                      sizeof(previousTxnId), 
                      sizeof(previousTxnId)); 

            TRAPD(leaveCode, message.WriteL(1, ptr)); //IPC V2
            if(leaveCode!=KErrNone)
                {
                message.Complete(ESenInternalError);
                CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenInternalError");
                }
            else // no internal error in writing error message
                {
		      	pClientMessage->ChunkHeader().SetPendingTransactionId(0);
		      	pClientMessage->ChunkHeader().SetTransactionId(previousTxnId);
			pClientMessage->ChunkHeader().SetMessageDirection( MSenMessage::EOutbound );
            
			//CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- ErrCode: %d"), KErrSenFailedAuthentication));
			CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- ErrCode: %d"), KErrSenAuthenticationFault));
			CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- transactionIndex  : %d"), transactionIndex));
			CSLOG_FORMAT((iConnectionID, KMinLogLevel , _L8("- previousTxnId  : %d"), previousTxnId));
			iReauthResendNeeded = ETrue ;
			message.Complete(ESenReAuthAndResendNeeded);
			CSLOG_L(iConnectionID,KMinLogLevel ,"- Completed with ESenReAuthAndResendNeeded");
				
				iPendingTransactions.Remove( transactionIndex );
				delete pClientMessage;
                }
		}
	else
	    {
	    retVal = KErrNotFound;
	    }
	
	return retVal;
	}

TInt CSenClientSession::ReauthenticationNeededL(CSenIdentityProvider*& aIdp)
	{
	_LIT(chunkNameLit, "caChunk"); // lenght = 7
	const TInt KMaxChunkNumDecimals = 64;
	const TInt maxChunkNameLen = KMaxChunkNumDecimals + 7;
	
	TInt retVal(KErrNotFound);
	if(aIdp)
		{
		HBufC* chunkNameDes = HBufC::NewLC(maxChunkNameLen);
		TPtr chunkName = chunkNameDes->Des();
		chunkName.Copy(chunkNameLit);
		chunkName.AppendNum(iConnectionID);

		CSenChunk* pSenChunk = CSenChunk::NewLC(chunkName.Left(KMaxName));
		pSenChunk->SetLogger(Log());
		retVal = pSenChunk->CreateChunk();
		if(retVal == KErrNone)
			{
			// Write identity provider to chunk
			HBufC8* desIdp = aIdp->AsXmlL();
			CleanupStack::PushL(desIdp);
			retVal = pSenChunk->DescToChunk(*desIdp);
			CleanupStack::PopAndDestroy(desIdp);
			if(retVal == KErrNone)
				{
				if(iConnectionSession.Handle())
			    	{
			    	retVal = iConnectionSession.ReauthenticationNeeded(*pSenChunk);
			    	}
				else
					{
					retVal = KErrSenInternal;
					}
				
				if(retVal == KErrNone)
					{
					TPtrC8 identityProvider;
					retVal = pSenChunk->DescFromChunk(identityProvider);
					if(retVal == KErrNone)
						{
						CSenIdentityProvider* pIdentityProvider =
							CSenIdentityProvider::NewLC(KNullDesC8);
						CSenXmlReader* pXmlReader = CSenXmlReader::NewLC(KXmlParserMimeType);
						pXmlReader->SetContentHandler( *pIdentityProvider );
						pIdentityProvider->SetReader(*pXmlReader);
	
						TInt leaveCode(KErrNone);
						TRAP( leaveCode, pIdentityProvider->ParseL( identityProvider ); )
						                    
						CleanupStack::PopAndDestroy(pXmlReader);
						if ( leaveCode != KErrNone )
							{
							retVal = KErrSenInternal;
							CleanupStack::PopAndDestroy(pIdentityProvider);
							}
						else
							{
							// Store provider Id before registering new IDP (in case it gets deleted by manager)
							TPtrC8 providerId = pIdentityProvider->ProviderID();
							HBufC8* pProviderIdBuf = providerId.AllocLC();
							retVal = iManager.RegisterIdentityProviderL(pIdentityProvider);
							// Ownership transfered to manager
							CleanupStack::Pop(2); //pProviderIdBuf, pIdentityProvider
							CleanupStack::PushL(pProviderIdBuf);
							if(retVal == KErrNone)
								{
								// Retrieve new IDP and update reference
								CSenWSDescription* pPattern = CSenWSDescription::NewLC();
								pPattern->SetEndPointL(*pProviderIdBuf);
								CSenIdentityProvider* connIdp = ((MSenCoreServiceManager&)iManager).IdentityProviderL(*pPattern);
								CleanupStack::PopAndDestroy(pPattern);
								if(connIdp)
									{
									aIdp = connIdp;
									}
								}
							CleanupStack::PopAndDestroy(pProviderIdBuf);
							}
						}
					}
				}
			}
			CleanupStack::PopAndDestroy(pSenChunk);
			CleanupStack::PopAndDestroy(chunkNameDes);
		}
	return retVal;
	}

CSenClientMessage* CSenClientMessage::NewL( const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer )
    {
    CSenClientMessage* pOperation = NewLC(aMessage, apConsumer);
    CleanupStack::Pop();
    return pOperation;
    }

CSenClientMessage* CSenClientMessage::NewLC( const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer )
    {
    CSenClientMessage* pOperation = new (ELeave) CSenClientMessage();
    CleanupStack::PushL(pOperation);
    pOperation->ConstructL(aMessage, apConsumer);
    return pOperation;
    }

CSenClientMessage* CSenClientMessage::NewL( const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer, MSenTransport& aTransport )
    {
    CSenClientMessage* pOperation = NewLC(aMessage, apConsumer, aTransport);
    CleanupStack::Pop();
    return pOperation;
    }

CSenClientMessage* CSenClientMessage::NewLC( const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer, MSenTransport& aTransport )
    {
    CSenClientMessage* pOperation = new (ELeave) CSenClientMessage();
    CleanupStack::PushL(pOperation);
    pOperation->ConstructL(aMessage, apConsumer, aTransport);
    return pOperation;
    }


void CSenClientMessage::ConstructL(const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer)
    {
    iMessage = aMessage;
    ipConsumer = apConsumer;
    ipChunkName = NULL;
    }

void CSenClientMessage::ConstructL(const RMessage2& aMessage, 
                                   MSenRemoteServiceConsumer* apConsumer,
                                   MSenTransport& aTransport)
    {
    ConstructL(aMessage, apConsumer);
    MSenProperties& p = aTransport.PropertiesL();
    /*
    CSenLayeredTransportProperties* ctxProperties = NULL;
    if( p.PropertiesClassType() == MSenProperties::ESenLayeredTransportProperties ||
        p.PropertiesClassType() == MSenProperties::ESenLayeredHttpTransportProperties ||
        p.PropertiesClassType() == MSenProperties::ESenLayeredVtcpTransportProperties)
        {
        CSenLayeredXmlProperties* properties = (CSenLayeredTransportProperties*)&p;
        ctxProperties = (CSenLayeredTransportProperties*)properties->CloneL();
        }
    else
        {        
        ctxProperties = CSenLayeredTransportProperties::NewL();    
        }
    ipMessageContext = CSenMessageContext::NewL( SenContext::EOutgoing, ctxProperties );
    */
    TInt error(KErrNone);
    ipMessageContext = CSenMessageContext::NewL( SenContext::EOutgoing, p.Clone(error) );
    User::LeaveIfError(error);
    }
    
CSenClientMessage::CSenClientMessage()
    {
    }
    
TInt CSenClientMessage::CtxId()
    {
    TInt ctxId = KErrNotFound;
    if( ipMessageContext )  
        {      
        TInt leaveCode( KErrNone );   
        CSenChunk* pChunk = NULL;
        TRAP( leaveCode, pChunk = ipMessageContext->ChunkL(); )
        if( !leaveCode && pChunk )
            {
            ctxId = pChunk->ChunkHeader().ContextId();                
            }
        }
    return ctxId;        
    }
    
CSenClientMessage::~CSenClientMessage()
    {
    delete ipMessageContext;
    ipMessageContext = NULL;
    iBinaryDataArray.ResetAndDestroy();
    }

RMessage2& CSenClientMessage::RMessage()
    {
    return iMessage;
    }
    
MSenRemoteServiceConsumer* CSenClientMessage::Consumer()
    {
    return ipConsumer;
    }
    
MSenMessageContext& CSenClientMessage::MessageContext()
    {
    return *ipMessageContext;
    }

RPointerArray<CSenBinaryData>& CSenClientMessage::BinaryDataArray()
    {
    return iBinaryDataArray;
    }

CSenBinaryData* CSenBinaryData::NewL(RChunk& aChunk, TDesC8& aCid, TInt aOffset, TInt aSize)
    {
    CSenBinaryData* pSelf = CSenBinaryData::NewLC(aChunk, aCid, aOffset, aSize);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }
    
CSenBinaryData* CSenBinaryData::NewLC(RChunk& aChunk, TDesC8& aCid, TInt aOffset, TInt aSize)
    {
    CSenBinaryData* pSelf = new (ELeave) CSenBinaryData(aOffset, aSize);
    CleanupStack::PushL(pSelf);
    pSelf->BaseConstructL(aChunk, aCid);
    return pSelf;
    }
    
CSenBinaryData* CSenBinaryData::NewL(RFs& aFs, RFile& aFile, TDesC8& aCid, TInt aOffset, TInt aSize)
    {
    CSenBinaryData* pSelf = CSenBinaryData::NewLC(aFs, aFile, aCid, aOffset, aSize);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }
    
CSenBinaryData* CSenBinaryData::NewLC(RFs& aFs, RFile& aFile, TDesC8& aCid, TInt aOffset, TInt aSize)
    {
    CSenBinaryData* pSelf = new (ELeave) CSenBinaryData(aOffset, aSize);
    CleanupStack::PushL(pSelf);
    pSelf->BaseConstructL(aFs, aFile, aCid);
    return pSelf;
    }
    
CSenBinaryData::~CSenBinaryData()
    {
    iFs.Close();
    iFile.Close();
    iChunk.Close();
    delete ipCid;
    }
        
RChunk& CSenBinaryData::Chunk()
    {
    return iChunk;
    }
    
RFile& CSenBinaryData::File()
    {
    return iFile;
    }
    
TDesC8& CSenBinaryData::Cid()
    {
    return *ipCid;
    }
    
TInt CSenBinaryData::Offset()
    {
    return iOffset;
    }
    
TInt CSenBinaryData::Size()
    {
    return iSize;
    }

CSenBinaryData::TSenBinaryDataType CSenBinaryData::Type()
    {
    return iType;
    }

CSenBinaryData::CSenBinaryData(TInt aOffset, TInt aSize)
:   iOffset(aOffset),
    iSize(aSize)
    {
    }
        
void CSenBinaryData::BaseConstructL(RChunk& aChunk, TDesC8& aCid)
    {
    iChunk  = aChunk;
    ipCid    = aCid.AllocL();
    iType = EChunk;
    }

void CSenBinaryData::BaseConstructL(RFs& aFs, RFile& aFile, TDesC8& aCid)
    {
    iFs     = aFs;
    iFile   = aFile;
    ipCid    = aCid.AllocL();
    iType = EFile;
    }

TFileOutgoingTransferProgress::TFileOutgoingTransferProgress( TInt aTxnId, TBool aIsIncoming, TInt aProgress, TBool aHasCidPostfix, TInt aCid, TInt aCid2 )
    {
    iTxnId = aTxnId;
    iIsIncoming = aIsIncoming;
    iProgress = aProgress;
    iCid = aCid;
    iCid2 = aCid2;
    iHasCidPostfix = aHasCidPostfix;
    }
// END OF FILE

