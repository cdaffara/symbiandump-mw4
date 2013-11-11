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
* Description: Implementation of CDmAdRtNodeDataApi.
*
*/


#include "dmadrtnodedataapic.h"
#include "DmAdRtNodeData.h"
#include "dmadcallback.h"
    
CDmAdRtNodeDataApi* CDmAdRtNodeDataApi::NewL()
    {
    CDmAdRtNodeDataApi* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CDmAdRtNodeDataApi* CDmAdRtNodeDataApi::NewLC()
    {
    CDmAdRtNodeDataApi* self = new (ELeave) CDmAdRtNodeDataApi();
    CleanupStack::PushL(self);
    return self;
    }

CDmAdRtNodeDataApi::CDmAdRtNodeDataApi()
    {
    }
    
CDmAdRtNodeDataApi::~CDmAdRtNodeDataApi()
    {
    }
    
//---------------------------------------------------------------------------------------
    
CDmAdRtNodeData* CDmAdRtNodeDataApi::CreateDmAdRtNodeDataL(const TDesC8& aUri, MDmAdCallBack* aCallBack)
    {
    return CDmAdRtNodeData::NewL(aUri, aCallBack);
    }
    
void CDmAdRtNodeDataApi::DeleteDmAdRtNodeData(CDmAdRtNodeData* aDmAdRtNodeData)
    {
    delete aDmAdRtNodeData;
    }
    
void CDmAdRtNodeDataApi::UpdateLeafDataL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aObject)
    {
    ASSERT(aDmAdRtNodeData);
    aDmAdRtNodeData->UpdateLeafDataL(aUri, aObject);
    }

void CDmAdRtNodeDataApi::FetchLeafObjectLC(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject)
    {
    ASSERT(aDmAdRtNodeData);
    aDmAdRtNodeData->FetchLeafObjectLC(aUri, aLuid, aObject);
    }   

void CDmAdRtNodeDataApi::SetDefaultSettingsL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri)
    {
    ASSERT(aDmAdRtNodeData);
    aDmAdRtNodeData->SetDefaultSettingsL(aUri);
    }

TBool CDmAdRtNodeDataApi::AreUriTypesSame(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri)
    {
    ASSERT(aDmAdRtNodeData);
    
    TDmAdUriType uriType1 = EDmAdUriTypeCert;
    //CDmAdRtNodeData::UriTypeL leaves if the URI type is unknown
    TRAPD(err, uriType1 = CDmAdRtNodeData::UriTypeL(aUri));    
    TDmAdUriType uriType2 = aDmAdRtNodeData->UriType();
    
    if (err == KErrNone && uriType1 == uriType2)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
