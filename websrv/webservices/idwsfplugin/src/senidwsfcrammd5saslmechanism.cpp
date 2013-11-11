/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/








// INCLUDE FILES
#include <SenXmlUtils.h>

#include "SenIdWsfCrammd5SaslMechanism.h"
#include "MSenSaslMessage.h"

#include <flogger.h>

namespace
    {
    _LIT8(KCrammd5, "CRAM-MD5");
    }

// Create instance of concrete ECOM interface implementation
CSenIdWsfCrammd5SaslMechanism* CSenIdWsfCrammd5SaslMechanism::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenIdWsfCrammd5SaslMechanism* pNew =
                        CSenIdWsfCrammd5SaslMechanism::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }

CSenIdWsfCrammd5SaslMechanism*
            CSenIdWsfCrammd5SaslMechanism::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenIdWsfCrammd5SaslMechanism* pNew =
                new (ELeave) CSenIdWsfCrammd5SaslMechanism(aServiceManager);
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenIdWsfCrammd5SaslMechanism::CSenIdWsfCrammd5SaslMechanism(
                                    MSenCoreServiceManager& aServiceManager)
:CSenIdWsfPlainSaslMechanism(aServiceManager)
    {
    }

CSenIdWsfCrammd5SaslMechanism::~CSenIdWsfCrammd5SaslMechanism()
    {
    }

const TDesC8& CSenIdWsfCrammd5SaslMechanism::Name()
    {
    return KCrammd5();
    }

TInt CSenIdWsfCrammd5SaslMechanism::HandleResponseL(
                                                MSenSaslMessage& aResponse,
                                                MSenSaslMessage& aNewRequest
    )
    {
    const TInt KPasswordMinLength = 64;

    TInt retVal = KErrNone;

    TPtrC8 authzId = ipAccount->AuthzID();
    TPtrC8 authnId = ipAccount->AdvisoryAuthnID();
    if (authnId == KNullDesC8)
        {
        authnId.Set(authzId);
        }

    // Get challenge
    TPtrC8 theChallengeDataBase64 = aResponse.Data();

    HBufC8* pChallengeData = // push pChallengeData;
        iServiceManager.DecodeFromBase64LC(theChallengeDataBase64);
    if (pChallengeData->Length() == 0)
        {
        iServiceManager.Log()->Write(_L(
        "ERROR: CSenIdWsfCrammd5SaslMechanism::HandleResponseL \
        challenge date length = 0"));
        User::Leave(KErrArgument);
        }

    // Create MD5 hash generator
    CMD5* pMd5Gen = CMD5::NewL();
    CleanupStack::PushL(pMd5Gen);

    // The password is the shared secret
    // transform password
    TPtrC8 password(KNullDesC8);
    if(ipPassword)
        {
        password.Set(ipPassword->Des());
        }

    HBufC8* pPassword8 = TransformL(password);
    CleanupStack::PushL(pPassword8);

    if (pPassword8->Length() <= KPasswordMinLength)
        {
        // Pad with 0 bytes
        pPassword8 = pPassword8->ReAllocL(KPasswordMinLength);
        CleanupStack::Pop();    // Replace pwd on cleanup stack,
                                // might have changed(!)
        CleanupStack::PushL(pPassword8);
        pPassword8->Des().AppendFill(TChar(0),
                                    KPasswordMinLength - pPassword8->Length());
        }
    else
        {
        // Password is longer than 64 bytes, replace pwd with MD5 hash
        HBufC8* pPasswordHash = pMd5Gen->Hash(*pPassword8).AllocL();
        CleanupStack::PopAndDestroy(); // pPassword8;
        pPassword8 = pPasswordHash;
        CleanupStack::PushL(pPassword8);
        }

    // Create HMAC generator
    CHMAC* pHmacGen = CHMAC::NewL(*pPassword8, pMd5Gen); // takes ownership of
                                                         // pMd5Gen AND makes
                                                         // use of pPassword8
    CleanupStack::PopAndDestroy(); // pPassword8
    CleanupStack::Pop(); // pMd5Gen


    CleanupStack::PushL(pHmacGen); // push

    // Calculate hash
    TPtrC8 ptrHash = pHmacGen->Hash(*pChallengeData);
    if (ptrHash.Length() > 0)
        {
			_LIT8(KSpace," ");  //CodeScannerWarnings
        HBufC8 *pResponseData8 = HBufC8::NewLC(
            authnId.Length() + 1 + ptrHash.Length() * 2); // push pData8
        TPtr8 ptrResponseData8 = pResponseData8->Des();

        ptrResponseData8.Append(authnId);
        ptrResponseData8.Append(KSpace);

        // ptrHash needs to be converted to a lowercase hex value
        for (TInt i = 0; i < ptrHash.Length(); i++)
            {
            ptrResponseData8.AppendNumFixedWidth(ptrHash[i], EHex, 2);
            }

        retVal = aNewRequest.ConstructRequestFromL(KCrammd5, ptrResponseData8);

        CleanupStack::PopAndDestroy();  // pResponseData8;
        }

    CleanupStack::PopAndDestroy(); // pHmacGen


    CleanupStack::PopAndDestroy(); // pChallengeData;
    return retVal;
    }

// End of File
