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
* Description:   Call back implementation
*
*/



#ifndef __DMADCALLBACKC_H__
#define __DMADCALLBACKC_H__

#include "dmadengineexportdef.h"
#include "dmadcallback.h"

//------------------------------------------------------------------------------------------------
// CDmAdCallBack
//------------------------------------------------------------------------------------------------
class CDmAdCallBack : public CBase, public MDmAdCallBack
    {
public:
    DMAD_IMPORT_C static CDmAdCallBack* NewL(MSmlDmCallback* aCallBack, const TDesC8& aSomeOwnUri);
    DMAD_IMPORT_C static CDmAdCallBack* NewLC(MSmlDmCallback* aCallBack, const TDesC8& aSomeOwnUri);
    DMAD_IMPORT_C ~CDmAdCallBack();

    DMAD_IMPORT_C void SetMappingL(const TDesC8& aURI, const TDesC8& aLuid);
    DMAD_IMPORT_C void SetStatusL(TInt aStatusRef, TInt aError);
    DMAD_IMPORT_C void SetResultsL(TInt aResultsRef, CBufBase& aObject, const TDesC8& aType);
    DMAD_IMPORT_C void FetchLinkL(const TDesC8& aUri, CBufBase& aData, MSmlDmAdapter::TError& aStatus);
    DMAD_IMPORT_C HBufC8* GetLuidAllocL(const TDesC8& aUri);
    DMAD_IMPORT_C HBufC8* FindChildUriL(const TDesC8& aParentUri, const TDesC8& aChildLuid); 
    
private:
    void ConstructL(const TDesC8& aSomeOwnUri);
    CDmAdCallBack(MSmlDmCallback* aCallBack);
private:
    MSmlDmCallback* iCallBack;
    HBufC8*         iSomeOwnUri;
   };

#endif
