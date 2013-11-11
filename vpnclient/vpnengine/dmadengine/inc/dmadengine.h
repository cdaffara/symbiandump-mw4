/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Engine
*
*/



#ifndef __DMADENGINE_H__
#define __DMADENGINE_H__

#include <smldmadapter.h>
#include "dmadengineexportdef.h"
#include "dmadcallback.h"
#include "dmadstoreapi.h"
#include "dmadddfapi.h"
#include "dmadrtnodedataapi.h"
#include "DmAdRtNode.h"

//------------------------------------------------------------------------------------------------
// CDmAdEngine
//------------------------------------------------------------------------------------------------
class CDmAdEngine : public CBase
    {
public:
    DMAD_IMPORT_C static CDmAdEngine* NewL(MDmAdCallBack*           aDmAdCallBack,
                                           MDmAdStoreApi*           aStoreApi,
                                           MDmAdDdfApi*             aDdfApi,
                                           MDmAdRtNodeDataApi*      aRtNodeDataApi);
                                           
    DMAD_IMPORT_C static CDmAdEngine* NewLC(MDmAdCallBack*          aDmAdCallBack,
                                            MDmAdStoreApi*          aStoreApi,
                                            MDmAdDdfApi*            aDdfApi,
                                            MDmAdRtNodeDataApi*     aRtNodeDataApi);
    DMAD_IMPORT_C ~CDmAdEngine();

    DMAD_IMPORT_C void ChildURIListL(const TDesC8& aUri, const TDesC8& aLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef);
    DMAD_IMPORT_C void AddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, const TInt aStatusRef);
    DMAD_IMPORT_C void UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef);
    DMAD_IMPORT_C void FetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef);
    DMAD_IMPORT_C void DeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef);
    DMAD_IMPORT_C void CompleteOutstandingCmdsL();
    
    DMAD_IMPORT_C void ChildURIListL(const TDesC& aUri, const TDesC& aLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, const TInt aResultsRef, const TInt aStatusRef);
    DMAD_IMPORT_C void AddNodeObjectL(const TDesC& aUri, const TDesC& aParentLuid, const TInt aStatusRef);
    DMAD_IMPORT_C void AddLeafObjectL(const TDesC& aUri, const TDesC& aParentLuid, const TDesC8& aObject, const TDesC& aType, const TInt aStatusRef);
    DMAD_IMPORT_C void UpdateLeafObjectL(const TDesC& aUri, const TDesC& aLuid, const TDesC8& aObject, const TDesC& aType, const TInt aStatusRef);
    DMAD_IMPORT_C void FetchLeafObjectL(const TDesC& aUri, const TDesC& aLuid, const TDesC& aType, const TInt aResultsRef, const TInt aStatusRef);
    DMAD_IMPORT_C void DeleteObjectL(const TDesC& aUri, const TDesC& aLuid, const TInt aStatusRef);
    DMAD_IMPORT_C void EndMessageL();

    DMAD_IMPORT_C void UpdateLeafObjectL(const TDesC8& aUri,
                                         const TDesC8& aLuid,
                                         RWriteStream*& aStream,
                                         const TDesC8& aType,
                                         TInt aStatusRef);
    DMAD_IMPORT_C void FetchLeafObjectSizeL(const TDesC8& aUri,
                                            const TDesC8& aLuid,
                                            const TDesC8& aType,
                                            TInt aResultsRef,
                                            TInt aStatusRef);
    DMAD_IMPORT_C void ExecuteCommandL(const TDesC8& aUri,
                                       const TDesC8& aLuid,
                                       const TDesC8& aArgument,
                                       const TDesC8& aType,
                                       TInt aStatusRef);
    DMAD_IMPORT_C void ExecuteCommandL(const TDesC8& aUri,
                                       const TDesC8& aLuid,
                                       RWriteStream*& aStream,
                                       const TDesC8& aType,
                                       TInt aStatusRef);
    DMAD_IMPORT_C void CopyCommandL(const TDesC8& aTargetUri,
                                    const TDesC8& aTargetLuid,
                                    const TDesC8& aSourceUri,
                                    const TDesC8& aSourceLuid,
                                    const TDesC8& aType,
                                    TInt aStatusRef);
    DMAD_IMPORT_C void StartAtomicL();
    DMAD_IMPORT_C void CommitAtomicL();
    DMAD_IMPORT_C void RollbackAtomicL();
    DMAD_IMPORT_C TBool StreamingSupport(TInt& aItemSize);
    DMAD_IMPORT_C void StreamCommittedL();

private:
    void ConstructL();
    CDmAdEngine(MDmAdCallBack*           aDmAdCallBack,
                MDmAdStoreApi*           aStoreApi,
                MDmAdDdfApi*             aDdfApi,
                MDmAdRtNodeDataApi*      aRtNodeDataApi);
    
    void DoChildUriListL(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, TInt aResultsRef, TInt aStatusRef);
    void DoAddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef);
    void DoUpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef);
    void DoFetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef);
    TInt DoFetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, CBufBase*& aObject);
    void DoDeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef);
    void DoCompleteOutstandingCmdsL();
    

    CDmAdRtNode* FindRtNodeInBuffer(const TDesC8& aUri);
    CDmAdRtNode* FindRtNodeInBufferByLuid(const TDesC8& aLuid, const TDesC8& aUri);
    CDmAdRtNode* RestoreRtNodeInBufferIfNotDoneL(const TDesC8& aUri, const TDesC8& aLuid);
    CDmAdRtNode* FetchRtNodeInBufferL(const TDesC8& aUri, const TDesC8& aLuid);
    void CompleteCommandsL(CDmAdRtNode& aRtNode, TInt aStatus);
    void CompleteCommandsL(CArrayPtr<CDmAdRtNode>& aRtNodes, TInt aStatus);
    void BuildTopLevelRtNodesListLC(CArrayPtr<CDmAdRtNode>*     aRtNodes,
                                    CArrayPtr<CDmAdRtNode>*&    aTopLevelRtNodes);
    void BuildChildRtNodesListLC(CArrayPtr<CDmAdRtNode>*     aRtNodes,
                                 const TDesC8&                aSomeParentUri,
                                 CArrayPtr<CDmAdRtNode>*&    aChildRtNodes);

    void SaveRtNodesL(void);
    TBool FindRtNodeInStoreL(const TDesC8& aLuid, const TDesC8& aUri);
    HBufC8* ParentRtNodeLuidForRtNodeLC(const TDesC8& aUri);
    
private:
    MDmAdCallBack*          iCallBack;
    MDmAdStoreApi*          iStoreApi;
    MDmAdDdfApi*            iDdfApi;
    MDmAdRtNodeDataApi*     iRtNodeDataApi;

    CArrayPtr<CDmAdRtNode>* iRtNodes;
    };

#endif
