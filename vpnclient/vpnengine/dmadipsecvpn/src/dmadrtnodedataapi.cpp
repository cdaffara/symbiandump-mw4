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
#include "vpnlogger.h"    
    
CDmAdRtNodeDataApi* CDmAdRtNodeDataApi::NewL(void)
    {
    TRACE("CDmAdRtNodeDataApi::NewL");
    
    CDmAdRtNodeDataApi* self = NewLC();
    CleanupStack::Pop(); // self
    return self;
    }

CDmAdRtNodeDataApi* CDmAdRtNodeDataApi::NewLC(void)
    {
    CDmAdRtNodeDataApi* self = new (ELeave) CDmAdRtNodeDataApi();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CDmAdRtNodeDataApi::ConstructL(void)
    {
    TRACE("CDmAdRtNodeDataApi::ConstructL");
    }
    
CDmAdRtNodeDataApi::CDmAdRtNodeDataApi()
    {
    TRACE("CDmAdRtNodeDataApi::CDmAdRtNodeDataApi");
    }
    
CDmAdRtNodeDataApi::~CDmAdRtNodeDataApi()
    {
    TRACE("CDmAdRtNodeDataApi::~CDmAdRtNodeDataApi");
    }
    
//---------------------------------------------------------------------------------------
    
CDmAdRtNodeData* CDmAdRtNodeDataApi::CreateDmAdRtNodeDataL(const TDesC8& aUri, MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdRtNodeDataApi::CreateDmAdRtNodeDataL");    
    return CDmAdRtNodeData::NewL(aUri, aCallBack);
    }
    
void CDmAdRtNodeDataApi::DeleteDmAdRtNodeData(CDmAdRtNodeData* aDmAdRtNodeData)
    {
    TRACE("CDmAdRtNodeDataApi::DeleteDmAdRtNodeData");
    delete aDmAdRtNodeData;
    }
    
void CDmAdRtNodeDataApi::UpdateLeafDataL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeDataApi::UpdateLeafDataL");
    aDmAdRtNodeData->UpdateLeafDataL(aUri, aObject);
    }

void CDmAdRtNodeDataApi::FetchLeafObjectLC(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject)
    {
    aDmAdRtNodeData->FetchLeafObjectLC(aUri, aLuid, aObject);
    }   

void CDmAdRtNodeDataApi::SetDefaultSettingsL(CDmAdRtNodeData* /*aDmAdRtNodeData*/, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdRtNodeDataApi::SetDefaultSettingsL");
    }

TBool CDmAdRtNodeDataApi::AreUriTypesSame(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri)
    {
    TRACE("CDmAdRtNodeDataApi::AreUriTypesSameL");
    
    TDmAdUriType uriType1 = EDmAdUriTypeVpnAp;
    
    //CDmAdRtNodeData::UriTypeL leaves if uri is unknown
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
