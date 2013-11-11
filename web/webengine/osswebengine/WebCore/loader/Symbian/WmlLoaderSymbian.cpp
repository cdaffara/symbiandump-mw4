/*
 * Copyright (C) 2005, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Nokia Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"
#include <wtf/PassRefPtr.h>
#include "WmlResourceLoader.h"
#include "WmlResourceLoaderClient.h"
#include "DocumentLoader.h"
#include "FrameLoader.h"
#include "ResourceError.h"
#include "ResourceResponse.h"
#include "SharedBuffer.h"

using namespace WebCore;

WmlResourceLoader::WmlResourceLoader(Frame* frame, WmlResourceLoaderClient* client)
    : ResourceLoader(frame, true, true)
    , m_client(client)
    , m_isdone(false)
{
}

WmlResourceLoader::~WmlResourceLoader()
{    
    delete m_client;
}

WmlResourceLoader* WmlResourceLoader::create(Frame* frame, WmlResourceLoaderClient* client)
{
    return new WmlResourceLoader(frame, client);
}

bool WmlResourceLoader::isDone() const
{
    return m_isdone;
}

void WmlResourceLoader::releaseResources()
{
}

void WmlResourceLoader::didReceiveResponse(const ResourceResponse& theResponse)
{
    
    RefPtr<WmlResourceLoader> protect(this);
    ResourceLoader::didReceiveResponse(theResponse);
    
    if (m_client)
        m_client->didReceiveResponse(theResponse);
        
}

void WmlResourceLoader::didReceiveData(const char* data, int length, long long lengthReceived, bool allAtOnce)
{
    RefPtr<WmlResourceLoader> protect(this);

    if (m_client)
        m_client->didReceiveData(data, length, lengthReceived);
    
    ResourceLoader::didReceiveData(data, length, lengthReceived, allAtOnce);
}

void WmlResourceLoader::didFinishLoading()
{
    RefPtr<WmlResourceLoader> protect(this);    
    
    if (m_client) {
        m_client->didFinishLoading();                
    }
    
    m_isdone = true;       
    m_documentLoader->removeSubresourceLoader(this);
    ResourceLoader::didFinishLoading();                          
}

void WmlResourceLoader::didFail(const ResourceError& error)
{
    RefPtr<WmlResourceLoader> protect(this);
    
    if (m_client)
        m_client->didFail(error);
    
    m_isdone = true;
    m_documentLoader->removeSubresourceLoader(this);
    ResourceLoader::didFail(error);        
}

void WmlResourceLoader::didCancel(const ResourceError& error)
{

    RefPtr<WmlResourceLoader> protect(this);
        
    if (m_client) {
        m_client->didCancel(error);
    }
    
    m_isdone = true;
    m_documentLoader->removeSubresourceLoader(this);
    ResourceLoader::didCancel(error);            
}
