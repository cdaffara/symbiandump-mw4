/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Collects data type used by both client and server.
*
*/




#ifndef PKI_SERVICE_CLIENT_SERVER_COMMON_H
#define PKI_SERVICE_CLIENT_SERVER_COMMON_H

#include "pkidefs.h"

static const TUid KPkiServiceUid3 = {0x101FAE07};
_LIT(KPkiServerName,"!PKIserver");
_LIT(KPkiServerImg,"pkiservice");


const TInt KPkiMajorVersion = 1;
const TInt KPkiMinorVersion = 0;
const TInt KPkiBuildVersion = 0;


const TInt KMyServerInitHeapSize=0x1000;        //  4KB
const TInt KMyServerMaxHeapSize=0x1000000;      // 16MB
const TInt KMyServerStackSize=0x2000;           //  8KB

namespace PkiService
    {        
    enum TPkiServiceCommands
        { 
        ELogon,                             // 0
        EChangePassword,                    // 1
        ESignWithKeyId,                     // 2
        ESignWithCert,                      // 3
        EDecrypt,                           // 4
        EStoreKeypair,                      // 5            
        EGenerateKeypair,                   // 6
        ECancelPendingOperation,            // 7
        EGetRequiredBufferSize,             // 8
        EReadPublicKey,                     // 9
        EReadCertificate,                   // 10
        EStoreCertificate,                  // 11
        EAttachCertificate,                 // 12
        ERemoveKeypair,                     // 13
        ERemoveCertificate,                 // 14
        ECertCount,                         // 15
        EInitialize,                        // 16
        ESaveCertificateRequest,            // 17
        EReadCertificateRequest,            // 18
        EDeleteCertificateRequest,          // 19
        ESetTrust,                          // 20
        ETrusted,                           // 21
        EKeyCount,                          // 22
        EGetKeyList,                        // 23
    	EGetCertList,                       // 24
        ESetApplicability,                  // 25
        EApplications,                      // 26          
        EGetCertDetails,                    // 27
        ELogoff,                            // 28
        ECertReqCount,                      // 29
        EGetCertReqList,                    // 30
    	EGetKeyDetails,						// 31
    	EApplicableCertCount,				// 32
    	EGetApplicableCertList,				// 33
    	ESetCertStoreType,                  // 34
    	ESetKeyStoreType,                   // 35
    	ESetStoreType,                      // 36
    	EGetCertStoreType,					// 37
    	EGetKeyStoreType,					// 38
    	ESetInformational                   // 39
        };
    }



const TInt KKeyUsageNotUsed = 0x5A5A5A5A;

/**
* Internal class. Not used by the API
*/

class TSecurityObjectDescriptor
{
    public:
        TSecurityObjectDescriptor()
        :iTrustedAuthority(), 
         iTrustedAuthorityUsed(EFalse),
         iIdentitySubjectName(),
         iIdentitySubjectNameUsed(EFalse),
         iIdentityRfc822Name(),
         iIdentityRfc822NameUsed(EFalse),
         iSerialNumber(),
         iSerialNumberUsed(EFalse),
         iSubjectKeyId(),
         iSubjectKeyIdUsed(EFalse),
         iOwnerType(EPKICACertificate),
         iOwnerTypeUsed(EFalse),
         iKeyUsage( (TPKIKeyUsage) 0),
         iKeyUsageDer(),
         iKeyUsageUsed(EFalse),
         iKeySize(0),
         iKeySizeUsed(EFalse),
         iKeyAlgorithm(EPKIInvalidAlgorithm),
         iKeyAlgorithmUsed(EFalse),
         iObjectSize(EFalse),
         iEncrypted(EFalse),
         iObjectName(),
         iObjectNameUsed(EFalse),
         iIsDeletable(EFalse),
         iIsDeletableUsed(EFalse)
            {};
            
        void SetSubjectKeyId(const TPKIKeyIdentifier &aKeyId)
            {
            if(aKeyId.Length() > 0)
                {
                iSubjectKeyId.Copy(aKeyId);
                iSubjectKeyIdUsed = ETrue;
                }
            else
                {
                iSubjectKeyIdUsed = EFalse;
                }
            };
        void SetOwnerType(const TPKICertificateOwnerType aOwnerType) 
            {
            iOwnerType = aOwnerType;
            iOwnerTypeUsed = ETrue;
            };
        void SetKeyUsage(const TPKIKeyUsage aKeyUsage)      // Optional if only certificate is needed
            {
            if((TInt)aKeyUsage != KKeyUsageNotUsed)
                {
                iKeyUsage = aKeyUsage;
                iKeyUsageUsed = ETrue;
                }
            else
                {
                iKeyUsage = (TPKIKeyUsage)0;
                iKeyUsageUsed = EFalse;
                }
            };                          
        // Key usage is not defined in the filter, this will be checked separately
        void SetKeySize(const TUint aKeySize)
            {
            if(aKeySize != 0)
                {
                iKeySize = aKeySize;
                iKeySizeUsed = ETrue;
                }
            else
                {
                iKeySizeUsed = EFalse;
                }
            };              // Optional if only certificate is needed
        void SetObjectName(const TDesC &aObjectName)
            {
            if(aObjectName.Length() > 0)
                {
                iObjectName.Copy(aObjectName);
                iObjectNameUsed = ETrue;
                }
            else
                {
                iObjectNameUsed = EFalse;
                }
            };
        // Issuer and serial are not defined in the filter, these will be checked separately
        void SetTrustedAuthority(const TDesC8 &aTrustedAuthority)
                {
                if(aTrustedAuthority.Length() > 0)
                    {
                    iTrustedAuthority.Copy(aTrustedAuthority);
                    iTrustedAuthorityUsed = ETrue;
                    }
                else
                    {
                    iTrustedAuthorityUsed = EFalse;
                    }
                };
        void SetIdentitySubjectName(const TDesC8 &aIdentitySubjectName)
                {
                    if(aIdentitySubjectName.Length() > 0)
                        {
                        iIdentitySubjectName.Copy(aIdentitySubjectName);
                        iIdentitySubjectNameUsed = ETrue;
                        }
                    else
                        {
                        iIdentitySubjectNameUsed = EFalse;
                        }
                };
        void SetIdentityRfc822Name(const TDesC8 &aIdentityRfc822Name)
                {
                    if(aIdentityRfc822Name.Length() > 0)
                        {
                        iIdentityRfc822Name.Copy(aIdentityRfc822Name);
                        iIdentityRfc822NameUsed = ETrue;
                        }
                    else
                        {
                        iIdentityRfc822NameUsed = EFalse;
                        }
                };
        void SetSerialNumber(const TDesC8 &aSerialNumber)
            {
            if(aSerialNumber.Length() > 0)
                {
                iSerialNumber.Copy(aSerialNumber);
                iSerialNumberUsed = ETrue;
                }
            else
                {
                iSerialNumberUsed = EFalse;
                }
            };
        void SetKeyAlgorithm(const TPKIKeyAlgorithm &aKeyAlgorithm) 
            {
            iKeyAlgorithm = aKeyAlgorithm;
            iKeyAlgorithmUsed = ETrue;
            };
		void SetIsDeletable(const TUint &aIsDeletable) 
		    {
		    iIsDeletable = aIsDeletable;
		    iIsDeletableUsed = ETrue;
		    };
        void Reset()
            {
            iObjectNameUsed = EFalse;
            iTrustedAuthorityUsed = EFalse;
            iIdentitySubjectNameUsed = EFalse;
            iIdentityRfc822NameUsed = EFalse;
            iSubjectKeyIdUsed = EFalse;
            iOwnerTypeUsed = EFalse;
            iKeyUsageUsed = EFalse;
            iKeySizeUsed = EFalse;
            iKeyAlgorithmUsed = EFalse;
            };
        TSecurityObjectDescriptor& operator=(const TSecurityObjectDescriptor& aDesc)
            {
            if (this != &aDesc)
                {
                this->iObjectName = aDesc.iObjectName;
                this->iObjectNameUsed = aDesc.iObjectNameUsed;
                this->iTrustedAuthority = aDesc.iTrustedAuthority;
                this->iTrustedAuthorityUsed = aDesc.iTrustedAuthorityUsed;
                this->iIdentitySubjectName = aDesc.iIdentitySubjectName;
                this->iIdentitySubjectNameUsed = aDesc.iIdentitySubjectNameUsed;
                this->iIdentityRfc822Name = aDesc.iIdentityRfc822Name;
                this->iIdentityRfc822NameUsed = aDesc.iIdentityRfc822NameUsed;
                this->iSerialNumber = aDesc.iSerialNumber;
                this->iSerialNumberUsed = aDesc.iSerialNumberUsed;
                this->iSubjectKeyId = aDesc.iSubjectKeyId;      
                this->iSubjectKeyIdUsed = aDesc.iSubjectKeyIdUsed;      
                this->iOwnerType = aDesc.iOwnerType;            
                this->iOwnerTypeUsed = aDesc.iOwnerTypeUsed;            
                this->iKeyUsage = aDesc.iKeyUsage;          
                this->iKeyUsageDer = aDesc.iKeyUsageDer;            
                this->iKeyUsageUsed = aDesc.iKeyUsageUsed;          
                this->iKeySize = aDesc.iKeySize;            
                this->iKeySizeUsed = aDesc.iKeySizeUsed;            
                this->iKeyAlgorithm = aDesc.iKeyAlgorithm;      
                this->iKeyAlgorithmUsed = aDesc.iKeyAlgorithmUsed;
    			this->iIsDeletable = aDesc.iIsDeletable;      
    			this->iIsDeletableUsed = aDesc.iIsDeletableUsed;
                }
            return *this;
            };
       
        // No get methods introduced, values are used directly!     
        TBuf8<KMaxX500DN>           iTrustedAuthority;          // Cert TrustedAuthority
        TBool                       iTrustedAuthorityUsed;
        TBuf8<KMaxX500DN>           iIdentitySubjectName;       // Identity subject name
        TBool                       iIdentitySubjectNameUsed;
        TBuf8<KMaxRfc822>           iIdentityRfc822Name;        // Identity subjectAltName rfc822 name
        TBool                       iIdentityRfc822NameUsed;
        TBuf8<KMaxSerial>           iSerialNumber;              // Serialnumber
        TBool                       iSerialNumberUsed;
        TPKIKeyIdentifier           iSubjectKeyId;              // SHA1 hash of the corresponding private key
        TBool                       iSubjectKeyIdUsed;
        TPKICertificateOwnerType    iOwnerType;                 // User, CA or peer. If user certificate, at least key usage must be set
        TBool                       iOwnerTypeUsed;
        TPKIKeyUsage                iKeyUsage;                  // Key usage flags
        TBuf8<KMaxUsageDer>         iKeyUsageDer;               // Der format flags
        TBool                       iKeyUsageUsed;
        TUint                       iKeySize;                   // Key size
        TBool                       iKeySizeUsed;
        TPKIKeyAlgorithm            iKeyAlgorithm;              // RSA, DSA
        TBool                       iKeyAlgorithmUsed;
		TBool                       iObjectSize;                // Byte size of the referenced security object
        TBool                       iEncrypted;                 // Only used for passing aEncrypted parameter from client to server
		TBuf<SHORT_FILENAME_LENGTH> iObjectName;
		TBool                       iObjectNameUsed;    
		TBool                       iIsDeletable;                //
		TBool                       iIsDeletableUsed;
};

#endif // PKI_SERVICE_CLIENT_SERVER_COMMON_H
