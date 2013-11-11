/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Device Management Adapter Engine.
*
*/


#include <utf.h>
#include <sysutil.h>

#include "dmadengine.h"
#include "dmadcommand.h"
#include "dmadutil.h"
#include "vpnlogger.h"

//===================================================================================================



//===================================================================================================

DMAD_EXPORT_C CDmAdEngine* CDmAdEngine::NewL(MDmAdCallBack*           aDmAdCallBack,
                                             MDmAdStoreApi*           aStoreApi,
                                             MDmAdDdfApi*             aDdfApi,
                                             MDmAdRtNodeDataApi*      aRtNodeDataApi)
    {
    TRACE("CDmAdEngine::NewL");
    
    CDmAdEngine* self = NewLC(aDmAdCallBack, aStoreApi, aDdfApi, aRtNodeDataApi);
    CleanupStack::Pop();
    return self;
    }

DMAD_EXPORT_C CDmAdEngine* CDmAdEngine::NewLC(MDmAdCallBack*          aDmAdCallBack,
                                              MDmAdStoreApi*          aStoreApi,
                                              MDmAdDdfApi*            aDdfApi,
                                              MDmAdRtNodeDataApi*     aRtNodeDataApi)
    {    
    CDmAdEngine* self = new (ELeave) CDmAdEngine(aDmAdCallBack, aStoreApi, aDdfApi, aRtNodeDataApi);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
void CDmAdEngine::ConstructL()
    {    
    TRACE("CDmAdEngine::ConstructL");
    iRtNodes = new (ELeave) CArrayPtrFlat<CDmAdRtNode>(4);
    }

CDmAdEngine::CDmAdEngine(MDmAdCallBack*           aDmAdCallBack,
                         MDmAdStoreApi*           aStoreApi,
                         MDmAdDdfApi*             aDdfApi,
                         MDmAdRtNodeDataApi*      aRtNodeDataApi) :
   iCallBack(aDmAdCallBack), iStoreApi(aStoreApi), iDdfApi(aDdfApi), iRtNodeDataApi(aRtNodeDataApi)
    {
    TRACE("CDmAdEngine::CDmAdEngine");
    }

DMAD_EXPORT_C CDmAdEngine::~CDmAdEngine()
    {
    TRACE("CDmAdEngine::~CDmAdEngine");    
    CDmAdRtNode::CleanupOperationDeleteCArrayPtr(iRtNodes);
    }

//===================================================================================================
    
DMAD_EXPORT_C void CDmAdEngine::ChildURIListL(const TDesC8& aUri, const TDesC8& aParentLuid, 
                                              const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, 
                                              TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::ChildURIListL");

    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aParentLuid %S"), &aParentLuid);
    DEBUG_LOG2(_L("aResultsRef = %d, aStatusRef = %d"), aResultsRef, aStatusRef);
    
    
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TRAPD(err, DoChildUriListL(uri, aParentLuid, aPreviousUriSegmentList, aResultsRef, aStatusRef));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        }
    }

DMAD_EXPORT_C void CDmAdEngine::AddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, 
                                               TInt aStatusRef)
    {
    TRACE("CDmAdEngine::AddNodeObjectL");
    
    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aParentLuid %S"), &aParentLuid);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
        
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TRAPD(err, DoAddNodeObjectL(uri, aParentLuid, aStatusRef));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        }
    }

DMAD_EXPORT_C void CDmAdEngine::UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, 
                                                  const TDesC8& aObject, const TDesC8& aType, 
                                                  TInt aStatusRef)
    {
    TRACE("CDmAdEngine::UpdateLeafObjectL");

    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid %S"), &aLuid);
    DEBUG_LOG1(_L8("aType %S"), &aType);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);

    
    DEBUG_LOG(_L("Object:"));    
    DEBUG_LOG_HEX(aObject);
    
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TRAPD(err, DoUpdateLeafObjectL(uri, aLuid, aObject, aType, aStatusRef));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        }
    }

DMAD_EXPORT_C void CDmAdEngine::FetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, 
                                                 const TDesC8& aType, TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::FetchLeafObjectL");
    
    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid %S"), &aLuid);
    DEBUG_LOG1(_L8("aType %S"), &aType);
    DEBUG_LOG2(_L("aResultsRef = %d, aStatusRef = %d"), aResultsRef, aStatusRef);
        
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TRAPD(err, DoFetchLeafObjectL(uri, aLuid, aType, aResultsRef, aStatusRef));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        }
    }

DMAD_EXPORT_C void CDmAdEngine::DeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::DeleteObjectL");
    DEBUG_LOG1(_L8("aUri %S"), &aUri);
    DEBUG_LOG1(_L8("aLuid %S"), &aLuid);
    DEBUG_LOG1(_L("aStatusRef = %d"), aStatusRef);
        
    TPtrC8 uri(TDmAdUtil::RemoveDotSlash(aUri));
    TRAPD(err, DoDeleteObjectL(uri, aLuid, aStatusRef));
    if (err != KErrNone)
        {
        iCallBack->SetStatusL(aStatusRef, err);
        }
    }

DMAD_EXPORT_C void CDmAdEngine::CompleteOutstandingCmdsL()
    {
    TRACE("CDmAdEngine::CompleteOutstandingCmdsL");
    
    TRAPD(err, DoCompleteOutstandingCmdsL());
    if (err != KErrNone)
        {
        iRtNodes->ResetAndDestroy();
        }
    }
    
//===================================================================================================
    
void CDmAdEngine::DoChildUriListL(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::DoChildUriListL");
    
    CBufBase* currentList;
    iDdfApi->BuildChildUriListLC(aUri, aParentLuid, aPreviousUriSegmentList, currentList);
    iCallBack->SetResultsL(aResultsRef, *currentList, KNullDesC8);
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    CleanupStack::PopAndDestroy(); //currentList
    }

    
void CDmAdEngine::DoAddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::DoAddNodeObjectL");
    
    if (!iDdfApi->IsNodeRtNodeL(aUri))
        {
        iDdfApi->NotRtNodeAddNodeObjectL(aUri, aParentLuid, aStatusRef);
        return;
        }
    
    if (FindRtNodeInStoreL(aParentLuid, aUri))
        {
        iCallBack->SetStatusL(aStatusRef, KErrAlreadyExists);
        return;
        }

    CDmAdRtNode* rtNode = FindRtNodeInBuffer(aUri);
    if (rtNode != NULL)
        {
        iCallBack->SetStatusL(aStatusRef, KErrAlreadyExists);
        return;
        }

    if (SysUtil::FFSSpaceBelowCriticalLevelL(0, 0))
        {
        iCallBack->SetStatusL(aStatusRef, KErrDiskFull);
        return;
        }
    
    rtNode = CDmAdRtNode::NewL(aUri, aParentLuid, iCallBack, iRtNodeDataApi);
    CleanupStack::PushL(rtNode);

    iRtNodeDataApi->SetDefaultSettingsL(rtNode->Data(), aUri);
    rtNode->SetSomeLeafAddedToRtNode(ETrue);
            
    CDmAdCommand* command = CDmAdCommand::NewLC(aUri, aStatusRef, EFalse);
    rtNode->AppendCommandL(command);
    CleanupStack::Pop(); //command
    
    iRtNodes->AppendL(rtNode);
    CleanupStack::Pop(); // rtNode

    }
    
void CDmAdEngine::DoUpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::DoUpdateLeafObjectL");
    
    CDmAdRtNode* rtNode = NULL;

    if (!iDdfApi->IsLeafUnderRtNodeL(aUri))
        {
        iDdfApi->NotRtNodeUpdateLeafObjectL(aUri, aLuid, aObject, aType, aStatusRef);
        return;
        }
    
    TPtrC8 rtNodeUri(iDdfApi->RtNodeUriForLeafL(aUri));
    if (!FindRtNodeInStoreL(aLuid, rtNodeUri))
        {
        rtNode = FindRtNodeInBuffer(rtNodeUri);
        if (rtNode == NULL)
            {
            iCallBack->SetStatusL(aStatusRef, KErrNotFound);
            return;
            }
        rtNode->AddLeafObjectL(aUri, aObject, aStatusRef);
        return;
        }

    rtNode = RestoreRtNodeInBufferIfNotDoneL(rtNodeUri, aLuid);
    rtNode->UpdateLeafObjectL(aUri, aObject, aStatusRef);
    }

    
void CDmAdEngine::DoFetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::DoFetchLeafObjectL");
    
    CBufBase* object = 0;
    TInt status = DoFetchLeafObjectL(aUri, aLuid, aType, object);
    CleanupStack::PushL(object);
    if (status == KErrNone)
        {
        iCallBack->SetResultsL(aResultsRef, *object, aType);
        }
    iCallBack->SetStatusL(aStatusRef, status);
    CleanupStack::PopAndDestroy(); //object
    }

TInt CDmAdEngine::DoFetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, CBufBase*& aObject)
    {
    TRACE("CDmAdEngine::DoFetchLeafObjectL");
    
    TInt status = KErrNone;
    aObject = 0;
    if (!iDdfApi->IsLeafUnderRtNodeL(aUri))
        {
        status = iDdfApi->NotRtNodeFetchLeafObjectLC(aUri, aLuid, aType, aObject);
        CleanupStack::Pop(); //aObject
        return status;
        }
    
    if (aLuid.Length() == 0)
        {
        aObject = CBufFlat::NewL(32);
        status = KErrNotFound;
        return status;
        }

    TPtrC8 rtNodeUri(TDmAdUtil::RemoveLastUriSeg(aUri));
    CDmAdRtNode* rtNode = RestoreRtNodeInBufferIfNotDoneL(rtNodeUri, aLuid);
    iRtNodeDataApi->FetchLeafObjectLC(rtNode->Data(), aUri, aLuid, aObject);
    CleanupStack::Pop(); //object
    return status;
    }

    
void CDmAdEngine::DoDeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef)
    {
    TRACE("CDmAdEngine::DoDeleteObjectL");
    
    if (aLuid.Length() == 0)
        {
        if (iDdfApi->IsNodeRtNodeL(aUri))
            {
            iCallBack->SetStatusL(aStatusRef, KErrNotFound);
            return;
            }
        iDdfApi->NotRtNodeDeleteObjectL(aUri, aLuid, aStatusRef);
        return;
        }

    /*
    if (IsLeaf(aUri))
        {
        DMADERR(DmAdErr::Printf(_L("*** CDmAdEngine::DoDeleteObjectL: %d (line=%d)\n"), KDmAdErr1, __LINE__));
        User::Leave(KErrGeneral);
        }
    */
    
    if (!iDdfApi->IsTopLevelRtNode(aUri))
        {
        HBufC8* parentRtNodeLuid = ParentRtNodeLuidForRtNodeLC(aUri);
        if (parentRtNodeLuid->Compare(aLuid) == 0)
            {
            DEBUG_LOG(_L("Not found 1"));            
            User::Leave(KErrNotFound);
            }
        DEBUG_LOG(_L("Not found 2"));
        User::Leave(KErrNotFound);
        CleanupStack::PopAndDestroy(); //parentRtNodeLuid
        }
    
    iStoreApi->DeleteRtNodeL(aLuid, aUri);
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    }


void CDmAdEngine::DoCompleteOutstandingCmdsL()
    {
    TRACE("CDmAdEngine::DoCompleteOutstandingCmdsL");
    
    SaveRtNodesL();
    }

//===================================================================================================

CDmAdRtNode* CDmAdEngine::FindRtNodeInBuffer(const TDesC8& aUri)
    {
    TRACE("CDmAdEngine::FindRtNodeInBuffer");
    
    CDmAdRtNode* rtNode = NULL;
    TInt countRtNodes = iRtNodes->Count();
    for (TInt i=0; i < countRtNodes; i++)
        {
        if (iRtNodes->At(i)->Uri().Compare(aUri) == 0)
            {
            rtNode = iRtNodes->At(i);
            break;
            }
        }
    return rtNode;
    }

CDmAdRtNode* CDmAdEngine::FindRtNodeInBufferByLuid(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdEngine::FindRtNodeInBufferByLuid");
    
    CDmAdRtNode* rtNode = NULL;
    TInt countRtNodes = iRtNodes->Count();
    for (TInt i=0; i < countRtNodes; i++)
        {
        if (iRtNodes->At(i)->Luid().Compare(aLuid) == 0 &&
            iRtNodes->At(i)->AreUriTypesSame(aUri))
            {
            rtNode = iRtNodes->At(i);
            break;
            }
        }
    return rtNode;
    }

CDmAdRtNode* CDmAdEngine::RestoreRtNodeInBufferIfNotDoneL(const TDesC8& aUri, const TDesC8& aLuid)
    {
    TRACE("CDmAdEngine::RestoreRtNodeInBufferIfNotDoneL");
    
    CDmAdRtNode* rtNode = FindRtNodeInBufferByLuid(aLuid, aUri);
    if (rtNode == NULL)
        {
        rtNode = FetchRtNodeInBufferL(aUri, aLuid);
        }
    return rtNode;
    }

CDmAdRtNode* CDmAdEngine::FetchRtNodeInBufferL(const TDesC8& aUri, const TDesC8& aLuid)
    {
    TRACE("CDmAdEngine::FetchRtNodeInBufferL");
    
    CDmAdRtNode* rtNode = CDmAdRtNode::NewL(aUri, aLuid, iCallBack, iRtNodeDataApi);
    CleanupStack::PushL(rtNode);

    iStoreApi->FetchRtNodeL(*rtNode);

    iRtNodes->AppendL(rtNode);
    CleanupStack::Pop(); //rtNode
    return rtNode;
    }

void CDmAdEngine::CompleteCommandsL(CDmAdRtNode& aRtNode, TInt aStatus)
    {
    TRACE("CDmAdEngine::CompleteCommandsL");
    
    TInt countCommands = aRtNode.Commands()->Count();
    for (TInt cmd=0; cmd < countCommands; cmd++)
        {
        CDmAdCommand* command = aRtNode.Commands()->At(cmd);
        if (command->Status() == KErrNone)
            {
            if (!command->IsLeaf())
                {
                if (aStatus == MSmlDmAdapter::EOk)
                    {
                    iCallBack->SetMappingL(aRtNode.Uri(), aRtNode.Luid());
                    }
                }
            iCallBack->SetStatusL(command->StatusRef(), aStatus);
            }
        else
            {
            iCallBack->SetStatusL(command->StatusRef(), command->Status());
            }
        }
    }

void CDmAdEngine::CompleteCommandsL(CArrayPtr<CDmAdRtNode>& aRtNodes, TInt aStatus)
    {
    TRACE("CDmAdEngine::CompleteCommandsL");
    
    for (TInt i=0; i < aRtNodes.Count(); i++)
        {
        CDmAdRtNode* rtNode = aRtNodes.At(i);
        CompleteCommandsL(*rtNode, aStatus);
        }
    }
    
void CDmAdEngine::BuildTopLevelRtNodesListLC(CArrayPtr<CDmAdRtNode>*     aRtNodes,
                                             CArrayPtr<CDmAdRtNode>*&    aTopLevelRtNodes)
    {
    CArrayPtr<CDmAdRtNode>* rtNodesList;
    rtNodesList = new (ELeave) CArrayPtrFlat<CDmAdRtNode>(4);
    CleanupStack::PushL(TCleanupItem(CDmAdRtNode::CleanupOperationDeleteCArrayPtr,
                                     rtNodesList));
    
    for (TInt i=0; i < aRtNodes->Count(); i++)
        {
        CDmAdRtNode* rtNode = aRtNodes->At(i);
        if (iDdfApi->IsTopLevelRtNode(rtNode->Uri()))
            {
            rtNodesList->AppendL(rtNode);
            aRtNodes->Delete(i);
            i--;
            }
        }
    
    aTopLevelRtNodes = rtNodesList;
    }
    
void CDmAdEngine::BuildChildRtNodesListLC(CArrayPtr<CDmAdRtNode>*     aRtNodes,
                                           const TDesC8&               aSomeParentUri,
                                           CArrayPtr<CDmAdRtNode>*&    aChildRtNodes)
    {
    CArrayPtr<CDmAdRtNode>* rtNodesList;
    rtNodesList = new (ELeave) CArrayPtrFlat<CDmAdRtNode>(4);
    CleanupStack::PushL(TCleanupItem(CDmAdRtNode::CleanupOperationDeleteCArrayPtr,
                                     rtNodesList));
    
    for (TInt i=0; i < aRtNodes->Count(); i++)
        {
        CDmAdRtNode* rtNode = aRtNodes->At(i);
        if (rtNode->IsSomeChild(aSomeParentUri))
            {
            rtNodesList->AppendL(rtNode);
            aRtNodes->Delete(i);
            i--;
            }
        }
    
    aChildRtNodes = rtNodesList;
    }
    
void CDmAdEngine::SaveRtNodesL(void)
    {
    TRACE("CDmAdEngine::SaveRtNodesL");
    
    CArrayPtr<CDmAdRtNode>* topLevelRtNodes;
    BuildTopLevelRtNodesListLC(iRtNodes, topLevelRtNodes);

    TInt countRtNodes;
    countRtNodes = topLevelRtNodes->Count();
    TInt i;
    for (i=0; i < countRtNodes; i++)
        {
        CDmAdRtNode* rtNode = topLevelRtNodes->At(i);
        CArrayPtr<CDmAdRtNode>* childRtNodes;
        BuildChildRtNodesListLC(iRtNodes, rtNode->Uri(), childRtNodes);
        
        TRAPD(err, iStoreApi->SaveTopLevelRtNodeL(*rtNode, *childRtNodes));
        CompleteCommandsL(*rtNode, err);
        CompleteCommandsL(*childRtNodes, err);
        
        CleanupStack::PopAndDestroy(); //childRtNodes
        }
    CleanupStack::PopAndDestroy(); //topLevelRtNodes

    //child rtNodes without top level RtNode
    countRtNodes = iRtNodes->Count();
    for (i=0; i < countRtNodes; i++)
        {
        CDmAdRtNode* rtNode = iRtNodes->At(i);
        TInt err = KErrNone;
        if (!rtNode->IsJustFetched())
            {
            TRAP(err, iStoreApi->SaveChildLevelRtNodeL(*rtNode));
            }
        CompleteCommandsL(*rtNode, err);
        }
    iRtNodes->ResetAndDestroy();
    }

TBool CDmAdEngine::FindRtNodeInStoreL(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdEngine::FindRtNodeInStoreL");
    
    TBool ret = EFalse;

    if (iDdfApi->IsTopLevelRtNode(aUri))
        {
        ret = iStoreApi->FindRtNodeL(aLuid, aUri);
        }
    else
        {
        HBufC8* parentRtNodeLuid = ParentRtNodeLuidForRtNodeLC(aUri);
        if (parentRtNodeLuid->Compare(aLuid) == 0)
            {
            ret = EFalse;
            }
        else
            {
            ret = iStoreApi->FindRtNodeL(aLuid, aUri);
            }
        CleanupStack::PopAndDestroy(); //parentRtNodeLuid
        }

    return ret;
    }

HBufC8* CDmAdEngine::ParentRtNodeLuidForRtNodeLC(const TDesC8& aUri)
    {
    HBufC8* parentRtNodeUri = iDdfApi->ParentRtNodeUriForRtNodeLC(aUri);
    HBufC8* parentRtNodeLuid = iCallBack->GetLuidAllocL(*parentRtNodeUri);
    CleanupStack::PopAndDestroy(); // parentRtNodeUri
    CleanupStack::PushL(parentRtNodeLuid);
    return parentRtNodeLuid;
    }
