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
* Description:   Defines common API for handling PKI certificates and keys.
*
*/



#if !defined (__PKIWRAPPER_H__)
#define __PKIWRAPPER_H__

#include <e32base.h>

#include "pkidefs.h"
#include "pkiserviceclientservercommon.h"

class CMapDescriptor;
class CPKISupport;
class CPKIMapper;

class CPKIWrapper : public CActive
{
    public:
        
        // Constructors, destructor
        static CPKIWrapper* NewL(CPKIMapper& aMapper);
        ~CPKIWrapper();

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Interface
        ////////////////////////////////////////////////////////////////////////////////////////////
        void InitOperation(const RMessage2& aMessage);
        void CancelPendingOperation();
        TInt GetRequiredBufferSizeL(const RMessage2& aMessage);
        void CompleteRequestAndCleanupL();

        void InitializeL(const RMessage2& aMessage);        
        void SetTrusted(TBool aValue);
        void SetApplications(const RArray<TUid>& aApplUids);
                
        void SetCertStoreType(TPkiServiceStoreType aStoreType);
               
        TPkiServiceStoreType CertStoreType() const;               
        
        void SetInformational(const TBool aInfoOnly);
        TBool Informational() const;
        
    private: // From CActive    
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
    
    private: // implementation   
        CPKIWrapper(CPKIMapper& aMapper);     
        void ConstructL();

        // ===================================================================================================================
        // Start phase functions
        // ===================================================================================================================              
        void StartSetTrustL();
        void StartTrustedL();
        void StartSetApplicabilityL();
        void StartApplicationsL();

        void StartRemoveCertificateL();

        // Certificate operations
        void StartReadCertificateL();
        // Peer or CA certificate
        void StartStoreCertificateL();
        // Enrollrd certificate
        void StartAttachCertificateL();
        
        // ===================================================================================================================
        // Execution phase functions
        // ===================================================================================================================
        // Crypto operations        
        void ExecuteSetTrustL();
        void ExecuteTrustedL();
        void ExecuteSetApplicabilityL();
        void ExecuteApplicationsL();
        void ExecuteRemoveCertificateL();

        // Certificate operations
        void ExecuteReadCertificateL();        
        // Peer or CA certificate
        void ExecuteStoreCertificateL();
        // Enrollrd certificate
        void ExecuteAttachCertificateL();
        
        
        enum TWrapperState
            {
            EExecute,
            EComplete
            };
        
        
        CPKIMapper&                         iMapper;
        CPKISupport*                        iPKISupport;
        TInt                                iIndex;
        CMapDescriptor*                     iCurrentMapping;
        TBuf<MAX_FILENAME_LENGTH>*          iObjectName;
        TInt                                iCurrentStatus;
        TWrapperState                       iCurrentState;
        TPckgBuf<TSecurityObjectDescriptor> iCurrentDescriptor;
        RMessage2                           iMessage;        
        HBufC8*                             iCertBuffer;
        TPtr8                               iPtrCertBuffer;
        TInt                                iRequiredBufferLength;
        RArray<TUid>                        iUidArray;
        TUint                               iCount;
		TUint                               iMaxCount;		
		TBool                               iTrusted;
		
		TBool                               iInfoOnly;

};

#endif
