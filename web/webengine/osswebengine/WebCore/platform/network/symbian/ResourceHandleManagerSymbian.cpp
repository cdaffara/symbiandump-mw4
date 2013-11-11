/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CResourceHandleManager
*
*/

#include "config.h"

#include "HashMap.h"
#include "CString.h"
#include "CookieJar.h"
#include "ResourceHandle.h"
#include "ResourceHandleClient.h"
#include "ResourceResponse.h"
#include "ResourceHandleManagerSymbian.h"
#include "ResourceHandleInternal.h"
#include "ResourceError.h"
#include "MimeTypeRegistry.h"
#include "WebCoreFrameBridge.h"
#include "WebCoreKeyValuePair.h"
#include "oom.h"
#include <e32hal.h>

const TUint	KPluginsRescueBuffer = 3*1024*1024; //3MB

#define notImplemented() {}

namespace WebCore {

static CResourceHandleManager* s_self = 0;

void cleanupHandleManager() 
{
    if(s_self){
    	delete s_self;
    	s_self = 0;
    }
}

CResourceHandleManager::CResourceHandleManager()
{
}

CResourceHandleManager::~CResourceHandleManager()
{
}

CResourceHandleManager* CResourceHandleManager::self()
{
    if (!s_self)
        s_self = new CResourceHandleManager();

    return s_self;
}

int CResourceHandleManager::add(ResourceHandle* resource)
{
    ASSERT(resource);

    ResourceHandleInternal* d = resource->getInternal();
    ResourceRequest r = resource->request();
    if (r.httpMethod() != "GET" && r.httpMethod() != "POST" &&
        r.httpMethod() != "DELETE" && r.httpMethod() != "PUT") {
        notImplemented();
        return KErrNotSupported;
    }
	if (d->m_connection)
		return (d->m_connection->submit());
    return KErrNone;
}

void CResourceHandleManager::cancel(ResourceHandle* resource)
{
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;
    MUrlConnection *connection = resource->getInternal()->m_connection;
    if (!connection)
        return;
    if (!connection->m_cancelled) {
        connection->m_cancelled = true;
        connection->cancel();
    }
}

void CResourceHandleManager::receivedResponse(ResourceHandle* resource, ResourceResponse& response, MUrlConnection* connection)
{
    if (!connection)
        return;
    if (connection->m_cancelled)
        return;
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;
    client->didReceiveResponse(resource, response);
}

void CResourceHandleManager::receivedData(ResourceHandle* resource, const TDesC8& data, int contentLength, MUrlConnection* connection)
{
    if (!connection)
        return;
    if (connection->m_cancelled)
        return;
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;

    // check if we have enough memory to handle this request
    if (resource->request().mainLoad()) {        
        OOM_PRE_CHECK(contentLength<<4, contentLength, "CResourceHandleManager::receiveData()")
        client->didReceiveData(resource, (const char*)data.Ptr(), data.Length(), data.Length());
        OOM_POST_CHECK_FAILED(client->didFail(resource, ResourceError(String(), KErrNoMemory, String(), String()));)
    }
    else if (client->isLoadingPlugins()) {
                      
        TMemoryInfoV1Buf info;
        UserHal::MemoryInfo( info );        
        int needed = data.Length();        
        if( needed+KPluginsRescueBuffer < info().iFreeRamInBytes ) {
            client->didReceiveData(resource, (const char*)data.Ptr(), data.Length(), data.Length());            
        } else {
            client->didFail(resource, ResourceError(String(), KErrNoMemory, String(), String()));    
        }
        
    }
    else {
        int needed = std::max(contentLength, std::max(client->receivedDataBufferSize(), data.Length()));
        OOM_PRE_CHECK(needed<<2, needed<<1, "CResourceHandleManager::receiveData()")        
        client->didReceiveData(resource, (const char*)data.Ptr(), data.Length(), data.Length());
        OOM_POST_CHECK_FAILED(connection->handleError(KErrNoMemory);)
    }
}

void CResourceHandleManager::receivedFinished(ResourceHandle* resource, TInt errorCode, MUrlConnection* connection)
{
    if (!connection)
        return;
    if (connection->m_cancelled)
        return;
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;
    if (connection->m_cancelled) {
        return;
    }
    if (errorCode) {
        client->didFail(resource, ResourceError(String(), errorCode, String(), String()));
    } else {
        client->didFinishLoading(resource);
    }
}

} // namespace WebCore



