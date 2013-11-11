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
* Description: Class definition of MDmAdStoreApi.
*
*/



#ifndef __DMADSTOREAPI_H__
#define __DMADSTOREAPI_H__

#include "DmAdRtNode.h"

//------------------------------------------------------------------------------------------------
// MDmAdStoreApi
//------------------------------------------------------------------------------------------------
class MDmAdStoreApi
    {
    public:
    virtual TBool FindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri) = 0;
    virtual void LuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList) = 0;
    virtual void FetchRtNodeL(CDmAdRtNode& aRtNode) = 0;
    virtual void SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes) = 0;
    virtual void SaveChildLevelRtNodeL(CDmAdRtNode& aRtNode) = 0;
    virtual void DeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri) = 0;
    };
#endif
