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
#include "idwsfauthenticationserviceclient.h"

#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>

#include <MSenServiceDescription.h>
#include <SenServiceConnection.h>
#include <SenXmlUtils.h>

#include "idwsfsaslmessage.h"
#include "idwsfmessage.h"  // for response parsing and request building
#include "idwsfdsqueryresponse.h"
#include "idwsfdiscoveryserviceclient.h"
#include "idwsfservicesession.h"

#include "msencoreservicemanager.h"
#include "sendebug.h" // internal Utils\inc - filelogging MACROs
#include "senlogger.h"

CIdWsfAuthenticationServiceClient* CIdWsfAuthenticationServiceClient::NewL(
                                                        CSIF& aSIF)//,
                                                        //RFileLogger& aLogger)
    {
    CIdWsfAuthenticationServiceClient* pNew = NewLC(aSIF);//, aLogger);
    CleanupStack::Pop();
    return pNew;
    }

CIdWsfAuthenticationServiceClient* CIdWsfAuthenticationServiceClient::NewLC(
                                                        CSIF& aSIF)//,
                                                        //RFileLogger& aLogger)
    {
    CIdWsfAuthenticationServiceClient* pNew =
        new (ELeave) CIdWsfAuthenticationServiceClient(
                    aSIF,
                    MSenServiceDescription::EIdWsfAuthenticationServiceClient);//,
                    //aLogger);

    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


CIdWsfAuthenticationServiceClient::CIdWsfAuthenticationServiceClient(
                                                CSIF& aSIF,
                                                TDescriptionClassType aType)//,
                                                //RFileLogger& aLogger)
    : CIdWsfDiscoveryServiceClient(aSIF, aType),// aLogger),
    iAuthenticator(NULL),
    iDsSession(NULL),
    iAttempts(0),
    iMaxAttempts(1)
    {
    }

void CIdWsfAuthenticationServiceClient::BaseConstructL()
    {
    CIdWsfDiscoveryServiceClient::BaseConstructL();
		TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("CIdWsfAuthenticationServiceClient::ConstructL()")));
    }

CIdWsfAuthenticationServiceClient::~CIdWsfAuthenticationServiceClient()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,
    "CIdWsfAuthenticationServiceClient::~CIdWsfAuthenticationServiceClient");
    delete iAuthenticator;
    }

TInt CIdWsfAuthenticationServiceClient::ValidateL(
                                        CIdWsfServiceSession& aSession,
                                        MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfAuthenticationServiceClient::ValidateL()");
    return CIdWsfAuthenticationServiceClient::ValidateL(aSession, 
                                                        KNullDesC8(), 
                                                        aRemoteConsumer);
    }


TInt CIdWsfAuthenticationServiceClient::ValidateL(
                                    CIdWsfServiceSession &aSession,
                                    const TDesC8& /*authnContext*/,
                                    MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfAuthenticationServiceClient::ValidateL(..)");
    // keep this as first line
    iServedConsumer = &aRemoteConsumer;

    if(!ipAccount)
        {
        return KErrSenNoEndpoint; 
        }

    TInt retVal(KErrNone);          // for returning error codes
    iCState = EAuthServiceClient;   // set state
    iDsSession = &aSession;         // save session
    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"- starting transaction.");
    StartTransaction();

    TPtrC8 mechanismNames =
                        Framework().Manager().SenSecurityMechanismNames8L();

    CIdWsfSaslMessage* pInitMessage =
        CIdWsfSaslMessage::NewL(Framework().Manager(),
                                KSaslXmlns,
                                KSaslRequestName,
                                KSaslRequestQName);
    CleanupStack::PushL(pInitMessage); // push pInitMessage

    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"- constructing initial request.");
    pInitMessage->ConstructInitRequestFromL(mechanismNames, *ipAccount);

    HBufC8* pInitRequest8 = pInitMessage->AsXmlL();
    CleanupStack::PushL(pInitRequest8); // push pInitRequest

    if(iService)
        {
        HBufC8* pResponse8 = NULL;

        HBufC8* pNextMessage = NULL;

#ifdef _SENDEBUG
        TPtr8 initRequest = pInitRequest8->Des();
//wslog        FILELOGALL(_L("SenCoreServiceManager"), _L("AS_init_req.xml"), initRequest);
#endif // _SENDEBUG

        retVal = iService->SubmitL(*pInitRequest8, KNullDesC8, *this, pResponse8);
        aRemoteConsumer.SetDataTrafficDetails(iAuthDetails); 
        CleanupStack::PushL(pResponse8); // pResponse8

        if(retVal == KErrNone && pResponse8)
            {
#ifdef _SENDEBUG
            TPtr8 initResponse = pResponse8->Des();
//wslog            FILELOGALL(_L("SenCoreServiceManager"), _L("AS_init_rsp.xml"), initResponse);
#endif // _SENDEBUG
            retVal = ParseResponseL(*pResponse8, pNextMessage);
            }
#ifdef _SENDEBUG
        else
            {
            CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfAuthenticationServiceClient::ValidateL:");
            CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("- SubmitL failed: %d"), retVal));
            }
#endif // _SENDEBUG

        CleanupStack::PopAndDestroy(1); // pResponse8

        while ( pNextMessage )
            {
#ifdef _SENDEBUG
            TPtr8 authRequest = pNextMessage->Des();
//wslog            FILELOGALL(_L("SenCoreServiceManager"), _L("AS_req.xml"), authRequest);
#endif // _SENDEBUG
            CleanupStack::PushL(pNextMessage); // pNextMessage
            retVal = iService->SubmitL(*pNextMessage, KNullDesC8, *this, pResponse8); 
            aRemoteConsumer.SetDataTrafficDetails(iAuthDetails); 	
            CleanupStack::PopAndDestroy(); // pNextMessage
            pNextMessage = NULL;

            if (retVal == KErrNone && pResponse8)
                {
                CleanupStack::PushL(pResponse8); // pResponse8
#ifdef _SENDEBUG
                TPtr8 authResponse = pResponse8->Des();
//wslog                FILELOGALL(_L("WsLog"), _L("AS_rsp.xml"), authResponse);
#endif // _SENDEBUG
                retVal = ParseResponseL(*pResponse8, pNextMessage);
                CleanupStack::PopAndDestroy(); // pResponse8
                }
            }
        }
    CleanupStack::PopAndDestroy(2); // pInitRequest8, pInitMessage
    
    if ((retVal == KErrNone) && iDsSession->IsReadyL())
        {
         CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"- starting transaction.");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- setting validator for DS session.");

        iDsSession->SetValidator(this);
        }

    return retVal;
    }


TInt CIdWsfAuthenticationServiceClient::ParseResponseL(TDesC8& aMessage,
                                                       HBufC8*& aParsedMessage)
    {
    TInt retVal(KErrNone); 

    HBufC8* pNextMessage = NULL;

    CSenSecurityMechanism* pAuthenticator = NULL;
    CIdWsfSaslMessage* pSaslResponse =
                            CIdWsfSaslMessage::NewL(Framework().Manager());
    CleanupStack::PushL(pSaslResponse);

    pSaslResponse->SetReader(*Framework().Manager().XMLReader());
    pSaslResponse->BuildFrom(aMessage);

    if(pSaslResponse->IsOk())
        {
        iCState = EHasSession;
        TransactionCompleted();
        iSession = iDsSession;  // The "stateObject" in Java is
                                // a "iSession" in here
        delete iAuthenticator;
        iAuthenticator = NULL;
        }


    if(iCState == EAuthServiceClient)
        {
        // expect a SASLResponse and look for the Status,
        // if "continue" instantiate a SASLMechanism
        // and let it deal with the message
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("Parse Response - EAuthServiceClient")));

        TPtrC8 mechName = pSaslResponse->Mechanism();

        if(pSaslResponse->IsContinue())
            {
            pAuthenticator = Framework().Manager().MechanismNamedL(mechName);
            if(pAuthenticator)
                {
                // mechanism ECOM plugin found...

                pAuthenticator->SetTransforms(pSaslResponse->Transforms());
                
                if (ipAccount->Password() == KNullDesC8)
                    {
                    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("Password will be asked from user.")));
                    iAttempts++;
                    }
                
                // This may invoke username/password UI prompt
                pAuthenticator->SetIdentity(ipAccount, &Framework().Manager());

                //if (pAuthenticator->IsPasswordFromUser() &&
                //  pAuthenticator->MaxAttempts()>0)
                if (pAuthenticator->MaxAttempts()>0)
                    {
                    //iAttempts++;
                    iMaxAttempts = pAuthenticator->MaxAttempts();
                    }
                delete iAuthenticator;
                iAuthenticator = NULL;
                iAuthenticator = pAuthenticator;
                iCState = ESaslMechanism;
                }
            else
                {
                retVal = KErrNotFound;
                }
            }
        if(pSaslResponse->IsAbort())
            {
            // if response is "abort" that means there is no matching
            // SASLMechanism and we should quite the whole process
            TransactionCompleted();
            pNextMessage = NULL;
            }
        }

    if(iCState == ESaslMechanism)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("Parse Response - ESaslMechanism")));
        pAuthenticator = iAuthenticator;

        if (pSaslResponse->IsAbort())
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("SASL Response: Abort")));
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L("      Attempts : %d, Max Attempts : %d"),
                                iAttempts, iMaxAttempts));
            // we could retry if allowed, otherwise quit the process
            if (iAttempts < iMaxAttempts)
                {
                pAuthenticator->ClearPasswordL();
                iAttempts++;    // even if we cannot get a new password
                                // we count this as an attempt!

                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("ESaslMechanism - Create a new init request")));

                CIdWsfSaslMessage* pInitMessage =
                    CIdWsfSaslMessage::NewL(Framework().Manager(),
                                            KSaslXmlns,
                                            KSaslRequestName,
                                            KSaslRequestQName);
                CleanupStack::PushL(pInitMessage); // push pInitMessage
                pInitMessage->ConstructInitRequestFromL(iAuthenticator->Name(),
                                                        *ipAccount);
                pNextMessage = pInitMessage->AsXmlL();
                CleanupStack::PopAndDestroy(); // pInitMessage
                CleanupStack::PushL(pNextMessage);
                }
            else
                {
                TransactionCompleted();
                }
            }
        else if (pSaslResponse->IsContinue())
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("SASL Response: Continue")));
            TPtrC8 mechanism = pSaslResponse->Mechanism();
            TPtrC8 authName = pAuthenticator->Name();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
                _L8(" Mechanism : %S, Authenticator Name : %S"), &mechanism,
                                                                 &authName
                ));

            if(mechanism != authName)
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,(
                _L("pSaslResponse->Mechanism() != pAuthenticator->Name()")));
                //switch to another mechanism
                pAuthenticator = Framework().Manager().MechanismNamedL(
                                                pSaslResponse->Mechanism());
                if(pAuthenticator)
                    {
                    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("Delete iAuthenticator")));
                    delete iAuthenticator; // free memory

                    // no need to delete after initialize from is in use,
                    // because iAuthenticator == pAuthenticator !!
                    iAuthenticator = pAuthenticator;
                    }
                else
                    {
                    retVal = KErrNotFound;
                    }
                }
            if(pAuthenticator)
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,
                "ESaslMechanism - Handle response and create a new request");
                CIdWsfSaslMessage* pSaslRequest =
                    CIdWsfSaslMessage::NewL(Framework().Manager(),
                                            KSaslXmlns,
                                            KSaslRequestName,
                                            KSaslRequestQName);

                CleanupStack::PushL(pSaslRequest); // pSaslRequest

                retVal = pAuthenticator->HandleResponseL(*pSaslResponse,
                                                            *pSaslRequest);
                pNextMessage = pSaslRequest->AsXmlL();
                CleanupStack::PopAndDestroy();  // pSaslRequest
                CleanupStack::PushL(pNextMessage);
                }
            else
                {
                retVal = KErrNotFound;
                }
            } // end IsContinue()
        }

    if(iCState == EHasSession)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("Parse Response - EHasSession")));
        // expect a DSQueryResponse with the ResourceOffering for the DS
        retVal = CIdWsfDiscoveryServiceClient::ProcessResponseL(pSaslResponse);

        if (iService)
            {
            iDsSession->SetClientServerIntervalL(
                                            iService->ClientServerInterval());// codescannerwarnings
            }

        if ( iService && iDsSession && iDsSession->IsReadyL())
            {
            TPtrC8 providerId = iService->ProviderId();
            iDsSession->SetTrustAnchorL(providerId);
            iCState = ENotInitialized;  // check if iSession should
                                        // be deleted here!?
            }
        }

    if(pNextMessage)
        {
        CleanupStack::Pop(); // pNextMessage
        }

    CleanupStack::PopAndDestroy(); // pSaslResponse
    aParsedMessage = pNextMessage;  // assign data pointer

    return retVal;
    }


void CIdWsfAuthenticationServiceClient::SetStatusL(const TInt /*aStatus */)
    {
    // No implementation in Java
    }

void CIdWsfAuthenticationServiceClient::SetAccount(
                                    CSenIdentityProvider* aIdentityProvicer)
    {
    ipAccount = aIdentityProvicer;
    }


TBool CIdWsfAuthenticationServiceClient::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EIdWsfDiscoveryServiceClient ) // direct superclass!    
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CIdWsfDiscoveryServiceClient::HasSuperClass( aType ); 
        }
    }

void CIdWsfAuthenticationServiceClient::SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) 
	{
	iAuthDetails = aDetails;
	}

// End of file


