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
* Description:   CPKISupport class implements the PKI interface for Symbian
*                PKI storage.
*
*/



#ifndef __PKISUPPORT_H__
#define __PKISUPPORT_H__

#include <mctkeystore.h>
#include "pkidefs.h"
#include "eventmediatorapi.h" // for logging

class CPKIWrapper;
class CMapDescriptor;
class TSecurityObjectDescriptor;
class CUnifiedCertStore;
class MCTWritableCertStore;
class CCTCertInfo;
class CCertAttributeFilter;

enum TSubState
{
    ESSComplete,
    ESSContinue,
    ESSCompleteRequest,
};

enum TInitPhaseState
{
    EInitDone = 0,
    EInitContinueInitialize,
    EInitInitializeCertStore,
    EInitRetrieveCertList,
    EInitCompleteImportCerts,       
};

enum TPending
{
    ENoPendingOperation,    
    EInitializeCertStore,    
    EListCerts,
    EListDevCerts,
    ECertAdd,
    ECertRetrieve,
    ECertRemove,
    ESetTrust,
    ETrusted,    
    ESetApplicability,
    EApplications,
    EIsApplicable,
};


/**
 *  Implements PKI support.
 *
 *  @lib (internal) pkiservice.exe
 *  @since S60 v3.0
 */
class CPKISupport : public CActive
{
    public:

        // Constructors, destructor
        static CPKISupport* NewL(
            CPKIMapper& aMapper, CPKIWrapper& aWrapper);
        static CPKISupport* NewLC(
            CPKIMapper& aMapper, CPKIWrapper& aWrapper);
        
        
        ~CPKISupport();
        
        void StartInitializeL(const RMessage2& aMessage);
        void SetCurrentFunction(TInt aFunc);
                        
        // Certificate Data manipulation
        void StoreCertificateL(const TDesC &aLabel, 
            TCertificateOwnerType aOwnerType, const TDesC8 &aBufferPtr, 
            const TBool& aIsDeletable, TRequestStatus& aStatus);
        
        void AttachCertificateL(const TDesC &aLabel, 
            const TDesC8 &aBufferPtr, TRequestStatus& aStatus);
        
        void RetrieveCertificateL(const TDesC &aLabel,             
            TPtr8 &aBufferPtr, const TPKICertificateOwnerType& aType, 
            TRequestStatus& aStatus);
                    
        void RemoveCertificateL(const TDesC &aLabel, 
            TRequestStatus& aStatus);
                        
        void SelectCertificateL(const TDesC &aLabel, 
                                const TPKICertificateOwnerType& aType = EPKICACertificate);
        
        // Asynchronous sertificate store request
        void GetCertificateStoreListAsync();
        
        // Helper methods
        TInt GetRequiredBufferSize();
        void SetCallerStatusPending(TRequestStatus& aStatus);
        void CompleteCallerStatus(TInt aError);
        void SetTrustL(const TDesC &aLabel,
                       TBool aTrusted, TRequestStatus& aStatus);
        void TrustedL(const TDesC &aLabel,
                      TRequestStatus& aStatus);
        void SetApplicabilityL(const TDesC &aLabel,  
                               const RArray<TUid>& aApplUids, 
                               TRequestStatus& Status);
        void ApplicationsL(const TDesC &aLabel,
                           TRequestStatus& Status);
                    
        inline void SetCertStoreType(TPkiServiceStoreType aStoreType) 
            {
            iCertStoreType = aStoreType;
            };
                    
        inline const TPkiServiceStoreType CertStoreType() const 
            {
            return iCertStoreType;
            };        
  
    private: // implementation
        CPKISupport(CPKIMapper& aMapper, CPKIWrapper& aWrapper);
        void ConstructL();
        
        void CancelCurrentOperation();
        CCTKeyInfo::EKeyAlgorithm ConvertPKIAlgorithm(TPKIKeyAlgorithm aAlg);
        TPKIKeyAlgorithm ConvertSymbianAlgorithm(CCTKeyInfo::EKeyAlgorithm aAlg);
        void FindInterfacesL();
        void ListAllCertificatesL();
        void ReadNextCertForImportL();
        void SaveCertInfoToCacheL();
		TBool GetApplicationsOfCTFCertL();
        
        void ContinueStoreCertificateL();
        void ContinueRetrieveCertificate();
        void ContinueStoreCertifiedKeypairL();   
        void ContinueRemoveCertificate();
        void ContinueSetTrust();
        void ContinueSetApplicability();
        void ContinueApplications();        
        void ContinueTrusted();        
        void DoRunOperationL();
        void DoRunLoggedInOperationL();
        
        // Function to remove all MIDP2 certificates from the 
        // local listing (MIDP2 certs shouldn't be supported by VPN)
        void CleanupCertListL();

        // CActive methods
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        
        /**
         * Used for logging.
         * Exctracts certificate info from the parameters 
         * and inserts result into iCertInfoForLogging.
         * Result string looks like this:
         * \nLabel: <label> \nOwner: <owner> \nSubject: <subject> \nIssuer: <issuer>"
         * 
         * @param aLabel        Label of the certificate
         * @param aOwnerType    Owner of the certificate
         * @param aBufferPtr    The certificate
         */
        void ExtractCertInfoL(const TDesC& aLabel,
				         	  const TCertificateOwnerType& aOwnerType,
						      const TDesC8& aBufferPtr);
						      
        /**
         * Logs certificate storing into VPN client UI log.
         * 
         * @param aError      0 if storing succeeded
         */
        void LogCertStoring(TInt aError);
                
    private: // data        
        CPKIMapper                  &iMapper;
        CPKIWrapper                 &iWrapper;
        
        TPending                    iPending;
        TPtr8                       iImportCertDataPtr;                
                    
        RFs                         iFSession;
        CUnifiedCertStore           *iCertStore;
        MCTWritableCertStore        *iWritableCertStore;       
        TRequestStatus              *iCallerStatus;
        TInt                        iSupportStatus;
        TPtr8                       *iOutBufferPtr;        
        TInt                        iCurrentFunction;
        TInitPhaseState             iInitState;
        TSubState                   iSubState;        
        RMPointerArray<CCTCertInfo> iCerts;
        CCertAttributeFilter        *iCertFilter;        
        RMessage2                   iMessage;
        TKeyIdentifier              iKeyId;
        TInt                        iRequiredBufferLength;
        TBool                       iTrusted;
        RArray<TUid>                iApplUids;
        TInt                        iImportCounter;
        HBufC8                      *iImportCertData;
		TBool						iToggleSwitch;		
		
		TPkiServiceStoreType        iCertStoreType;
		TInt                        iDeviceCertStoreIndex;
        MCTWritableCertStore*       iWritableDeviceCertStore;
                
        REventMediator              iEventMediator; // for logging
        HBufC8*                     iCertInfoForLogging;
};
#endif
