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
* Description:  Acts as a container of Netscape compatible plugins.
*                It provides an adaptor interface between the S60 Browser and
*                Netscape compatible plugins.
*
*/

#ifndef PLUGINWIN_H
#define PlUGINWIN_H

//  INCLUDES
#include <e32def.h>
#include <coecntrl.h>
#include <PluginAdapterInterface.h>
#include <npapi.h>
#include <rt_gesturehelper.h>

// FORWARD DECLARATIONS
class PluginSkin;
class PluginHandler;
class PluginStream;
class WebView;
class MPluginNotifier;
class MOptionMenuHandler;
struct NPObject;

// CLASS DECLARATION

// LOCAL CONSTANTS AND MACROS
// These are used to designate the handling of incoming content and loading Plugins
// during intial construction of the plugin. We could add ELoadModeConstruct.
// They are also used when a plugin requests to download additional content, and
// the mapping of target (windowType) to content/plugin handling. Currently, the
// loadMode is simplistic, and these are sufficent. If we start to support window
// target=_Self or others, we can add ELoadModeSelf, etc.
enum TPluginEventType
    {
    EEventActivate,
    EEventDeactivate,
    EEventLoseFocus,
    EEventGainFocus,
    };

/**
* PluginWin
* Acts as a container of Netscape compatible plugins. It provides an
* adaptor interface between the S60 Browser and Netscape compatible plugins.
*
* @lib plugin.dll
* @since 3.1
*/
class PluginWin :  public CCoeControl,
                   public MCoeControlObserver,                    
                   public MCoeForegroundObserver,
                   public MPluginAdapter,
                   public MCoeControlHitTest
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @since 3.1
        * @param aHandle A unique id that if identifies the corresponding plugin.
        * @param aPluginSkin The PluginInst's listener.
        * @param aWebKitView The WebKitView.
        * @return A pointer to the newly created PluginWin object
        */
        static PluginWin* NewL(PluginSkin* pluginskin,
                               const WebView& view);

        /**
        * Destructor.
        */
        virtual ~PluginWin();

        /**
        * ConstructL
        * A part of the two phase construction method
        * @since 3.1
        * @return void.
        */
        void ConstructL(const WebView& view);


    public: // New functions

        /**
        * Refresh the plugin.
        *
        * @since 3.1
        * @return The error code from the plugin
        */
        TInt refreshPlugin(CFbsBitGc& bitmapContext,TRect aRect);


        /**
        * ResizePluginRect
        * Resize the plugin rect.
        *
        * @since 3.1
        * @param aRect the new rect
        * @return void
        */
        void resizePluginRect(TRect& rect);

 
        /**
        * pluginSkin
        * 
        *
        * @since 3.1        
        * @return 
        */
        PluginSkin* pluginSkin() { return m_pluginskin; }
        
        
        /**
        * addPluginOptionsL
        * 
        *
        * @since 3.1        
        * @return 
        */
        void addPluginOptionsL(CEikMenuPane& menuPane, TInt commandBase, TInt after);
        
        /**
        * addPluginOptionsL
        * 
        *
        * @since 3.1        
        * @return 
        */
        void handlePluginCommandL(TInt id);
        
        /**
        * SetPluginFocusL
        * Give or take focus from the plugin .
        *
        * @since 3.1
        * @param aPluginFocus if true the PluginInst instance takes focus,
        *                     otherwise it releases it.
        */
        void setPluginFocusL(TBool focus);

        /**
        * isPluginInFullscreen
        * returns if plugin in fullscreen
        *
        */        
        bool isPluginInFullscreen() { return m_fullscreen; }

        /**
        * isForceScroll
        * returns true if plugin in forceScroll mode
        */        
        bool isForceScroll() { return m_forceScroll; }
        
        /**
        * NotifyPluginVisible
        * Notify Plugins of their visibility.
        *
        */
        void NotifyPluginVisible(TBool visible);
 public: // Functions from CCoeControl

        /**
        * Returns the number of child controls.
        *
        * @since 3.1
        * @param aIndex index of child control
        * @return child control
        */
        CCoeControl* ComponentControl( TInt index )  const;

        /**
        * Returns the number of child controls.
        *
        * @since 3.1
        * @return Always returns 0
        */
        TInt CountComponentControls() const;

        /**
        * Hides or shows the PluginInst.
        *
        * @since 3.1
        * @param aVisible if true it makes the PluginInst visible otherwise invisible.
        * @return void.
        */
        void makeVisible(TBool visible);

        /**
        * Handles an event from an observed control.
        *
        * @since 3.1
        * @param aControl The control that sent the event.
        * @param aEventType The event type.
        * @return void.
        */
        void HandleControlEventL(CCoeControl *control, TCoeEvent eventType);

        /**
        * Updates the size of the control.
        *
        * @since 3.1
        * @return void
        */
        void SizeChanged();

        /**
        * Updates the size of the control.
        *
        * @since 3.1
        * @return void
        */
        void PositionChanged();

        /**
        * Updates the size or position of the control.
        *
        * @since 5.0
        * @return void
        */
        void windowChanged();

        /**
        * Updates the size or position of the control.
        *
        * @since 5.0
        * @return void
        */
        void windowChangedL();

        /**
        * Hands a key event to the control.
        *
        * @since 3.1
        * @param aKeyEvent The key event.
        * @param aKeyEvent The type of the key event.
        * @return EKeyWasNotConsumed.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& keyevent,TEventCode type);
        
        /**
        * 
        *
        * @since 3.1
        * @param 
        * @param 
        * @return 
        */
        void processEventL( TPluginEventType eventType, TBool& consumed );
        TBool HitRegionContains(const TPoint &aPoint, const CCoeControl &/*aControl*/) const;

        /**
        * HandlePointerEventL
        * From CCoeControl
        *
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

 
        /**
        * Draw
        * From CCoeControl
        *
        */        
        void Draw(const TRect& rect) const;
 
  public: //From MCoeForegroundObserver

    /**
        * HandleGainingForeground 
        * @since 3.1
        * @return none
        */
    void HandleGainingForeground();
    
    
    /**
        * HandleLosingForeground 
        * @since 3.1
        * @return none
        */
    void HandleLosingForeground();


        /**
        * Return window object usable from scriptable plugins
        * @since 3.1
        * @return none
        */
        NPObject* windowScriptNPObject();

        /**
        * Sets the plugin's behaviour as windowed or windowless
        * @since 3.2
        * @return
        */
        void setWindowedPlugin(bool windowedPlugin) { m_windowedPlugin = windowedPlugin; }

        /**
        * Sets the plugin's behaviour as windowed or windowless
        * @since 3.2
        * @return
        */
        void setTransparentPlugin(bool transparentPlugin) { m_transparentPlugin = transparentPlugin; }

        /**
        * Forces a redraw of the screen
        * @since 3.2
        * @return
        */
        void forceRedraw(bool drawNow);
        
        /**
        * Move/scroll window by aOffset
        * Sets the cursor poistion
        * @since 3.1
        * @return
        */
        void moveWindow(const TPoint& aOffset);

        /**
        * Deactivate plugin on plugin request
        * @since 3.1
        * @return
        */
        void pluginDeactivate(const TPoint& aCurPosition);        

        /**
        * zoom on double tap on the plugin
        * Sets the cursor poistion
        * @return
        */
        void adaptiveZoom(const TPoint& aCurPosition);

public: // Functions from MPluginAdapter

    /**
    * Gets the pointer to the minor version number of the plug-in version
    * number of the Browser Plug-in API.
    * @since 3.1
    * @return Series 60 returns 1. Series 80 and the Nokia 7710 smartphone return 101.
    */
    TUint16 GetVersion() {return 1;};

    /**
    * Called when the plug-in has completed its creation.
    * This function is called from the constructL function of the plug-in.
    * @since 3.1
    * @param aPluginControl A pointer to the plug-in control window
    * @return void
    */
    void PluginConstructedL(CCoeControl* aControl);

    /**
    * Called after the plug-in has been deleted.
    * This function is called from the destructor of the plug-in.
    * @since 3.1
    * @return void
    */
    void PluginFinishedL() {m_control = NULL;}

    /**
    * Called by the plug-in to open a URL in the parent frame.
    * This function is not implemented in Series 60. It is supported in Series 80 and in the Nokia 7710 smartphone.
    * @since 3.1
    * @param aUrl Name of the URL to open
    * @return void
    */
    void OpenUrlL(const TDesC& /*aUrl*/) {}

    /**
    * Called by the plug-in to open a URL in a named window or frame.
    * This function is not implemented in Series 60. It is supported in
    * Series 80 and in the Nokia 7710 smartphone.
    * @since 3.1
    * @param aUrl Name of the URL to open
    * @param aNewTargetWindowName The name of the target window or frame
    * @return void
    */
    void OpenUrlInTargetWindowL(const TDesC& /*aUrl*/,
                                const TDesC& /*aNewTargetWindowName*/) {}

    /**
    * Specifies whether or not the frame has a previous navigation history item.
    * This function is not implemented in Series 60. It is supported in Series 80
    * and in the Nokia 7710 smartphone.
    * @since 3.1
    * @return ETrue if the frame has a previous navigation history item false otherwise
    */
    TBool HasNavigationHistoryPrevious() const{return EFalse;}

    /**
    * Informs the browser to load the next navigation history URL in its parent frame.
    * This function is not implemented in Series 60. It is supported in Series 80
    * and in the Nokia 7710 smartphone.
    * @since 3.1
    * @return void
    */
    void LoadNavigationHistoryPreviousL(){}

    /**
    * Returns whether the frame has a next navigation history item.
    * This function is not implemented in Series 60. It is supported
    * in Series 80 and in the Nokia 7710 smartphone.
    * @since 3.1
    * @return Returns whether the frame has a next navigation history item.
    */
    TBool HasNavigationHistoryNext() const{return EFalse;}

    /**
    * Informs the browser to load the next navigation history URL in its
    * parent frame. This function is not implemented in Series 60. It is
    * supported in Series 80 and in the Nokia 7710 smartphone.
    * @since 3.1
    * @return void
    */
    void LoadNavigationHistoryNextL(){}

    /**
    * Returns the window system level control object for the plug-in.
    * @since 3.1
    * @return A pointer to the parent control window.
    */
    CCoeControl* GetParentControl(){return this;};

    /**
    * Returns the observer for the plug-in control.
    * @since 3.1
    * @return Observer that the plug-in can use to send events to the browser.
    */
    MCoeControlObserver* GetParentControlObserver(){return this;};

    /**
    * Sets the plug-in notifier allowing the plug-in to control portions of the browser.
    * @since 3.1
    * @param aNotifier Pointer to an interface that the browser can use to send events to the plug-in.
    * @return void
    */
    void SetPluginNotifier(MPluginNotifier* notifier) { m_notifier = notifier; }

    /**
    * Sets the plug-in option menu handler, allowing the plug-in to handle the custom
    * menu commands from the browser.
    * @since 3.1
    * @param aOptionMenuHandler Pointer to an interface that the browser can use to send
    * option menu commands to the plug-in.
    * @return void
    */
    void SetOptionMenuHandler(MOptionMenuHandler* optionmenuhandler){ m_optionmenuhandler = optionmenuhandler; }


    /**
    * PluginDataUrl  
    * @since 3.1
    * @param       
    * @return void
    */
    HBufC8* PluginDataUrl();
    
    
    void ToggleScreenMode(bool aFullScreen);
    void PlayPausePluginL ();
    void HandlePointerEventFromPluginL(const TPointerEvent& aEvent);
    TBool HandleGesture(const RT_GestureHelper::TGestureEvent& aEvent);    
	TBool Windowed() { return m_windowedPlugin;}
    protected: // New functions

        /**
        * C++ default constructor.
        *
        * @since 3.1
        * @param 
        * @param 
        */
        PluginWin(PluginSkin* pluginskin);
                               
    private:    // Data
        
        CCoeControl*                        m_control;
        MPluginNotifier*                    m_notifier;
        MOptionMenuHandler*                 m_optionmenuhandler;
                        
        PluginSkin*                         m_pluginskin;      //not owned               
        TBool                               m_pluginfocus;
        bool                                m_windowedPlugin;
        bool                                m_transparentPlugin;
        CFbsBitmap*                         m_bitmap;       //The bitmap for windowless plugin
        bool                                m_fullscreen;
        bool                                m_windowCreated;
        bool                                m_forceScroll;
    };


#endif      // PluginWin_H

// End of File
