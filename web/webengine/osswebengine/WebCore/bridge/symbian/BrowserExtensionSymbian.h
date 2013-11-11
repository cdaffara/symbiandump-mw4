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

#include "BrowserExtension.h"

namespace WebCore {

class Frame;
class FrameSymbian;

class BrowserExtensionSymbian : public BrowserExtension
    {
    public:
        BrowserExtensionSymbian(Frame*);

        virtual void createNewWindow(const ResourceRequest&);
        virtual void createNewWindow(const ResourceRequest&,
                                     const WindowArgs&,
                                     Frame*& part);

        virtual void setIconURL(const KURL& url);
        virtual void setTypedIconURL(const KURL& url, const String&  type);

        virtual int getHistoryLength();
        virtual void goBackOrForward(int distance);
        virtual KURL historyURL(int distance);

        virtual bool canRunModal() { return EFalse; }
        virtual bool canRunModalNow() { return EFalse; }
        virtual void runModal() {}

    private:
         void createNewWindow(const ResourceRequest&,
                              const WindowArgs&,
                              Frame** part);

        FrameSymbian *m_frame;
    };

}
