/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI Service API
*
*/



#include <e32std.h>
#include <e32math.h>

#include "pkiserviceapi.h"
#include "clistatic.h"
#include "pkcs10.h"
#include "pkiserviceclientservercommon.h"
#include "pkisession.h"

#include <certificateapps.h>


/**---------------------------------------------------------
 *
 * RPKIServiceAPI class constructor
 *
 *----------------------------------------------------------*/

EXPORT_C RPKIServiceAPI::RPKIServiceAPI()
    {
    }
    
EXPORT_C TInt RPKIServiceAPI::Connect()
//
// Connect to the server attempting to start it if necessary
//
    {
    TInt retry=2;
    for (;;)
        {
        TInt r=CreateSession(KPkiServerName,
                             TVersion(KPkiMajorVersion,
                                      KPkiMinorVersion,
                                      KPkiBuildVersion),
                             KDefaultMessageSlots);

        if (r!=KErrNotFound && r!=KErrServerTerminated)
            {
            if (r == KErrNone)
                {
                r = SendReceive(PkiService::EInitialize,TIpcArgs(NULL));
                if (r != KErrNone)
                    {
                    RSessionBase::Close();
                    }
                }
            return r;
            }
        if (--retry==0)
            return r;
        r = Launcher::LaunchServer(KPkiServerName, KPkiServerImg,
                                   KPkiServiceUid3, KMyServerInitHeapSize,
                                   KMyServerMaxHeapSize, KMyServerStackSize);

        if (r!=KErrNone && r!=KErrAlreadyExists)
            return r;
        }
    }

/**---------------------------------------------------------
 *
 * Logon(TRequestStatus& aRequestStatus)
 *
 * 
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::Logon(TRequestStatus& aRequestStatus)
    {
    SendReceive(PkiService::ELogon,TIpcArgs(NULL), aRequestStatus);
    }

/**----------------------------------------------------------
* Lock keystore
* PIN code must be given again to unlock the keystore
*
* @param aRequestStatus [out] A reference to the request status object. On
* request completion, contains the return code of the request.
*
*-------------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::Logoff(TRequestStatus& aRequestStatus)
	{
	SendReceive(PkiService::ELogoff,TIpcArgs(NULL), aRequestStatus);
	}


    
/**---------------------------------------------------------
 *
 * ChangePassword(TRequestStatus& aRequestStatus)
 *
 * 
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ChangePassword(TRequestStatus& aRequestStatus)
    {
    SendReceive(PkiService::EChangePassword,TIpcArgs(NULL), aRequestStatus);
    }

/**---------------------------------------------------------
 *
 * SignL(const TDesC8& aKeyId, const TDesC8& aHashIn, TDes8& aSignature,
 *       TAny **aResourceObject, TRequestStatus& aRequestStatus)
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::Sign(const TPKIKeyIdentifier& aKeyId,
                                   const TDesC8& aHashIn,
                                   TDes8& aSignature) const
    {
    return SendReceive(PkiService::ESignWithKeyId, TIpcArgs(&aKeyId, &aHashIn, &aSignature));        
    }

/**---------------------------------------------------------
 *
 * SignL(const TDesC8& aTrustedAuthority, const TPKICertificateOwnerType aOwnerType, const TPKIKeyUsage aKeyUsage,
 *      const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm, const TDesC8& aHashIn, TDes8& aSignature, 
 *      TAny **aResourceObject, TRequestStatus& aRequestStatus)
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::Sign(const TDesC8& aTrustedAuthority,
                                   const TDesC8& aIdentitySubjectName,
                                   const TDesC8& aIdentityRfc822Name,
                                   const TPKIKeyUsage aKeyUsage,
                                   const TUint aKeySize,
                                   const TPKIKeyAlgorithm aKeyAlgorithm,
                                   const TDesC8& aHashIn,
                                   TDes8& aSignature) const    
    {
    TInt err = KErrNone;
    
    TSecurityObjectDescriptor *privateKeyDesc = new TSecurityObjectDescriptor;
    if (privateKeyDesc != NULL)
        {        
        privateKeyDesc->SetTrustedAuthority(aTrustedAuthority);
        privateKeyDesc->SetIdentitySubjectName(aIdentitySubjectName);
        privateKeyDesc->SetIdentityRfc822Name(aIdentityRfc822Name);
        privateKeyDesc->SetOwnerType(EPKIUserCertificate);  // Always user cert
        privateKeyDesc->SetKeyUsage(aKeyUsage);
        privateKeyDesc->SetKeySize(aKeySize);
        privateKeyDesc->SetKeyAlgorithm(aKeyAlgorithm);
        
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*privateKeyDesc);

        //Send request to the server
        err = SendReceive (PkiService::ESignWithCert, TIpcArgs(&pckgTSecurityObjectDescriptor, &aHashIn, &aSignature));

        delete privateKeyDesc;
        privateKeyDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;        
    }


/**---------------------------------------------------------
 *
 * TInt DecryptL(const TDesC8& aKeyId, const TDesC8& aDataIn, TDes8& aDataOut, 
 *      TAny **aResourceObject, TRequestStatus& aRequestStatus)
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::Decrypt(const TPKIKeyIdentifier& aKeyId,
                                      const TDesC8& aDataIn,
                                      TDes8& aDataOut) const
    {
    return SendReceive (PkiService::EDecrypt, TIpcArgs(&aKeyId, &aDataIn, &aDataOut));            
    }

/**---------------------------------------------------------
 *
 * StoreKeypairL(TDesC8& aKeyId, const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm,
 *               const TDesC8& aKeyDataIn, TBool aEncrypted, TRequestStatus& aRequestStatus)
 * 
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::StoreKeypair(TPKIKeyIdentifier& aKeyId,
                                           const TDesC8& aKeyDataIn,
                                           TRequestStatus& aRequestStatus)
    {
   
    SendReceive (PkiService::EStoreKeypair, 
                 TIpcArgs(&aKeyId, &aKeyDataIn), 
                 aRequestStatus);
    }

/**---------------------------------------------------------
 *
 * GenerateKeypairL(TDesC8& aKeyId, const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm, 
 *                  TRequestStatus& requestStatus)
 * 
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::GenerateKeypair(TPKIKeyIdentifier& aKeyId,
                                               const TUint aKeySize,
                                               const TPKIKeyAlgorithm aKeyAlgorithm,
                                               TRequestStatus& aRequestStatus)
    {
    
    __ASSERT_DEBUG(aKeyAlgorithm == EPKIRSA || aKeyAlgorithm == EPKIDSA, User::Invariant());
    
    SendReceive (PkiService::EGenerateKeypair, 
                 TIpcArgs(&aKeyId, aKeySize, static_cast<TUint>(aKeyAlgorithm)), 
                 aRequestStatus);   
    }


/**---------------------------------------------------------
 *
 * CancelPendingOperation() 
 * 
 *
 * Returns: 
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::CancelPendingOperation()
    {
    //Send request to the server
    return SendReceive (PkiService::ECancelPendingOperation, TIpcArgs(NULL));            
    }

/**---------------------------------------------------------
 *
 * GetRequiredBufferSize 
 * 
 *
 * Returns: 
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::GetRequiredBufferSize(TInt &aSize)
    {
    TInt size;
    TPckg<TInt> pckgSize(size);
    TInt ret = SendReceive (PkiService::EGetRequiredBufferSize, TIpcArgs(&pckgSize));
    aSize = size;
    return ret;
    }

    
/**---------------------------------------------------------
 *
 * ReadPublicKeyL(const TDesC8& aKeyId, TDes8& aDataOut)
 *
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::ReadPublicKey(const TPKIKeyIdentifier& aKeyId,
                                            TDes8& aDataOut) const
    {        
    return  SendReceive (PkiService::EReadPublicKey, 
                         TIpcArgs(&aKeyId, &aDataOut));                                 
    }

    
/**---------------------------------------------------------
 *
 * ReadCertificateL(const TDesC8& aTrustedAuthority, const TPKICertificateOwnerType aOwnerType, const TPKIKeyUsage aKeyUsage,
 *                   const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm, HBufC8 *&aCert)
 * 
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ReadCertificateL(const TDesC8& aTrustedAuthority,
                                               const TDesC8& aIdentitySubjectName,
                                               const TDesC8& aIdentityRfc822Name,
                                               const TPKICertificateOwnerType aOwnerType,
                                               const TUint aKeySize,
                                               const TPKIKeyAlgorithm aKeyAlgorithm,
                                               TDes8 &aCert,
                                               TAny **aResourceObject,
                                               TRequestStatus& aRequestStatus)
    {
    TPckgBuf<TSecurityObjectDescriptor> *pckgTSecurityObjectDescriptor = new (ELeave) TPckgBuf<TSecurityObjectDescriptor>();
    CleanupStack::PushL(pckgTSecurityObjectDescriptor);
    TSecurityObjectDescriptor& certDesc = (*pckgTSecurityObjectDescriptor)();
    
    if (aTrustedAuthority.Length() > 0)
        {
        certDesc.SetTrustedAuthority(aTrustedAuthority);    
        }
    
    if (aIdentitySubjectName.Length() > 0)
        {
        certDesc.SetIdentitySubjectName(aIdentitySubjectName);
        }
    
    if (aIdentityRfc822Name.Length() > 0)
        {
        certDesc.SetIdentityRfc822Name(aIdentityRfc822Name);
        }
    
    if (aKeySize != 0)
        {
        certDesc.SetKeySize(aKeySize);
        }
            
    if(aOwnerType != EPKICACertificate)
        {
        certDesc.SetKeyUsage(EX509DigitalSignature);                        
        certDesc.SetKeyAlgorithm(aKeyAlgorithm);
        }
                
    certDesc.SetOwnerType(aOwnerType);
        
    //Send request to the server
    SendReceive (PkiService::EReadCertificate, TIpcArgs(pckgTSecurityObjectDescriptor, &aCert), aRequestStatus);            
    CleanupStack::Pop();
    
    *aResourceObject = pckgTSecurityObjectDescriptor;
    }

/**---------------------------------------------------------
 *
 * ReadCertificateL
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::ReadCertificate(const TDesC8& aTrustedAuthority,
                                               const TDesC8& aSerialNumber,
                                               TDes8 &aCert)
{
    TInt err = KErrNone;
   
    TSecurityObjectDescriptor *certDesc = new TSecurityObjectDescriptor;
    if (certDesc != NULL)
        {        
        certDesc->SetTrustedAuthority(aTrustedAuthority);
        certDesc->SetSerialNumber(aSerialNumber);
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);

        //Send request to the server
        err = SendReceive (PkiService::EReadCertificate, TIpcArgs(&pckgTSecurityObjectDescriptor, 
                                                                   &aCert));                   
        
        delete certDesc;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
}


/**--------------------------------------------------------------------------
* Read a certificate having listed characteristics
* @param aKeyId SubjectKeyID.
* @param aCert [out] Returned ASN1 encoded certificate.
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
* @param aRequestStatus [out] A reference to the request status object. On
* request completion, contains the return code of the request.
*---------------------------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ReadCertificateL(const TPKIKeyIdentifier& aKeyId,
                                                TDes8 &aCert,
                                                TAny **aResourceObject,
                                                TRequestStatus& aRequestStatus)
{
    TPckgBuf<TSecurityObjectDescriptor> *pckgTSecurityObjectDescriptor = new (ELeave) TPckgBuf<TSecurityObjectDescriptor>();
    CleanupStack::PushL(pckgTSecurityObjectDescriptor);
    TSecurityObjectDescriptor& certDesc = (*pckgTSecurityObjectDescriptor)();

    certDesc.SetSubjectKeyId(aKeyId);
    
    //Send request to the server
    SendReceive (PkiService::EReadCertificate, TIpcArgs(pckgTSecurityObjectDescriptor, &aCert), aRequestStatus);            
    CleanupStack::Pop();
    
    *aResourceObject = pckgTSecurityObjectDescriptor;
}


/**---------------------------------------------------------
 *
 * ListCertificatesL
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ListCertificatesL(CArrayFix<TCertificateListEntry> *&aCertList)
{
	TInt certCount = SendReceive(PkiService::ECertCount, TIpcArgs(NULL));

	TInt certListGranularity = certCount;
	if (certListGranularity == 0)
		{
		certListGranularity = 1;
		}

	CArrayFix<TCertificateListEntry>* certList;	
	certList = new (ELeave) CArrayFixFlat<TCertificateListEntry>(certListGranularity);
	if (certCount == 0)
		{
		aCertList = certList;
		return;
		}

	CleanupStack::PushL(certList);

	CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateListEntry));
	CleanupStack::PushL(list);
	list->ResizeL(certCount * sizeof(TCertificateListEntry));

	TPtr8 ptrList = list->Ptr(0);

	User::LeaveIfError(SendReceive (PkiService::EGetCertList, TIpcArgs(&ptrList)));

	TCertificateListEntry certInfo;
	for (TInt i = 0; i < certCount; i++)
		{
		list->Read(i * sizeof(TCertificateListEntry), (TAny*)&certInfo, sizeof(TCertificateListEntry));
		certList->AppendL(certInfo);
		}

	CleanupStack::PopAndDestroy(1);     // list
	CleanupStack::Pop();                // certList

	aCertList = certList;
}

/**---------------------------------------------------------
* List CA certificates applicable for all requested applications  
* @param aApplications List of applications
* @param aCertList [out] Returned list of certificates.
* @param aRequestStatus [out] A reference to the request status object. On
* request completion, contains the return code of the request.
*---------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ListApplicableCertificatesL(const RArray<TUid>& aApplications,
										  CArrayFix<TCertificateListEntry>*& aCertList)
{

	TPckgC<TInt> applCount(aApplications.Count());
	// Allocate a buffer for the application uid list
	CBufFlat* applList = CBufFlat::NewL(sizeof(TUid));
	CleanupStack::PushL(applList);
	applList->ResizeL(aApplications.Count() * sizeof(TUid));

	TInt i = 0;
	for(i=0;i<aApplications.Count();i++)
		{
		applList->Write(i * sizeof(TUid), (TAny*)&(aApplications[i]), sizeof(TUid));
		}

	// Application Uid list
	TPtr8 ptrList = applList->Ptr(0);

	TInt certCount = SendReceive(PkiService::EApplicableCertCount, TIpcArgs(&applCount, &ptrList));

	CleanupStack::PopAndDestroy(1);     // applList
	TInt certListGranularity = certCount;
	if (certListGranularity == 0)
		{
		certListGranularity = 1;
		}

	CArrayFix<TCertificateListEntry>* certList;	
	certList = new (ELeave) CArrayFixFlat<TCertificateListEntry>(certListGranularity);
	if (certCount == 0)
		{
		aCertList = certList;
		return;
		}

	CleanupStack::PushL(certList);

	CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateListEntry));
	CleanupStack::PushL(list);
	list->ResizeL(certCount * sizeof(TCertificateListEntry));

	TPtr8 ptrList2 = list->Ptr(0);

	User::LeaveIfError(SendReceive (PkiService::EGetApplicableCertList, TIpcArgs(&ptrList2)));

	TCertificateListEntry certInfo;
	for (i = 0; i < certCount; i++)
		{
		list->Read(i * sizeof(TCertificateListEntry), (TAny*)&certInfo, sizeof(TCertificateListEntry));
		certList->AppendL(certInfo);
		}

	CleanupStack::PopAndDestroy(1);     // list
	CleanupStack::Pop();                // certList

	aCertList = certList;
}



/**---------------------------------------------------------
 *
 * ListKeysL
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ListKeysL(CArrayFix<TKeyListEntry> *&aKeyList)
{
    TInt keyCount = SendReceive(PkiService::EKeyCount, TIpcArgs(NULL));
    User::LeaveIfError(keyCount);

    TInt keyListGranularity = keyCount;
    if (keyListGranularity == 0)
        {
        keyListGranularity = 1;
        }

    CArrayFix<TKeyListEntry>* keyList;	
    keyList = new (ELeave) CArrayFixFlat<TKeyListEntry>(keyListGranularity);
    if (keyCount == 0)
        {
        aKeyList = keyList;
        return;
        }

    CleanupStack::PushL(keyList);

    CBufFlat* list = CBufFlat::NewL(sizeof(TKeyListEntry));
    CleanupStack::PushL(list);
    list->ResizeL(keyCount * sizeof(TKeyListEntry));

    TPtr8 ptrList = list->Ptr(0);

    User::LeaveIfError(SendReceive (PkiService::EGetKeyList, TIpcArgs(&ptrList)));

    TKeyListEntry keyInfo;
    for (TInt i = 0; i < keyCount; i++)
        {
        list->Read(i * sizeof(TKeyListEntry), (TAny*)&keyInfo, sizeof(TKeyListEntry));
        keyList->AppendL(keyInfo);
        }

    CleanupStack::PopAndDestroy(1);     // list
    CleanupStack::Pop();                // keyList

    aKeyList = keyList;
}


/**---------------------------------------------------------
 *
 * StoreCertificateL(const TPKICertificateOwnerType aOwnerType, 
 *                   const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm, const TDesC8& aDataIn)
 * 
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::StoreCertificateL(const TPKICertificateOwnerType aOwnerType,
                                                const TUint aKeySize,
                                                const TPKIKeyAlgorithm aKeyAlgorithm,
                                                const TDesC8& aDataIn,
                                                TAny **aResourceObject,
                                                TRequestStatus& aRequestStatus)
    {
    __ASSERT_DEBUG(aDataIn.Length() > 0, User::Invariant());
    
    TPckgBuf<TSecurityObjectDescriptor> *pckgTSecurityObjectDescriptor = new (ELeave) TPckgBuf<TSecurityObjectDescriptor>();
    CleanupStack::PushL(pckgTSecurityObjectDescriptor);
    TSecurityObjectDescriptor& certDesc = (*pckgTSecurityObjectDescriptor)();
        
    certDesc.SetOwnerType(aOwnerType);
    certDesc.SetKeySize(aKeySize);
    certDesc.SetKeyAlgorithm(aKeyAlgorithm);
    certDesc.SetIsDeletable(ETrue);

    //Send request to the server
    SendReceive (PkiService::EStoreCertificate, TIpcArgs(pckgTSecurityObjectDescriptor, &aDataIn), aRequestStatus);            
    CleanupStack::Pop();
    
    *aResourceObject = pckgTSecurityObjectDescriptor;
    }


/**------------------------------------------------------------------------
* Store a certificate having listed characteristics
* For CA certificates, only aOwnerType and aDataIn parameters are meaningful.
*
* @param aOwnerType Type of the certificate.
* @param aLabel Label of the key
* @param aCertFormat certificate format
* @param aIsDeletable true if certificate is deletable
* @param aKeySize Size of the key in bits.
* @param aKeyAlgorithm Algorithm of the key.
* @param aDataIn A descriptor of the buffer conatining ASN1 coded certificate bytes.
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
* @param aRequestStatus [out] A reference to the request status object. On
* request completion, contains the return code of the request.
*----------------------------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::StoreCertificate(const TPKICertificateOwnerType aOwnerType,
									           const TBool& aIsDeletable,
                                               const TUint aKeySize,
                                               const TPKIKeyAlgorithm aKeyAlgorithm,
                                               const TDesC8& aDataIn) const
    {  
    __ASSERT_DEBUG(aDataIn.Length() > 0, User::Invariant());
         
    TInt err = KErrNone;   
    TSecurityObjectDescriptor *certDesc = new TSecurityObjectDescriptor;
    if (certDesc != NULL)
        {        
        certDesc->SetOwnerType(aOwnerType);
        certDesc->SetKeySize(aKeySize);
        certDesc->SetKeyAlgorithm(aKeyAlgorithm);
    	certDesc->SetIsDeletable(aIsDeletable);
    	
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);

        //Send request to the server
        err = SendReceive (PkiService::EStoreCertificate, 
                           TIpcArgs(&pckgTSecurityObjectDescriptor, &aDataIn));            
        
        delete certDesc;
        certDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
        
    return err;        
    }


    
/**---------------------------------------------------------
 *
 * AttachCertificateL(const TDesC8& aKeyId, 
 *                   const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm, const TDesC8& aDataIn)
 * 
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::AttachCertificateL(const TPKIKeyIdentifier& aKeyId,
                                                 const TUint aKeySize,
                                                 const TPKIKeyAlgorithm aKeyAlgorithm,
                                                 const TDesC8& aDataIn,
                                                 TAny **aResourceObject,
                                                 TRequestStatus& aRequestStatus)
    {
    
    __ASSERT_DEBUG(aDataIn.Length() > 0, User::Invariant());
    
    TPckgBuf<TSecurityObjectDescriptor> *pckgTSecurityObjectDescriptor = new (ELeave) TPckgBuf<TSecurityObjectDescriptor>();
    CleanupStack::PushL(pckgTSecurityObjectDescriptor);
    TSecurityObjectDescriptor& certDesc = (*pckgTSecurityObjectDescriptor)();
        
    certDesc.SetSubjectKeyId(aKeyId);
    certDesc.SetOwnerType(EPKIUserCertificate);
    certDesc.SetKeySize(aKeySize);
    certDesc.SetKeyAlgorithm(aKeyAlgorithm);

    //Send request to the server
    SendReceive (PkiService::EAttachCertificate, TIpcArgs(pckgTSecurityObjectDescriptor, &aDataIn), aRequestStatus);            
    CleanupStack::Pop();
    
    *aResourceObject = pckgTSecurityObjectDescriptor; 
    }

/**--------------------------------------------------------------------------------
* Attach a user certificate having listed characteristics to a 
*   generated key identified by keyId
*
* @param aKeyId Idenfies the key to attach the certificate
* @param aIsDeletable true if certificate is deletable
* @param aKeySize Size of the key in bits.
* @param aKeyAlgorithm Algorithm of the key.
* @param aDataIn A descriptor of the buffer containing ASN1 
*   coded certificate bytes.
* @return the return code of the request.
*---------------------------------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::AttachCertificate(const TPKIKeyIdentifier& aKeyId,
									            const TBool& aIsDeletable,
                                                const TUint aKeySize,
                                                const TPKIKeyAlgorithm aKeyAlgorithm,
                                                const TDesC8& aDataIn) const
    {  
    
    __ASSERT_DEBUG(aDataIn.Length() > 0, User::Invariant());
      
    TInt err(KErrNone);
    TSecurityObjectDescriptor* certDesc = new TSecurityObjectDescriptor;    
    if (certDesc != NULL)
        {        
        certDesc->SetSubjectKeyId(aKeyId);
        certDesc->SetOwnerType(EPKIUserCertificate);
        certDesc->SetKeySize(aKeySize);
        certDesc->SetKeyAlgorithm(aKeyAlgorithm);
        certDesc->SetIsDeletable(aIsDeletable);

        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);

        //Send request to the server
        err = SendReceive (PkiService::EAttachCertificate, TIpcArgs(&pckgTSecurityObjectDescriptor, &aDataIn));            
        
        delete certDesc;
        certDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
    
    return err;
    }



    
/**---------------------------------------------------------
 *
 * RemoveKeypairL(const TDesC8& aKeyId)
 *
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::RemoveKeypair(const TPKIKeyIdentifier& aKeyId) const
    {
    return SendReceive(PkiService::ERemoveKeypair, TIpcArgs(&aKeyId));           
    }


/**---------------------------------------------------------
 *
 * RemoveCertificateL(const TDesC8& aTrustedAuthority, const TPKICertificateOwnerType aOwnerType, const TPKIKeyUsage aKeyUsage, 
 *                  const TUint aKeySize, const TPKIKeyAlgorithm aKeyAlgorithm)
 * 
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::RemoveCertificate(const TDesC8& aTrustedAuthority,
                                                const TDesC8& aSerialNumber) const
    {
    TInt err = KErrNone;
    
    TSecurityObjectDescriptor *certDesc = new TSecurityObjectDescriptor;
    if (certDesc != NULL)
        {        
        certDesc->SetTrustedAuthority(aTrustedAuthority);
        certDesc->SetSerialNumber(aSerialNumber);
        
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);

        //Send request to the server
        err = SendReceive (PkiService::ERemoveCertificate, TIpcArgs(&pckgTSecurityObjectDescriptor));            
        
        delete certDesc;
        certDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;        
}

/**---------------------------------------------------------
 *
 * Pkcs10SignCallbackL
 *
 *----------------------------------------------------------*/
TBool RPKIServiceAPI::Pkcs10SignCallbackL(const TDesC8& aDigest, TDes8& aSignature, 
                                          const TPKIKeyIdentifier& aKeyId, 
                                          TAny* aContext)
{    
    RPKIServiceAPI *pkiServiceApi = STATIC_CAST(RPKIServiceAPI*, aContext);    
    User::LeaveIfError(pkiServiceApi->Sign(aKeyId, aDigest, aSignature));       
    return ETrue;
}


/**---------------------------------------------------------
 *
 * CreateAndSaveCertificateRequestL
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::CreateAndSaveCertificateRequestL(const TPKIKeyIdentifier& aKeyId,
                                                                const TDesC8& aSubjectName,
                                                                const TDesC8& aSubjectAltNameRfc822,       
                                                                const TDesC8& aChallengePw,       
                                                                const TDesC8& aDNSName,
                                                                TDes& aCertRequestRef,
                                                                TInt& aRequestLength)
{
    TBuf8<2> testPublicKey;
    HBufC8 *publicKeyBuffer = NULL;
    
    TInt bSize = 0;
    
    // Get public key
    TInt err = ReadPublicKey(aKeyId, testPublicKey);   
    if(err == KPKIErrBufferTooShort)
    {
        GetRequiredBufferSize(bSize);
    }
    else if(err == KErrNone)
    {
        return;
    }
    else
    {
        User::Leave(err);
    }
    
    publicKeyBuffer = HBufC8::NewLC(bSize);
    TPtr8 publicKeyBufferDes = publicKeyBuffer->Des();
    err = ReadPublicKey(aKeyId, publicKeyBufferDes);    
    User::LeaveIfError(err);
    
    CPkcs10Req *pkcs10Instance = new (ELeave) CPkcs10Req;
    CleanupStack::PushL(pkcs10Instance);

    if (pkcs10Instance->SetDistinguishedNameExtendedL(aSubjectName) != KErrNone)
    {
        User::Leave(KPKIErrCertRequestParam);
    }
    if (pkcs10Instance->SetSubjectAltNameRfc822(aSubjectAltNameRfc822) != KErrNone)
    {
        User::Leave(KPKIErrCertRequestParam);
    }
    if (pkcs10Instance->SetChallengePw(aChallengePw) != KErrNone)
    {
        User::Leave(KPKIErrCertRequestParam);
    }
    if (pkcs10Instance->SetDNSName(aDNSName) != KErrNone)
    {
        User::Leave(KPKIErrCertRequestParam);
    }
    if (pkcs10Instance->SetPublicKey(publicKeyBufferDes) != KErrNone)
    {
        User::Leave(KPKIErrCertRequestParam);
    }

    TSignCallback signCallBackL = Pkcs10SignCallbackL;
    HBufC8* certRequest = pkcs10Instance->CreateCertificateRequestLC(
        HASH_TYPE_MD5, aKeyId, ETrue, EFalse, signCallBackL, this);
    if (!certRequest)
    {
        CleanupStack::PopAndDestroy(2);    // publicKeyBuffer, pkcs10Instance
        User::Leave(KPKIErrCertRequest);
    }
    //Send request to the server
    
    TRequestStatus requestStatus;
    SendReceive (PkiService::ESaveCertificateRequest, TIpcArgs(certRequest, &aCertRequestRef, &aKeyId), requestStatus);
    User::WaitForRequest(requestStatus);
    aRequestLength = certRequest->Length();
    CleanupStack::PopAndDestroy(3);    // certRequesr, publicKeyBuffer, pkcs10Instance
    User::LeaveIfError(requestStatus.Int());
}


/**---------------------------------------------------------
 *
 * ReadCertificateRequest
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::ReadCertificateRequest(const TDesC& aCertRequestRef,
                                                    TDes8& aCertRequest) const
{
    //Send request to the server
    return SendReceive (PkiService::EReadCertificateRequest, TIpcArgs(&aCertRequestRef, &aCertRequest));
}

/**---------------------------------------------------------
 *
 * DeleteCertificateRequest
 *
 *----------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::DeleteCertificateRequest(const TDesC& aCertRequestRef) const
{
    //Send request to the server
    return SendReceive (PkiService::EDeleteCertificateRequest, TIpcArgs(&aCertRequestRef));
}

/**---------------------------------------------------------
 *
 * Finalize
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::Finalize(TAny *aResourceObject)
    {
    if(aResourceObject != NULL)
        {
        delete static_cast<TPckgBuf<TSecurityObjectDescriptor>*>(aResourceObject);
        }
    }


/**-----------------------------------------------------------------------
* Change trust setting of a certificate in Symbian certificate store
*
* @param aTrustedAuthority Text or ASN1 format subtree name of the CA
* @param aSerialNumber serial number of the certificate
* @param aTrusted ETrue, if trusted; EFalse, otherwise
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
* @param aRequestStatus [out] A reference to the request status object. On
* request completion, contains the return code of the request.
*------------------------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::SetTrust(const TDesC8& aTrustedAuthority,
                                       const TDesC8& aSerialNumber,
                                       const TBool& aTrusted) const
{
    TInt err = KErrNone;
    
    TSecurityObjectDescriptor *certDesc = new TSecurityObjectDescriptor;
    if (certDesc != NULL)
        {        
        certDesc->SetTrustedAuthority(aTrustedAuthority);
        certDesc->SetSerialNumber(aSerialNumber);
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);

        TPckgC<TBool> trust(aTrusted);
        //Send request to the server
        err = SendReceive (PkiService::ESetTrust, TIpcArgs(&pckgTSecurityObjectDescriptor, &trust));            
        
        delete certDesc;
        certDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;                
}


/**---------------------------------------------------------------------------
* Read trust setting of a certificate in Symbian certificate store
*
* @param aTrustedAuthority Text or ASN1 format subtree name of the CA
* @param aSerialNumber serial number of the certificate
* @param aTrusted returns ETrue, if trusted; EFalse, otherwise
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
*---------------------------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::Trusted(const TDesC8& aTrustedAuthority,
                                      const TDesC8& aSerialNumber,
                                      TBool& aTrusted) const
{
    TInt err = KErrNone;
    TSecurityObjectDescriptor *certDesc = new TSecurityObjectDescriptor;
    if (certDesc != NULL)
        {
        certDesc->SetTrustedAuthority(aTrustedAuthority);
        certDesc->SetSerialNumber(aSerialNumber);
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);
        TPckg<TBool> trust(aTrusted);
        
        //Send request to the server
        err = SendReceive (PkiService::ETrusted, TIpcArgs(&pckgTSecurityObjectDescriptor, &trust)); 
        
        delete certDesc;
        certDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;        
}


/**---------------------------------------------------------------------------
* Set applications of a certificate in Symbian certificate store
*
* @param aTrustedAuthority Text or ASN1 format subtree name of the CA
* @param aSerialNumber serial number of the certificate
* @param aApplications list of applications (uids) for whom the certificate is applicable
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
*---------------------------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::SetApplicabilityL(const TDesC8& aTrustedAuthority,
                                                const TDesC8& aSerialNumber,
                                                const RArray<TUid>& aApplications) const
{
    TSecurityObjectDescriptor *certDesc = new (ELeave) TSecurityObjectDescriptor;
    CleanupStack::PushL(certDesc);
    certDesc->SetTrustedAuthority(aTrustedAuthority);
    certDesc->SetSerialNumber(aSerialNumber);
    TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);


    TPckgC<TInt> applCount(aApplications.Count());
    // Allocate a buffer for the application uid list
    CBufFlat* applList = CBufFlat::NewL(sizeof(TUid));
    CleanupStack::PushL(applList);
    applList->ResizeL(aApplications.Count() * sizeof(TUid));
    
    TInt i = 0;
    for(i=0;i<aApplications.Count();i++)
        {
        applList->Write(i * sizeof(TUid), (TAny*)&(aApplications[i]), sizeof(TUid));
        }

    // Application Uid list
	TPtr8 ptrList = applList->Ptr(0);

    //Send request to the server
    User::LeaveIfError(SendReceive (PkiService::ESetApplicability, TIpcArgs(&pckgTSecurityObjectDescriptor, &applCount, &ptrList)));            
	CleanupStack::PopAndDestroy(applList);  
    CleanupStack::PopAndDestroy(); //certDesc
}


/**---------------------------------------------------------------------------
* Get applications of a certificate in Symbian certificate store
*
* @param aTrustedAuthority Text or ASN1 format subtree name of the CA
* @param aSerialNumber serial number of the certificate
* @param aApplications list of applications (uids) for whom the certificate is applicable
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
*---------------------------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ApplicationsL(const TDesC8& aTrustedAuthority,
                                            const TDesC8& aSerialNumber,
                                            RArray<TUid>& aApplications) const
{
    TSecurityObjectDescriptor *certDesc = new (ELeave) TSecurityObjectDescriptor;
    CleanupStack::PushL(certDesc);
    certDesc->SetTrustedAuthority(aTrustedAuthority);
    certDesc->SetSerialNumber(aSerialNumber);
    TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);

    TInt count = 0;
    TPckg<TInt> pckgCount(count);
    // Allocate a buffer for the application uid list
    CBufFlat* applList = CBufFlat::NewL(sizeof(TUid));
    CleanupStack::PushL(applList);
    // Reserve space for applications
    CCertificateAppInfoManager* appsMan  = CCertificateAppInfoManager::NewLC();
    const RArray<TCertificateAppInfo>& apps = appsMan->Applications();
    applList->ResizeL(apps.Count() * sizeof(TUid));
    count = apps.Count();
    CleanupStack::PopAndDestroy(1);  // appsMan
  
    // Application Uid list
	TPtr8 ptrList = applList->Ptr(0);

    //Send request to the server
    User::LeaveIfError(SendReceive (PkiService::EApplications, TIpcArgs(&pckgTSecurityObjectDescriptor, &pckgCount, &ptrList)));

    TUid temp;
    for (TInt i = 0; i < count; i++)
        {
        applList->Read(i * sizeof(TUid), (TAny*)&temp, sizeof(TUid));
        aApplications.Append(temp);
        }
	CleanupStack::PopAndDestroy(1);  // applList
    CleanupStack::PopAndDestroy(); //certDesc
}


/**---------------------------------------------------------------------------
* Get certificate details of a certificate in Symbian certificate store
*
* @param aTrustedAuthority Text or ASN1 format subtree name of the CA
* @param aSerialNumber serial number of the certificate
* @param aCertDetails details of a certificate
* @param aResourceObject [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
*---------------------------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::CertificateDetails(const TDesC8& aTrustedAuthority,
                                                 const TDesC8& aSerialNumber,
                                                 TCertificateListEntry &aCertDetails) const
{
    TInt err = KErrNone;
    TSecurityObjectDescriptor *certDesc = new TSecurityObjectDescriptor;
    if (certDesc != NULL)
        {                
        certDesc->SetTrustedAuthority(aTrustedAuthority);
        certDesc->SetSerialNumber(aSerialNumber);
        TPckg<TSecurityObjectDescriptor> pckgTSecurityObjectDescriptor(*certDesc);


        TPckg<TCertificateListEntry> certDetails(aCertDetails);

        //Send request to the server
        err = SendReceive (PkiService::EGetCertDetails, TIpcArgs(&certDetails, &pckgTSecurityObjectDescriptor));
        delete certDesc;
        certDesc = NULL;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
}


/**-------------------------------------------------------------------------
* Get details of a key   
* @param aKeyId SubjectKeyID
* @param aKeyDetails [out] Returned details of a key.
*-------------------------------------------------------------------------*/
EXPORT_C TInt RPKIServiceAPI::KeyDetails(const TPKIKeyIdentifier& aKeyId,
						                TKeyListEntry &aKeyDetails) const
{	
	TPckg<TKeyListEntry> keyDetails(aKeyDetails);

	//Send request to the server
	return SendReceive (PkiService::EGetKeyDetails, TIpcArgs(&keyDetails, &aKeyId));
}


/**---------------------------------------------------------------------------
* List all certificate request  
* @param aCertReqList [out] Returned list of certificates.
*---------------------------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::ListCertificateRequestsL(
    CArrayFix<TCertificateRequestListEntry> *&aCertReqList) const
{
    // Get request count
    TInt reqCount = SendReceive(PkiService::ECertReqCount, TIpcArgs(NULL));

    TInt reqListGranularity = reqCount;
    if (reqListGranularity == 0)
        {
        reqListGranularity = 1;
        }

    CArrayFix<TCertificateRequestListEntry>* reqList;	
    reqList = new (ELeave) CArrayFixFlat<TCertificateRequestListEntry>(reqListGranularity);
    if (reqCount == 0)
        {
        aCertReqList = reqList;
        return;
        }

    CleanupStack::PushL(reqList);

    CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateRequestListEntry));
    CleanupStack::PushL(list);
    list->ResizeL(reqCount * sizeof(TCertificateRequestListEntry));

	TPtr8 ptrList = list->Ptr(0);

    User::LeaveIfError(SendReceive (PkiService::EGetCertReqList, TIpcArgs(&ptrList)));

    TCertificateRequestListEntry reqRef;
    for (TInt i = 0; i < reqCount; i++)
        {
        list->Read(i * sizeof(TCertificateRequestListEntry), (TAny*)&reqRef, sizeof(TCertificateRequestListEntry));
        reqList->AppendL(reqRef);
        }

    CleanupStack::PopAndDestroy(1);     // list
    CleanupStack::Pop();                // reqList

    aCertReqList = reqList;
}



    
/**---------------------------------------------------------
 *
 * Close()
 *
 * Closes a session to PKIService
 *
 *
 *----------------------------------------------------------*/
EXPORT_C void RPKIServiceAPI::Close(void)
{
    RSessionBase::Close();
}


/**
 * Set store types (either device or user stores used)
 *
 */
EXPORT_C TInt RPKIServiceAPI::SetStoreType(TPkiServiceStoreType aStoreType) const
    {
    TInt ret = KErrNone;
    TInt type(aStoreType);
    // Make sure a valid store type has been specified
    TPckgC<TInt> storeType(aStoreType);

    // Use the requested store type for both cert and key stores.
	ret = SendReceive(PkiService::ESetCertStoreType, TIpcArgs(type));
	if (ret == KErrNone) 
	    {
    	ret = SendReceive(PkiService::ESetKeyStoreType, TIpcArgs(type));
	    }

    return ret;
    }

EXPORT_C TInt RPKIServiceAPI::SetStoreType(TInt aStore, 
                                           TPkiServiceStoreType aStoreType) const
    {
    TInt ret = KErrNone;

    switch (aStore) 
        {
            case STORE_KEYSTORE:
            	ret = SendReceive (PkiService::ESetKeyStoreType, TIpcArgs(aStoreType));
                break;
            case STORE_CERTSTORE:
            	ret = SendReceive (PkiService::ESetCertStoreType, TIpcArgs(aStoreType));
            	break;
            default:
                ret = KErrNotSupported;
        }

    return ret;
    }

EXPORT_C TInt RPKIServiceAPI::CertStoreType(TPkiServiceStoreType& aStoreType) const
	{
	aStoreType = static_cast<TPkiServiceStoreType>(SendReceive(PkiService::EGetCertStoreType));
	return KErrNone;
	}

EXPORT_C TInt RPKIServiceAPI::KeyStoreType(TPkiServiceStoreType& aStoreType) const
	{
	aStoreType = static_cast<TPkiServiceStoreType>(SendReceive(PkiService::EGetKeyStoreType));
    return KErrNone;	
    }

EXPORT_C void RPKIServiceAPI::SetInformational(const TBool aInfoOnly)
    {
	SendReceive(PkiService::ESetInformational, TIpcArgs(aInfoOnly));
    }
    
