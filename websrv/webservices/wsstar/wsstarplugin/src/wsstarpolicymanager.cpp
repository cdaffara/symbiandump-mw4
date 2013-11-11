/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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










#include <e32std.h>
#include <utf.h> // for reading policy from file (utf-8 conversion)

#include <SenServiceConnection.h> // public error codes

#include "senwspolicymanager.h"
#include "seninternalwspolicy.h"

#include "wsstarpolicymanager.h"

#include "senpointermap.h"
#include "wsstarhttpclient.h"
#include "wspolicy.h"
#include "sendebug.h"
#include "wsstardictionary.h"
#include "MSenServicePolicy.h"
#include "wsstarpolicyrequest.h"
#include "senlogger.h"

class CWSStarHandlerContext;
class CWSStarPolicyHandler;
using namespace MSWSPolicy;

namespace
    {
    _LIT8(KHttp, "http");
/*    
    TInt IsUriAlreadyExist(const TDesC8& aContract, const TDesC8 aUriName, RPolicyRequestMap& aRequestMap)
        {
        TInt uriCount(aRequestMap.Count());
        // check this if it is already in aRequestMap                
        for (TInt j = 0; j< uriCount; j++)
            {
            CPolicyReqMap* req = aRequestMap[j];
            TPtrC8 contract = req->Contract();
            TPtrC8 uri = req->Uri();
            if(contract.Compare(aContract) == 0 && uri.Compare(aUriName) == 0)
                {
                return KErrNone;
                }
            }
        return KErrNotFound;
        }
  */  
    }
    
//---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
//---------------------------------------------------------------------------
//
CWSStarPolicyManager* CWSStarPolicyManager::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarPolicyManager* self   = new (ELeave) CWSStarPolicyManager(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarPolicyManager::CWSStarPolicyManager(MSenHandlerContext& aCtx): iSenHandlerContext(aCtx)
    {
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarPolicyManager::~CWSStarPolicyManager()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("~CWSStarPolicyManager::~CWSStarPolicyManager()")));
    delete iRegistry;
    delete iPolicyManager;
//    iPolicyMap.ResetAndDestroy();
    iContracts.ResetAndDestroy();
    delete iDictionary;
    
    }

//---------------------------------------------------------------------------
// Second phase construction.
//---------------------------------------------------------------------------
//
void CWSStarPolicyManager::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyManager::ConstructL()")));
    iRegistry = CPolicyRegistry::NewL();
    iDictionary = CWSStarDictionary::NewL(*(CSIF*)iSenHandlerContext.GetAnyL(HandlerContextKey::KSIF()), *Log());//codescannerwarnings
    iPolicyManager = CSenWSPolicyManager::NewL(*iSenHandlerContext.GetSenCoreServiceManager());
    }
 
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//     
TInt CWSStarPolicyManager::InvokeL(const TDesC8& aContract)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPolicyManager::InvokeL(const TDesC8& aContract)");
    
    
    if( aContract.Length() < 1 )
        {
        return KErrArgument;
        }
    if(!IsContractExist(aContract))
        {
        HBufC8* cont = aContract.AllocL();
        iContracts.Append(cont);
        }

    return KErrNone;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
// 
TInt CWSStarPolicyManager::IsContractExist(const TDesC8& aContract)    
    {
    
    TInt count(iContracts.Count());
    for(TInt i = 0; i< count; i++)
        {
        
        TPtrC8 contract = iContracts[i]->Des();
        if(contract.Compare(aContract) == 0)
            {
            return ETrue;
            }
        }

    return EFalse;
    }
//---------------------------------------------------------------------------
// 
#if !defined( RD_SEN_WS_STAR_DISABLE_POLICY_HTTP_GET )
TInt CWSStarPolicyManager::DownloadPolicyL( CSenWSDescription* apSD ) 
#else // RD_SEN_WS_STAR_DISABLE_POLICY_HTTP_GET == TRUE
TInt CWSStarPolicyManager::DownloadPolicyL( CSenWSDescription* /* apSD */ ) 
#endif  // RD_SEN_WS_STAR_DISABLE_POLICY_HTTP_GET
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyManager::DownloadPolicyL()")));
    TInt contCount = iContracts.Count();
    for(TInt j = 0; j< contCount; j++)    
    {
        TPtrC8 ptr = iContracts[j]->Des();
        RegisterWsPolicyL(ptr);            
    }


    TInt error(KErrNone);
    TInt leaveCode(KErrNone);
    
    RExpiredPolicyArray list;
    CleanupClosePushL(list);    
    TInt retVal = iPolicyManager->ExpiredPoliciesL(list, error);
    if(retVal == KErrNone)
    {
        TPtrC8 finalUri(KHttp);
        if( finalUri != KNullDesC8 )
            {
            error = KErrNotFound; // testing
            TInt leaveCode(KErrNone);
    #if !defined( RD_SEN_WS_STAR_DISABLE_POLICY_HTTP_GET )
            TRAP( leaveCode, error = GetHttpPolicyL( finalUri, list, apSD); )
    #endif 

            
                        
            }
    }
    if( error )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPolicyManager::DownloadPolicyL:");
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- GetHttpPolicyL returned error: %d"), error ));
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," => attempting to read policy from file system..");
        TRAP( leaveCode, error = GetPolicyFromFileL( list ); )
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- GetPolicyFromFileL returned: %d"), error ));
        }

    CleanupStack::Pop(&list);  
    
    TInt count(list.Count());
    for(TInt i = 0; i< count; i++)
    {
        CExpiredPolicy* req = list[i];
        if(req->Contents() != KNullDesC8())
        {
            RPointerArray<CSenWSDescription> serviceDescriptions;
            CleanupClosePushL(serviceDescriptions);
            TInt retVal(KErrNone);
            TInt leaveCode(KErrNone);
            TRAP(leaveCode, retVal = iSenHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, req->Contract()));
            if(leaveCode == KErrNone && retVal == KErrNone && serviceDescriptions.Count() > 0)
                {
                for(TInt j = 0; j < serviceDescriptions.Count(); j++)
                    {
                    CSenWSDescription* sD = serviceDescriptions[j];

                    CSenWSDescription* tempSd = CSenWSDescription::NewLC();
                        CBufFlat *pBuf = CBufFlat::NewL(64);
                        CleanupStack::PushL(pBuf);
                        RBufWriteStream bufWs(*pBuf);
                        CleanupClosePushL(bufWs);
                        
                        if(sD->Contract() == KWSStarSTSContract)
                            {
                            CWSStarTrustClient* pTrustClient = (CWSStarTrustClient*)sD;   
                            pTrustClient->WriteAsXMLToL(bufWs);
                            tempSd->SetReader(*iSenHandlerContext.GetSenCoreServiceManager()->XMLReader());
                            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(pBuf->Ptr(0)));
                            tempSd->ParseL(pBuf->Ptr(0));
                            }
                        else if (sD->DescriptionClassType() == MSenServiceDescription::EWSStarServiceSession)
                            {
                            CWSStarServiceSession* pSd = (CWSStarServiceSession*)sD;
                            pSd->WriteAsXMLToL(bufWs);
                            tempSd->SetReader(*iSenHandlerContext.GetSenCoreServiceManager()->XMLReader());
                            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(pBuf->Ptr(0)));
                            tempSd->ParseL(pBuf->Ptr(0));
                            }

                    CleanupStack::PopAndDestroy(&bufWs);
                    CleanupStack::PopAndDestroy(pBuf);
                    
                    TInt errorCode(KErrGeneral);
                    AddWsPolicy1L(tempSd, req->Uri(), req->Contents(), errorCode );             
                    iDictionary->InvokeL(&iSenHandlerContext, tempSd);
			        TSenDataTrafficDetails details;
			        iDictionary->DataTrafficDetails(details);
				    iPolicyDetails += details;  
                    CleanupStack::PopAndDestroy(tempSd); 
                    }
                }
            CleanupStack::PopAndDestroy(&serviceDescriptions); 
        }
    }
    list.ResetAndDestroy();
/*
//This is ONLY working for MS cases. i.e. urn:wstar:sts contract.
//Right now its not parsing any other Sds to find dictionaries becuase 
//the search uses MS specifc namespace to dfind the dictioanry elements
#ifndef RD_SEN_WS_STAR_DISABLE_DICTIONARY_FETCHING
        iDictionary->InvokeL(&iSenHandlerContext); 
#endif // not defined RD_SEN_WS_STAR_DISABLE_DICTIONARY_FETCHING
//        }
    requestMap.ResetAndDestroy();
    iRegistry->ResetRegistry();
*/    
    return error;
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
TInt CWSStarPolicyManager::GetHttpPolicyL( const TDesC8& aEndpoint, RExpiredPolicyArray& aRequestMap, CSenWSDescription* /*apSD*/ )
    {
    TInt retVal(KErrNone);
    CSenWSDescription* pInitializer = CSenWSDescription::NewLC();
    pInitializer->SetEndPointL(aEndpoint);

    CWSStarHttpClient* pPolicyClient = CWSStarHttpClient::NewLC(*(CSIF*)iSenHandlerContext.GetAnyL(HandlerContextKey::KSIF()), pInitializer, *Log());//codescannerwarnings
        
    // Fix the first URI and request part of the request
    TInt count( aRequestMap.Count() );
    for (TInt i = 0; i< count; i++)
        {
        
        CExpiredPolicy* req = aRequestMap[i];
        TPtrC8 contract = req->Contract();
        TPtrC8 endpoint = req->Uri();
        
        TPtrC8 request(endpoint.Right(endpoint.Length() - aEndpoint.Length()));

        RPointerArray<CSenWSDescription> serviceDescriptions;
        CleanupClosePushL(serviceDescriptions);
        TInt retVal(KErrNone);
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, retVal = iSenHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, contract));
        if(leaveCode == KErrNone && retVal == KErrNone && serviceDescriptions.Count() > 0)
            {
            CSenWSDescription* sD = serviceDescriptions[0];

            HBufC8* pResponse = NULL;
            TInt leaveCode(KErrNone);
            TInt submitRetCode(KErrNone);

            TRAP( leaveCode, submitRetCode = pPolicyClient->GetHttpL( request, pResponse, sD ); )
            TSenDataTrafficDetails details;
            pPolicyClient->DataTrafficDetails(details);
            iPolicyDetails += details; 
                    
            if( submitRetCode && retVal == KErrNone )
                {
                // Store the error code from the 1st failing request 
                retVal = submitRetCode;
                }    
            if( leaveCode && retVal == KErrNone )
                {
                retVal = leaveCode;
                }    
            if(pResponse && pResponse->Length() > 0)
                {
                CleanupStack::PushL(pResponse);
                TPtrC8 pReq = pResponse->Des();                
                req->SetContentsL( pReq );          
                CleanupStack::PopAndDestroy(pResponse);            
                }    
            else // if(retVal == 404)    
                {
                delete pResponse;
                TPtrC8 pReq(KNullDesC8());
                req->SetContentsL( pReq );          
                }

            }
        CleanupStack::PopAndDestroy(&serviceDescriptions);        


        } // for each request
    CleanupStack::PopAndDestroy(pPolicyClient); 
    CleanupStack::PopAndDestroy(pInitializer);  
    return retVal;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
// 
// start of "debug / test method"    
TInt CWSStarPolicyManager::GetPolicyFromFileL( RExpiredPolicyArray& aRequestMap )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPolicyManager::GetPolicyFromFileL:");

    TInt retVal(KErrNone);
    
    TInt count(aRequestMap.Count());
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Count: %d"), count));

    _LIT(KPath, "c:\\");
    for (TInt i = 0; i< count; i++)
        {
        TFileName filepath(KPath);    

        CExpiredPolicy* req = aRequestMap[i];
        TPtrC8 contract = req->Contract();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Contract: '%S'"), &contract));

        TPtrC8 endpoint = req->Uri();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Endpoint: '%S'"), &endpoint));

        TInt namePosition = endpoint.LocateReverse('/');
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Nameposition: %d"), namePosition));


        if(namePosition > 0)
            {
//            _LIT(KXmlFileExt, ".xml");

            TPtrC8 name = endpoint.Right( endpoint.Length() - (namePosition+1) );
            
            if( name == KNullDesC8 ) 
                {
                // Note: if metadata endpoint ends with '/' -char, USE hardcoded DEFAULT VALUE
                _LIT8(KDefaultPolicyFilename, "secpol.xml");
                name.Set( KDefaultPolicyFilename );                
                }
            
            HBufC* nameBuf = HBufC::NewLC(name.Length());
            TPtr nameBufPtr( nameBuf->Des() );
            
            retVal = CnvUtfConverter::ConvertToUnicodeFromUtf8( nameBufPtr, name );  
            
            if(retVal == KErrNone)
                {
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Policy file name: '%S'"), nameBuf));
                TFileName filename; 
                filename.Append( nameBufPtr );
                filepath.Append( filename );
    
                RFs fss;
                User::LeaveIfError( fss.Connect() );
                CleanupClosePushL( fss );
                
                RFile  xmlFile;
                retVal = xmlFile.Open(fss , filepath, EFileRead );
                if(retVal == KErrNone)
                    {
                    TInt size;
                    xmlFile.Size(size);
                    if(size > 0)
                        {
                        HBufC8 *pBuf=HBufC8::NewLC(size); // CodeScannerWarning
                        TPtr8 buf8ptr(pBuf->Des());
                        xmlFile.Read(buf8ptr, size);
                        req->SetContentsL(buf8ptr);
                        CleanupStack::PopAndDestroy(pBuf);
                        }
                    }
                xmlFile.Close();                
                CleanupStack::PopAndDestroy( &fss );
                }
            CleanupStack::PopAndDestroy( nameBuf );
            }
        }
    return retVal;
    }
// end of "debug / test method"    

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//   
CSenElement* CWSStarPolicyManager::PolicyL(MSenServiceDescription* aSD)	//codescannerwarnings
    {
    TInt error(KErrGeneral);
    CSenWSDescription* sd = (CSenWSDescription*)aSD;
    CSenElement* ele = WsPolicy1L(sd, error);    
    
    if(error == KErrNone)
        return ele;
    
    return NULL;    
    } 
    
CSenElement* CWSStarPolicyManager::PolicyL(const TDesC8& aUri)	//codescannerwarnings
    {
    TInt error(KErrGeneral);
    
    CSenElement* ele = WsPolicy1L(NULL, aUri, error);    
    
    if(error == KErrNone)
        return ele;
    
    return NULL;    
    } 
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
// 
TInt CWSStarPolicyManager::UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyManager::UpdatePolicyL ")));    
    TInt retVal(KErrNone);
    
    TInt count(iContracts.Count());
    for ( TInt i = 0; i < count; i++ )
        {
        TPtrC8 contract = iContracts[i]->Des();
        RPointerArray<CSenWSDescription> serviceDescriptions;
        CleanupClosePushL(serviceDescriptions);

        TInt leaveCode(KErrNone);
        TInt getSDsError(KErrNone);
        TRAP(leaveCode, getSDsError = iSenHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, contract));
        if( getSDsError && retVal == KErrNone )
            {
            // store the return code from 1st error:
            retVal = getSDsError;
            }
            
        TInt sdCount(serviceDescriptions.Count());
        if( sdCount > 0  && getSDsError == KErrNone && leaveCode == KErrNone )
            {
            for ( TInt j = 0; j < sdCount; j++ )
                {
                CSenWSDescription* sD = serviceDescriptions[j];
                TInt updateError(KErrNone);
                if(sD->Contract() == KWSStarSTSContract)
                    {
                    CWSStarTrustClient* pTrustClient = (CWSStarTrustClient*)sD;   
                    updateError = pTrustClient->UpdatePolicyL( aPolicyHandler, pTrustClient );
                    }
                else if (sD->DescriptionClassType() == MSenServiceDescription::EWSStarServiceSession)
                    {
                    CWSStarServiceSession* pWsSession = (CWSStarServiceSession*)sD;
                    updateError = pWsSession->UpdatePolicyL( aPolicyHandler, pWsSession );
                    }
                if( updateError && retVal == KErrNone )
                    {
                    // store the code from first update error:
                    retVal = updateError;
                    }
                } // for each SD
            } // if any SD to process
            CleanupStack::PopAndDestroy(); //serviceDescriptions
            
        } // for each item in policy map    
    return retVal;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
RFileLogger* CWSStarPolicyManager::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iSenHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }


TInt  CWSStarPolicyManager::RegisterWsPolicyL( const TDesC8& aContract )
    {
    
    RPointerArray<CSenWSDescription> serviceDescriptions;
    CleanupClosePushL(serviceDescriptions);
    TInt retVal(KErrNone);
    TInt leaveCode(KErrNone);
    TInt errorCode(KErrGeneral);    
    TRAP(leaveCode, retVal = iSenHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, aContract));
    if(leaveCode == KErrNone && retVal == KErrNone && serviceDescriptions.Count() > 0)
        {
        for(TInt j = 0; j < serviceDescriptions.Count(); j++)
            {
            CSenWSDescription* sD = serviceDescriptions[j];
            CSenWSDescription* tempSd = CSenWSDescription::NewLC();
                CBufFlat *pBuf = CBufFlat::NewL(64); 
                CleanupStack::PushL(pBuf);
                RBufWriteStream bufWs(*pBuf);
                CleanupClosePushL(bufWs);
                
                if(sD->Contract() == KWSStarSTSContract)
                    {
                    CWSStarTrustClient* pTrustClient = (CWSStarTrustClient*)sD;   
                    pTrustClient->WriteAsXMLToL(bufWs);
                    tempSd->SetReader(*iSenHandlerContext.GetSenCoreServiceManager()->XMLReader());
                    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(pBuf->Ptr(0)));
                    tempSd->ParseL(pBuf->Ptr(0));
                    }
                else if (sD->DescriptionClassType() == MSenServiceDescription::EWSStarServiceSession)
                    {
                    CWSStarServiceSession* pSd = (CWSStarServiceSession*)sD;
                    pSd->WriteAsXMLToL(bufWs);
                    tempSd->SetReader(*iSenHandlerContext.GetSenCoreServiceManager()->XMLReader());
                    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(pBuf->Ptr(0)));
                    tempSd->ParseL(pBuf->Ptr(0));
                    }
            
            CleanupStack::PopAndDestroy(&bufWs);
            CleanupStack::PopAndDestroy(pBuf);

            RegisterWsPolicy1L(tempSd, errorCode);
            
            CleanupStack::PopAndDestroy(tempSd); 

            }
        }
    CleanupStack::PopAndDestroy(&serviceDescriptions);        
    return KErrNone;            
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  
TInt CWSStarPolicyManager::UnRegisterWsPolicyL(MSenServiceDescription& aServiceDescription,
                                                    TInt& aErrorTo)
    {
    CSenWSDescription* sD = (CSenWSDescription*)&aServiceDescription;
    return UnRegisterWsPolicy1L(sD, aErrorTo);
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  

TInt CWSStarPolicyManager::PolicyConsumerAddedL(CSenServiceSession* aNewSession, MSenRemoteServiceConsumer& aRemoteConsumer)
    {
  
    CSenElement* clientPolicy = NULL;
    MSenServicePolicy* service = NULL;
    TPtrC8 cosumerid = aRemoteConsumer.Id();

    if(aNewSession->Contract() == KWSStarSTSContract)
        {
        CWSStarTrustClient* pTrustClient = (CWSStarTrustClient*)aNewSession;   
        service = pTrustClient->ServicePolicy();
        clientPolicy = service->ClientPolicy();        
        }
    else if(aNewSession->DescriptionClassType() == MSenServiceDescription::EWSStarServiceSession)
        {
        service = aNewSession->ServicePolicy();
        clientPolicy = service->ClientPolicy();                
        }
    

    if(clientPolicy)
        {
        RPointerArray<CSenElement> metaDataPoliciesUris;
        CleanupClosePushL(metaDataPoliciesUris);
        clientPolicy->ElementsL(metaDataPoliciesUris, KMetaDataLocalName);
        TInt count(metaDataPoliciesUris.Count());
        if(count> 0)
            {
            CSenElement* metaData = NULL;
            for(TInt i=0; i<count; i++)
                {
                metaData = metaDataPoliciesUris[i];
                if(metaData && metaData->HasContent())
                    {
                    TPtrC8 uriName = metaData->Content();
                    AddPolicyConsumer1L(uriName,cosumerid);        
                    }
                }
            }
        CleanupStack::PopAndDestroy(&metaDataPoliciesUris);             
        }
        
    return KErrNone;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  

TInt CWSStarPolicyManager::AddWsPolicy1L(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo)
{
    return iPolicyManager->AddWsPolicy1L(aPattern, aUri, aPolicy, aErrorTo);
}

TInt CWSStarPolicyManager::RemoveWsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo)
{
    return iPolicyManager->RemoveWsPolicy1L(aPattern, aUri, aErrorTo);
}
                                                                                                                                                                                               
TInt CWSStarPolicyManager::RegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
{
    return iPolicyManager->RegisterWsPolicyL(aPattern, aErrorTo);    
}

TInt CWSStarPolicyManager::UnRegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
{
    return iPolicyManager->UnRegisterWsPolicy1L(aPattern, aErrorTo);
}
                                                    
CSenElement*  CWSStarPolicyManager::WsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo)
{
    return iPolicyManager->WsPolicy1L(aPattern, aUri, aErrorTo);
}

CSenElement*  CWSStarPolicyManager::WsPolicy1L( CSenWSDescription* aPattern, 
                                                TInt& aErrorTo)
{
    return iPolicyManager->WsPolicy1L(aPattern, aErrorTo);
}
CSenElement*  CWSStarPolicyManager::WsPolicy1L( const TDesC8& aProviderId, TInt& aErrorTo)
{
    return iPolicyManager->WsPolicy1L(aProviderId, aErrorTo);
}
TInt CWSStarPolicyManager::AddPolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    return iPolicyManager->AddPolicyConsumer1L(aUri,aConsumerId);    
}

TInt CWSStarPolicyManager::RemovePolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    return iPolicyManager->RemovePolicyConsumer1L(aUri,aConsumerId);    
}
void CWSStarPolicyManager::DataTrafficDetails(TSenDataTrafficDetails& aDetails) 		
{
	aDetails = iPolicyDetails;
}

// END OF FILE

