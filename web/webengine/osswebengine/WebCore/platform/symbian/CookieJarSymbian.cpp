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
* Description:  
*
*/


#include "config.h"
#include "CookieJar.h"
#include "PlatformString.h"
#include "DeprecatedString.h"

#include "KURL.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "CookieHandler.h"

#include <e32std.h>

namespace WebCore {
    
bool cookiesEnabled()
{
    return StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cookiesEnabled();
}

String cookies( const KURL& url )
{
    if (cookiesEnabled() && StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cookieHandler()) {
        long length = 0;
        unsigned short* cookies = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cookieHandler()->cookiesForUrl(url.des(), length);
        if (cookies) {
            TPtrC cookieDes(cookies, length);
            String cookieString( cookieDes );
            User::Free( cookies );
            return cookieString;
        }
    }
    return String();
}

void setCookies( const KURL& url, const KURL& policyURL, const String& cookieStr )
{
    if (cookiesEnabled() && StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cookieHandler()) {
        // <http://bugzilla.opendarwin.org/show_bug.cgi?id=6531>, <rdar://4409034>
        // cookiesWithResponseHeaderFields doesn't parse cookies without a value
        String cookieString = cookieStr.contains('=') ? cookieStr : cookieStr + "=";
        StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cookieHandler()->addCookie( cookieString.des(),
            url.des(), policyURL.des() );
    }
}

}
