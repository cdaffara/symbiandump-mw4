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
#include <SenCredential.h>

#include "idwsfresourceoffering.h"

CIdWsfResourceOffering* CIdWsfResourceOffering::NewL()
    {
    CIdWsfResourceOffering* pNew = new (ELeave) CIdWsfResourceOffering;
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfResourceOffering::CIdWsfResourceOffering()
    {
    }

void CIdWsfResourceOffering::ConstructL()
    {
    // Leave everything uninitialized for now
    }

CIdWsfResourceOffering::~CIdWsfResourceOffering()
    {
    delete ipResourceId;
    iServices.ResetAndDestroy();
    }

TPtrC8 CIdWsfResourceOffering::ResourceId()
    {
    if(ipResourceId)
        return *ipResourceId;
    else
        return KNullDesC8();
    }

void CIdWsfResourceOffering::SetResourceIdL(const TDesC8& aId)
    {
    HBufC8* pNew = NULL;
    if(aId.Length()>0)
        pNew = aId.AllocL();
    delete ipResourceId;
    ipResourceId = pNew;
    }

CIdWsfServiceInstance& CIdWsfResourceOffering::LastServiceL()
    {
    __ASSERT_ALWAYS(iServices.Count() > 0, User::Leave(KErrNotFound));
    return *(iServices[iServices.Count() - 1]);
    }

void CIdWsfResourceOffering::AddServiceInstanceL(
                                            CIdWsfServiceInstance* aInstance)
    {
    if(ipResourceId)
        {
        aInstance->SetResourceIdL(*ipResourceId);
        }
    User::LeaveIfError(iServices.Append(aInstance));
    }

void CIdWsfResourceOffering::AddCredentialL(const CSenCredential& aCredential)
    {
    // Replace all credentials with the given id, with the credential
    for (TInt i = 0; i < iServices.Count(); i++)
        {
        iServices[i]->SetCredentialL(aCredential);
        }
    }

RPointerArray<CIdWsfServiceInstance>& CIdWsfResourceOffering::ServicesL()
    {
    return iServices;
    }

TInt CIdWsfResourceOffering::SetFacetL(CSenFacet& aFacet)
    {
    TInt retVal = KErrNone;

    TInt count(iServices.Count());
    for (TInt i=0; i<count; i++)
        {
        retVal = iServices[i]->SetFacetL(aFacet);
        if (retVal != KErrNone) return retVal;
        }

    return retVal;
    }

// End of File
