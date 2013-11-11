/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Trolltech ASA
 * Copyright (C) 2007 Nokia
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Frame.h"

#include "Element.h"
#include "RenderObject.h"
#include "RenderWidget.h"
#include "RenderLayer.h"
#include "Page.h"
#include "Document.h"
#include "HTMLElement.h"
#include "DOMWindow.h"
#include "FrameLoadRequest.h"
#include "WebFrameLoaderClient.h"
#include "DOMImplementation.h"
#include "ResourceHandleInternal.h"
#include "Document.h"
#include "Settings.h"
#include "Plugin.h"
#include "FrameView.h"
#include "FramePrivate.h"
#include "GraphicsContext.h"
#include "HTMLDocument.h"
#include "ResourceHandle.h"
#include "FrameLoader.h"
#include "PlatformMouseEvent.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformWheelEvent.h"
#include "MouseEventWithHitTestResults.h"
#include "SelectionController.h"
#include "kjs_proxy.h"
#include "TypingCommand.h"
#include "JSLock.h"
#include "kjs_window.h"
#include "runtime_root.h"
#include "runtime.h"
#include "WebCoreFrameBridge.h"
#include "WebCoreWidget.h"

namespace WebCore {

void Frame::setBridge(WebCoreFrameBridge* bridge)
{ 
    if (d->m_bridge == bridge)
        return;

    if (d->m_bridge) {
        d->m_bridge->clearFrame();
        d->m_bridge->deref();
        d->m_bridge = 0;
    }

    d->m_bridge = bridge;
    if (d->m_bridge)
        d->m_bridge->ref();
    
}

WebCoreFrameBridge* Frame::bridge() const
{
    return d->m_bridge;
}


void Frame::issueTransposeCommand()
{
}

KJS::Bindings::Instance* Frame::createScriptInstanceForWidget(WebCore::Widget* widget)
{
    MWebCoreWidget* aView = widget->getView();
    if (!aView)
        return 0;
    void* nativeHandle = aView;
    RefPtr<KJS::Bindings::RootObject> rootObject = createRootObject(widget->getView(), scriptProxy()->interpreter());
    void* npObject = widget->getView()->pluginScriptableObject();
    if (npObject) {
        KJS::Bindings::Instance* instance = KJS::Bindings::Instance::createBindingForLanguageInstance(KJS::Bindings::Instance::CLanguage, npObject, rootObject);
        return instance;
    }
    return NULL;    
}

void Frame::clearPlatformScriptObjects()
{
}

DragImageRef Frame::dragImageForSelection() 
{
    return 0;
}
    
void Frame::dashboardRegionsChanged()
{
}

}
