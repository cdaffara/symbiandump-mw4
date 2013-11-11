/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* A data object for CPKIMapper class thar holds the information 
* required to map API set to use the storage model which is not 
* native for that API.
*
*/

#ifndef C_MAPDESCRIPTOR_H
#define C_MAPDESCRIPTOR_H

#include <e32base.h>
#include "pkidefs.h"

class TSecurityObjectDescriptor;
class CX500DistinguishedName;
class CX520AttributeTypeAndValue;

/**
 *  CMapDescriptor
 *
 *  Maintains information of installed certificates and keys
 *
 *  @lib internal (pkiservice.exe)
 *  @since S60 v3.0
 */
class CMapDescriptor : public CBase
{
public:
    enum TValidity
    {
        EValid,
        EExpired,
        ENotValidYet
    };
                
    static CMapDescriptor* NewL(const TDesC& aLabel,
                                const CX509Certificate& aCertificate,
                                TPKICertificateOwnerType aOwnerType,
                                TPkiServiceStoreType aCertStoreType);  
    
    ~CMapDescriptor();
       
    TBool IsMatchingL(TSecurityObjectDescriptor &aDesc, 
                      const TBool aInfoOnly, 
                      TPkiServiceStoreType aCertStoreType) const;                  
    TBool IsEqual(CMapDescriptor &aDesc);       
    void SetMapDeletable(TBool aDeletable);
    void SetMapTrusted(const TBool &aTrusted);
    void SetMapApplications(const RArray<TUid> &aApplications);
    
    TPtrC Label() const;
    const TPKIKeyIdentifier& CertificateKeyId() const;
    TPKICertificateOwnerType OwnerType() const;
    TUint KeySize() const;
    TPtrC8 TrustedAuthority() const;
    TPtrC8 IdentitySubjectName() const;
    TPtrC8 SerialNumber() const;
    TPKIKeyAlgorithm KeyAlgorithm() const;
    TBool Deletable() const;
    
    TTime EndTime() const;
    
    TBool IsMatchingCertStore(const TDesC& aCertStoreName);           
    TValidity CertValidity() const;

    TBool IsApplicable(TUid aApplUid) const;
    
private:            
    void ConstructL(const TDesC& aLabel,
                    const CX509Certificate& aCertificate,
                    TPKICertificateOwnerType aOwnerType,
                    TPkiServiceStoreType aCertStoreType);  
    
    TBool MatchL(const CX500DistinguishedName& aDn1,
                 const CX500DistinguishedName& aDn2) const;
    
    
    TBool HasElementL(const CX500DistinguishedName& aDn,
                      const CX520AttributeTypeAndValue& aElement) const;
               
    
    TBuf<SHORT_FILENAME_LENGTH> iLabel;
    TPkiServiceStoreType        iCertStoreType;
    TPKICertificateOwnerType    iOwnerType;             
    
    HBufC8*                     iIssuerName;          
    HBufC8*                     iSubjectName;       
    HBufC8*                     iRfc822Name;        
    HBufC8*                     iSerialNumber;      
    
    TPKIKeyIdentifier           iKeyId; 
        
    TUint                       iKeySize;                   
    TPKIKeyAlgorithm            iKeyAlgorithm;              
    TTime                       iStartTime;
    TTime                       iEndTime;
   
    TBool                       iIsDeletable;              
    TBool                       iIsTrusted;
    RArray<TUid>                iApplUids;
};


#endif //C_MAPDESCRIPTOR_H
