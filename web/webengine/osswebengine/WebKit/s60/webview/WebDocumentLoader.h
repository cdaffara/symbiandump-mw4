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
* Description:   DocumentLoader in webkit side
*
*/


#ifndef __WEBDOCUMENTLOADER_H__
#define __WEBDOCUMENTLOADER_H__

#include <e32base.h>
#include <wtf/RefPtr.h>
#include "DocumentLoader.h"
#include <wtf/HashSet.h>

namespace WebCore
{
    struct ResourceRequest;
}

class WebView;

class WebDocumentLoader : public WebCore::DocumentLoader
{
public:

    WebDocumentLoader(const WebCore::ResourceRequest&, const WebCore::SubstituteData& );

    virtual void attachToFrame();
    virtual void detachFromFrame();

    void increaseLoadCount(unsigned long identifier);
    void decreaseLoadCount(unsigned long identifier);
    bool userWasAskedToLoadNonSecureItem() { return m_userWasAskedToLoadNonSecureItem; }
    void setUserAgreedToLoadNonSecureItem(bool userAgreed);
    bool userAgreedToLoadNonSecureItem() { return m_userAgreedToLoadNonSecureItem;}

private:
    bool m_hasEverBeenDetached;
    HashSet<unsigned long> m_loadingResources;
    bool m_userWasAskedToLoadNonSecureItem;
    bool m_userAgreedToLoadNonSecureItem;
};


#endif // __WEBDOCUMENTLOADER_H__

// END OF FILE
