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
* Description: Class definition of MDmAdDdfApi.
*
*/



#ifndef __DMADDDFAPI_H__
#define __DMADDDFAPI_H__

#include <smldmadapter.h>

//------------------------------------------------------------------------------------------------
// MDmAdDdfApi
//------------------------------------------------------------------------------------------------
class MDmAdDdfApi
    {
    public:
    virtual void NotRtNodeAddNodeObjectL(const TDesC8& aUri, const TDesC8& aParentLuid, TInt aStatusRef) = 0;
    virtual void NotRtNodeUpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aLuid, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef) = 0;
    virtual TInt NotRtNodeFetchLeafObjectLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, const TDesC8& /*aType*/, CBufBase*& aObject) = 0; 
    virtual void NotRtNodeDeleteObjectL(const TDesC8& aUri, const TDesC8& aLuid, TInt aStatusRef) = 0;
    
    virtual TBool IsNodeRtNodeL(const TDesC8& aUri) = 0;
    virtual TBool IsLeafUnderRtNodeL(const TDesC8& aUri) = 0;
    virtual TPtrC8 RtNodeUriForLeafL(const TDesC8& aLeafUri) = 0;
    virtual TBool IsTopLevelRtNode(const TDesC8& aUri) = 0;
    virtual HBufC8* ParentRtNodeUriForRtNodeLC(const TDesC8& aUri) = 0;
    virtual void BuildChildUriListLC(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, CBufBase*& aCurrentList) = 0;
    };
#endif
