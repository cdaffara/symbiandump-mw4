/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* A server side session object. Owns instances of  the CPKIService 
* and CPKIWrapper classes.  All requests to the service objects go 
* through this object.
*
*/



#include "pkisession.h"
#include "pkiservice.h"
#include "PKIMapper.h"
#include "pkiwrapper.h"
#include "certificaterequeststore.h"
#include "keyoperationqueue.h"
#include "pkiserviceconstants.h"

#include "log_r6.h"
#include "pkiserviceassert.h"

static const TInt KBufferSizeNotDefined = -1;

CPKISession* CPKISession::NewL(CPKIService& aServer, 
                               CPKIMapper& aMapper,
                               CKeyOperationQueue& aKeyOperationQueue)
    {
    CPKISession* self;
    self = new (ELeave) CPKISession(aServer, aMapper, aKeyOperationQueue);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


CPKISession::CPKISession(CPKIService& aServer, CPKIMapper& aMapper, CKeyOperationQueue& aKeyOperationQueue)
:iServer(aServer), iMapper(aMapper), 
 iKeyOperationQueue(aKeyOperationQueue), iRequiredBufferSize(KBufferSizeNotDefined)
    {
    }


void CPKISession::ConstructL()
{
    LOG_("-> CPKISession::ConstructL()");    
    iWrapper = CPKIWrapper::NewL(iMapper);        
    LOG_("<- CPKISession::ConstructL()");
}

/**---------------------------------------------------------
 *
 * ~CPKISession(void)
 *
 *----------------------------------------------------------*/
CPKISession::~CPKISession(void)
    {
    LOG_("-> CPKISession::~CPKISession()");
    
    delete iKeyList;    	   
    delete iWrapper;
	iUidArray.Close();
	
	iServer.SessionDeleted();
    LOG_("<- CPKISession::~CPKISession()");
    }


/**---------------------------------------------------------
 *
 * ServiceL(const RMessage& aMessage)
 *
 *----------------------------------------------------------*/
void CPKISession::ServiceL(const RMessage2& aMessage)
    {
    TInt Status = KErrNone;
    TInt count = 0;
    
    LOG_1("CPKISession::ServiceL: function = %d", aMessage.Function());

    switch(aMessage.Function())
        {                            
        case PkiService::ECancelPendingOperation:
            iWrapper->CancelPendingOperation();
            aMessage.Complete(KErrNone);
            break;
        
        case PkiService::EGetRequiredBufferSize:
            if (iRequiredBufferSize == KBufferSizeNotDefined)
                {                
                Status = iWrapper->GetRequiredBufferSizeL(aMessage);
                }
            else
                {
                TPckg<TInt> pckgSize(iRequiredBufferSize);
                aMessage.WriteL(0, pckgSize);
                aMessage.Complete(KErrNone);
                }
            break;

        case PkiService::ECertCount:        
            count = iMapper.CertCount(iWrapper->Informational());
            aMessage.Complete(count);
            break;
            
        case PkiService::EApplicableCertCount:
    		{
    		TInt aC = 0;
    		TPckg<TInt> pckgApplCount(aC);
    		aMessage.ReadL(0, pckgApplCount);

    		// Read applications
    		// Allocate list for applications
    		CBufFlat* list = CBufFlat::NewL(sizeof(TUid));
    		CleanupStack::PushL(list);
    		list->ResizeL(aC * sizeof(TUid));
    		TPtr8 ptrList = list->Ptr(0);
    		aMessage.ReadL(1, ptrList);
    		iUidArray.Close();
    		if(aC > 0)
    			{
    			TUid tempUid;
    			for (TInt i = 0; i < aC; i++)
    				{
    				list->Read(i * sizeof(TUid), (TAny*)&tempUid, sizeof(TUid));
    				iUidArray.Append(tempUid);
    				}
    			}
    		CleanupStack::PopAndDestroy(1);     // list
    		
    		TInt matchCount = iMapper.ApplicableCertCount(iUidArray);
    		aMessage.Complete(matchCount); 
    		}
    		break;
       case PkiService::EGetCertDetails:
            {
            TPckgBuf<TSecurityObjectDescriptor> secDescPtr;
            aMessage.ReadL(1, secDescPtr);
            TCertificateListEntry* resultCertInfo = new (ELeave) TCertificateListEntry;
            CleanupStack::PushL(resultCertInfo);            
            Status = iMapper.GetCertDetailsL(secDescPtr(), 
                                             iWrapper->CertStoreType(), 
                                             iWrapper->Informational(),
                                             *resultCertInfo);
		    if (Status == KErrNone)
		        {
		        TPckg<TCertificateListEntry> certDetailsPtr(*resultCertInfo);
	            aMessage.WriteL(0, certDetailsPtr);
		        }
		    aMessage.Complete(Status);
		    CleanupStack::PopAndDestroy(resultCertInfo);            
            }
		    break;
        case PkiService::EGetCertList:
		    iMapper.GetCertListL(aMessage, iWrapper->Informational());
		    aMessage.Complete(KErrNone);
		    break;

        case PkiService::EGetApplicableCertList:		
		    iMapper.GetApplicableCertListL(aMessage, iUidArray);
		    aMessage.Complete(KErrNone);
		    break;

        case PkiService::EGetKeyList:
        	{
        	PKISERVICE_ASSERT(iKeyList != NULL);
        	
	        CBufFlat* list = CBufFlat::NewL(sizeof(TKeyListEntry));
	        CleanupStack::PushL(list);
	        list->ResizeL(iKeyList->Count() * sizeof(TKeyListEntry));	        
	
	        for(TInt i = 0; i < iKeyList->Count(); i++)
	            {
	            const TKeyListEntry& keyInfo = (*iKeyList)[i];
                list->Write(i * sizeof(TKeyListEntry),
                            (TAny*)&keyInfo,
                            sizeof(TKeyListEntry));
	            }
	        TPtr8 ptrList = list->Ptr(0);
	        aMessage.WriteL(0, ptrList);	
	        CleanupStack::PopAndDestroy(list); // list
	          
	        delete iKeyList;
	        iKeyList = NULL;
            aMessage.Complete(KErrNone);
        	}
            break;
    		
        case PkiService::ECertReqCount:                    
            count = iServer.CertificateRequestStore().CertReqCountL();
            aMessage.Complete(count); 
            break;    		
    		
        case PkiService::EGetCertReqList:
            {
            CArrayFixFlat<TCertificateRequestListEntry>* certReqList =
                        iServer.CertificateRequestStore().GetCertReqListLC();
                       
            TUint bufferGranularity = sizeof(TCertificateRequestListEntry);
            if (certReqList->Count() > 0)
                {
                bufferGranularity = bufferGranularity * certReqList->Count();
                }
            CBufFlat* list = CBufFlat::NewL(bufferGranularity);            
            CleanupStack::PushL(list);                    
            
            if (certReqList->Count() > 0)
                {
                list->ResizeL(sizeof(TCertificateRequestListEntry) * certReqList->Count());
                }
                        
            for (TInt i = 0; i < certReqList->Count(); ++i)
                {
                const TCertificateRequestListEntry &certReqInfo = (*certReqList)[i];               
                list->Write(i * sizeof(TCertificateRequestListEntry),
                            &certReqInfo,
                            sizeof(TCertificateRequestListEntry));

                }
            TPtr8 ptrList = list->Ptr(0);
            aMessage.WriteL(0, ptrList);               
                        
            CleanupStack::PopAndDestroy(list);                                                            
            CleanupStack::PopAndDestroy(certReqList);                        
            
            aMessage.Complete(KErrNone);            
            }
            break;
            
        case PkiService::ESaveCertificateRequest:
            {
            TInt requestSize = aMessage.GetDesLength(0);                        
            HBufC8* request = HBufC8::NewLC(requestSize);
            TPtr8 requestPtr = request->Des();
            
            TKeyIdentifier keyId;
            
            aMessage.ReadL(0, requestPtr);
            aMessage.ReadL(2, keyId); //keyId is ignored.
            
            HBufC* certRequestRef = 
                iServer.CertificateRequestStore().SaveCertRequestLC(*request);
            
            aMessage.WriteL(1, *certRequestRef);
            
            CleanupStack::PopAndDestroy(certRequestRef);
            CleanupStack::PopAndDestroy(request);
                    
            aMessage.Complete(KErrNone);
            }
            break;
            
        case PkiService::EReadCertificateRequest:
            {
            TInt certRequestRefSize = aMessage.GetDesLength(0);
            HBufC *certRequestRef = HBufC::NewLC(certRequestRefSize);                     
            TPtr certRequestRefPtr = certRequestRef->Des();
            
            aMessage.ReadL(0, certRequestRefPtr);
            
            if (certRequestRef->Length() == 0)
                {
                User::Leave(KErrNotFound);
                }
                
            HBufC8* certificateRequest = 
                iServer.CertificateRequestStore().ReadCertRequestLC(*certRequestRef);
            
            if (aMessage.GetDesMaxLength(1) >= certificateRequest->Length())
                {
                aMessage.WriteL(1, *certificateRequest);
                aMessage.Complete(KErrNone);
                }
            else
                {          
                iRequiredBufferSize = certificateRequest->Length();       
                aMessage.Complete(KPKIErrBufferTooShort);
                }            
            
            CleanupStack::PopAndDestroy(certificateRequest);
            CleanupStack::PopAndDestroy(certRequestRef);            
            
            }
            break;
            
        case PkiService::EDeleteCertificateRequest:
            {
            TInt certRequestRefSize = aMessage.GetDesLength(0);
            HBufC *certRequestRef = HBufC::NewLC(certRequestRefSize);                     
            TPtr certRequestRefPtr = certRequestRef->Des();
            
            aMessage.ReadL(0, certRequestRefPtr);
            
            iServer.CertificateRequestStore().DeleteCertRequestL(*certRequestRef);
            
            CleanupStack::PopAndDestroy(certRequestRef);
            aMessage.Complete(KErrNone);                        
            }
            break;
        case PkiService::ESetCertStoreType:        
            iWrapper->SetCertStoreType(static_cast<TPkiServiceStoreType>(aMessage.Int0()));
            aMessage.Complete(KErrNone);
            break;

        case PkiService::ESetKeyStoreType:  
            SetKeyStoreL(static_cast<TPkiServiceStoreType>(aMessage.Int0()));
            aMessage.Complete(KErrNone);
            break;

        case PkiService::ESetStoreType:
            iWrapper->SetCertStoreType(static_cast<TPkiServiceStoreType>(aMessage.Int0()));
            SetKeyStoreL(static_cast<TPkiServiceStoreType>(aMessage.Int1()));
            aMessage.Complete(KErrNone);
            break;

		case PkiService::EGetCertStoreType:
			{
			Status = iWrapper->CertStoreType();
			aMessage.Complete(Status);
			break;
			}

		case PkiService::EGetKeyStoreType:
			{
			Status = KeyStore();
			aMessage.Complete(Status);
			break;
			}			
			
        case PkiService::ESetInformational:
            {
            iWrapper->SetInformational(aMessage.Int0());
            aMessage.Complete(KErrNone);
            }
            break;
            
        case PkiService::EInitialize://falls through
        case PkiService::EKeyCount: //falls through    		                
        case PkiService::EGetKeyDetails: //falls through    		
        case PkiService::EDecrypt: //falls through    		
        case PkiService::ESignWithKeyId: //falls through    		
        case PkiService::ESignWithCert: //falls through    		
        case PkiService::EReadPublicKey: //falls through    		
        case PkiService::ELogon: //falls through    		
        case PkiService::ELogoff: //falls through    		
        case PkiService::EChangePassword: //falls through    		
        case PkiService::ERemoveKeypair: //falls through    		
        case PkiService::EGenerateKeypair: //falls through    		
        case PkiService::EStoreKeypair: //falls through    		
            iKeyOperationQueue.AddOperationL(*this, aMessage,
                                             iUsedKeyStore, iWrapper->CertStoreType());
            break;  
        default:      
            iRequiredBufferSize = KBufferSizeNotDefined;
            iWrapper->InitOperation(aMessage);
            break;
        }
    }


void CPKISession::SetKeyStoreL(TPkiServiceStoreType aStoreType)
    {      
    switch(aStoreType)
        {
        case EPkiStoreTypeAny:
            iUsedKeyStore = STORETYPE_ANY_KEY_ID;  
            break;
        case EPkiStoreTypeUser:
            iUsedKeyStore = STORETYPE_USER_KEY_ID;    
            break;
        case EPkiStoreTypeDevice:
            iUsedKeyStore = STORETYPE_DEVICE_KEY_ID;    
            break;                    
        default:
            User::Leave(KPKIErrNotSupported);
            break;            
        }    
    }


TPkiServiceStoreType CPKISession::KeyStore() const
    {
    
    TPkiServiceStoreType usedStore = EPkiStoreTypeAny;        
    switch(iUsedKeyStore)    
        {            
        case STORETYPE_USER_KEY_ID:
            usedStore = EPkiStoreTypeUser;
            break;
        case STORETYPE_DEVICE_KEY_ID:
            usedStore = EPkiStoreTypeDevice;
            break;
        case STORETYPE_ANY_KEY_ID:
            usedStore = EPkiStoreTypeAny;        
            break;
        default:
            PKISERVICE_ASSERT(iUsedKeyStore == 0);
            break;
        }    
    return usedStore;
    }


void CPKISession::SetRequiredBufferSize(TInt aSize)
    {
    iRequiredBufferSize = aSize;
    }


void CPKISession::SetKeyList(CArrayFixFlat<TKeyListEntry> *aKeyList)
    {
    iKeyList = aKeyList;
    }


void CPKISession::InitializeWrapperL(const RMessage2& aMessage)
    {
    iWrapper->InitializeL(aMessage);
    }
