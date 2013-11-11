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

#ifndef __URLCONNECTION_H__
#define __URLCONNECTION_H__

#include <e32base.h>
#include "ResourceHandle.h"
#include "ResourceHandleInternal.h"

#include "BrCtlDefs.h"

namespace WebCore {
    class Frame;
};

class MUrlConnection
{
public:
    MUrlConnection(WebCore::ResourceHandle* _handle) {m_cancelled = false; m_handle = _handle; m_handle->ref(); }
    virtual ~MUrlConnection() { if (m_handle) m_handle->getInternal()->m_connection = 0; derefHandle(); }
    virtual int submit() = 0;
    virtual void cancel() = 0;
    virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response) = 0;
    virtual WebCore::ResourceHandle* handle() { return m_handle; }
    virtual int totalContentSize() = 0;
    virtual void setDefersLoading(bool /*defers*/) {}
    virtual void handleError(int error) {};

public: // data    
    WebCore::ResourceHandle* m_handle;
    bool m_cancelled;

protected:
    void derefHandle() {
        if (!m_handle)
            return;
        // circular deref
        WebCore::ResourceHandle* h = m_handle;
        m_handle = 0; h->deref();
        }
private:
    MUrlConnection() {}
};

MUrlConnection* CreateUrlConnection(WebCore::ResourceHandle* _handle, WebCore::Frame* frame);

#endif // __URLCONNECTION_H__
// end of file
