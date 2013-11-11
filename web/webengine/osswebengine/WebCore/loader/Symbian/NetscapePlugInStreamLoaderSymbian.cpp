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
#include "NetscapePlugInStreamLoader.h"
#include "PluginStreamLoaderClient.h"
#include "DocumentLoader.h"
#include "FrameLoader.h"
#include "ResourceError.h"
#include "ResourceResponse.h"
#include "SharedBuffer.h"
#include "ResourceHandle.h"
#include <wtf/PassRefPtr.h>

using namespace WebCore;

NetscapePlugInStreamLoader::NetscapePlugInStreamLoader(Frame* frame, PlugInStreamLoaderDelegate stream)
    : ResourceLoader(frame, true, true)
    , m_stream(stream)
    , m_isdone(false)
{
}

NetscapePlugInStreamLoader::~NetscapePlugInStreamLoader()
{
    delete m_stream;
}

NetscapePlugInStreamLoader* NetscapePlugInStreamLoader::create(Frame* frame, PlugInStreamLoaderDelegate d)
{
    return new NetscapePlugInStreamLoader(frame, d);
}

bool NetscapePlugInStreamLoader::isDone() const
{
    return m_isdone;
}

void NetscapePlugInStreamLoader::releaseResources()
{
    if (ResourceLoader::handle())
        ResourceLoader::handle()->setClient(0);
}

void NetscapePlugInStreamLoader::didReceiveResponse(const ResourceResponse& theResponse)
{
    
    RefPtr<NetscapePlugInStreamLoader> protect(this);

    ResourceLoader::didReceiveResponse(theResponse);
    
    if (m_stream)
        m_stream->didReceiveResponse(theResponse);
        
}

void NetscapePlugInStreamLoader::didReceiveData(const char* data, int length, long long lengthReceived, bool allAtOnce)
{
    RefPtr<NetscapePlugInStreamLoader> protect(this);

    if (m_stream)
        m_stream->didReceiveData(data, length, lengthReceived);
    
    ResourceLoader::didReceiveData(data, length, lengthReceived, allAtOnce);
}

void NetscapePlugInStreamLoader::didFinishLoading()
{
    if (cancelled())
        return;

    RefPtr<NetscapePlugInStreamLoader> protect(this);    

    if (m_stream) {
        m_stream->didFinishLoading();                
    }

    m_isdone = true;    

    if (cancelled())
        return;
    
    m_documentLoader->removeSubresourceLoader(this);
    ResourceLoader::didFinishLoading();              
    
}

void NetscapePlugInStreamLoader::didFail(const ResourceError& error)
{
    if (cancelled())
        return;

    RefPtr<NetscapePlugInStreamLoader> protect(this);

    if (m_stream)
        m_stream->didFail(error);
    
    m_isdone = true;   

    if (cancelled())
        return;
     
    m_documentLoader->removeSubresourceLoader(this);
    ResourceLoader::didFail(error);

}

void NetscapePlugInStreamLoader::didCancel(const ResourceError& error)
{

    if (cancelled())
        return;
    
    RefPtr<NetscapePlugInStreamLoader> protect(this);
    
    if (m_stream) {
        m_stream->didCancel(error);        
    }
    
    if (cancelled())
        return;    
    
    m_isdone = true;     
    m_documentLoader->removeSubresourceLoader(this);
    ResourceLoader::didCancel(error);    
}
