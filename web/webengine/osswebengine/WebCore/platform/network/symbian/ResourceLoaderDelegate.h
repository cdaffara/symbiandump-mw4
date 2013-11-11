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

#ifndef __RESOURCELOADERDELEGATE_H__
#define __RESOURCELOADERDELEGATE_H__

#include <e32base.h>
#include "HttpSessionManager.h"

class MUrlConnection;
class MBrCtlLinkContent;

namespace WebCore {
    class ResourceHandle;
    struct ResourceRequest;
    class ResourceResponse;
    class Frame;
};

class ResourceLoaderDelegate
{
    public:
        ResourceLoaderDelegate() {}
        ~ResourceLoaderDelegate() {}
        MUrlConnection* CreateUrlConnection(WebCore::ResourceHandle* handle, WebCore::Frame* frame);
        void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response);
        HttpSessionManager* httpSessionManager() { return &m_httpSessionManager; }
    private:
        MUrlConnection* checkLinkResolverL(WebCore::ResourceHandle* handle, WebCore::Frame* frame);
        bool checkSecurityStatus(const WebCore::ResourceRequest& request, WebCore::Frame* frame);
        bool handleSpecialSchemeL(TPtrC8 url8, WebCore::Frame* frame);
    
    private:
        HttpSessionManager m_httpSessionManager;
};

#endif // __RESOURCELOADERDELEGATE_H__
// end of file
