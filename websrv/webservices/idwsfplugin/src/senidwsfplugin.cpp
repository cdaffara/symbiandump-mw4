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
#include <w32std.h>

#include <SenServiceConnection.h> // error codes and framework ids
#include <SenBaseFragment.h>
#include <MSenServiceDescription.h>

#include "msencoreservicemanager.h"
#include "msentransport.h"
#include "sendebug.h"
#include "senlogger.h"

#include "senidwsfplugin.h"
#include "idwsfdiscoveryserviceclient.h"
#include "idwsfservicesession.h"
#include "idwsfcoreserviceconsumer.h"
#include "midwsfsessionvalidator.h"

#include <SenTransportProperties.h>
#include <SenSoapConstants.h>


// Create instance of concrete ECOM interface implementation
CSenIdWsfPlugin* CSenIdWsfPlugin::NewL(TAny* aManager)
    {
    MSenCoreServiceManager* manager =
                        reinterpret_cast<MSenCoreServiceManager*>(aManager);

    CSenIdWsfPlugin* self = new (ELeave) CSenIdWsfPlugin(*manager);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Implementation of CSIF

// Constructor
CSenIdWsfPlugin::CSenIdWsfPlugin(MSenCoreServiceManager& aManager)
:   iManager(aManager)
    {
    }

CSenIdWsfPlugin::~CSenIdWsfPlugin()
    {
    //CSenIdWsfPlugin::Log()->Write(_L8("CSenIdWsfPlugin::~CSenIdWsfPlugin()"));
    }

// Second phase construction.
void CSenIdWsfPlugin::ConstructL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::ConstructL - Version 2 [2006-05-09]");

    // Results to add a DUPLICATE xmlns in parse!
    BaseConstructL(KIdWsfFrameworkXMLNS, KIdWsfFrameworkLocalname);
    }

// New methods

/*
RFileLogger* CSenIdWsfPlugin::Log() const
    {
    return iManager.Log();
    }*/

//////////////////////////////////////////////////////////////////////////


// Implementation of CSIF

/**
* Return a system wide unique string to identify this framework
*
* @return TDesC8 that uniquely identifies this framework.
*/
const TDesC8& CSenIdWsfPlugin::Id()
    {
    return KDefaultIdWsfFrameworkID();
    }
    
TInt CSenIdWsfPlugin::CompareSDScore(const TSDScore& aSDScoreLeft,
                                                const TSDScore& aSDScoreRight)
    {
     //"greater" means better matchScore, i.e. further down in a list!
    return (aSDScoreLeft.iScore - aSDScoreRight.iScore);
    }

/**
* Try to find one or more services that match the given ServiceDescription.
* Register ServiceDescriptions for these to the owner of the framework,
* i.e. to the ServiceManager. Return the number of ServiceDescriptions that
* were added.
*
* @param    aPattern - A ServiceDescriptin that should be treated as a pattern
* @param    aRemoteConsumer - the consumer sending the request
* @param    aErrorMsg - in case that return value is negative, this may contain
*           error as descriptor (SOAP Fault etc)
*
*   return (TInt) aCount of matching ServiceDescriptions that were added
*/
TInt CSenIdWsfPlugin::AddServiceDescriptionL( MSenServiceDescription& aPattern,
                                              MSenRemoteServiceConsumer& aRemoteConsumer,
                                              HBufC8*& /* aErrorMsg */ )
    {
    CSLOG(aRemoteConsumer.ConnectionId() , KMinLogLevel,(_L("Entered CSenIdWsfPlugin::AddServiceDescriptionL")));

    if( aPattern.Contract().Length() == 0 
        || ( aPattern.FrameworkId() != KDefaultIdWsfFrameworkID &&
             aPattern.FrameworkId() != KNullDesC8 ) )
        {
        return 0;
        }


#ifdef _SENDEBUG
    HBufC8* pAsXml = aPattern.AsXmlL();
    if(pAsXml)
        {
        CleanupStack::PushL(pAsXml);
        CSLOG_ALL(aRemoteConsumer.ConnectionId()  , KMaxLogLevel,(*pAsXml));
        CleanupStack::PopAndDestroy(); // pAsXml
        }
#endif

    RArray<TSDScore> searchResults;
    RPointerArray<CSenWSDescription> serviceDescriptions;
    RPointerArray<CSenWSDescription> tempArray;

    CleanupClosePushL(searchResults);
    CleanupClosePushL(serviceDescriptions);
    CleanupClosePushL(tempArray);
    
    TLinearOrder<TSDScore> order(&CSenIdWsfPlugin::CompareSDScore);

    TInt retVal =
        Manager().ServiceDescriptionsL(serviceDescriptions,
                                    KIdWsfFrameworkDiscoveryServiceContract);

    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(3); // close the RPointerArrays
        return 0; // zero SD's were added
        }

    CIdWsfDiscoveryServiceClient* pDSClient = NULL;
    CSenIdentityProvider* pIdentityProvider = IdentityProviderL(&aPattern);

    //////////////////////////////////////////////////////////////////////////

    if(!pIdentityProvider)
        {
        CleanupStack::PopAndDestroy(3); // close the RPointerArrays
        return 0;//no possibility to add any SD as long as NO IdP is available!
        }
    //////////////////////////////////////////////////////////////////////////

    TInt attempts = 0;

    RPointerArray<CSenWSDescription> invalidDSClients;
    CleanupClosePushL(invalidDSClients);
    MSenServiceDescription *pDSDescription = NULL;

    TInt i = 0;
    while((i < serviceDescriptions.Count()) && (searchResults.Count() < 1))
        {
        pDSDescription = (MSenServiceDescription*) serviceDescriptions[i];
        if(pDSDescription->DescriptionClassType() ==
                    MSenServiceDescription::EIdWsfDiscoveryServiceClient)
            {
            pDSClient = (CIdWsfDiscoveryServiceClient*) pDSDescription;
            if(pDSClient->IsReadyL())
                {
                if( (!pIdentityProvider &&
                            (pDSClient->TrustAnchor() == KNullDesC8())) ||
                     (pIdentityProvider &&
                            (pDSClient->TrustAnchor() ==
                                    pIdentityProvider->ProviderID()) ))
                    {
                    pDSClient->FindServiceL(tempArray, 
                                            aPattern, 
                                            aRemoteConsumer);
                    attempts++;
                    for(TInt j = 0; j < tempArray.Count(); j++)
                        {
                        TSDScore temp;
                        temp.iSD = tempArray[j];
                        temp.iScore = tempArray[j]->ScoreMatchL(aPattern);
                        searchResults.Append(temp);
                        //searchResults.Append(tempArray[j]);
                        }
                    tempArray.Reset();
                    searchResults.Sort(order);
                    }
                else
                    {
                    invalidDSClients.Append(
                                (CSenWSDescription*)pDSDescription);
                    }
                }
            else
                {
                invalidDSClients.Append(
                                (CSenWSDescription*)pDSDescription);
                }
            }
        else
            {
            invalidDSClients.Append(
                                (CSenWSDescription*)pDSDescription);
            }
        i++;
        }

    if(searchResults.Count() < 1)
        {
        if(attempts < 1)    // no DS trusted by the IDP
                            //that was in the database (senidentities.xml)
            {
            // get a DS with the IdP as trust anchor
            pDSClient = DSClientL(pIdentityProvider, aRemoteConsumer);
            if(pDSClient && pDSClient->IsReadyL())
                {
                pDSClient->FindServiceL(tempArray, 
                                        aPattern,
                                        aRemoteConsumer);
                for(TInt j = 0; j < tempArray.Count(); j++)
                    {
                    TSDScore temp;
                    temp.iSD = tempArray[j];
                    temp.iScore = tempArray[j]->ScoreMatchL(aPattern);
                    searchResults.Append(temp);
                    //searchResults.Append(tempArray[j]);
                    }
                tempArray.Reset();
                searchResults.Sort(order);
                }
            }
        }

    if(searchResults.Count() < 1)
        {
        // try to validate invalid DS descriptions (clients)
        i = 0;
        while((i < invalidDSClients.Count()) && (searchResults.Count() < 1))
            {
            pDSDescription = (MSenServiceDescription*) invalidDSClients[i];
            if(pDSDescription->DescriptionClassType() ==
                        MSenServiceDescription::EIdWsfDiscoveryServiceClient)
                {
                pDSClient = (CIdWsfDiscoveryServiceClient*) pDSDescription;
                if(ValidateDSL(pDSClient,pIdentityProvider, aRemoteConsumer))
                    {
                    retVal = pDSClient->FindServiceL(tempArray, 
                                                     aPattern,
                                                     aRemoteConsumer);
                    CSLOG_FORMAT((aRemoteConsumer.ConnectionId()  , KNormalLogLevel, _L8("DS client FindServiceL() returned: (%d)"),
                        retVal));
                    retVal = 0;

                    for(TInt j = 0; j < tempArray.Count(); j++)
                        {
                        TSDScore temp;
                        temp.iSD = tempArray[j];
                        temp.iScore = tempArray[j]->ScoreMatchL(aPattern);
                        searchResults.Append(temp);
                        //searchResults.Append(tempArray[j]);
                        }
                    searchResults.Sort(order);
                    }
                }
            i++;
            }
        }

    for(i=0; i<searchResults.Count(); i++)
        {
        Manager().AddServiceDescriptionL(searchResults[i].iSD);
        }

    TInt countOfAddedSDs = searchResults.Count();

    // close: invalidDSClients, tempArray, serviceDescriptions, searchResults
    CleanupStack::PopAndDestroy(4);
                                  
	
		
	

    return countOfAddedSDs;
    }

TInt CSenIdWsfPlugin::AddServiceDescriptionL( const TDesC8& aServiceTypeContract,
                                              MSenRemoteServiceConsumer& aRemoteConsumer,
                                              HBufC8*& aErrorMsg )
    {

    /*
    * Ask the DiscoveryService client to search for services of
    * the given type;
    * Add these services to the framework manager.
    */
    CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L("IdWsf-plugin: Searching for session with contract:")));
    CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(aServiceTypeContract));


    MSenServiceDescription *pPattern = CIdWsfServiceInstance::NewLC(aServiceTypeContract);

    TInt count = AddServiceDescriptionL(*pPattern, aRemoteConsumer, aErrorMsg);
    CleanupStack::PopAndDestroy( pPattern  );
    return count;
    }

/**
* Attempt to register the ServiceDescription to the ServiceManager that
* owns the framework.
* This is a callback used by the ServiceManager to give frameworks a chance
* to replace a generic ServiceDescription into something that is more specific,
* and more useful, to the ServiceInvocationFramework.
*
* @param aServiceDescription A ServiceDescription
*
*   return TInt aError - indicates the error or KErrNone if successfull
*/
TInt CSenIdWsfPlugin::RegisterServiceDescriptionL(MSenServiceDescription& aServiceDescription)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::RegisterServiceDescriptionL");

    // Sanity checks

#ifdef _SENDEBUG
    HBufC8* pAsXml = aServiceDescription.AsXmlL();
    if(pAsXml)
        {
        CleanupStack::PushL(pAsXml);
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(*pAsXml));
        CleanupStack::PopAndDestroy(); // ptr
        }
#endif // _SENDEBUG


    TInt retval = KErrNone;

    CIdWsfServiceSession *pSession = NULL;

    TPtrC8 contract = aServiceDescription.Contract();

    if(contract == KIdWsfFrameworkAuthenticationServiceContract)
        {
        retval = RegisterASClientL(&aServiceDescription);
        }
    else if(contract.Length()>0 &&
            (contract == KIdWsfFrameworkDiscoveryServiceContract))
        {
        retval = RegisterDSClientL(&aServiceDescription);
        }
    else
        {
        if(aServiceDescription.DescriptionClassType() ==
                    MSenServiceDescription::EIdWSFServiceSession)
            {
            pSession = (CIdWsfServiceSession*) &aServiceDescription;

            retval = Manager().AddServiceDescriptionL(pSession);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("Creating session from description...")));
#ifdef _SENDEBUG
            TPtrC8 con = aServiceDescription.Contract();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8(" Using Contract: '%S'"), &con));
#endif

            pSession = CIdWsfServiceSession::NewLC(*this);
            retval = pSession->InitializeFromL(aServiceDescription);
            if (retval == KErrNone)
                {
                retval = Manager().AddServiceDescriptionL(pSession);
                CleanupStack::Pop(pSession);
                }
            else
                {
                CleanupStack::PopAndDestroy(pSession);
                }
            }
        }

    return retval;
    }

TInt CSenIdWsfPlugin::UnregisterServiceDescriptionL(
                                MSenServiceDescription& aServiceDescription)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("UnRegisterServiceDescriptionL(.)")));


#ifdef _SENDEBUG
    HBufC8* pAsXml = aServiceDescription.AsXmlL();
    if(pAsXml)
        {
        CleanupStack::PushL(pAsXml);
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(*pAsXml));
        CleanupStack::PopAndDestroy(); // ptr
        }
#endif // _SENDEBUG


    TInt retval = KErrNone;
    CIdWsfServiceSession *pSession = NULL;
    TPtrC8 contract = aServiceDescription.Contract();

    if(contract == KIdWsfFrameworkAuthenticationServiceContract)
        {
        retval = UnRegisterASClientL(&aServiceDescription);
        }
    else if(contract.Length()>0 &&
        (contract == KIdWsfFrameworkDiscoveryServiceContract))
        {
        retval = UnRegisterDSClientL(&aServiceDescription);
        }
    else
        {
        if(aServiceDescription.DescriptionClassType() ==
                MSenServiceDescription::EIdWSFServiceSession)
            {
            pSession = (CIdWsfServiceSession*) &aServiceDescription;
            retval = Manager().RemoveServiceDescriptionL(*pSession);
            }
        else
            {
#ifdef _SENDEBUG
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Creating session from description...");
            TPtrC8 con = aServiceDescription.Contract();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("-contract: '%S'"), &con));
#endif // _SENDEBUG

            pSession = CIdWsfServiceSession::NewLC(*this);
            retval = pSession->InitializeFromL(aServiceDescription);
            if (retval == KErrNone)
                {
                retval = Manager().RemoveServiceDescriptionL(*pSession);
                }
            CleanupStack::PopAndDestroy(pSession);
            }
        }

    return retval;
    }

TInt CSenIdWsfPlugin::UnRegisterASClientL(
                                MSenServiceDescription *aServiceDescription)
    {
#ifdef _SENDEBUG
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::UnRegisterASClientL");
     TPtrC8 endpoint = aServiceDescription->Endpoint();
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(endpoint));
#endif

    TInt retval = KErrNone;

    CIdWsfServiceSession *pSession = NULL;
    CIdWsfAuthenticationServiceClient *pASClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                MSenServiceDescription::EIdWsfAuthenticationServiceClient)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("This ServiceDescription is already an AS Client")));
        pASClient = (CIdWsfAuthenticationServiceClient*)aServiceDescription;

        retval = Manager().RemoveServiceDescriptionL(*pASClient);
        }
    else
        {
        pSession = CIdWsfServiceSession::NewLC(*this);
        retval = pSession->InitializeFromL(*aServiceDescription);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(pSession); 
            return retval;
            }
#ifdef _SENDEBUG
        TPtrC8 sessionEndpoint = pSession->Endpoint();
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Creating ASClient and new Session using Endpoint:");
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(sessionEndpoint));
#endif

        pASClient = CIdWsfAuthenticationServiceClient::NewLC(*this);//, *Log());
        pASClient->SetServiceSessionL(pSession);    // pSession will be owned by ASClient
        CleanupStack::Pop(2); // pASClient, pSession
        CleanupStack::PushL(pASClient);

        retval = Manager().RemoveServiceDescriptionL(*pASClient);
        if(retval != KErrNone)
            {
            delete pASClient;
            }
        CleanupStack::Pop(); // pASClient
        }

    return retval;
    }

TInt CSenIdWsfPlugin::RegisterASClientL(
                                MSenServiceDescription *aServiceDescription)
    {
#ifdef _SENDEBUG
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::RegisterASClientL");
    TPtrC8 endpoint = aServiceDescription->Endpoint();
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(endpoint));
#endif // _SENDEBUG

    TInt retval = KErrNone;

    CIdWsfServiceSession *pSession = NULL;
    CIdWsfAuthenticationServiceClient *pASClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                MSenServiceDescription::EIdWsfAuthenticationServiceClient)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("This ServiceDescription is already an AS Client")));
        pASClient = (CIdWsfAuthenticationServiceClient*)aServiceDescription;

        retval = Manager().AddServiceDescriptionL(pASClient);
        }
    else
        {
        // Create new Authentication Service client and initialize
        // it from given description
        pSession = CIdWsfServiceSession::NewLC(*this);
        retval = pSession->InitializeFromL(*aServiceDescription);

        if(retval != KErrNone)
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::RegisterASClientL");
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L("- InitializeFromL(SD) failed: %d"), retval));
            CleanupStack::PopAndDestroy(); // pSession
            return retval;
            }
        else if(pSession->Endpoint() == KNullDesC8)
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- InitializeFromL => KErrSenNoEndpoint");
            CleanupStack::PopAndDestroy(); // pSession
            return KErrSenNoEndpoint;
            }

#ifdef _SENDEBUG
        TPtrC8 sessionEndpoint = pSession->Endpoint();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
            _L8("- Creating AS client and new session using endpoint: '%S'"),
            &sessionEndpoint));
#endif // _SENDEBUG

        pASClient = CIdWsfAuthenticationServiceClient::NewLC(*this);//, *Log());
        pASClient->SetServiceSessionL(pSession);    // pSession will be
                                                    // owned by ASClient
        CleanupStack::Pop(2); // pASClient, pSession
        CleanupStack::PushL(pASClient);

        retval = Manager().AddServiceDescriptionL(pASClient);
        if(retval != KErrNone)
            {
            delete pASClient;
            }
        CleanupStack::Pop(); // pASClient
        }

    return retval;
    }

TInt CSenIdWsfPlugin::RegisterDSClientL(
                                MSenServiceDescription* aServiceDescription)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("RegisterDSClientL(..)")));

    TInt retval = KErrNone;

    CIdWsfServiceSession *pSession = NULL;
    CIdWsfDiscoveryServiceClient *pDSClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                    MSenServiceDescription::EIdWsfDiscoveryServiceClient)
        {
        /* There already is a session, but it may be necessary
         * to reset the user information.
         */
        pDSClient = (CIdWsfDiscoveryServiceClient*)aServiceDescription;
        retval = Manager().AddServiceDescriptionL(pDSClient);
        }
    else
        {
        pSession = CIdWsfServiceSession::NewLC(*this);
        retval = pSession->InitializeFromL(*aServiceDescription);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(); // pSession
            return retval;
            }
#ifdef _SENDEBUG
        TPtrC8 endpoint = pSession->Endpoint();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("Creating DSClient for %S"), &endpoint));
#endif // _SENDEBUG

        pDSClient = CIdWsfDiscoveryServiceClient::NewLC(*this);//, *Log());
        pDSClient->SetServiceSessionL(pSession);
        CleanupStack::Pop(2); // pDSClient, pSession
        CleanupStack::PushL(pDSClient);

        retval = Manager().AddServiceDescriptionL(pDSClient);
        if(retval != KErrNone)
            {
            delete pDSClient;
            }
        CleanupStack::Pop(); // pDSClient
        }
    return retval;
    }

TInt CSenIdWsfPlugin::UnRegisterDSClientL(
                                MSenServiceDescription* aServiceDescription)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("RegisterDSClientL(..)")));

    TInt retval = KErrNone;

    CIdWsfServiceSession *pSession = NULL;
    CIdWsfDiscoveryServiceClient *pDSClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
            MSenServiceDescription::EIdWsfDiscoveryServiceClient)
        {
        // There already is a session, but it may be necessary
        // to reset the user information.
        //
        pDSClient = (CIdWsfDiscoveryServiceClient*)aServiceDescription;
        retval = Manager().RemoveServiceDescriptionL(*pDSClient);
        }
    else
        {
        pSession = CIdWsfServiceSession::NewLC(*this);
        retval = pSession->InitializeFromL(*aServiceDescription);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(); // pSession
            return retval;
            }
#ifdef _SENDEBUG
        TPtrC8 endpoint = pSession->Endpoint();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("Creating DSClient for %S"), &endpoint));
#endif

        pDSClient = CIdWsfAuthenticationServiceClient::NewLC(*this);//, *Log());
        pDSClient->SetServiceSessionL(pSession);
        CleanupStack::Pop(2); // pDSClient, pSession
        CleanupStack::PushL(pDSClient);

        retval = Manager().RemoveServiceDescriptionL(*pDSClient);
        if(retval != KErrNone)
            {
            delete pDSClient;
            }
        CleanupStack::Pop(); // pDSClient
        }
    return retval;
    }

/**
* Create a ServiceSession based upon the given ServiceDescription.
* The ServiceSession is expected to be "valid", i.e. ready for use
* by a client.
*
* @param aServiceDescription A ServiceDescription
*
* @param aNewSession (CSenServiceSession)
*
* return TInt error or KErrNone if successful
*/
TInt CSenIdWsfPlugin::CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                             MSenServiceDescription& aPattern,
                                             CSenServiceSession*& aNewSession,
                                             MSenRemoteServiceConsumer& aRemoteConsumer,
                                             HBufC8*& /* aErrorMsg */ ) // TODO: start providing aErrorMsg on SOAP faults(!)
    {
#ifdef _SENDEBUG
    CSLOG(aRemoteConsumer.ConnectionId()  , KMinLogLevel,(_L("CreateServiceSessionL()")));
    TPtrC8 contract = aServiceDescription.Contract();
    CSLOG_FORMAT((aRemoteConsumer.ConnectionId()  , KMinLogLevel, _L8("- Asked to create ServiceSession using '%S' as Contract...)"),
            &contract));
#endif

    delete aNewSession;
    aNewSession = NULL;

    if(aServiceDescription.DescriptionClassType() == MSenServiceDescription::EIdWSFServiceSession)
        {
        aNewSession = (CIdWsfServiceSession*) &aServiceDescription;
        CSLOG_L(aRemoteConsumer.ConnectionId()  , KMinLogLevel,"- service description is already an ID-WSF service session.");
        // Check for messageThreading! (and other facets that may have to be
        // set or cleared)
        // We can assume that the current session matches with the pattern
        // but that there may be a need to adjust some settings.
        ((CIdWsfServiceSession*)aNewSession)->CopyFacetsFromL(aPattern);
        }
    else
        {
        aNewSession = (CIdWsfServiceSession*) NewServiceDescriptionL();
        ((CIdWsfServiceSession*)aNewSession)->InitializeFromL( aServiceDescription );
        }

    if(aNewSession)
        {
        if(!aNewSession->IsReadyL())
            {
            if(ValidateL(*((CIdWsfServiceSession*)aNewSession), aRemoteConsumer))
                {
                // note (below): sessions were just previously written(!)
                // -> due to nature of current CSenXmlDao implementation:
                // it writes everything and every time it is saving some
                // new object.
                Manager().SaveL(*this);
                return KErrNone;
                }
            else
                {
                return KErrNotReady;
                }
            }
        }
    return KErrNone;
    }


TBool CSenIdWsfPlugin::ValidateL(CIdWsfServiceSession& aSession,
                                MSenRemoteServiceConsumer& aRemoteConsumer)
    {
#ifdef _SENDEBUG
    TPtrC8 endpoint = aSession.Endpoint();
    CSLOG_FORMAT((aRemoteConsumer.ConnectionId()  , KMinLogLevel, _L8("Session to '%S' Endpoint needs to be revalidated!"),
                            &endpoint));
#endif // _SENDEBUG

    CIdWsfDiscoveryServiceClient* pDSClient = NULL;
    TPtrC8 trustAnchor = aSession.TrustAnchor();
    pDSClient = DSClientL(trustAnchor, aRemoteConsumer);

    if(!pDSClient)
        {
        // this typically occurs, if no IDP was obtained
        // the file might have been empty, or no matching
        // IDP was found(!)
        return EFalse;
        }

    TInt retVal = pDSClient->ValidateL(aSession, aRemoteConsumer);

    TBool success = (retVal == KErrNone) && aSession.IsReadyL();
    if(success)
        {
        // note (above): this leads to instant, second-time-write of sessions
        // -> due to nature of current CSenXmlDao implementation:
        // it writes everything and every time it is saving some new object.
        Manager().SaveL(aSession);
        }

    return success;
    }


CIdWsfDiscoveryServiceClient* CSenIdWsfPlugin::DSClientL(
            const TDesC8& aTrustAnchor,
            MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L("DSClientL(const TDesC& aTrustAnchor, MSenRemoteServiceConsumer&)")));
    CSenIdentityProvider* pIdentityProvider = IdentityProviderL(aTrustAnchor);
    if(pIdentityProvider)
        {
        return DSClientL(pIdentityProvider, aRemoteConsumer);
        }
    else
        {
        return NULL;
        }
    }


CSenIdentityProvider* CSenIdWsfPlugin::IdentityProviderL(
                                                    const TDesC8& aProviderId)
    {
    CIdWsfServiceInstance *pPattern = CIdWsfServiceInstance::NewLC();
    pPattern->SetProviderIdL( aProviderId );

    // 2004-08-10. one-line fix // this is checked by ID-MGR for matches
    if(aProviderId.Length()>0)
        {
        pPattern->SetEndPointL( aProviderId );
        }

    CSenIdentityProvider* pIdentityProvider = IdentityProviderL( pPattern );
    CleanupStack::PopAndDestroy( pPattern ); 

    return pIdentityProvider;

    }

CSenIdentityProvider* CSenIdWsfPlugin::IdentityProviderL(
                                            MSenServiceDescription* aPattern)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::IdentityProviderL");

    CSenIdentityProvider* pIdentityProvider = NULL;

    if(aPattern)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Querying IdentityProvider using a pattern.");
        pIdentityProvider = Manager().IdentityProviderL(*aPattern);
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"No pattern given for IdentityProvider query.");
        }

    if(!pIdentityProvider)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Querying IdentityProvider without a pattern.");
        pIdentityProvider = Manager().IdentityProviderL();
        }

#ifdef _SENDEBUG
    if(!pIdentityProvider)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"FATAL: Did not obtain an IdentityProvider!");
        }
#endif

    return pIdentityProvider;
    }

TBool CSenIdWsfPlugin::ValidateDSL(CIdWsfDiscoveryServiceClient* aDSClient,
                                   CSenIdentityProvider* aIDP,
                                   MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CIdWsfAuthenticationServiceClient *pASClient = ASClientL(aIDP);
    if(!pASClient)
        {
        return EFalse;
        }

    CIdWsfServiceSession* pSession = aDSClient->IdWsfServiceSession();
    if(pSession)
        {
        pASClient->ValidateL(*pSession, aRemoteConsumer);
        }
    return aDSClient->IsReadyL();
    }

/**
* Obtain or construct a valid DiscoveryServiceClient. Ask
* the owning ServiceManager for ServiceDescriptions that match
* the ID-WSF Discovery Service contract, and that have the
* given trust anchor (aIdentityProvider).
* if given.
*
* @param aIdentityProvider
* @return DiscoveryServiceClient
*/
CIdWsfDiscoveryServiceClient* CSenIdWsfPlugin::DSClientL(
            CSenIdentityProvider* aIdentityProvider,
            MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L("DSClientL(CSenIdentityProvider*, MSenRemoteServiceConsumer&)")));

    TPtrC8 trustAnchor(KNullDesC8);
    if(aIdentityProvider)
        {
        trustAnchor.Set(aIdentityProvider->ProviderID());
        }

    CIdWsfDiscoveryServiceClient* pDSClient = NULL;

    RPointerArray<CSenWSDescription> matches;
    CleanupClosePushL(matches);

    CoreServiceDescriptionsL(matches, KIdWsfFrameworkDiscoveryServiceContract,
                                KNullDesC8);
    CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L(">>> TrustAnchor to match:")));

    CSLOG(aRemoteConsumer.ConnectionId()   , KMaxLogLevel,(trustAnchor));

    // we now should have DiscoveryServiceClients or nothing
    for(TInt i = 0; i < matches.Count(); i++)
        {
        CIdWsfDiscoveryServiceClient* pCandidate =
            reinterpret_cast<CIdWsfDiscoveryServiceClient*>(matches[i]);

        if(pCandidate && trustAnchor.Length()>0)
            {
            TPtrC8 candidatesTrustAnchor = pCandidate->TrustAnchor();
            CSLOG(aRemoteConsumer.ConnectionId()   , KMaxLogLevel,(_L(">>> DS Candidate TrustAnchor:")));
            CSLOG(aRemoteConsumer.ConnectionId()   , KMaxLogLevel,(candidatesTrustAnchor));

            if(candidatesTrustAnchor.Length() == 0 ||
                trustAnchor != candidatesTrustAnchor)
                {
                CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L(">>> DS Candidate not trusted")));
                pCandidate = NULL;
                }
            }

        if(pCandidate)
            {
            CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L(">>> DS Candidate trusted")));
            if(!pDSClient)
                {
                pDSClient = pCandidate;
                }
            else if(pCandidate->ValidUntilL() > pDSClient->ValidUntilL())
                {
                CSLOG(aRemoteConsumer.ConnectionId()   , KMinLogLevel,(_L("Found: Discovery Service Session having longer validity!")));
                pDSClient = pCandidate;
                }
            }
        }

    if(!pDSClient)
        {
        /* Obtain an AuthenticationServiceClient and ask it to validate
        * a new DiscoveryServiceClient.
        */
        pDSClient = CIdWsfDiscoveryServiceClient::NewLC(*this);//, *Log());
        CIdWsfServiceSession* pBrandNewSession =
                                CIdWsfServiceSession::NewL(*this);

        // The SetServiceSessionL() will also set the DS client
        // as the first consumer of this new service session.
        // Ownership of pBrandNewSession is transferred to pDSClient:
        pDSClient->SetServiceSessionL(pBrandNewSession);

        // NOTE
        //
        // This policy related feature is in still in progress, and
        // therefore: NO ready implementation has been provided.
        //
        // add the _REAL_ consumer to assure policy transaction(!)
        //pBrandNewSession->AddConsumerL(aRemoteConsumer);
        //
        // NOTE

        CIdWsfAuthenticationServiceClient* pASClient =
                                                ASClientL(aIdentityProvider);
        if(pASClient)
            {
            CIdWsfServiceSession* pSession = pDSClient->IdWsfServiceSession();
            if(pSession)
                {
                pASClient->ValidateL(*pSession, aRemoteConsumer);
                }
            }

        if(pDSClient->IsReadyL())
            {
            TInt retVal = Manager().AddServiceDescriptionL(pDSClient);
            if(retVal != KErrNone)
                {
                CleanupStack::PopAndDestroy(); // pDSClient
                pDSClient = NULL;
                }
            else 
                {
                CleanupStack::Pop(); // pDSClient
                }
            }
        else
            {
            // bugfix 2005-03-02, matmatt
            CleanupStack::PopAndDestroy(); // pDSClient 
            pDSClient = NULL; // not valid
            }
        }
    else
        {
        if(!pDSClient->IsReadyL())
            {
            CIdWsfAuthenticationServiceClient* pASClient =
                                            ASClientL(aIdentityProvider);
            if(pASClient)
                {
                CIdWsfServiceSession* pSession = pDSClient->IdWsfServiceSession();
                if(pSession)
                    {
                    pASClient->ValidateL(*pSession, aRemoteConsumer);
                    }
                }

            if(pDSClient->IsReadyL())
                {
                Manager().SaveL(*this);
                }
            else
                {
                pDSClient = NULL; // not valid, fixed 2005-12-07
                }
            }
        }
    CleanupStack::PopAndDestroy(); //matches.Close();

    return pDSClient;
    }

/**
* Obtain or construct a valid AuthenticationServiceClient. Ask
* the owning ServiceManager for ServiceDescriptions that match
* the ID-WSF Authentication Service contract, and the ProviderID
* of the ServiceDescription, if given.
*
* @param providerID
* @return AuthenticationServiceClient
*/
CIdWsfAuthenticationServiceClient* CSenIdWsfPlugin::ASClientL(
                                    CSenIdentityProvider* aIdentityProvider)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("ASClientL(..)")));

    TPtrC8 providerId(KNullDesC8);
    if(aIdentityProvider)
        {
        providerId.Set(aIdentityProvider->ProviderID());
        }

    if(providerId.Length()>0)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("Searching for AS at %S"), &providerId));
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,(_L8("Searching for AS at NULL")));
        }

    CIdWsfAuthenticationServiceClient* pASClient = NULL;

    RPointerArray<CSenWSDescription> matches;
    CleanupClosePushL(matches);

    CoreServiceDescriptionsL(matches,
                             KIdWsfFrameworkAuthenticationServiceContract(),
                             providerId);

    if(matches.Count() > 0)
        {
        pASClient =
            reinterpret_cast<CIdWsfAuthenticationServiceClient*>(matches[0]);
        pASClient->SetAccount(aIdentityProvider);
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("No matching AS description available!")));
        }

    if(matches.Count() > 1)
        {
        // search for a instance with longest validity
        for(TInt i = 0; i < matches.Count(); i++)
            {
            CIdWsfAuthenticationServiceClient* pNewerASClient =
              reinterpret_cast<CIdWsfAuthenticationServiceClient*>(matches[i]);
            pNewerASClient->SetAccount(aIdentityProvider);
            if(pNewerASClient->ValidUntilL() > pASClient->ValidUntilL())
                {
                pASClient = pNewerASClient;
                }
            }
        }

    CleanupStack::PopAndDestroy(); // matches.Close();
    return pASClient;
    }

TInt CSenIdWsfPlugin::CoreServiceDescriptionsL(
                RPointerArray<CSenWSDescription>& aMatches,
                const TDesC8& aContract,
                const TDesC8& aProviderID)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("CoreServiceDescriptionsL(..)")));

    RPointerArray<CSenWSDescription> serviceDescriptions;
    CleanupClosePushL(serviceDescriptions);

    TInt retVal =
        Manager().ServiceDescriptionsL(serviceDescriptions, aContract);
    if(retVal != KErrNone)
        {
        return retVal;
        }

    CIdWsfCoreServiceConsumer* pDescription = NULL;
    for(TInt i = 0; i < serviceDescriptions.Count(); i++)
        {
        pDescription = (CIdWsfCoreServiceConsumer*)serviceDescriptions[i];
        if(aProviderID.Length()>0)
            {
#ifdef _SENDEBUG
            HBufC8* pAsXml = pDescription->AsXmlL();
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::CoreServiceDescriptionsL:");
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ," - CoreServiceConsumer as XML:");
            if(pAsXml)
                {
                CleanupStack::PushL(pAsXml);
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(*pAsXml));
                CleanupStack::PopAndDestroy(); // asXml
                }
#endif

            TPtrC8 providerID = pDescription->ProviderId();
            if(providerID.Length()>0 && (aProviderID == providerID))
                {
#ifdef _SENDEBUG
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(_L8("    > added to matches")));
#endif
                aMatches.Append(pDescription);
                }
            }
        else
            {
            aMatches.Append(pDescription);
            }
        }
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("Found %d matches for"), aMatches.Count()));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("     a contract: %S"), &aContract));

    CleanupStack::PopAndDestroy(); // serviceDescriptions.Close();
    return KErrNone;
    }

/**
* Return a new instance of a framework specific class that
* implements the ServiceDesccription interface.
*
* @param aNewSession (CSenServiceSession)
*
* return TInt error or KErrNone if successful
*/
CSenWSDescription* CSenIdWsfPlugin::NewServiceDescriptionL()

    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("CSenIdWsfPlugin::NewServiceDescriptionL()")));
    CIdWsfServiceSession* pSD = CIdWsfServiceSession::NewL(*this);
    return pSD;
    }

/**
* Return the ServiceManager for which this instance is working.
* This is used by the ServiceSession instances created by this framework.
*
* Note, in Sen, MSenCoreServiceManager so all Frameworks
* return their "owner" - i.e. - the only service manager in Sen Framework
*
* @return (MSenCoreServiceManager)
*/
MSenCoreServiceManager& CSenIdWsfPlugin::Manager()
    {
    return iManager;
    }

/**
* Set the ServiceManager to which this instance reports.
* The (singleton) ServiceManager calls this method when
* it instantiates a framework.
*
* @param aServiceManager The singleton ServiceManager in the system.
*/
void CSenIdWsfPlugin::SetManager(MSenCoreServiceManager& aServiceManager)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("CSenIdWsfPlugin::SetManager(.)")));
    iManager = aServiceManager;
    }


/**
* Return an object that can do SAX based parsing of a XML fragment
* that contains framework specific configuration information.
* The ServiceManager asks for this BaseFragment upon startup
* in order to give this framework a chance to read its configuration
* information from the central ServiceManager configuration document.
*
* @return (CSenBaseFragment)
*/
CSenBaseFragment& CSenIdWsfPlugin::ConfigParser()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L8("CSenIdWsfPlugin::ConfigParser()")));
    return *this;
    }

/**
* Write the ServiceInvocationFramework configuration as XML to a HBufC8.
*
* This method is invoked by the ServiceManager when it saves
* its state and/or configuration.
*
* Upon startup the chunk of XML that is written will be passed back to
* the configuation parser of the framework.
*
* The "schema" that shall be adhered to is:
* <pre>
* &lt;element name="Framework"&gt;
*       &lt;any/&gt;&lt;!-- this is where implementations write additional
*       information --&gt;
*       &lt;attribute name="class" type="String"&gt;
*    &lt;!-- In Symbian the class attribute is used as cue for
*    the ECOM Resolver --&gt;
* &lt;/element&gt;
* </pre>
*
* @param aTo - a HBufC8 to which the framework should write its configuration
*/


void CSenIdWsfPlugin::EndElementL(const TDesC8& aNsUri,
                                  const TDesC8& aLocalName,
                                  const TDesC8& aQName)
    {
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
        _L8("ID-WSF plugin @ end of element: %S (xmlns='%S', localname='%S')"),
                &aQName, &aNsUri, &aLocalName));
    CSIF::EndElementL(aNsUri,aLocalName,aQName);
    }

// over-ride CSenBaseFragment
HBufC* CSenIdWsfPlugin::AsXmlUnicodeL()
    {
    HBufC8* pUtf8 = AsXmlL();
    CleanupStack::PushL(pUtf8);
    HBufC* pAsAxlInUnicode = SenXmlUtils::ToUnicodeLC(*pUtf8);
    CleanupStack::Pop();           // pAsAxlInUnicode
    CleanupStack::PopAndDestroy(); // pUtf8
    return pAsAxlInUnicode;
    }

HBufC8* CSenIdWsfPlugin::AsXmlL()
    {
    _LIT8(KFramework,"<Framework ");
    _LIT8(KXmlns,"xmlns=\"");
    _LIT8(KQuot,"\" ");
    _LIT8(KClass,"class=\"");
    _LIT8(KClosedTag,"\"/>\n");
    HBufC8* retVal = HBufC8::NewLC(256);
    TPtr8 ptr = retVal->Des();
    ptr.Append(KFramework); // Codescanner warnings
    ptr.Append(KXmlns);
    ptr.Append(KIdWsfFrameworkXMLNS);
    ptr.Append(KQuot);

    ptr.Append(KClass);
    ptr.Append(KIdWsFrameworkCue);
    ptr.Append(KClosedTag);
    CleanupStack::Pop(); // retVal
    return retVal;
    }

TInt CSenIdWsfPlugin::OnEvent(const TInt aEvent,
                            TAny* /* aArgument */)
    {
    TInt retVal(KErrNone);
    TInt leaveCode(KErrNone);
    switch(aEvent)
        {
        case KSenEventWsfReady:
            // Core constructed                             
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"KSenEventWsfReady");
            {
            TRAP(leaveCode, retVal = AssignSessionValidatorsL());
            }
        break;
        case KSenEventIdWsfSessionValidatorRequested:
            {
            // Someone needs validator
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"KSenEventIdWsfSessionValidatorRequested");
            TRAP(leaveCode, retVal = AssignSessionValidatorsL());
            }
        break;
#ifdef _SENDEBUG
        default:
            {
            //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"==> no action taken.");
            }
        break;
#endif // _SENDEBUG
        }
    // if leave occurred, return leave code instead
    if(leaveCode)
        {
        retVal = leaveCode;
        }

    return retVal;
    }

TAny* CSenIdWsfPlugin::ExtendedInterface()
    {
    // there is no extended interface in ID-WSF framework at the moment
    return NULL;
    }

TInt CSenIdWsfPlugin::AssignSessionValidatorsL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenIdWsfPlugin::AssignSessionValidatorsL()");

    CSenWSDescription* pOpenMask = CSenWSDescription::NewLC(
            KNullDesC8, KNullDesC8); // this matches with *any* ServDescription
    pOpenMask->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    
    RWSDescriptionArray all;
    iManager.ServiceDescriptionsL(all, *pOpenMask);
    CleanupClosePushL(all);

    RWSDescriptionArray asClients;
    iManager.ServiceDescriptionsL(asClients, 
                                KIdWsfFrameworkAuthenticationServiceContract);
    CleanupClosePushL(asClients);

    RWSDescriptionArray dsClients;
    iManager.ServiceDescriptionsL(dsClients,
                                KIdWsfFrameworkDiscoveryServiceContract);
    CleanupClosePushL(dsClients);

    TInt count(all.Count());
    TInt asCount(asClients.Count());
    TInt dsCount(dsClients.Count());

    for(TInt i=0; i<count; i++)
        {   
        CSenWSDescription* pSD = (CSenWSDescription*)all[i];

        if(pSD->DescriptionClassType() ==
            MSenServiceDescription::EIdWSFServiceSession)
            {
            CIdWsfServiceSession* pSession = (CIdWsfServiceSession*)pSD;
            if(!pSession->Validator() && pSession->TrustAnchor().Length()>0)
                {
                // session does not have validator, attempt to find one
                for(TInt d=0; d<dsCount; d++)
                    {
                    pSD = (CSenWSDescription*)dsClients[d];
                    if(pSD->DescriptionClassType() ==
                        MSenServiceDescription::EIdWsfDiscoveryServiceClient)
                        {
                        CIdWsfDiscoveryServiceClient* pDsClient =
                            (CIdWsfDiscoveryServiceClient*)dsClients[d];
                        TPtrC8 trustAnchor = pSession->TrustAnchor();
                        if(trustAnchor == pDsClient->TrustAnchor())
                            {
                            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("Validator (DSC): 0x%x"), pDsClient));
                            pSession->SetValidator(pDsClient);
                            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("Validator (DSC): 0x%x"), pDsClient));
                            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("Validator for ID-WSF session found, trustanchor: %S"), &trustAnchor));
                            d = dsCount; // break this loop
                            }
                        }

                    }
                }
            }

        if(pSD->DescriptionClassType() ==
            MSenServiceDescription::EIdWsfDiscoveryServiceClient)
            {
            CIdWsfDiscoveryServiceClient* pDsClient = 
                        (CIdWsfDiscoveryServiceClient*)pSD;
            if(!pDsClient->Validator() && 
                pDsClient->TrustAnchor().Length()>0)
                {
                // session does not have validator, attempt to find one
                for(TInt a=0; a<asCount; a++)
                    {
                    pSD = (CSenWSDescription*)asClients[a];
                    if(pSD->DescriptionClassType() ==
                        MSenServiceDescription::EIdWsfAuthenticationServiceClient)
                        {
                        CIdWsfAuthenticationServiceClient* pAsClient =
                            (CIdWsfAuthenticationServiceClient*)asClients[a];

                        TPtrC8 trustAnchor = pDsClient->TrustAnchor();

                        if(trustAnchor == pAsClient->ProviderId())
                            {
                            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("Validator (ASC): 0x%x"), pAsClient));
                            CSenIdentityProvider* pAccount =
                                IdentityProviderL(pAsClient->ProviderId());
                            if(pAccount)
                                {
                                pAsClient->SetAccount(pAccount);
                                //pDsClient->SetValidator(pAsClient);

                                pDsClient->IdWsfServiceSession()->SetValidator(pAsClient);
                                //pAsClient->ValidateL(*pDsClient->IdWsfServiceSession()); // TEST ONLY!!!!!
                                a = asCount; // break the loop, validator was found
                                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("Validator for DS client found, trustanchor: %S"), &trustAnchor));
                                }
#ifdef _SENDEBUG
                            else
                                {
                                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Account (IDP) for ASC not found! No DS validator available");
                                }
#endif
                            }
                        }

                    }
                }
            }
        }
    
    CleanupStack::PopAndDestroy(4); // close dsClients, close asClients
                                    // close all, delete pOpenMask
    return KErrNone;
    }

TInt CSenIdWsfPlugin::SetTransportPropertiesL(MSenTransport& aTransport)
    {
    //CSenHttpTransportProperties* pProps = CSenHttpTransportProperties::NewLC();
    CSenTransportProperties* pProps = CSenTransportProperties::NewLC();
    pProps->SetPropertyL(KSenSoapActionHeaderName, KSenSoapActionHeaderValueEmpty, KHttpHeaderType);        
    HBufC8* pPropsAsXml = pProps->AsUtf8L();
    CleanupStack::PopAndDestroy(pProps);

    CleanupStack::PushL( pPropsAsXml );
    // In ID-WSF, even message layer will be forced to have an empty SOAPAction value(!), only
    // the message's transport properties sent by consumer applicatoin may override this 
    // (although they never should do that, it has been made possible for the application messages).
    TInt retVal = aTransport.SetPropertiesL( *pPropsAsXml, MSenLayeredProperties::ESenMessageLayer, NULL );
    TInt retVal2 = aTransport.SetPropertiesL( *pPropsAsXml, MSenLayeredProperties::ESenFrameworkLayer, NULL );
    CleanupStack::PopAndDestroy( pPropsAsXml );
    return retVal<retVal2?retVal:retVal2;
    }

    
// End of File
    
