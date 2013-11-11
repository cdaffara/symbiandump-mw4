/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: A class that provides a signing operation.
*
*/



#ifndef C_PKISERVICESIGNER_H
#define C_PKISERVICESIGNER_H

#include <e32base.h>

#include "pkidefs.h"

class MCTKeyStoreManager;
class CLogonServices;

/**
 * This class signs a given data by using a stored private key.
 */
class CPkiServiceSigner : public CActive
    {
public:
    static CPkiServiceSigner* NewL(CLogonServices& aLogonServices);
    ~CPkiServiceSigner();

    void Sign(const TPKIKeyIdentifier& aKeyId,
              const TDesC8& aDataToBeSigned,
              HBufC8*& aSignature,
              CUnifiedKeyStore& aUnifiedKeyStore,
              TInt aUsedKeyStore, 
              TRequestStatus& aStatus);


    void DoCancel();
    void RunL();

private:
    enum TSignerState
        {
        ESignerIdle = 0,
        ESignerGettingKey,
		ESignerOpeningSigner,
		ESignerSigning		        
        };
        
    CPkiServiceSigner(CLogonServices& aLogonServices);    
    void Cleanup();

    CLogonServices&  iLogonServices;
        
    TSignerState     iState;
    TRequestStatus*  iClientStatus;
    CUnifiedKeyStore* iKeyStore; //Not owned by this class
    TInt              iUsedKeyStore;
    TPtrC8              iInput;    //Not owned by this class
    HBufC8**            iOutputBuffer;//Not owned by this class
    
    const CCTKeyInfo*          iUsedKeyInfo;
    
    MRSASigner*                iRsaSigner;
    MDSASigner*                iDsaSigner;    
    
    CRSASignature*             iRsaSignature;
    CDSASignature*             iDsaSignature;    
    
    RMPointerArray<CCTKeyInfo> iKeysList; 
    };

#endif //C_PKISERVICESIGNER_H


