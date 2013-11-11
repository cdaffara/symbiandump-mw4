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

#ifndef __RESOLVEDCONNECTION_H__
#define __RESOLVEDCONNECTION_H__

#include <e32base.h>
#include "UrlConnection.h"
#include "BrCtlDefs.h"
#include "BrCtlLinkResolver.h"

namespace WebCore {
    class Frame;
};

class CPeriodic;

class ResolvedConnection : public MUrlConnection, public MBrCtlLinkContent
{
    public: // new functions
        ResolvedConnection(WebCore::ResourceHandle* handle, WebCore::Frame* frame);
        virtual ~ResolvedConnection();
    public: // from MUrlConnection
        virtual int submit();
        virtual void cancel();
        /**
        */
        virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response);
        virtual int totalContentSize() { return m_maxSize; }

    public: // from MBrCtlLinkContent
        /**
        * Resolver calls this method when content is resolved.
        * @param aContentType The content type of the response
        * @param aCharset The charset of the response. May be empty in case of image
        * @param aContentBuf content data. Ownership is not transfered
        * @return void
        */
        virtual void HandleResolveComplete(const TDesC& aContentType,
                                           const TDesC& aCharset,
                                           const HBufC8* aContentBuf);

        /**
        * This method is called if there is some error while resolving the content
        * @param aError system wide error code.
        * @return void
        */
        virtual void HandleResolveError(TInt aError);
        void processResponse();

    private:
        void complete(int error);

    private:
        WebCore::Frame* m_frame;
        bool m_didSubmit;
        CPeriodic* m_scheduler;
        HBufC* m_contentType;
        HBufC* m_charset;
        HBufC8* m_contentBuf;
        int m_maxSize;
};



#endif // __RESOLVEDCONNECTION_H__
// end of file
