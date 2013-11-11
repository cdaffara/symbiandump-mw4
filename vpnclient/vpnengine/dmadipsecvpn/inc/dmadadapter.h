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
* Description:   VPN OMA DM Adapter
*
*/



#ifndef __DMADADAPTER_H__
#define __DMADADAPTER_H__

#include <smldmadapter.h>
   
#include "dmadcallbackc.h"
#include "DmAdStore.h"
#include "dmadddf.h"
#include "dmadrtnodedataapic.h"
#include "dmadengine.h"

//------------------------------------------------------------------------------------------------
// CDmAdAdapter 
//------------------------------------------------------------------------------------------------
class CDmAdAdapter : public CSmlDmAdapter
    {
public:
    static CDmAdAdapter* NewL(MSmlDmCallback* aDmCallback);
    static CDmAdAdapter* NewLC(MSmlDmCallback* aDmCallback);
    ~CDmAdAdapter();

    void DDFVersionL(CBufBase& aVersion);
    void DDFStructureL(MSmlDmDDFObject& aDDF);
    
    void ChildURIListL(const TDesC8& aUri, const TDesC8& aLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef);
    void AddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, const TInt aStatusRef);
    void UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef);
    void FetchLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef);
    void DeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef);
    void CompleteOutstandingCmdsL();
    void UpdateLeafObjectL(const TDesC8& aUri,
                           const TDesC8& aLuid,
                           RWriteStream*& aStream,
                           const TDesC8& aType,
                           TInt aStatusRef);
    void FetchLeafObjectSizeL(const TDesC8& aUri,
                              const TDesC8& aLuid,
                              const TDesC8& aType,
                              TInt aResultsRef,
                              TInt aStatusRef);
    void ExecuteCommandL(const TDesC8& aUri,
                         const TDesC8& aLuid,
                         const TDesC8& aArgument,
                         const TDesC8& aType,
                         TInt aStatusRef);
    void ExecuteCommandL(const TDesC8& aUri,
                         const TDesC8& aLuid,
                         RWriteStream*& aStream,
                         const TDesC8& aType,
                         TInt aStatusRef);
    void CopyCommandL(const TDesC8& aTargetUri,
                      const TDesC8& aTargetLuid,
                      const TDesC8& aSourceUri,
                      const TDesC8& aSourceLuid,
                      const TDesC8& aType,
                      TInt aStatusRef);
    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport(TInt& aItemSize);
    void StreamCommittedL();

private:
    CDmAdAdapter(MSmlDmCallback* aDmCallback);
    void ConstructL(MSmlDmCallback* aDmCallback);

private:
    CDmAdCallBack*          iCallBack;
    CDmAdStore*             iStore;
    CDmAdDdf*               iDdf;
    CDmAdRtNodeDataApi*     iRtNodeDataApi;
    CDmAdEngine*            iEngine;
    };

#endif
