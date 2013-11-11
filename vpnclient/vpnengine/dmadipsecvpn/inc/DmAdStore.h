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
* Description: Class definition of CDmAdStore.
*
*/



#ifndef __DMADSTORE_H__
#define __DMADSTORE_H__

#include <e32base.h>

#include "dmadstoreapi.h"
#include "DmAdRtNode.h"

#include "dmadstorevpnap.h"
#include "vpnapi.h"

#include "eventlogger.h"

//------------------------------------------------------------------------------------------------
// CDmAdStore
//------------------------------------------------------------------------------------------------
class CDmAdStore : public CBase, public MDmAdStoreApi
    {
public:
    static CDmAdStore* NewL(void);
    ~CDmAdStore();
    TBool FindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void LuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList);
    void FetchRtNodeL(CDmAdRtNode& aRtNode);
    void SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);
    void SaveChildLevelRtNodeL(CDmAdRtNode& aRtNode);
    void DeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);

private:
    void ConstructL(void);
    CDmAdStore();

    TBool VpnApFindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void VpnApLuidListL(RPointerArray<HBufC8>& aLuidList);
    void VpnApFetchRtNodeL(CDmAdRtNode& aRtNode);
    void VpnApSaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);
    void VpnApDeleteRtNodeL(const TDesC8& aLuid);
    
    TPtrC8 PolicyRefToLuid(const TDesC& aPolicyRef);
    TPtrC LuidToPolicyRef(const TDesC8& aLuid);
    TBool PolicyFindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void PolicyLuidListL(RPointerArray<HBufC8>& aLuidList);
    void PolicyFetchRtNodeL(CDmAdRtNode& aRtNode);
    void PolicySaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);
    void PolicyDeleteRtNodeL(const TDesC8& aLuid);
    
private:
    CVpnAp*         iVpnAp;
    RVpnServ        iVpnApi;
        
    DEFINE_EVENT_LOGGER
    };

#endif
