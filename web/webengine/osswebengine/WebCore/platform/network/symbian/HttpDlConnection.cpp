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

#include "config.h"

#include "HttpDlConnection.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include <httpfiltercommonstringsext.h>

using namespace WebCore;

HttpDlConnection::HttpDlConnection()
{
    m_httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    m_httpSessionMgr->addDlRequest(this);
}

HttpDlConnection* HttpDlConnection::dlConnectionFromTransaction(RHTTPTransaction& transaction)
{
    RHTTPTransactionPropertySet propSet = transaction.PropertySet();
    THTTPHdrVal val;
    if (propSet.Property(transaction.Session().StringPool().StringF(HttpFilterCommonStringsExt::ETransactionCallback,
        HttpFilterCommonStringsExt::GetTable()), val)) {
        return (HttpDlConnection*)(TInt(val));
    }
    return NULL;
}
// end of file
