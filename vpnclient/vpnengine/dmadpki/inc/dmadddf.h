/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class definition of CDmAdDdf
*
*/



#ifndef __DMADDDF_H__
#define __DMADDDF_H__

#include <smldmadapter.h>
#include "dmadddfapi.h"
#include "dmadcallback.h"
#include "DmAdStore.h"
#include "dmadddf_lits.h"
//------------------------------------------------------------------------------------------------
// CDmAdDdf 
//------------------------------------------------------------------------------------------------
class CDmAdDdf : public CBase, public MDmAdDdfApi
    {
public:
    static CDmAdDdf* NewL(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore);
    static CDmAdDdf* NewLC(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore);
    ~CDmAdDdf();

    void BuildDDFVersionL(CBufBase& aDDFVersion);
    void BuildDDFStructureL(MSmlDmDDFObject& aDDF);
    
    void NotRtNodeAddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef);
    void NotRtNodeUpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef);
    TInt NotRtNodeFetchLeafObjectLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, const TDesC8& /*aType*/, CBufBase*& aObject); 
    void NotRtNodeDeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef);
    
    TBool IsNodeRtNodeL(const TDesC8& aUri);
    TBool IsLeafUnderRtNodeL(const TDesC8& aUri);
    TPtrC8 RtNodeUriForLeafL(const TDesC8& aLeafUri);
    TBool IsTopLevelRtNode(const TDesC8& aUri);
    HBufC8* ParentRtNodeUriForRtNodeLC(const TDesC8& aUri);
    void BuildChildUriListLC(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, CBufBase*& aCurrentList);
    
private:
    CDmAdDdf(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore);

private:
    MDmAdCallBack*          iCallBack;
    CDmAdStore*             iStore;
    };

#endif
