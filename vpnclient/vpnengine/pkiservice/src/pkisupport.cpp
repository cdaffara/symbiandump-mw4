/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* CPKISupport class implements the PKI interface for Symbian Certificate 
* storage.
*
*/



#include <securitydefs.h>
#include <cctcertinfo.h>
#include <x509cert.h>
#include <ccertattributefilter.h>
#include <asymmetric.h>
#include <unifiedcertstore.h>
#include <mctwritablecertstore.h>

#include "pkiwrapper.h"
#include "PKIMapper.h"
#include "mapdescriptor.h"
#include "pkisupport.h"
#include "pkisession.h"
#include "log_r6.h"
#include "pkiserviceassert.h"
#include "pkiserviceconstants.h"
#include <vpnlogmessages.rsg>


CPKISupport* CPKISupport::NewLC(CPKIMapper& aMapper, CPKIWrapper& aWrapper)
    {
    CPKISupport* self = new (ELeave)CPKISupport(aMapper, aWrapper);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPKISupport* CPKISupport::NewL(CPKIMapper& aMapper, CPKIWrapper& aWrapper)
    {
    CPKISupport* self = CPKISupport::NewLC(aMapper, aWrapper);
    CleanupStack::Pop(self);
    return self;
    }

void CPKISupport::ConstructL()
	{
	User::LeaveIfError(iEventMediator.Connect());
	}

CPKISupport::~CPKISupport()
    {
    LOG_("CPKISupport::~CPKISupport");
    Cancel();    

    iCerts.Close();
    delete iCertFilter;
    delete iCertStore;
    iFSession.Close();
    iApplUids.Close();
    delete iImportCertData;
    iEventMediator.Close();
    delete iCertInfoForLogging; 
    iCertInfoForLogging = NULL;
    }

CPKISupport::CPKISupport(CPKIMapper& aMapper, CPKIWrapper& aWrapper) 
    :CActive(CActive::EPriorityStandard), 
    iMapper(aMapper),
    iWrapper(aWrapper),
    iPending(ENoPendingOperation),
    iImportCertDataPtr(NULL, 0)
    {
    LOG_("-> CPKISupport::CPKISupport()\n");   
    CActiveScheduler::Add(this);
    
    SetCertStoreType(EPkiStoreTypeAny);
    LOG_("<- CPKISupport::CPKISupport()\n");
    } 

// ---------------------------------------------------------------------------
// StartInitializeL
// ---------------------------------------------------------------------------
//
void  CPKISupport::StartInitializeL(const RMessage2& aMessage)
    {
    LOG_("-> CPKISupport::StartInitialize()");
    User::LeaveIfError(iFSession.Connect());
    iCertStore = CUnifiedCertStore::NewL(iFSession, ETrue);    
    
    iMessage = aMessage;
    iCallerStatus = NULL;
    iSupportStatus = KErrNone;

    iInitState = EInitInitializeCertStore;
    iCertStore->Initialize(iStatus);
    iPending = EInitializeCertStore;
    SetActive();
    
    LOG_("<- CPKISupport::StartInitialize()");
    }


// ---------------------------------------------------------------------------
// FindInterfacesL
// ---------------------------------------------------------------------------
//
void  CPKISupport::FindInterfacesL()
    {
    TInt i(0);
    
    // Find writable cert interface
    LOG(Log::Printf(_L("Find interfaces")));

    TBool allStoresFound(EFalse);

    MCTWritableCertStore* certStore;

    TInt storeCount = iCertStore->WritableCertStoreCount();
    LOG_1("Found %d cert stores\n", storeCount);


    // Find both user and device cert stores
    for(i = 0; i < storeCount; i++)
        {
        certStore= &(iCertStore->WritableCertStore(i));

        if(certStore->Token().Label().Compare(KUserCertStore) == 0)
            {
            // found the user cert store... See if device cert store 
            // was already found as well
            iWritableCertStore = certStore;

            LOG(Log::Printf(_L("Found writable user certificate store\n")));
            
            if (!allStoresFound) 
                {
                allStoresFound = ETrue;
                }
            else 
                {
                break;
                }
            }
        else if (certStore->Token().Label().Compare(KDeviceCertStore) == 0) 
            {

            // found the device cert store
            iWritableDeviceCertStore = certStore;

            LOG(Log::Printf(_L("Found writable device certificate store\n")));
            
            // See if user cert store was already found as well
            if (!allStoresFound) 
                {
                allStoresFound = ETrue;
                }
            else 
                {
                break;
                }
            }
        }
    
    if (iWritableDeviceCertStore == NULL) 
        {
        LOG(Log::Printf(_L("Note: Device cert store not found\n")));
        }
    }


// ---------------------------------------------------------------------------
// ContinueImportCTFObjectsL
// ---------------------------------------------------------------------------
//
void  CPKISupport::ListAllCertificatesL()
{
    LOG_("-> CPKISupport::ListAllCertificatesL");

    PKISERVICE_ASSERT( !iCertFilter );
    iCertFilter = CCertAttributeFilter::NewL();
    
    iSupportStatus = KErrNone;
    iInitState = EInitRetrieveCertList;
    iImportCounter = 0;
	iToggleSwitch = EFalse;

    // Get list of all known certificates
    GetCertificateStoreListAsync();
    LOG_("<- CPKISupport::ListAllCertificatesL");
}

    
    
// ---------------------------------------------------------------------------
// ImportCTFCertsL
// ---------------------------------------------------------------------------
//
void  CPKISupport::ReadNextCertForImportL()
{
    LOG_("-> CPKISupport::ImportNextCertL()");
    PKISERVICE_ASSERT(iImportCounter < iCerts.Count());
    
    if(iImportCounter == 0)
        {
        LOG_1("Import %d certificates\n", iCerts.Count());
        }
        
    LOG_1("Import certificate:%d", iImportCounter);
    LOG_1("Label: %S", &(iCerts[iImportCounter]->Label()));
        
    delete iImportCertData;
    iImportCertData = NULL;
    iImportCertData = HBufC8::NewL(iCerts[iImportCounter]->Size());        
    iImportCertDataPtr.Set(iImportCertData->Des());
        
    iCertStore->Retrieve(
        *(iCerts[iImportCounter]), iImportCertDataPtr, iStatus);
    iPending = ECertRetrieve;
    SetActive();
    LOG_("<- CPKISupport::ImportCTFCertsL() SetActive() and ret: EFalse");
}

// ---------------------------------------------------------------------------
// ImportOneCTFCertL
// ---------------------------------------------------------------------------
//
void CPKISupport::SaveCertInfoToCacheL()
    {
    LOG_("Saving cert info to cache");
    PKISERVICE_ASSERT(iImportCertData != NULL);
    
    CX509Certificate* certificate = CX509Certificate::NewLC(iImportCertDataPtr);       
    const CCTCertInfo* currentCertInfo = iCerts[iImportCounter]; 
    TPkiServiceStoreType storeType = EPkiStoreTypeAny;
    switch(currentCertInfo->Token().TokenType().Type().iUid)
        {
        case STORETYPE_DEVICE_CERT_ID:
            storeType = EPkiStoreTypeDevice;
            break;
        case STORETYPE_USER_CERT_ID:
            storeType = EPkiStoreTypeUser;
            break;
        default:
            storeType = EPkiStoreTypeAny;
            break;
        }
    
    CMapDescriptor* newMapping = CMapDescriptor::NewL(currentCertInfo->Label(),
                                                      *certificate,
                                                      (TPKICertificateOwnerType)currentCertInfo->CertificateOwnerType(),
                                                      storeType);  
    CleanupStack::PushL(newMapping);
    
    newMapping->SetMapDeletable(currentCertInfo->IsDeletable());
    newMapping->SetMapApplications(iApplUids);
    iApplUids.Reset();
    User::LeaveIfError(iMapper.AddMapping(newMapping));
            
    CleanupStack::Pop(newMapping);
    CleanupStack::PopAndDestroy(certificate);
    iImportCounter++;    
    }

// ---------------------------------------------------------------------------
// GetApplicationsOfCTFCertL
// ---------------------------------------------------------------------------
//
TBool CPKISupport::GetApplicationsOfCTFCertL()
{
    LOG_("-> CPKISupport::GetApplicationsOfCTFCertL()");
    PKISERVICE_ASSERT(iImportCertData != NULL);
    
    if(iToggleSwitch == EFalse)
        {
        if(iCerts[iImportCounter]->CertificateOwnerType() == ECACertificate)
            {
            PKISERVICE_ASSERT(iApplUids.Count() == 0);
            
            iToggleSwitch = ETrue;
            // Get applications				
            iCertStore->Applications(*(iCerts[iImportCounter]), iApplUids, iStatus);
            iPending = EApplications;
            LOG_("<- CPKISupport::GetApplicationsOfCTFCertL() SetActive(), ret: ETrue");
            SetActive();
            return ETrue;
            }
        else
            {
            LOG_("<- CPKISupport::GetApplicationsOfCTFCertL() Not a CA cert, ret: EFalse");
            return EFalse;
            }
        }
    else
        {
        LOG_("<- CPKISupport::GetApplicationsOfCTFCertL() iToggleSwitch == ETrue, ret: EFalse");
        iToggleSwitch = EFalse;
        return EFalse;
        }
}


// ---------------------------------------------------------------------------
// CancelCurrentOperation
// ---------------------------------------------------------------------------
//
void CPKISupport::CancelCurrentOperation()
    {
    LOG_1("CPKISupport::CancelCurrentOperation iPending: %d", iPending);    
    MCTWritableCertStore* certStore(NULL);
    switch(iPending)
        {
        case ENoPendingOperation:
            // No pending operation 
            PKISERVICE_ASSERT( iStatus.Int() != KRequestPending);        
            break;
        case EInitializeCertStore:
            // FALLTROUGH
        case EListCerts:
            // FALLTROUGH
        case EListDevCerts:
            // FALLTROUGH
        case ECertRetrieve:
            // FALLTROUGH
        case ECertRemove:
            // FALLTROUGH
        case ESetTrust:
            // FALLTROUGH
        case ETrusted:
            // FALLTROUGH
        case ESetApplicability:
            // FALLTROUGH
        case EApplications:
            // FALLTROUGH
        case EIsApplicable:
            PKISERVICE_ASSERT( iCertStore );
            iCertStore->Cancel();
            break;
        case ECertAdd:
            if (iCertStoreType == EPkiStoreTypeDevice) 
                {
                LOG_("CPKISupport::CancelCurrentOperation cancelling device cert store op");
                certStore = iWritableDeviceCertStore;
                }
            else if (iCertStoreType == EPkiStoreTypeUser)
                {
                LOG_("CPKISupport::CancelCurrentOperation cancelling user cert store op");
                certStore = iWritableCertStore;
                }
            else 
                {
                LOG_("CPKISupport::CancelCurrentOperation cancelling user cert store op (unknown store type was defined)");
                certStore = iWritableCertStore;
                }
            PKISERVICE_ASSERT( certStore );
            certStore->CancelAdd();
            break;
        default:
            LOG_("CPKISupport::CancelCurrentOperation - default");
            break;           
        }
    }       

// ---------------------------------------------------------------------------
// ConvertPKIAlgorithm
// ---------------------------------------------------------------------------
//
CCTKeyInfo::EKeyAlgorithm CPKISupport::ConvertPKIAlgorithm(TPKIKeyAlgorithm aAlg)
{
    CCTKeyInfo::EKeyAlgorithm algorithm = CCTKeyInfo::EInvalidAlgorithm;

    switch(aAlg)
        {
        case EPKIRSA:
            algorithm = CCTKeyInfo::ERSA;
            break;
        case EPKIDSA:
            algorithm = CCTKeyInfo::EDSA;
            break;
        case EPKIDH:
            algorithm = CCTKeyInfo::EDH;
            break;
        default:
            break;
        }
    return algorithm;
}

// ---------------------------------------------------------------------------
// ConvertSymbianAlgorithm
// ---------------------------------------------------------------------------
//
TPKIKeyAlgorithm CPKISupport::ConvertSymbianAlgorithm(CCTKeyInfo::EKeyAlgorithm aAlg)
{
    TPKIKeyAlgorithm algorithm = EPKIInvalidAlgorithm;

    switch(aAlg)
        {
        case ERSA:
            algorithm = EPKIRSA;
            break;
        case EDSA:
            algorithm = EPKIDSA;
            break;
        case EDH:
            algorithm = EPKIDH;
            break;
        default:
            break;
        }
    return algorithm;
}

// ---------------------------------------------------------------------------
// CActive methods
// ---------------------------------------------------------------------------
//
void CPKISupport::RunL()
    {
    LOG_1("CPKISupport::RunL err:%d", iStatus.Int());
    
    iSupportStatus = iStatus.Int();
	if((iSupportStatus == KErrNotSupported) && 
	    iToggleSwitch && (iInitState == EInitCompleteImportCerts))
		{
		// Some implementations of MIDP2 certstore return KErrNotSupported
		// when calling iCertStore->Applications()
		LOG_1("CPKISupport::RunL ignore error :%d", iSupportStatus);
		iSupportStatus = KErrNone;		
		}
		
    iPending = ENoPendingOperation;
    if(iSupportStatus == KErrNone)
        {
        DoRunOperationL();     
        }
    else
        {
        iSubState = ESSCompleteRequest;
        }
    
    if(iSubState == ESSCompleteRequest)
        {
        if(iInitState != EInitDone)
            {
            iMessage.Complete(iSupportStatus);
            }
        else
            {
            CompleteCallerStatus( iSupportStatus );
            }
		}
    }

// ---------------------------------------------------------------------------
// GetRequiredBufferSize
// ---------------------------------------------------------------------------
//    
TInt CPKISupport::GetRequiredBufferSize()
    {
    LOG_1("CPKISupport::GetRequiredBufferSize:%d", iRequiredBufferLength);
    return iRequiredBufferLength;
    }

// ---------------------------------------------------------------------------
// SetCallerStatusPending
// ---------------------------------------------------------------------------
//    
void CPKISupport::SetCallerStatusPending(TRequestStatus& aStatus)
    {
    iCallerStatus = &aStatus;
    aStatus = KRequestPending;    
    }

// ---------------------------------------------------------------------------
// CompleteCallerStatus
// ---------------------------------------------------------------------------
//    
void CPKISupport::CompleteCallerStatus(TInt aError)
    {
    LOG_("-> CPKISupport::CompleteCallerStatus");
    if(iCallerStatus)
        {
        if(iCurrentFunction == PkiService::EStoreCertificate ||
           iCurrentFunction == PkiService::EAttachCertificate)
        	LogCertStoring(aError);
        
        LOG_("Completing caller status");
        User::RequestComplete(iCallerStatus, aError);        
        }
    else
        {
        LOG_("Completing rmessage");
        iMessage.Complete(aError);
        }      
    LOG_("<- CPKISupport::CompleteCallerStatus");        
    }

// ---------------------------------------------------------------------------
// LogCertStoring
// ---------------------------------------------------------------------------
//    
void CPKISupport::LogCertStoring(TInt aError)
    {
    TUid uId(KPkiServiceUid3);
    if(aError == KErrNone)
    	iEventMediator.ReportLogEvent(uId, EInfo, R_VPN_MSG_CERT_INSTALLED, 1, iCertInfoForLogging);
    else 
    	{
    	TPckgBuf<TInt> int1Des(aError);
    	iEventMediator.ReportLogEvent(uId, EError, R_VPN_MSG_CERT_INSTALL_FAILED, 2, &int1Des, iCertInfoForLogging);
    	}
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CPKISupport::DoCancel()
    {
    LOG_1("CPKISupport::DoCancel:%d", iStatus.Int());
    CancelCurrentOperation();  
    CompleteCallerStatus( iStatus.Int() );
    }

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CPKISupport::RunError(TInt aError)
    {
    LOG_1("RunError status = %d", aError);
    CompleteCallerStatus( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// SetCurrentFunction
// ---------------------------------------------------------------------------
//
void CPKISupport::SetCurrentFunction(TInt aFunc)
    {
    iCurrentFunction = aFunc;
    iSubState = ESSComplete;
    }


// ---------------------------------------------------------------------------
// StoreCertificateL
// ---------------------------------------------------------------------------
//
void CPKISupport::StoreCertificateL(const TDesC &aLabel, 
                                    TCertificateOwnerType aOwnerType, 
                                    const TDesC8 &aBufferPtr, 
                                    const TBool& aIsDeletable, 
                                    TRequestStatus& aStatus)
{
    LOG_("Store certificate");
    iSupportStatus = KErrNone;
    SetCallerStatusPending( aStatus );
    iKeyId.Zero();

    MCTWritableCertStore* certStore(NULL);

    CX509Certificate* tempCert = CX509Certificate::NewLC(aBufferPtr);
    iKeyId = tempCert->SubjectKeyIdentifierL();
    CleanupStack::PopAndDestroy();

    if(aOwnerType == EUserCertificate)
        {
        if (iCertStoreType == EPkiStoreTypeDevice) 
            {
            LOG(Log::Printf(_L(" Using device cert store\n")));
            certStore = iWritableDeviceCertStore;
            }
        else if (iCertStoreType == EPkiStoreTypeUser)
            {
            LOG(Log::Printf(_L(" Using user cert store\n")));
            certStore = iWritableCertStore;
            }
        else if (iCertStoreType == EPkiStoreTypeAny)
            {
            LOG(Log::Printf(_L(" Any cert store type, using user cert store\n")));
            certStore = iWritableCertStore;
            }
        else 
            {
            LOG(Log::Printf(_L(" Error: Unknown cert store type, defaulting to user cert store\n")));
            certStore = iWritableCertStore;
            }
        }
    else 
        {
        certStore = iWritableCertStore;
        }
    
    certStore->Add(aLabel, EX509Certificate, 
                   aOwnerType, NULL, NULL, 
                   aBufferPtr, aIsDeletable, iStatus);
    
    ExtractCertInfoL(aLabel, aOwnerType, aBufferPtr);
        
    iPending = ECertAdd;
    SetActive();
}

// ---------------------------------------------------------------------------
// ExtractCertInfoL
// ---------------------------------------------------------------------------
//
void  CPKISupport::ExtractCertInfoL(const TDesC& aLabel,
								    const TCertificateOwnerType& aOwnerType,
								    const TDesC8& aBufferPtr)
{
	_LIT8(KFormat,"Type: %S, Label: %S, Subject: %S, Issuer: %S");
	
	TBuf8<4> type;
	if(aOwnerType == EPKICACertificate)
		type.Copy(_L8("CA"));
   	else
   		type.Copy(_L8("USER"));
	
	CX509Certificate* tempCert = CX509Certificate::NewLC(aBufferPtr);
	
	HBufC* subject = tempCert->SubjectL();
	CleanupStack::PushL(subject);
	
	HBufC* issuer = tempCert->IssuerL();
	CleanupStack::PushL(issuer);
	
	// Calculates size of 
	// "Type: <type>  Label: <label>  Subject: <subject>  Issuer: <issuer>"
    TInt bufSize(((TDesC8)KFormat).Length() + type.Length() + aLabel.Length() + subject->Length() + issuer->Length());
    delete iCertInfoForLogging; iCertInfoForLogging = NULL;
	iCertInfoForLogging = HBufC8::NewL(bufSize);
   	
	TPtr8 ptrLogMsg = iCertInfoForLogging->Des();        	
   	ptrLogMsg.Format(KFormat, &type, &aLabel, subject, issuer);
	
	CleanupStack::PopAndDestroy(issuer);
	CleanupStack::PopAndDestroy(subject);
    CleanupStack::PopAndDestroy(tempCert);
}


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::AttachCertificateL(const TDesC& aLabel, 
    const TDesC8 &aBufferPtr, 
    TRequestStatus& aStatus)
{
    TKeyIdentifier tempKeyId;
    CX509Certificate* certificate = CX509Certificate::NewLC(aBufferPtr);

    LOG_("Attach certificate");
    
    iSupportStatus = KErrNone;   
    
    ExtractCertInfoL(aLabel, EUserCertificate, aBufferPtr);    
    iKeyId = certificate->SubjectKeyIdentifierL();
        
    SetCallerStatusPending( aStatus );
    CleanupStack::PopAndDestroy(certificate);    // certificate
    
    iSubState = ESSComplete;

    MCTWritableCertStore* certStore(NULL);

    LOG(Log::Printf(_L("-------------------"));

        TBuf<256> buf;
        buf.Format(_L("Attaching certificate '%S'"), &aLabel);
        Log::Printf(buf);
        Log::Printf(_L("Certificate's KEY ID:"));
        Log::HexDump(NULL, NULL, iKeyId.Ptr(), iKeyId.Length());
    );
    if (iCertStoreType == EPkiStoreTypeDevice) 
        {
        LOG(Log::Printf(_L("CPKISupport::ContinueAttachCertificate() Using device cert store\n")));
        certStore = iWritableDeviceCertStore;
        }
    else if (iCertStoreType == EPkiStoreTypeUser)
        {
        LOG(Log::Printf(_L("CPKISupport::ContinueAttachCertificate() Using user cert store\n")));
        certStore = iWritableCertStore;
        }
    else if (iCertStoreType == EPkiStoreTypeAny)
        {
        LOG(Log::Printf(_L("CPKISupport::ContinueAttachCertificate() Any cert store, using user cert store\n")));
        certStore = iWritableCertStore;
        }
    else 
        {
        LOG(Log::Printf(_L("Error: Unknown cert store type, defaulting to user cert store\n")));
        certStore = iWritableCertStore;
        }
    LOG(Log::Printf(_L("-------------------")));

    certStore->Add(aLabel, 
                   EX509Certificate, 
                   EUserCertificate, 
                   NULL, 
                   NULL, 
                   aBufferPtr, 
                   iStatus);
    iPending = ECertAdd;
    SetActive();
}


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::RetrieveCertificateL(const TDesC &aLabel, 
    TPtr8 &aBufferPtr, const TPKICertificateOwnerType& aType, 
    TRequestStatus& aStatus)
{
    LOG(Log::Printf(_L("Retrieve certificate\n")));
    iSupportStatus = KErrNone;
    iCerts.Close();
    iSubState = ESSContinue;
    iOutBufferPtr = &aBufferPtr;
    SetCallerStatusPending( aStatus );
    SelectCertificateL(aLabel, aType);
}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::ContinueRetrieveCertificate()
{
    iSubState = ESSComplete;
    iPending = ECertRetrieve;
    
    TUint certificateSize = iCerts[0]->Size();
    iRequiredBufferLength = certificateSize;
    if (certificateSize <= iOutBufferPtr->MaxLength())
        {        
        iCertStore->Retrieve(*(iCerts[0]), *iOutBufferPtr, iStatus);
        SetActive();
        }
    else
        {
        iStatus = KRequestPending;
        SetActive();
        
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete(ownStatus, KPKIErrBufferTooShort);
        }
}


/**
    Removes all unnecessary certificates from the certificate
    list before generating the local mapping data.
    More specifically, all MIDP2 and non X509 certificates are removed.
    (MIDP2 certificates can never have a "VPN" trust setting.)
    
    iListCleaned instance variable is used to limit the frequency
    of this cleanup - it's only necessary once, during PKI startup.
*/
void CPKISupport::CleanupCertListL() 
    {
    LOG_("Removing invalid certs (MIDP2 certs)");

    TInt certcount = iCerts.Count();
    _LIT(KMidp2Label, "MIDP2");
    LOG_1("Total cert count, before cleanup: %d", iCerts.Count());
    RMPointerArray<CCTCertInfo> removedInfos;
    CleanupClosePushL(removedInfos);
    for (TInt i = certcount - 1; i >= 0; i--) 
        {
        CCTCertInfo* info = iCerts[i];
        if (info->Label().Compare(KMidp2Label) == 0 ||
            info->CertificateFormat() != EX509Certificate) 
            {
            // CCTCertInfo has private destructor
            removedInfos.AppendL( info );
            iCerts.Remove(i);
            continue;
            }
        }
    CleanupStack::PopAndDestroy(); // removedInfos
    iCerts.Compress();
    LOG_1("Total cert count, after cleanup: %d", iCerts.Count());
    }

// ---------------------------------------------------------------------------
// DoRunOperationL
// ---------------------------------------------------------------------------
//
void CPKISupport::DoRunOperationL()
    {
    LOG_1("CPKISupport::DoRunOperationL iInitState:%d", iInitState);
    LOG_1("CPKISupport::DoRunOperationL iCurrentFunction:%d", 
        iCurrentFunction);
    LOG_1("CPKISupport::DoRunOperationL iSubState:%d", iSubState);
    
    switch(iInitState)
        {
        case EInitDone:
            DoRunLoggedInOperationL();
            break;

        case EInitInitializeCertStore:            
            if(iMapper.CacheCreated())
                {
                FindInterfacesL();
                iInitState = EInitDone;
                iMessage.Complete(KErrNone);
                return;
                }
            else
                {
				iMapper.SetCacheCreated();
                FindInterfacesL();
                ListAllCertificatesL();
                }
            break;
        case EInitRetrieveCertList:
            LOG_("CPKISupport::DoRunOperationL() EInitRetrieveCertList");
            // Cert list might be new. Remove all MIDP2 certificates first,
            // if it hasn't been already done
            CleanupCertListL();
            iInitState = EInitCompleteImportCerts;
            if(iImportCounter < iCerts.Count())
                {
                ReadNextCertForImportL();
                }
            else
                {
                LOG_(" Cert store is empty");
                iInitState = EInitDone;
                iMessage.Complete(KErrNone);
                }
            break;
        case EInitCompleteImportCerts:
            LOG_("CPKISupport::DoRunOperationL() EInitCompleteImportCerts");            
			if(GetApplicationsOfCTFCertL())
				{
                LOG_(" Getting cert's applications returned with FALSE, breaking");
				break;
				}
            SaveCertInfoToCacheL();    // Handles one certificate, if found
            if(iImportCounter < iCerts.Count())
                {
                ReadNextCertForImportL();
                }
            else
                {
                LOG_(" All certificates imported, doing clean ups");
                // Cleanup
                delete iImportCertData;
                iImportCertData = NULL;

                iCerts.Close();

                delete iCertFilter;
                iCertFilter = NULL;
                iInitState = EInitDone;
                iMessage.Complete(KErrNone);
                }
            break;  // repeat            
        default:
            PKISERVICE_INVARIANT();
            iSupportStatus = KPKIErrUnexpectedState;
            iSubState = ESSCompleteRequest;
            break;
        } // End switch(iInitState)
    
    }

// ---------------------------------------------------------------------------
// DoRunLoggedInOperationL
// ---------------------------------------------------------------------------
//
void CPKISupport::DoRunLoggedInOperationL()
    {
    LOG_1("CPKISupport::DoRunLoggedInOperationL iCurrentFunction:%d", 
        iCurrentFunction);
    LOG_1("CPKISupport::DoRunLoggedInOperationL iSubState:%d", iSubState);
     
    switch(iCurrentFunction)
        {
        case PkiService::EAttachCertificate:
            switch(iSubState)
                {
                case ESSComplete:
                    iSubState = ESSCompleteRequest;
                    break;
                default:
                    iSupportStatus = KPKIErrUnexpectedState;
                    iSubState = ESSCompleteRequest;
                    break;
                }
            break;
            
        case PkiService::EReadCertificate:
            switch(iSubState)
                {
                case ESSContinue:
                    {
                    if(iCerts.Count() > 1)
                        {
                        LOG(Log::Printf(_L("Duplicate Certificate\n")));
                        iSubState = ESSCompleteRequest;
                        iSupportStatus = KErrNotFound;
                        }
                    else if (iCerts.Count() < 1)
                        {
                        LOG(Log::Printf(_L("No matching certificates found\n")));
                        iSubState = ESSCompleteRequest;
                        iSupportStatus = KErrNotFound;
                        }
					else if ((iCerts[0]->CertificateOwnerType() == EUserCertificate) &&
							(((iCertStoreType == EPkiStoreTypeDevice) && (iCerts[0]->Token().Label().Compare(KDeviceCertStore)!=0)) ||
                    	  	((iCertStoreType == EPkiStoreTypeUser) && (iCerts[0]->Token().Label().Compare(KUserCertStore)!=0))))
						{
						LOG_1("User certificate's certificate info does not match certificate store type. Store type is: %d", iCertStoreType);
						iSubState = ESSCompleteRequest;
						iSupportStatus = KErrNotFound;                    	
						}
                    else
                        {
                        ContinueRetrieveCertificate();
                        }
                    break;
                    }
                case ESSComplete:
                    iSubState = ESSCompleteRequest;
                    break;
                default:
                    iSupportStatus = KPKIErrUnexpectedState;
                    iSubState = ESSCompleteRequest;
                    break;
                }
            break;
            
        case PkiService::ESetTrust:
        case PkiService::ETrusted:
            switch(iSubState)
                {
                case ESSContinue:
                    {
                    if(iCerts.Count() != 1)
                        {
                        iSubState = ESSCompleteRequest;
                        iSupportStatus = KErrNotFound;
                        }
                    else
                        {
                        if(iCurrentFunction == PkiService::ESetTrust)
                            {
                            ContinueSetTrust();
                            }
                        else
                            {
                            ContinueTrusted();
                            }
                        }
                    break;
                    }
                case ESSComplete:
                    if(iCurrentFunction == PkiService::ETrusted)
                        {
                        iWrapper.SetTrusted(iTrusted);
                        }
                    iSubState = ESSCompleteRequest;
                    break;
                default:
                    iSupportStatus = KPKIErrUnexpectedState;
                    iSubState = ESSCompleteRequest;
                    break;
                }
            break;

        case PkiService::ESetApplicability:
        case PkiService::EApplications:
            switch(iSubState)
                {
                case ESSContinue:
                    {
                    if(iCerts.Count() != 1)
                        {
                        iSubState = ESSCompleteRequest;
                        iSupportStatus = KErrNotFound;
                        }
                    else
                        {
                        if(iCurrentFunction == PkiService::ESetApplicability)
                            {
                            ContinueSetApplicability();
                            }
                        else if(iCurrentFunction == PkiService::EApplications)
                            {
                            ContinueApplications();
                            }
                        else
                            {
                            PKISERVICE_INVARIANT();
                            }
                        }
                    break;
                    }
                case ESSComplete:
                    if(iCurrentFunction == PkiService::EApplications)
                        {
                        iWrapper.SetApplications(iApplUids);
                        }
                            
                    iSubState = ESSCompleteRequest;
                    break;
                default:
                    iSupportStatus = KPKIErrUnexpectedState;
                    iSubState = ESSCompleteRequest;
                    break;
                }
            break;

        case PkiService::ERemoveCertificate:
            switch(iSubState)
                {
                case ESSContinue:
                    if(iCerts.Count() != 1)
                        {
                        iSupportStatus = KPKIErrNotFound;
                        iSubState = ESSCompleteRequest;
                        }
                    else
                        {
                        ContinueRemoveCertificate();
                        }
                    break;
                case ESSComplete:
                    iSubState = ESSCompleteRequest;
                    break;
                default:
                    iSupportStatus = KPKIErrUnexpectedState;
                    iSubState = ESSCompleteRequest;
                    break;
                }
            break;            
        default:
            iSubState = ESSCompleteRequest;
            break;
        }
    }    






// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::RemoveCertificateL(const TDesC &aLabel, 
    TRequestStatus& aStatus)
{
    LOG(Log::Printf(_L("Remove certificate\n")));
    iSupportStatus = KErrNone;
    iCerts.Close();
    iSubState = ESSContinue;
    SetCallerStatusPending( aStatus );
    SelectCertificateL(aLabel);
}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::ContinueRemoveCertificate()
{
    iCertStore->Remove(*(iCerts[0]), iStatus);
    iPending = ECertRemove;
    iSubState = ESSComplete;
    SetActive();
}


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::SetTrustL(const TDesC &aLabel, 
    TBool aTrusted, TRequestStatus& aStatus)
{
    LOG(Log::Printf(_L("SetTrust\n")));
    iSupportStatus = KErrNone;
    iCerts.Close();
    iSubState = ESSContinue;
    iTrusted = aTrusted;
    SetCallerStatusPending( aStatus );
    SelectCertificateL(aLabel);
}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::ContinueSetTrust()
{
    iSubState = ESSComplete;    
    iCertStore->SetTrust(*(iCerts[0]), iTrusted, iStatus);
    iPending = ESetTrust;
    SetActive();
}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::TrustedL(const TDesC &aLabel, 
                           TRequestStatus& aStatus)
{
    LOG(Log::Printf(_L("Trusted\n")));
    iSupportStatus = KErrNone;
    iCerts.Close();
    iSubState = ESSContinue;
    SetCallerStatusPending( aStatus );
    SelectCertificateL(aLabel);
}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::ContinueTrusted()
{
    iSubState = ESSComplete;
    iCertStore->Trusted(*(iCerts[0]), iTrusted, iStatus);
    iPending = ETrusted;
    SetActive();
}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::SetApplicabilityL(const TDesC &aLabel, 
                                    const RArray<TUid>& aApplUids, TRequestStatus& aStatus)
{
    LOG(Log::Printf(_L("SetApplicability\n")));
    iSupportStatus = KErrNone;
    iCerts.Close();
    iSubState = ESSContinue;
	iApplUids.Close();
	for(TInt i = 0;i<aApplUids.Count();i++)
		{
		iApplUids.Append(aApplUids[i]);
		}
    SetCallerStatusPending( aStatus );
    SelectCertificateL(aLabel);
}

// ---------------------------------------------------------------------------
// ContinueSetApplicability
// ---------------------------------------------------------------------------
//
void CPKISupport::ContinueSetApplicability()
    {
    PKISERVICE_ASSERT(iCerts.Count());
    iSubState = ESSComplete;
    iCertStore->SetApplicability(*(iCerts[0]), iApplUids, iStatus);
    iPending = ESetApplicability;
    SetActive();
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPKISupport::ApplicationsL(const TDesC &aLabel,
                                TRequestStatus& aStatus)
{
    LOG(Log::Printf(_L("Applications\n")));
    iSupportStatus = KErrNone;
    iCerts.Close();
    iSubState = ESSContinue;
    SetCallerStatusPending( aStatus );
    SelectCertificateL(aLabel);
}

// ---------------------------------------------------------------------------
// ContinueApplications
// ---------------------------------------------------------------------------
//
void CPKISupport::ContinueApplications()
{
    iSubState = ESSComplete;
	iApplUids.Close();
    iCertStore->Applications(*(iCerts[0]), iApplUids, iStatus);
    iPending = EApplications;
    SetActive();
}


// ---------------------------------------------------------------------------
// SelectCertificateL
// ---------------------------------------------------------------------------
//
void CPKISupport::SelectCertificateL(const TDesC &aLabel, 
                                     const TPKICertificateOwnerType& aType )
{
    delete iCertFilter;
    iCertFilter = NULL;
    iCertFilter = CCertAttributeFilter::NewL();

    LOG(Log::Printf(_L(" Select by label: %S\n"), &aLabel));
    iCertFilter->SetLabel(aLabel);

    if (aType != 0) 
        {
        LOG_1(" Select by owner type: %d", aType);
        iCertFilter->SetOwnerType(TCertificateOwnerType(aType));
        }
    GetCertificateStoreListAsync();
}



// ---------------------------------------------------------------------------
// GetCertificateStoreListAsync()
// ---------------------------------------------------------------------------
//
void CPKISupport::GetCertificateStoreListAsync()
    {
    LOG_1("-> CPKISupport::GetCertificateStoreListAsync() iSubState:%d",
            iSubState );
    // preconditions
    PKISERVICE_ASSERT( iCertFilter && iStatus != KRequestPending );
    LOG_1("CPKISupport::GetSertificateStoreListAsync iSubState:%d", 
        iSubState );

    // Get list of all known certificates
    iCertStore->List(iCerts, *iCertFilter, iStatus);
    iPending = EListCerts;
    SetActive();
    LOG_("<- CPKISupport::GetCertificateStoreListAsync()");
    }


