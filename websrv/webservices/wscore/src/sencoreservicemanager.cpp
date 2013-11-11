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
#include <mmtsy_names.h>
#include <SenServiceConnection.h>   // error codes and framework ids
#include <SenHostletConnection.h>   // error codes for hostlet connections
#include <MSenServiceDescription.h>
#include <MSenConsumerPolicy.h>
#include <SenXmlUtils.h>
#include "senwsdescription.h"
#include "senwspattern.h"
#include <MSenServicePolicy.h>

#include "sencoreservicemanager.h"
#include "senservicemanagerdefines.h"
#include "senclientsession.h"
#include "senxmldao.h"

#include "sencoreshutdowntimer.h"

#include "senserviceinvocationframework.h"  // internal Framework\inc
#include "senservicesession.h"              // internal Framework\inc
#include "sendebug.h"                       // internal Utils\inc
#include "senlogger.h"                      // internal Utils\inc
#include "senguidgen.h"                     // internal Utils\inc
#include "senmultiuseridentitymanager.h"         // internal IdentityManager\inc
#include "sencredentialmanager.h"           // internal CredentialManager\inc
#include "seninternalcredential.h"

// To be kept ONLY UNTIL there are real ECOM transport plug-ins available:
#include "SenHttpTransportProperties.h"

#include "sentransportcontext.h"

#include "sentransport.h"

#include "senservercontext.h"

#include <RSenDocument.h>

#include "senxmldebug.h"


namespace
    {
    const TInt KBase64DecodeMultiplier  = 1;
    const TInt KBase64EncodeMultiplier  = 2;
// Deleted to fix for compiler warning  #177-D
    _LIT(KSOAServerName,"SOAServer");
    const TInt KFlatBufSize            = 64;
    }



CSenCoreServiceManager* CSenCoreServiceManager::NewL()
    {
    CSenCoreServiceManager* pNew = CSenCoreServiceManager::NewLC();
    CleanupStack::Pop();
    return pNew;
    }

CSenCoreServiceManager* CSenCoreServiceManager::NewLC()
    {
    CSenCoreServiceManager* pNew =
        new (ELeave) CSenCoreServiceManager(EPriorityNormal);
    CleanupStack::PushL(pNew);

    pNew->ConstructL();
    return pNew;
    }

CSenCoreServiceManager::CSenCoreServiceManager(TInt aPriority)

#ifndef RD_SEN_SC_MSG_DISPATCHER_ENABLED // == FALSE, DISPATCHER IS DISABLED   
#ifdef EKA2
:   CPolicyServer(aPriority, coreServiceManagerPolicy, EUnsharableSessions),
  #else
:   CServer2(aPriority, EUnsharableSessions), //IPC V2
  #endif //EKA2

#else // RD_SEN_SC_MSG_DISPATCHER_ENABLED == TRUE, DISPATCHER IS ENABLED
  #ifdef EKA2
:   CPolicyServer(aPriority, coreServiceManagerPolicy, ESharableSessions),
  #else
:   CServer2(aPriority, ESharableSessions), //IPC V2
  #endif //EKA2
#endif //RD_SEN_SC_MSG_DISPATCHER_ENABLED
    iDAO(NULL),
    iIdentityManager(NULL),
    //iTransport(NULL),
    iGuidGenerator(NULL),
    iReader(NULL),
    iNextTransactionId(0)
//    ipActiveHostletEndpoints(NULL)
    {
    }

void CSenCoreServiceManager::ConstructL()
    {
    iShutdownTimer = CSenCoreShutdownTimer::NewL();
    
    // Start by creating a GUID generator
    iGuidGenerator = CSenGuidGen::NewL();


#ifdef _SENDEBUG

    // Open connection to the file logger server
    // Create a logger
    TLSLOG_OPEN_TO_MODE( KSenCoreServiceManagerLogChannelBase, KSenCoreServiceManagerLogLevel, KSenCoreServiceManagerLogDir, KSenCoreServiceManagerLogFile, EFileLoggingModeAppend );
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::ConstructL - Log file opened [2008-11-12], WSF version 1.9 - build 48");
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Core instance (memory address): 0x%X"), this));

//    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"Zapping filelogged messages (.xml)");

    TLSLOG_OPEN(KSenFaultsLogChannel, KSenFaultsLogLevel, KSenFaults, KSenFaultsLog);
    TLSLOG_OPEN(KSenUtilsLogChannel, KSenUtilsLogLevel, KSenUtils, KSenUtilsLog);

    // zap logged messages (.xml)
//    RFs fss;
//    User::LeaveIfError(fss.Connect() == NULL);
//    CleanupClosePushL(fss);
//
//    fss.Delete(_L("c:\\logs\\WsLog\\AS_init_req.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\AS_init_rsp.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\AS_req.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\AS_rsp.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\DS_lookup_req.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\DS_lookup_rsp.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\service_body_req.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\service_body_rsp.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\service_env_req.xml"));
//    fss.Delete(_L("c:\\logs\\WsLog\\service_env_rsp.xml"));
//    CleanupStack::PopAndDestroy(); // fss.Close()
#endif // _SENDEBUG
#ifdef _RD_SEN_ENABLE_CREDLOG    
    CREDLOG_OPEN();
    
    CREDLOG_L(KSenCredsLogLevelMin,    "Creds.log level 1: MIN enabled.");
    CREDLOG_L(KSenCredsLogLevelNormal, "Creds.log level 2: NORMAL enabled.");
    CREDLOG_L(KSenCredsLogLevelMax,    "Creds.log level 3: MAX enabled.");
    

    _LIT8(KTestingCredsLogLine, "[min]: Logging level verification complete.");
    CREDLOG(KSenCredsLogLevelMin, KTestingCredsLogLine);

    _LIT8(KTestingCredsLongLogLine, "[normal] ------------------------------------------------------------------------------"); // could be more that 120 chars or so..
    CREDLOG_ALL(KSenCredsLogLevelNormal, KTestingCredsLongLogLine);

    _LIT8(KTestingCredsLogLineFormat, "[max] - Logging channel is: %d");
    CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelMax, KTestingCredsLogLineFormat, KSenCredsLogChannel)); // on format lines you need to give channel (at the moment)
#endif // _RD_SEN_ENABLE_CREDLOG    

    // Create THE common XML reader inside Serene
    iReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser

    iIdentityManager = CSenMultiUserIdentityManager::NewL(*this);

    iCredentialManager = CSenCredentialManager::NewL(*this);

    iDAO = CSenXMLDAO::NewL(*(MSenCoreServiceManager*)this);
    	
	CREDLOG_L(KSenCredsLogLevelMin,    "GetImsiImeiL Started");    	
    // Load the sessions from XML-file
    GetImsiImeiL();
	CREDLOG_L(KSenCredsLogLevelMin,    "GetImsiImeiL Completed");    
    
    iDAO->Load();

    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"SenCoreServiceManager starting!");

    // Create server context for SEN.EXE (could be done in thread function as well)
    ipServerContext = CSenServerContext::NewL(*this);
   
    StartL(KSenServiceManager);
    NotifyFrameworksL(KSenEventWsfReady);
    }

CSenCoreServiceManager::~CSenCoreServiceManager()
    {

    // Delete member variables
    CSenCoreServiceManager::SetShowPasswordDialog(EFalse);
    delete ipServerContext;
    delete iDAO;
    delete iCredentialManager;
    if(iIdentityManager != NULL) // Fix for codescanner warning
    {
    iIdentityManager->WriteDatabase();
    }
    delete iIdentityManager;
    delete iGuidGenerator;
    delete iShutdownTimer;
    delete iReader;
    delete ipVtcpTransport;
//    delete ipVtcpEndpoint;
//    if(ipActiveHostletEndpoints)
//        {
//        ipActiveHostletEndpoints->ResetAndDestroy();
//        delete ipActiveHostletEndpoints;
//        }

#ifdef _SENDEBUG
    TLSLOG_CLOSE(KSenFaultsLogChannel);
    TLSLOG_CLOSE(KSenUtilsLogChannel);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Log file closed.");

    TLSLOG_CLOSE(KSenCoreServiceManagerLogChannelBase);
#endif // _SENDEBUG

    CREDLOG_CLOSE();

    iStringPool.Close();
//    iLog.Close();
    }

//////////////////////////////////////////////////////////////////////////
// from MSenIdentityManager

MSenIdentity& CSenCoreServiceManager::IdentityL()
    {
    return iIdentityManager->IdentityL();
    }

TInt CSenCoreServiceManager::UserNameL(HBufC8*& aUserName)
    {
    return iIdentityManager->UserNameL(aUserName);
    }

CSenIdentityProvider* CSenCoreServiceManager::IdentityProviderL(
                                                        const TDesC8& aURI )
    {
    if(iIdentityManager)
        return iIdentityManager->IdentityProviderL(aURI);
    else
        return NULL;
    }

CSenIdentityProvider* CSenCoreServiceManager::IdentityProviderL(
                                                const CDesC8Array& aIdpList,
                                                TBool aStrict)
    {
    if(iIdentityManager)
        return iIdentityManager->IdentityProviderL(aIdpList,aStrict);
    else
        return NULL;
    }


CSenIdentityProvider* CSenCoreServiceManager::IdentityProviderL(
                                MSenServiceDescription& aServiceDescription )
    {
    if(iIdentityManager)
        return iIdentityManager->IdentityProviderL(aServiceDescription);
    else
        return NULL;
    }

CSenIdentityProvider* CSenCoreServiceManager::IdentityProviderL(
                                            MSenServiceDescription& aService,
                                            const CDesC8Array& aIdpList,
                                            TBool aStrict)
    {
    if(iIdentityManager)
        return iIdentityManager->IdentityProviderL(aService, aIdpList, aStrict);
    else
        return NULL;
    }

const RPointerArray<CSenIdentityProvider>& CSenCoreServiceManager::IdentityProvidersL()
    {
    if(!iIdentityManager) User::Leave(KErrNotFound);
    return iIdentityManager->IdentityProvidersL();
    }

CSenIdentityProvider* CSenCoreServiceManager::IdentityProviderL()
    {
    if(iIdentityManager)
        return iIdentityManager->IdentityProviderL();
    else
        return NULL;
    }

TInt CSenCoreServiceManager::RegisterIdentityProviderL(
                                                CSenIdentityProvider* aIdp)
    {
    return iIdentityManager->RegisterIdentityProviderL(aIdp);
    }

TInt CSenCoreServiceManager::UnregisterIdentityProviderL(
                                                CSenIdentityProvider& aIdp)
    {
    return iIdentityManager->UnregisterIdentityProviderL(aIdp);
    }

TBool CSenCoreServiceManager::AssociateServiceL(const TDesC8& aServiceID,
                                                const TDesC8& aProviderID )
    {
    return iIdentityManager->AssociateServiceL(aServiceID, aProviderID);
    }

TBool CSenCoreServiceManager::DissociateServiceL(const TDesC8& aServiceID,
                                                 const TDesC8& aProviderID )
    {
    return iIdentityManager->DissociateServiceL(aServiceID, aProviderID);
    }

TInt CSenCoreServiceManager::IdentitiesL( CDesC8Array& aIdentitiesList )
    {
    return iIdentityManager->IdentitiesL( aIdentitiesList );
    }

TInt CSenCoreServiceManager::AuthenticationForL(CSenIdentityProvider& aAccount, TPckgBuf<TSenAuthentication>& aResponse)
    {
    return iIdentityManager->AuthenticationForL( aAccount, aResponse );
    }

//////////////////////////////////////////////////////////////////////////


// getter
MSIF* CSenCoreServiceManager::Framework(const TDesC8& aFrameworkID)
    {
    return(iDAO->Framework(aFrameworkID));
    }


TInt CSenCoreServiceManager::InitServiceConnectionL(MSenRemoteServiceConsumer& aConsumer,
                                                    CSenWSDescription& aPattern,
                                                    HBufC8*& aErrorMsg)
    {
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL");
    TInt retVal(KErrNotFound);

//    CSenWSDescription* pSD = iDAO->FindMatchingServiceDescription(aPattern);
        
    TInt bestScore;
    CSenWSDescription* pSD = 
        iDAO->FindMatchingSDAndBestScoreL(aPattern, bestScore);	//CodeScannerWarnings

    if(!pSD) // service NOT found
        {
         CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
         CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ," - No matching service description in XML database.");

        // request frameworks to search for a matching service
        // as soon as a matching service is found retry the primary search

        TInt additions = iDAO->AddServiceDescriptionToFrameworksL( aPattern, aConsumer, aErrorMsg );
        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
        TInt error(KErrNone);
        if( additions < 0 )
            {
            error = additions;
            additions = 0;
            }
        CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8(" - Just added %d service description(s)."), additions));
        if ( error )
            {
            retVal = error;
            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"/ / / / / / / / / / / / / / /");
            CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8(" - Error occurred: (%d)."), error));
            if( aErrorMsg )
                {
                CSLOG_ALL(aConsumer.ConnectionId(),KMinLogLevel ,*aErrorMsg);
                }
             CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"/ / / / / / / / / / / / / / /");
            }

        if( additions > 0 ) // at least one SD was added, and no error occurred
            {
			/* We should now try the primary search again.
			 * Note that we try to pick a service that matches 
			 * at least as well as the best we had earlier
			 * but at least the contract or endpoint should
			 * match.
			 */ 
            pSD = iDAO->FindScoreMatchingServiceDescriptionL(aPattern, bestScore+1);	//CodeScannerWarnings
//            pSD = iDAO->FindMatchingServiceDescription(aPattern);
            }
        }

    if(pSD) // Service was found
        {
        // Update ProviderPolicy information
        pSD->RebuildFrom(aPattern);
        // Update ServicePolicy information
        MSenServicePolicy* servicePolicy = pSD->ServicePolicy();
        if(servicePolicy)
            {
            MSenServicePolicy* givenServicePolicy = aPattern.ServicePolicy();
            if(givenServicePolicy)
                servicePolicy->RebuildServicePolicyFrom(*givenServicePolicy);
            }

#ifdef _SENDEBUG
        TPtrC8 endpoint = pSD->Endpoint();
        if(endpoint.Length()>0)
            {
            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
           CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8("- Matching service found, endpoint: %S"), &endpoint));
            }
        else
            {
            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- Matching service found; it has no endpoint.");
            }
#endif // _SENDEBUG

        TPtrC8 patternFrameworkID = pSD->FrameworkId();
        if(patternFrameworkID != KNullDesC8)
            {
            MSIF* pFramework = NULL;
            pFramework = iDAO->Framework( patternFrameworkID );

            if(pFramework)
                {
                CSenServiceSession* pSession = NULL;

                TInt err = pFramework->CreateServiceSessionL(*pSD,
                                                            aPattern,
                                                            pSession,
                                                            aConsumer,
                                                            aErrorMsg);
                CleanupStack::PushL(pSession);
                if( err != KErrNone )
                    {
                    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
                    CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8(" - CreateServiceSessionL failed, error: %d"), err));

                    if ( pSession && pSession != pSD )
                        {
                        // Since session creation failed, delete the returned orphan
                        // object. This should never occur; it is badly against the
                        // "framework design paradigms"):
                        CleanupStack::PopAndDestroy( pSession );
                        }
                    else
                        {
                        // NULL was returned
                        CleanupStack::Pop( pSession );
                        }
                    return err;
                    }
                else
                    {
                    // Create service session OK
                    // Note: isReady in Symbian c == isValid in Java
                    if( pSession && pSession->IsReadyL() )
                        {
                        // bind consumer to the new session:
                        pSession->AddConsumerL(aConsumer);
                        aConsumer.SetSessionL(*pSession);
                        UpdateTouchL(pSession);
                        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
                        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ," - CreateServiceSessionL OK!");
                        CleanupStack::Pop(); // pSession
                        return KErrNone;
                        }
                    else
                        {
                        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CSenCoreServiceManager::InitServiceConnectionL:");
                        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- Removing service description");

                        if(pSession)
                            {
                            iDAO->Remove(*pSession);
                            }
                        else
                            {
                            iDAO->Remove(*pSD);
                            }
                        CleanupStack::Pop(pSession);
                        return KErrNotReady;
                        }
                    }
                }
            }

        }
    // else { // service was not found }
    return retVal;
    }

TInt CSenCoreServiceManager::ServiceDescriptionsL( RWSDescriptionArray& aMatches, const TDesC8& aContract )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::ServiceDescriptionsL(<contract>)");
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - Collecting existing service descriptions with contract: %S"), &aContract));
    return iDAO->FindAllMatchingServiceDescriptions(aMatches, aContract);
    }

TInt CSenCoreServiceManager::ServiceDescriptionsL(RWSDescriptionArray& aMatches, MSenServiceDescription& aPattern)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::ServiceDescriptionsL(<pattern>)");
    return iDAO->FindAllMatchingServiceDescriptions(aMatches, aPattern);
    }

// Note that this method *always* takes the ownership of the SD
TInt CSenCoreServiceManager::RegisterServiceDescriptionL( CSenWSDescription* apServiceDescription )
    {
    if(!apServiceDescription)
        {
        return KErrArgument;
        }

    TInt retVal(KErrNotFound);
    MSIF* framework = Framework(apServiceDescription->FrameworkId());
    if(framework)
        {
        retVal = framework->RegisterServiceDescriptionL(*apServiceDescription);

        if(!iDAO->Owns(apServiceDescription))
            {
            delete apServiceDescription;
            apServiceDescription = NULL;
            }
        }
    else
        {
        delete apServiceDescription;
        apServiceDescription = NULL;
        }
    return retVal;
    }


TInt CSenCoreServiceManager::UnregisterServiceDescriptionL( MSenServiceDescription& aServiceDescription )
    {
    TInt retVal(KErrNotFound);
    MSIF* framework = Framework(aServiceDescription.FrameworkId());
    if(framework)
        {
        retVal = framework->UnregisterServiceDescriptionL(aServiceDescription);
        }
    return retVal;
    }


CDesC8Array& CSenCoreServiceManager::SupportedFrameworksL()
    {
    return iDAO->SupportedFrameworksL();
    }

RFileLogger* CSenCoreServiceManager::Log() const
    {
    return (RFileLogger*) &iLog;
    }

void CSenCoreServiceManager::IncrementConnections()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::IncrementConnections");
    if( iShutdownTimer && iShutdownTimer->IsActive() ) // iConnectionCount <= 0
        {
        iShutdownTimer->Deque(); // will prevent the active scheduler (of this server) from being stopped
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Shutdown timer cancelled, serving emerged new client.");
        }
    iConnectionCount++;
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Client count increased to: %d"),iConnectionCount));
    }

void CSenCoreServiceManager::DecrementConnections()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CSenCoreServiceManager::DecrementConnections");
    iConnectionCount--;
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Client count decreased to: %d"),iConnectionCount));
    if (iConnectionCount <= 0)
        {
        if( iShutdownTimer )
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Shutdown timer activated.");
            iShutdownTimer->ActivateShutdown(); // will stop the active scheduler (of this server) after N secs, unless cancelled
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Stopping active scheduler.");
            CActiveScheduler::Stop(); // in practice, not utilized anymore (with shutdown timer functionality)
            }
       }
    }

// from MSenCoreServiceManager
HBufC8* CSenCoreServiceManager::RandomGuidL()
    {
    HBufC8* pRandomGuid =  iGuidGenerator->GetRandomGuid8LC();
    CleanupStack::Pop();
    return pRandomGuid;
    }

// NOTE(!): AddServiceDescriptionL() always takes the ownership of aServiceDescription
TInt CSenCoreServiceManager::AddServiceDescriptionL( CSenWSDescription* apServiceDescription )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");

    if(!apServiceDescription)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel," - Illegal argument: Cannot add NULL as ServiceDescription!");
        return KErrArgument; 
        }

    TPtrC8 contract     = apServiceDescription->Contract();
    TPtrC8 endpoint     = apServiceDescription->Endpoint();

#ifdef _SENDEBUG
    TPtrC8 frameworkID  = apServiceDescription->FrameworkId();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8(" - Framework ID: '%S'"),&frameworkID));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8(" - Endpoint: '%S'"),&endpoint));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8(" - Contract: '%S'"),&contract));
#endif

    if ( contract.Length() == 0 && endpoint.Length() == 0 )
        {

        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - No endpoint & no contract: invalid SD NOT added!");
        return KErrSenNoContractNoEndPoint;
        }

    CSenWSDescription* pDuplicate =
        iDAO->FindExactServiceDescriptionL(*apServiceDescription);	//CodeScannerWarnings

    if(pDuplicate)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Duplicate found, going to unregister -> register!");

        // Signal the frameworks: the duplicate ownership is
        // theirs (it can be deleted, etc)
        if( iDAO->Remove( *pDuplicate ) ) // true if matching SD already exists
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Removed duplicate, adding new SD.");

            TInt retVal = iDAO->Add(*apServiceDescription);
            if(retVal!=KErrNone)
                {
                // Failed to add new SD to XML DAO, must free the orphan
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Could NOT add as new SD. Deleting this orphan.");
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - error code: %d"),retVal));

                // delete the new instance and return error (to framework)
             
                return retVal; 
                }
#ifdef _SENDEBUG
            else
                {
                // Successfully added new SD, XMLDAO now owns it
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Successfully added as new SD instance.");
                }
#endif
            }
        else
            {
            // Failed to remove duplicate SD from database
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Remove duplicate SD failed. Returning error.");

            // delete the new instance and return error (to framework)
            
            return KErrGeneral; 
            }

        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::AddServiceDescriptionL()");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Adding new service description.");
        iDAO->Add(*apServiceDescription);
        }

    return KErrNone;
    }

TInt CSenCoreServiceManager::RemoveServiceDescriptionL( CSenWSDescription& aServiceDescription )
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenCoreServiceManager::RemoveServiceDescriptionL:")));

#ifdef _SENDEBUG
    CBufFlat* pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    RBufWriteStream writeStream;
    writeStream.Open(*pBuf);
    CleanupClosePushL(writeStream);
    aServiceDescription.WriteAsXMLToL(writeStream);
    TPtr8 asXml = pBuf->Ptr(0);
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(asXml));
    CleanupStack::PopAndDestroy(2); // writeStream.Close(), pBuf
#endif // _SENDEBUG

    if( iDAO->Remove( aServiceDescription ) )
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(_L("Removed matching SD!")));
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(_L("No matching SD found!")));
        return KErrNotFound;
        }

    return KErrNone;
    }

// 2005-09-11: note that function leaves(!), if framework was
// not successfully added to XML DAO
CSenBaseFragment* CSenCoreServiceManager::InstallFrameworkL(const TDesC8& aCue)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase, KMinLogLevel,(_L("CSenCoreServiceManager::InstallFrameworkL, cue:")));
    TLSLOG(KSenCoreServiceManagerLogChannelBase, KMinLogLevel,(aCue));
    CSIF* pFramework = CSIF::NewL(aCue, *this);
    CleanupStack::PushL(pFramework);

#ifdef _SENDEBUG
        HBufC8* ptrBuf = pFramework->AsXmlL();
        if(ptrBuf)
            {
            CleanupStack::PushL(ptrBuf);
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*ptrBuf));
            CleanupStack::PopAndDestroy();
            }
#endif // _SENDEBUG


    CSenBaseFragment* frameworkConfigParser = & pFramework->ConfigParser();
    TInt retCode(iDAO->Add(*pFramework));
    if(retCode==KErrNone)
        {
        CleanupStack::Pop(); //
        }
    else
        {
        // delete orpan SIF plug-in instance now
        CleanupStack::PopAndDestroy();
        }

    // leave, if add plug-in to DAO failed
    User::LeaveIfError(retCode);


    return frameworkConfigParser;
    }

TInt CSenCoreServiceManager::SaveL(const CSenServiceSession& aServiceSession)
    {
    return iDAO->SaveL(aServiceSession);
    }

TInt CSenCoreServiceManager::SaveL(const CSIF& aServiceInvocationFramework)
    {
    return iDAO->SaveL(aServiceInvocationFramework);
    }

CSenXmlReader* CSenCoreServiceManager::XMLReader()
    {
    return iReader;
    }

// notifies *ALL* framework plug-ins about certain event
TInt CSenCoreServiceManager::NotifyFrameworksL(const TInt aEvent)
    {
    return NotifyFrameworksL(KNullDesC8, aEvent, NULL);
    }

// same as above, with an additional pointer argument
TInt CSenCoreServiceManager::NotifyFrameworksL(const TInt aEvent,
                                               TAny* aArgument)
    {
    return NotifyFrameworksL(KNullDesC8, aEvent, aArgument);
    }


// notifies only those frameworks plug-ins, which match
// with given framework ID (aFrameworkID)
//
// Note(!): if aFrameworkID == KNullDesC8 (zero-length),
// then *all* known framework plug-ins are notified
// about the event.
TInt CSenCoreServiceManager::NotifyFrameworksL(const TDesC8& aFrameworkID,
                                             const TInt aEvent)
    {
    return NotifyFrameworksL(aFrameworkID, aEvent, NULL);
    }


TInt CSenCoreServiceManager::NotifyFrameworksL(const TDesC8& aFrameworkID,
                                             const TInt aEvent,
                                             TAny* aArgument)
    {
    return iDAO->NotifyFrameworksL(aFrameworkID, aEvent, aArgument);
    }



//MSenIdentityManager& CSenCoreServiceManager::IdentityManager()
//    {
//    return *(MSenIdentityManager*)this;
//    }


// CServer implementation:
CSession2* CSenCoreServiceManager::NewSessionL( const TVersion &aVersion, const RMessage2& /*aMessage*/) const //IPC V2
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenCoreServiceManager::NewSessionL() start")));

    // Check we're the right version
    if ( !User::QueryVersionSupported(TVersion(KWsfServMajorVersionNumber,
         KWsfServMinorVersionNumber,
         KWsfServBuildVersionNumber),
         aVersion))
        {
        User::Leave(KErrNotSupported);
        }
    return CSenClientSession::NewL( *const_cast<CSenCoreServiceManager*> (this), *ipServerContext );
    }


void CSenCoreServiceManager::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* pActiveScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(pActiveScheduler) ;


    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install(pActiveScheduler);

    // Give a name to this thread
    User::LeaveIfError(User::RenameThread(KSOAServerName));

    RSenDocument::ManualXmlEngineTlsAttachL();

    // Construct our server
    CSenCoreServiceManager::NewLC(); 

    RSemaphore semaphore;
    CleanupClosePushL(semaphore);
    User::LeaveIfError(semaphore.OpenGlobal(KSenServiceManagerSemaphoreName));

    // Semaphore opened ok
    semaphore.Signal();
    CleanupStack::PopAndDestroy(); // close semaphore

    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(); // CSenCoreServiceManager
    
    RSenDocument::ManualXmlEngineTlsCleanup();

    CleanupStack::PopAndDestroy(); // pActiveScheduler
    }

void CSenCoreServiceManager::PanicClient(const RMessage2& aMessage,
                                         TWsfServPanic aPanic)
    {
    aMessage.Panic(KSenServiceManagerPanic, aPanic);
    }

void CSenCoreServiceManager::PanicServer(TWsfServPanic aPanic)
    {
    User::Panic(KSenServiceManagerPanic, aPanic);
    }

// @param aCommand is the 2nd argument given to RProcess::Create() method by the invoking client
TInt CSenCoreServiceManager::ThreadFunction(TAny* /* aCommand */)
    {
#ifdef _SENDEBUG
#ifdef _DEBUG
#ifndef _RD_SEN_DO_NOT_USE_UHEAP_MARKING
    __UHEAP_MARK;
#endif // _RD_SEN_DO_NOT_USE_UHEAP_MARKING   
#endif // _DEBUG
#endif // _SENDEBUG

    CTrapCleanup* pCleanupStack = CTrapCleanup::New();
    if ( pCleanupStack == NULL )
        {
        PanicServer(ECreateTrapCleanup);
        }

    TRAPD( err, ThreadFunctionL(); )
    if (err != KErrNone)
        {
        PanicServer( ESrvCreateServer );
        }

    delete pCleanupStack;
    pCleanupStack = NULL;
    REComSession::FinalClose();
#ifdef _SENDEBUG
#ifdef _DEBUG
#ifndef _RD_SEN_DO_NOT_USE_UHEAP_MARKING
    __UHEAP_MARKEND; // check memory leaks
#endif // _RD_SEN_DO_NOT_USE_UHEAP_MARKING   
#endif // _DEBUG
#endif // _SENDEBUG

    return KErrNone;
    }


// We come here, if CSenClientSessions's ServiceL has leaved:
TInt CSenCoreServiceManager::RunError(TInt aError)
    {
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenCoreServiceManager::RunError( %d )"),aError));
    if (aError == KErrBadDescriptor)
        {
        // A bad descriptor error implies a badly programmed client,
        // so panic it..
        PanicClient(Message(), EBadDescriptor);
        }
    else
        {
        // .. otherwise report the error to the client
        Message().Complete(aError);
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    return KErrNone;    // handled the error fully
    }



// Base64 encode/decode
HBufC8* CSenCoreServiceManager::EncodeToBase64LC(const TDesC8& aSource)
    {
    HBufC8* pResult8 =
            HBufC8::NewLC(aSource.Length() * KBase64EncodeMultiplier);
    TPtr8 result8 = pResult8->Des();
    Base64Codec().Encode(aSource, result8);
    return pResult8;
    }

HBufC8* CSenCoreServiceManager::DecodeFromBase64LC(const TDesC8& aSource)
    {
    HBufC8* pResult8 =
            HBufC8::NewLC(aSource.Length() * KBase64DecodeMultiplier);
    TPtr8 result8 = pResult8->Des();
    Base64Codec().Decode(aSource, result8);
    return pResult8;
    }

// private getter, which always initializes the base64codec
TBase64& CSenCoreServiceManager::Base64Codec()
    {
    return iBase64Codec;
    }

TPtrC8 CSenCoreServiceManager::SenSecurityMechanismNames8L()
    {
    return iIdentityManager->SenSecurityMechanismNames8L();
    }

CSenSecurityMechanism* CSenCoreServiceManager::MechanismNamedL(
                                                        const TDesC8& aName)
    {
    return iIdentityManager->MechanismNamedL(aName);
    }

void CSenCoreServiceManager::SetShowPasswordDialog(const TBool aState)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::SetShowPasswordDialog");
    iIdentityManager->SetShowPasswordDialog(aState);
    }

TInt CSenCoreServiceManager::ContainsServiceDescriptionL(TBool& aContains,
                                                         CSenWSDescription& aPattern)
    {
    return iDAO->ContainsServiceDescriptionL(aContains, aPattern);
    }

TInt CSenCoreServiceManager::ContainsIdentityProviderL(TBool& aContains,
                                                    CSenIdentityProvider& aIDP)
    {
    return iIdentityManager->ContainsIdentityProviderL(aContains, aIDP);
    }


TInt CSenCoreServiceManager::SizeOfServiceDescriptionsL(RWSDescriptionArray& aArray)
    {
    TInt size(0);

    TInt count = aArray.Count();
    CBufFlat* pBuf = NULL;
    for(TInt i = 0; i < count; i++)
        {
        pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);

        aArray[i]->WriteAsXMLToL(bufWs);
        TPtr8 p8 = pBuf->Ptr(0);
        size += p8.Length();
        CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
        pBuf = NULL;
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenCoreServiceManager::SizeOfServiceDescriptionsL: %d"),size));


    return size;
    }
TInt CSenCoreServiceManager::SizeOfIdentityProvidersL(const RPointerArray<CSenIdentityProvider>& aArray)
    {
    TInt size(0);

    TInt count = aArray.Count();
    CBufFlat* pBuf = NULL;
    for(TInt i = 0; i < count; i++)
        {
        pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);

        aArray[i]->WriteAsXMLToL(bufWs);
        TPtr8 p8 = pBuf->Ptr(0);
        size += p8.Length();
        CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
        pBuf = NULL;
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenCoreServiceManager::SizeOfIdentityProvidersL: %d"),size));

    return size;
    }

TInt CSenCoreServiceManager::SizeOfCredentialsL(RSenCredentialArray& aArray)
    {
    _LIT8(KNoProperties, "-");
    TInt size(0);

    TInt count = aArray.Count();
    CBufFlat* pBuf = NULL;
    for(TInt i = 0; i < count; i++)
        {
        pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);

        aArray[i]->WriteAsXMLToL(bufWs);
        if ( aArray[i]->HasProperties() )
            {
            aArray[i]->PropertiesL().WriteAsXMLToL(bufWs);	//codescannerwarnings            
            }
        else
            {
            bufWs.WriteL(KNoProperties);
            }

        TPtr8 p8 = pBuf->Ptr(0);
        size += p8.Length();
        
        CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
        pBuf = NULL;
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CSenCoreServiceManager::SizeOfCredentialsL: %d"),size));


    return size;
    }

CSenTransportBase* CSenCoreServiceManager::CreateL( CSenWSDescription& aInitializer,
                                                    CSenServiceSession* apSession)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::CreateL:");
    TInt leaveCode(KErrNone);

    CSenTransportBase* pTransport = NULL;

    TPtrC8 transportCue = aInitializer.TransportCue();
    if( apSession && (apSession->TransportCue().Length()>0) )
        {
        transportCue.Set( apSession->TransportCue() );
        } 

    TPtrC8 uriScheme;
    if( transportCue.Length() == 0 )
        {
        // Next, attempt resolve the <scheme> from the endpoint

        TPtrC8 uri = aInitializer.Endpoint();
        if(uri.Length()>0)
            {
            TInt index = uri.Locate(':');
            if(index!=KErrNotFound)
                {
                uriScheme.Set( uri.Left(index) );
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- URI scheme is: '%S'"),&uriScheme));
                transportCue.Set(iDAO->TransportPluginCueBySchemeL(uriScheme));
                }
            }
        }
        
/*
    TBool isVtcp(EFalse);
    if ( transportCue == KSenTransportCueVirtualTCP || uriScheme == KSenTransportSchemeTCP )
        {
        isVtcp = ETrue;
        if ( ipVtcpTransport )
            {
            if( ipVtcpEndpoint && *ipVtcpEndpoint == aInitializer.Endpoint() )
                {
                return ipVtcpTransport;
                }
            else
                {
//                delete ipVtcpTransport; // not owned!
                ipVtcpTransport = NULL;
                delete ipVtcpEndpoint;
                ipVtcpEndpoint = NULL;
                //ipVtcpEndpoint = aInitializer.Endpoint().AllocL();
                }
            }
//        else { // we are creating a new transport instance }        
        }
*/    
    
    // TEST
    CSenTransportContext* pCtx = NULL;
    if(apSession)
        {
        pCtx = CSenTransportContext::NewLC( *this, aInitializer, *apSession );
        }
    else
        {
        pCtx = CSenTransportContext::NewLC( *this, aInitializer );
        }
        
    if( transportCue.Length() > 0 )
        {
        // Utilize the cue:
       if ( transportCue == KSenTransportCueVirtualTCP || uriScheme == KSenTransportSchemeTCP )       
            {
            if ( !ipVtcpTransport )
                {
                TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, _L8("- About to instantiate new vTCP transport (args: cue: '%S', ctx)"), &transportCue ));
                TRAP( leaveCode, ipVtcpTransport = CSenTransport::NewL( transportCue, pCtx ); )
                if( leaveCode )
                    {
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Could not load vTCP transport plugin, leave: %d"),leaveCode));
				     CleanupStack::Pop(pCtx);					
                    User::Leave( leaveCode ); // throw forward
                    }
                CleanupStack::Pop(pCtx); // ownership is NOW safely transferred to newly created transport plug-in
                }
            else
                {
                CleanupStack::PopAndDestroy(pCtx); 
                }
            pTransport = ipVtcpTransport;
            }
        else
            {
            TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, _L8("- About to instantiate new transport (args: cue: '%S', ctx)"), &transportCue ));
            TRAP( leaveCode, pTransport = CSenTransport::NewL(transportCue, pCtx); )
            if( leaveCode )
                {
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Could not load new transport plugin, leave: %d"),leaveCode));
				CleanupStack::Pop(pCtx);				
                User::Leave( leaveCode ); // throw forward
                }
            CleanupStack::Pop(pCtx); // ownership is NOW safely transferred to newly created transport plug-in
            
            }
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- About to instantiate new transport (arg: ctx)");
        // default to HTTP Channel transport plug-in:
        TRAP( leaveCode, pTransport = CSenTransport::NewL(pCtx); )
        if( leaveCode )
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Could not load new transport plugin, leave: %d"),leaveCode));
			CleanupStack::Pop(pCtx);
            User::Leave( leaveCode ); // throw forward
            }
        CleanupStack::Pop(pCtx); // ownership is NOW safely transferred to newly created transport plug-in
        }

    CleanupStack::PushL(pTransport);

    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" aInitializer.DescriptionClassType() == %d "),aInitializer.DescriptionClassType() )); 

    if( aInitializer.DescriptionClassType() == MSenServiceDescription::EWSDescription )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"Class type is: MSenServiceDescription::EWSDescription.");
        TUint32 desiredIapId(0);
        TInt retVal = aInitializer.IapId(desiredIapId);
        if(retVal==KErrNone)
            {
            CSenTransportProperties* pProperties = CSenTransportProperties::NewLC();
            pProperties->SetIapIdL(desiredIapId);
            HBufC8* pPropertiesAsXml = pProperties->AsUtf8LC();
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Consumer Policy -layer's Transport Properties as XML:");
            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*pPropertiesAsXml));
            pTransport->SetPropertiesL(*pPropertiesAsXml, MSenLayeredProperties::ESenProviderSessionLayer, NULL);
            CleanupStack::PopAndDestroy(pPropertiesAsXml);
            CleanupStack::PopAndDestroy(pProperties);
            }
        }
    else if( aInitializer.DescriptionClassType() == MSenServiceDescription::EWSPattern )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"Class type is: MSenServiceDescription::EWSPattern.");
        CSenWSPattern* pConsumerPolicy = (CSenWSPattern*)&aInitializer;

        TUint32 desiredIapId(0);
        TInt retVal = pConsumerPolicy->ConsumerIapId(desiredIapId);
        if(retVal==KErrNone)
            {
            CSenTransportProperties* pProperties = CSenTransportProperties::NewLC();
            pProperties->SetIapIdL(desiredIapId);
            HBufC8* pPropertiesAsXml = pProperties->AsUtf8LC();
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Consumer Policy -layer's Transport Properties as XML:");
            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(*pPropertiesAsXml));
            pTransport->SetPropertiesL(*pPropertiesAsXml, MSenLayeredProperties::ESenConsumerSessionLayer, NULL);
            CleanupStack::PopAndDestroy(pPropertiesAsXml);
            CleanupStack::PopAndDestroy(pProperties);
            }
        retVal = aInitializer.IapId(desiredIapId);
        if(retVal==KErrNone)
            {
            CSenTransportProperties* pProperties = CSenTransportProperties::NewLC();
            pProperties->SetIapIdL(desiredIapId);
            HBufC8* pPropertiesAsXml = pProperties->AsUtf8LC();
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Provider Policy -layer's Transport Properties as XML:");
            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(*pPropertiesAsXml));
            pTransport->SetPropertiesL(*pPropertiesAsXml, MSenLayeredProperties::ESenProviderSessionLayer, NULL);
            CleanupStack::PopAndDestroy(pPropertiesAsXml);
            CleanupStack::PopAndDestroy(pProperties);
            }
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Initializer SD's type is unknown!");
        }
#endif // _SENDEBUG     

    // Check if Serene-wide proxy settings have been defined:
    const TDesC8& proxyHost = iDAO->ProxyHost();
    const TInt proxyPort = iDAO->ProxyPort();

    if ( proxyHost.Length() > 0 || proxyPort > 0 )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Using proxy settings to get HTTP transport, but without policy info.");

        CSenTransportProperties* pProperties = CSenTransportProperties::NewLC();
        pProperties->SetProxyHostL(proxyHost);
        pProperties->SetProxyPortL(proxyPort);
        HBufC8* pPropertiesAsXml = pProperties->AsUtf8LC();
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Serene-wide -layer's Transport Properties as XML:");
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*pPropertiesAsXml));
        pTransport->SetPropertiesL(*pPropertiesAsXml, MSenLayeredProperties::ESenConsumerSessionLayer, NULL);
        CleanupStack::PopAndDestroy(pPropertiesAsXml);
        CleanupStack::PopAndDestroy(pProperties);
        }

    // Set framework specific transport properties

    TPtrC8 frameworkID(aInitializer.FrameworkId());
    if ( frameworkID.Length()== 0 && apSession)
        {
        frameworkID.Set(apSession->FrameworkId());
        }
    if ( frameworkID.Length() > 0 )
        {
        MSIF* pFramework = Framework(frameworkID);
        if ( pFramework )
            {
            TInt setRetVal = pFramework->SetTransportPropertiesL(*pTransport);
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- pFramework->SetTransportPropertiesL returned: %d"), setRetVal )); 
            }
        }
#ifdef _SENDEBUG   
    else
        {     
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel," - Initializer SD's Framework ID is zero-length!");    
        }
#endif // _SENDEBUG
#ifdef _SENDEBUG
    HBufC8* pEffectiveProperties = NULL;
    TInt effRetVal = pTransport->PropertiesL(pEffectiveProperties);
    CleanupStack::PushL(pEffectiveProperties);
    if( effRetVal == KErrNone && pEffectiveProperties )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- Effective Transport Properties (flattened) as XML:");    
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*pEffectiveProperties));
        }
    CleanupStack::PopAndDestroy(pEffectiveProperties);
#endif // _SENDEBUG

    CleanupStack::Pop(pTransport);
/*    
    if( isVtcp )
        {
        ipVtcpTransport = pTransport;
        ipVtcpEndpoint = aInitializer.Endpoint().AllocL();
        }
*/
    return pTransport;
    }

MSenProvider& CSenCoreServiceManager::LookupHostletForL(const TDesC8& aHostletEndpoint,
                                                         const TDesC& aReqThreadId,
                                                         const TDesC8& aReqConsumerId)
    {
    // LookupHostletForL() method needs to be wrapped inside critical section
    // NOTE: do *NOT* file log before this line(!):
    iDAO->iCriticalSection.Wait(); // will pass through only one thread at the time
    MSenProvider& provider = iDAO->LookupHostletForL(aHostletEndpoint, aReqThreadId, aReqConsumerId);
    iDAO->iCriticalSection.Signal(); // allow next, pending thread to proceed, if such exists
    return provider;
    }

TInt CSenCoreServiceManager::ReleaseHostletL(const MSenProvider* aHostlet,
                                             const TDesC& aReqThreadId,
                                             const TDesC8& aReqConsumerId)
    {
    // LookupHostletForL() method needs to be wrapped inside critical section
    // NOTE: do *NOT* file log before this line(!):
    iDAO->iCriticalSection.Wait(); // will pass through only one thread at the time
    TInt retVal = iDAO->ReleaseHostletL(aHostlet, aReqThreadId, aReqConsumerId);
    iDAO->iCriticalSection.Signal(); // allow next, pending thread to proceed, if such exists
    return retVal;
    }

RStringPool& CSenCoreServiceManager::StringPool()
    {
    return iStringPool;
    }

// Helper
/*
RPointerArray<HBufC8>& CSenCoreServiceManager::ActiveHostletEndpointsL()
    {
    if(!ipActiveHostletEndpoints)
        {
        ipActiveHostletEndpoints = new (ELeave) RPointerArray<HBufC8>;
        }
    return *ipActiveHostletEndpoints;
    }


TInt CSenCoreServiceManager::AddActiveHostletConnectionEndpointL(const TDesC8& aEndpoint)
    {
    LOG_WRITEFORMAT((_L8("CSenCoreServiceManager::AddActiveHostletConnectionEndpointL(%S)"), &aEndpoint));
    RPointerArray<HBufC8>& active = ActiveHostletEndpointsL();
    TInt count(active.Count());
    for(TInt i=0; i<count; i++)
        {
        HBufC8* pEndpoint = active[i];
        if(pEndpoint && *pEndpoint == aEndpoint)
            {
            LOG_WRITE_L("- Endpoint already reserved.");
            return KErrSenEndpointReserved; // from SenHostletConnection.h
            }
        }

    User::LeaveIfError(active.Append(aEndpoint.AllocL()));
    return KErrNone;
    }

TInt CSenCoreServiceManager::RemoveActiveHostletConnectionEndpoint(const TDesC8& aEndpoint)
    {
    LOG_WRITEFORMAT((_L8("CSenCoreServiceManager::RemoveActiveHostletConnectionEndpoint(%S):"), &aEndpoint));
    TInt retVal(KErrNotFound);
    RPointerArray<HBufC8>& active = ActiveHostletEndpointsL();
    TInt count(active.Count());
    TInt i = 0;
    for(; i<count; i++)
        {
        HBufC8* pEndpoint = active[i];
        if(pEndpoint && *pEndpoint == aEndpoint)
            {
            LOG_WRITEFORMAT((_L8("- Match (%d) in the list of active endpoints found!"), i));
            break; // match
            }
        }
    if(i<count)
        {
        // match occured, remove the endpoint
        LOG_WRITEFORMAT((_L8("- Removing (%d) the endpoint now."), i));
        active.Remove(i);
        retVal = KErrNone;
        }
    return retVal;
    }
*/

TInt CSenCoreServiceManager::NextTransactionId()
    {
    iNextTransactionId++;
    return iNextTransactionId;
    }

TInt CSenCoreServiceManager::SaveCredentialDB()
    {
    return iCredentialManager->SaveCredentialDB();
    }

TInt CSenCoreServiceManager::CredentialsL(const CSenWSDescription& aPattern,
								          RSenCredentialArray& aCredentials)
    {
    return iCredentialManager->CredentialsL(aPattern, aCredentials);
    }

TInt CSenCoreServiceManager::CredentialsL(const CSenWSDescription& aPattern,
					                      const CSenIdentityProvider& aIdP,
								          RSenCredentialArray& aCredentials)
    {
    return iCredentialManager->CredentialsL(aPattern, aIdP, aCredentials);
    }

TInt CSenCoreServiceManager::CredentialsL(const CSenWSDescription& aPattern,
								          RSenCredentialPtrArray& aCredentials)
    {
    return iCredentialManager->CredentialsL(aPattern, aCredentials);
    }

TInt CSenCoreServiceManager::CredentialsL(const CSenWSDescription& aPattern,
					                      const CSenIdentityProvider& aIdP,
								          RSenCredentialPtrArray& aCredentials)
    {
    return iCredentialManager->CredentialsL(aPattern, aIdP, aCredentials);
    }

RSenCredentialPtr CSenCoreServiceManager::AddCredentialL(CSenInternalCredential* apCredential,
                                                          TInt& aErrorTo)
    {
    return iCredentialManager->AddCredentialL(apCredential, aErrorTo);
    }

RSenCredentialPtr CSenCoreServiceManager::AddCredentialL(CSenIdentityProvider* apIdP,
                                                         CSenInternalCredential* apCredential,
                                                         TInt& aErrorTo)
    {
    return iCredentialManager->AddCredentialL(apIdP, apCredential, aErrorTo);
    }

RSenCredentialPtr CSenCoreServiceManager::AddCredentialL(const TDesC8& aCredential,
                                                         TInt& aErrorTo)
    {
    return iCredentialManager->AddCredentialL(aCredential, aErrorTo);
    }

RSenCredentialPtr CSenCoreServiceManager::AddCredentialL(CSenIdentityProvider* apIdP,
	        								              const TDesC8& aCredential,
			        						              TInt& aErrorTo)
    {
    return iCredentialManager->AddCredentialL(apIdP, aCredential, aErrorTo);
    }

TInt CSenCoreServiceManager::RemoveCredentialsL(const CSenWSDescription& aPattern)
    {
    return iCredentialManager->RemoveCredentialsL(aPattern);
    }

TInt CSenCoreServiceManager::RemoveCredentialsL(const CSenWSDescription& aPattern,
						                        const CSenIdentityProvider& aIdP)
    {
    return iCredentialManager->RemoveCredentialsL(aPattern, aIdP);
    }

TInt CSenCoreServiceManager::RemoveCredentialsL(const TDesC8& aProviderId)
    {
    return iCredentialManager->RemoveCredentialsL(aProviderId);
    }

TInt CSenCoreServiceManager::RemoveCredentialL(TInt aInternalCredentialId)	//codescannerwarnings
    {
    return iCredentialManager->RemoveCredentialL(aInternalCredentialId);	//codescannerwarnings
    }

RSenCredentialPtr CSenCoreServiceManager::CredentialL(TInt aInternalCredentialId,
                                                      TInt& aErrorTo)
    {
    return iCredentialManager->CredentialL(aInternalCredentialId, aErrorTo);
    }

RSenCredentialPtr CSenCoreServiceManager::UpdateCredentialL(TInt aInternalCredentialId,
                                                     CSenInternalCredential* apCredential,
                                                     TInt& aErrorTo)
    {
    return iCredentialManager->UpdateCredentialL(aInternalCredentialId,
                                                 apCredential,
                                                 aErrorTo);
    }

RSenCredentialPtr CSenCoreServiceManager::UpdateCredentialL(TInt aInternalCredentialId,
                                                            const TDesC8& aCredential,
                                                            TInt& aErrorTo)
    {
    return iCredentialManager->UpdateCredentialL(aInternalCredentialId,
                                                 aCredential,
                                                 aErrorTo);
    }

TInt CSenCoreServiceManager::FindMatchingIdentityProviderL(CSenIdentityProvider &aIdP,
                                                           CSenIdentityProvider*& apMatch)
    {
    return iIdentityManager->FindMatchingIdentityProviderL(aIdP, apMatch);
    }

TInt CSenCoreServiceManager::UpdateIdentityProviderL(CSenIdentityProvider& aIdp)
    {
    return iIdentityManager->UpdateIdentityProviderL(aIdp);
    }

TInt CSenCoreServiceManager::NextConnectionID()
    {
    return iConnectionID++;
    }
void CSenCoreServiceManager::UpdateTouchL(CSenServiceSession* aSession)
	{
	MSenServiceDescription& sd = aSession->AsServiceDescription();
	
	iIdentityManager->UpdateTouchIdentityDBL(sd);
	iCredentialManager->UpdateTouchCredDBL(sd);
	iDAO->UpdateTouchSessionDBL(sd);

	}
TPtrC8 CSenCoreServiceManager::IMSI()
    {
    return iIMSI;    
    }
/*
void CSenCoreServiceManager::GetIMSIL()
    {
    TBuf<15> phoneImsi;
    
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );

    RMobilePhone phone;
    User::LeaveIfError( server.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( phone.Open( server, KMmTsyPhoneName ) );
    CleanupClosePushL( phone );
    User::LeaveIfError( phone.Initialise() );

    TRequestStatus status;    
    phone.GetSubscriberId( status, phoneImsi );
    User::WaitForRequest( status );
    //User::LeaveIfError( status.Int() );
    
    iIMSI.Copy(phoneImsi);
    
    status = KErrNone;

    CleanupStack::PopAndDestroy( 2 ); // phone, server
    }
*/    
TPtrC8 CSenCoreServiceManager::IMEI()
	{
    return iIMEI;		
	}
		
void CSenCoreServiceManager::GetImsiImeiL()
    {
	CREDLOG_L(KSenCredsLogLevelMin,    "GetImsiImeiL Entered");    	
    TBuf<15> phoneImsi;
    TBuf<RMobilePhone::KPhoneSerialNumberSize> phoneImei;
    RMobilePhone::TMobilePhoneIdentityV1 phoneIdV1;
        
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );

    RMobilePhone phone;
    User::LeaveIfError( server.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( phone.Open( server, KMmTsyPhoneName ) );
    CleanupClosePushL( phone );
    User::LeaveIfError( phone.Initialise() );

	CREDLOG_L(KSenCredsLogLevelMin,    "GetImsiImeiL RMobilePhone Initialized");

    TRequestStatus status;
    phone.GetSubscriberId( status, phoneImsi );    
    User::WaitForRequest( status );
	status = KErrNone;
	CREDLOG_L(KSenCredsLogLevelMin,    "GetImsiImeiL GetSubscriberId Completed");		
		
    phone.GetPhoneId( status, phoneIdV1 );
    phoneImei = phoneIdV1.iSerialNumber;
    User::WaitForRequest( status ); 
	status = KErrNone;          
    //User::LeaveIfError( status.Int() );
	CREDLOG_L(KSenCredsLogLevelMin,    "GetImsiImeiL phoneIdV1 Completed");    
    
    iIMSI.Copy(phoneImsi);    
    iIMEI.Copy(phoneImei);
    
    status = KErrNone;

    CleanupStack::PopAndDestroy( 2 ); // phone, server
    }
//END OF FILE
