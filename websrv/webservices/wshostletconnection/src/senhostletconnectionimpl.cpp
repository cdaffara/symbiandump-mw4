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
#include <SenXmlUtils.h>

//#include "SenHostletConnectionLog.h"
#include "sendebug.h"
#include "senlogger.h"

#include "senhostletconnectionimpl.h"
#include "MSenHostlet.h"              // public
#include "MSenHostletRequest.h"       // public
#include "MSenHostletResponse.h"      // public
#include "senhostletrequest.h"
#include "senhostletresponse.h"

#include "rsenhostletconnection.h"

#include "senservicemanagerdefines.h" // internal Core\inc  - IPC enumerations

#include "SenXmlServiceDescription.h"

#include "senguidgen.h"  // internal: Utils\inc - the prefix length constant for WSF GUIDs
#include "senchunk.h"
#include "senidentifier.h"

#include "MSenMessage.h"
#include "senconnagentserver.h"
#include "senvtcptransportproperties.h"
#include "SenXmlReader.h"

namespace
    {
    _LIT8( KSenCidPostfix, "@example.org" );
    const TInt KMaxCidLength = 512;
    }


CSenHostletConnectionImpl* CSenHostletConnectionImpl::NewL(MSenHostlet& aProvider)
    {
    CSenHostletConnectionImpl* pNew = NewLC(aProvider);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenHostletConnectionImpl* CSenHostletConnectionImpl::NewLC(MSenHostlet& aProvider)
    {
    CSenHostletConnectionImpl* pNew = new (ELeave) CSenHostletConnectionImpl(aProvider);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

void CSenHostletConnectionImpl::ConstructL()
    {
    TInt connErr = iConnection.Connect();
    TInt connAttemp(0); // max 5 attempts are allowed
    while ( (connErr == KErrServerTerminated || connErr == KErrServerBusy)
                     && connAttemp < KSenMaxClientConnectionOpeningAttempts  )
        {
//        TLSLOG_L(iTlsLogChannel, KSenHostletConnectionLogLevel, "CSenHostletConnection::ConstructL - Server busy/going down");
        User::After(1000000); // wait for a second if server has been shut down
//        TLSLOG_L(iTlsLogChannel, KSenHostletConnectionLogLevel, "CSenHostletConnection::ConstructL - Creating connection");
        connErr = iConnection.Connect();
        connAttemp++;
        }
    if ( connErr != KErrNone )
        {
        User::Leave( connErr );
        }

    iConnectionID = iConnection.ConnectionID();
    iTlsLogChannel = KSenHostletConnectionLogChannelBase + iConnectionID;

    ipRegistrationTimer = CSenRegistrationTimer::NewL( *this );

#ifdef _SENDEBUG
    RThread thread;
    RProcess process;
    TFileName logFile;
    logFile.Append( KSenHostletConnectionLogFile().Left(KSenHostletConnectionLogFile().Length()-4) ); // exclude ".log" file extension
    logFile.AppendNum( iConnectionID );
    logFile.Append( KSenUnderline );
    logFile.Append( process.Name().Left(32));
    logFile.Append( KSenUnderline );
    logFile.Append( thread.Name().Left(20));
    logFile.Append( KSenHostletConnectionLogFile().Right(4) ); // postfix with ".log" file extension

    // Open connection to the file logger server
    TLSLOG_OPEN( iTlsLogChannel, KSenHostletConnectionLogLevel, KSenHostletConnectionLogDir, logFile );
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnection::ConstructL - About to establish new hostlet connection..");
    TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L("- Connection ID: (%d)"), iConnectionID));
#endif

    User::LeaveIfError(EstablishConnectionL(iProvider));
    }

CSenHostletConnectionImpl::CSenHostletConnectionImpl(MSenHostlet& aProvider) :
    iProvider(aProvider),
    iErrorNumber(0),
    iErrorNumberBuffer(NULL, 0),
    iTxnId(KErrNotFound),
    iTxnIdBuffer(NULL, 0),
    iHostletTransactionMap(NULL),
    ipSessionId(NULL),
    iSessionIdBuffer(NULL, 0),
    iChunkNameNumber(0),
    ipChunkName(NULL),
    iAlive(ETrue),
    iConnectionID(KErrNotReady),
    iTlsLogChannel(KSenHostletConnectionLogChannelBase),
    iCancelSession(EFalse),
    ipFileProgressObserver(NULL),
    iRegisterFileObserverDone(EFalse),
    ipRegistrationTimer(NULL)
    {
    CActiveScheduler::Add(this);
    }

CSenHostletConnectionImpl::~CSenHostletConnectionImpl()
    {
    delete ipRegistrationTimer; // Invokes connagent registrations & subscribes for RProperty updates
    delete ipFileProgressObserver; // Subscriber (observer) for RProperty update
    TLSLOG(iTlsLogChannel, KMinLogLevel, _L("CSenHostletConnectionImpl::~CSenHostletConnectionImpl"));
    CSenHostletConnectionImpl::Cancel(); // Causes call to DoCancel()
    CSenHostletConnectionImpl::iAlive = EFalse;
   if (iFilesObserver)
        {
        CSenConnAgentServer::Close();
        }
    if (iAsyncOpsArray)
        {
        TInt count(iAsyncOpsArray->Count());
        for(TInt i=0; i<count; i++)
            {
            CSenAsyncOperation* pOp = (*iAsyncOpsArray)[i];
            if ( pOp )
                {
                TRequestStatus* status = &pOp->iStatus;
                User::RequestComplete( status, KErrSenCancelled );
                pOp->iActive = NULL;
                }
            }
        iAsyncOpsArray->ResetAndDestroy();
        delete iAsyncOpsArray;
        }

    if ( iHostletTransactionMap )
        {
        iHostletTransactionMap->Reset();
        delete iHostletTransactionMap;
        }

    delete ipSessionId;
    delete ipChunkName;

    iConnection.Close();

    // Close the log file and the connection to the server.
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "Log file closed.");
    TLSLOG_CLOSE( iTlsLogChannel );
    //TLSLOG_CLOSE( KSenHostletConnectionLogChannel );
    }


TInt CSenHostletConnectionImpl::Identifier()
    {
    return iConnectionID;    
    }

TInt CSenHostletConnectionImpl::RegisterFilesObserver()
    {
    TLSLOG(iTlsLogChannel, KMinLogLevel, _L("CSenHostletConnectionImpl::RegisterFilesObserver"));
    TInt retVal(KErrNone);
    const TDesC& name = CSenConnAgentServer::Open();
    if (name == KNullDesC)
        {
        return KErrGeneral;
        }
    retVal = iConnection.RegisterTransferObserver(&name, iFilesObserver);
    TLSLOG(iTlsLogChannel, KMinLogLevel, _L("CSenHostletConnectionImpl::RegisterFilesObserver Completed"));
    return retVal; 
    }

TPtrC CSenHostletConnectionImpl::SessionID()
    {
    if(ipSessionId)
        {
        return *ipSessionId;
        }
    else
        {
        return KNullDesC();
        }
    }

TInt CSenHostletConnectionImpl::RespondL(MSenHostletResponse& aResponse)
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::RespondL");
    TInt retVal(KErrNone);
    
    retVal = HostletTransactionMapL().Find(aResponse.RequestId());
    
    if ( retVal != KErrNotFound )
        {
        CSenHostletTransaction* pTransaction = (CSenHostletTransaction*)HostletTransactionMapL().ValueAt(retVal);
        CSenChunk* pOperation = (CSenChunk*)pTransaction->Chunk();
        if ( pOperation )
            {
            // First check whether this request has ALREADY BEEN responded (to prevent multiple RespondL
            // calls performed inside MSenHostlet::ServiceL / per single request):
            MSenMessage::TDirection direction = pOperation->ChunkHeader().MessageDirection();
            
            if( direction == MSenMessage::EOutbound )
                {
                // Note: direction is always consumers point of view. So outbound here
                // means "a request from consumer" - inbound would be "response to consumer".
                
                // Note: this sort of really double-safety action, since hostlet transport plugin also turns the 
                // direction to "inbound" before sending responses to consumers. But better be safe than sorry
                pOperation->ChunkHeader().SetMessageDirection( MSenMessage::EInbound );


                MSenProperties* pProperties = aResponse.ResponseProperties();
                if( pProperties )
                    {
                    // Serialize properties
                    HBufC8* pPropsAsXml = pProperties->AsUtf8LC();
                    // Store the properties class type into chunk
                    MSenProperties::TSenPropertiesClassType type
                        = pProperties->PropertiesClassType();
                    pOperation->ChunkHeader().SetPropertiesType( type );
                    // Store response message AND properties into chunk
                    TPtrC8 properties = pPropsAsXml->Des();
                    pOperation->DescsToChunk( aResponse.ResponseUtf8(), properties );
                    CleanupStack::PopAndDestroy(pPropsAsXml);
                    }
                else
                    {
                    // There is only a response message. Store it into chunk.
                    pOperation->ChunkHeader().SetPropertiesType(MSenProperties::ENotInUse);
                    pOperation->DescToChunk( aResponse.ResponseUtf8() );
                    }            
                CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
                iConnection.ProvideResponse(pSenAO->iStatus, 
                                            pSenAO->iErrorNumberBuffer,
                                            pSenAO->iTxnIdBuffer,
                                            *pOperation,
                                            aResponse.ResponseCode());
                }
            else
                {
                retVal = KErrAlreadyExists; 
                }                
            }
        else
            {
            retVal = KErrNotFound; 
            }
        }
        
    TLSLOG_FORMAT((iTlsLogChannel, KNormalLogLevel, _L8("- ProvideResponse returned: %d"), retVal));
    return retVal;
    }

// This method is executed when one of the static constructors of
// service connection (NewL or NewLC) is called.
TInt CSenHostletConnectionImpl::EstablishConnectionL(MSenHostlet& aProvider)
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::InitializeL");

	TInt retVal = KErrNone;
    ipInterface = NULL;
    ipInterface = aProvider.GetInterfaceByUid(KSenInterfaceUidFilesObserver);
    if(ipInterface)
    {
        iFilesObserver = (MSenFilesObserver*) ipInterface;

        
        if ( !iRegisterFileObserverDone )
            {
            TLSLOG_L( iTlsLogChannel, KMinLogLevel, "EstablishConnectionL(): - Calling IssueRegistrations()");
            ipRegistrationTimer->IssueRegistrations( ETrue );
                TLSLOG_L(iTlsLogChannel, KMinLogLevel ,"- RegisterFilesObserver() failed!");
            } // end of if ( !iRegisterFileObserverDone )
        
    }
    if(retVal == KErrNone)
        {
    TPtrC8 endpoint     = aProvider.Endpoint();
    TPtrC8 contract     = aProvider.Contract();
    TPtrC8 frameworkId  = aProvider.FrameworkId();
    CSenXmlServiceDescription* pDescription = CSenXmlServiceDescription::NewLC(endpoint, contract);
    pDescription->SetFrameworkIdL(frameworkId);

    aProvider.DescribeServiceL(*pDescription);

    if(pDescription->Endpoint().Length()==0 && pDescription->Contract().Length()==0)
        {
        User::Leave(KErrSenNoContractNoEndPoint);
        }
        
    if ( pDescription->Endpoint().Length() == 0 )
        {
#ifdef EKA2            
        RProcess process;
        TSecureId sId = process.SecureId();
        if ( sId == 0 )
            {
            User::Leave(KErrSenNoEndpoint);
            }

        TBuf8<128> buf;
        TUint i(sId);
        _LIT8(KFormat, "%u");
        buf.Format(KFormat, i);

        pDescription->SetEndPointL(buf);
#else
        User::Leave(KErrSenNoEndpoint); // in EKA1, endpoints are not generated!
#endif // EKA2/EKA1        
        }

    // Ensure that hostlet connection transport-plugin cue is applied,
    // unless the application has specified some other plug-in:
    TPtrC8 cue = pDescription->TransportCue();
    if( cue.Length() == 0 )
        {
        // Set the default cue (hostlet connection transport plug-in) to the XML SD:
        pDescription->SetTransportCueL(KSenTransportCueHostletConnection);
//        LOG_WRITEFORMAT((_L8("- Setting the default transport cue: '%S'"), &KSenTransportCueHostletConnection()));
        }


#ifdef _FORCE_DESCRIBED_MSW_2005_08_CONTRACT_TO_2006_10
    /// // Hard coded workaround for update hostlet contract: // ///
    //     Update contract from 2005 => 2006
    _LIT8( KMessaging200508, "http://schemas.live.com/mws/2005/08/messaging" );
    _LIT8( KMessaging200510, "http://schemas.live.com/mws/2006/10/messaging" );
    if( pDescription->Contract() == KMessaging200508 )
        {
        pDescription->SetContractL( KMessaging200510 );
        }
#endif // _FORCE_DESCRIBED_MSW_2005_08_CONTRACT_TO_2006_10


#ifdef _SENDEBUG
    //TPtrC8 providerID  = ((CSenXmlServiceDescription&)aSD).ProviderId();

    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|");
    TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L8("- FrameworkID:  %S"), &frameworkId ));
    TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L8("- Endpoint:     %S"), &endpoint ));
    TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L8("- Contract:     %S"), &contract ));
    TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L8("- TransportCue: %S"), &cue ));
//    LOG_WRITEFORMAT((iTlsLogChannel, KSenHostletConnectionLogLevel, _L8("- ProviderID:  %S"), &providerID ));
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|");
#endif //_SENDEBUG            
    
    HBufC8* pServiceDescriptionXml = pDescription->AsXmlL();
    CleanupStack::PopAndDestroy(pDescription);

    CleanupStack::PushL(pServiceDescriptionXml);

#ifdef _SENDEBUG
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::EstablishConnectionL - Service description:");
    if ( pServiceDescriptionXml )
        {
            TPtrC8 xml = pServiceDescriptionXml->Des();
            TLSLOG_ALL(iTlsLogChannel, KMinLogLevel, (xml));
        }
#endif // _SENDEBUG
    
	 if ( pServiceDescriptionXml )
		{
			TPtr8 ptrServiceDescriptionXml(pServiceDescriptionXml->Des());

			retVal = iConnection.Establish(ptrServiceDescriptionXml);
		}
		else
		{
	    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "EstablishConnectionL ---- Returning KErrGeneral");		
		retVal = KErrGeneral;
		}

    TLSLOG_FORMAT((iTlsLogChannel, KNormalLogLevel, _L("- Establish returned: %d"), retVal));
    User::LeaveIfError( retVal );

    CleanupStack::PopAndDestroy(pServiceDescriptionXml);

    // Start awaiting for next hostlet request
    iErrorNumber = 0;
    iTxnId = 0;

    CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this); // appends itself into array

    // NOTE, following ASYNC AwaitRequest() will stay as pending IPC -operation; this is the design:
    iConnection.AwaitRequest(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer); 
        }
	TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::InitializeL Completed");        
    return retVal;
    }

void CSenHostletConnectionImpl::RunL()
    {
    TLSLOG_FORMAT((iTlsLogChannel, KNormalLogLevel, _L("CSenHostletConnectionImpl::RunL( %d )"),  iStatus.Int()));
    }
    
void CSenHostletConnectionImpl::HandleErrorFromChildAOL(TInt aError, TInt aTxnId, const TDesC8& aDesc, CSenAsyncOperation& aChild )
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::HandleErrorFromChildAOL");
    TInt leaveCode(KErrNone);
    TRAP( leaveCode, iProvider.OnServiceCompleteL(aTxnId, aError, aDesc); )
    
    HostletTransactionMapL().RemoveByKey(aTxnId);
    
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::HandleErrorFromChildAOL:");
#ifdef _SEDEBUG
    if( leaveCode != KErrNone ) 
        {
        TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L(" - OnServiceCompleteL leaved: %d"), leaveCode));
        }
    if( aChild.iStatus.Int() == ESenOnServiceComplete && aError == KErrSenCancelled )
        {
        TLSLOG_L(iTlsLogChannel, KMinLogLevel, "- Await was cancelled (ESenOnServiceComplete).");
        }
#endif // _SENDEBUG
    if( aChild.iStatus.Int() == ESenOnServiceComplete && iAlive )
        {
        CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this); // appends itself into array
        iConnection.AwaitRequest(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer); 
        }
    leaveCode = 0; // not used in release builds
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::HandleErrorFromChildAOL Completed");
    }
    
void CSenHostletConnectionImpl::HandleMessageFromChildAOL(TInt aStatus, CSenAsyncOperation& aChild)
    {
    TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L("CSenHostletConnectionImpl::HandleMessageFromChildAOL( %d )"), aStatus));

    iAsyncOpsCount--;
//    TLSLOG_FORMAT((iTlsLogChannel, KSenHostletConnectionLogLevel, _L("One asynchronous operation completed. Pending iAsyncOpsCount: %i"), iAsyncOpsCount));
    
    TLSLOG_FORMAT((iTlsLogChannel, KNormalLogLevel, _L("One asynchronous operation completed. Pending iAsyncOpsCount: %i"), iAsyncOpsCount));
    
    if ( aStatus == ESenOnServiceComplete) // temporary: implement "acquire request handle" operation and use switch case here..
        {
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, iProvider.OnServiceCompleteL(aChild.iTxnId, aChild.iErrorNumber, KNullDesC8); ) 
        
        HostletTransactionMapL().RemoveByKey(aChild.iTxnId);
        
        TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::HandleMessageFromChildAOL: ESenOnServiceComplete");
#ifdef _SEDEBUG
        if( leaveCode != KErrNone ) 
            {
            TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L(" - OnServiceCompleteL leaved: %d"), leaveCode));
            }
        if( aChild.iErrorNumber == KErrSenCancelled )
            {
            TLSLOG_L(iTlsLogChannel, KMinLogLevel, "- Await was cancelled.");
            }
#endif // _SENDEBUG
        if( aChild.iErrorNumber != KErrSenCancelled && iAlive )
            {
            CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this); // appends itself into array

            // NOTE, following ASYNC AwaitRequest() will stay as pending IPC -operation; this is the design:
            iConnection.AwaitRequest(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer); 
            }
        leaveCode = 0; // not used in release builds
        }
    else if ( aStatus > KErrNone )
        {
        TLSLOG_L(iTlsLogChannel, KMinLogLevel, "- handle received.");
        CSenChunk* pOperation = CSenChunk::NewLC(KNullDesC);
        TInt retVal = pOperation->OpenChunkFromHandleNumberL(aStatus);
        if ( retVal == KErrNone )
            {
            CSenHostletTransaction* pHostletTxn = CSenHostletTransaction::NewL(pOperation);
            CleanupStack::Pop(pOperation);
            CleanupStack::PushL(pHostletTxn);
            
            TInt* pTxnId = new (ELeave) TInt(pOperation->ChunkHeader().TransactionId());
            CleanupStack::PushL(pTxnId);
            retVal = HostletTransactionMapL().Append(pTxnId, pHostletTxn);
            
            if ( retVal == KErrNone )
                {
                CleanupStack::Pop(pTxnId);
                CleanupStack::Pop(pHostletTxn);
                TPtrC8 request;
                retVal = pOperation->DescFromChunk(request);
                
                if ( retVal == KErrNone )
                    {
                    RThread thread;
                    CSenIdentifier* pIdentifier = CSenIdentifier::NewL();
                    pHostletTxn->SetIdentifier(pIdentifier);
                    
                    CSenHostletRequest* pRequest =
                            CSenHostletRequest::NewL(pOperation->ChunkHeader().TransactionId(),
                                                     request,
                                                     thread,
                                                     *pIdentifier,
                                                     pOperation);
                    pHostletTxn->SetRequest(pRequest);
                                                     
                    CSenHostletResponse* pResponse = 
                            CSenHostletResponse::NewL(pOperation->ChunkHeader().TransactionId());
                    pHostletTxn->SetResponse(pResponse);
                    
					MSenProperties::TSenPropertiesClassType type = MSenProperties::ESenVtcpTransportProperties ;
					TPtrC8 properties = pRequest->Properties(type) ;
					if (properties != KNullDesC8)
						{
						CSenXmlReader* pXmlReader = CSenXmlReader::NewL();
						if(pXmlReader)	
							{
							CleanupStack::PushL(pXmlReader);
							CSenVtcpTransportProperties * vtcpTransProp = CSenVtcpTransportProperties::NewL(properties, *pXmlReader);
							if(vtcpTransProp)
							    {
							    CleanupStack::PushL(vtcpTransProp);
								TBool boolValue = EFalse ;
								if (vtcpTransProp->OnewayMessageOnOffL(boolValue) != KErrNotFound
									&& boolValue)
									{
									//Now hostlet implementation can automaticaly reply with zero-length
									//string (KNullDesC8)
									//NOW SET THE ONE WAY RESPONSE PROPERTY AS WELL
									pResponse->SetResponseUtf8L(KNullDesC8(), KErrNone, vtcpTransProp) ;
						            iProvider.ServiceL(*pRequest, *pResponse);
									//RespondL(*pResponse) ; 
									}
									else
									{
						      		iProvider.ServiceL(*pRequest, *pResponse);
									//RespondL(*pResponse) ; 
									}
								CleanupStack::PopAndDestroy(vtcpTransProp) ;
								}
							CleanupStack::PopAndDestroy(pXmlReader) ;					
							}
						}
					else
						{
			            iProvider.ServiceL(*pRequest, *pResponse);
						}
//                    if( iAlive )
//                        {
//                        CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this); // appends itself into array
//                        iConnection.AwaitRequest(pSenAO->iStatus, pSenAO->iErrorNumberBuffer, pSenAO->iTxnIdBuffer); 
//                        }
                    }
                else
                    {
                    User::Leave(retVal);
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(pTxnId);
                CleanupStack::PopAndDestroy(pHostletTxn);
                User::Leave(retVal);
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(pOperation);
            User::Leave(retVal);
            }
        }
    else
        {
        User::Leave(aStatus);
        }
                  
    /*                                         
    switch (aStatus)
        {
        case ESenHostletRequestPending:
            {
            TLSLOG_L(iTlsLogChannel, KSenHostletConnectionLogLevel, "HandleMessageFromChildAOL: ESenHostletRequestPending");
            if (iErrorNumber == KErrNone)
                {
                TLSLOG_FORMAT((iTlsLogChannel, KSenHostletConnectionLogLevel, _L("- Request from consumer received:"), iSessionIdBuffer.Length()));


                //TInt handle = iConnection.AcquireRequestHandle(aChild.iErrorNumberBuffer, aChild.iTxnIdBuffer);
                //CSenChunk* pChunk = CSenChunk::OpenChunkFromRMsgL

                }

            }
        break;

        case ESenInternalError:
            {
            // we could have someting in iErrorNumber in some cases
            TLSLOG_L(iTlsLogChannel, KSenHostletConnectionLogLevel, "HandleMessageFromChildAOL: ESenInternalError");
            TLSLOG_FORMAT((iTlsLogChannel, KSenHostletConnectionLogLevel, 
                _L("- Last received error from server: (%d)"),
                    iErrorNumber));

            //DeliverResponseL(KNullDesC8, KErrSenInternal);
            }
            break;


        default:
            {
#ifdef _SENDEBUG                
            if(aStatus == KErrPermissionDenied)
                {
                TLSLOG_L(iTlsLogChannel, KSenHostletConnectionLogLevel, "HandleMessageFromChildAOL: default - KErrPermissionDenied");
                }
            else
                {
                TLSLOG_L(iTlsLogChannel, KSenHostletConnectionLogLevel, "RunL: default - unexpected error.");
                TLSLOG_FORMAT((iTlsLogChannel, KSenHostletConnectionLogLevel, _L("Last received error from server: (%d)"), iErrorNumber));
                }
            TLSLOG_FORMAT((iTlsLogChannel, KSenHostletConnectionLogLevel, _L("RunL, iStatus.Int(): (%d)"), iStatus.Int()));
            
#endif // _SENDEBUG             
            // WSF does not generalize errors:
            //DeliverResponseL(KNullDesC8, aStatus);
            break ;
            }
        }*/
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::HandleMessageFromChildAOL Completed");
    }

void CSenHostletConnectionImpl::DoCancel()
    {
    TLSLOG(iTlsLogChannel, KMinLogLevel, _L("CSenHostletConnectionImpl::DoCancel"));
    TInt cancelLeaveCode(KErrNone);
    if (iCancelSession == EFalse)
        {
        CSenAsyncOperation* pSenAO = CSenAsyncOperation::NewL(this);
        TRAP(cancelLeaveCode, iConnection.CancelSession(pSenAO->iStatus));
        
    #ifdef _SENDEBUG
        if(cancelLeaveCode!=KErrNone)
            {
            TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L("- CancelSession leaved: %d)"), cancelLeaveCode));
            }
    #endif
        cancelLeaveCode=0;   
        iCancelSession = ETrue; 
        }
    TLSLOG(iTlsLogChannel, KMinLogLevel, _L("CSenHostletConnectionImpl::DoCancel Completed"));
    }
RHostletTransactionMap& CSenHostletConnectionImpl::HostletTransactionMapL()
    {
    if ( !iHostletTransactionMap )
        {
        iHostletTransactionMap = new (ELeave) RHostletTransactionMap(ETrue, ETrue);
        }
    return *iHostletTransactionMap;
    }
    
RPointerArray<CSenAsyncOperation>& CSenHostletConnectionImpl::AsyncOpsArrayL()
    {
    if(!iAsyncOpsArray)
        {
        iAsyncOpsArray = new (ELeave) RPointerArray<CSenAsyncOperation>;
        }
    return *iAsyncOpsArray;
    }

TInt CSenHostletConnectionImpl::RegisterAndSubscribeFileProgressObserversL()
    {
    TInt retCode( KErrNone );
#ifndef RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC        
    if(iFilesObserver && !iRegisterFileObserverDone)
    	{
    	
  #ifndef RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS // pub&sub not in use for outgoing file progress
        TLSLOG( iTlsLogChannel, KMinLogLevel,(_L("CSenHostletConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == FALSE")));
    	retCode = RegisterFilesObserver();
  #else // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE
        TLSLOG( iTlsLogChannel, KMinLogLevel,(_L("CSenHostletConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS == TRUE")));
    	ipFileProgressObserver = CSenFileProgressObserver::NewL( *iFilesObserver, iConnectionID );

    #ifdef RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS
        TLSLOG_L( iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == TRUE");
        TLSLOG_L( iTlsLogChannel, KMinLogLevel, "=> calling RegisterFilesObserver()");
        retCode = RegisterFilesObserver(); // MIXED MODE, using conn agent for SOAP progress..
    #else // RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == FALSE
        TLSLOG_L( iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS == FALSE");
    #endif // RD_SEN_USE_CONNAGENT_FOR_SOAP_PROGRESS

  #endif // RD_SEN_USE_PUBSUB_FOR_OUTGOING_FILE_PROGRESS end 	
    	iRegisterFileObserverDone = ETrue; // register only once
    	}
#else
    TLSLOG_L( iTlsLogChannel, KMinLogLevel, "CSenHostletConnectionImpl::RegisterAndSubscribeFileProgressObserversL: RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC == TRUE, not registering connagent, nor subscribing for the RProperty");
#endif // end of: #ifndef RD_SEN_DISABLE_TRANSFER_PROGRESS_FOR_HC    
    return retCode;
    }
        




CSenAsyncOperation* CSenAsyncOperation::NewL(CSenHostletConnectionImpl* aActive)
    {
    CSenAsyncOperation* pNew = NewLC(aActive);
    CleanupStack::Pop();
    return(pNew);
    }

CSenAsyncOperation* CSenAsyncOperation::NewLC(CSenHostletConnectionImpl* aActive)
    {
    CSenAsyncOperation* pNew = new (ELeave) CSenAsyncOperation(aActive);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
    
CSenAsyncOperation::CSenAsyncOperation(CSenHostletConnectionImpl* aActive)
:   CActive(EPriorityNormal),
    iActive(aActive),
    iErrorNumber(0),
    iErrorNumberBuffer(NULL, 0),
    iTxnId(0),
    iTxnIdBuffer(NULL, 0)
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
#ifdef EKA2
    iActive->AsyncOpsArrayL().AppendL(this);
#else
    RPointerArray<CSenAsyncOperation>& ops = iActive->AsyncOpsArrayL();
    User::LeaveIfError(ops.Append(this));
#endif

    iActive->iAsyncOpsCount++;
    }

CSenAsyncOperation::~CSenAsyncOperation()
    {
    Cancel(); // invokes CSenAsyncOperation::DoCancel()
    }

void CSenAsyncOperation::RunL()
    {
    if ( iActive )
        {
        iActive->iErrorNumber = iErrorNumber;
        iActive->iTxnId = iTxnId;    
        iActive->HandleMessageFromChildAOL(iStatus.Int(), *this);
            
        TInt idx = iActive->AsyncOpsArrayL().Find(this);
        if (idx >= 0)
            {
            iActive->AsyncOpsArrayL().Remove(idx);
            }
        }
    delete this;
    }
    
TInt CSenAsyncOperation::RunError(TInt aError)
    {
    TInt leaveCode( KErrNone );
    if (iActive)
        {
        TRAP( leaveCode, iActive->HandleErrorFromChildAOL(aError, iTxnId, KNullDesC8, *this); ) 

        RPointerArray<CSenAsyncOperation>* pOps = NULL;
        TRAP( leaveCode, pOps = &(iActive->AsyncOpsArrayL()); )
        if ( pOps )
            {
            TInt idx = pOps->Find(this);
            if ( idx != KErrNotFound )
                {
                pOps->Remove( idx );
                }
            }
        delete this;
        }
    return leaveCode; // return != KErrNone ONLY and ONLY IF the error really could not be handled by this AO!
    }
        
void CSenAsyncOperation::DoCancel()
    {
    }

CSenHostletTransaction* CSenHostletTransaction::NewL(CSenChunk* aSenChunk)
    {
    CSenHostletTransaction* pNew = NewLC(aSenChunk);
    CleanupStack::Pop(pNew);
    return(pNew);
    }

CSenHostletTransaction* CSenHostletTransaction::NewLC(CSenChunk* aSenChunk)
    {
    CSenHostletTransaction* pNew = new (ELeave) CSenHostletTransaction(aSenChunk);
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenHostletTransaction::~CSenHostletTransaction()
    {
    delete iIdentifier;
    delete iSenChunk;
    delete iRequest;
    delete iResponse;
    }

CSenHostletTransaction::CSenHostletTransaction(CSenChunk* aSenChunk)
    : iSenChunk( aSenChunk )
    {
    }

void CSenHostletTransaction::SetChunk(CSenChunk* aChunk)
    {
    iSenChunk = aChunk;
    }
        
CSenChunk* CSenHostletTransaction::Chunk()
    {
    return iSenChunk;
    }

void CSenHostletTransaction::SetIdentifier(CSenIdentifier* aIdentifier)
    {
    iIdentifier = aIdentifier;
    }
    
CSenIdentifier* CSenHostletTransaction::Identifier()
    {
    return iIdentifier;
    }

void CSenHostletTransaction::SetRequest(CSenHostletRequest* aRequest)
    {
    iRequest = aRequest;
    }
    
CSenHostletRequest* CSenHostletTransaction::Request()
    {
    return iRequest;
    }

void CSenHostletTransaction::SetResponse(CSenHostletResponse* aResponse)
    {
    iResponse = aResponse;
    }

CSenHostletResponse* CSenHostletTransaction::Response()
    {
    return iResponse;
    }
    


// ------------------------------------------------------------------------------------------------------------

CSenFileProgressObserver* CSenFileProgressObserver::NewL( MSenFilesObserver& aObserver,
                                                          TInt aConnectionID )
    {
    CSenFileProgressObserver* pNew = NewLC( aObserver, aConnectionID );
    CleanupStack::Pop();
    return(pNew);
    }

CSenFileProgressObserver* CSenFileProgressObserver::NewLC( MSenFilesObserver& aObserver,
                                                           TInt aConnectionID )
    {
    CSenFileProgressObserver* pNew = new (ELeave) CSenFileProgressObserver( aObserver, aConnectionID );
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
    
    

CSenFileProgressObserver::CSenFileProgressObserver( MSenFilesObserver& aObserver,
                                                    TInt aConnectionID )
//:   CActive(EPriorityNormal),
:   CActive(EPriorityMore),
    iFileProgressObserver(&aObserver),
    iConnectionID(aConnectionID)
    {
    }

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
	iFileProgressProperty.Subscribe( iStatus );
    if(!IsActive())
    	SetActive();
    iStatus = KRequestPending;

    TLSLOG_L( KSenHostletConnectionLogChannelBase + iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenFileProgressObserver::RunL" );
       
    TPckgBuf<TFileOutgoingTransferProgressBase> progress;
    TInt getPropertyCode = iFileProgressProperty.Get( progress );
    if ( getPropertyCode == KErrNone )
        {
        TLSLOG_L( KSenHostletConnectionLogChannelBase + iConnectionID, KSenServiceConnectionStatusLogLevel, "CSenFileProgressObserver::RunL [new value published] - Calling TransferProgress" );
        TFileOutgoingTransferProgressBase data = progress();

        HBufC8* pCid = HBufC8::NewLC( KMaxCidLength );
        TPtr8 cid = pCid->Des();
        cid.Num( data.iCid );
        if( data.iCid2 != KErrNotFound )
            {
            cid.AppendNum( data.iCid2 );
            }
        if( data.iHasCidPostfix )
            {
            cid.Append( KSenCidPostfix );
            }
        TLSLOG_FORMAT(( KSenHostletConnectionLogChannelBase + iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("- txn '%d', progress '%d', isIncoming '%d', cid '%S',  '"), data.iTxnId, data.iProgress, data.iIsIncoming, &cid ));
        TRAP_IGNORE( iFileProgressObserver->TransferProgress( data.iTxnId, data.iIsIncoming, KNullDesC8, cid, data.iProgress );	)
        CleanupStack::PopAndDestroy( pCid );
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT(( KSenHostletConnectionLogChannelBase + iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("- MAJOR: iFileProgressProperty.Get(propertyValue) failed: %d"), getPropertyCode ));
        }
#endif // _SENDEBUG 
    }
    
TInt CSenFileProgressObserver::RunError( TInt aError )
	{
	TLSLOG_FORMAT(( KSenHostletConnectionLogChannelBase + iConnectionID, KSenServiceConnectionStatusLogLevel, _L8("CSenFileProgressObserver::RunError:[%d]"), aError ));
	// return aError;
	return KErrNone; // ignore the error, this is conscious decision.
	}


// ------------------------------------------------------------------------------------------------------------
    
    
// End of file

