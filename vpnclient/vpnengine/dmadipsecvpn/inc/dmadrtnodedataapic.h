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
* Description: Class definition of CDmAdRtNodeDataApi.
*
*/



#ifndef __DMADRTNODEDATAAPIC_H__
#define __DMADRTNODEDATAAPIC_H__

#include <e32base.h>

#include "dmadrtnodedataapi.h"
#include "DmAdRtNodeData.h"
#include "dmadcallback.h"

//------------------------------------------------------------------------------------------------
// CDmAdRtNodeDataApi
//------------------------------------------------------------------------------------------------
class CDmAdRtNodeDataApi : public CBase, public MDmAdRtNodeDataApi
    {
public:
    static CDmAdRtNodeDataApi* NewL(void);
    static CDmAdRtNodeDataApi* NewLC(void);
    ~CDmAdRtNodeDataApi();

    CDmAdRtNodeData* CreateDmAdRtNodeDataL(const TDesC8& aUri, MDmAdCallBack* aCallBack);
    void DeleteDmAdRtNodeData(CDmAdRtNodeData* aDmAdRtNodeData);
    void UpdateLeafDataL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aObject);
    void FetchLeafObjectLC(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
    void SetDefaultSettingsL(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri);
    TBool AreUriTypesSame(CDmAdRtNodeData* aDmAdRtNodeData, const TDesC8& aUri);

private:
    void ConstructL(void);
    CDmAdRtNodeDataApi();
   };

#endif
