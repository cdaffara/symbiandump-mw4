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
#include "Page.h"

#include "Frame.h"
#include "FloatRect.h"
#include "Screen.h"
#include "WebCorePageBridge.h"

namespace WebCore {

Page::Page(CWebCorePageBridge* bridge)
: m_frameCount(0)
, m_bridge(bridge)

{
    init();
}

// These methods scale between window and WebView coordinates because JavaScript/DOM operations
// assume that the WebView and the window share the same coordinate system.

FloatRect Page::windowRect() const
{
    return FloatRect();
    //return scaleScreenRectToWidget(flipScreenRect([bridge() windowFrame]), widget());
}

void Page::setWindowRect(const FloatRect& r)
{
    //[bridge() setWindowFrame:flipScreenRect(scaleWidgetRectToScreen(r, widget()))];
}

}
