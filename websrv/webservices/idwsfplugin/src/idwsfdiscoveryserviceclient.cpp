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









#include <s32mem.h>

#include <SenFacet.h>
#include <SenServiceConnection.h>
#include <SenXmlReader.h>
#include <SenXmlUtils.h>

#include "msencoreservicemanager.h"
#include "sendebug.h"

#include "idwsfmessage.h"  // for response parsing and request building
#include "idwsfdsqueryresponse.h"
#include "idwsfdiscoveryserviceclient.h"
#include "idwsfservicesession.h"
#include "idwsfdsqueryresponse.h"

#include "senlogger.h"
// for the parsing and the construction of outgoing DSLookups

namespace
    {
    // 2004-07-21:
    _LIT8(KDiscoReqStart1,"<Query xmlns=\"urn:liberty:disco:2003-08\" >");

    // the resouse id element is not needed if no content is available
    _LIT8(KDiscoReqResIdStart,"<ResourceID>");
    _LIT8(KDiscoReqResIdEnd,"</ResourceID>");

    _LIT8(KEncrStart,"<Encrypted");
    _LIT8(KDiscoReqEncrResIdStart,"<EncryptedResourceID>");
    _LIT8(KDiscoReqEncrResIdEnd,"</EncryptedResourceID>");

    _LIT8(KDiscoReqStart2,"<RequestedServiceType><ServiceType>");
    _LIT8(KDiscoReqEnd2,"</ServiceType>");
    _LIT8(KDiscoOptionsStart,"<Options>");
    _LIT8(KDiscoOptionsEnd,"</Options>");
    _LIT8(KSenOption,"Option");
    _LIT8(KDiscoReqEnd, " </RequestedServiceType> </Query>");

    const TInt KFLATBUF_SIZE = 128;
    }


CIdWsfDiscoveryServiceClient* CIdWsfDiscoveryServiceClient::NewL(
                                                         CSIF& aSIF)//,
                                                         //RFileLogger& aLogger)
    {
    CIdWsfDiscoveryServiceClient* pNew = NewLC(aSIF);//, aLogger);
    CleanupStack::Pop();
    return pNew;
    }

CIdWsfDiscoveryServiceClient* CIdWsfDiscoveryServiceClient::NewLC(
                                                        CSIF& aSIF)//,
                                                        //RFileLogger& aLogger)
    {
    CIdWsfDiscoveryServiceClient* pNew =
        new (ELeave) CIdWsfDiscoveryServiceClient(
                        aSIF,
                        MSenServiceDescription::EIdWsfDiscoveryServiceClient);//,
                        //aLogger);

    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

CIdWsfDiscoveryServiceClient::CIdWsfDiscoveryServiceClient(CSIF& aSIF,
                                                           TDescriptionClassType aType)//,
                                                           //RFileLogger& aLogger) 
    : CIdWsfCoreServiceConsumer(aSIF, aType),
    iCState(ENotInitialized),
    iSession(NULL)//,
    //iLog(aLogger)
    {
    }

// returns KErrNotFound, if no matching security mechanism was found
// in ProcessResponce() call
// returns KErrNotReady, if no service session has been initilized
TInt CIdWsfDiscoveryServiceClient::FindServiceL(
                    RPointerArray<CSenWSDescription>& aServiceArray,
                    MSenServiceDescription& aPattern,
                    MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfDiscoveryServiceClient::FindServiceL()");

    // keep this line first
    iServedConsumer = &aRemoteConsumer;

    iResults.ResetAndDestroy(); // clear array and destroy its contents
    iCState = EHasResults;

    HBufC8* pLookupReq = NULL;
    TPtrC8 contract = aPattern.Contract();

    TInt retVal(KErrNone);
    if(contract.Length()>0)
        {
        // DsLookupRequestL never returns NULL
        pLookupReq = DsLookupRequestL(aPattern);

        CleanupStack::PushL(pLookupReq);

        if(iService)
            {
            HBufC8* pResponse = NULL;
    #ifdef _SENDEBUG
            TPtr8 lookupRequest = pLookupReq->Des();
//wslog            FILELOGALL(_L("SenCoreServiceManager"), _L("DS_lookup_req.xml"), lookupRequest);
    #endif // _SENDEBUG

            retVal = iService->SubmitL(*pLookupReq, KNullDesC8, *this, pResponse); 
            aRemoteConsumer.SetDataTrafficDetails(iDiscDetails);           
        
            
       
            CleanupStack::PushL(pResponse);
            if(retVal == KErrNone && pResponse)
                {
    #ifdef _SENDEBUG
                TPtr8 lookupResponse= pResponse->Des();
//wslog                FILELOGALL(_L("SenCoreServiceManager"), _L("DS_lookup_rsp.xml"), lookupResponse);
    #endif // _SENDEBUG

                CIdWsfDsQueryResponse* pDsQueryResponse =
                    ParseResponseLC(*pResponse); // push #3

                retVal = ProcessResponseL(pDsQueryResponse);

                CleanupStack::PopAndDestroy(1); // pDsQueryResponse

                TInt count(iResults.Count());
                for(TInt i=0; i<count; i++)
                    {
    #ifdef _SENDEBUG
                    if(iResults[0])
                        {
                        HBufC* pAsXml = iResults[0]->AsXmlUnicodeL();
                        if(pAsXml)
                            {
                            CleanupStack::PushL(pAsXml);
                            CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("Result (%d):"), i));
                            CSLOG_ALL(aRemoteConsumer.ConnectionId(), KMaxLogLevel ,(*pAsXml));
                            CleanupStack::PopAndDestroy(); // pAsXml
                            }

                        }
    #endif
                    // we now could annotate any new service (session) with
                    // relevant facets from the input ServiceDescription,
                    // but not with disco_options!
                    ((CIdWsfServiceSession*)iResults[0])->CopyFacetsFromL(
                                                                    aPattern);
                    ((CIdWsfServiceSession*)iResults[0])->SetValidator(this);
                    TInt appendRetVal = aServiceArray.Append(
                                    (CSenWSDescription*)iResults[0]);
                    if(appendRetVal!=KErrNone)
                        {
                        // out of memory, free orphan
                        delete iResults[0];
                        }
                    iResults.Remove(0);
                    }
                }
            CleanupStack::PopAndDestroy(); // pResponse
            }
        else
            {
            retVal = KErrNotReady;
            }
        CleanupStack::PopAndDestroy(); // pLookupReq
        }
    else
        {
        retVal = KErrSenNoContract;
        }
    return retVal;
    }



void CIdWsfDiscoveryServiceClient::BaseConstructL()
    {
    CIdWsfCoreServiceConsumer::BaseConstructL();

    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("CIdWsfDiscoveryServiceClient::BaseConstructL()")));
    }

TInt CIdWsfDiscoveryServiceClient::ValidateL(
                                    CIdWsfServiceSession& aSession,
                                    MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfDiscoveryServiceClient::ValidateL");

    // keep this line first
    iServedConsumer = &aRemoteConsumer;
    iSession = &aSession; // iSession == Java's stateObject !!!

    iCState = EHasSession;
    CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"- creating discovery lookup request.");


    HBufC8* pLookupReqUtf8 = DsLookupRequestL(aSession);
    CleanupStack::PushL(pLookupReqUtf8);

    HBufC8* pSubmitResponse = NULL; // for ref-to-ptr

#ifdef _SENDEBUG
    TPtr8 lookupRequest= pLookupReqUtf8->Des();
//wslog    FILELOGALL(_L("SenCoreServiceManager"), _L("DS_lookup_req.xml"), lookupRequest);
#endif // _SENDEBUG

    TInt retVal = iService->SubmitL(*pLookupReqUtf8, KNullDesC8, *this, pSubmitResponse); 
    CleanupStack::PopAndDestroy(); // pLookupReqUtf8

    CleanupStack::PushL(pSubmitResponse);
	aRemoteConsumer.SetDataTrafficDetails(iDiscDetails);	

    if(retVal == KErrNone && pSubmitResponse)
        {
        // submit ok
#ifdef _SENDEBUG
        TPtr8 lookupResponse = pSubmitResponse->Des();
//wslog        FILELOGALL(_L("SenCoreServiceManager"), _L("DS_lookup_rsp.xml"), lookupResponse);
#endif // _SENDEBUG
        CIdWsfDsQueryResponse* theResponse = ParseResponseLC(*pSubmitResponse);
        retVal = ProcessResponseL(theResponse);
        CleanupStack::PopAndDestroy(); // theResponse
        }
#ifdef _SENDEBUG
    else
        {
        CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfDiscoveryServiceClient::ValidateL");
        CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("- SubmitL failed: %d"), retVal));
        }
#endif // _SENDEBUG
    CleanupStack::PopAndDestroy(); // pSubmitResponse
        
    if ((retVal == KErrNone) && iSession->IsReadyL())
        {
        CSLOG_L(aRemoteConsumer.ConnectionId(), KMinLogLevel ,"- setting this DS client as validator for ID-WFS session.");
        iSession->SetValidator(this);
        }

    return retVal;
    }

CIdWsfDsQueryResponse* CIdWsfDiscoveryServiceClient::ParseResponseLC(
                                                            TDesC8& aMessage)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("CIdWsfDiscoveryServiceClient::ParseResponseLC")));
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(aMessage));

    // must be first, if we promise LC in this method..
    CIdWsfDsQueryResponse* pResponse = CIdWsfDsQueryResponse::NewLC();


    pResponse->SetReader(*Framework().Manager().XMLReader());
    pResponse->ParseL(aMessage);
    return pResponse; // item resides in cleanupstack
    }

// returns KErrNotFound, if no matching security mechanism was found
TInt CIdWsfDiscoveryServiceClient::ProcessResponseL(
                                            CIdWsfDsQueryResponse* aResponse)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("CIdWsfDiscoveryServiceClient::ProcessResponseL")));

    TInt retVal(KErrNone);

    RPointerArray<CIdWsfServiceInstance> services; // 2004-08-09, bugfix

    CleanupClosePushL(services);
    aResponse->GetAllServicesL(services);

    CIdWsfServiceSession* pTempSession = NULL; // session in Java!

    switch(iCState)
        {
        case EHasSession:
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("   State: EHasSession")));
            pTempSession = iSession; // iSession == stateObject in Java!
            if (pTempSession)
                {
#ifdef _SENDEBUG
                if(pTempSession->IsReadyL())
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"   Current session is valid.");
                    }
                else
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"   Current session is expired.");
                    }
#endif

                if (iService)
                    {
                    pTempSession->SetClientServerIntervalL(
                                                iService->ClientServerInterval());// codescannerwarnings
                    }

                TInt count(services.Count());
                for(TInt i=0; !pTempSession->IsReadyL()
                            && i<count; i++) // not-operation bugfix, 2004-07-20
                    {
                    if(services[i])
                        {
                        retVal = pTempSession->InitializeFromL(*services[i]);
                        }
                    }

                if(pTempSession->IsReadyL() && iService)
                    {
                    pTempSession->SetTrustAnchorL(iService->TrustAnchor());
                    }
                }
            iCState = ENotInitialized;
            }
            break;

        case EHasResults:
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("   State: EHasResults")));
            TInt count(services.Count());
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L("Response included %d services."),count));
            for (TInt i=0; i<count; i++)
                {
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L("Trying to initialize service number %d "),i));
                pTempSession =
                    (CIdWsfServiceSession*)Framework().NewServiceDescriptionL();
                CleanupStack::PushL(pTempSession);

                if (iService)
                    {
                    pTempSession->SetClientServerIntervalL(
                                            iService->ClientServerInterval());	//codescannerwarnings
                    }

                for(TInt j=i; j<count && !pTempSession->IsReadyL(); j++)
                    {
                    retVal = pTempSession->InitializeFromL(*services[j]);
                    if(!pTempSession->IsReadyL())
                        {
                        i++;
                        }
                    }
                

                if(pTempSession->IsReadyL())
                    {
                    if(iService)
                        {
                        pTempSession->SetTrustAnchorL(iService->TrustAnchor());
                        }
                    TInt appendRetVal = iResults.Append(pTempSession); // only ready ones goto list
                    if(appendRetVal == KErrNone)
                        {
                        CleanupStack::Pop(); //pTempSession
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy(); // pTempSession
                        }
                    }
                else
                    {
                    // No ready Session found
                    CleanupStack::PopAndDestroy(); //pTempSession
                    }
                }
            }
            break;

        default:
            // do nothing
            break;
        }

    CleanupStack::PopAndDestroy(); // close services, // 2004-08-09, bugfix
    return retVal;
    }

void CIdWsfDiscoveryServiceClient::SetStatusL(const TInt /* aStatus */ )
    {
    }



CIdWsfDiscoveryServiceClient::~CIdWsfDiscoveryServiceClient()
    {
    iResults.ResetAndDestroy();
    }


// overrides the internalserviceconsumer.log() function
// to enable using the log delivered as constructor arg.
/*RFileLogger* CIdWsfDiscoveryServiceClient::Log() const
    {
    return (RFileLogger*) &iLog;
    }
*/


HBufC8* CIdWsfDiscoveryServiceClient::DsLookupRequestL(
                                            MSenServiceDescription& aPattern)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,(_L("CIdWsfDiscoveryServiceClient::DsLookupRequestL")));

    RFacetArray options;
    CleanupClosePushL(options);

    CIdWsfDiscoveryServiceClient::DSOptionsL(options, aPattern);

    CBufFlat* pFlat = CBufFlat::NewL(KFLATBUF_SIZE);
    CleanupStack::PushL(pFlat);

    RBufWriteStream ws;
    ws.Open(*pFlat);
    CleanupClosePushL(ws);

    // create the disco lookup query and fill it with the passed service type
    ws.WriteL(KDiscoReqStart1);
    TPtrC8 resourceId = iService->ResourceId();
    if(resourceId.Length()>0)
        {
        if(resourceId.Find(KEncrStart) == 0)
            {
            ws.WriteL(KDiscoReqEncrResIdStart);
            ws.WriteL(resourceId);
            ws.WriteL(KDiscoReqEncrResIdEnd);
            }
        else
            {
            ws.WriteL(KDiscoReqResIdStart);
            ws.WriteL(resourceId);
            ws.WriteL(KDiscoReqResIdEnd);
            }
        }
    ws.WriteL(KDiscoReqStart2);
    ws.WriteL(aPattern.Contract());
    ws.WriteL(KDiscoReqEnd2);

    if (options.Count())
        {
        ws.WriteL(KDiscoOptionsStart);

        CSenXmlElement* pOption = NULL; 
        HBufC8* pOptionAsXml = NULL;

        TInt count(options.Count());
        for (TInt i=0; i<count; i++)
            {
            pOption = CSenXmlElement::NewL(KSenOption); 
            CleanupStack::PushL(pOption);
            pOption->CopyFromL(*options[i]);
            pOptionAsXml = pOption->AsXmlL();
            CleanupStack::PushL(pOptionAsXml);
            ws.WriteL(*pOptionAsXml);
            CleanupStack::PopAndDestroy(2); // pOptionAsXml, pOption
            }
        ws.WriteL(KDiscoOptionsEnd);
        }

    ws.WriteL(KDiscoReqEnd);

    CleanupStack::PopAndDestroy(); // ws.Close();

    TPtr8 p8 = pFlat->Ptr(0);
    HBufC8* pReq8 = p8.AllocL();
    CleanupStack::PopAndDestroy( pFlat );
    
#ifdef _SENDEBUG   
    CleanupStack::PushL( pReq8 );
    if( pReq8 )
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(_L8("Returning Discovery Service Lookup Request:")));
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(*pReq8));
        
        }
    CleanupStack::Pop( pReq8 ); // pReq8 == return value
#endif // _SENDEBUG        
    options.ResetAndDestroy();
    CleanupStack::Pop(); // options
    return pReq8;
    }

TInt CIdWsfDiscoveryServiceClient::DSOptionsL(
                                RFacetArray& aOptionArray,
                                MSenServiceDescription& aServiceDescription)
    {
    TInt retVal = KErrNone;

    RFacetArray facets;
    CleanupClosePushL(facets);
    aServiceDescription.FacetsL(facets);

    CSenFacet* pFacet = NULL;
    TInt count(facets.Count());
    for (TInt i=0; i<count && retVal == KErrNone; i++)
        {
        if (facets[i]->Type() == KDiscoOption)
            {
            pFacet = CSenFacet::NewL(*facets[i]);
            retVal = aOptionArray.Append(pFacet);
            }
        }
    facets.ResetAndDestroy();
    CleanupStack::Pop(); //facets

    return retVal;
    }




TBool CIdWsfDiscoveryServiceClient::HasSuperClass( TDescriptionClassType aType )
    {
   if( aType == MSenServiceDescription::ECoreServiceConsumer ) // direct superclass!    
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CIdWsfCoreServiceConsumer::HasSuperClass( aType ); 
        }
    }

void CIdWsfDiscoveryServiceClient::SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) 
	{
	iDiscDetails = aDetails;
	}
// End of File

