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
* Description: Class definition of CDmAdRtNode.
*
*/



#ifndef __DMADRTNODE_H__
#define __DMADRTNODE_H__

#include <e32base.h>

#include "dmadengineexportdef.h"
#include "dmadcallback.h"
#include "dmadcommand.h"
#include "dmadrtnodedataapi.h"

//------------------------------------------------------------------------------------------------
// CDmAdRtNode
//------------------------------------------------------------------------------------------------
class CDmAdRtNode : public CBase
    {
public:
    DMAD_IMPORT_C static CDmAdRtNode* NewL(const TDesC8& aUri, const TDesC8& aLuid, MDmAdCallBack* aCallBack, MDmAdRtNodeDataApi* aRtNodeDataApi);
    DMAD_IMPORT_C ~CDmAdRtNode();
    DMAD_IMPORT_C TPtrC8 Uri(void) const;
    DMAD_IMPORT_C TPtrC8 Luid(void) const;
    DMAD_IMPORT_C void SetLuidL(const TDesC8& aLuid);
    static void CleanupOperationDeleteCArrayPtr(TAny* aPtr);
    void AppendCommandL(CDmAdCommand* aCommand);
    void AddLeafObjectL(const TDesC8& aUri, const TDesC8& aObject, TInt aStatusRef);
    void UpdateLeafObjectL(const TDesC8& aUri, const TDesC8& aObject, TInt aStatusRef);
    TBool IsSomeChild(const TDesC8& aSomeParentUri) const;
    TBool AreUriTypesSame(const TDesC8& aUri);
    
    inline CArrayPtr<CDmAdCommand>* Commands(void) const;
    inline TBool IsSomeLeafAddedToRtNode(void) const;
    inline TBool IsJustFetched(void) const;
    inline CDmAdRtNodeData* Data(void) const;
    inline void SetSomeLeafAddedToRtNode(TBool aSomeLeafAddedToRtNode);

private:    
    CDmAdRtNode(MDmAdRtNodeDataApi* aRtNodeDataApi);
    void ConstructL(const TDesC8& aUri, const TDesC8& aLuid, MDmAdCallBack* aCallBack);
    
private:
    MDmAdRtNodeDataApi*         iRtNodeDataApi;
    
    CArrayPtr<CDmAdCommand>*    iCommands;
    HBufC8*                     iUri;
    HBufC8*                     iLuid;
    TBool                       iSomeLeafAddedToRtNode;
    TBool                       iJustFetched;
    CDmAdRtNodeData*            iData;
    };

#include "dmadrtnode.inl"

#endif
