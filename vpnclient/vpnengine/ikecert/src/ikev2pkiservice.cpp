/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   IKEv2 specifig certificate reading related stuff
*
*/

#include <x500dn.h>
#include <x509cert.h>
#include <asn1dec.h>

#include "ikev2pkiservice.h"
#include "utlcrypto.h"
#include "ikecert.h"
#include "ikecaelem.h"
#include "ikecalist.h"
#include "ikedebug.h"
#include "ikepolparser.h"
#include "ikev2const.h"
#include "ikecertconst.h"
//
// CIkePkiService Class
//
_LIT8(KEmptyString, "");


const TInt KDefaultCertificateBufferSize = 2048;

//
//  Certificate field indicators for GetCertificateFieldDERL()
//

#ifdef _DEBUG

#define SET_ACTIVE DEBUG_LOG2(_L("CIkeV2PkiService::SetActive (0x%x) %d\n"), this, __LINE__);\
                   SetActive()

#else

#define SET_ACTIVE SetActive()

#endif 


EXPORT_C CIkeV2PkiService* CIkeV2PkiService::NewL(MIkeV2PkiServiceObserver& aObserver, MIkeDebug& aDebug)
    {     
    CIkeV2PkiService* self = new (ELeave) CIkeV2PkiService(aObserver, aDebug);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CIkeV2PkiService::CIkeV2PkiService(MIkeV2PkiServiceObserver& aObserver, MIkeDebug& aDebug)
    :CActive(EPriorityStandard),  
    iObserver(aObserver),
    iDebug(aDebug),
    iState(EPkiServiceIdle),     
    iCertPtr(NULL, 0)
    {
    CActiveScheduler::Add(this);
    }


void CIkeV2PkiService::ConstructL()
    {    
    User::LeaveIfError(iPkiService.Connect());

    iTrustedCAList   = new (ELeave) CIkeCaList(2);
    iReadCertificate = HBufC8::NewL(KDefaultCertificateBufferSize);
    iCertPtr.Set(iReadCertificate->Des());

    //The code assumes that these are not NULL.
    //Reallocated, when needed
    iSubjName = HBufC8::NewL(2);      
    iRfc822Name = HBufC8::NewL(2);              
    }


EXPORT_C CIkeV2PkiService::~CIkeV2PkiService()
    {
	Cancel();

    delete iUserCertificate;
    delete i1Certificate;
    delete i2Certificate;
    delete i2CertificateName;
	delete iTrustedCAList;

    iCasTrustedByPeer.Reset();
    iCasTrustedByPeer.Close();

	delete iCaName;
	delete iReadCertificate;
	delete iSubjName;
	delete iRfc822Name;
	
	iPkiService.Close();	
    }


void CIkeV2PkiService::DoCancel()
    {
    
    iPkiService.CancelPendingOperation();    
    iState = EPkiServiceIdle;
    
    delete iCaName;
    iCaName = NULL;

    __ASSERT_DEBUG(iReadCertificate != NULL, User::Invariant());
    iReadCertificate->Des().Zero();
    
        
    __ASSERT_DEBUG(iSubjName != NULL, User::Invariant());
    iSubjName->Des().Zero();
    
    __ASSERT_DEBUG(iRfc822Name != NULL, User::Invariant());
    iRfc822Name->Des().Zero();

			
    iCasTrustedByPeer.Reset();    
    
    delete iIkeDataCAList;		
    iIkeDataCAList = NULL;
    
    iTrustedCAList->ResetAndDestroy();
    }


TInt CIkeV2PkiService::RunError(TInt /*aError*/)
    {    
    //Currently RunL may leave.
    //But we seem to ignore the possible leave.
    
	return KErrNone; 
    }


EXPORT_C void CIkeV2PkiService::ReadTrustedUserCertificateL()
    {    
    __ASSERT_ALWAYS(!IsActive(), User::Invariant());
    __ASSERT_ALWAYS(iTrustedCAList != NULL, User::Invariant());    
    __ASSERT_ALWAYS(iIkeData->iOwnCert.iOwnCertExists, User::Invariant());

    iCasTrustedByPeer.Reset();
		
	for (TInt i = 0; i < iTrustedCAList->Count(); ++i)
	    {		    
        CIkeCaElem* caElem = (*iTrustedCAList)[i];
        User::LeaveIfError(iCasTrustedByPeer.Append(caElem));
	    }


    iState = EReadingCertificate;

    if (iTrustedCAList->Count() > 0)
        {
        CIkeCaElem* CaElem = iCasTrustedByPeer[0];	                   
        HBufC8* caName = IkeCert::GetCertificateFieldDERL(CaElem->Certificate(), KSubjectName);
        if (caName == NULL)
            {
            User::Leave(KErrArgument);
            }
        delete iCaName;
        iCaName = caName;
        
        ReadUserCertificateL(*iCaName, EFalse);
        }
    else
        {
        //No CA's found.
        //We can't read anything
        User::Leave(KErrNotFound);
        }
    }


EXPORT_C TInt CIkeV2PkiService::Ikev2SignatureL(const TDesC8& aTrustedAuthority, 
                                                const TOwnCertInfo& aOwnCertInfo, 
                                                const TDesC8& aMsgOctets, 
                                                TDes8& aSignature, TUint8 aAuthMeth)
    {
    __ASSERT_ALWAYS(!IsActive(), User::Invariant());	

	TPKIKeyAlgorithm keyAlgorithm = EPKIRSA;	
    TInt length = aOwnCertInfo.iSubjectDnSuffix.Length();
    if ( length )
        {
        delete iSubjName;
        iSubjName = NULL;
        iSubjName = HBufC8::NewL(length);  	   			 
        iSubjName->Des().Copy(aOwnCertInfo.iSubjectDnSuffix);		
        }
    else 
        {
        iSubjName->Des().Zero();
        } 

    length = aOwnCertInfo.iRfc822NameFqdn.Length();
    if ( length )
        {
        delete iRfc822Name;
        iRfc822Name = NULL;
        iRfc822Name = HBufC8::NewL(length);  	   			 
        iRfc822Name->Des().Copy(aOwnCertInfo.iRfc822NameFqdn);        	 
        }
    else
        {
        iRfc822Name->Des().Zero();
        }

	//
	// Build PKCS1v15 format signature (ASN1 encoded) for RSA and SHA1 for DSA
	//
	CUtlMessageDigest* digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
	CleanupStack::PushL(digest);
	HBufC8* asn1EncodedHash =NULL;
	HBufC8* DSSHash = NULL;
			
	switch( aAuthMeth )
		{
			case RSA_DIGITAL_SIGN:
				asn1EncodedHash = IkeCert::BuildPkcs1v15HashL(digest->Final(aMsgOctets));
				User::LeaveIfNull(asn1EncodedHash);
	    		CleanupStack::PopAndDestroy(digest);
    			CleanupStack::PushL(asn1EncodedHash);
    			User::LeaveIfError(iPkiService.Sign(aTrustedAuthority, *iSubjName, *iRfc822Name, 
		                                    		EX509DigitalSignature, aOwnCertInfo.iPrivateKeyLength, 
	                                    			keyAlgorithm, *asn1EncodedHash, aSignature));
   				CleanupStack::PopAndDestroy(asn1EncodedHash);
   				DEBUG_LOG(_L("Signing Auth data using RSA key."));
   				break;
			case DSS_DIGITAL_SIGN:
				DSSHash = HBufC8::New(20);
				DSSHash->Des().Append(digest->Final(aMsgOctets));
				CleanupStack::PopAndDestroy(digest);
    			CleanupStack::PushL(DSSHash);
				User::LeaveIfError(iPkiService.Sign(aTrustedAuthority, *iSubjName, *iRfc822Name, 
		                                    EX509DigitalSignature, aOwnCertInfo.iPrivateKeyLength, 
	                                    	keyAlgorithm, *DSSHash, aSignature));
   				CleanupStack::PopAndDestroy(DSSHash);
   				DEBUG_LOG(_L("Signing Auth data using DSA key."));
   				break;
   			default:
   				DEBUG_LOG1(_L("Authentication method %d not supported when using digital signatures."), aAuthMeth);
   				User::Leave(KErrNotSupported);
   				break;			
		}

	return aSignature.Length();
    }	 


EXPORT_C const CIkeCaList& CIkeV2PkiService::CaList() const
    {
    return *iTrustedCAList;
    }
            
    
EXPORT_C const TDesC8& CIkeV2PkiService::UserCertificateData() const 
    {    
    if (iUserCertificate != NULL)
        {
        return *iUserCertificate;
        }
    else
        {
        return KEmptyString;
        }
    }

EXPORT_C const TDesC8& CIkeV2PkiService::I2CertificateData() const 
    {    
    if (i2Certificate != NULL)
        {
        return *i2Certificate;
        }
    else
        {
        return KEmptyString;
        }
    }

EXPORT_C const TDesC8& CIkeV2PkiService::I1CertificateData() const 
    {    
    if (i1Certificate != NULL)
        {
        return *i1Certificate;
        }
    else
        {
        return KEmptyString;
        }
    }
    
    
EXPORT_C const TDesC8& CIkeV2PkiService::TrustedCaName() const
    {
    if ( i2CertificateName != NULL )
        {
        return *i2CertificateName;
        }
    if (iCaName != NULL)
        {
        return *iCaName;
        }
    else
        {
        return KEmptyString;
        }
    }               				


void CIkeV2PkiService::ReadUserCertificateL(const TDesC8& aTrustedAuthority, TBool aGetCACert)
    {
    __ASSERT_DEBUG(iReadCertificate != NULL, User::Invariant());
   //
   // Read certificate from PKI store using pkiserviceapi
   //  
	TPKIKeyAlgorithm keyAlgorithm = EPKIRSA; 
	TPKICertificateOwnerType ownerType; 	
	TUint keySize = 0;
	
	 if ( aGetCACert )
	    {
	        ownerType = EPKICACertificate;

	        //Init CA cert ident data.
	        //aTrustedAuthority (issuer) checking for CA certs is not supported.
	        //__ASSERT_ALWAYS(aTrustedAuthority.Length() == 0, User::Invariant());
	        if ( aTrustedAuthority.Length() == 0 )
	            {
	            delete iSubjName;
	            iSubjName = NULL;
	            iSubjName = iCaName->AllocL();
	            iRfc822Name->Des().Zero();
	            } 
	     }
	 else
	     {
	     ownerType = EPKIUserCertificate;	    
	     TInt length = iIkeData->iOwnCert.iSubjectDnSuffix.Length();
	     if ( length )
	         {
	         delete iSubjName;
	         iSubjName = NULL;
	         iSubjName = HBufC8::NewL(length);  	   			 
	         iSubjName->Des().Copy(iIkeData->iOwnCert.iSubjectDnSuffix);		
	         }
	     else 
	         {
	         iSubjName->Des().Zero();
	         } 

	     length = iIkeData->iOwnCert.iRfc822NameFqdn.Length();
	     if ( length )
	         {
	         delete iRfc822Name;
	         iRfc822Name = NULL;
	         iRfc822Name = HBufC8::NewL(length);  	   			 
	         iRfc822Name->Des().Copy(iIkeData->iOwnCert.iRfc822NameFqdn);        	 
	         }
	     else
	         {
	         iRfc822Name->Des().Zero();
	         }
	     keySize = iIkeData->iOwnCert.iPrivateKeyLength;
	     }
	iPkiService.ReadCertificateL(aTrustedAuthority,
	                              *iSubjName, *iRfc822Name,
			                      ownerType, keySize,
			                      keyAlgorithm, iCertPtr,
			                      &iResArray, iStatus);
    SET_ACTIVE;
    }	 


void CIkeV2PkiService::CIkeV2PkiServiceApplUidArrayCleanup(TAny* any)
    {
    RArray<TUid>* applUidList = reinterpret_cast<RArray<TUid>*>(any);
    applUidList->Reset();
    applUidList->Close();
    delete applUidList;    
    }


void CIkeV2PkiService::RunL()
    {   
    DEBUG_LOG1(_L("CIkeV2PkiService::RunL: Status %d"), iStatus.Int());
    
	//
	// A PKI service operation completed. Take actions according to
	// iOperation code
	//

    TInt err = KErrNone;
			
	TInt status = iStatus.Int();				

    iPkiService.Finalize(iResArray);
    iResArray = NULL;

	
	switch ( iState )
	    {
		case EBuildingCaList:
            TRAP(err, BuildingCaListRunL());
            break;				
		case EReadingCertificate:
		    TRAP(err, ReadUserCertificateRunL());
			break;
		case EReadingCertificateChain:
		    TRAP(err, ReadCertificateChainRunL());
		    break;
		default:
		    DEBUG_LOG(_L("RunL called in unknown state"));
		    User::Invariant();
			break;
	    }	

	if ( err != KErrNone )
	    {	
	    DEBUG_LOG(_L("Operation completed. Signalling observer."));

        SignalObserverL(err);
	    }   
    }


void CIkeV2PkiService::ReadUserCertificateRunL()
    {        
	//
	// A Certificate has been read PKI store.
	// Build X509 certificate object from certificate data
	//
	switch(iStatus.Int())
	    {
	    case KErrNone:	        
	        iUserCertificate = iReadCertificate->AllocL();
	        iReadCertificate->Des().Zero();
	        SignalObserverL(KErrNone);            
	        break;
	    case KPKIErrBufferTooShort:
		    {	
            //
            // Allocate a new buffer for ASN1 coded certificate read from PKI store
            // Buffer size is now asked from pkiserviceapi 
            //            
            TInt realCertSize;        
            User::LeaveIfError(iPkiService.GetRequiredBufferSize(realCertSize));

            delete iReadCertificate;
            iReadCertificate = NULL;

            iReadCertificate = HBufC8::NewL(realCertSize);
            iCertPtr.Set(iReadCertificate->Des());
            		   		   
            ReadUserCertificateL(*iCaName, EFalse);
		    }
	        break;
	    case KPKIErrNotFound:
	        {	            
            //
            // Get next user certificate from PKI store using either Key
            // identifier or CA name as read argument
            //                                    
            iCasTrustedByPeer.Remove(0);
            if ( iCasTrustedByPeer.Count() > 0 )
                {
                
                CIkeCaElem* CaElem = iCasTrustedByPeer[0];	                                   
                HBufC8* caName = IkeCert::GetCertificateFieldDERL(CaElem->Certificate(), KSubjectName);
                if (caName == NULL)
                    {
                    User::Leave(KErrArgument);
                    }
                delete iCaName;
                iCaName = caName;
                caName=NULL;
                delete caName;
                ReadUserCertificateL(*iCaName, EFalse);
                }	   
            else
                {
                User::Leave(KErrNotFound);
                }
	        }
            break;
	    case KErrNotFound:
	        ReadCertificateChainL();
	        break;
        default:
            User::Leave(iStatus.Int());
            break;            
	    }
    }    


void CIkeV2PkiService::BuildingCaListRunL()
    {       
    
    switch(iStatus.Int())
        {
        case KErrNone:
            {                        
            iIkeDataCAList->Delete(0);     
            
    	    ASSERT(iReadCertificate);
    		HBufC8* caCert = iReadCertificate; // Link CA buffer to CIkeCaElem
    		CleanupStack::PushL(caCert);		
    		
    		iReadCertificate = NULL;    		
    		iReadCertificate = HBufC8::NewL(KDefaultCertificateBufferSize);
    		iCertPtr.Set(iReadCertificate->Des());
    		
    		
     		CIkeCaElem* caElem = CIkeCaElem::NewL(caCert);
    		CleanupStack::Pop(caCert);		
    		CleanupStack::PushL(caElem);
    		
    		//Append ca cert to list, if not already present.
    		if (iTrustedCAList->FindCaElem(caElem->KeyHash()) == NULL)
    		    {    		    
    		    iTrustedCAList->AppendL(caElem);
    		    CleanupStack::Pop(caElem);
    		    }
            else
                {
                CleanupStack::PopAndDestroy(caElem);
                }
            
            if (iIkeDataCAList->Count() > 0)
                {
                ImportNextCaElemFromIkeDataListL();
                }
            else
                {
                
                if (iIkeData->iOwnCert.iOwnCertExists)
                    {
                    ReadTrustedUserCertificateL();
                    }
                else
                    {
                    SignalObserverL(KErrNone);
                    }
                }
            }
            break;
        case KPKIErrBufferTooShort:
            {
                
            DEBUG_LOG(_L("Buffer too short"));
            
            TInt certSize = 0;            
		    User::LeaveIfError(iPkiService.GetRequiredBufferSize(certSize));
            
            __ASSERT_DEBUG(iCertPtr.MaxLength() < certSize, User::Invariant());
            
            delete iReadCertificate;
            iReadCertificate = NULL;            
            iReadCertificate = HBufC8::NewL(certSize);
            iCertPtr.Set(iReadCertificate->Des());
            
            //Tries to reimport the certificate.
            ImportNextCaElemFromIkeDataListL();            
            }
            break;
        default:        
            DEBUG_LOG1(_L("Error code %d"), iStatus.Int());
            User::Leave(iStatus.Int());
            break;
        }
    }


EXPORT_C void CIkeV2PkiService::InitIkeV2PkiService(const CIkeData* aIkeData)
    {          
    __ASSERT_DEBUG(iState == EPkiServiceIdle, User::Invariant());
    __ASSERT_DEBUG(iIkeDataCAList == NULL, User::Invariant());
    __ASSERT_DEBUG(aIkeData->iCAList != NULL, User::Invariant());
    __ASSERT_DEBUG(aIkeData->iCAList->Count() > 0, User::Invariant());
    __ASSERT_DEBUG(iIkeData == NULL, User::Invariant());
    
    iIkeData = aIkeData;
    
    iState = EBuildingCaList;
    TRAPD(err, InitIkeV2PkiServiceL());
    if (err != KErrNone)
        {
        iStatus = KRequestPending;
        SET_ACTIVE;
        
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }     
    }
    

void CIkeV2PkiService::InitIkeV2PkiServiceL()
    {               
    if (iIkeData->iClientCertType != NULL)
        {
        if (iIkeData->iClientCertType->GetData().Compare(_L("DEVICE")) == 0)
            {
            User::LeaveIfError(iPkiService.SetStoreType(EPkiStoreTypeDevice));
            }
        else
            {
            User::LeaveIfError(iPkiService.SetStoreType(EPkiStoreTypeUser));
            }
        }
    
    iIkeDataCAList = new (ELeave) CArrayFixFlat<TCertInfo>(2);
    for (TInt i = 0; i < iIkeData->iCAList->Count(); ++i)
        {
        const TCertInfo* info = (*iIkeData->iCAList)[i];
        iIkeDataCAList->AppendL(*info);
        }       
         
    ImportNextCaElemFromIkeDataListL();         
    }
    
    
void CIkeV2PkiService::ImportNextCaElemFromIkeDataListL()
    {        
    __ASSERT_DEBUG(iIkeDataCAList != NULL, User::Invariant());
    __ASSERT_DEBUG(iIkeDataCAList->Count() > 0, User::Invariant());
    
    const TCertInfo certInfo = (*iIkeDataCAList)[0];        
    switch(certInfo.iFormat)
        {            
        case CA_NAME:    
            delete iSubjName;
            iSubjName = NULL;
            iSubjName = HBufC8::NewL(certInfo.iData.Length());
            iSubjName->Des().Copy(certInfo.iData);                    
        	iPkiService.ReadCertificateL(KEmptyString,
                                         *iSubjName, KEmptyString,
		                                 EPKICACertificate, 0,
		                                 EPKIRSA, iCertPtr,
		                                 &iResArray, iStatus);
            SET_ACTIVE;  
            break;                  
        case KEY_ID:
            if (!IkeParser::TextToHexOctets(certInfo.iData, iCertKeyId))
                {
                User::Leave(KErrArgument);
                }
            iPkiService.ReadCertificateL(iCertKeyId, iCertPtr,
            			                 &iResArray, iStatus);
            SET_ACTIVE;                     
            break;
       case APPL_UID:           
            {            
            //Get the list of applicable CA certs and appends it
            //to the original list, which was defined in the policy.
            //After this removes the currently handled node and
            //calls the method recursively.
            RArray<TUid>*  applUidList = IkeParser::GetApplUidListL(certInfo.iData);	
            CleanupStack::PushL(TCleanupItem(CIkeV2PkiServiceApplUidArrayCleanup,
                                 applUidList));

            CArrayFix<TCertificateListEntry>* applicableCaCertList;
            iPkiService.ListApplicableCertificatesL(*applUidList, applicableCaCertList);                                
            
            CleanupStack::PopAndDestroy(); //applUidList
                        
            if (applicableCaCertList->Count() > 0)
                {                                            
                CleanupStack::PushL(applicableCaCertList);
                TCertInfo* info = new (ELeave) TCertInfo;
                CleanupDeletePushL(info);
                for (TInt i = 0; i < applicableCaCertList->Count(); i++)
                    {
                    const TCertificateListEntry& entry = (*applicableCaCertList)[i];
                    info->iFormat = CA_NAME;
                    info->iData.Zero();
                    info->iData.Copy(entry.iIdentitySubjectName);

                    iIkeDataCAList->AppendL(*info);
                    DEBUG_LOG1(_L("Appending Applicable cert to the list (%S)"), &(info->iData));
                                                
                    }
                
                CleanupStack::PopAndDestroy(info);
                CleanupStack::PopAndDestroy(applicableCaCertList);
                
                iIkeDataCAList->Delete(0);                
                ImportNextCaElemFromIkeDataListL();
                }
            else
                {
                delete applicableCaCertList;
                applicableCaCertList = NULL;
                
                iStatus = KRequestPending;
                SET_ACTIVE;
                
                TRequestStatus* status = &iStatus;
                User::RequestComplete(status, KErrNotFound);                
                }                                                                
            }
            break;
        default:
            User::Leave(KErrArgument);                
            break;
        }
    }

void CIkeV2PkiService::ReadCertificateChainL()
    {
    delete iCaName;
    iCaName = NULL;
    iCaName = IkeCert::GetCertificateFieldDERL(iCasTrustedByPeer[0]->Certificate(), KSubjectName);;
    iState = EReadingCertificateChain;
    ReadUserCertificateL(KEmptyString, EFalse);
    }

void CIkeV2PkiService::ReadCertificateChainRunL()
    {
    TInt err;
    HBufC8* issuerName=NULL;
    TRAP(err, issuerName =  IkeCert::GetCertificateFieldDERL(iReadCertificate, KIssuerName));
    if (err!=KErrNone)
        {
        err=KKmdIkeNoCertFoundErr;
        User::Leave(err);
        }
    if ( issuerName->Compare(iCaName->Des())==0)
        {
        iReadCertificate->Des().Zero();
        delete issuerName;
        issuerName = NULL;
        SignalObserverL(KErrNone);    
        }
    else
        {
         delete issuerName;
         issuerName = NULL;
         delete iCaName;
         iCaName = NULL;
         iCaName =  IkeCert::GetCertificateFieldDERL(iReadCertificate, KIssuerName);
         delete iSubjName;
         iSubjName = NULL;
         iSubjName = iCaName->AllocL();
         if ( !iUserCertificate)
             iUserCertificate = iReadCertificate->AllocL();
         else if ( !i2Certificate )
             {
             i2Certificate = iReadCertificate->AllocL();
             i2CertificateName= IkeCert::GetCertificateFieldDERL(i2Certificate, KSubjectName);
             }
         else if ( !i1Certificate)
             i1Certificate = iReadCertificate->AllocL();
                            
         iPkiService.ReadCertificateL(KEmptyString,
                                      *iSubjName, KEmptyString,
                                      EPKICACertificate, 0,
                                      EPKIRSA, iCertPtr,
                                      &iResArray, iStatus);
         SET_ACTIVE;
        }
        
    }
    
void CIkeV2PkiService::SignalObserverL(TInt aStatus)
{    
    DEBUG_LOG1(_L("CIkeV2PkiService::SignalObserverL: Signalling with %d"), aStatus);
    
    if (aStatus != KErrNone)
        {
        delete iUserCertificate;
        iUserCertificate = NULL;
        
		delete iCaName;
		iCaName = NULL;
				
		iCertPtr.Zero();
        
        iTrustedCAList->ResetAndDestroy(); // Trusted CA certificate list 
               
        }

    iIkeData = NULL;

    iSubjName->Des().Zero();
    iRfc822Name->Des().Zero();

    iCertKeyId.Zero();
    iResArray = NULL;
    
    iCasTrustedByPeer.Reset();
    	    	    	    
    delete iIkeDataCAList;
    iIkeDataCAList = NULL;
    	    	       
    iState = EPkiServiceIdle;
    iObserver.IkeV2PkiInitCompleteL(aStatus);		
}
