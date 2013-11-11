/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CDmAdPrivKey.
*
*/


#include <asn1dec.h>

#include "dmadstoreprivkey.h"
#include "vpnlogger.h"

CDmAdPrivKey* CDmAdPrivKey::NewL(RPKIServiceAPI& aPkiServiceApi)
    {
    TRACE("CDmAdPrivKey::NewL");
    
    CDmAdPrivKey* self = NewLC(aPkiServiceApi);
    CleanupStack::Pop(self); 
    return self;
    }

CDmAdPrivKey* CDmAdPrivKey::NewLC(RPKIServiceAPI& aPkiServiceApi)
    {
    CDmAdPrivKey* self = new (ELeave) CDmAdPrivKey(aPkiServiceApi);
    CleanupStack::PushL(self);
    return self;
    }

CDmAdPrivKey::CDmAdPrivKey(RPKIServiceAPI& aPkiServiceApi) : iPkiServiceApi(&aPkiServiceApi)
    {
    TRACE("CDmAdPrivKey::CDmAdPrivKey");
    }
    
CDmAdPrivKey::~CDmAdPrivKey()
    {
    TRACE("CDmAdPrivKey::~CDmAdPrivKey");
    }
    
TBool CDmAdPrivKey::FindL(const TDesC8& aLuid)
    {
    TRACE("CDmAdPrivKey::FindL");
    
    TBool found = EFalse;

    const TInt KMaxPublicKeyLth = 512;
    HBufC8* privKeyData = HBufC8::NewLC(KMaxPublicKeyLth);
    TPtr8 privKeyDataPtr(privKeyData->Des());
       
    TInt status = iPkiServiceApi->ReadPublicKey(aLuid,
                                                privKeyDataPtr);
    if (status == KErrNone)
        {
        found = ETrue;
        }
    
    CleanupStack::PopAndDestroy(privKeyData); 
    return found;
    }

void CDmAdPrivKey::FetchL(const TDesC8& aLuid, CDmAdPrivKeyParms& aParms)
    {
    TRACE("CDmAdPrivKey::FetchL");
    
    TPkiServiceStoreType currentKeyStore;
    User::LeaveIfError(iPkiServiceApi->KeyStoreType(currentKeyStore));
    User::LeaveIfError(iPkiServiceApi->SetStoreType(STORE_KEYSTORE, EPkiStoreTypeAny));
    
    TKeyListEntry* entry = new (ELeave) TKeyListEntry();
    CleanupStack::PushL(entry);
    User::LeaveIfError(iPkiServiceApi->KeyDetails(aLuid, *entry));
    
    User::LeaveIfError(iPkiServiceApi->SetStoreType(STORE_KEYSTORE, currentKeyStore));
    
    aParms.SetKeyTypeL(entry->iKeyAlgorithm);
    aParms.SetKeyIdL(entry->iSubjectKeyId);
    aParms.SetKeyLength(entry->iKeySize);    
    
    CleanupStack::PopAndDestroy(); // entry
    }
    
void CDmAdPrivKey::DeleteL(const TDesC8& aLuid)
    {
    TRACE("CDmAdPrivKey::DeleteL");
    
    User::LeaveIfError(iPkiServiceApi->RemoveKeypair(aLuid));
    }

void CDmAdPrivKey::ListL(RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdPrivKey::ListL");
    
    TPkiServiceStoreType currentKeyStore;
    User::LeaveIfError(iPkiServiceApi->KeyStoreType(currentKeyStore));
    User::LeaveIfError(iPkiServiceApi->SetStoreType(STORE_KEYSTORE, EPkiStoreTypeAny));    
    
    CArrayFix<TKeyListEntry>* keyList;
    iPkiServiceApi->ListKeysL(keyList);
    CleanupStack::PushL(keyList);

    User::LeaveIfError(iPkiServiceApi->SetStoreType(STORE_KEYSTORE, currentKeyStore));

    for (TInt i=0; i<keyList->Count(); ++i)
        {
        TKeyListEntry& entry = keyList->At(i);
        HBufC8* luidElem = entry.iSubjectKeyId.AllocLC();
        aLuidList.AppendL(luidElem);
        CleanupStack::Pop(luidElem);
        }
    
    CleanupStack::PopAndDestroy(keyList);
    }
    
