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
* Description:  Implemetation of CWebKitView
*
*/

#ifndef __WEBUTIL_H__
#define __WEBUTIL_H__

#include <e32std.h>
#include "BrCtlDefs.h"

class WebFrame;
class WebView;
class TBrCtlImageCarrier;
class TBrCtlSubscribeTo;
class CEikMenuPane;
//
namespace WebCore {
    class Node;
    class String;
    class IntPoint;
    class Frame;
    class RenderImage;
    class Element;
}

TBrCtlDefs::TBrCtlElementType nodeTypeB(WebCore::Node* node, WebCore::Frame* frame);
WebCore::String getNodeUrlAtPointInFrame(WebFrame& f_, WebCore::IntPoint& p_);
WebFrame* frameAndPointUnderCursor(WebCore::IntPoint& p_, WebView& v_);
int imageCountInFrame(WebFrame& wf_, bool visibleOnly_);
CArrayFixFlat<TBrCtlImageCarrier>* imagesInFrame(WebFrame& wf_, bool visibleOnly_);
CArrayFixFlat<TBrCtlSubscribeTo>* findSubscribeToInFrame(WebFrame& wf_);
int focusedImage(WebView* webView, TBrCtlImageCarrier*& aImageCarrier);
void loadFocusedImage(WebView* webView);
WebCore::RenderImage* renderImageUnderCursor(WebView* webView);
HBufC* generateFrameName();
void addOneMenuItemAfter(CEikMenuPane& menuPane, unsigned int after, int command, int resourceId, unsigned int commandBase);
void insertOneMenuItem(CEikMenuPane& menuPane, int command, int resourceId, unsigned int commandBase);
int textMultiplier(int fontLevel, int originalSize);
void addFocusedUrlToContacts(WebView* webView);
int mapHttpErrors(int err );
bool handleSelectElementScrolling(WebView* webView, int tb);

#endif // END OF FILE

