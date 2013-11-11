/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
 
#include "config.h"
#include "WmlLoader.h"

#include "Document.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClient.h"
#include "Logging.h"
#include "ResourceHandle.h"
#include "ResourceResponse.h"
#include "ResourceRequest.h"
#include "SubresourceLoader.h"

using namespace std;

namespace WebCore {

WmlLoader::WmlLoader(Frame* frame)
    : m_frame(frame)
    , m_loadIsInProgress(false)
{
}

auto_ptr<WmlLoader> WmlLoader::create(Frame* frame)
{
    return auto_ptr<WmlLoader>(new WmlLoader(frame));
}

WmlLoader::~WmlLoader()
{
}

void WmlLoader::startLoading()
{    
    if (m_resourceLoader)
        return;

    if (!m_frame->document()) {
        LOG(WmlDatabase, "Documentless-frame - Wml won't be loaded");
        return;
    }
    // Set flag so we can detect the case where the load completes before
    // SubresourceLoader::create returns.
    m_loadIsInProgress = true;
}

void WmlLoader::stopLoading()
{
    clearLoadingState();
}

void WmlLoader::didReceiveResponse(SubresourceLoader* resourceLoader, const ResourceResponse& response)
{
    // If we got a status code indicating an invalid response, then lets
    // ignore the data and not try to decode the error page as an Wml.
    int status = response.httpStatusCode();
    LOG(WmlDatabase, "WmlLoader::didReceiveResponse() - Loader %p, response %i", resourceLoader, status);

    if (status && (status < 200 || status > 299)) {
        ResourceHandle* handle = resourceLoader->handle();
        finishLoading(handle ? handle->request().url() : KURL(), 0);
    }
}

void WmlLoader::didReceiveData(SubresourceLoader* loader, const char*, int size)
{
    LOG(WmlDatabase, "WmlLoader::didReceiveData() - Loader %p, number of bytes %i", loader, size);
}

void WmlLoader::didFail(SubresourceLoader* resourceLoader, const ResourceError&)
{
    LOG(WmlDatabase, "WmlLoader::didFail() - Loader %p", resourceLoader);
    
    if (m_loadIsInProgress) {
        ASSERT(resourceLoader == m_resourceLoader);
        ResourceHandle* handle = resourceLoader->handle();
        finishLoading(handle ? handle->request().url() : KURL(), 0);
    }
}

void WmlLoader::didFinishLoading(SubresourceLoader* resourceLoader)
{
    LOG(WmlDatabase, "WmlLoader::didFinishLoading() - Loader %p", resourceLoader);

    if (m_loadIsInProgress) {
        ASSERT(resourceLoader == m_resourceLoader);
        ResourceHandle* handle = resourceLoader->handle();
        finishLoading(handle ? handle->request().url() : KURL(), m_resourceLoader->resourceData());
    }
}

void WmlLoader::finishLoading(const KURL& WmlURL, PassRefPtr<SharedBuffer> data)
{
    if (!WmlURL.isEmpty() && m_loadIsInProgress) {
        LOG(WmlDatabase, "WmlLoader::finishLoading() - Committing WmlURL %s to database", WmlURL.url().ascii());     
    }

    clearLoadingState();
}

void WmlLoader::clearLoadingState()
{
    m_resourceLoader = 0;
    m_loadIsInProgress = false;
}

}
