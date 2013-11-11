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

#include <http/rhttpheaders.h>
#include "HttpConnection.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include <httpfiltercommonstringsext.h>

using namespace WebCore;

// -----------------------------------------------------------------------------
// HttpConnection::addRequestHeadersL
// Add headers to the request as required by the WebKit.
// -----------------------------------------------------------------------------
//
void HttpConnection::addRequestHeadersL()
{
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    RHTTPSession& session = httpSessionMgr->httpSession();
    RStringPool stringPool = session.StringPool();
    const TStringTable& stringTable = RHTTPSession::GetTable();
    RHTTPHeaders hdr = m_transaction->Request().GetHeaderCollection();
    RHTTPTransactionPropertySet propSet = m_transaction->PropertySet();
    // Add own adress to the transaction properties
    propSet.RemoveProperty( stringPool.StringF(HttpFilterCommonStringsExt::ESelfPtr,
        HttpFilterCommonStringsExt::GetTable()));
    propSet.SetPropertyL( stringPool.StringF( HttpFilterCommonStringsExt::ESelfPtr,
        HttpFilterCommonStringsExt::GetTable()), THTTPHdrVal((TInt)this));   
    // pipelining
    RStringF property = stringPool.StringF( HTTP::EHttpPipelining, stringTable );
    THTTPHdrVal pipelining;
    if (!httpSessionMgr->httpPipelining()){
        pipelining = stringPool.StringF( HTTP::EDisablePipelining, stringTable );
    }
    else { 
        pipelining = stringPool.StringF( HTTP::EEnablePipelining, stringTable );
    }
    propSet.RemoveProperty( property );
    propSet.SetPropertyL( property, pipelining );
}

// end of file
