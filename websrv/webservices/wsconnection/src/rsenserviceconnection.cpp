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








#include <e32math.h>
#include <f32file.h>

#include "sendebug.h"                 // internal Utils\inc - logging MACROs
#include "senlogger.h"
#include "senservicemanagerdefines.h" // internal Core\inc  - IPC enumerations

#include "rsenserviceconnection.h"

#include <SenSoapMessage.h>

#if defined (__WINS__) && !defined(EKA2)
static const TUint KServerMinHeapSize =    0x1000;     //  4K
static const TUint KServerMaxHeapSize =    0x100000;   // 1000K
#endif

static TInt StartServerL();
static TInt CreateServerProcessL();

CSenProgressResourceHandler::CSenProgressResourceHandler(TInt aTxnId,
        TBool aIncoming, TBool aIsSoap, TInt aProgress,
        RSenServiceConnection* aOwner): CActive(EPriorityStandard),
        iOwner(aOwner)
    {
    TTransferProgress& data = iProgressData();
    data.iTxnId = aTxnId;
    data.iIncoming = aIncoming;
    data.iSoap = aIsSoap;
    data.iProgress = aProgress;
    }

CSenProgressResourceHandler::~CSenProgressResourceHandler()
    {
    delete iSoapOrCid;
    }

void CSenProgressResourceHandler::DoCancel()
    {
    
    }

void CSenProgressResourceHandler::RunL()
    {
    iOwner->Remove(this);
    delete this;
    }

TInt CSenProgressResourceHandler::SetBuffer(const TDesC8& aSoapOrCid)
    {
    iSoapOrCid = aSoapOrCid.Alloc();
    if (iSoapOrCid)
        {
        return KErrNone;
        }
    return KErrNoMemory;
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

RSenServiceConnection::RSenServiceConnection(RFileLogger* aLog)
    : RSessionBase(),
      iMessageContextId(0),
      iLog(aLog)
    {
    // No implementation required
    }

void RSenServiceConnection::SetLog(RFileLogger* aLog)
    {
    iLog = aLog;
    }
void RSenServiceConnection::SetChannel(TInt aChannel)
{
     iTLSLogChannel=aChannel;
}

TInt RSenServiceConnection::Connect()
    {
    TInt error(KErrNone);
    TRAPD( leave, error = ::StartServerL(); )
    
    if( leave != KErrNone )
        {
        error = leave;
        }
        
    if( error == KErrNone )
        {
        error = CreateSession( KSenServiceManager, Version(), KDefaultMessageSlots );
        /// By defining TRequestStatus (last arg) to NULL, one could force 
        /// error = CreateSession( KSenServiceManager, Version(), KDefaultMessageSlots, NULL, NULL ); 
#ifdef _SENDEBUG
        if(error != KErrNone)
            {
            TLSLOG_FORMAT((KSenServiceConnectionLogChannel, KMinLogLevel , _L8("RSenServiceConnection::Connect - CreateSession returned error: (%d)"), error));
            }
        else
            {
            TLSLOG_L(KSenServiceConnectionLogChannel, KMinLogLevel,"RSenServiceConnection::Connect - CreateSession returned KErrNone");
            }
#endif // _SENDEBUG
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannel, KMinLogLevel , _L8("RSenServiceConnection::Connect - StartServerL failed, error: (%d)"), error));
        }
#endif // _SENDEBUG
    return error;
    }

TVersion RSenServiceConnection::Version() const
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::Version");
    return(TVersion( KWsfServMajorVersionNumber, KWsfServMinorVersionNumber, KWsfServBuildVersionNumber) );
    }

void RSenServiceConnection::Initialize( TRequestStatus& aStatus,
                                        TDes8& aErrPtr,
                                        TDes& aSessionID,
                                        CSenChunk& aSenChunk,
                                        TInt aSdLength,
                                        TInt /* aCpLength */,
                                        TBool aHasAuthenticationCallback)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::Initialize");
    TIpcArgs args;
    args.Set(0, &aErrPtr);
    args.Set(1, &aSessionID);
    // Note: chunk will carry XML SD - and possibly consumer policy -
    // over the process boundary.
    aSenChunk.ChunkToArgs(args, 2);
    
    iFourInts().iInt1 = aSdLength;
    iFourInts().iInt2 = aHasAuthenticationCallback;
    args.Set(3,&iFourInts);
    
    SendReceive(ESenServInitialize, args, aStatus);
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::Initialize Completed");
    }


TInt RSenServiceConnection::IsReady(TBool& aReady)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::IsReady");
    TPtr8 descriptor(reinterpret_cast<TUint8*>(&aReady), sizeof(aReady), sizeof(aReady));
    TIpcArgs args;
    args.Set(0, &descriptor);
    return SendReceive(ESenServIsReady, args);
    }

TInt RSenServiceConnection::HasFacet(TDesC8& aUri, TBool& aHasFacet)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::HasFacet");
    TLSLOG_ALL(iTLSLogChannel, KMaxLogLevel,(aUri));
    TPtr8 descriptor(reinterpret_cast<TUint8*>(&aHasFacet), sizeof(aHasFacet), sizeof(aHasFacet));
    TInt length = aUri.Length();
    TIpcArgs args;
    args.Set(0, &aUri);
    args.Set(1, &descriptor);
    args.Set(2, length);
    return SendReceive(ESenServHasFacet, args);
    }

TInt RSenServiceConnection::CompleteServerMessagesOnOff(TBool& aOnOff)
    {
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceConnection::CompleteServerMessagesOnOff")));

    // Note : Using TPtr8 since this is binary information
    TPtr8 descriptor(reinterpret_cast<TUint8*>(&aOnOff),sizeof(aOnOff),
        sizeof(aOnOff));

    TIpcArgs args;
    args.Set(0, &descriptor);

    return SendReceive(ESenServServerMessagesOnOff, args);
    }

void RSenServiceConnection::Transaction( TRequestStatus& aStatus,
                                         TDes8& aErrPtr,
                                         TDes8& aTxnPtr,
                                         CSenChunk& aClientOp )
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::Transaction");

    // Set the message context ID in here: it is only locally unique, inside each service connection - "in client scope"
    // Context IDs use negative range, to avoid collisions with txnIDs (which are always positive integers)
    aClientOp.ChunkHeader().SetContextId( --iMessageContextId );
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Transaction - SetContextId: %d"), iMessageContextId));
    
    TIpcArgs args;
    args.Set(0, &aErrPtr);
    args.Set(1, &aTxnPtr);
    // This RChunk will pass message to server and include response when this
    // txn is complete
    aClientOp.ChunkToArgs(args,2);
    SendReceive(ESenServTransaction, args, aStatus);
    }

TInt RSenServiceConnection::SendMsgAndReceiveTxnId(CSenChunk& aClientOp)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::SendMsgAndReceiveTxnId");
    TInt retVal(KErrNone);

    TIpcArgs args;
    aClientOp.ChunkToArgs(args,0);
    args.Set(1,aClientOp.ChunkHeader().ContextId());
    retVal = SendReceive(ESenServSendMsgAndGetTxnId, args);

    return retVal;
    }
TInt RSenServiceConnection::MoveFile(CSenChunk& aClientOp,
                                     const TDesC8& aCid,
                                     RFile& aFile)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::SendFile");
    
    TInt retVal = aClientOp.DescToChunk(aCid,2);
    
    TIpcArgs args;

    if ( retVal == KErrNone)
        {
        aClientOp.ChunkToArgs(args,0);
        aFile.TransferToServer(args, 1, 2);
        
        retVal = SendReceive(ESenServMoveFile, args);
        }

    return retVal;
    }
    
TInt RSenServiceConnection::SendFileHandle(CSenChunk& aSenChunk, 
                      						RFile& aFile)
	{
  	 TInt retVal(KErrNone);
     TIpcArgs args;

     if ( retVal == KErrNone)
        {
         aSenChunk.ChunkToArgs(args,0);
         aFile.TransferToServer(args, 1, 2);       
         retVal = SendReceive(ESenServSendFileHandle, args);
        }

     return retVal;
	}

TInt RSenServiceConnection::MoveChunk(CSenChunk& aClientOp,
                                      const TDesC8& aCid,
                                      RChunk& aChunk,
                                      TInt aDataOffset,
                                      TInt aDataSize)
    {
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceConnection::SendChunk")));
    
    TInt retVal = aClientOp.DescToChunk(aCid,2);
    
    TIpcArgs args;
    
    if ( retVal == KErrNone )
        {
        TIpcArgs args;
        aClientOp.ChunkToArgs(args,0);
        args.Set(1, aChunk);
        args.Set(2, aDataOffset);
        args.Set(3, aDataSize);
        retVal = SendReceive(ESenServMoveChunk, args);
        }

    return retVal;
    }

TInt RSenServiceConnection::RequestServiceDescription(TDes8& aSdLength)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RequestServiceDescription");
    TIpcArgs args;
    args.Set(0, &aSdLength);
    return SendReceive(ESenServRequestServiceDescription, args);
    }


TInt RSenServiceConnection::ReceiveServiceDescription(TDes8& aServiceDescription)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::ReceiveServiceDescription");
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Waiting for SD: maxlength(%d):"),
                                aServiceDescription.MaxLength()));
    TIpcArgs args;
    args.Set(0, &aServiceDescription);
    return SendReceive(ESenServReceiveServiceDescription, args);
    }

TInt RSenServiceConnection::StartTransaction()
    {
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceConnection::StartTransaction")));
    return SendReceive(ESenServStartTransaction);
    }

TInt RSenServiceConnection::TransactionCompleted()
    {
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceConnection::TransactionCompleted")));
    return SendReceive(ESenServTransactionCompleted);
    }

TInt RSenServiceConnection::SetTransportPropertiesL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::SetTransportPropertiesL");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);

    TInt retVal = SendReceive(ESenServSetTransportProperties, args);
    
    return retVal;
    }

TInt RSenServiceConnection::TransportPropertiesL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::TransportPropertiesL");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);

    TInt retVal = SendReceive(ESenServTransportProperties, args);
    
    return retVal;
    }

void  RSenServiceConnection::CancelSession(TRequestStatus& aStatus)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::CancelSession(sync)");
    SendReceive(ESenServCancelSession, aStatus);
    }

// ASYNC VERSION
void RSenServiceConnection::CancelRequests(TRequestStatus& aStatus)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::CancelSession(async)");
    TIpcArgs args; // empty
    SendReceive(ESenServCancelSession, args, aStatus);
    }

void RSenServiceConnection::CancelRequest(TInt aTransactionID)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::CancelRequest");
    TIpcArgs args;
    args.Set(0, aTransactionID);
    SendReceive(ESenServCancelRequest, args);
    }

void RSenServiceConnection::DataTrafficDetails(TSenDataTrafficDetails& aDetails,
											   TSenDataTrafficOperations& aOperations) 
	{
	TLSLOG_L(iTLSLogChannel, KSenServiceConnectionLogLevel,"RSenServiceConnection::DataTrafficDetails");
    TPtr8 trafficDetailsDesc(reinterpret_cast<TUint8*>(&aDetails),sizeof(aDetails),
        					sizeof(aDetails));
    TIpcArgs args;
    args.Set(0,&trafficDetailsDesc);
    args.Set(1,&aOperations.iReset);
    SendReceive(ESenTrafficDetails, args);
    }

TInt RSenServiceConnection::ConnectionID()
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::ConnectionID");
    TInt connectionID = SendReceive( ESenServConnectionID );
    //LOG_WRITEFORMAT((_L("- Connection ID: (%d)"), connectionID));
    return connectionID;
    }

TInt RSenServiceConnection::IdentityProviderL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::IdentityProviderL");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);

    TInt retVal = SendReceive(ESenServConnectionIdentityProvider, args);
    
    return retVal;
    }
    
TInt RSenServiceConnection::RegisterIdentityProvider(TDesC8& aMessage)
    {
#ifdef _SENDEBUG
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterIdentityProvider");

    TPtrC8 outBufferLeft = aMessage.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"), &outBufferLeft,
        aMessage.Length()));
#endif // _SENDEBUG

    TInt length = aMessage.Length();

    TIpcArgs args;
    args.Set(0, &aMessage);
    args.Set(1, length);

    return SendReceive(ESenServRegisterIdentityProvider, args);
    }
	
TInt RSenServiceConnection::RegisterTransferObserver(
        const TDesC* aServerName, TAny* aConnection)
    {
    TInt retVal(KErrNone);
    CActiveSchedulerWait asWait;
    TIpcArgs args(aServerName, aConnection, &asWait);
    CSenConnAgentSync* caSync = new CSenConnAgentSync();
    if(caSync)
    	{
	    SendReceive(ESenObserveTransfer, args, caSync->iStatus);
	    caSync->Start();
	    Mem::FillZ(&asWait, sizeof(asWait));
	    asWait.Start();
	    retVal = caSync->iStatus.Int();
	    delete caSync;
    	}
    else
    	{
    	retVal = KErrNoMemory;
    	}
    return retVal;
    }

TInt RSenServiceConnection::SendFileProgress( TInt aTxnId, 
                                              TBool aIncoming,
                                              TBool aIsSoap, 
                                              const TDesC8& aSoapOrCid, 
                                              TInt aProgress)
    {
    CSenProgressResourceHandler* resHandler = new CSenProgressResourceHandler(
            aTxnId, aIncoming, aIsSoap, aProgress, this);
    if (!resHandler)
        {
        return KErrNoMemory; // new (ELeave) was not used!
        }
    TInt retVal = resHandler->SetBuffer(aSoapOrCid);
    if ( retVal != KErrNone)
        {
        delete resHandler;
        }
    else
        {
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
        SendReceive(ESenTransferProgress, args, resHandler->iStatus);
        }
    return retVal;
    }

void RSenServiceConnection::Remove(
        CSenProgressResourceHandler* aResourceHandler)
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


void RSenServiceConnection::Close()
    {
    CSenProgressResourceHandler* resHandler = iFirst;
    while (resHandler)
        {
        resHandler->Cancel();
        CSenProgressResourceHandler* tmp = resHandler;
        resHandler = resHandler->iNext;
        delete tmp;
        }
    RSessionBase::Close();
    }

TInt RSenServiceConnection::SearchIdentityProviderL(CSenChunk& aSenChunk, const TDesC8& aProviderId)
	{
	TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::SearchIdentityProviderL");
	
	TInt length = aProviderId.Length();
	
	TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);
    args.Set(1, &aProviderId);
    args.Set(2, length);
  
    TInt retVal = SendReceive(ESenServSearchIdentityProvider, args);
    return retVal;
	}

TInt RSenServiceConnection::RegisterAuthenticationObserver(const TDesC* aServerName, TAny* aConnection)
	{
	TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterAuthenticationObserver");
	CActiveSchedulerWait asWait;
    TIpcArgs args(aServerName, aConnection, &asWait);
    TRequestStatus rs = KRequestPending;
    SendReceive(ESenObserveAuthCallback, args, rs);
    Mem::FillZ(&asWait, sizeof(asWait));
    asWait.Start();
    User::WaitForRequest(rs);
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterAuthenticationObserver Completed");
    return rs.Int();
	}

TInt RSenServiceConnection::RegisterCoBrandingObserver(
        const TDesC* aServerName, TAny* aConnection)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterCoBrandingObserver");
	CActiveSchedulerWait asWait;
    TIpcArgs args(aServerName, aConnection, &asWait);
    TRequestStatus rs = KRequestPending;
    SendReceive(ESenObserveCoBranding, args, rs);
    Mem::FillZ(&asWait, sizeof(asWait));
    asWait.Start();
    User::WaitForRequest(rs);
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterCoBrandingObserver Completed");
    return rs.Int();
    }
#ifdef __ENABLE_ALR__
TInt RSenServiceConnection::RegisterMobilityObserverL(
        const TDesC* aServerName, TAny* aConnection)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterMobilityObserverL");
    TInt retVal(KErrNone);
    CActiveSchedulerWait asWait;
    TIpcArgs args(aServerName, aConnection, &asWait);
    CSenConnAgentSync* caSync = new CSenConnAgentSync();
    if(caSync)
    	{
	    SendReceive(ESenServRegisterMobiltyObserver, args, caSync->iStatus);
	    caSync->Start();
	    Mem::FillZ(&asWait, sizeof(asWait));
	    asWait.Start();
	    retVal = caSync->iStatus.Int();
	    delete caSync;
    	}
    else
    	{
    	retVal = KErrNoMemory;
    	}
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::RegisterMobilityObserverL Completed");
    return retVal;
    }


TInt RSenServiceConnection::MigrateToPrefferedCarrierL(TBool &aUserChoice)
	{
	TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::MigrateToPrefferedCarrierL");
	
	 // Note : Using TPtr8 since this is binary information
	TPtr8 descriptor(reinterpret_cast<TUint8*>(&aUserChoice),sizeof(aUserChoice),
        sizeof(aUserChoice));

    TInt retVal(KErrNone);
    TIpcArgs args(&descriptor) ; 
    
    CSenConnAgentSync* caSync = new CSenConnAgentSync();
    if(caSync)
    	{
        caSync->Start();    	
	    SendReceive(ESenServMigrateToPreffredCarrier, args, caSync->iStatus);
	    retVal = caSync->iStatus.Int();
	    delete caSync;
    	}
    else
    	{
    	retVal = KErrNoMemory;
    	}
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::MigrateToPrefferedCarrierL Completed");
    return retVal;
	    
	}	

TInt RSenServiceConnection::NewCarrierAcceptedL(TBool &aUserChoice)
	{
	TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::MigrateToPrefferedCarrierL");
	
	 // Note : Using TPtr8 since this is binary information
	 TPtr8 descriptor(reinterpret_cast<TUint8*>(&aUserChoice),sizeof(aUserChoice),
        sizeof(aUserChoice));

    TInt retVal(KErrNone);
    TIpcArgs args(&descriptor) ;
    CSenConnAgentSync* caSync = new CSenConnAgentSync();
    if(caSync)
    	{
        caSync->Start();    	
	    SendReceive(ESenServNewCarrierAccepted, args, caSync->iStatus);
	    retVal = caSync->iStatus.Int();
	    delete caSync;
    	}
    else
    	{
    	retVal = KErrNoMemory;
    	}
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceConnection::MigrateToPrefferedCarrierL Completed");
    return retVal;	    
	}	
#endif

RFileLogger* RSenServiceConnection::Log() const
    {
    return iLog;
    }

static TInt StartServerL()
    {
    TInt result(KErrNotFound);

#ifdef _SENDEBUG   
    RFileLogger log;
    CleanupClosePushL(log);
    User::LeaveIfError( log.Connect() );
    log.CreateLog( KSenServiceConnectionLogDir, _L("SenStartServer.log"), EFileLoggingModeAppend );
    log.Write(_L("- StartServerL() - invoked by SC."));
#endif // _SENDEBUG

    TFindServer findSenServiceManager(KSenServiceManager);
    TFullName name;

    result = findSenServiceManager.Next(name);
    if (result == KErrNone)
        {
        // Server already running
#ifdef _SENDEBUG        
        log.Write(_L8("- Server already running, KErrNone"));
        log.CloseLog();
        CleanupStack::PopAndDestroy(); // log.Close()
#endif // _SENDEBUG        
        return KErrNone;
        }

    RSemaphore semaphore;
    CleanupClosePushL(semaphore);

    // First, try to open the semaphore (if someone else created it already):
    result = semaphore.OpenGlobal( KSenServiceManagerSemaphoreName );
    if( result == KErrNone )
        {
        // If another client is starting up SEN.EXE, this client should return
        // to re-connect loop (wait one sec, max 5 times)
#ifdef _SENDEBUG        
        log.Write(_L8("- OpenGlobal OK => Another client already starting up SEN.EXE. About to WAIT and RE-CONNECT."));
#endif        
        result = KErrServerBusy;
        }
    else // could not find any already opened global semaphore ==> it is safe to create new one
        {   
        // Semaphore has not yet been created, so instantiate it now(!)
        result = semaphore.CreateGlobal( KSenServiceManagerSemaphoreName, 0 );
#ifdef _SENDEBUG        
        if( result != KErrNone )
            {
            log.Write(_L8("CreateGlobal failed"));
            log.WriteFormat(_L8("- error: %d"), result);
            }
#endif        
        }
        
    if (result != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // semaphore.Close()
#ifdef _SENDEBUG        
        log.CloseLog();
#endif // _SENDEBUG        
        if( result != KErrServerBusy )
            {
#ifdef _SENDEBUG        
            log.Write(_L8("- CreateGlobal failed => Another client already starting up SEN.EXE. About to WAIT and RE-CONNECT."));
            CleanupStack::PopAndDestroy(); // log.Close()
#endif // _SENDEBUG        
            result = KErrServerBusy; // again, this error will activate re-connect loop (max 5 times)
            }
        return result;
        }

    result = CreateServerProcessL();
    if (result != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // semaphore.Close()
#ifdef _SENDEBUG        
        log.Write(_L8("CreateServerProcessL failed"));
        log.CloseLog();
        CleanupStack::PopAndDestroy(); // log.Close()
#endif // _SENDEBUG         
        return result;
        }

    // Now start waiting for signal from server: it will release this semaphore from wait:
    semaphore.Wait();  
    CleanupStack::PopAndDestroy(); // semaphore.Close()
    
#ifdef _SENDEBUG        
    log.Write(_L("Log file closed."));
    log.CloseLog();
    CleanupStack::PopAndDestroy(); // log.Close()    
#endif // _SENDEBUG
    return  KErrNone;
    }

static TInt CreateServerProcessL()
    {
    TInt result(KErrNone);

#ifdef _SENDEBUG
    RFileLogger log;
    CleanupClosePushL(log);
    User::LeaveIfError( log.Connect() );
    // Note: appends to the log created in StartServer(), as it already exists
    log.CreateLog( KSenServiceConnectionLogDir, _L("SenStartServer.log"), EFileLoggingModeAppend );
    log.Write(_L("- CreateServerProcessL() - invoked by SC."));
#endif //_SENDEBUG

    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

#if defined( __WINS__ ) && !defined(EKA2)
    // WINDOWS emulator version
    RLibrary lib;
    CleanupClosePushL(lib);

    RThread server;
    CleanupClosePushL(server);

    result = lib.Load( KSenServiceManagerFileName, serverUid );

    if(result==KErrNone)
        {
#ifdef _SENDEBUG
        log.Write(_L8("Library successfully loaded, KErrNone"));
#endif // _SENDEBUG
        
        //  Get the WinsMain function
        TLibraryFunction functionWinsMain = lib.Lookup(1);

        //  Call it and cast the result to a thread function
        TThreadFunction serverThreadFunction =
            reinterpret_cast<TThreadFunction>(functionWinsMain());

        TName threadName(KSenServiceManagerFileName);

        // Append a random number to make it unique
        threadName.AppendNum(Math::Random(), EHex);

        result = server.Create(threadName,   // create new server thread
                               serverThreadFunction, // thread's main function
                               KDefaultStackSize,
                               NULL,
                               &lib,
                               NULL,
                               KServerMinHeapSize,
                               KServerMaxHeapSize,
                               EOwnerProcess);

        // if successful, server thread now has the handle to library:

        if(result== KErrNone)
            {
            server.SetPriority(EPriorityMore);
            }
        }
#ifdef _SENDEBUG
    else
        {
        log.Write(_L8("- Could not load library"));
        }
#endif // _SENDEBUG

    CleanupStack::Pop(); // server (thread)
    CleanupStack::PopAndDestroy(); // lib
    CleanupClosePushL(server);

#else // _WINS_ not defined (EKA2, for example)
    RProcess server;
    CleanupClosePushL(server);
    result = server.Create( KSenServiceManagerFileName, _L(""), serverUid );
#endif // _WINS_

    if( result == KErrNone )
        {
#ifdef _SENDEBUG
        log.Write( _L8("- Server successfully created (KErrNone)") );
#endif // _SENDEBUG
        server.Resume();
        }
#ifdef _SENDEBUG
    else
        {
        log.Write( _L8("- Could not create server") );
        }
#endif // _SENDEBUG

    CleanupStack::PopAndDestroy(); // server.Close();

#ifdef _SENDEBUG
    CleanupStack::PopAndDestroy(); // log.Close();
#endif // _SENDEBUG

    return result;
    }
    

// End of file



