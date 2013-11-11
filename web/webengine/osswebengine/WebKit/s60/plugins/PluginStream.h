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
*/

#ifndef __PLUGINSTREAM_H__
#define __PLUGINSTREAM_H__

#include <e32base.h>
#include <npupp.h>

class PluginSkin;

namespace WebCore {
    class Frame;
    class NetscapePlugInStreamLoaderClient;
    struct ResourceRequest;
}

class PluginStream 
{
public:
    PluginStream(PluginSkin* pluginskin, WebCore::NetscapePlugInStreamLoaderClient* loaderclient,
            void* notifydata = 0);
    virtual ~PluginStream();
    
public:
    void createNPStreamL(TPtrC8 url, TPtrC16 mimetype, long long length);
    void writeStreamL(const char* data, int length);
    void destroyStream(int reason);  
    void close();
    
private:
    void generateTempFileL();
    void writeStreamToPluginL(const char* data, int length);
    void writeStreamToFileL(const char* data, int length);
    
private:        
    void*               m_notifydata; //not owned;
    PluginSkin*         m_pluginskin;      //not owned
    WebCore::NetscapePlugInStreamLoaderClient*   m_loaderclient; //not owned
    
    NPStream* m_stream; //owned
    TUint16 m_type;  //owned
    HBufC* m_fileName;  //owned
    
     bool m_streamDestroyed;
};

#endif
