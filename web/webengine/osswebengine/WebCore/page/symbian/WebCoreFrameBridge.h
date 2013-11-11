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

#ifndef __WEBCOREFRAMEBRIDGE_H__
#define __WEBCOREFRAMEBRIDGE_H__

#include "Shared.h"
#include <wtf/Vector.h>
#include "BrCtlDefs.h"

namespace WebCore {
    class Node;
    class Frame;
    class HTMLFrameOwnerElement;
    class Page;
    class String;
    class IntRect;
}

namespace KJS {
    class JSObject;
}

class MScrollView;
class MWebCoreWidget;
class TPtrC8;
class TRect;

typedef enum ObjectElementType {
    ObjectElementNone,
    ObjectElementImage,
    ObjectElementFrame,
    ObjectElementPlugin
} m_elementType;

typedef enum WebCoreTextSize
{
    EFontAllSmall = 0,
    EFontSmaller,
    EFontNormal,
    EFontLarger,
    EFontAllLarge
} m_textSize;

class WebCoreFrameBridge : public WebCore::Shared<WebCoreFrameBridge>
{
public:

    virtual MScrollView* documentView() const = 0;
    virtual WebCore::Frame* createChildFrameNamed(const WebCore::String* frameName, const TPtrC8 withURL, const TPtrC referrer,
        WebCore::HTMLFrameOwnerElement* ownerElement, bool allowsScrolling, int marginWidth, int marginHeight) = 0;
    virtual ObjectElementType determineObjectFromMIMEType(const WebCore::String& MIMEType, const TPtrC8 URL) = 0;    
    virtual void ref() = 0;
    virtual void deref() = 0;   

    WebCore::Frame* frame();
    void close();
    void addData(const char* data, int length);
    void receivedData(const char* data, int length, WebCore::String textEncodingName);
    void clearFrame();
    TPtrC8 baseURL();
    void createFrameViewWithScrollView(MWebCoreWidget* view, int marginWidth, int marginHeight);
    void forceLayoutAdjustingViewSize(bool flag);
    void forceLayoutWithMinimumPageWidth(float minPageWidth, float maxPageWidth, bool adjustingViewSize);
    void sendResizeEvent();
    void sendScrollEvent();
    void installInFrame(MWebCoreWidget* view);
    void setShouldCreateRenderers(bool f);
    bool shouldCreateRenderers();
    bool needsLayout();
    void setNeedsLayout();                    
    void updateFocusableRectList();
    void updateThumbnail();
    Vector<WebCore::IntRect>* focusableRectList() { return &m_focusableNodeList; }
    bool searchFor(TPtrC string, bool forward, bool caseSensitive, bool wrapFlag, bool startInSelection);
    bool getTypeFromElement(WebCore::Node* node, TBrCtlDefs::TBrCtlElementType& aElType, TRect& aFocusRect, WebCore::Node*& aRNode) const;
    
    virtual ~WebCoreFrameBridge();

    int textZoomPercent() const;
    void scalingFactorChanged(int factor);
    int maxBidiWidth();
    void setWritingDirectionRtl(bool isRtl);
    virtual void notifyMetaData(WebCore::String& name, WebCore::String& value) = 0;
    virtual unsigned getHistoryLength() = 0;
    virtual void scheduleHistoryNavigation(int distance) = 0;
    
    void addJSExtension(const TDesC& id, KJS::JSObject* obj);

protected: 

    WebCoreFrameBridge();
    void init();
    void finishInitializingWithPage();

    WebCore::Frame*              m_frame;                    // frame

private:

    WebCoreFrameBridge(const WebCoreFrameBridge&);                 // not implemented
    WebCoreFrameBridge& operator=(const WebCoreFrameBridge&);      // not implemented
    
    bool                         m_closed;
    bool                         m_shouldCreateRenderers;
    Vector<WebCore::IntRect>     m_focusableNodeList;

};

#endif // __WEBCOREFRAMEBRIDGE_H__

// END OF FILE




