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

#include "SenIdWsfPlainSaslMechanism.h"
#include "MSenCoreServiceManager.h"

namespace
    {
    //_LIT(KPlain,  "PLAIN");
    _LIT8(KPlain,   "PLAIN");
    }

// Create instance of concrete ECOM interface implementation
CSenIdWsfPlainSaslMechanism* CSenIdWsfPlainSaslMechanism::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenIdWsfPlainSaslMechanism* pNew =
                        CSenIdWsfPlainSaslMechanism::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }


CSenIdWsfPlainSaslMechanism* CSenIdWsfPlainSaslMechanism::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenIdWsfPlainSaslMechanism* pNew =
                    new (ELeave) CSenIdWsfPlainSaslMechanism(aServiceManager);
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenIdWsfPlainSaslMechanism::CSenIdWsfPlainSaslMechanism(
                                    MSenCoreServiceManager& aServiceManager)
: CSenSecurityMechanism(aServiceManager)
    {
    }

CSenIdWsfPlainSaslMechanism::~CSenIdWsfPlainSaslMechanism()
    {
    delete ipPassword;
    }


void CSenIdWsfPlainSaslMechanism::SetIdentity(
                                    CSenIdentityProvider* aIdentityProvider,
                                    MSenIdentityManager* aIdentityManager)
    {
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, SetIdentityL(aIdentityProvider, aIdentityManager);)
    leaveCode=0; // not used
    }

void CSenIdWsfPlainSaslMechanism::SetIdentityL(
                                    CSenIdentityProvider* aIdentityProvider,
                                    MSenIdentityManager* aIdentityManager)
    {
    CSaslMechanism::SetIdentity(aIdentityProvider,aIdentityManager);

    TPtrC8 password = ipAccount->Password();

    delete ipPassword;
    ipPassword = NULL;

    if (password != KNullDesC8)
        {
        ipPassword = SenXmlUtils::DecodeHttpCharactersLC(password);
        CleanupStack::Pop(); // DecodeHttpCharactersLC
        }
    else
        {
        HBufC8* pPassword2 = CSaslMechanism::Password8L();
        if(pPassword2)
            {
            ipPassword = pPassword2;
            }
        else
            {
            ipPassword = KNullDesC8().AllocL();
            }
        }
    }
    

TBool CSenIdWsfPlainSaslMechanism::IsPasswordFromUser()
    {
        TBool retVal;

        if (!ipAccount)
            {
            retVal = EFalse;
            }
        else
            {
            retVal = (ipAccount->Password() == KNullDesC8);
            }

        return retVal;
    }

void CSenIdWsfPlainSaslMechanism::ClearPasswordL()
    {
    if (ipAccount)
        {
        CSenElement& element = ipAccount->AsElement();
        delete element.RemoveElement(KSenIdpPasswordLocalname);
        }
    delete ipPassword;
    ipPassword = NULL;
    HBufC8* pPassword = Password8L();
    if (pPassword)
        {
        ipPassword = pPassword;
        }
    }

TInt CSenIdWsfPlainSaslMechanism::MaxAttempts()
    {
    return 3;
    }

const TDesC8& CSenIdWsfPlainSaslMechanism::Name()
    {
    return KPlain();
    }

TInt CSenIdWsfPlainSaslMechanism::HandleResponseL(
    MSenSaslMessage& /* aResponse */,
    MSenSaslMessage& aNewRequest
    )
    {
    TPtrC8 authzId = ipAccount->AuthzID();
    TPtrC8 authnId = ipAccount->AdvisoryAuthnID();

    if (authnId == KNullDesC8)
        {
        authnId.Set(authzId);
        authzId.Set(KNullDesC8);
        }

    TPtrC8 password(KNullDesC8);

    if(ipPassword)
        {
        password.Set(ipPassword->Des());
        }

    HBufC8* pSharedSecret = TransformL(password);
    CleanupStack::PushL(pSharedSecret);

    HBufC8* pData = HBufC8::NewLC(authzId.Length()
                                + authnId.Length()
                                + pSharedSecret->Length() + 2);
    TPtr8 data = pData->Des();

    if(authnId!=authzId)
        {
        data.Append(authzId); //  if authzId == authnId, leave out authzId
        }

    data.Append('\x00');    // NOTE, this zero line is a MUST,
                            // even if no authzId was added
    data.Append(authnId);
    data.Append('\x00');
    data.Append(*pSharedSecret);
    TInt retVal = aNewRequest.ConstructRequestFromL(KPlain, *pData);
    CleanupStack::PopAndDestroy(); // pData;
    CleanupStack::PopAndDestroy(); // pSharedSecret
    return retVal;
    }

// End of File
