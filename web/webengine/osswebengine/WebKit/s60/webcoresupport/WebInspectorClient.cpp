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
* Description:   Implementation of WebInspectorClient
*
*/

#include <../bidi.h>
#include "config.h"
#include "WebInspectorClient.h"
#include "DebugStream.h"

namespace WebCore {

void WebInspectorClient::inspectorDestroyed()
{
/*
    [[m_windowController.get() webView] close];
*/
    delete this;
}

Page* WebInspectorClient::createPage()
{
/*
    if (!m_windowController)
        m_windowController.adoptNS([[WebInspectorWindowController alloc] initWithInspectedWebView:m_webView]);

    return core([m_windowController.get() webView]);
*/
    notImplemented();
    return 0;
}

void WebInspectorClient::showWindow()
{
/*
    updateWindowTitle();
    [m_windowController.get() showWindow:nil];
*/
    notImplemented();
}

void WebInspectorClient::closeWindow()
{
/*
    [m_windowController.get() close];
*/
    notImplemented();
}

void WebInspectorClient::attachWindow()
{
/*
    [m_windowController.get() attach];
*/
    notImplemented();
}

void WebInspectorClient::detachWindow()
{
/*
    [m_windowController.get() detach];
*/
    notImplemented();
}

void WebInspectorClient::highlight(Node* node)
{
/*
    [m_windowController.get() highlightAndScrollToNode:kit(node)];
*/
    notImplemented();
}

void WebInspectorClient::hideHighlight()
{
/*
    [m_windowController.get() hideHighlight];
*/
    notImplemented();
}

void WebInspectorClient::inspectedURLChanged(const String&)
{
/*
    m_inspectedURL = newURL;
    updateWindowTitle();
*/
    notImplemented();
}
/*
void WebInspectorClient::updateWindowTitle() const
{
    [[m_windowController.get() window] setTitle:[NSString stringWithFormat:@"Web Inspector %C %@", 0x2014, (NSString *)m_inspectedURL]];
}
*/
}
