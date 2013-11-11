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
* Description:  ResourceRequest
*
*/


#ifndef ResourceRequest_h
#define ResourceRequest_h

#include "ResourceRequestBase.h"
#include "FormData.h"
#include "KURL.h"
#include "HTTPHeaderMap.h"

namespace WebCore {

    struct ResourceRequest : ResourceRequestBase {

        ResourceRequest(const String& url, bool mainLoad = false) 
            : ResourceRequestBase(KURL(url.deprecatedString()), UseProtocolCachePolicy)
            , m_mainLoad(mainLoad)
        {
        }

        ResourceRequest(const KURL& url, bool mainLoad = false) 
            : ResourceRequestBase(url, UseProtocolCachePolicy)
            , m_mainLoad(mainLoad)
        {
        }

        ResourceRequest(const KURL& url, const String& referrer,  bool mainLoad = false, ResourceRequestCachePolicy policy = UseProtocolCachePolicy) 
            : ResourceRequestBase(url, policy)
            , m_mainLoad(mainLoad)
        {
            setHTTPReferrer(referrer);
        }
        
        ResourceRequest()
            : ResourceRequestBase(KURL(), UseProtocolCachePolicy)
            , m_mainLoad(false)
        {
        }

        void setMainLoad(bool mainLoad) { m_mainLoad = mainLoad; }
        bool mainLoad() const { return m_mainLoad; }

        void doUpdatePlatformRequest() const {}
        void doUpdateResourceRequest() const {}
        bool m_mainLoad;
    };

} // namespace WebCore

#endif // ResourceRequest_h
