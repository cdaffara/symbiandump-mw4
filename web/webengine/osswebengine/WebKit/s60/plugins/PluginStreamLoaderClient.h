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
* Description:  Loads the plugin content
*
*//*
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
* Description:  
*
*/

#ifndef __PLUGINLOADER_H__
#define __PLUGINLOADER_H__

#include <wtf/RefPtr.h>

class PluginSkin;
class PluginStream;

namespace WebCore {

class Frame;
class String;
class ResourceError;
struct ResourceRequest;
class ResourceResponse;  
class NetscapePlugInStreamLoader;

class NetscapePlugInStreamLoaderClient
{

public:
    static NetscapePlugInStreamLoaderClient* NewL(const String& url, PluginSkin* pluginskin, Frame* frame, void* notifydata = 0);
    static NetscapePlugInStreamLoaderClient* NewL(const ResourceRequest& request, PluginSkin* pluginskin, Frame* frame, void* notifydata=0);
    virtual ~NetscapePlugInStreamLoaderClient();             
    
    void start();    
    void stop();
    void cancelWithError(const ResourceError& error);

public:
    void didReceiveResponse(const ResourceResponse&);
    void didReceiveData(const char*, int, long long);
    void didFinishLoading();
    void didFail(const ResourceError& error);
    void didCancel(const ResourceError& error);
    
private:
    void ConstructL(const String& url, PluginSkin* pluginskin, Frame* frame, void* notifydata = 0);
    void ConstructL(const ResourceRequest& request, PluginSkin* pluginskin, Frame* frame, void* notifydata=0);

    NetscapePlugInStreamLoaderClient();

private:
    Frame* m_frame;
    PluginStream* m_pluginstream;
    ResourceRequest* m_request;              
    NetscapePlugInStreamLoader* m_loader;
    int m_error;
    
};

} //namespace

#endif
