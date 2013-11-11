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
#include "idwsfserviceprovider.h"

CIdWsfServiceProvider* CIdWsfServiceProvider::NewL(const TDesC8& aProviderID,
                                                   const TDesC8& aServiceURL)
    {
    CIdWsfServiceProvider* self =
                    CIdWsfServiceProvider::NewLC(aProviderID,aServiceURL);
    CleanupStack::Pop(self);
    return self;
    }

CIdWsfServiceProvider* CIdWsfServiceProvider::NewLC(const TDesC8& aProviderID,
                                                    const TDesC8& aServiceURL)
    {
    CIdWsfServiceProvider* self = new (ELeave) CIdWsfServiceProvider();
    CleanupStack::PushL(self);
    self->ConstructL(aProviderID,aServiceURL);
    return self;
    }

CIdWsfServiceProvider::CIdWsfServiceProvider()
    {
    }

void CIdWsfServiceProvider::ConstructL(const TDesC8& aProviderID,
                                       const TDesC8& aEndpoint)
    {
    SetProviderIdL(aProviderID);
    SetEndPointL(aEndpoint);
    }


CIdWsfServiceProvider::~CIdWsfServiceProvider()
    {
    delete ipProviderId;
    delete ipEndpoint;
    }

TInt CIdWsfServiceProvider::SetProviderIdL(const TDesC8& aProviderID)
    {
    HBufC8* pNew = NULL;
    if(aProviderID.Length()>0)
        {
        pNew = aProviderID.AllocL();
        }
    delete ipProviderId;
    ipProviderId = pNew;
    return KErrNone;
    }

TInt CIdWsfServiceProvider::SetEndPointL(const TDesC8& aServiceURL)
    {
    HBufC8* pNew = NULL;
    if(aServiceURL.Length()>0)
        {
        pNew = aServiceURL.AllocL();
        }
    delete ipEndpoint;
    ipEndpoint = pNew;
    return KErrNone;
    }

TPtrC8 CIdWsfServiceProvider::ProviderId()
    {
    if(ipProviderId)
        return *ipProviderId;
    else
        return KNullDesC8();
    }

TPtrC8 CIdWsfServiceProvider::Endpoint()
    {
    if(ipEndpoint)
        return *ipEndpoint;
    else
        return KNullDesC8();
    }


TBool CIdWsfServiceProvider::Equals(const TDesC8& aProviderID)
    {
    TPtrC8 provider(KNullDesC8());
    if(ipProviderId)
        {
        provider.Set(*ipProviderId);
        }

    if(provider == aProviderID)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// End of File
