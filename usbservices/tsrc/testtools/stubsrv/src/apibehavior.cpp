/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "apibehavior.h"

EXPORT_C TApiBehavior::TApiBehavior() 
    : iLib(0), iApi(0), iCompleteCode(0), iAsyncCompleteCode(0), iOutput(KNullDesC8)
    {
    }

EXPORT_C TApiBehavior::TApiBehavior(TUint aLib, TUint aApi) 
    : iLib(aLib), iApi(aApi), iCompleteCode(0), iAsyncCompleteCode(0), iOutput(KNullDesC8)
    {
    }

EXPORT_C TApiBehavior::TApiBehavior(TUint aLib, TUint aApi, 
    TInt aCompleteCode, TInt aAsyncCompleteCode, const TDesC8& aOutput)
    : iLib(aLib), iApi(aApi), iCompleteCode(aCompleteCode), 
    iAsyncCompleteCode(aAsyncCompleteCode), iOutput(aOutput)
    {
    }
    
EXPORT_C TBool TApiBehavior::MatchId(TUint aLib, TUint aApi) const
    {
    return iLib == aLib && iApi == aApi;
    }

EXPORT_C TBool TApiBehavior::MatchId(const TApiBehavior& aBeh) const
    {
    return MatchId(aBeh.iLib, aBeh.iApi);
    }
    
EXPORT_C TBool TApiBehavior::operator==(const TApiBehavior& aBeh) const
    {
    return 
        (iLib == aBeh.iLib &&
        iApi == aBeh.iApi &&
        iCompleteCode == aBeh.iCompleteCode &&
        iAsyncCompleteCode == aBeh.iAsyncCompleteCode &&
        iOutput == aBeh.iOutput);
    }
    
EXPORT_C TBool TApiBehavior::operator!=(const TApiBehavior& aBeh) const
    {
    return  !(*this == aBeh);
    }
