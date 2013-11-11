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



#include <s32strm.h>
#include <f32file.h>
#include <e32property.h>

#include "senserviceconnectionimpl.h"

#include "sendebug.h"                 // internal Utils\inc - logging MACROs
#include "senlogger.h"
#include "senservicemanagerdefines.h" // internal Core\inc  - IPC enumerations, plus 
                                      // to include KServerUid3 for SEN.EXE SecureID 
                                      // (UID3) that can be nicely used as Property 
                                      // Category UID

#include <SenServicePattern.h>
#include <SenXmlServiceDescription.h>
#include <SenXmlUtils.h>
#include "SenHttpTransportProperties.h"
#include <SenIdentityProvider.h>

#include "senguidgen.h"  // internal: Utils\inc - the prefix length constant for WSF GUIDs
#include "senchunk.h"
#include "senmessagebase.h"

#include "SenSoapEnvelope2.h"
#include "SenParser.h"

#include "senservicemanagerdefines.h"
#include "SenBaseAttribute.h"
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengserializer.h>
#include "senconnagentserver.h"
#include "senxmldebug.h"
#include "senatomentry.h"


namespace
    {
    const TInt KMaxChunkNumDecimals = 64;
    _LIT8(KIdentityProviderName, "IdentityProvider");
    _LIT8( KSenCidPostfix, "@example.org" );
    const TInt KMaxCidLength = 512;
    }

#ifdef RD_SEN_ENABLE_USE_OF_WSD_IN_SC_FOR_HW_TARGET
// in hardware, we can use static variable
TInt CSenConnectionChunk::iServOpenChunkCount;   
#endif    
CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewL(MSenServiceConsumer& aObserver,
                                                           const TDesC8& aContract)
    {
    CSenServiceConnectionImpl* pNew = NewLC(aObserver, aContract);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewLC(MSenServiceConsumer& aObserver,
                                                            const TDesC8& aContract)
    {

    CSenServiceConnectionImpl* pNew = new (ELeave) CSenServiceConnectionImpl(aObserver, NULL);

    CleanupStack::PushL(pNew);


    CSenXmlServiceDescription* pSD =
            CSenXmlServiceDescription::NewLC(KNullDesC8, aContract);
    pNew->ConstructL(*pSD);
    CleanupStack::PopAndDestroy(); // pServicePattern
    return pNew;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewL(MSenServiceConsumer& aObserver,
                                                           MSenServiceDescription& aPattern)
    {
    CSenServiceConnectionImpl* pNew = NewLC(aObserver, aPattern);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewLC(MSenServiceConsumer& aObserver,
                                                            MSenServiceDescription& aPattern)
    {
    CSenServiceConnectionImpl* pNew =
                            new (ELeave) CSenServiceConnectionImpl(aObserver, NULL);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aPattern);
    return pNew;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewL(MSenServiceConsumer& aObserver,
                                                           CSenServicePattern& aServicePattern)
    {
    CSenServiceConnectionImpl* pNew = NewLC(aObserver, aServicePattern);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewLC(MSenServiceConsumer& aObserver,
                                                            CSenServicePattern& aServicePattern)
    {
    CSenServiceConnectionImpl* pNew = new (ELeave) CSenServiceConnectionImpl(aObserver, NULL);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aServicePattern);
    return pNew;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewL(MSenServiceConsumer& aConsumer,
                                                           const TDesC8& aContract,
                                                           MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    CSenServiceConnectionImpl* pNew = NewLC(aConsumer, aContract, aExtendedConsumer);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewLC(MSenServiceConsumer& aConsumer,
                                                            const TDesC8& aContract,
                                                            MSenExtendedConsumerInterface& aExtendedConsumer)
    {

    CSenServiceConnectionImpl* pNew = new (ELeave) CSenServiceConnectionImpl(aConsumer, &aExtendedConsumer);

    CleanupStack::PushL(pNew);


    CSenXmlServiceDescription* pSD =
            CSenXmlServiceDescription::NewLC(KNullDesC8, aContract);
    pNew->ConstructL(*pSD);
    CleanupStack::PopAndDestroy(); // pServicePattern
    return pNew;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewL(MSenServiceConsumer& aConsumer,
                                                                MSenServiceDescription& aServiceDescription,
                                                           MSenExtendedConsumerInterface& aExtendedConsumer)
    {

    CSenServiceConnectionImpl* pNew = NewLC(aConsumer, aServiceDescription, aExtendedConsumer);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewLC(MSenServiceConsumer& aConsumer,
                                                                MSenServiceDescription& aServiceDescription,
                                                                MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    CSenServiceConnectionImpl* pNew =
                            new (ELeave) CSenServiceConnectionImpl(aConsumer,&aExtendedConsumer);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aServiceDescription);
    return pNew;        
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewL(MSenServiceConsumer& aConsumer,
                                                           CSenServicePattern& aServicePattern,
                                                           MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    CSenServiceConnectionImpl* pNew = NewLC(aConsumer, aServicePattern, aExtendedConsumer);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceConnectionImpl* CSenServiceConnectionImpl::NewLC(MSenServiceConsumer& aConsumer,
                                                            CSenServicePattern& aServicePattern,
                                                            MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    CSenServiceConnectionImpl* pNew = new (ELeave) CSenServiceConnectionImpl(aConsumer,&aExtendedConsumer);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aServicePattern);
    return pNew;
    }

void CSenServiceConnectionImpl::ConstructL(MSenServiceDescription& aSD)
    {
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::ConstructL(aSD)");
    CSenElement& sdAsElement = ((CSenXmlServiceDescription&)aSD).AsElement();

    // Check if this is an internal service connection:
    const TDesC8* pInternal = sdAsElement.AttrValue( KSenAttrInternalSC );
    if( pInternal )
        {
        iIsInternalSC = ETrue;
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"This is 'internal SC' => Will NOT create CSenConnectionStatusObserver.");
        delete SenXmlUtils::RemoveAttributeL(sdAsElement, KSenAttrInternalSC);
        iDispatcherEnabled = EFalse;
        }
    else // not internal, but starndard SC
        {
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"This is normal SC => Creating CSenConnectionStatusObserver.");
#ifdef RD_SEN_SC_MSG_DISPATCHER_ENABLED
        // Dispatcher is enabled ONLY IF the feature is NOT DISABLED AT COMPILER LEVEL:
        iDispatcherEnabled = ETrue; // ==> Default to using the dispatcher with any "normal" SC
#endif // RD_SEN_SC_MSG_DISPATCHER_ENABLED
        }

    // Check if dispatcher is EXPLICITELY DISABLED (no matter if this is internal or not)
    const TDesC8* pDispatcherDisabled = sdAsElement.AttrValue( KSenAttrDisableDispatcherSC );
    if( pDispatcherDisabled )
        {
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"Dispatcher is EXPLICITELY DISABLED for this SC.");
        delete SenXmlUtils::RemoveAttributeL(sdAsElement, KSenAttrDisableDispatcherSC );
        // Dispatcher is NOW EXPLICITELY DISABLED
        iDispatcherEnabled = EFalse; 
        }

    ipInitPatternAsXml = aSD.AsXmlL();
    if( aSD.DescriptionClassType() == MSenServiceDescription::EServicePattern )
        {
        CSenServicePattern* pServicePattern = (CSenServicePattern*)&aSD;
        ipInitConsumerPolicyAsXml = pServicePattern->ConsumerPolicyAsXmlL();
        }
        
        
    ipRegistrationTimer = CSenRegistrationTimer::NewL( *this );        
/*    
#ifdef _SENDEBUG
    TPtrC8 frameworkID = aSD.FrameworkId();
    TPtrC8 endpoint    = aSD.Endpoint();
    TPtrC8 contract    = aSD.Contract();
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|");
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel , _L8("- FrameworkID: %S"), &frameworkID ));
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel , _L8("- Endpoint:    %S"), &endpoint ));
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel , _L8("- Contract:    %S"), &contract ));
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|");
#endif //_SENDEBUG           
*/
#ifndef RD_SEN_SC_PROXY_ENABLED_BY_DEFAULT
    ConnectL(); 
// #else // ConnectL occurs on  "proxy" / factory  -level
#endif
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::ConstructL(aSD) Completed");
    }


void CSenServiceConnectionImpl::ConnectL()
    {
    ipSessionId = HBufC::NewL(KSenUuidMaxLength); 
    ipChunkName = HBufC::NewL(KSenUuidMaxLength+KMaxChunkNumDecimals);

    iSessionIdBuffer.Set(ipSessionId->Des());

    TLSLOG_OPEN(KSenServiceConnectionLogChannel, KSenServiceConnectionLogLevel, KSenServiceConnectionLogDir, KSenServiceConnectionLogFile);
    TInt connErr = iConnection.Connect();
    TInt connAttemp(0); // max 5 attempts are allowed (KMaxConnectionOpeningAttempts)
    while ( (connErr == KErrServerTerminated
                     ||
             connErr == KErrServerBusy)
                     &&
             connAttemp < KSenMaxClientConnectionOpeningAttempts )
        {
//        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel," - Server terminated/busy/max_conns => waiting.");
        User::After(1000000); // wait for a second if server has been shut down  
//        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel," - Wait is over.");
        connErr = iConnection.Connect();
        connAttemp++;
        }
    if ( connErr != KErrNone )
        {
        User::Leave( connErr );
        }
    
    if( iDispatcherEnabled ) // DISPATCHER IS ENABLED
        {
        TInt iShareAuto = iConnection.ShareAuto();
        if(iShareAuto != KErrNone )
            {
            User::Leave( iShareAuto );
            }
        }
        
    iConnectionID = iConnection.ConnectionID();
    iConnection.SetChannel(KSenServiceConnectionLogChannelBase+iConnectionID);   
    iTlsLogStatusChannel = KSenServiceConnectionStatusLogChannelBase + iConnectionID;

#ifdef _SENDEBUG   
    RProcess process; 
    RThread thread;
    TFileName logFile;
    logFile.Append( KSenServiceConnectionLogFile().Left(KSenServiceConnectionLogFile().Length()-4) ); // exclude ".log" file extension
    logFile.AppendNum( iConnectionID );
    logFile.Append( KSenUnderline );
    logFile.Append( process.Name().Left(32));
    logFile.Append( KSenUnderline );
    logFile.Append( thread.Name().Left(20));
    logFile.Append( KSenServiceConnectionLogFile().Right(4) ); // postfix with ".log" file extension
    
    
//    KSenServiceConnectionChannelBase
    TLSLOG_OPEN(KSenServiceConnectionLogChannelBase+iConnectionID, KSenServiceConnectionLogLevel, KSenServiceConnectionLogDir, logFile);
    // Open connection to the file logger server
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,"CSenServiceConnection::ConstructL - Log file opened");
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- About to initialize service connection");
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Connection ID: (%d)"), iConnectionID));

    logFile.Zero();
    logFile.Append( KSenServiceConnectionStatusLogFile().Left(KSenServiceConnectionStatusLogFile().Length()-4) ); // exclude ".log" file extension
    logFile.AppendNum( iConnectionID );
    logFile.Append( KSenUnderline );
    logFile.Append( process.Name().Left(32));
    logFile.Append( KSenUnderline );
    logFile.Append( thread.Name().Left(20));
    logFile.Append( KSenServiceConnectionStatusLogFile().Right(4) ); // postfix with ".log" file extension
    TLSLOG_OPEN( iTlsLogStatusChannel, KSenServiceConnectionStatusLogLevel, KSenServiceConnectionLogDir, logFile );


    TLSLOG_OPEN(KSenMessagesLogChannel, KSenMessagesLogLevel, KSenMessages, KSenMessagesLog);
#endif // _SENDEBUG    

    iFragment = CSenBaseFragment::NewL(KServiceConnectionElementLocalName);
     
    InitializeL( /*aSD*/ );

    if( iDispatcherEnabled ) // DISPATCHER IS ENABLED
        {
        RThread aThread;
        iOwnerThreadId = aThread.Id();
        ipSenServiceDispatcher = CSenServiceDispatcher::NewL(iConnection,iConnectionID);
        ipSenServiceDispatcher->SetOwnerThreadId(iOwnerThreadId);
        TInt dispatcherThreadID = ipSenServiceDispatcher->GetDispactherThreadId();
        //    ipSenUnderTakerWaiter = CSenUnderTakerWaiter::NewL(this,dispatcherThreadID);
        //    ipSenUnderTakerWaiter->StartWaiter();
        }
    }

void CSenServiceConnectionImpl::SetConsumer( MSenServiceConsumer& aConsumer, MSenExtendedConsumerInterface* apExtConsumer )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SetConsumer");
    iObserver = &aConsumer;
    ipExtendedConsumerInterface = apExtConsumer;
    }
   
void CSenServiceConnectionImpl::Consumer( MSenServiceConsumer*& aConsumer, MSenExtendedConsumerInterface*& apExtConsumer )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Consumer");
    aConsumer = iObserver;
    apExtConsumer = ipExtendedConsumerInterface;
    }   

TPtrC8 CSenServiceConnectionImpl::InitDescription()
    {
    return *ipInitPatternAsXml;
    }
CSenServiceConnectionImpl::CSenServiceConnectionImpl(MSenServiceConsumer& aObserver,
                                                     MSenExtendedConsumerInterface* aExtendedConsumerInteface)
 :  iConnection(&iLog),
    iObserver(&aObserver),
    iInBuf(NULL),
    iInBuffer(NULL,0),
    iOutBuf(NULL),
    iOutBuffer(NULL,0),
    iInitialized(EFalse),
    iFragment(NULL),
    iResponseLength(0),
    iErrorNumber(0),
    iErrorNumberBuffer(NULL, 0),
    iTxnId(0),
    iTxnIdBuffer(NULL, 0),
    iChunkMap(NULL),
    ipSessionId(NULL),
    iSessionIdBuffer(NULL, 0),
    iChunkNameNumber(0),
    ipChunkName(NULL),
    iResponse(NULL),
    ipInitChunk(NULL),
    ipCompleteServerMsgsOnOff(NULL),
    iConnectionID( KErrNotReady ),
    //iTlsLogChannel( KSenServiceConnectionLogChannelBase )
    iTlsLogStatusChannel( KSenServiceConnectionStatusLogChannelBase ),
    iConnectionAgentCallbackInitialized(EFalse),
    iRegisterFileObserverDone(EFalse),
    iRegisterMobilityObserverDone(EFalse),
    iRegisterAuthObserverDone(EFalse),
    iRegisterCoBrandingObserverDone(EFalse),
    iReauthenticationNeeded(EFalse),
    ipExtendedConsumerInterface(aExtendedConsumerInteface),
#ifdef __ENABLE_ALR__
    iMobiltyObserver(NULL),
#endif
    iCancelSession(EFalse),
    iIsInternalSC(EFalse),
    iDispatcherEnabled(EFalse),
    ipRegistrationTimer(NULL)
    {
    CActiveScheduler::Add(this);
    }

CSenServiceConnectionImpl::~CSenServiceConnectionImpl()
    {
    delete ipRegistrationTimer;
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::~CSenServiceConnectionImpl")));
    CSenServiceConnectionImpl::Cancel(); // Causes call to DoCancel()

#if !defined (RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS) && !defined( RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS ) // pub&sub is not in use for outgoing file progress
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::~CSenServiceConnectionImpl(): RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == FALSE")));
    if (ipAuthProvider || iFilesObserver || ipCoBrandingObserver)
#else // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::~CSenServiceConnectionImpl(): RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE")));
    if (ipAuthProvider || ipCoBrandingObserver) // iFilesObserver is not used
#endif // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS end
        {
        CSenConnAgentServer::Close();
        iConnectionAgentCallbackInitialized = EFalse;
        }

#ifdef __ENABLE_ALR__        
    if (iMobiltyObserver)
        {
        CSenConnAgentServer::Close();
        iRegisterMobilityObserverDone = EFalse;
        }
#endif //__ENABLE_ALR__

//    delete ipSenUnderTakerWaiter;
    delete ipSenServiceDispatcher;

    if ( iAsyncOpsArray )
        {
        TInt count(iAsyncOpsArray->Count());
        for ( TInt i = 0; i < count; i++ )
            {
            CSenAsyncOperation* pOp = (*iAsyncOpsArray)[i];
            if ( pOp )
                {
                pOp->iActive = NULL;
                pOp->Deque();
                delete pOp;
                }
            }
        iAsyncOpsArray->Close();
        delete iAsyncOpsArray;
        }

    if(iChunkMap)
        {
        iChunkMap->Reset();
        delete iChunkMap;
        iChunkMap = NULL;
        }

    delete iInBuf;
    delete iOutBuf;
    delete iFragment;
    delete ipSessionId;
    delete ipChunkName;
    delete ipInitPatternAsXml;
    delete ipInitConsumerPolicyAsXml;
    delete ipInitChunk;
    delete ipCompleteServerMsgsOnOff;
    
    // Close the log file and the connection to the server.
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("Log file closed.")));
    TLSLOG_CLOSE (KSenServiceConnectionLogChannelBase+iConnectionID);
    TLSLOG_CLOSE(KSenServiceConnectionLogChannel);
    TLSLOG_CLOSE( iTlsLogStatusChannel );
    TLSLOG_CLOSE( KSenMessagesLogChannel );

    iConnection.Close();
    delete ipConnectionStatusObserver;
    delete ipFileProgressObserver;
    }


TPtrC CSenServiceConnectionImpl::SessionID()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SessionID()")));
    if(ipSessionId)
        {
        return *ipSessionId;
        }
    else
        {
        return KNullDesC();
        }
    }


TInt CSenServiceConnectionImpl::RegisterAndSubscribeFileProgressObserversL()
    {
    TInt retCode( KErrNone );
    if(iFilesObserver && !iRegisterFileObserverDone)
    	{
    	
#ifndef RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS // pub&sub not in use for outgoing file progress
        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == FALSE")));
    	retCode = RegisterFilesObserver();
#else // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE
        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE")));
    	ipFileProgressObserver = CSenFileProgressObserver::NewL( *iFilesObserver, iConnectionID, iDispatcherEnabled, ipSenServiceDispatcher, ChunkMapL() );

    #ifdef RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel, "CSenServiceConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == TRUE");
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel, "=> calling RegisterFilesObserver()");
        retCode = RegisterFilesObserver(); // MIXED MODE, using conn agent for SOAP progress..
    #else // RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == FALSE
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel, "CSenServiceConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == FALSE");
    #endif // RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS

#endif // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS end 	
    	iRegisterFileObserverDone = ETrue; // register only once
    	}
    return retCode;
    }
    
TInt CSenServiceConnectionImpl::SubmitL(CSenConnectionChunk& aClientOp)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SubmitL(CSenChunk)")));
    TInt retVal(KErrNone);
    
    iErrorNumber = 0;
    iErrorNumberBuffer.Set(reinterpret_cast<TUint8*>(&iErrorNumber),
                           sizeof(TInt),
                           sizeof(TInt));

    iTxnId = 0;
    iTxnIdBuffer.Set(reinterpret_cast<TUint8*>(&iTxnId),
                     sizeof(TInt),
                     sizeof(TInt));

    TRequestStatus status;

    iAsyncOpsCount++;
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("CSenServiceConnectionImpl::SubmitL(CSenChunk) - iAsyncOpsCount: %d"), iAsyncOpsCount));
    iConnection.Transaction(status, iErrorNumberBuffer, iTxnIdBuffer, aClientOp);
    
    if ( aClientOp.HasBinaryDataArray() )
        {
        RegisterAndSubscribeFileProgressObserversL();
        retVal = MoveBinaryContainersToServer(aClientOp, aClientOp.BinaryDataArrayL());		//codescannerwarnings
        }
    else if (aClientOp.HasSharedFileHandle())
       {
       	retVal = MoveFileChunkHandleToServer(aClientOp) ;
       }

    // Pass handle to SendMsgAndReceiveTxnId, so that RHandleBase can be re-used:
    retVal = iConnection.SendMsgAndReceiveTxnId( aClientOp );
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Waiting for response.)");

    User::WaitForRequest(status);   // Wait here until response is received (TXN == COMPLETE)
    iAsyncOpsCount--;

    switch(status.Int())
        {
        case ESenServRequestResponsed:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL - ESenServRequestResponsed");
            retVal = iErrorNumber;
            }
            break;

        case ESenReAuthAndResendNeeded:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL: ESenReAuthAndResendNeeded");
            
            if ( iAsyncOpsCount >= KAsyncMessageSlots)
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Cannot re-init, max simultaneous async ops on-going: KErrSenServiceConnectionBusy");
                retVal = KErrSenServiceConnectionBusy;
                }
            else
                {
                aClientOp.ChunkHeader().SetDoInitBeforeSending(ETrue);
                if(ipAuthProvider)
                	{
                	if(iReauthenticationNeeded)
                		{
                		TInt lerror(KErrNone);
                		TRAP(lerror, ReauthNeededL(KNullDesC8)); // ignore errors
                		}
                	else
                		{
                		iReauthenticationNeeded = ETrue;
                		}
                	}
                retVal = SubmitL(aClientOp); 
                iReauthenticationNeeded = EFalse;
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL: ESenReAuthAndResendNeeded Completed");
                return retVal;
                }
            }
            break;

        case ESenResendNeeded:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL: ESenResendNeeded");
            
            if ( iAsyncOpsCount >= KAsyncMessageSlots)
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Cannot re-send, max simultaneous async ops on-going: KErrSenServiceConnectionBusy");
                retVal = KErrSenServiceConnectionBusy;
                }
            else
                {
                return SubmitL(aClientOp);
                }
            }
            break;

        case ESenInternalError:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL - ESenInternalError");
            retVal = KErrSenInternal;
            }
            break;
        default:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL - default");
#ifdef _SENDEBUG                
            if(status.Int() == KErrPermissionDenied)
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL: default - KErrPermissionDenied");
                }
            else
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"SubmitL: default - unexpected error.");
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("Last received error from server: (%d)"), iErrorNumber));
                }
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("SubmitL, request status.Int(): (%d)"), status.Int()));
            
#endif // _SENDEBUG             
            // WSF does not generalize errors:
            retVal = status.Int();
            }
            break;
        }
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SubmitL(CSenChunk) Completed")));
    return retVal;
    }

TInt CSenServiceConnectionImpl::ResponseFromChunk(CSenChunk& aClientOp,
                                                  HBufC8*& aResponseTo)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::ResponseFromChunk()")));
    TInt leaveCode(KErrNone); // handle OOM case:
    // Read response message, SOAP fault OR error
    TPtrC8 response;
    TInt retVal = aClientOp.DescFromChunk(response); // if handle is closed, this will panic.
    if(retVal==KErrNone)
        {
        // It is *mandatory* tor TRAP AllocL(), since HandleBase MUST be closed(!):
        TRAP( leaveCode, aResponseTo = response.AllocL(); )
        }
    
    if(leaveCode!=KErrNone)
        {
        // OOM occured
        retVal = leaveCode;
        }
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::ResponseFromChunk() Completed")));
    return retVal;
    }

TInt CSenServiceConnectionImpl::SubmitL(const TDesC8& aRequest,
                                        HBufC8*& aResponseTo )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL");

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
        if( pOperation )
        	{
	        CleanupStack::PushL( pOperation );        	    
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif //_SENDEBUG  
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = pOperation->DescToChunk(aRequest);
	            if (retVal == KErrNone)
	                {
	                retVal = SubmitL(*pOperation);
	                if (retVal != KErrNone && retVal != KErrSenSoapFault)
	                    {
	                    aResponseTo = NULL;
	                    }
	                else
	                    {
	                    retVal = ResponseFromChunk(*pOperation, aResponseTo);
	                    }
	                }
	            }
	        CleanupStack::PopAndDestroy(pOperation);
        	}
        	
	     else
	     	{
#ifdef _SENDEBUG        
				TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
	     	}

        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("CSenServiceConnectionImpl::SubmitL - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
        
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL Completed");
    return retVal;
    }

TInt  CSenServiceConnectionImpl::SubmitL(RFile& aFile, HBufC8*& aResponse)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL");

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName() ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
        if( pOperation )
        	{
	        CleanupStack::PushL( pOperation );        	    
	        pOperation->SetLogger(&iLog);
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            pOperation->HasSharedFileHandle(ETrue);
	        	iSharedFileHandle = aFile;
	          	retVal = SubmitL(*pOperation);
                if (retVal != KErrNone && retVal != KErrSenSoapFault)
                    {
                    aResponse = NULL;
                    }
                else
                    {
                    retVal = ResponseFromChunk(*pOperation, aResponse);
                    }
	            }
	        CleanupStack::PopAndDestroy(pOperation);
        	}
        	
	     else
	     	{
#ifdef _SENDEBUG        
				TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
	     	}

        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("CSenServiceConnectionImpl::SubmitL - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
        

    return retVal;
    }
	
// Setter
TInt CSenServiceConnectionImpl::CompleteServerMessagesOnOff( const TBool& aCompleteOnOff )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::CompleteServerMessagesOnOff");
    if( !iInitialized )
        {
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- SC is not yet initialized; saving facet value for post CTR use.");
        //TInt leaveCode(KErrNone);
        delete ipCompleteServerMsgsOnOff; // to prevent memory leaks in sequal calls
        ipCompleteServerMsgsOnOff = NULL;
        //TRAP( leaveCode, ipCompleteServerMsgsOnOff = new (ELeave) TBool(aCompleteOnOff); )
        // if ( leaveCode )
        ipCompleteServerMsgsOnOff = new TBool(aCompleteOnOff); // it is better to make NULL check than trap new (ELeave)
        if ( !ipCompleteServerMsgsOnOff )
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- FATAL: OOM occurred!");
            //return leaveCode;
            return KErrNoMemory;
            }
        //return KErrSenNotInitialized;
#ifdef _SENDEBUG        
        if( *ipCompleteServerMsgsOnOff )
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- CompleteServerMessagesOnOff mode: ON (TRUE) saved.");
            }
        else
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- CompleteServerMessagesOnOff mode: OFF (FALSE) saved.");
            }
#endif // _SENDEBUG            
        return KErrNone;
        }
    
    TInt retVal = iConnection.CompleteServerMessagesOnOff((TBool&)aCompleteOnOff);

    if (retVal == KErrConnectionInitializing)
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8(" CSenServiceConnectionImpl::CompleteServerMessagesOnOff - Calling SetStatus(%d)"), 
            KSenConnectionStatusNew ));
        DeliverStatus( KSenConnectionStatusNew );
        }
    else if (retVal == KErrConnectionExpired)
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8(" CSenServiceConnectionImpl::CompleteServerMessagesOnOff - Calling SetStatus(%d)"), 
            KErrConnectionExpired ));
        DeliverStatus( KSenConnectionStatusExpired );
        }
    return retVal;
    }

// Getter
TInt CSenServiceConnectionImpl::HasFacetL(const TDesC8& aURI, TBool& aHasFacet)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::HasFacetL");
    TInt retVal(iConnection.HasFacet((TDesC8&)aURI, aHasFacet));

    if (retVal == KErrConnectionInitializing)
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8(" CSenServiceConnectionImpl::HasFacetL - Calling SetStatus(%d)"), 
            KSenConnectionStatusNew ));
        DeliverStatus( KSenConnectionStatusNew );
        }
    else if (retVal == KErrConnectionExpired)
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8(" CSenServiceConnectionImpl::HasFacetL - Calling SetStatus(%d)"), 
            KSenConnectionStatusExpired ));
        DeliverStatus( KSenConnectionStatusExpired );
        }
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::HasFacetL Completed");
    return retVal;
    }

// Getter
TInt CSenServiceConnectionImpl::IsReady(TBool& aReady)
    {
//     TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::IsReady"); // DO NOT ADD LOGGING IN HERE
    if( !iInitialized && iIsInternalSC )
        {
        aReady = EFalse;
        return KErrNone;
        }	
    TInt retVal(iConnection.IsReady(aReady));
//    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::IsReady Completed"); // DO NOT ADD LOGGING IN HERE
    return retVal;
    }

TInt CSenServiceConnectionImpl::CheckConnection()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::CheckConnection")));
    TInt retVal(KErrNone);
    
    if(!iInitialized)
        {
        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::CheckConnection - KErrSenNotInitialized")));
        retVal = KErrSenNotInitialized;
        }
     else if (iAsyncOpsCount > KAsyncMessageSlots-1)
        {
        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::CheckConnection - KErrServerBusy (not enough message slots)")));
        retVal = KErrServerBusy;
        }
    else if (!IsActive())
        {
       	SetActive();
        iStatus = KRequestPending;
        }
	TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::CheckConnection Completed")));
    return retVal;
    }

TInt CSenServiceConnectionImpl::SendL(CSenConnectionChunk* apClientOp)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SendL(CSenChunk)")));
    TInt retVal( KErrNone );
    CleanupStack::PushL( apClientOp );
    
    iErrorNumber = 0;
    iTxnId = 0;
    
    iAsyncOpsCount++;
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("iAsyncOpsCount: %d"), iAsyncOpsCount));
    
    CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
    iConnection.Transaction(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer, *apClientOp);
    
    if ( apClientOp->HasBinaryDataArray() )
        {
        RegisterAndSubscribeFileProgressObserversL();
        retVal = MoveBinaryContainersToServer(*apClientOp, apClientOp->BinaryDataArrayL());
        }
	    else if (apClientOp->HasSharedFileHandle())
	    {
	    retVal = MoveFileChunkHandleToServer(*apClientOp) ;
	    }


    if( iDispatcherEnabled )  // DISPATCHER IS ENABLED
        {
        //iTxnId = retVal;
        TInt* pTxnId = new (ELeave) TInt(++iVirtualTransactionCounter);
        CleanupStack::PushL(pTxnId);

        TInt appendRetVal(KErrNone);
        appendRetVal = ChunkMapL().Append( pTxnId, apClientOp );
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- ChunkMapL().Append(%d, %d) returned: %d"), *pTxnId, apClientOp->Chunk().Handle(), appendRetVal));
        if( appendRetVal == KErrNone )
            {
            TMessage msg;
            msg.iSenConnectionChunk = apClientOp;
            msg.iSenAsyncOperation = pSenAO;

            TThreadMessage thrMsg = { msg, iVirtualTransactionCounter };
    		
    		// INSTEAD OF DOING ANY IPC OPERATION WITH WSF (Core), THE REQUEST IS ADDED TO THE QUEUE!
            appendRetVal = ipSenServiceDispatcher->AddToTheQueue( thrMsg ); //NOTE THAT CHUNK MAP IS NOT USED IN HERE (!)
            if( appendRetVal==KErrNone )
                {
                //Required transaction id to respond back to the client
                iTxnId = *pTxnId;
                // OK, pTxnId, was appended successfully to the dispatchers map                
                CleanupStack::Pop( pTxnId ); // now owned by the map
                CleanupStack::Pop( apClientOp ); // now owned by the map
                retVal = iVirtualTransactionCounter;
                }
            else
                {
                CleanupStack::PopAndDestroy( pTxnId ); // orphanized
                CleanupStack::PopAndDestroy( apClientOp ); // orphanized
                retVal = appendRetVal;
                }
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- ipSenServiceDispatcher->AddToTheQueue() returned: %d"), appendRetVal));
                
            }
        else
            {
            CleanupStack::PopAndDestroy( pTxnId ); // orphanized
            CleanupStack::PopAndDestroy( apClientOp ); // orphanized
            }
        }
    else // DISPATCHER IS DISABLED
        {
        // This is EXPECTED TO BE "quick" synchronous operation and returns the TxnId:        
        retVal = iConnection.SendMsgAndReceiveTxnId(*apClientOp);
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher NOT used]: SendMsgAndReceiveTxnId returned: %d"), retVal));
        if ( retVal > KErrNone )
            {
            iTxnId = retVal;
            TInt* pTxnId = new (ELeave) TInt( iTxnId );
            CleanupStack::PushL(pTxnId);

            TInt appendRetVal(KErrNone);
            appendRetVal = ChunkMapL().Append( pTxnId, apClientOp );
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- ChunkMapL().Append(%d, %d) returned: %d"), *pTxnId, apClientOp->Chunk().Handle(), appendRetVal));
            if(appendRetVal==KErrNone)
                {
                // OK, pTxnId, was appended successfully to iChunkMap
                CleanupStack::Pop( pTxnId ); // now owned by the map
                CleanupStack::Pop( apClientOp ); // now owned by the map
                }
            else
                {
                CleanupStack::PopAndDestroy( pTxnId ); // orphanized
                CleanupStack::PopAndDestroy( apClientOp ); // orphanized
                retVal = appendRetVal;
                }
            }
        else
            {
            CleanupStack::PopAndDestroy( apClientOp ); // orphanized
            }
        }
	TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SendL(CSenChunk) Completed")));        
    return retVal;
    }

TInt CSenServiceConnectionImpl::SendL(const TDesC8& aRequest)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(TDesC8)");

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
		if( pOperation )
			{
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif //_SENDEBUG  
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = pOperation->DescToChunk(aRequest);
	            if (retVal == KErrNone)
	                {
	                retVal = SendL(pOperation);
	                }
	            }
    	    }
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("CSenServiceConnectionImpl::SendL - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(TDesC8) Completed");    	
    return retVal;
    }

// This method is executed when one of the static constructors of service connection (NewL or NewLC) is called.
void CSenServiceConnectionImpl::InitializeL( /* MSenServiceDescription& aSD */ )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::InitializeL");
    if (!IsActive())
        {
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Opening a new chunk for connection initialization descriptors");
        delete ipInitChunk; // added
        ipInitChunk = NULL; // added
        TInt error( KErrConnectionInitializing );

        if( iIsInternalSC )
            {
            ipInitChunk = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); 
            if( !ipInitChunk )
                {
                error = KErrConnectionInitializing;
                //User::Leave( error );
                }
            }
        else
            {
            ipInitChunk = CSenChunk::NewL(NextChunkName()); // added
            }
        
        
        if( /* !error && */ ipInitChunk ) 
            {
            ipInitChunk->SetLogger(&iLog);
            error = ipInitChunk->CreateChunk();
            }

        TInt serviceDescriptionLength(0);
        TInt consumerPolicyLength(0);

        // => ipInitPatternAsXml & ipInitConsumerPolicyAsXml  allocated now in ConstructL, old ConstructL moved to ConnectL
        if( !error )
            {
            TPtrC8 serviceDescPtr = ipInitPatternAsXml->Des();
            serviceDescriptionLength = serviceDescPtr.Length();
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|");
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- SD as XML:");
            TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,( serviceDescPtr ));
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|");
/*
            if( aSD.DescriptionClassType() == MSenServiceDescription::EServicePattern )
                {
                CSenServicePattern* pServicePattern = (CSenServicePattern*)&aSD;
*/                
            if( ipInitConsumerPolicyAsXml )
                {
    	        TPtrC8 consumerPolicyPtr(ipInitConsumerPolicyAsXml->Des());
    	        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::InitializeL - ConsumerPolicy:");
    	        TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(*ipInitConsumerPolicyAsXml));
            	error = ipInitChunk->DescsToChunk(serviceDescPtr, consumerPolicyPtr);
                consumerPolicyLength = consumerPolicyPtr.Length();
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- DescsToChunk returned: %d"), error));
                }
            else
            	{
            	// No consumer policy provided by the service consumer application
            	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::InitializeL - No consumer policy is used.");
            	error = ipInitChunk->DescToChunk(serviceDescPtr);
            	TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- DescToChunk returned: %d"), error));
            	}
            }

        if( error )
            {
            // At the moment, pass through the original error code. 
            // error code constant ("KErrSenDescToChunkFailed") 
            TInt leaveCode(KErrNone);
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::InitializeL");

            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Since the SC is NOT ready, pass KSenConnectionStatusCreateFailed to SetStatus. Error: (%d)"), error ));
            DeliverStatus( KSenConnectionStatusCreateFailed );
            TRAP( leaveCode, iObserver->HandleErrorL( error, KNullDesC8 ); )
            if(leaveCode)
                {
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("MSenServiceConsumer::HandleErrorL leaved: %d"), leaveCode));
                }
            }
        else // no errors
            {
            iErrorNumber = 0;
            iErrorNumberBuffer.Set(reinterpret_cast<TUint8*>(&iErrorNumber), sizeof(TInt), sizeof(TInt));
            iAsyncOpsCount++;
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- iAsyncOpsCount: %d"), iAsyncOpsCount));
            CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);

            if( !iIsInternalSC )
                {
                ipConnectionStatusObserver = CSenConnectionStatusObserver::NewL(*iObserver, iConnectionID);
                }
            
            if( ipExtendedConsumerInterface )
                {
                ipInterface = NULL;
                ipInterface = ipExtendedConsumerInterface->GetInterfaceByUid( KSenInterfaceUidAuthenticationProvider );
                if(ipInterface)
                    {
                    ipAuthProvider = (MSenAuthenticationProvider*) ipInterface;
                    // Register authentication observer
					
                    error = RegisterAuthenticationObserver();
#ifdef _SENDEBUG                    
                    if( error )
                        {
                        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- RegisterAuthenticationObserver() failed!");                            
                        }
#endif // _SENDEBUG                        
                    }
                TInt retCode( KErrNone );
                ipInterface = NULL;
                ipInterface = ipExtendedConsumerInterface->GetInterfaceByUid( KSenInterfaceUidCoBrandingObserver );
                if(ipInterface)
                    {
                    ipCoBrandingObserver = (MSenCoBrandingObserver*) ipInterface;
                    
                    // Register CoBranding observer
                    retCode = RegisterCoBrandingObserver();
              
                    if( retCode )
                        {
                        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- RegisterCoBrandingObserver() failed!");
                        if( !error )
                            {
                            // none of the previous "register" IPC operations have failed so far:
                            error = retCode;  // ok to assign this as the error
                            }
                        }
                    }
                ipInterface = NULL;
                ipInterface = ipExtendedConsumerInterface->GetInterfaceByUid( KSenInterfaceUidFilesObserver );
                if( ipInterface )
                    {
                    iFilesObserver = (MSenFilesObserver*) ipInterface;
                    // Register Files observer

                    //retCode = RegisterFilesObserver();// Moved to SendL and SubmitL to avoid unnecessary calls of RegisterFilesObserver
									 					// and making the constructor light weight
 					
 					// ==> 2008-11-05 mmattola: 
 					// commenting RegisterFilesObserver() OUT would be OK otherwise,
 					//  BUT(!): that would mean that that the client CANNOT receive any "progress" info BEFORE / PRIOR sending something out(!) => not good!
						
				    
                    
                    if ( !iRegisterFileObserverDone )
                        {
                        TLSLOG_L( KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel, "InitializeL(): - Calling IssueRegistrations()");
                        //ipRegistrationTimer->IssueRegistrations( ETrue );
                        
                        /*
                        retCode = RegisterAndSubscribeFileProgressObserversL(); // returning this in here, to ENABLE early reception
                        if( retCode )
                            {
                            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- RegisterFilesObserver() failed!");
                            if( !error )
                                {
                                // none of the previous "register" IPC operations have failed so far:
                                error = retCode; // ok to assign this as the error
                                }
                            }
                        */
                        } // end of if ( !iRegisterFileObserverDone )
                    }
                ipInterface = NULL;
                ipInterface = ipExtendedConsumerInterface->GetInterfaceByUid( KSenInterfaceUidHostletConsumer );
                if(ipInterface)
                    {
                    ipHostletConsumer = (MSenHostletConsumer*) ipInterface;
                    }
                    
                ipInterface = NULL;
                ipInterface = ipExtendedConsumerInterface->GetInterfaceByUid( KSenInterfaceUidUserinfoProvider );
                //if(ipInterface)
                //    {
                //    iUserinfoProvider = (MSenUserinfoProvider*) ipInterface;
                //    }
                
                ipInterface = NULL;
                ipInterface = ipExtendedConsumerInterface->GetInterfaceByUid( KSenInterfaceUidAlrObserver );

#ifdef __ENABLE_ALR__
                if(ipInterface)
                    {
                    iMobiltyObserver = (MMobilityProtocolResp*) ipInterface;
                    
                    // Register mobility observer
                    retCode = RegisterMobilityObserverL();

                    if( retCode )
                        {
                        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- RegisterMobilityObserver() failed!");
                        if( !error )
                            {
                            // none of the previous "register" IPC operations have failed so far:
                            error = retCode; // ok to assign this as the error
                            }
                        }
                    }
#endif //__ENABLE_ALR__

                if( !error )
                	{
                	if(ipAuthProvider)
                    	{
                    	iConnection.Initialize( pSenAO->iStatus,
                                            	pSenAO->iErrorNumberBuffer,
                                            	iSessionIdBuffer,
                                            	*ipInitChunk,
                                            	serviceDescriptionLength,
                                            	consumerPolicyLength,
                                            	ETrue );
                    	}
                    	else
                    	{
                    	iConnection.Initialize( pSenAO->iStatus,
                                            	pSenAO->iErrorNumberBuffer,
                                            	iSessionIdBuffer,
                                            	*ipInitChunk,
                                            	serviceDescriptionLength,
                                            	consumerPolicyLength,
                                            	EFalse );
                    	}
                	}
                else // occured
                	{
                	// Initialization of connection agent / some of the "register" -alike IPC ops failed 
                	TInt leaveCode(KErrNone);
                	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::InitializeL");

                	TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Since the SC is NOT ready, pass KSenConnectionStatusCreateFailed to SetStatus. Error: (%d)"), error ));
                	DeliverStatus( KSenConnectionStatusCreateFailed );
                	TRAP( leaveCode, iObserver->HandleErrorL( error, KNullDesC8 ); )
                	if(leaveCode)
                		{
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("MSenServiceConsumer::HandleErrorL leaved: %d"), leaveCode));
                	    }
                	}
                                    
                
            }
            else
                {
                iConnection.Initialize( pSenAO->iStatus,
                                        pSenAO->iErrorNumberBuffer,
                                        iSessionIdBuffer,
                                        *ipInitChunk,
                                        serviceDescriptionLength,
                                        consumerPolicyLength,
                                        EFalse );
                }
            }
        }
    else
        {
        // Error occurred!
        TInt leaveCode(KErrNone);
        // This is actually AO "busy" case, but due sequential calls to SC's ConstructL()
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::InitializeL");
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Since the SC is NOT ready (AO is busy), pass KSenConnectionStatusCreateFailed to SetStatus.");
        DeliverStatus( KSenConnectionStatusCreateFailed );

        TRAP( leaveCode, iObserver->HandleErrorL(KErrConnectionInitializing, KNullDesC8); )
        if(leaveCode)
            {
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("MSenServiceConsumer::HandleErrorL leaved: %d"), leaveCode ));
            }
        }
		TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::InitializeL() Completed")));
    }

TPtrC CSenServiceConnectionImpl::NextChunkName()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::NextChunkName()")));
    if(ipChunkName)
        {
        iChunkNameNumber++;
        TPtr chunkname = ipChunkName->Des();
        chunkname.Zero();
        TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,(iSessionIdBuffer));

        if(iSessionIdBuffer.Length()>KSenUuidPrefixLength)
            {
            chunkname.Append(iSessionIdBuffer.Right(iSessionIdBuffer.Length()-KSenUuidPrefixLength));
            }
        chunkname.AppendNum(iChunkNameNumber);

        // Remove dashes
         _LIT(KDash, "-");
        TInt pos = chunkname.Find(KDash);
        while (pos != KErrNotFound)
            {
            chunkname.Replace(pos, KDash().Length(), KNullDesC);
            pos = chunkname.Find(KDash);
            }
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L("CSenServiceConnectionImpl::NextChunkName(): %S"), &chunkname));
        return chunkname.Left(KMaxName); // max length of TName
        }
    else
        {
        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::NextChunkName() returns KNullDesC()")));
        return KNullDesC();
        }  
    }
void CSenServiceConnectionImpl::RunL()
    {
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::RunL( %d )"),
                                            iStatus.Int()));
    }
void CSenServiceConnectionImpl::HandleMessageFromChildAOL(TInt aStatus)
    {
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::HandleMessageFromChildAOL( %d )"),
                                            aStatus));
    iAsyncOpsCount--;
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- One async op completed. Pending iAsyncOpsCount: %d"), iAsyncOpsCount));
                                            
    switch (aStatus)
        {
        case ESenServInitialized:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: ESenServInitialized");

            TInt leaveCode(KErrNone);
            if ( iErrorNumber == KErrNone )
                {
                iInitialized = ETrue;

                if ( ipCompleteServerMsgsOnOff )
                    {
                    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Performing ctr-implicit call to pending CompleteServerMessagesOnOff" );
                    TBool complete = *ipCompleteServerMsgsOnOff;
                    TInt errorCode = CompleteServerMessagesOnOff( complete );
#ifdef _SENDEBUG                        
                    if ( errorCode )
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8( "´- Ctr-implicit call to CompleteServerMessagesOnOff failed: (%d)!"), errorCode));
                        }
                    else
                        {
                        if( complete )
                            {
                            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , "- Ctr-implicit call ok. Complete msgs mode: ON (true)" );
                            }
                        else
                            {
                            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , "- Ctr-implicit call ok. Complete msgs mode: OFF (false)" );
                            }
                        }                        
#endif // _SENDEBUG                    


                    }
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::HandleMessageFromChildAOL - Calling SetStatus(%d)"), KSenConnectionStatusReady ));
                DeliverStatus( KSenConnectionStatusReady ); // Service session is valid and ready to be used!
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- Session ID (%d bytes):"), iSessionIdBuffer.Length()));
                TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel ,( iSessionIdBuffer.Right(iSessionIdBuffer.Length()-5) ));
                }
            else
                {
                // Error occurred   
                DeliverStatus( KSenConnectionStatusCreateFailed );
                
                TPtrC8 errorDescription;
                TInt retVal = ipInitChunk->DescFromChunk(errorDescription);

                TInt error = iErrorNumber;
                if ( error == KErrSenSoapFault && !iInitialized ) // China DC
                    {
                    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Since the SC is NOT ready, pass KErrSenAuthenticationFault to HandleErrorL instead of KErrSenSoapFault");
                    error = KErrSenAuthenticationFault;
                    }
                    
                if ( retVal == KErrNone )
                    {
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("HandleMessageFromChildAOL: ESenServInitialize failed: %d"), error ));
                    TRAP( leaveCode, iObserver->HandleErrorL(error, errorDescription); )
                    if( leaveCode )
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("MSenServiceConsumer::HandleMessageFromChildAOL - HandleErrorL leaves with %d"), leaveCode));
                        }
                    }
                else
                    {
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- DescFromChunk failed: %d - callback will provide err code (%d) only."), retVal, error));
                    DeliverResponseL( error );
                    }
                }
            delete ipInitChunk;
            ipInitChunk = NULL;
            delete ipInitPatternAsXml;
            ipInitPatternAsXml = NULL;
            delete ipInitConsumerPolicyAsXml;
            ipInitConsumerPolicyAsXml = NULL;

            break;
            }

        case ESenServRequestResponsed:       
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: ESenServRequestResponsed");
            if ( iDispatcherEnabled ) // DISPATCHER IS ENABLED
                {
                TInt keyAt = ipSenServiceDispatcher->GetVirtualTransactionID( &iTxnId );
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: Looking for virtual transaction ID");
                if( keyAt != KErrNotFound )
                    {
                    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: Found virtual transaction ID");
                    TInt index = ChunkMapL().Find(keyAt);
                    if ( index != KErrNotFound )
                        {
                        CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                        TPtrC8 response;
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher is enabled]: HandleMessageFromChildAOL - mapped RChunk's handle: %d"), pOperation->Chunk().Handle()));

                        TInt retVal = pOperation->DescFromChunk(response);
                        if ( retVal == KErrNone )
                            {
                            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- [dispatcher is enabled]: DescFromChunk ok");
                            DeliverResponseL(iErrorNumber, pOperation);
                            }
                        else
                            {
                            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is enabled]: DescFromChunk failed: %d"), retVal));
                            DeliverResponseL(retVal);
                            }
                        // Delete TxnId and CSenClientOp
                        retVal = ChunkMapL().Remove(*pOperation);
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is enabled]: ChunkMap().Remove() returned: %d"), retVal));
                        retVal = ipSenServiceDispatcher->RemoveFromTransMap(&keyAt);
                        }
                    else
                        {
                        LOG_WRITEFORMAT((_L("- Closing the transaction due SendL has failed: %d"), iErrorNumber));
                        }
                    }
                else
                    {
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher is enabled]: Closing the transaction due SendL has failed: %d"), iErrorNumber));
                    }
                }
            else // DISPATCHER IS DISABLED
                {
                TInt index = ChunkMapL().Find(iTxnId);
                if ( index != KErrNotFound )
                    {
                    CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                    TPtrC8 response;
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher is disabled]: HandleMessageFromChildAOL - mapped RChunk's handle: %d"), pOperation->Chunk().Handle()));

                    TInt retVal = pOperation->DescFromChunk(response);
                    if ( retVal == KErrNone )
                        {
                        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- [dispatcher is disabled]: DescFromChunk ok");
                        DeliverResponseL(iErrorNumber, pOperation);
                        }
                    else
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is disabled]: DescFromChunk failed: %d"), retVal));
                        DeliverResponseL(retVal);
                        }

                    // Delete TxnId and CSenClientOp
                    retVal = ChunkMapL().Remove(*pOperation);
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is disabled]: ChunkMap().Remove() returned: %d"), retVal));
                    retVal = 0; // not used in release builds
                    }
                else
                    {
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher is disabled]: Closing the transaction due SendL has failed: %d"), iErrorNumber));
                    }
                }
            break;
            }

        case ESenInternalError:
            {
            delete ipInitChunk; // added, pre-emptive de-alloc in case of errors..
            ipInitChunk = NULL; // added
            delete ipInitPatternAsXml;
            ipInitPatternAsXml = NULL;
            delete ipInitConsumerPolicyAsXml;
            ipInitConsumerPolicyAsXml = NULL;
            
            // we could have someting in iErrorNumber in some cases
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: ESenInternalError");
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Last received error from server: (%d)"),
                    iErrorNumber));

            DeliverResponseL(KErrSenInternal);
            }
            break;

        case ESenServRequestCancelled:
            {
            DeliverResponseL( KErrSenCancelled);
            }
            break;

        case ESenReAuthAndResendNeeded:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: ESenReAuthAndResendNeeded");
            
            TInt retVal(KErrNone);
            if ( iAsyncOpsCount >= KAsyncMessageSlots)
                {
                retVal = KErrSenServiceConnectionBusy;
                }
                
            if ( retVal == KErrNone )
                {
                if ( iDispatcherEnabled ) // DISPATCHER IS ENABLED
                    {
                    TInt keyAt = ipSenServiceDispatcher->GetVirtualTransactionID(&iTxnId);
                    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: Looking for virtual transaction ID");
                    if(keyAt != KErrNotFound)
                        {
                        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: Found virtual transaction ID");
                        TInt index = ChunkMapL().Find(keyAt);
                        if ( index != KErrNotFound )
                            {
                            CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                            
                            pOperation->ChunkHeader().SetDoInitBeforeSending(ETrue);
                            
                            iAsyncOpsCount++;
                            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher enabled]: iAsyncOpsCount: %d"), iAsyncOpsCount));
                            CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
                            iConnection.Transaction(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer, *pOperation);

                            if ( pOperation->HasBinaryDataArray() )
                                {
                                retVal = MoveBinaryContainersToServer(*pOperation, pOperation->BinaryDataArrayL());
                                }
                            // Reauthentication
                            if(ipAuthProvider)
                            	{
                            	pSenAO->iReauthTxnId = iTxnId;
                            	if(iReauthenticationNeeded)
                            		{
                            		iReauthenticationNeeded = EFalse; // unmark reauthentication flag
        	                    	TInt lerror(KErrNone);
        	                    	TRAP(lerror, ReauthNeededL(KNullDesC8));
        	                    	}
                            	}
                            // This is quick and returns the TxnId:
                            retVal = iConnection.SendMsgAndReceiveTxnId(*pOperation);
                            //iTxnId = retVal;
                            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher enabled]: SendMsgAndReceiveTxnId returned: %d"), retVal));
                        	TInt* pActlTxnId = new (ELeave) TInt(0);
    			            *pActlTxnId = iTxnId /*retVal*/;
    				        CleanupStack::PushL(pActlTxnId);
    				
                            retVal = ipSenServiceDispatcher->UpdateTransMap(&keyAt,pActlTxnId);
                            if(retVal == KErrNone)
                                {
                                CleanupStack::Pop(pActlTxnId);
                                }
                            else
                                {
    			    	        CleanupStack::PopAndDestroy(pActlTxnId);
                                }
                            }
                        else
                            {
                            retVal = KErrSenInternal;
                            }
                        }
                    else
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher enabled]: ChunkMap().FindKey(%d) failed!"), iTxnId));
                        retVal = KErrSenInternal;
                        }
                        
                    }
                else // DISPATCHER IS DISABLED
                    {
                    TInt index = ChunkMapL().Find(iTxnId);
                    if ( index != KErrNotFound )
                        {
                        CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                        
                        pOperation->ChunkHeader().SetDoInitBeforeSending(ETrue);
                        
                        iAsyncOpsCount++;
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher disabled]: iAsyncOpsCount: %d"), iAsyncOpsCount));
                        CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
                        iConnection.Transaction(pSenAO->iStatus, pSenAO->iErrorNumberBuffer,
                                                pSenAO->iTxnIdBuffer, *pOperation);

                        if ( pOperation->HasBinaryDataArray() )
                            {
                            retVal = MoveBinaryContainersToServer(*pOperation,
                                                                  pOperation->BinaryDataArrayL());
                            }
                        // Reauthentication
                        if(ipAuthProvider)
                        	{
                        	pSenAO->iReauthTxnId = iTxnId;
                        	if(iReauthenticationNeeded)
                        		{
                        		iReauthenticationNeeded = EFalse; //unmark reauthentication flag
    	                    	TInt lerror(KErrNone);
    	                    	TRAP(lerror, ReauthNeededL(KNullDesC8));
    	                    	}
                        	}
                        // This is quick and returns the TxnId:
                        retVal = iConnection.SendMsgAndReceiveTxnId(*pOperation);
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher disabled]: SendMsgAndReceiveTxnId returned: %d"), retVal));
                        }
                    else
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher disabled]: ChunkMap().FindKey(%d) failed!"), iTxnId));
                        retVal = KErrSenInternal;
                        }
                    }
                }
                
            if ( retVal < KErrNone )
                {
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher disabled]:  Resend failed: %d"), retVal));
                // Note: do NOT return anything to consumer here (never call DeliverResponseL(retVal) here!)
                }
            }
            break;

        case ESenResendNeeded:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: ESenResendNeeded");

            TInt retVal(KErrNone);
            if ( iAsyncOpsCount >= KAsyncMessageSlots )
                {
                retVal = KErrSenServiceConnectionBusy;
                }
                
            if ( retVal == KErrNone )
                {
                if( iDispatcherEnabled ) // DISPATCHER IS ENABLED
                    {
                    TInt keyAt = ipSenServiceDispatcher->GetVirtualTransactionID( &iTxnId );
                    
                    if(keyAt != KErrNotFound)
                        {
                        TInt index = ChunkMapL().Find(keyAt);
                        if(index != KErrNotFound)
                            {
                            CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                            
                            iAsyncOpsCount++;
                            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is enabled]: iAsyncOpsCount: %d"), iAsyncOpsCount));
                            CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL( this );
                            iConnection.Transaction(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer, *pOperation);
                                
                            if ( pOperation->HasBinaryDataArray() )
                                {
                                retVal = MoveBinaryContainersToServer(*pOperation, pOperation->BinaryDataArrayL());
                                }

                            // This is EXPECTED TO BE "quick" SYNC IPC OPERATION THAT "just" returns the TxnId:
                            
                            // shouldn't this potentially "slow" IPC be performed 
                            // by dispatcher(?) => resending is not handled by "dispatcher thread" now..
                            retVal = iConnection.SendMsgAndReceiveTxnId(*pOperation); 
                            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is enabled]: SendMsgAndReceiveTxnId returned: %d"), retVal));
                            }
                        else
                            {
                            retVal = KErrSenInternal;
                            }
                        }
                    else
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher is enabled]: ChunkMap().FindKey(%d) failed!"), iTxnId));                    
                        retVal = KErrSenInternal;
                        }
                    }
                else // DISPATCHER IS DISABLED
                    {
                    TInt index = ChunkMapL().Find(iTxnId);
                    if(index != KErrNotFound)
                        {
                        CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                        
                        iAsyncOpsCount++;
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is disabled]: iAsyncOpsCount: %d"), iAsyncOpsCount));
                        CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
                        iConnection.Transaction(pSenAO->iStatus, pSenAO->iErrorNumberBuffer,
                                                pSenAO->iTxnIdBuffer, *pOperation);
                            
                        if ( pOperation->HasBinaryDataArray() )
                            {
                            retVal = MoveBinaryContainersToServer(*pOperation,
                                                                  pOperation->BinaryDataArrayL());
                            }

                        // This is quick and returns the TxnId:
                        retVal = iConnection.SendMsgAndReceiveTxnId(*pOperation);
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- [dispatcher is disabled]:  SendMsgAndReceiveTxnId returned: %d"), retVal));
                        }
                    else
                        {
                        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- [dispatcher is disabled]: ChunkMap().FindKey(%d) failed!"), iTxnId));                    
                        retVal = KErrSenInternal;
                        }
                    }
                
                }
                
            if ( retVal < KErrNone )
                {
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("=> Resend failed: %d"), retVal));
                // Note: do NOT return anything to consumer here (never call DeliverResponseL(retVal) here!)
                }
            }
            break;
            
        case ESenReAuthNeeded:
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: ESenReAuthNeeded");

            TInt retVal(KErrNone);
            if ( iAsyncOpsCount >= KAsyncMessageSlots )
                {
                retVal = KErrSenServiceConnectionBusy;
                }
                
            if ( retVal == KErrNone )
                {
                iErrorNumber = 0;
                iErrorNumberBuffer.Set(reinterpret_cast<TUint8*>(&iErrorNumber), sizeof(TInt), sizeof(TInt));
                iAsyncOpsCount++;
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- iAsyncOpsCount: %d"), iAsyncOpsCount));
                CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
                
                TInt serviceDescriptionLength(0);
                TInt consumerPolicyLength(0);
                
                TPtrC8 serviceDescPtr = ipInitPatternAsXml->Des();
                serviceDescriptionLength = serviceDescPtr.Length();
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"- SD as XML:");
                TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,( serviceDescPtr ));
                
                if( ipInitConsumerPolicyAsXml )
                    {
        	        TPtrC8 consumerPolicyPtr(ipInitConsumerPolicyAsXml->Des());
        	        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"CSenServiceConnectionImpl::Reauthentication - ConsumerPolicy:");
        	        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,(*ipInitConsumerPolicyAsXml));
                	retVal = ipInitChunk->DescsToChunk(serviceDescPtr, consumerPolicyPtr);
                    consumerPolicyLength = consumerPolicyPtr.Length();
                    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- DescsToChunk returned: %d"), retVal));
                    }
                else
                	{
                	// No consumer policy provided by the service consumer application
                	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - No consumer policy is used.");
                	retVal = ipInitChunk->DescToChunk(serviceDescPtr);
                	TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- DescToChunk returned: %d"), retVal));
                	}
                	
    	        if ( retVal == KErrNone )
    	            {
                    if ( ipAuthProvider )
                        {
                        CSenXmlServiceDescription* pServDesc =
                      	CSenXmlServiceDescription::NewLC();
	                    CSenXmlReader* pXmlReader =
	                       	CSenXmlReader::NewLC(KXmlParserMimeType);
	                    pXmlReader->SetContentHandler(*pServDesc);
	                    pServDesc->SetReader(*pXmlReader);
	                    TInt retVal = pServDesc->BuildFrom(serviceDescPtr);
	                    CleanupStack::PopAndDestroy(pXmlReader);
	                    
	                    if(retVal == KErrNone)
	                       	{
	                        TPtrC8 providerId = pServDesc->ProviderId();
	                        TInt lerror(KErrNone);
	                        TRAP(lerror, retVal = ReauthNeededL(providerId));
	                        if(lerror != KErrNone)
	                        	{
	                        	// Leave occured, most probably because of callback function
	                        	retVal = lerror;
	                        	}
	                      	CleanupStack::PopAndDestroy(pServDesc);
	                       	}
	                    else
	                    	{
	                    	CleanupStack::PopAndDestroy(pServDesc);
	                    	}

                        // Call the asynchronous connection initilizer
	                    if(retVal == KErrNone)
	                    	{
	                    if ( ipAuthProvider )
	                        {
	                        iConnection.Initialize( pSenAO->iStatus,
	                                                pSenAO->iErrorNumberBuffer,
	                                                iSessionIdBuffer,
	                                                *ipInitChunk,
	                                                serviceDescriptionLength,
	                                                consumerPolicyLength,
	                                                ETrue );
	                        }
	                    else
	                        {
	                        iConnection.Initialize( pSenAO->iStatus,
	                                                pSenAO->iErrorNumberBuffer,
	                                                iSessionIdBuffer,
	                                                *ipInitChunk,
	                                                serviceDescriptionLength,
	                                                consumerPolicyLength,
	                                                EFalse );
	                         }
                        }
                        }
                    else
                        {
                        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - authentication provider not found!");
                        retVal = KErrGeneral;
                        }
    	            }
                }
                
            if ( retVal < KErrNone )
                {
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- ReAuth failed: %d"), retVal));
                // Note: do NOT return anything to consumer here (never call DeliverResponseL(retVal) here!)
                // Error occurred   
                //TRAP( leaveCode, iObserver->SetStatus( KSenConnectionStatusCreateFailed ); )
                DeliverStatus( KSenConnectionStatusCreateFailed );
                
                TPtrC8 errorDescription;
                TInt retVal = ipInitChunk->DescFromChunk(errorDescription);

                TInt error = KErrSenSoapFault;
                if ( error == KErrSenSoapFault && !iInitialized ) // China DC
                    {
                    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Since the SC is NOT ready, pass KErrSenAuthenticationFault to HandleErrorL instead of KErrSenSoapFault");
                    error = KErrSenAuthenticationFault;
                    }

                if ( retVal == KErrNone )
                	{
                     TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("HandleMessageFromChildAOL: ESenServInitialize failed: %d"), error ));
                     TInt leaveCode(KErrNone);
                     TRAP( leaveCode, iObserver->HandleErrorL(error, errorDescription); )
                     if( leaveCode )
                    	 {
                         TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("MSenServiceConsumer::HandleMessageFromChildAOL - HandleErrorL leaves with %d"), leaveCode));
                         }
                     }
                else
                     {
                     TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- DescFromChunk failed: %d - callback will provide err code (%d) only."), retVal, error));
                     //DeliverResponseL(retVal);
                     DeliverResponseL( error );
                     }
                
                delete ipInitChunk;
                ipInitChunk = NULL;
                delete ipInitPatternAsXml;
                ipInitPatternAsXml = NULL;
                delete ipInitConsumerPolicyAsXml;
                ipInitConsumerPolicyAsXml = NULL;
                }
            }
            break;

        default:
            {
            delete ipInitChunk; // added, pre-emptive de-alloc in case of errors..
            ipInitChunk = NULL; // added
            
#ifdef _SENDEBUG                
            if ( aStatus == KErrPermissionDenied )
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"HandleMessageFromChildAOL: default - KErrPermissionDenied");
                }
            else
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"RunL: default - unexpected error.");
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("Last received error from server: (%d)"), iErrorNumber));
                }
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("RunL, iStatus.Int(): (%d)"), iStatus.Int()));
            
#endif // _SENDEBUG             
            // WSF does not generalize errors:
            if( iDispatcherEnabled ) // DISPATCHER IS ENABLED
                {
                TInt index = ChunkMapL().Find(iVirtualTransactionCounter);
                if ( index != KErrNotFound )
                    {
                    CSenConnectionChunk* pOperation = (CSenConnectionChunk*)ChunkMapL().ValueAt(index);
                    DeliverResponseL(aStatus);
                    ChunkMapL().Remove(*pOperation);
                    }
                }
            else // DISPATCHER IS DISABLED
                {
                DeliverResponseL(aStatus);
                }
            break ;
            }
        }
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::HandleMessageFromChildAOL Completed");
    }

// Helper function, which delivers response to Service Consumer 
void CSenServiceConnectionImpl::DeliverResponseL(const TInt aErrorCode, CSenChunk* apOperation)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::DeliverResponseL");
    iTransactionIdKnown = ETrue;
    
    TInt statusCode(aErrorCode);
    TPtrC8 response(KNullDesC8);
    CSenMessageBase* pMessage = NULL;
    if( apOperation )
        {
        pMessage = CSenMessageBase::NewLC(*apOperation);
        iResponse = pMessage;

        TInt retVal = apOperation->DescFromChunk(response);
        if( retVal == KErrNone )
            {
            TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- DescFromChunk ok");
            // If data was read ok from chunk, then the status code is in 
            // the IPC arg that was written by Core over process boundary:
            statusCode = iErrorNumber; 
            }
        else
            {
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KNormalLogLevel , _L8("- DescFromChunk failed: %d"), retVal));
            statusCode = retVal;
            response.Set(KNullDesC8);
            }
        }


    if( ipHostletConsumer )
        {
        ipHostletConsumer->SetConnectionId( iConnectionID );
        }        

        
    if( statusCode == KErrNone )
        {
        // STANDARD, "OK" RESPONSE
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- response, (%d bytes):"), response.Length()));
        TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,(response));
        TRAPD( err, iObserver->HandleMessageL( response ); )
        if(err)
            {
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("DeliverResponseL: HandleMessageL() leaved: %d"), err));
            }
        }
    else  // SOAP FAULT or ERROR
        {
        if( statusCode == KErrSenSoapFault)
            {
            TInt error = statusCode;   
                
            // SOAP FAULT
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- SOAP fault (%d bytes):"), response.Length()));
            TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,(response));

            if ( error == KErrSenSoapFault && !iInitialized ) // China DC
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::DeliverResponseL:");
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- Since the SC is NOT ready, pass KErrSenAuthenticationFault to HandleErrorL instead of KErrSenSoapFault");
                error = KErrSenAuthenticationFault;
                }
            TRAPD(err, iObserver->HandleErrorL( error, response ));
            if(err)
                {
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("DeliverResponseL: HandleErrorL() leaved: %d"), err));
                }
            }
        else // some other ERROR, not a SOAPFault
            {
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- error code (%d), error data (%d bytes):"), statusCode, response.Length()));
            TLSLOG_ALL(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,(response));

            TRAPD(err, iObserver->HandleErrorL(statusCode, response));
            if(err)
                {
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("DeliverResponseL: HandleErrorL() leaved: %d"), err));
                }
            }
        }
    if( apOperation )
        {
        iResponse = NULL;
        CleanupStack::PopAndDestroy( pMessage );
        }
    iTransactionIdKnown = EFalse;
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::DeliverResponseL Completed");
    }

void CSenServiceConnectionImpl::DoCancel()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::DoCancel")));
	if (iCancelSession == EFalse)
	{
	    if( iDispatcherEnabled )
	        {
	        ipSenServiceDispatcher->ResetQueue();
	        }

	    TInt cancelLeaveCode(KErrNone);
	    TRAP(cancelLeaveCode, 
	            CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
                iConnection.CancelSession(pSenAO->iStatus);
                );
#ifdef _SENDEBUG
	    if(cancelLeaveCode!=KErrNone)
	        {
	        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- CancelSession leaved: %d)"), 
	            cancelLeaveCode));
	        }
#endif //_SENDEBUG  
	    cancelLeaveCode=0;
	    TRequestStatus* status = &iStatus;
	    User::RequestComplete( status, KErrCancel );
  		//iCancelSession = ETrue;
	}
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::DoCancel Completed")));
    }

RFileLogger* CSenServiceConnectionImpl::Log() const
    {
    return (RFileLogger*) &iLog;
    }


TInt CSenServiceConnectionImpl::SendL(CSenSoapEnvelope& aMessage )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP)");

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
        if( pOperation )
        	{
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif // _SENDEBUG
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = AssignPropertiesToChunkL(*pOperation, aMessage);	//codescannerwarnings
	            if (retVal == KErrNone)
	                {
	                pOperation->ChunkHeader().SetMessageType(MSenMessage::ESoapMessage);
	                retVal = SendL(pOperation);
	                }
	            }
        	}
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SendL(SOAP) - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP) Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::SubmitL(CSenSoapEnvelope& aMessage, 
                                        HBufC8*& aResponseTo)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL(SOAP)");

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
        if( pOperation )
        	{
	        CleanupStack::PushL( pOperation );
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif // _SENDEBUG
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = AssignPropertiesToChunkL(*pOperation, aMessage);	//codescannerwarnings
	            if (retVal == KErrNone)
	                {
	                pOperation->ChunkHeader().SetMessageType(MSenMessage::ESoapMessage);
	                retVal = SubmitL(*pOperation);
	                if (retVal != KErrNone && retVal != KErrSenSoapFault)
	                    {
	                    aResponseTo = NULL;
	                    }
	                else
	                    {
	                    retVal = ResponseFromChunk(*pOperation, aResponseTo);
	                    }
	                }
	            }
	        CleanupStack::PopAndDestroy(pOperation);
        	}
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL(SOAP) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SubmitL(SOAP) - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL(SOAP) Completed");
    return retVal;
    }
    

CSenElement& CSenServiceConnectionImpl::AsElement()
    {
    return iFragment->AsElement();
    }

    /**
    *  @return the current object as element. May return NULL.
    *          Caller takes ownership.
*/
CSenElement* CSenServiceConnectionImpl::ExtractElement()
    {
    return iFragment->ExtractElement();
    }

void CSenServiceConnectionImpl::SetReader(CSenXmlReader& aReader)
    {
    iFragment->SetReader(aReader);
    }

CSenXmlReader* CSenServiceConnectionImpl::Reader()
    {
    return iFragment->Reader();
    }

void CSenServiceConnectionImpl::ParseL(const TDesC8& aBuf)
    {
    iFragment->ParseL(aBuf);
    }

TInt CSenServiceConnectionImpl::BuildFrom(const TDesC8& aBuf)
    {
    return iFragment->BuildFrom(aBuf);
    }

void CSenServiceConnectionImpl::DelegateParsingL(MSenFragment& aDelegate)
    {
    iFragment->DelegateParsingL(aDelegate);
    }

void CSenServiceConnectionImpl::DelegateParsingL(
                                         const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes)
    {
    iFragment->DelegateParsingL(aNsUri, aLocalName, aQName, aAttributes);
    }

void CSenServiceConnectionImpl::ParseWithL(CSenXmlReader& aReader)
    {
    iFragment->ParseWithL(aReader);
    }

void CSenServiceConnectionImpl::SetOwner(MSenFragment& aFragment)
    {
    iFragment->SetOwner(aFragment);
    }

void CSenServiceConnectionImpl::ResumeParsingFromL( const TDesC8& aNsUri,
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aQName)
    {
    iFragment->ResumeParsingFromL(aNsUri, aLocalName, aQName);
    }

void CSenServiceConnectionImpl::SetAttributesL(
                                            const RAttributeArray& aAttributes)
    {
    iFragment->SetAttributesL(aAttributes);
    }


void CSenServiceConnectionImpl::WriteStartElementL(
                                const TDesC8& aNsUri,
                                const TDesC8& aLocalName,
                                const TDesC8& aQName,
                                const RAttributeArray& aAttributes
                                )
    {
    iFragment->WriteStartElementL(aNsUri, aLocalName, aQName, aAttributes);
    }

void CSenServiceConnectionImpl::WriteEndElementL(
                              const TDesC8& aNsUri,
                              const TDesC8& aLocalName,
                              const TDesC8& aQName
                              )
    {
    iFragment->WriteEndElementL(aNsUri, aLocalName, aQName);
    }


TInt CSenServiceConnectionImpl::ServiceDescriptionL(HBufC8*& aServiceDescription)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::ServiceDescriptionL");

    TInt retVal(KErrSenNotInitialized);
    
    if(iInitialized)
        {
        iResponseLength = 0;
        iInBuffer.Set(reinterpret_cast<TUint8*>(&iResponseLength),
            sizeof(TInt),
            sizeof(TInt));

        retVal = iConnection.RequestServiceDescription(iInBuffer);
        if(retVal==KErrNone)
            {
            if(iResponseLength<1)
                {
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"- FATAL error in RequestServiceDescription:");
                TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"  return code OK, response size <= 0 bytes!");
                aServiceDescription = NULL;
                return KErrUnderflow;
                }

            aServiceDescription = HBufC8::NewL(iResponseLength);
            TPtr8 buf = aServiceDescription->Des();
            retVal = iConnection.ReceiveServiceDescription(buf);
            }
        }
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::ServiceDescriptionL Completed");
    return retVal;
    }


const TDesC8& CSenServiceConnectionImpl::LocalName() const
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::LocalName()")));
    if(iFragment)
        return iFragment->LocalName();
    else
        return KNullDesC8;
    }

const TDesC8& CSenServiceConnectionImpl::NsUri() const
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::NsUri()")));
    if(iFragment)
        return iFragment->NsUri();
    else
        return KNullDesC8;
    }

const TDesC8& CSenServiceConnectionImpl::NsPrefix() const
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::NsPrefix()")));
    if(iFragment)
        return iFragment->NsPrefix();
    else
        return KNullDesC8;
    }

void CSenServiceConnectionImpl::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::WriteAsXMLToL()")));
    if(iFragment)
        {
        iFragment->WriteAsXMLToL(aWriteStream);
        }
    }

HBufC* CSenServiceConnectionImpl::AsXmlUnicodeL()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::AsXmlUnicodeL()")));
    if(iFragment)
        {
        return iFragment->AsXmlUnicodeL();
        }
    else
        {
        return KNullDesC().AllocL();
        }
    }

HBufC8* CSenServiceConnectionImpl::AsXmlL()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::AsXmlL()")));
    if(iFragment)
        {
        return iFragment->AsXmlL();
        }
    else
        {
        return KNullDesC8().AllocL();
        }
    }

TInt CSenServiceConnectionImpl::StartTransaction()
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::StartTransaction");
    
    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        	retVal = iConnection.StartTransaction();	
        	
        }    
    else if(retVal == KErrSenNotInitialized)
    	{  	
    	 TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8(" CSenServiceConnectionImpl::StartTransaction - Error(%d)"), 
            KErrSenNotInitialized ));
    	}	 
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::StartTransaction Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::TransactionCompleted()
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::TransactionCompleted");
    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        	retVal = iConnection.TransactionCompleted();	
        }    
    else if(retVal == KErrSenNotInitialized)
    	{
    	 TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8(" CSenServiceConnectionImpl::TransactionCompleted - Error(%d)"), 
            KErrSenNotInitialized )); 		
    	}
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::TransactionCompleted Completed");
    return retVal;    
    }

TBool CSenServiceConnectionImpl::ConsistsOfL(MSenFragment& aCandidate)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::ConsistsOfL()")));
    if (iFragment)
        {
        return iFragment->ConsistsOfL(aCandidate);
        }
    else
        {
        return EFalse;
        }
    }


TInt CSenServiceConnectionImpl::CancelAllRequests()
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::CancelAllRequests(sync)");
    if ( iDispatcherEnabled )
        {
        ipSenServiceDispatcher->ResetQueue();
        }
    TInt cancelLeaveCode(KErrNone);
    TInt retVal(KErrNone);
    TRAP( cancelLeaveCode,
    		CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
    		iConnection.CancelSession(pSenAO->iStatus); 
    		);

    if(cancelLeaveCode!=KErrNone)
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CancelSession() leaved with (code: %d)"), cancelLeaveCode));
        if(retVal==KErrNone)
            {
            retVal = cancelLeaveCode;
            }
        }

	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::CancelAllRequests(sync) Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::TxnId()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::TxnId()")));
    if( iTransactionIdKnown )
        {
        if( iTxnId>KErrNone )
            {
            if( iDispatcherEnabled )
                {
                TInt virtualTransactionId = ipSenServiceDispatcher->GetVirtualTransactionID( &iTxnId );
                return virtualTransactionId;
                }
            else
                {
                return iTxnId;
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        return KErrNotFound;
        }
    }
    
MSenMessage* CSenServiceConnectionImpl::Response()
    {
    return iResponse;
    }
    
TInt CSenServiceConnectionImpl::SetTransportPropertiesL(const TDesC8& aProperties)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SetTransportProperties");

    TInt retVal(KErrSenNotInitialized);
    
    if(iInitialized)
        {
        CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkName());
        pSenChunk->SetLogger(Log());
        retVal = pSenChunk->CreateChunk();
        if (retVal == KErrNone)
            {
            retVal = pSenChunk->DescToChunk(aProperties);
            if (retVal == KErrNone)
                {
                retVal = iConnection.SetTransportPropertiesL(*pSenChunk);
                }
            }
        CleanupStack::PopAndDestroy(pSenChunk);
        }
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SetTransportProperties Completed");
    return retVal;
    }
    
TInt CSenServiceConnectionImpl::CancelTransaction(TInt aTransactionID)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::CancelTransaction");
    TInt retVal(0);
    if ( iDispatcherEnabled ) // DISPATCHER IS ENABLED
        {
        TInt actualTransID = ipSenServiceDispatcher->GetActualTransactionID(&aTransactionID);
        
    	if( actualTransID != KErrNotFound )
    		{
        	iConnection.CancelRequest(actualTransID);
    		}
    	else
        	{
	    	TMessage message = ipSenServiceDispatcher->GetMessageFromQueue(aTransactionID);
	    	CSenAsyncOperation* pAsyncOp = message.iSenAsyncOperation;
	    	if (pAsyncOp)
	    	    {
	            TInt idx(-1);
	            TRAP( retVal,
					idx = AsyncOpsArrayL().Find(pAsyncOp);
		            if (idx >= 0)
		                {
		                AsyncOpsArrayL().Remove(idx);
		                }
				  );
	    	    pAsyncOp->iActive = NULL;
    	    
    	    pAsyncOp->Cancel();
            delete pAsyncOp;
            pAsyncOp = NULL;
            }
    	ipSenServiceDispatcher->RemoveFromQueue(aTransactionID);
    	TRAP(retVal, DeliverResponseL(KErrSenCancelled,NULL));
    	if( retVal != KErrNone)
    		{
    		TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::CancelTransaction returns [%d]" ), retVal ));
    		return retVal;
    		}
    	}  
        }
    else // DISPATCHER IS DISABLED
        {
        iConnection.CancelRequest(aTransactionID);
        }
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::CancelTransaction returns [%d]" ), KErrNone ));
    return KErrNone;
    }
        
TInt CSenServiceConnectionImpl::TransportPropertiesL(HBufC8*& aProperties)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::TransportPropertiesL");

    TInt retVal(KErrSenNotInitialized);
    
    if(iInitialized)
        {
        CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkName());
        pSenChunk->SetLogger(Log());
        retVal = pSenChunk->CreateChunk();
        if (retVal == KErrNone)
            {
            retVal = iConnection.TransportPropertiesL(*pSenChunk);
            if(retVal == KErrNone)
                {
                TPtrC8 transportProperties;
                retVal = pSenChunk->DescFromChunk(transportProperties);
                if(retVal == KErrNone)
                    {
                    aProperties = transportProperties.AllocL();
                    }
                }
            }
        CleanupStack::PopAndDestroy(pSenChunk);
        }
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::TransportPropertiesL Completed");
    return retVal;
    }

RChunkMap& CSenServiceConnectionImpl::ChunkMapL()
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::ChunkMapL()")));
    if(!iChunkMap)
        {
        iChunkMap = new (ELeave) RChunkMap(ETrue, ETrue);
        }
    return *iChunkMap;
    }
    
RPointerArray<CSenAsyncOperation>& CSenServiceConnectionImpl::AsyncOpsArrayL() //codescannerwarnings
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::AsyncOpsArrayL()")));
    if(!iAsyncOpsArray)
        {
        iAsyncOpsArray = new (ELeave) RPointerArray<CSenAsyncOperation>;
        }
    return *iAsyncOpsArray;
    }

TInt CSenServiceConnectionImpl::SendL( const TDesC8& aRequest,
                                       const TDesC8& aProperties )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL");

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
        if( pOperation )
	        {
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif // _SENDEBUG
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = pOperation->DescsToChunk(aRequest, aProperties);
	            if (retVal == KErrNone)
	                {
	                pOperation->ChunkHeader().SetPropertiesType(MSenProperties::ESenTransportProperties);
	                retVal = SendL(pOperation);
	                }
	            }
	        }
		else
			{
#ifdef _SENDEBUG 
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(TDesC8& aRequest,TDesC8& aProrperties)- KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
            }
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SendL - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::SubmitL( const TDesC8& aRequest,
                                         const TDesC8& aProperties,
                                         HBufC8*& aResponse )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL");
    TInt retVal = CheckConnection();
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL(NextChunkName(), iIsInternalSC ); // this will return NULL in H/W targets if no chunks available (uses WSD)
		if( pOperation )
			{
	        CleanupStack::PushL( pOperation );
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif // _SENDEBUG
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = pOperation->DescsToChunk(aRequest, aProperties);
	            if (retVal == KErrNone)
	                {
	                pOperation->ChunkHeader().SetPropertiesType(MSenProperties::ESenTransportProperties);
	                retVal = SubmitL(*pOperation);
	                if (retVal != KErrNone && retVal != KErrSenSoapFault)
	                    {
	                    aResponse = NULL;
	                    }
	                else
	                    {
	                    retVal = ResponseFromChunk(*pOperation, aResponse);
	                    }
	                }
	            }
	        CleanupStack::PopAndDestroy(pOperation);
			}
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SubmitL - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL Completed");
    return retVal;
    }

 TInt CSenServiceConnectionImpl::HttpMethodFromPropertiesL( const TDesC8& aProperties,
                                                            CSenHttpTransportProperties::TSenHttpMethod& aMethod )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::HttpMethodFromPropertiesL");
    TInt retVal(KErrNotFound);
    if ( aProperties.Length() > 0 && aProperties.Find(KHttpMethodLocalName)!= KErrNotFound && 
        (aProperties.Find(KHttpGet)!= KErrNotFound || aProperties.Find(KHttpDelete)!= KErrNotFound) ) 
        {
        // Properties strings are found, now use CSenHttpProperties instance to validate that HTTP
        // method really is either HTTP GET or HTTP DELETE:
        CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC(); // push tp
        
        // Parse aProperties into HTTP transport properties object:
        TInt leaveCode( KErrNone );
        TRAP( leaveCode, pHttpProperties->ReadFromL( aProperties ); )
        if ( leaveCode == KErrNone )
            {
            CSenHttpTransportProperties::TSenHttpMethod method;
            retVal = pHttpProperties->HttpMethodL( method );
            if ( retVal == KErrNone )
                {
                aMethod = method;
                }
#ifdef _SENDEBUG            
            else 
                {
                // HttpMethodL failed:
                TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- HttpMethodL call failed. Error code: (%d)"), retVal));
                }
#endif // _SENDEBUG            
            }
#ifdef _SENDEBUG            
        else 
            {
            // ReadFromL leaved:
            TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Reading (parsing) of HTTP properties failed. Leave code: (%d)"), leaveCode));
            
            }
#endif // _SENDEBUG            
        CleanupStack::PopAndDestroy( pHttpProperties ); // de-alloc tp
        }
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::HttpMethodFromPropertiesL Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::SendL( CSenSoapEnvelope& aMessage, const TDesC8& aProperties )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP)");

    CSenHttpTransportProperties::TSenHttpMethod httpMethod;
    TInt readHttpMethodRetVal = HttpMethodFromPropertiesL( aProperties, httpMethod );

    if ( readHttpMethodRetVal == KErrNone && 
         (httpMethod == CSenHttpTransportProperties::ESenHttpGet ||
         httpMethod == CSenHttpTransportProperties::ESenHttpDelete ))
        {
        // At the moment, Service Connection does not support sending 
        // of SOAP-ENVs via HTTP headers for GET/DELETE requests:
        return KErrNotSupported; // documented in public header(s)
        }

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
		if( pOperation )
			{
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif // _SENDEBUG
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = AssignPropertiesToChunkL( *pOperation, aMessage, aProperties );	//codescannerwarnings
	            if (retVal == KErrNone)
	                {
	                pOperation->ChunkHeader().SetMessageType(MSenMessage::ESoapMessage);
	                retVal = SendL(pOperation);
	                }
	            }
			}
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
            }
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SendL(SOAP) - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP) Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::SubmitL( CSenSoapEnvelope& aMessage, const TDesC8& aProperties, HBufC8*& aResponseTo)
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL");

    CSenHttpTransportProperties::TSenHttpMethod httpMethod;
    TInt readHttpMethodRetVal = HttpMethodFromPropertiesL( aProperties, httpMethod );

    if ( readHttpMethodRetVal == KErrNone && 
         (httpMethod == CSenHttpTransportProperties::ESenHttpGet ||
         httpMethod == CSenHttpTransportProperties::ESenHttpDelete ))
        {
        // At the moment, Service Connection does not support sending 
        // of SOAP-ENVs via HTTP headers for GET/DELETE requests:
        return KErrNotSupported; // documented in public header(s)
        }

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
    	if( pOperation )
			{
			CleanupStack::PushL( pOperation );
#ifdef _SENDEBUG
	        pOperation->SetLogger( &iLog );
#endif // _SENDEBUG
	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            retVal = AssignPropertiesToChunkL(*pOperation, aMessage, aProperties);	//codescannerwarnings
	            if (retVal == KErrNone)
	                {
	                pOperation->ChunkHeader().SetMessageType(MSenMessage::ESoapMessage);
	                retVal = SubmitL(*pOperation);
	                if (retVal != KErrNone && retVal != KErrSenSoapFault)
	                    {
	                    aResponseTo = NULL;
	                    }
	                else
	                    {
	                    retVal = ResponseFromChunk(*pOperation, aResponseTo);
	                    }
	                }
	            }
	        CleanupStack::PopAndDestroy(pOperation);
			}
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL( CSenSoapEnvelope&, const TDesC8&, HBufC8*&) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::AssignPropertiesToChunkL(CSenChunk& aChunk,
                                                        CSenSoapEnvelope& aMessage,
                                                        const TDesC8& aProperties)	//codescannerwarnings
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::AssignPropertiesToChunkL()")));
    TInt retVal(KErrNone);
    
    if ( aMessage.SoapAction2().Length() > 0 )
        {
        CSenTransportProperties* pProps = CSenTransportProperties::NewLC();
        if ( aProperties.Length() > 0 )
            {
            pProps->ReadFromL(aProperties);
            }
        // This is performed to achieve two things:
        // 1) define HTTP header for SOAP 1.1 binding
        // 2) perform step (1) without using HTTP TP, so that each transport plug-in can still parse this value..
        pProps->SetPropertyL(KSenSoapActionHeaderName, aMessage.SoapAction2(), KHttpHeaderType);

        HBufC8* pPropsAsXml = pProps->AsUtf8LC();
        HBufC8* pSoapMsg = aMessage.AsXmlL();
        retVal = aChunk.DescsToChunk(*pSoapMsg, *pPropsAsXml);
        aChunk.ChunkHeader().SetPropertiesType(MSenProperties::ESenTransportProperties);
        delete pSoapMsg;
        CleanupStack::PopAndDestroy(pPropsAsXml);
        CleanupStack::PopAndDestroy(pProps);
        }
    else
        {
        HBufC8* pSoapMsg = aMessage.AsXmlL();
        if ( aProperties.Length() > 0 )
            {
            retVal = aChunk.DescsToChunk(*pSoapMsg, aProperties);
            aChunk.ChunkHeader().SetPropertiesType(MSenProperties::ESenTransportProperties);
            }
        else
            {
            retVal = aChunk.DescToChunk(*pSoapMsg);
            }
        delete pSoapMsg;
        }    
		TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::AssignPropertiesToChunkL returns [%d]" ), retVal ));
    return retVal;
    }
    
TInt CSenServiceConnectionImpl::AssignMessageToChunkL( CSenChunk& aChunk,
                                                      MSenMessage& aMessage ) //codescannerwarnings
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::AssignMessageToChunk");
    TInt retVal(KErrNone);
    
    CSenTransportProperties* pTempTransportProperties = NULL;
    
    MSenProperties* pProperties = aMessage.Properties();
    if( !pProperties )
        {
        // use transport properties in order to serialize SOAP action
        pTempTransportProperties = CSenTransportProperties::NewLC();
        pProperties = pTempTransportProperties;
        }

    if ( aMessage.IsSafeToCast( MSenMessage::ESoapEnvelope2 ) )
        {
        CSenSoapEnvelope2& message = (CSenSoapEnvelope2&)aMessage;
        if ( message.SoapAction().Length() > 0 )
            {
            if( pProperties->IsSafeToCast( MSenProperties::ESenXmlProperties ) )
                {
                CSenXmlProperties* pConcreteProps = (CSenXmlProperties*)pProperties;
                pConcreteProps->SetPropertyL(KSenSoapActionHeaderName, message.SoapAction(), KHttpHeaderType);
                }
            }
        }

    HBufC8* pPropsAsXml = pProperties->AsUtf8L();
    
    if( pTempTransportProperties )
        {
        CleanupStack::PopAndDestroy( pTempTransportProperties );
        }
    CleanupStack::PushL(pPropsAsXml);

    RBuf8 buffer;
    CleanupClosePushL( buffer );        
	CXmlEngSerializer* pSerializer(NULL);
	if ( aMessage.IsSafeToCast( MSenMessage::ESoapEnvelope2 ) )
	    {
	    pSerializer = CXmlEngSerializer::NewL(ESerializerXOPInfoset);
	    }
	else
	    {
	    pSerializer = CXmlEngSerializer::NewL(ESerializerDefault);
	    }
	CleanupStack::PushL( pSerializer );
	
    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    
    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;
    
    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

	pSerializer->SetOutput(buffer);
	pSerializer->SetSerializationOptions(options);
	
    RSenDocument document;
    if (aMessage.IsSafeToCast( MSenMessage::ESoapEnvelope2))
        {
        CSenSoapEnvelope2& message = (CSenSoapEnvelope2&)aMessage;
        document = message.AsDocumentL();
        }
    else if(aMessage.IsSafeToCast( MSenMessage::EAtomMessage))
        {
        CSenAtomMessage& message = (CSenAtomMessage&)aMessage;
        document = message.AsDocumentL();
        }
    else
        {
        return KErrNotSupported;
        }
	TInt leaveCode(KErrNone);
	
	TRAP( leaveCode, pSerializer->SerializeL(document); )

    if( leaveCode != KErrNone )
        {
        retVal = leaveCode;
        }
            
    if( retVal == KErrNone )
        {

#ifdef _SENDEBUG
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"-----------------------------------------------------------------------------------");
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"- OK. Message about to send:");
        TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,( buffer.Right(100) ));
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"- Transport properties:");
        if( pPropsAsXml )
            {
            TPtrC8 properties = pPropsAsXml->Des();
            TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,( properties.Right(100) ));
            //LOG_WRITEALL(( properties ));
            }
        TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMaxLogLevel ,"-----------------------------------------------------------------------------------");
#endif // _SENDEBUG        

        retVal = aChunk.DescsToChunk( buffer, *pPropsAsXml );
        aChunk.ChunkHeader().SetPropertiesType(MSenProperties::ESenTransportProperties);
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- Failed, error code: %d"), retVal ));
        }
#endif // _SENDEBUG        
        

    CleanupStack::PopAndDestroy(pSerializer);
    CleanupStack::PopAndDestroy(&buffer);
    CleanupStack::PopAndDestroy(pPropsAsXml);
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::AssignMessageToChunkL returns [%d]" ), retVal ));
    return retVal;
    }
    
// THIS IS THE SECOND, __MUCH IMPROVED__ VARIANT, WHICH SUPPORTS BOTH CHUNK AND FILE ATTACHEMENTS,
// plus multiple BLOBs PER ONE MESSAGE AND multiple simultaneous transfers
TInt CSenServiceConnectionImpl::MoveBinaryContainersToServer( CSenConnectionChunk& aOperation, RArray<TXmlEngDataContainer>& aList )
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::MoveBinaryContainersToServer()")));
    TInt retVal(KErrNone);
    
    TPtrC8 cid8;
    TBuf8<100> cid(KNullDesC8);
    TInt size;
    TInt offset;

    // First check if we are resending this operation (message). If so, operation has already "parsed" the blob
    // Proper solution that allows sending of multiple BLOBS, of various types:
    
    TInt containerCount( aOperation.ContainerCount() );
    if ( containerCount > 0 )
		{
		RBinaryMap& map = aOperation.ContainerMap();
		TBool noError(ETrue);
		for (TInt i = 0; i < containerCount && noError; i++)
		    {
		    TDesC8* pCid = map.KeyAt(i);
		    const CSenBinaryContainer* pContainer = map.ValueAt(i);
		    if( pCid && pContainer )
		        {
		        TPtrC8 cid = pContainer->Cid();
		        CSenBinaryContainer::TDataType type = pContainer->Type();
		        switch( type )
		            {
	                case CSenBinaryContainer::EFile:
	                    {
	                    RFile file = pContainer->File();
                        retVal = iConnection.MoveFile( aOperation, cid, file );
                        if ( retVal != KErrNone )
                            {
                            noError = EFalse;
                            break;
                            }                        
	                    }
	                break;
	                
	                case CSenBinaryContainer::EChunk:
	                    {
	                    RChunk chunk = pContainer->Chunk();
	                    TInt size = pContainer->ChunkSize();
	                    TInt offset = pContainer->ChunkOffset();
                        retVal = iConnection.MoveChunk( aOperation, cid, chunk, size, offset );
                        if ( retVal != KErrNone )
                            {
                            noError = EFalse;
                            break;
                            }                        
	                    }
	                break;
	                default:
	                // do nothing
	                break;
		            }
		        }
		    }
		}
    else
        {
        // this is the first time that we are movign binary containers to the server(!)
    	TInt count = aList.Count();	
    	for(TInt i = 0; i < count /* && retVal == KErrNone*/; i++)
    		{
    		if ( aList[i].NodeType() == TXmlEngNode::EChunkContainer )
    			{
    			cid = aList[i].Cid();
    			size = aList[i].Size();
    			offset = aList[i].AsChunkContainer().ChunkOffset();
    			RChunk& chunk = aList[i].AsChunkContainer().Chunk();
    			aOperation.AddChunkToContainerMap( cid, chunk, size, offset );
    			cid8.Set(cid.Ptr(), cid.Length());
                retVal = iConnection.MoveChunk( aOperation, cid8, chunk, offset, size );
                
                if ( retVal != KErrNone )
                    {
                    break;
                    }
    			}
    		else if( aList[i].NodeType() == TXmlEngNode::EFileContainer )
    			{
    			cid = aList[i].Cid();
    			RFile& file = aList[i].AsFileContainer().File();
    			
    			// Real solution that supports sending of multiple files / BLOBs
    			aOperation.AddFileToContainerMap( cid, file );
    			cid8.Set(cid.Ptr(), cid.Length());
                retVal = iConnection.MoveFile( aOperation, cid8, file );

                if ( retVal != KErrNone )
                    {
                    break;
                    }
    			}
    		}
     }
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::MoveBinaryContainersToServer returns [%d]" ), retVal ));
    return retVal;
    }
    
TInt CSenServiceConnectionImpl::MoveFileChunkHandleToServer(CSenConnectionChunk& aOperation)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::MoveFileChunkHandleToServer()")));
    TInt retVal(KErrNone);
    retVal = iConnection.SendFileHandle(aOperation,iSharedFileHandle);
    return retVal;
    }
    
    //Sending  Large File Throught RFile handle 
TInt CSenServiceConnectionImpl::SendL(RFile& aFile)
    {
  	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(RFile)");

	TInt retVal(KErrNone);
  	retVal = CheckConnection();
  	 if(retVal == KErrNone)
	    {
         if (aFile.SubSessionHandle() )
         {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL(NextChunkName()); 
        
        if( pOperation )
        	{
			#ifdef _SENDEBUG
			        pOperation->SetLogger(&iLog);
			#endif
			        retVal = pOperation->CreateChunk();
			        pOperation->HasSharedFileHandle(ETrue);
			        iSharedFileHandle = aFile;
			        if (retVal == KErrNone)	            
			        	{
						retVal = SendL(pOperation);	
			            }
        	}
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
			
        }
        else
        {
        	
			TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("MSenServiceConsumer::HandleMessageFromChildAOL - HandleErrorL leaves with %d"), retVal));
            retVal=KErrArgument;
        	}
         
        }
    
    
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SendL(SOAP) - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	

    return retVal;
      
    }
    
TInt CSenServiceConnectionImpl::SendL( MSenMessage& aMessage )
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::MoveFileChunkHandleToServer(aMessage)")));
    if ( aMessage.IsSafeToCast( MSenMessage::ESoapEnvelope2 ) )
        {
        CSenSoapEnvelope2& message = (CSenSoapEnvelope2&)aMessage;
        return SendL(message);
        }
    else
        {
        return KErrNotSupported;
        }
    }

    
TInt CSenServiceConnectionImpl::SendL( CSenSoapEnvelope2& aMessage )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(CSenSoapEnvelope2)");

    MSenProperties* pProperties = aMessage.Properties();
    if( pProperties && pProperties->IsSafeToCast( MSenProperties::ESenHttpTransportProperties ) )
        {
        CSenHttpTransportProperties& properties = (CSenHttpTransportProperties&)*pProperties;
        CSenHttpTransportProperties::TSenHttpMethod httpMethod;
        TInt readHttpMethodRetVal = properties.HttpMethodL( httpMethod );

        if ( readHttpMethodRetVal == KErrNone && 
             (httpMethod == CSenHttpTransportProperties::ESenHttpGet ||
             httpMethod == CSenHttpTransportProperties::ESenHttpDelete ))
            {
            // At the moment, Service Connection does not support sending 
            // of SOAP-ENVs via HTTP headers for GET/DELETE requests:
            return KErrNotSupported; // documented in public header(s)
            }
        }

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
	    if( pOperation )
	        {
#ifdef _SENDEBUG
        pOperation->SetLogger(&iLog);
#endif

        retVal = pOperation->CreateChunk();
        if (retVal == KErrNone)
            {
            pOperation->ChunkHeader().SetMessageType(aMessage.Type());
            
            // we can cast, condition is checked at the begin of that method
            RSenDocument document;
            if (aMessage.IsSafeToCast( MSenMessage::ESoapEnvelope2))
                {
                CSenSoapEnvelope2& message = (CSenSoapEnvelope2&)aMessage;
                document = message.AsDocumentL();
                }
            else if(aMessage.IsSafeToCast( MSenMessage::EAtomMessage))
                {
                CSenAtomMessage& message = (CSenAtomMessage&)aMessage;
                document = message.AsDocumentL();
                }
	        document.GetDataContainerList(pOperation->BinaryDataArrayL());	//codescannerwarnings
	        pOperation->RemoveBinaryDataArrayIfEmpty();
            
            retVal = AssignMessageToChunkL(*pOperation, aMessage);	//codescannerwarnings
            if (retVal == KErrNone)
                {
                retVal = SendL(pOperation);
                }
            }
#ifdef _SENDEBUG
			else
				{
			        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("-pOperation->CreateChunk Failed, error code: %d"), retVal ));
				}
#endif // _SENDEBUG
	        }
		else
			{
#ifdef _SENDEBUG        
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(SOAP-MSG2) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SendL(SOAP-MSG2) - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SendL(CSenSoapEnvelope2) Completed");
    return retVal;
    }

TInt CSenServiceConnectionImpl::SubmitL( MSenMessage& aMessage,
                                         CSenSoapEnvelope2*& aResponseTo )
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SubmitL(aMessage, aResponseTo)")));
    if ( aMessage.IsSafeToCast( MSenMessage::ESoapEnvelope2) )
        {
        CSenSoapEnvelope2& message = (CSenSoapEnvelope2&)aMessage;
        
        return SubmitL(message, aResponseTo);
        }
    else
        {
        return KErrNotSupported; 
        }
    }

TInt CSenServiceConnectionImpl::SubmitL( CSenSoapEnvelope2& aMessage, CSenSoapEnvelope2*& aResponseTo )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL(CSenSoapEnvelope2)");

    MSenProperties* pProperties = aMessage.Properties();
    if( pProperties && pProperties->IsSafeToCast( MSenProperties::ESenHttpTransportProperties ) )
        {
        CSenHttpTransportProperties& properties = (CSenHttpTransportProperties&)*pProperties;
        CSenHttpTransportProperties::TSenHttpMethod httpMethod;
        TInt getHttpMethodRetVal = properties.HttpMethodL( httpMethod );

        if ( getHttpMethodRetVal == KErrNone && 
             httpMethod == CSenHttpTransportProperties::ESenHttpGet || 
             httpMethod == CSenHttpTransportProperties::ESenHttpDelete )
            {
            // At the moment, Service Connection does not support sending 
            // of SOAP-ENVs via HTTP headers for GET/DELETE requests:
            return KErrNotSupported; // documented in public header(s)
            }
        }

    TInt retVal = CheckConnection();
    
    if(retVal == KErrNone)
        {
        CSenConnectionChunk* pOperation = CSenConnectionChunk::NewL( NextChunkName(), iIsInternalSC ); // returns NULL in hardware env (uses WSD in h/w, too), if no server chunk slot available
        if( pOperation )
        	{
	        CleanupStack::PushL( pOperation );
	        
	#ifdef _SENDEBUG
	        pOperation->SetLogger(&iLog);
#endif // _SENDEBUG

	        retVal = pOperation->CreateChunk();
	        if (retVal == KErrNone)
	            {
	            pOperation->ChunkHeader().SetMessageType(aMessage.Type());
	            
	            RSenDocument document = aMessage.AsDocumentL();
		        document.GetDataContainerList(pOperation->BinaryDataArrayL());	//codescannerwarnings
		        pOperation->RemoveBinaryDataArrayIfEmpty();
	            
	            retVal = AssignMessageToChunkL(*pOperation, aMessage);	//codescannerwarnings
	            if (retVal == KErrNone)
	                {
	                retVal = SubmitL(*pOperation);
	                
	                if (retVal != KErrNone && retVal != KErrSenSoapFault)
	                    {
	                    aResponseTo = NULL;
	                    }
	                else
	                    {
	                    HBufC8* pResponseBuf = NULL;
	                    retVal = ResponseFromChunk(*pOperation, pResponseBuf);
	                    if( retVal == KErrNone && pResponseBuf && pResponseBuf->Length()>0)
	                        {
	                        CleanupStack::PushL(pResponseBuf);
	                        CSenSoapEnvelope2* pResponseSoapEnv2 = CSenSoapEnvelope2::NewL();
	                        CleanupStack::PushL(pResponseSoapEnv2);
	                        
	                        CSenParser* pParser = CSenParser::NewLC();
	                    	TRAP( retVal, pParser->ParseL(*pResponseBuf, *pResponseSoapEnv2); )
	                    	CleanupStack::PopAndDestroy(pParser);

	                        if( retVal == KErrNone )
	                            {
	                            aResponseTo = pResponseSoapEnv2;
	                            // ownership was safely transferred to caller; remove pointer from cleanup stack:
	                            CleanupStack::Pop(pResponseSoapEnv2);
	                            }
	                        else
	                            {
	                            CleanupStack::PopAndDestroy(pResponseSoapEnv2);
	                            }
	                        
	                        CleanupStack::PopAndDestroy(pResponseBuf);
	                        }
	                    }
	                
	                }
	            }
	        CleanupStack::PopAndDestroy(pOperation);
        	}
		else
			{
#ifdef _SENDEBUG
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL(SOAP-MSG2) - KErrServerBusy");
#endif // _SENDEBUG
			return KErrServerBusy;    	
			}
        }
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SubmitL(SOAP-MSG2) - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG    	
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::SubmitL(CSenSoapEnvelope2) Completed");
    return retVal;
    }
    
TInt CSenServiceConnectionImpl::Identifier()
    {
    return iConnectionID;
    }
    
TInt CSenServiceConnectionImpl::RegisterFilesObserver()
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterFilesObserver");
    TInt retVal(KErrNone);
    if(!HasConnectionAgentCallbackInitialized())
    	{
	    const TDesC& name = CSenConnAgentServer::Open();
	    if (name == KNullDesC)
	        {
	        return KErrGeneral;
	        }
	    retVal = iConnection.RegisterTransferObserver(&name, iFilesObserver);
	    iConnectionAgentCallbackInitialized = ETrue;
    	}
    else
    	{
    	retVal = iConnection.RegisterTransferObserver(&KNullDesC(), iFilesObserver);
    	}
    iRegisterFileObserverDone = ETrue;
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterFilesObserver Completed");
    return retVal; 
    }
#ifdef __ENABLE_ALR__

TInt CSenServiceConnectionImpl::RegisterMobilityObserverL()
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterMobilityObserverL");
    TInt retVal(KErrNone);
    if(!HasConnectionAgentCallbackInitialized())
    	{
	    const TDesC& name = CSenConnAgentServer::Open();
	    if (name == KNullDesC)
	        {
	        User::Leave(KErrGeneral);
	        }
	    iConnection.RegisterMobilityObserverL(&name, iMobiltyObserver);
	    iConnectionAgentCallbackInitialized = ETrue;
    	}
    else
    	{
    	iConnection.RegisterMobilityObserverL(&KNullDesC(), iMobiltyObserver);
    	}    	
    iRegisterMobilityObserverDone = ETrue;
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterMobilityObserverL Completed");
    return retVal; 
    }


void CSenServiceConnectionImpl::MigrateToPrefferedCarrierL(TBool &aUserChoice)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::MigrateToPrefferedCarrierL()")));
    if (iMobiltyObserver)
        {
        User::LeaveIfError(iConnection.MigrateToPrefferedCarrierL(aUserChoice));    	
        }
	else
	    {
        User::Leave(KErrNotFound);
	    }	 
	  TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::MigrateToPrefferedCarrierL() Completed")));
    }

void CSenServiceConnectionImpl::NewCarrierAcceptedL(TBool &aUserChoice)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::NewCarrierAcceptedL()")));
    if (iMobiltyObserver)
        {
        User::LeaveIfError(iConnection.NewCarrierAcceptedL(aUserChoice));    	
        }
	else
	    {
        User::Leave(KErrNotFound);
	    }
	  TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::NewCarrierAcceptedL() Completed")));  	 
    }
#endif


TInt CSenServiceConnectionImpl::SendProgressToHostlet(TInt aTxnId,
        TBool aIncoming, const TDesC8& aMessage, const TDesC8& aCid,
        TInt aProgress)
    {
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SendProgressToHostlet()")));
    TBool isSoap = (aMessage != KNullDesC8);
    TPtrC8 ptr;
    if (isSoap)
        {
        ptr.Set(aMessage);
        }
    else
        {
        ptr.Set(aCid);
        }
    return iConnection.SendFileProgress(aTxnId, aIncoming, isSoap,ptr, aProgress);
    }

void CSenServiceConnectionImpl::DeliverStatus(TInt aStatus)
    {
    TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("Invoking SetStatus(%d) callback."), aStatus ));
    TLSLOG_FORMAT(( iTlsLogStatusChannel, KSenServiceConnectionStatusLogLevel, _L8("CSenServiceConnectionImpl::DeliverStatus [legacy status update] - Invoking SetStatus(%d)"), aStatus ));
    
    TInt leaveCode(KErrNone);

    if( ipHostletConsumer )
        {
        ipHostletConsumer->SetConnectionId( iConnectionID );
        }
    
    TRAP( leaveCode, iObserver->SetStatus(aStatus); )
    if( leaveCode )
        {
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("Fatal(!) -- CSenServiceConnectionImpl::DeliverStatus(): MSenServiceConsumer::SetStatus - leaves with %d"), leaveCode));
        TLSLOG_FORMAT(( iTlsLogStatusChannel, KSenServiceConnectionStatusLogLevel, _L("Fatal(!) -- CSenServiceConnectionImpl::DeliverStatus(): MSenServiceConsumer::SetStatus - leaves with %d"), leaveCode));
        leaveCode = KErrNone;
        }
    TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::DeliverStatus() Completed")));
    }
    
    
 TInt CSenServiceConnectionImpl::IdentityProviderL( CSenIdentityProvider*& apIdentityProvider )
    {
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::IdentityProviderL");
    
    delete apIdentityProvider;
    apIdentityProvider = NULL;

    TInt retVal = CheckConnection();
    if(retVal == KErrNone)
        {
        TInt retVal(KErrNone);
        
        CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkName());
        pSenChunk->SetLogger(Log());
        retVal = pSenChunk->CreateChunk();
        
        if (retVal == KErrNone)
            {
            retVal = iConnection.IdentityProviderL(*pSenChunk);
            if(retVal == KErrNone)
                {
                TPtrC8 identityProvider;
                retVal = pSenChunk->DescFromChunk(identityProvider);
                if(retVal == KErrNone)
                    {
                    apIdentityProvider = CSenIdentityProvider::NewLC(KNullDesC8);
                    CSenXmlReader* pXmlReader = CSenXmlReader::NewLC(KXmlParserMimeType); // use libxml2 sax parser
                    pXmlReader->SetContentHandler( *apIdentityProvider );
                    apIdentityProvider->SetReader(*pXmlReader);
    
                    TInt leaveCode(KErrNone);
                    TRAP( leaveCode, apIdentityProvider->ParseL( identityProvider ); )
                    
                    CleanupStack::PopAndDestroy(pXmlReader);
                    
                    if ( leaveCode != KErrNone )
                        {
                        CleanupStack::PopAndDestroy( apIdentityProvider );
                        apIdentityProvider = NULL;
                        }
                    else
                        {
                        CleanupStack::Pop( apIdentityProvider );
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy(pSenChunk);
        }    
#ifdef _SENDEBUG        
    else
    	{
        // error occured in CheckConnection()  	
        TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::IdentityProviderL - CheckConnection failed, error(%d)"), retVal ));
    	}	 
#endif // _SENDEBUG 
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::IdentityProviderL Completed");   	
    return retVal;    
    }

TInt CSenServiceConnectionImpl::SearchIdentityProviderL( CSenIdentityProvider*& apIdentityProvider, const TDesC8& aProviderId )
			{
			TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::SearchIdentityProviderL()")));
	    TInt retVal(KErrNone);
	    
	    CSenChunk* pSenChunk = CSenChunk::NewLC(NextChunkName());
	    pSenChunk->SetLogger(Log());
	    retVal = pSenChunk->CreateChunk();
	    
	    if (retVal == KErrNone)
	        {
	        retVal = iConnection.SearchIdentityProviderL(*pSenChunk, aProviderId);
	        if(retVal == KErrNone)
	            {
	            TPtrC8 identityProvider;
	            retVal = pSenChunk->DescFromChunk(identityProvider);
	            if(retVal == KErrNone)
	                {
	                apIdentityProvider = CSenIdentityProvider::NewLC(KNullDesC8);
	                CSenXmlReader* pXmlReader = CSenXmlReader::NewLC(KXmlParserMimeType); // use libxml2 sax parser
	                pXmlReader->SetContentHandler( *apIdentityProvider );
	                apIdentityProvider->SetReader(*pXmlReader);

	                TInt leaveCode(KErrNone);
	                TRAP( leaveCode, apIdentityProvider->ParseL( identityProvider ); )
	                
	                CleanupStack::PopAndDestroy(pXmlReader);
	                
	                if ( leaveCode != KErrNone )
	                    {
	                    CleanupStack::PopAndDestroy( apIdentityProvider );
	                    apIdentityProvider = NULL;
	                    }
	                else
	                    {
	                    CleanupStack::Pop( apIdentityProvider );
	                    }
	                }
	            }
	        }
	    CleanupStack::PopAndDestroy(pSenChunk);
			TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::SearchIdentityProviderL returns [%d]" ), retVal ));
	    return retVal; 
		}
 
TInt CSenServiceConnectionImpl::RegisterAuthenticationObserver()
	{
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterAuthenticationObserver");
	TInt retVal(KErrNone);
    if(!HasConnectionAgentCallbackInitialized())
      	{
	    const TDesC& name = CSenConnAgentServer::Open();
	    if (name == KNullDesC)
	        {
	        return KErrGeneral;
	        }
	    retVal = iConnection.RegisterAuthenticationObserver(&name, ipAuthProvider);
	    iConnectionAgentCallbackInitialized = ETrue;
       	}
    else
       	{
       	retVal = iConnection.RegisterAuthenticationObserver(&KNullDesC(), ipAuthProvider);
       	}
       iRegisterAuthObserverDone = ETrue;
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterAuthenticationObserver Completed");
    return retVal;
	}
TInt CSenServiceConnectionImpl::RegisterCoBrandingObserver()
{
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterCoBrandingObserver");
	TInt retVal(KErrNone);
    if(!HasConnectionAgentCallbackInitialized())
    	{
	    const TDesC& name = CSenConnAgentServer::Open();
	    if (name == KNullDesC)
	        {
	        return KErrGeneral;
	        }
	    retVal = iConnection.RegisterCoBrandingObserver(&name, ipCoBrandingObserver);
	    iConnectionAgentCallbackInitialized = ETrue;
    	}
    else
    	{
    	retVal = iConnection.RegisterCoBrandingObserver(&KNullDesC(), ipCoBrandingObserver);
    	}
    iRegisterCoBrandingObserverDone = ETrue;
    TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::RegisterCoBrandingObserver Completed");
    return retVal;    
}

TBool CSenServiceConnectionImpl::HasConnectionAgentCallbackInitialized()
	{
	TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::HasConnectionAgentCallbackInitialized()")));
#ifdef __ENABLE_ALR__
	if((ipAuthProvider || iFilesObserver || iMobiltyObserver || ipCoBrandingObserver) && iConnectionAgentCallbackInitialized)
#else
	if((ipAuthProvider || iFilesObserver || ipCoBrandingObserver) && iConnectionAgentCallbackInitialized)
#endif //__ENABLE_ALR__
		{
		return ETrue;
		}
	return EFalse;
	}

TInt CSenServiceConnectionImpl::ReauthNeededL(const TDesC8& aProviderId)
	{
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::ReauthNeededL");
	TInt retVal(KErrNotFound);
	if(ipAuthProvider)
		{
		TBool identityProviderCreated = EFalse;
		// Get identity from authentication callback
	    CSenIdentityProvider* pIdP =
	            (CSenIdentityProvider*)ipAuthProvider->IdentityProviderL();
	    
	    if ( !pIdP )
	        {
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - IdentityProviderL() from authentication provider returned NULL.");
	        const TPtrC8 userName = ipAuthProvider->UsernameL();
	        const TPtrC8 password = ipAuthProvider->PasswordL();
	        if ( userName != KNullDesC8 &&
	             password != KNullDesC8 )
	        	{
		        if ( &aProviderId != NULL && aProviderId.Length() > 0 )
		        	{
		        	TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::Reauthentication - trying to search identity provider with the given id: %S"), &aProviderId ));
		            retVal = SearchIdentityProviderL(pIdP, aProviderId);
		            if(retVal == KErrNone && pIdP)
		               	{
		               	CleanupStack::PushL(pIdP);
		               	identityProviderCreated = ETrue;
		               	pIdP->SetUserInfoL( userName ,
		               						userName ,
		               						password );
		                }
		        	}
		        else
		        	{
					TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - provider id not found, trying to retrieve connection identity provider.");
		        	// Try to retrieve IdentityProvider
		        	IdentityProviderL(pIdP);
		        	if(pIdP)
		        		{
		        		CleanupStack::PushL(pIdP);
		        		identityProviderCreated = ETrue;
		        		}
		        	else
		        		{
						TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - identity provider not found, creating one with default parameters.");
		        		// Create IdentityProvider with default parameters
		        		pIdP = CSenIdentityProvider::NewLC(KIdentityProviderName);
		        		identityProviderCreated = ETrue;
		        		}
		        	
			        if(pIdP)
			        	{
			        	pIdP->SetUserInfoL( userName ,
	   										userName ,
	   										password );
		        		}
		        	}
	            } // if(userName != KNull...)
	        else
	        	{
				TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - UserNameL() and/or PasswordL() from authentiction provider returned NULL.");
	        	retVal = KErrGeneral;
	        	}
	        } // if(!pIdP)
	    
	    if ( pIdP )
	        {
			TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::Reauthentication - CSenServiceConnectionImpl::Reauthentication - identity provider ready to register.");
	        HBufC8* pIdPAsXml = pIdP->AsXmlL();
	        CleanupStack::PushL(pIdPAsXml);
	
	        // Update IdentityProvider into DB
	        TPtr8 ptr = pIdPAsXml->Des();
	        retVal = iConnection.RegisterIdentityProvider(ptr);
	        
	        CleanupStack::PopAndDestroy(pIdPAsXml);
	        if ( identityProviderCreated )
	            {
	            CleanupStack::PopAndDestroy(pIdP);
	            }
	        }
		}
	TLSLOG_L(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel ,"CSenServiceConnectionImpl::ReauthNeededL Completed");
	return retVal;
	}
	
void CSenServiceConnectionImpl::DataTrafficDetails(TSenDataTrafficDetails& aDetails,
												   TSenDataTrafficOperations& aOperations) 
	{
	TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::DataTrafficDetails()")));
	iConnection.DataTrafficDetails(aDetails, aOperations);
	}	
	

TAny* CSenServiceConnectionImpl::InterfaceByUid( TUid aUID )
    {
    if ( aUID == KSenInterfaceUidInternalServiceConnection )
			{
			TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::InterfaceByUid(%d) == KSenInterfaceUidInternalServiceConnection" ), aUID.iUid ));
			// Must be cast to M-class  (as the same C-class implements multiple M-classes):
			MSenInternalServiceConnection* connection = (MSenInternalServiceConnection*) this;
			return connection; 
			}
	#ifdef __ENABLE_ALR__			
    else if ( aUID == KSenInterfaceUidAlrServiceConnection )
    	{
			TLSLOG_FORMAT((KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("CSenServiceConnectionImpl::InterfaceByUid(%d) == KSenInterfaceUidInternalServiceConnection" ), aUID.iUid ));
			MSenAlrServiceConnection* connection = NULL;
			// Must be cast to M-class  (as the same C-class implements multiple M-classes):
			connection = (MSenAlrServiceConnection*) this; // (mobility observer) is only available since S60 3.2 
			return connection; 
    	}
	#endif // __ENABLE_ALR__		
    return NULL;
    }

TInt CSenServiceConnectionImpl::PendingTrasanctionsCount()
	{
	TLSLOG(KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel,(_L("CSenServiceConnectionImpl::PendingTrasanctionsCount()")));
	return iAsyncOpsArray->Count();
	}										

CSenAsyncOperation* CSenAsyncOperation::NewL(CSenServiceConnectionImpl* aActive)
    {
    CSenAsyncOperation* pNew = NewLC(aActive);
    CleanupStack::Pop();
    return(pNew);
    }

CSenAsyncOperation* CSenAsyncOperation::NewLC(CSenServiceConnectionImpl* aActive)
    {
    CSenAsyncOperation* pNew = new (ELeave) CSenAsyncOperation(aActive);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
    
CSenAsyncOperation::CSenAsyncOperation(CSenServiceConnectionImpl* aActive)
:   CActive(EPriorityNormal),
    iActive(aActive),
    iErrorNumber(0),
    iErrorNumberBuffer(NULL, 0),
    iTxnId(0),
    iTxnIdBuffer(NULL, 0),
    iReauthTxnId(0)
    {
    CActiveScheduler::Add(this);
    }

void CSenAsyncOperation::ConstructL()
    {
    iErrorNumberBuffer.Set(reinterpret_cast<TUint8*>(&iErrorNumber),
                           sizeof(TInt),
                           sizeof(TInt));
                           
    iTxnIdBuffer.Set(reinterpret_cast<TUint8*>(&iTxnId),
                     sizeof(TInt),
                     sizeof(TInt));
                     
    if(!IsActive())
    	SetActive();
	iStatus = KRequestPending;
#ifdef EKA2
    iActive->AsyncOpsArrayL().AppendL(this);	//codescannerwarnings
#else
    User::LeaveIfError(iActive->AsyncOpsArrayL().Append(this));	//codescannerwarnings
#endif
    }
    
CSenAsyncOperation::~CSenAsyncOperation()
    {
    if ( IsActive() )
        {
        Cancel(); // calls CSenAsyncOperation::DoCancel()
        }
    }

void CSenAsyncOperation::RunL()
    {
    if ( iActive )
        {
        iActive->iErrorNumber = iErrorNumber;
        if( iReauthTxnId != 0 && iTxnId == 0 )
        	{
        	iActive->iTxnId = iReauthTxnId;
        	iActive->iReauthenticationNeeded = ETrue;
        	}
        else
        	{
        	iActive->iTxnId = iTxnId;
        	}  
        iActive->HandleMessageFromChildAOL(iStatus.Int());
            
        TInt idx = iActive->AsyncOpsArrayL().Find(this);	//codescannerwarnings
        if (idx >= 0)
            {
            iActive->AsyncOpsArrayL().Remove(idx);	//codescannerwarnings
            }
        }
    
    delete this;
    }
        
void CSenAsyncOperation::DoCancel()
    {
    }


CSenConnectionStatusObserver* CSenConnectionStatusObserver::NewL(MSenServiceConsumer& aObserver,
                                                                 TInt aConnectionID)
    {
    CSenConnectionStatusObserver* pNew = NewLC(aObserver, aConnectionID);
    CleanupStack::Pop();
    return(pNew);
    }

CSenConnectionStatusObserver* CSenConnectionStatusObserver::NewLC(MSenServiceConsumer& aObserver,
                                                                  TInt aConnectionID)
    {
    CSenConnectionStatusObserver* pNew = new (ELeave) CSenConnectionStatusObserver(aObserver,
                                                                                   aConnectionID);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
    
CSenConnectionStatusObserver::CSenConnectionStatusObserver(MSenServiceConsumer& aObserver,
                                                           TInt aConnectionID)
:   CActive(EPriorityMore),
    iObserver(&aObserver),
    iConnectionID(aConnectionID)
    {}

void CSenConnectionStatusObserver::ConstructL()
    {
    RThread thread;
    TSecureId secureID = thread.SecureId();
    

    _LIT_SECURITY_POLICY_PASS(KReadPropPassAll);
    _LIT_SECURITY_POLICY_S0(KWritePropSenCoreSidOnlyPolicy, KServerUid3.iUid);
    
    // Default category value is the secure id of the loading process 
    // Define the property
    TInt err = RProperty::Define( // Secureid of the process will be used
                                  iConnectionID,
                                  RProperty::EInt,
                                  KReadPropPassAll,
//                                  KReadPropThisSidOnlyPolicy,
                                  KWritePropSenCoreSidOnlyPolicy);

	// Use secure id of the loading process instead of server process (ws client process secure id)
	User::LeaveIfError(iConnectionStatusProperty.Attach(secureID, iConnectionID)); // secureid of the process will be category
    CActiveScheduler::Add(this);
    // Initial subscription
    iConnectionStatusProperty.Subscribe(iStatus);
    if(!IsActive())
    	SetActive();
	iStatus = KRequestPending;
    }

CSenConnectionStatusObserver::~CSenConnectionStatusObserver()
    {
    Cancel();
    iConnectionStatusProperty.Close();
    // Use secure id of the loading process instead of server process (ws client process secure id) 
    RThread thread;
    TSecureId secureID = thread.SecureId();    
    TInt err = RProperty::Delete( secureID, iConnectionID );
    }

void CSenConnectionStatusObserver::DoCancel()
    {
    iConnectionStatusProperty.Cancel();
    }

void CSenConnectionStatusObserver::RunL()
    {
        
    TLSLOG_L( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenConnectionStatusObserver::RunL" );
    iConnectionStatusProperty.Subscribe(iStatus);
    if(!IsActive())
    	SetActive();
	iStatus = KRequestPending;
   
    TInt propertyValue(KErrNotFound);
    TInt getPropertyCode = iConnectionStatusProperty.Get(propertyValue);
    if ( getPropertyCode == KErrNone)
        {
#if !defined ( RD_SEN_DISABLE_TRANSPORT_STATUS_CODE_CALLBACK )
        TLSLOG_FORMAT(( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("CSenConnectionStatusObserver::RunL [new value published] - Calling SetStatus(%d)"), propertyValue ));
        iObserver->SetStatus( propertyValue ); // Note: client drop 2007-01-12 cannot handle foreign status codes 
#endif        
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT(( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("- MAJOR: iConnectionStatusProperty.Get(propertyValue) failed: %d"), getPropertyCode ));
        }
#endif // _SENDEBUG        
    }
   
// ------------------------------------------------------------------------------------------------------------

CSenFileProgressObserver* CSenFileProgressObserver::NewL( MSenFilesObserver& aObserver,
                                                          TInt aConnectionID, 
                                                          TBool aIsDispatcherEnabled,
                                                          CSenServiceDispatcher* apSenServiceDispatcher, 
                                                          RChunkMap& aChunkMap )
    {
    CSenFileProgressObserver* pNew = NewLC( aObserver, aConnectionID, aIsDispatcherEnabled, apSenServiceDispatcher,aChunkMap );
    CleanupStack::Pop();
    return(pNew);
    }

CSenFileProgressObserver* CSenFileProgressObserver::NewLC( MSenFilesObserver& aObserver,
                                                           TInt aConnectionID,
                                                           TBool aIsDispatcherEnabled,
                                                           CSenServiceDispatcher* apSenServiceDispatcher, 
                                                           RChunkMap& aChunkMap )
    {
    CSenFileProgressObserver* pNew = new (ELeave) CSenFileProgressObserver( aObserver, aConnectionID, aIsDispatcherEnabled, apSenServiceDispatcher, aChunkMap );
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
    
    

CSenFileProgressObserver::CSenFileProgressObserver( MSenFilesObserver& aObserver,
                                                    TInt aConnectionID,
                                                    TBool aIsDispatcherEnabled,
                                                    CSenServiceDispatcher* apSenServiceDispatcher, 
                                                    RChunkMap& aChunkMap )
//:    CActive(EPriorityNormal),
:   CActive(EPriorityMore),
    iFileProgressObserver(&aObserver),
    iConnectionID(aConnectionID),
    iIsDispatcherEnabled(aIsDispatcherEnabled),
    ipSenServiceDispatcher(apSenServiceDispatcher),
    iChunkMap(aChunkMap)
    {}

void CSenFileProgressObserver::ConstructL()
    {
    _LIT_SECURITY_POLICY_PASS(KReadPropPassAll);
    _LIT_SECURITY_POLICY_S0(KWritePropSenCoreSidOnlyPolicy, KServerUid3.iUid);
    
    // Define the property
    TInt err = RProperty::Define( KSenInterfaceUidFilesObserver,
                                  iConnectionID,
                                  RProperty::ELargeByteArray, 
                                  KReadPropPassAll,
                                  KWritePropSenCoreSidOnlyPolicy,
                                  512 );
	// Use KSenInterfaceUidFilesObserver extended consumer interface UID
	User::LeaveIfError(iFileProgressProperty.Attach(KSenInterfaceUidFilesObserver,  iConnectionID)); // KSenInterfaceUidFilesObserver UID will be category
    CActiveScheduler::Add(this);
    // Initial subscription
    iFileProgressProperty.Subscribe(iStatus);
    if(!IsActive())
    	SetActive();
    iStatus = KRequestPending;
    }

CSenFileProgressObserver::~CSenFileProgressObserver()
    {
    Cancel();
    iFileProgressProperty.Close();
    
    TInt err = RProperty::Delete( KSenInterfaceUidFilesObserver, iConnectionID );
    
    }

void CSenFileProgressObserver::DoCancel()
    {
    iFileProgressProperty.Cancel();
    }

void CSenFileProgressObserver::RunL()
    {
    // Resubscribe before processing new value to prevent missing updates(!):
	iFileProgressProperty.Subscribe(iStatus);
    if(!IsActive())
    	SetActive();
    iStatus = KRequestPending;

    TLSLOG_L( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenFileProgressObserver::RunL" );
       
    TPckgBuf<TFileOutgoingTransferProgressBase> progress;
    TInt getPropertyCode = iFileProgressProperty.Get( progress );
    if ( getPropertyCode == KErrNone )
        {
        TLSLOG_L(KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenFileProgressObserver::RunL [new value published] - Calling TransferProgress");
        TFileOutgoingTransferProgressBase data = progress();

        TInt index(KErrNotFound);
        if ( iIsDispatcherEnabled ) // DISPATCHER IS ENABLED
            {
            TLSLOG_L(KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenFileProgressObserver::RunL : SC's message dispatcher is +enabled+.");
            TInt keyAt = ipSenServiceDispatcher->GetVirtualTransactionID( &data.iTxnId );
            if( keyAt != KErrNotFound )
                {
                TInt index = iChunkMap.Find( keyAt );
                }
            }
        else // DISPATCHER IS DISABLED
            {
            TLSLOG_L(KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenFileProgressObserver::RunL : SC's message dispatcher is -disabled-.");
            index = iChunkMap.Find( data.iTxnId );
            }

        // Check from the chunk (allocated by the service connection implemenation in first place),
        // WHETHER THIS TRANSACTION HAS ALREADY BEEN COMPLETED(!). There is no need to report on
        // "file progress" if the the transaction is already completed / over.
        if ( index != KErrNotFound )
            {
            CSenConnectionChunk* pOperation = (CSenConnectionChunk*)iChunkMap.ValueAt(index);
            TLSLOG_FORMAT(( KSenServiceConnectionLogChannelBase+iConnectionID, KMinLogLevel , _L8("- RChunk's handle: %d"), pOperation->Chunk().Handle() ));

            if( !data.iIsIncoming && pOperation->ChunkHeader().MessageDirection() != MSenMessage::EOutbound )
                {
                TLSLOG_FORMAT(( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("CSenFileProgressObserver::RunL : the transaction (txnID: %d) is already completed, no need to signalize 'progress'"), data.iTxnId ));
                return; 
                }
            }
            
        //TBuf8<128> cid;
        HBufC8* pCid = HBufC8::NewLC( KMaxCidLength );
        TPtr8 cid = pCid->Des();
        //cid.Format( KSenNumericCidFmt, data.iCid );
        cid.Num( data.iCid );
        if( data.iCid2 != KErrNotFound )
            {
            cid.AppendNum( data.iCid2 );
            }
        if( data.iHasCidPostfix )
            {
            cid.Append( KSenCidPostfix );
            }
        TLSLOG_FORMAT(( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("CSenFileProgressObserver::RunL: txn '%d', progress '%d', isIncoming '%d', cid '%S',  '"), data.iTxnId, data.iProgress, data.iIsIncoming, &cid ));
        TRAP_IGNORE( iFileProgressObserver->TransferProgress( data.iTxnId, data.iIsIncoming, KNullDesC8, cid, data.iProgress );	)
        CleanupStack::PopAndDestroy( pCid );
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT(( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("- MAJOR: iFileProgressProperty.Get(propertyValue) failed: %d"), getPropertyCode ));
        }
#endif // _SENDEBUG 
    }
    
TInt CSenFileProgressObserver::RunError(TInt aError)
	{
	TLSLOG_FORMAT(( KSenServiceConnectionStatusLogChannelBase+iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("CSenFileProgressObserver::RunError:[%d]"), aError ));
	// return aError;
	return KErrNone; // ignore the error, this is conscious decision.
	}


// ------------------------------------------------------------------------------------------------------------

CSenConnectionChunk* CSenConnectionChunk::NewL(const TDesC& aChunkName, TBool aInternal )
    {
    CSenConnectionChunk* pChunk = CSenConnectionChunk::NewLC(aChunkName, aInternal);
    CleanupStack::Pop();
    return pChunk;
    }

CSenConnectionChunk* CSenConnectionChunk::NewLC(const TDesC& aChunkName, TBool aInternal)
    {
    CSenConnectionChunk* pChunk = new (ELeave) CSenConnectionChunk();

    
    if ( pChunk->ServOpenChunkCount() >= (aInternal?(KMaxServMessageSlots+2):KMaxServMessageSlots ) )
        {
        delete pChunk;
        pChunk = NULL;
        }
    CleanupStack::PushL(pChunk); // push even NULL, since client expects an item in cleanupstack
    if ( pChunk )
        {
        pChunk->ConstructL( aChunkName );
        }
    return pChunk;
    }

void CSenConnectionChunk::ConstructL(const TDesC& aChunkName)
    {
    if (aChunkName.Length() > 0)
        {
        ipChunkName = aChunkName.AllocL();
        }
    else
        {
        ipChunkName = NULL;
        }
    iServOpenChunkCount++;
    }
    
CSenConnectionChunk::CSenConnectionChunk()
:   ipDataContainerArray(NULL), 
    iMap( EFalse, ETrue )
    {
    }
    
CSenConnectionChunk::~CSenConnectionChunk()
    {
    if ( ipDataContainerArray )
        {
        ipDataContainerArray->Close();
        delete ipDataContainerArray;
        }
    iMap.Reset(); // will de-allocate the CSenBinaryContainers, if any

    iServOpenChunkCount--;
    }

RArray<TXmlEngDataContainer>& CSenConnectionChunk::BinaryDataArrayL()	//codescannerwarnings
    {
    if ( !ipDataContainerArray )
        {
        ipDataContainerArray = new (ELeave) RArray<TXmlEngDataContainer>;
        }
        
    return *ipDataContainerArray;
    }

TBool CSenConnectionChunk::HasBinaryDataArray()
    {
    if ( ipDataContainerArray )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CSenConnectionChunk::HasSharedFileHandle()
{
return iHasSharedFileHandle ;
	

}

void CSenConnectionChunk::HasSharedFileHandle(TBool aHasSharedFileHandle)
{
	iHasSharedFileHandle = aHasSharedFileHandle;
}

void CSenConnectionChunk::RemoveBinaryDataArrayIfEmpty()
    {
    if ( ipDataContainerArray )
        {
        if ( ipDataContainerArray->Count() == 0 )
            {
            ipDataContainerArray->Close();
            delete ipDataContainerArray;
            ipDataContainerArray = NULL;
            }
        }
    }
TInt CSenConnectionChunk::ServOpenChunkCount()
    {
    TInt retCode( iServOpenChunkCount );
#if defined( RD_SEN_ENABLE_USE_OF_WSD_IN_SC_FOR_HW_TARGET )
    retCode = iServOpenChunkCount;
#endif // WINS || RD_SEN_ENABLE_USE_OF_WSD_IN_SC_FOR_HW_TARGET
    return retCode;
    }
    
TInt CSenConnectionChunk::AddFileToContainerMap( const TDesC8& aCid, RFile aFile )
    {
    TInt retVal(0);
    TInt error ( KErrSenInternal );
    
    // First, check for duplicates
    TInt index = iMap.Find( aCid );
    if ( index != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    // Otherwise, it is "safe" to add a binary container under this CID
    TRAP( retVal, 
    		CSenBinaryContainer* pContainer = CSenBinaryContainer::NewL( aCid, aFile );
    		CleanupStack::PushL(pContainer);
    		
		    if( retVal == KErrNone)
		    	{
		    	const HBufC8* pCid = pContainer->CidPtr();
					
					if( pCid )
					    {
					    error = iMap.Append( pCid, pContainer );        
					    }
					
					if( error )
					    {
					    CleanupStack::PopAndDestroy( pContainer );    
					    }
					else
					    {
					    CleanupStack::Pop( pContainer );
					    }
					}
				);			    
    return error;
    }
    
TInt CSenConnectionChunk::AddChunkToContainerMap( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset )
    {
    TInt retVal(0);
    TInt error ( KErrSenInternal );
    // First, check for duplicates
    TInt index = iMap.Find( aCid );
    if ( index != KErrNotFound )
			{
      return KErrAlreadyExists;
      }

    // Otherwise, it is "safe" to add a binary container under this CID
    TRAP( retVal, 
    		CSenBinaryContainer* pContainer = CSenBinaryContainer::NewL( aCid, aChunk, aChunkSize, aChunkOffset );
    		CleanupStack::PushL(pContainer);
    		
		    if( retVal == KErrNone)
		  		{
			    const HBufC8* pCid = pContainer->CidPtr();
			    
			    if( pCid )
			        {
			        error = iMap.Append( pCid, pContainer );        
			        }
			    
			    if( error )
			        {
			        CleanupStack::PopAndDestroy( pContainer );    
			        }
			    else
			        {
			        CleanupStack::Pop( pContainer );
			        }
			    }
	    	);
	  return error;  
  	}
    
RBinaryMap& CSenConnectionChunk::ContainerMap()
    {
    return iMap;
    }

TInt CSenConnectionChunk::ContainerCount()
    {
    return iMap.Count();
    }
    
CSenBinaryContainer::CSenBinaryContainer( TDataType aType )
: iType(aType)
    {
    }

CSenBinaryContainer::~CSenBinaryContainer()
    {
    delete ipCid;
    ipCid = NULL;
    iFile.Close();  
    iChunk.Close(); 
    }
    
void CSenBinaryContainer::ConstructL( const TDesC8& aCid, RFile aFile )
    {
    ipCid = aCid.AllocL();
    iFile.Duplicate( aFile );
    }

void CSenBinaryContainer::ConstructL( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset )
    {
    ipCid = aCid.AllocL();
    iChunk = aChunk; // MOST LIKELY CODE SHOULD OPEN THE CHUNK IN HERE. 
    iChunkSize = aChunkSize;
    iChunkOffset = aChunkOffset;
    }
    

CSenBinaryContainer* CSenBinaryContainer::NewL( const TDesC8& aCid, RFile aFile )
    {
    CSenBinaryContainer* pNew = CSenBinaryContainer::NewLC( aCid, aFile );
    CleanupStack::Pop();
    return pNew;        
    }
    
CSenBinaryContainer* CSenBinaryContainer::NewLC( const TDesC8& aCid, RFile aFile )
    {
    CSenBinaryContainer* pNew = new (ELeave) CSenBinaryContainer( CSenBinaryContainer::EFile );  
    CleanupStack::PushL( pNew );
    pNew->ConstructL( aCid, aFile );
    return pNew;
    }

CSenBinaryContainer* CSenBinaryContainer::NewL( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset )
    {
    CSenBinaryContainer* pNew = CSenBinaryContainer::NewLC( aCid, aChunk, aChunkSize, aChunkOffset );
    CleanupStack::Pop();
    return pNew;        
    }
    
CSenBinaryContainer* CSenBinaryContainer::NewLC( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset )
    {
    CSenBinaryContainer* pNew = new (ELeave) CSenBinaryContainer( CSenBinaryContainer::EChunk );  
    CleanupStack::PushL( pNew );
    pNew->ConstructL( aCid, aChunk, aChunkSize, aChunkOffset );
    return pNew;
    }

TBool CSenBinaryContainer::IsFile() const
    {
    // checks iType (TDataType)
    TBool isFile( iType == CSenBinaryContainer::EFile );
    return isFile;
    }
    
    
TBool CSenBinaryContainer::IsChunk() const
    {
    // checks iType (TDataType)
    TBool isChunk( iType == CSenBinaryContainer::EChunk );
    return isChunk;
    }
    
CSenBinaryContainer::TDataType CSenBinaryContainer::Type() const
    {
    return iType;
    }

TPtrC8 CSenBinaryContainer::Cid() const
    {
    TPtrC8 cid(KNullDesC8);
    if( ipCid )
        {
        cid.Set( *ipCid );
        }
    return cid;
    }

const HBufC8* CSenBinaryContainer::CidPtr() const
    {
    return ipCid;
    }
    
RFile CSenBinaryContainer::File() const
    {
    return iFile;
    }
RChunk CSenBinaryContainer::Chunk() const
    {
    return iChunk;
    }
    
TInt CSenBinaryContainer::ChunkSize() const
    {
    return iChunkSize;   
    }
    
TInt CSenBinaryContainer::ChunkOffset() const
    {
    return iChunkSize;
    }
    
// End of file


