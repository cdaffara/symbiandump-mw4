/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#ifndef __HTTPCALLBACKS_H__
#define __HTTPCALLBACKS_H__

#include <e32base.h>
#include <http/mhttptransactioncallback.h>

// This class implements the switch between Browser transactions and Download Manager transactions
class HttpCallbacks : public MHTTPTransactionCallback
{
public:
    HttpCallbacks();
    ~HttpCallbacks();

public: // from MHTTPTransactionCallback
    void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent &aEvent);
    TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent &aEvent);

private: // data
};

#endif // __HTTPCALLBACKS_H__
// end of file
