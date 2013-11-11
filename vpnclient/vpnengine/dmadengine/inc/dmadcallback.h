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
* Description:   Call back interface
*
*/



#ifndef __DMADCALLBACK_H__
#define __DMADCALLBACK_H__

#include <smldmadapter.h>

//------------------------------------------------------------------------------------------------
// MDmAdCallBack
//------------------------------------------------------------------------------------------------
class MDmAdCallBack
    {
    public:
    virtual void SetResultsL(TInt aResultsRef, CBufBase& aObject, const TDesC8& aType) = 0;
    virtual void SetStatusL(TInt aStatusRef, TInt aError) = 0;
    virtual void SetMappingL(const TDesC8& aUri, const TDesC8& aLuid) = 0;
    virtual void FetchLinkL(const TDesC8& aUri, CBufBase& aData, MSmlDmAdapter::TError& aStatus) = 0;
    virtual HBufC8* GetLuidAllocL(const TDesC8& aUri) = 0;
    virtual HBufC8* FindChildUriL(const TDesC8& aParentUri, const TDesC8& aChildLuid) = 0; 
    };
#endif
