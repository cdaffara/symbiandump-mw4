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
* Description:   CPKIMapper class holds the information required to map API set 
*				 to use the storage model which is not native for that API.
*
*/



#if !defined (__PKIMAPPER_H__)
#define __PKIMAPPER_H__

#include <e32base.h>
#include "pkiserviceclientservercommon.h"
#include "pkidefs.h"

class CMapDescriptor;

/**
 *  CPKIMapper
 *
 *  Maintains array of CMapDescriptor objects
 *
 *  @lib internal (pkiservice.exe)
 *  @since S60 v3.0
 */
class CPKIMapper : public CBase
{
    public:
        // Constructors, destructor
        // When constructing an invocation, check if some key/certificate has been manually removed.
        static CPKIMapper* NewL();        
        ~CPKIMapper();
        
        TInt AddMapping(CMapDescriptor* aMap);       
        void DeleteMapping(TInt aIndex);
        CMapDescriptor& GetMapDescriptorAtIndex(TInt aIndex);
        // One-to-one mapping functions        
        void GetCertificateKeyIdL(TSecurityObjectDescriptor& aDescriptor, TPKIKeyIdentifier& aKeyId,
                                  TPkiServiceStoreType aStoreType = EPkiStoreTypeUser) const;
                                 
        TInt ResolveCertMappingL(TSecurityObjectDescriptor& aDescriptor, TDes16& aFilename, 
                                TInt &aIndex, TBool aInfoOnly, 
                                TPkiServiceStoreType aStoreType = EPkiStoreTypeUser) const;
        
        TInt CertCount(TBool aInfoOnly);
		TInt ApplicableCertCount(const RArray<TUid>& aUidArray);		
        TInt GetCertListL(const RMessage2& aMessage, TBool aInfoOnly);
        
        TInt GetCertDetailsL(TSecurityObjectDescriptor& aSecDesc, 
                             TPkiServiceStoreType aStoreType, 
                             TBool aInfoOnly,
                             TCertificateListEntry& aResultCertInfo);

        
		void GetApplicableCertListL(const RMessage2& aMessage, const RArray<TUid>& aUidArray);               
        		    
        // Function to check whether the given certificate is unique (doesn't exist in cert store)
        TBool CertificateIsUniqueL(const TDesC8& aCertData);
        
        // Function that returns a guaranteedly unique certificate label.
        void GenerateUniqueNameL(const TDesC8& aCertData, TDes& aName, 
                                 TCertificateOwnerType aOwnerType = ECACertificate);       
        void SetCacheCreated();
        TBool CacheCreated();        
    
    private: // implementation
                        
        TBool LabelIsUnique(const TDesC& aLabel) const;
        void LogMap(CMapDescriptor& aDescriptor) const;
        void LogSearchArguments(TSecurityObjectDescriptor& aDescriptor) const;        
                
    private: // C'tor    

        CPKIMapper();
        void ConstructL();
    
    private: // data    
        TBool   iCacheCreated;       
        TInt    iCount;
        /// Used when generating uniqname

        RPointerArray<CMapDescriptor>*      iMapping;
        TPckgBuf<TSecurityObjectDescriptor> iCurrentDescriptor;
    	TBuf<MAX_FILENAME_LENGTH>           iObjectName;
};


#endif
