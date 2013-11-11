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
* Description:   extension of WebCore::ContextMenuClient
*
*/


#ifndef WebContextMenuClient_h
#define WebContextMenuClient_h

#include "ContextMenuClient.h"


class WebContextMenuClient : public WebCore::ContextMenuClient {
    public:
        virtual ~WebContextMenuClient() {  }
        virtual void contextMenuDestroyed();
        
        virtual WebCore::PlatformMenuDescription getCustomMenuFromDefaultItems(WebCore::ContextMenu*);
        virtual void contextMenuItemSelected(WebCore::ContextMenuItem*, const WebCore::ContextMenu*);

        virtual void downloadURL(const WebCore::KURL& url);
        virtual void searchWithGoogle(const WebCore::Frame*);
        virtual void lookUpInDictionary(WebCore::Frame*);
        virtual void speak(const WebCore::String&);
        virtual void stopSpeaking();
    };


#endif // !WebContextMenuClient_h
