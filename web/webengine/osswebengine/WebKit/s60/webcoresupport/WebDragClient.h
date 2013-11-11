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
* Description:   extension of WebCore::DragClient
*
*/

#ifndef WebDragClient_h
#define WebDragClient_h

#include "DragClient.h"

using namespace WebCore;
    

class WebDragClient : public WebCore::DragClient {
    public:
        virtual void willPerformDragDestinationAction(WebCore::DragDestinationAction, WebCore::DragData*);
        virtual void willPerformDragSourceAction(WebCore::DragSourceAction, const WebCore::IntPoint&, WebCore::Clipboard*);
        virtual WebCore::DragDestinationAction actionMaskForDrag(WebCore::DragData*);
        //We work in window rather than view coordinates here
        virtual WebCore::DragSourceAction dragSourceActionMaskForPoint(const WebCore::IntPoint& windowPoint);
        
        virtual void startDrag(WebCore::DragImageRef dragImage, const WebCore::IntPoint& dragImageOrigin, const WebCore::IntPoint& eventPos, WebCore::Clipboard*, WebCore::Frame*, bool linkDrag = false);
        virtual DragImageRef createDragImageForLink(WebCore::KURL&, const WebCore::String& label, WebCore::Frame*);
        
        virtual void dragControllerDestroyed();
        
        virtual ~WebDragClient() {};
    };
    

#endif // !WebDragClient_h

