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
* Description:  Implementation of SynchResourceHandleClient
*
*/


// INCLUDE FILES

#include "config.h"

#include "SynchLoader.h"

#include "DocumentLoader.h"
#include "FrameLoader.h"

namespace WebCore {

// ============================SynchLoader MEMBER FUNCTIONS ===============================

class SynchResourceHandleClient;

SynchLoader::SynchLoader(Frame* frame, SynchResourceHandleClient* client, bool sendResourceLoadCallbacks, bool shouldContentSniff)
    : ResourceLoader(frame, sendResourceLoadCallbacks, shouldContentSniff)
    , m_client(client)
    , m_isdone(false)
{
    m_documentLoader->addSubresourceLoader(this);
}

SynchLoader::~SynchLoader()
{    
}

PassRefPtr<SynchLoader> SynchLoader::create(Frame* frame, const ResourceRequest& request, SynchResourceHandleClient* client)
{
    if (!frame)
        return 0;
    
    FrameLoader* fl = frame->loader();
    if (fl->state() == FrameStateProvisional)
        return 0;
    
    RefPtr<SynchLoader> loader(new SynchLoader(frame, client, true, false));
    if (!loader->load(request)) {
        return 0;
    }

    return loader.release();
}

bool SynchLoader::load(const ResourceRequest& r)
{
    return ResourceLoader::load(r);
}

void SynchLoader::didReceiveResponse(const ResourceResponse& r)
{
    ASSERT(!r.isNull());

    if (m_client) {
        m_client->didReceiveResponse(m_client->resourceHandle(), r);
    }
}

void SynchLoader::didReceiveData(const char* data, int length, long long lengthReceived, bool allAtOnce)
{
    if (m_client) {
        m_client->didReceiveData(m_client->resourceHandle(), data, length, lengthReceived);
    }
}

void SynchLoader::didFinishLoading()
{
    if (cancelled()) {
        return;
    }

    // Decrement ref counter on handle, so connection can be torn down or cancelled
    m_handle = 0;

    if (m_client) {
        m_client->didFinishLoading(m_client->resourceHandle());
    }
}

void SynchLoader::didFail(const ResourceError& error)
{
    if (cancelled()) {
        return;
    }

    // Decrement ref counter on handle, so connection can be torn down or cancelled
    m_handle = 0;

    if (m_client) {
        m_client->didFail(m_client->resourceHandle(), error);
    }
}

void SynchLoader::didCancel(const ResourceError& /*error*/)
{
    // Decrement ref counter on handle, so connection can be cancelled
    m_handle = 0;

    if (m_client) {
        m_client->cancel();    
    }
}

void SynchLoader::removeSubresourceLoader()
{
    if (!m_isdone) {
        // Avoid multiple cycles
        m_isdone = true;
        
        RefPtr<SynchLoader> protect(this);
        
        m_documentLoader->removeSubresourceLoader(this);
    }
 
}


// ============================ SynchResourceHandleClient MEMBER FUNCTIONS ===============================

SynchResourceHandleClient::SynchResourceHandleClient(TRequestStatus& aStatus, 
                                                     const ResourceRequest& request, 
                                                     Frame* frame)
    : m_finished(false)
    , m_response()
    , m_error(String(), KErrNone, String(), String())
    , m_data(0)
{
    m_status = &aStatus;
    
    // Send request
    m_loader = SynchLoader::create(frame, request, this);
    
    if (m_loader) {
        m_resourceHandle = m_loader->handle();
    }
}

SynchResourceHandleClient::~SynchResourceHandleClient()
{
}

PassRefPtr<SynchResourceHandleClient> SynchResourceHandleClient::create(TRequestStatus& aStatus, 
                                                                        const ResourceRequest& request, 
                                                                        Frame* frame)
{
    RefPtr<SynchResourceHandleClient> client(new SynchResourceHandleClient(aStatus, request, frame));
    if (!client->loader()) {
        return 0;
    }

    return client.release();
}

void SynchResourceHandleClient::didReceiveResponse(ResourceHandle* /*resource*/, const ResourceResponse& response)
{
    m_response = response;
}

void SynchResourceHandleClient::didReceiveData(ResourceHandle* /*resource*/, const char* data, int length, int /*lengthReceived*/)
{
    m_data.append(data, length);
}

void SynchResourceHandleClient::didFinishLoading(ResourceHandle* /*resource*/)
{
    // Decrement ref counter on handle, so connection can be torn down or cancelled
    m_resourceHandle = 0;
    
    finish(ResourceError());
}

void SynchResourceHandleClient::didFail(ResourceHandle* /*resource*/, const ResourceError& error)
{
    // Decrement ref counter on handle, so connection can be torn down or cancelled
    m_resourceHandle = 0;
    
    finish(error);
}

void SynchResourceHandleClient::cancel()
{
    cancel(ResourceError(String(), KErrCancel, String(), String()));
}

void SynchResourceHandleClient::cancel(const ResourceError& err)
{
    // Operation was cancelled
    // Need to cancel outstanding request, if not done so already
    if (resourceHandle()) {
        ResourceHandle* h = m_resourceHandle.get();
        m_resourceHandle = 0;
        h->cancel();
    }
    
    finish(err);
}

void SynchResourceHandleClient::finish(const ResourceError& error)
{
    m_error = error;
    
    if (!m_finished) { // Avoid multiple execution
        if (m_loader) {
            m_loader->removeSubresourceLoader();
        }

        if (m_status) {       
            // Complete SynchRequest AO
            if (KRequestPending == m_status->Int()) {
                User::RequestComplete(m_status, error.errorCode());
            }        
        }
    }
    
    m_finished = true;
    //m_loader = 0;
}

} // End namespace WebCore

