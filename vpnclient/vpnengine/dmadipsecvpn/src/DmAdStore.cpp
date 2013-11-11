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
* Description: Implementation of CDmAdStore.
*
*/



#include "DmAdStore.h"
#include "DmAdRtNodeData.h"
#include "dmadutil.h"
#include "vpnlogger.h"

#include <vpnlogmessages.rsg>
    
CDmAdStore* CDmAdStore::NewL(void)
    {
    TRACE("CDmAdStore::NewL");
    
    CDmAdStore *self = new (ELeave) CDmAdStore();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

void CDmAdStore::ConstructL(void)
    {
    TRACE("CDmAdStore::ConstructL");

    INIT_EVENT_LOGGER;                    
                
    User::LeaveIfError(iVpnApi.Connect());    
    iVpnAp = CVpnAp::NewL();    
    }
    
CDmAdStore::CDmAdStore()
    {
    }

CDmAdStore::~CDmAdStore()
    {
    TRACE("CDmAdStore::~CDmAdStore");
    
    iVpnApi.Close();
    delete iVpnAp;
    
    RELEASE_EVENT_LOGGER;        
    }
    
TBool CDmAdStore::FindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdStore::FindRtNodeL");
    
    TDmAdUriType uriType = CDmAdRtNodeData::UriTypeL(aUri);
    switch (uriType)
        {
        case EDmAdUriTypeVpnAp:
            return VpnApFindRtNodeL(aLuid, aUri);
        case EDmAdUriTypePolicy:
            return PolicyFindRtNodeL(aLuid, aUri);
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            return EFalse;
        }
    }

void CDmAdStore::LuidListL(const TDesC8& aUri, const TDesC8& /*aLuid*/, RPointerArray<HBufC8>& aLuidList)
    {        
    TDmAdUriType uriType = CDmAdRtNodeData::UriTypeL(aUri);
    switch (uriType)
        {
        case EDmAdUriTypeVpnAp:
            VpnApLuidListL(aLuidList);
            break;
        case EDmAdUriTypePolicy:
            PolicyLuidListL(aLuidList);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }
    
void CDmAdStore::FetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::FetchRtNodeL");
        
    TDmAdUriType uriType = aRtNode.Data()->UriType();
    switch (uriType)
        {
        case EDmAdUriTypeVpnAp:
            VpnApFetchRtNodeL(aRtNode);
            break;
        case EDmAdUriTypePolicy:
            PolicyFetchRtNodeL(aRtNode);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }
    
void CDmAdStore::SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes)
    {
    TRACE("CDmAdStore::SaveTopLevelRtNodeL");
    
    TDmAdUriType uriType = aRtNode.Data()->UriType();
    switch (uriType)
        {
        case EDmAdUriTypeVpnAp:
            VpnApSaveTopLevelRtNodeL(aRtNode, aChildRtNodes);
            break;
        case EDmAdUriTypePolicy:
            PolicySaveTopLevelRtNodeL(aRtNode, aChildRtNodes);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

void CDmAdStore::SaveChildLevelRtNodeL(CDmAdRtNode& /*aRtNode*/)
    {
    TRACE("CDmAdStore::SaveChildLevelRtNodeL");    
    User::Leave(KErrGeneral);
    }


void CDmAdStore::DeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdStore::DeleteRtNodeL");
    
    TDmAdUriType uriType = CDmAdRtNodeData::UriTypeL(aUri);
    switch (uriType)
        {
        case EDmAdUriTypeVpnAp:
            VpnApDeleteRtNodeL(aLuid);
            break;
        case EDmAdUriTypePolicy:
            PolicyDeleteRtNodeL(aLuid);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

//=========================================================================
//=========================================================================

TBool CDmAdStore::VpnApFindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdStore::VpnApFindRtNodeL");
    
    (void)aUri;
    TBool ret = EFalse;
    if (aLuid.Length() > 0)
        {
        if (iVpnAp->FindVpnApL(TDmAdUtil::DesToInt(aLuid)))
            {
            ret = ETrue;
            }
        }
    return ret;
    }

void CDmAdStore::VpnApLuidListL(RPointerArray<HBufC8>& aLuidList)
    {
    
    TRACE("CDmAdStore::VpnApLuidListL");

    RArray<TVpnApCommsId> idArray;
    CleanupClosePushL(idArray);
    iVpnAp->ListVpnApsL(idArray);
    
    for (TInt i=0; i<idArray.Count(); ++i)
        {
        HBufC8* luid = TDmAdUtil::IntToDes8LC(idArray[i]);
        aLuidList.AppendL(luid);
        CleanupStack::Pop(); //luid
        }
    
    CleanupStack::PopAndDestroy(); //idArray
    }
    
void CDmAdStore::VpnApFetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::VpnApFetchRtNodeL");
    
    TPtrC8 luid(aRtNode.Luid());
    if (luid.Length() == 0)
        {
        DEBUG_LOG(_L("Luid length is 0"));       
        User::Leave(KErrNotFound);
        }

    CDmAdVpnApData* data = aRtNode.Data()->VpnApData();
    TVpnApParms* vpnApParms = new (ELeave) TVpnApParms();
    CleanupStack::PushL(vpnApParms);
    iVpnAp->GetVpnApL(TDmAdUtil::DesToInt(luid), *vpnApParms);
    data->RestoreL(*vpnApParms);
    CleanupStack::PopAndDestroy(); //vpnApParms
    }
    
void CDmAdStore::VpnApSaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes)
    {
    TRACE("CDmAdStore::VpnApSaveTopLevelRtNodeL");
    
    if (aChildRtNodes.Count() > 0)
        {
        DEBUG_LOG(_L("aChildRtNodes.Count() > 0"));        
        User::Leave(KErrGeneral);
        }

    if (aRtNode.IsJustFetched())
        {
        return;
        }
    
    CDmAdVpnApData* data = aRtNode.Data()->VpnApData();

    TVpnApParms* vpnApParms = new (ELeave) TVpnApParms();
    CleanupStack::PushL(vpnApParms);
    data->ToStoreL(*vpnApParms);

    
    //Check the policy existence.
    //It is possible to set the policyId to be empty, but
    //if the policyId is set it has to be found.
    if ( vpnApParms->iPolicyId.Length() > 0)
        {        
        CArrayFixFlat<TVpnPolicyInfo>* policyInfoList = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(2);
        CleanupStack::PushL(policyInfoList);
        User::LeaveIfError(iVpnApi.GetPolicyInfoList(policyInfoList));   
        
        TBool policyFound = EFalse;
        for (TInt i = 0; i < policyInfoList->Count(); ++i)
            {
            const TVpnPolicyInfo& info = (*policyInfoList)[i];
            if (info.iId.Compare(vpnApParms->iPolicyId) == 0) 
                {
                policyFound = ETrue;
                break;
                }
            }                     
        CleanupStack::PopAndDestroy(policyInfoList);
        if (!policyFound)
            {
            DEBUG_LOG1(_L("Policy with id %S not found"), &(vpnApParms->iPolicyId));
            User::Leave(KErrGeneral);
            }
        }

    if (aRtNode.IsSomeLeafAddedToRtNode())
        {                
        
        TUint32 id = iVpnAp->AddVpnApL(*vpnApParms);
        
        TBuf<KVpnApSvrMaxFieldLength> buf;
        buf.Copy(vpnApParms->iPolicyId);
        
        if (vpnApParms->iRealConnRefType == TVpnApParms::EIapRealConnRef)
            {            
    	    LOG_EVENT(iVpnApi, R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_AP, &buf, 
    	              id, vpnApParms->iRealConnRef);            
            }
        else  //Connection type is SNAP
            {
            __ASSERT_DEBUG(vpnApParms->iRealConnRefType == TVpnApParms::ESnapRealConnRef, User::Invariant());
    	    LOG_EVENT(iVpnApi, R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_SNAP, &buf, 
    	              id, vpnApParms->iRealConnRef);            
            
            }
        
        HBufC8* luid = TDmAdUtil::IntToDes8LC(id);
        aRtNode.SetLuidL(*luid);
        CleanupStack::PopAndDestroy(); //luid
        }
    else
        {
        TPtrC8 luid(aRtNode.Luid());
        iVpnAp->UpdateVpnApL(TDmAdUtil::DesToInt(luid), *vpnApParms);
        }
    
    CleanupStack::PopAndDestroy(); //vpnApParms
    }


void CDmAdStore::VpnApDeleteRtNodeL(const TDesC8& aLuid)
    {
    TRACE("CDmAdStore::VpnApDeleteRtNodeL");    
    
    if (aLuid.Length() == 0)
        {
        DEBUG_LOG(_L("aLuid length is 0"));        
        User::Leave(KErrNotFound);
        }

    iVpnAp->DeleteVpnApL(TDmAdUtil::DesToInt(aLuid));
    }

//=========================================================================
//=========================================================================

TPtrC8 CDmAdStore::PolicyRefToLuid(const TDesC& aPolicyRef)
    {
    TRACE("CDmAdStore::PolicyRefToLuid");
    
    TPtrC8 ret((const TUint8*)aPolicyRef.Ptr(), aPolicyRef.Length() * 2);
    return ret;
    }
    
TPtrC CDmAdStore::LuidToPolicyRef(const TDesC8& aLuid)
    {
    TRACE("CDmAdStore::LuidToPolicyRef");
    
    TPtrC ret((const TUint16*)aLuid.Ptr(), aLuid.Length() / 2);
    return ret;
    }
    
TBool CDmAdStore::PolicyFindRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::PolicyFindRtNodeL");
    
    TBool ret = EFalse;
    if (aLuid.Length() > 0)
        {
        TVpnPolicyDetails* details = new (ELeave) TVpnPolicyDetails();
        if (iVpnApi.GetPolicyDetails(LuidToPolicyRef(aLuid), *details) == KErrNone)
            {
            ret = ETrue;
            }
        delete details;
        }
    return ret;
    }


void CDmAdStore::PolicyLuidListL(RPointerArray<HBufC8>& aLuidList)
    {
    
    TRACE("CDmAdStore::PolicyLuidListL");

    CArrayFixFlat<TVpnPolicyInfo>* policyList;
    policyList = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(2);
    CleanupStack::PushL(policyList);

    User::LeaveIfError(iVpnApi.GetPolicyInfoList(policyList));

    for (TInt i=0; i<policyList->Count(); ++i)
        {
        HBufC8* luidElem = PolicyRefToLuid(policyList->At(i).iId).AllocLC();
        aLuidList.AppendL(luidElem);
        CleanupStack::Pop(); // luidElem
        }
    
    CleanupStack::PopAndDestroy(); //policyList
    }
    
    
void CDmAdStore::PolicyFetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::PolicyFetchRtNodeL");
    
    TPtrC policyRef(LuidToPolicyRef(aRtNode.Luid()));
    
    if (policyRef.Length() == 0)
        {
        DEBUG_LOG(_L("policyRef length is 0"));        
        User::Leave(KErrNotFound);
        }

    CDmAdPolicyData* data = aRtNode.Data()->PolicyData();
    
    TVpnPolicyDetails* details = new (ELeave) TVpnPolicyDetails();
    CleanupStack::PushL(details);
    if (iVpnApi.GetPolicyDetails(policyRef, *details) != KErrNone)
        {
        DEBUG_LOG(_L("Details not found"));
        User::Leave(KErrNotFound);
        }
    data->DetailsRestoreL(*details);
    CleanupStack::PopAndDestroy(); //details

    HBufC8* policyData;
    if (iVpnApi.GetPolicyData(policyRef, policyData) != KErrNone)
        {
        DEBUG_LOG(_L("Policy data not found"));
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(policyData);
    data->ContentRestoreL(*policyData);
    CleanupStack::PopAndDestroy(); //policyData
    }
    
void CDmAdStore::PolicySaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes)
    {
    TRACE("CDmAdStore::PolicySaveTopLevelRtNodeL");
    
    if (aChildRtNodes.Count() > 0)
        {
        DEBUG_LOG(_L("aChildRtNodes.Count() is more than 0"));        
        User::Leave(KErrGeneral);
        }

    if (aRtNode.IsJustFetched())
        {
        return;
        }
        
    CDmAdPolicyData* data = aRtNode.Data()->PolicyData();

    TVpnPolicyDetails* details = new (ELeave) TVpnPolicyDetails();
    CleanupStack::PushL(details);
    data->DetailsToStoreL(*details);

    TInt err;
    if (aRtNode.IsSomeLeafAddedToRtNode())
        {
        if ((err = iVpnApi.AddPolicy(*details, data->ContentToStore())) != KErrNone)
            {
            DEBUG_LOG1(_L("iVpnApi.AddPolicy failed with %d"), err);            
            User::Leave(err);
            }
        aRtNode.SetLuidL(PolicyRefToLuid(details->iId));
        }
    else
        {
        TPtrC policyRef(LuidToPolicyRef(aRtNode.Luid()));
        if (policyRef.Compare(details->iId) != 0)
            {
            DEBUG_LOG(_L("Details Id doesn't match"));            
            User::Leave(KErrGeneral);
            }
        if ((err = iVpnApi.UpdatePolicyDetails(*details)) != KErrNone)
            {
            DEBUG_LOG1(_L("iVpnApi.UpdatePolicyDetails failed with %d"), err);            
            User::Leave(err);
            }
        if ((err = iVpnApi.UpdatePolicyData(details->iId, data->ContentToStore())) != KErrNone)
            {
            DEBUG_LOG1(_L("iVpnApi.UpdatePolicyData failed with %d"), err);            
            User::Leave(err);
            }
        }
    
    CleanupStack::PopAndDestroy(); //details
    }


void CDmAdStore::PolicyDeleteRtNodeL(const TDesC8& aLuid)
    {
    TRACE("CDmAdStore::PolicyDeleteRtNodeL");   

    if (aLuid.Length() == 0)
        {
        DEBUG_LOG(_L("aLuid length is 0"));        
        User::Leave(KErrNotFound);
        }

    User::LeaveIfError(iVpnApi.DeletePolicy(LuidToPolicyRef(aLuid)));  
    }
