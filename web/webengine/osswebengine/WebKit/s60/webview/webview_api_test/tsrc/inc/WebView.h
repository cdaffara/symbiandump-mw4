/*
* ==============================================================================
*  Name        : WebKitView.h
*  Part of     : WEBKIT / Browser View
*  Interface   : Implemetation of CWebKitView
*  Description :
*  Version     : 3.1
*
*    Copyright (c) 2006, Nokia Corporation
*    All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions
*   are met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in
*        the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the Nokia Corporation nor the names of its
*        contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
*   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
*   DAMAGE.
*
*    Please see file patentlicense.txt for further grants.
* ==============================================================================
*/

#ifndef WEBKITVIEW_H
#define WEBKITVIEW_H

//  INCLUDES
#include <e32std.h>
#include <eikscrlb.h>
#include "Shared.h"
#include "BrCtlDefs.h"
#include "PageScaler.h"
#include "Timer.h"
#include "MemoryManager.h"

namespace WebCore
{
    class String;
    class Page;
    class DOMDocument;
    class FormState;
    class Element;
    class Frame;
}

class CPluginHandler;
class WebPreferences;
class WebBackForwardList;
class WebScriptObject;
class WebHistoryItem;
class WebFrame;
class CBrCtl;
class WebCoreGraphicsContext;
class CWebFepTextEditor;
class CPeriodic;
class CPageView;
class WebFormFill;
class WebFormFillPopup;
class CToolBar;
class WebToolBarInterface;
class WebPageScrollHandler;
class WebPopupDrawer;
class TBrCtlImageCarrier;
class PluginPlayer;
class PluginWin;
class CWidgetExtension;
class MWidgetCallback;
class WebTabbedNavigation;
class WebPageZoomHandler;
class WebPointerEventHandler;
class WebPageFullScreenHandler; 

class WebView : public CEikBorderedControl, public WebCore::Shared<WebView>, private MPageScalerCallback, public MOOMStopper
    {
    public:
        /**
        * Two-phased constructor.
        */
        static WebView* NewL( CCoeControl& aParent, CBrCtl* brctl );

        /**
        * Destructor.
        */
        virtual ~WebView();
        
        void MakeVisible(TBool visible);
        void MakeViewVisible(TBool visible);
        
        WebFrame* mainFrame();
        
        WebFrame* selectedFrame();
        
        void setMainFrameUrlL(TDesC& url);

        TDesC& mainFrameUrl();

        WebCore::DOMDocument* mainFrameDocument();

        TDesC& mainFrameTitle();
        
        WebCore::Page* page();
                     
        bool isMIMETypeRegisteredAsPlugin(const WebCore::String& MIMEType);
        
        CPluginHandler* pluginForExtension(const WebCore::String& pluginExtension);
        
        void loadBackForwardListFromOtherView(WebView* otherView);
        
        void updateWebCoreSettingsFromPreferences(WebPreferences* preferences);
        
        TBool defersCallbacks();
        
        void setDefersCallbacks(TBool defer);
        
        WebBackForwardList* backForwardList();
        
        void setMaintainsBackForwardList(TBool flag);
        
        TBool goBack();
        
        TBool goForward();
        
        TBool goToBackForwardItem(WebHistoryItem* item);
        
        void setTextSizeMultiplier(TInt multiplier);
        
        TInt textSizeMultiplier();
        
        WebScriptObject* windowScriptObject();
        
        void setGroupName(TDesC& groupName);

        TDesC& groupName();

        TInt estimatedProgress();

        TBool shouldClose();

        TBool isLoading();

        // repainting
        void syncRepaint(const TRect& rect);
        void scheduleRepaint(const TRect& rect);
        void syncRepaint();
        void doRepaint();
        void pageLoadFinished();
        void updatePageScaler();
        void doLayout();

        
        TSize maxBidiSize() const;
        void clearOffScreenBitmap();
        void scrollBuffer(TPoint aTo, TPoint aFrom, TBool aMayUseCopyScroll);
        TRect offscreenRect() const {return m_offscreenrect;}
        CPageScaler* pageScaler() const{ return m_pageScaler; }
        //void updateScrollBarsL(CEikScrollBar::TOrientation aOrientation, const TInt aThumbPos, const int aScrollSpan);
        int scalingFactor() const;
        void openUrl(const TDesC& url);
        CBrCtl* brCtl() const { return m_brctl; } 
        CWebFepTextEditor* fepTextEditor() { return m_webfeptexteditor; }
        WebFormFillPopup* formFillPopup() { return m_webFormFillPopup; }
        void setFormFillPopup(WebFormFillPopup* popup) { m_webFormFillPopup = popup; }
        WebFormFill* formFill() { return m_webFormFill; }
        void setEditable(TBool aEditable);
        bool isEditable() const { return m_isEditable; }
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aEventCode );
        void setFocusedElementType(TBrCtlDefs::TBrCtlElementType focusedElementType_) { m_focusedElementType = focusedElementType_; }
        TBrCtlDefs::TBrCtlElementType focusedElementType() const { return m_focusedElementType; }
        WebPageScrollHandler* pageScrollHandler() { return m_pageScrollHandler;}
        WebPageFullScreenHandler* pageFullScreenHandler() { return m_pageFullScreenHandler;}
        WebPointerEventHandler* pointerEventHandler() { return m_webpointerEventHandler; }
        void updateScrollbars(int documentHeight, int displayPosY,
            int documentWidth, int displayPosX);
        void openPageViewL();
        void closePageView();
        void cancelPageView();
        CPageView* pageView() const { return m_pageView; }
        bool inPageViewMode() const { return m_pageView != NULL; }
        int searchFor(const TPtrC& keyword);
        int searchAgain(bool forward);
        void exitFindState();
        bool inFindState() const { return m_inFindState; }
        bool pageScalerEnabled() const { return m_pageScalerEnabled; }
        bool isSmallPage();
        void willSubmitForm(WebCore::FormState* formState);
        
        PluginPlayer* pluginPlayer() { return m_pluginplayer; } 
        void closePluginPlayer();
        void openPluginPlayer(PluginWin* plugin);
        
        void setAccessPointId(TUint32 apid) {m_apid = apid;}        
        TUint32 accessPointId() { return m_apid; }

        CWidgetExtension* createWidgetExtension(MWidgetCallback &aWidgetCallback);
        CWidgetExtension* widgetExtension() { return m_widgetextension; }
        
        void activateVirtualKeyboard();
        
        /**
        * Display a popup with the url of an anchor
        */
        void handleShowAnchorHrefL();

        /**
        * Return the image that is under the cursor
        */
        TBrCtlImageCarrier* focusedImageLC();

        /**
        * Load the image that is under the cursor
        */
        void loadFocusedImageL();

        /**
        * Get the popupDrawer
        */
        WebPopupDrawer* popupDrawer() { return m_popupDrawer; }

        /**
        * Delete the popupDrawer
        */
        void removePopup();
        
        /**
        *  Zooming
        */
        void setZoomLevelIndex(int idx);

        RArray<TUint>* zoomLevels();

        /**
        *
        * Zooming new interface
        * @since 5.0
        * @param
        * @return
        */
        void setZoomLevel( int );

        int zoomLevel() const               { return m_currentZoomLevel; }
        void restoreZoomLevel(int zoomLevel);

        void setBitmapZoomLevel(int zoomLevel);

        void showZoomSliderL();

        void hideZoomSliderL();

        void notifyZoomSliderModeChangeL(bool);
    
        int maxZoomLevel();

        int minZoomLevel();
        
        void updateZoomLevel(TBrCtlDefs::TBrCtlSettings setting, unsigned int value);

        void checkForZoomChange();

        void updateMinZoomLevel( TSize size );

        void resetZoomLevel();

        bool historyLoad(){ return m_historyLoad;}

        void setHistoryLoad(bool value);
        
        bool redirectWithLockedHistory(){ return m_redirectWithLockedHistory;}

        void setRedirectWithLockedHistory(bool value);


        /** 
         * 
         * Fullscreen Escape Interface 
         * 
         */ 
        void EnterFullscreenBrowsingL(void); 
        void LeaveFullscreenBrowsingL(void); 
        void notifyFullscreenModeChangeL(bool); 


        void setZoomLevelAdaptively();
        void resetLastZoomLevelIfNeeded();

        /**
        * Toolbar
        **/

        void launchToolBarL();

        void closeToolBarL();
        
        /**
        * HandlePointerBufferReadyL
        * From CCoeControl
        *
        */
        void HandlePointerBufferReadyL();
        
        /**
        * HandlePointerEventL
        * From CCoeControl
        *
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
        * Set flag if the user is currently scrolling
        * 
        *
        */
        void setViewIsScrolling(bool scrolling) { m_viewIsScrolling = scrolling; }
        
        /**
        * Return if the user is currently scrolling
        * 
        *
        */
        bool viewIsScrolling() { return m_viewIsScrolling; }
        
        
                
        WebTabbedNavigation* tabbedNavigation() { return m_tabbedNavigation; }
        void notifyMetaData(WebCore::String& name, WebCore::String& value);
        void forceLayoutAndResize(WebFrame* frame);
        const TPtrC userAgent() { return *m_userAgent; } 
        void adjustOffscreenRect();
    
        /**
        * Handle to container Window
        */        
        RWindow& GetContainerWindow() {return Window();}

        // from OOMStopper
        void Stop();
        TOOMPriority Priority()     { return EOOM_PriorityHigh; }

    
    public: // from MPageScalerCallback
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void DrawDocumentPart(CFbsBitGc& aGc, CFbsBitmap* aBitmap, const TRect& aDocumentAreaToDraw);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TRect DocumentViewport() ;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ScaledPageChanged(const TRect& aArea, TBool aFullScreen, TBool aScroll);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TSize DocumentSize();

        /**
        *
        * @since 3.2
        * @param
        * @return
        */
        TBool TouchScrolling() ;

        CCoeControl& PageControlView();

     protected: // From CCoeControl

        /**
        * FocusChanged
        * From CCoeControl
        *
        */
        void FocusChanged(TDrawNow aDrawNow);

        /**
        * From CCoeControl
        * Handle changes in size of the display.
        */
        void SizeChanged();

        /**
        * From CCoeControl
        * Handle changes in size of the display.
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl
        * Get the number of controls in the Browser Control
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * Get a specified control with a given index
        */
        CCoeControl* ComponentControl(TInt aIndex) const;
                
        /**        
        * From CCoeControl
        * Fep integration for inline editing
        */
        TCoeInputCapabilities InputCapabilities() const;        
                
                
    private:
        WebView( CBrCtl* brctl );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CCoeControl& aParent );
        void initializePageScalerL();

        //
        static TInt scrollTimerCb(TAny* aAny);
        void autoScroll();
        int millisecondsScrolled() const;
        int search(TPtrC keyword, bool forward, bool wrapFlag);
        void zoomLevelChanged(int newZoomLevel);

    public:
        void fepTimerFired(WebCore::Timer<WebView>*);
        void fepVKBTimerFired(WebCore::Timer<WebView>*);
        bool isClosing() const { return m_isClosing; }
        
        void synchRequestPending(bool, WebCore::Frame*);
        bool isSynchRequestPending() const { return m_synchRequestPending; }
        WebCore::Frame* synchFrame() const { return m_synchFrame; }

        void setPluginFullscreen(bool val) { m_pluginFullscreen = val; }
        bool isPluginFullscreen() { return m_pluginFullscreen; }
    private:
        WebCore::Page* m_page;
        CBrCtl* m_brctl;
        RRegion                 m_repaints;
        CPeriodic*              m_repainttimer;    
        WebCoreGraphicsContext* m_webcorecontext;   // owned
        CFbsBitmapDevice*       m_bitmapdevice;     // onwed
        TRect                   m_offscreenrect;
        
        bool                    m_isEditable;
        CWebFepTextEditor*      m_webfeptexteditor;
        CToolBar*               m_toolbar;
        WebToolBarInterface*    m_toolbarinterface;
        
        WebFormFillPopup*       m_webFormFillPopup; // not owned
        WebPageScrollHandler*   m_pageScrollHandler;  // owned
        WebPopupDrawer*         m_popupDrawer;  // owned
        PluginPlayer*           m_pluginplayer; //owned
        WebPointerEventHandler* m_webpointerEventHandler; //owned
        
        TTime m_scrollingStartTime;
        TKeyEvent m_currentEventKey;
        CPeriodic* m_fastScrollTimer;
        int m_scrollingSpeed;
        TBrCtlDefs::TBrCtlElementType m_focusedElementType;
        CPageScaler* m_pageScaler;
        bool m_pageScalerEnabled;
        bool m_inFindState;
        CPageView* m_pageView;
        TPoint m_savedPosition; // contentPosition
        TPoint m_savedCursorPosition;
        HBufC* m_findKeyword;
        WebTabbedNavigation* m_tabbedNavigation;
        WebFormFill*            m_webFormFill;      // owned

        // for fep editor
        WebCore::Timer<WebView>* m_fepTimer;        
        TKeyEvent   m_keyevent;
        TEventCode  m_eventcode;

        RArray<TUint>   m_zoomLevelArray;
        int            m_currentZoomLevelIndex;
        TUint32         m_apid;
        
        bool       m_isClosing;
        CWidgetExtension* m_widgetextension;
        HBufC* m_userAgent;
        bool m_isdialogLaunched;
        

        WebPageZoomHandler* m_pageZoomHandler;      // owned
        int                 m_currentZoomLevel;
        int                 m_lastZoomLevel;
        int                 m_maxZoomLevel;
        int                 m_minZoomLevel;
        int                 m_defaultZoomLevel;

        // dirty zooming specific
        TRect               m_srcRectForZooming;
        TRect               m_destRectForZooming;
        int                 m_startZoomLevel;
        bool                m_dirtyZoomMode;
        bool                m_zoomLevelChangedByUser;
        bool                m_isPluginsVisible;
        bool                m_historyLoad;
        bool                m_redirectWithLockedHistory;
        // full screen mode
        WebPageFullScreenHandler* m_pageFullScreenHandler;  // owned 
        bool m_viewIsScrolling;
        TPoint* m_ptrbuffer;

        // synchronous requests 
        WebCore::Frame*              m_synchFrame;
        bool                m_synchRequestPending;
        bool                m_pluginFullscreen;
    };

#endif






