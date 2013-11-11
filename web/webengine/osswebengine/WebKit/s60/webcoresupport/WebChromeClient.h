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
* Description:   extension of WebCore::ChromeClient
*
*/


#ifndef WebChromeClient_h
#define WebChromeClient_h

#include "ChromeClient.h"

namespace WebCore {
    class FloatRect;
    class Frame;
    class IntRect;
    class Page;
    class String;
};

class WebView;
struct FrameLoadRequest;

class WebChromeClient : public WebCore::ChromeClient {
    public:
        WebChromeClient(WebView *webView);
        WebView *webView() { return m_webView; }

        virtual ~WebChromeClient() {  }
        virtual void chromeDestroyed();

        virtual void setWindowRect(const WebCore::FloatRect&) {}
        virtual WebCore::FloatRect windowRect();

        virtual WebCore::FloatRect pageRect();

        virtual float scaleFactor();

        virtual void focus() {}
        virtual void unfocus() {}

        virtual bool canTakeFocus(WebCore::FocusDirection);
        virtual void takeFocus(WebCore::FocusDirection);

        virtual WebCore::Page* createWindow(WebCore::Frame*, const WebCore::FrameLoadRequest&);
        virtual WebCore::Page* createModalDialog(WebCore::Frame*, const WebCore::FrameLoadRequest&);
        virtual void show();

        virtual bool canRunModal();
        virtual void runModal() {}

        virtual void setToolbarsVisible(bool);
        virtual bool toolbarsVisible();

        virtual void setStatusbarVisible(bool);
        virtual bool statusbarVisible();

        virtual void setScrollbarsVisible(bool);
        virtual bool scrollbarsVisible();

        virtual void setMenubarVisible(bool);
        virtual bool menubarVisible();

        virtual void setResizable(bool);

        virtual void addMessageToConsole(const WebCore::String& aMessage, WebCore::MessageLevel aMessageLevel, unsigned int aLine, const WebCore::String& aSourceURL);

        virtual bool canRunBeforeUnloadConfirmPanel();
        virtual bool runBeforeUnloadConfirmPanel(const WebCore::String& message, WebCore::Frame* frame);

        virtual void closeWindowSoon();

        virtual void runJavaScriptAlert(WebCore::Frame*, const WebCore::String&);
        virtual bool runJavaScriptConfirm(WebCore::Frame*, const WebCore::String&);
        virtual bool runJavaScriptPrompt(WebCore::Frame*, const WebCore::String& message, const WebCore::String& defaultValue, WebCore::String& result);

        virtual void setStatusbarText(const WebCore::String&);
        virtual bool shouldInterruptJavaScript() { return false; }
        virtual bool tabsToLinks() const;

        virtual WebCore::IntRect windowResizerRect() const;
        virtual void addToDirtyRegion(const WebCore::IntRect&);
        virtual void scrollBackingStore(int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect);
        virtual void updateBackingStore() {}

        virtual void mouseDidMoveOverElement(const WebCore::HitTestResult&, unsigned modifierFlags) {}

        virtual void setToolTip(const WebCore::String&) {}

        virtual void print(WebCore::Frame*) {}
        void setElementVisibilityChanged(bool visibility);

        bool elementVisibilityChanged() {
            return m_visibility;   
        }
		
		 bool elementVisibilityChangedByMouse() {
            return m_visibilityByMouse;
        }
        
        bool elementVisibilityChangedByKey() {
            return m_visibilityByKey;
        }
		
        virtual void focusedElementChanged(WebCore::Element*);
        
protected: // new functions
        void addMessageToConsoleL(const WebCore::String& aMessage, WebCore::MessageLevel aMessageLevel, unsigned int aLine, const WebCore::String& aSourceURL);
        
private:
    WebView *m_webView;
    void GetDateAndTimeL(TDes& date, TDes& time) const;
    bool m_visibility;
    bool m_visibilityByMouse;
    bool m_visibilityByKey;
};


#endif // WebChromeClient_h
