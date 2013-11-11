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
* Description: Class definition of MDmAdRtNodeDataApi.
*
*/



#ifndef __DMADRTNODEDATAAPI_H__
#define __DMADRTNODEDATAAPI_H__

#include <e32base.h>

class MDmAdCallBack;
class CDmAdRtNodeData;

//------------------------------------------------------------------------------------------------
// MDmAdRtNodeDataApi
//------------------------------------------------------------------------------------------------
class MDmAdRtNodeDataApi
    {
    public:
    virtual CDmAdRtNodeData* CreateDmAdRtNodeDataL(const TDesC8& aUri, MDmAdCallBack* aCallBack) = 0;
    virtual void DeleteDmAdRtNodeData(CDmAdRtNodeData* aDmAdRtNodeData) = 0;
    
    virtual void UpdateLeafDataL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aObject) = 0;
    virtual void FetchLeafObjectLC(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject) = 0;
    virtual void SetDefaultSettingsL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri) = 0;
    virtual TBool AreUriTypesSame(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri) = 0;
    };
#endif
