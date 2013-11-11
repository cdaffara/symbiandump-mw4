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
* Description:   declaration of WebPolicyManager
*
*/


#ifndef _WEBPOLICYMANAGER_H
#define _WEBPOLICYMANAGER_H

#include <e32base.h>
#include "Shared.h"
#include "FrameLoaderClient.h"

class WebFrameLoaderClient;

namespace WebCore {
};

class WebPolicyManager {
    public:
        WebPolicyManager(WebFrameLoaderClient* webFrameLoaderClient);
        ~WebPolicyManager();
        void dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction, const WebCore::String& MIMEType, const WebCore::ResourceRequest&);
        void dispatchDecidePolicyForNewWindowAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&, const WebCore::String& frameName);
        void dispatchDecidePolicyForNavigationAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&);
        void cancelPolicyCheck();
        void dispatchUnableToImplementPolicy(const WebCore::ResourceError&);
        void dispatchWillSubmitForm(WebCore::FramePolicyFunction, PassRefPtr<WebCore::FormState>);
        WebCore::ResourceError interruptForPolicyChangeError(const WebCore::ResourceRequest&);
        bool canShowMIMEType(const WebCore::String& MIMEType) const ;
        
        bool newWindowuserGesture() { return m_newWindowUserGesture; }
        TDesC* newWindowTargetName() { return m_newWindowTargetName; }

    private:
        WebFrameLoaderClient* m_webFrameLoaderClient;
        bool m_newWindowUserGesture;
        HBufC* m_newWindowTargetName;
};
    

#endif // _WEBPOLICYMANAGER_H

