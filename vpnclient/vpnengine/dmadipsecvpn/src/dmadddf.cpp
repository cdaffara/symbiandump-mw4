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
* Description: Implementation of CDmAdDdf.
*
*/


#include <utf.h>

#include "dmadddf.h"
#include "dmadeventlog.h"
#include "dmadutil.h"
#include "vpnlogger.h"

CDmAdDdf* CDmAdDdf::NewL(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore)
    {
    TRACE("CDmAdDdf::NewL");
    
    CDmAdDdf* self = NewLC(aDmAdCallBack, aStore);
    CleanupStack::Pop();
    return self;
    }

CDmAdDdf* CDmAdDdf::NewLC(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore)
    {
    CDmAdDdf* self = new (ELeave) CDmAdDdf(aDmAdCallBack, aStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
void CDmAdDdf::ConstructL(void)
    {
    TRACE("CDmAdDdf::ConstructL");
    }

CDmAdDdf::CDmAdDdf(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore) :
   iCallBack(aDmAdCallBack), iStore(aStore)
    {
    TRACE("CDmAdDdf::CDmAdDdf");
    }

CDmAdDdf::~CDmAdDdf()
    {
    TRACE("CDmAdDdf::~CDmAdDdf");
    }
    
//===================================================================================================
    
void CDmAdDdf::BuildDDFVersionL(CBufBase& aDDFVersion)
    {
    TRACE("CDmAdDdf::BuildDDFVersionL");
    aDDFVersion.InsertL(0, KDmAdDdfVersion);
    }

void CDmAdDdf::BuildDDFStructureL(MSmlDmDDFObject& aDDF)
    {
    TRACE("CDmAdDdf::BuildDDFStructureL");
    
    TSmlDmAccessTypes accessTypesExec;
    accessTypesExec.SetReplace(); // accessTypesExec.SetExec();

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesAdd;
    accessTypesAdd.SetAdd();
    
    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();
    
    TSmlDmAccessTypes accessTypesGetDelete;
    accessTypesGetDelete.SetGet();
    accessTypesGetDelete.SetDelete();
    
    TSmlDmAccessTypes accessTypesAll;
    accessTypesAll.SetGet();
    accessTypesAll.SetAdd();
    accessTypesAll.SetDelete();
    accessTypesAll.SetReplace();
    
    TSmlDmAccessTypes accessTypesNoDelete;
    accessTypesNoDelete.SetGet();
    accessTypesNoDelete.SetAdd();
    accessTypesNoDelete.SetReplace();

    MSmlDmDDFObject* obj;
    MSmlDmDDFObject* objNokiaIpSecVpn;
    MSmlDmDDFObject* objIpSecVpnGeneral;
    MSmlDmDDFObject* objPolicy;
    MSmlDmDDFObject* objPolicyX;
    MSmlDmDDFObject* objAp;
    MSmlDmDDFObject* objApX;
    MSmlDmDDFObject* objApXPolicyRef;

//----------------------------------------------------------------
    
    objNokiaIpSecVpn = &TDmAdUtil::AddChildObjectL(aDDF, KDmAdNodeNokiaIpSecVpn);
    TDmAdUtil::FillNodeInfoL(*objNokiaIpSecVpn,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeNokiaIpSecVpn,
                             ETrue,
                             KDmAdNokiaIpSecVpnRootTypeProperty);
    
//----------------------------------------------------------------
    
    objIpSecVpnGeneral = &TDmAdUtil::AddChildObjectL(*objNokiaIpSecVpn, KDmAdNodeGeneral);
    TDmAdUtil::FillNodeInfoL(*objIpSecVpnGeneral,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeGeneral,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objIpSecVpnGeneral, KDmAdLeafClientVersion);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafClientVersion,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objIpSecVpnGeneral, KDmAdLeafEventLog);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetDelete,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafEventLog,
                             EFalse);
    
//----------------------------------------------------------------
    
    objPolicy = &TDmAdUtil::AddChildObjectL(*objNokiaIpSecVpn, KDmAdNodePolicy);
    TDmAdUtil::FillNodeInfoL(*objPolicy,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodePolicy,
                             EFalse,
                             KNullDesC8);

    
    objPolicyX = &TDmAdUtil::AddChildObjectL(*objPolicy, KDmAdNodeRt);
    TDmAdUtil::FillNodeInfoL(*objPolicyX,
                             accessTypesAll,
                             MSmlDmDDFObject::EZeroOrMore,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeRt,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafName);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafName,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafId);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafId,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafVersion);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafVersion,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafDescription);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafDescription,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafIssuer);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafIssuer,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafContact);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafContact,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPolicyX, KDmAdLeafContent);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafContent,
                             EFalse);
    
    
//----------------------------------------------------------------
    
    objAp = &TDmAdUtil::AddChildObjectL(*objNokiaIpSecVpn, KDmAdNodeAp);
    TDmAdUtil::FillNodeInfoL(*objAp,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeAp,
                             EFalse,
                             KNullDesC8);

    
    objApX = &TDmAdUtil::AddChildObjectL(*objAp, KDmAdNodeRt);
    TDmAdUtil::FillNodeInfoL(*objApX,
                             accessTypesAll,
                             MSmlDmDDFObject::EZeroOrMore,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeRt,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objApX, KDmAdLeafName);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafName,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objApX, KDmAdLeafThisRef);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafThisRef,
                             EFalse);
    
    objApXPolicyRef = &TDmAdUtil::AddChildObjectL(*objApX, KDmAdNodePolicyRef);
    TDmAdUtil::FillNodeInfoL(*objApXPolicyRef,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodePolicyRef,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objApXPolicyRef, KDmAdLeafId);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafId,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objApXPolicyRef, KDmAdLeafUri);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafUri,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objApX, KDmAdLeafConRef);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafConRef,
                             EFalse);
    
    }
    
//---------------------------------------------------------------------------------------
    
void CDmAdDdf::NotRtNodeAddNodeObjectL(const TDesC8& /*aUri*/, 
                                       const TDesC8& /*aParentLuid*/, 
                                       TInt aStatusRef)
    {
    TRACE("CDmAdDdf::NotRtNodeAddNodeObjectL");
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    }

void CDmAdDdf::NotRtNodeUpdateLeafObjectL(const TDesC8& /*aUri*/, 
                                          const TDesC8& /*aLuid*/, 
                                          const TDesC8& /*aObject*/, 
                                          const TDesC8& /*aType*/, 
                                          TInt aStatusRef)
    {
    TRACE("CDmAdDdf::NotRtNodeUpdateLeafObjectL");
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    }

TInt CDmAdDdf::NotRtNodeFetchLeafObjectLC(const TDesC8& aUri, 
                                          const TDesC8& /*aLuid*/, 
                                          const TDesC8& /*aType*/, 
                                          CBufBase*& aObject)
    {
    TInt status = KErrNotFound;
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafClientVersion) == 0)
        {
        HBufC8* version = CnvUtfConverter::ConvertFromUnicodeToUtf8L(KVersion);
        CleanupStack::PushL(version);
        object->InsertL(0, *version);
        CleanupStack::PopAndDestroy(); //version
        status = KErrNone;
        }
    else if (lastSeg.Compare(KDmAdLeafEventLog) == 0)
        {
        TDmAdEventLog::EventLogL(*object);
        status = KErrNone;
        }
    aObject = object;
    return status;
    }

void CDmAdDdf::NotRtNodeDeleteObjectL(const TDesC8& aUri, 
                                      const TDesC8& /*aLuid*/, 
                                      TInt aStatusRef)
    {
    TRACE("CDmAdDdf::NotRtNodeDeleteObjectL");
    
    TInt status = KErrNone; //KErrNotFound;
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafEventLog) == 0)
        {
        TDmAdEventLog::DeleteLogL();
        status = KErrNone;
        }
    iCallBack->SetStatusL(aStatusRef, status);
    }
    
//---------------------------------------------------------------------------------------
    
TBool CDmAdDdf::IsNodeRtNodeL(const TDesC8& aUri)
    {
    TRACE("CDmAdDdf::IsNodeRtNodeL");
    
    TBool ret = EFalse;
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);

    if (lastSeg.Compare(KDmAdLeafEventLog) == 0) // for DeleteObjectL: In this case this method is called also when URI is leaf (not always node)
        {
        return EFalse;
        }
    
    if (lastSeg.Compare(KDmAdNodeNokiaIpSecVpn) == 0 ||
        lastSeg.Compare(KDmAdNodeGeneral) == 0 ||
        lastSeg.Compare(KDmAdNodePolicy) == 0 ||
        lastSeg.Compare(KDmAdNodeAp) == 0 ||
        lastSeg.Compare(KDmAdNodePolicyRef) == 0)
        {
        ret = EFalse;
        }
    else
        {
        if (TDmAdUtil::NumOfURISegs(aUri) < 2)
            {
            DEBUG_LOG(_L("Node not found"));            
            User::Leave(KErrNotFound);
            }
        TPtrC8 prevSeg = TDmAdUtil::LastUriSeg(TDmAdUtil::RemoveLastUriSeg(aUri));
        if (prevSeg.Compare(KDmAdNodePolicy) == 0 ||
            prevSeg.Compare(KDmAdNodeAp) == 0)
            {
            ret = ETrue;
            }
        else
            {
            DEBUG_LOG(_L("Node not found"));
            User::Leave(KErrNotFound);
            }
        }
    return ret;
    }

TBool CDmAdDdf::IsLeafUnderRtNodeL(const TDesC8& aUri)
    {
    TRACE("CDmAdDdf::IsLeafUnderRtNodeL");
    
    TBool ret = EFalse;
    TPtrC8 nodeUri(TDmAdUtil::RemoveLastUriSeg(aUri));
    ret = IsNodeRtNodeL(nodeUri);
    if (!ret)
        {
        TPtrC8 nodeUriSeg = TDmAdUtil::LastUriSeg(nodeUri);
        if (nodeUriSeg.Compare(KDmAdNodePolicyRef) == 0)
            {
            ret = ETrue;
            }
        }
    return ret;
    }

TPtrC8 CDmAdDdf::RtNodeUriForLeafL(const TDesC8& aLeafUri)
    {
    TRACE("CDmAdDdf::RtNodeUriForLeafL");
    
    TPtrC8 ret;
    TPtrC8 nodeUri(TDmAdUtil::RemoveLastUriSeg(aLeafUri));
    TPtrC8 nodeUriSeg(TDmAdUtil::LastUriSeg(nodeUri));
    TPtrC8 prevNodeUri(TDmAdUtil::RemoveLastUriSeg(nodeUri));
    TPtrC8 prevNodeUriSeg(TDmAdUtil::LastUriSeg(prevNodeUri));
    
    if (prevNodeUriSeg.Compare(KDmAdNodePolicy) == 0 ||
        prevNodeUriSeg.Compare(KDmAdNodeAp) == 0)
        {
        ret.Set(nodeUri);
        }
    else if (nodeUriSeg.Compare(KDmAdNodePolicyRef) == 0)
        {
        ret.Set(prevNodeUri);
        }
    else
        {
        DEBUG_LOG(_L("Node not found"));
        User::Leave(KErrNotFound);
        }
    return ret;
    }

TBool CDmAdDdf::IsTopLevelRtNode(const TDesC8& aUri)
    {
    TRACE("CDmAdDdf::IsTopLevelRtNode");
    
    (void)aUri;
    return ETrue;
    }

HBufC8* CDmAdDdf::ParentRtNodeUriForRtNodeLC(const TDesC8& /*aUri*/)
    {
    DEBUG_LOG(_L("CDmAdDdf::ParentRtNodeUriForRtNodeLC method not implemented"));
    User::Leave(KErrGeneral);
    return NULL;
    }

//---------------------------------------------------------------------------------------
    
void CDmAdDdf::BuildChildUriListLC(const TDesC8& aUri, 
                                   const TDesC8& aParentLuid, 
                                   const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, 
                                   CBufBase*& aCurrentList)
    {
    CBufBase* currentList = CBufFlat::NewL(128);
    CleanupStack::PushL(currentList);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdNodeNokiaIpSecVpn) == 0)
        {
        currentList->InsertL(0, KDmAdListOfNokiaIpSecVpnChildren);
        }
    else if (lastSeg.Compare(KDmAdNodeGeneral) == 0)
        {
        currentList->InsertL(0, KDmAdListOfIpSecVpnGeneralChildren);
        }
    else if (lastSeg.Compare(KDmAdNodePolicyRef) == 0)
        {
        if (!iStore->FindRtNodeL(aParentLuid, aUri))
            {
            DEBUG_LOG(_L("Node not found"));            
            User::Leave(KErrNotFound);
            }
        currentList->InsertL(0, KDmAdListOfPolicyRefChildren);
        }
    else if (lastSeg.Compare(KDmAdNodePolicy) == 0 ||
             lastSeg.Compare(KDmAdNodeAp) == 0)
        {
        TDmAdUtil::BuildRtNodeChildUriListL(iCallBack, iStore, aUri, aParentLuid, aPreviousUriSegmentList, *currentList);
        }
    else
        {
        TPtrC8 prevSeg = TDmAdUtil::LastUriSeg(TDmAdUtil::RemoveLastUriSeg(aUri));
        if (prevSeg.Compare(KDmAdNodePolicy) == 0 ||
            prevSeg.Compare(KDmAdNodeAp) == 0)
            {
            if (!iStore->FindRtNodeL(aParentLuid, aUri))
                {
                DEBUG_LOG(_L("Node not found"));       
                User::Leave(KErrNotFound);
                }
            
            if (prevSeg.Compare(KDmAdNodePolicy) == 0)
                {
                currentList->InsertL(0, KDmAdListOfPolicyXChildren);
                }
            else if (prevSeg.Compare(KDmAdNodeAp) == 0)
                {
                currentList->InsertL(0, KDmAdListOfApXChildren);
                }
            }
        else
            {
            DEBUG_LOG(_L("Node not found"));       
            User::Leave(KErrNotFound);
            }
        }

    aCurrentList = currentList;
    }
