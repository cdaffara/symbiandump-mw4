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
* Description:  Represents the Object Widget From Webcore
*
*/


#ifndef __PLUGINSKIN_H__
#define __PLUGINSKIN_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <wtf/Vector.h>
#include <wtf/HashSet.h>
#include <npupp.h>
#include "WebCoreWidget.h"
#include "WebCannedImages.h"
#include "PluginPlayer.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "MemoryManager.h"
//#include <Element.h>

// FORWARD DECLARATIONS
class PluginWin;
class PluginStream;
class WebFrame;

namespace WebCore {
    class String;
    class Element;
}

// LOCAL CONSTANTS AND MACROS
// These are used to designate the handling of incoming content and loading Plugins
// during intial construction of the plugin. We could add ELoadModeConstruct.
// They are also used when a plugin requests to download additional content, and
// the mapping of target (windowType) to content/plugin handling. 
enum TPluginLoadMode
    {
    ELoadModeTop,
    ELoadModeSaveAsFile,
    ELoadModePlugin,
    ELoadModeNew,
    ELoadModeParent,
    ELoadModeSelf,
    ELoadModeNone
    };
 
 enum TWindowType
    {
    EWindowTypePlugin = 0,
    EWindowTypeBlank,
    EWindowTypeNew,
    EWindowTypeSelf,
    EWindowTypeCurrent,
    EWindowTypeParent,
    EWindowTypeTop,
    EWindowTypeUnknown
    };

_LIT(KBlank, "_blank");
_LIT(KNew, "_new");
_LIT(KSelf, "_self");
_LIT(KCurrent, "_current");
_LIT(KParent, "_parent");
_LIT(KTop, "_top");
    
/**
*  PluginSkin
*  Represents the Object Widget
*  This class acts as an Adapter between the webcore and the plugin engines.
*
*  @lib plugin.dll
*  @since 3.1
*/

class PluginSkin : public CBase,
                   public MWebCoreObjectWidget,
                   public MMemoryCollector

  {
    public: //  Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static PluginSkin* NewL( WebFrame& frame,
                                 const TPtrC8& url,
                                 const Vector<WebCore::String>& attributesNames,
                                 const Vector<WebCore::String>& attributeValues,
                                 const TPtrC8& baseUrl,
                                 const WebCore::String& mimeType,
                                 TBool canInteract);

        /**
        * destructor.
        */
        virtual ~PluginSkin();

    private:    // Constructors

        /**
        * C++ default constructor.
        */
        PluginSkin( WebFrame& webFrame );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const WebCore::String& mimeType,
                         const Vector<WebCore::String>& attributesNames,
                         const Vector<WebCore::String>& attributeValues,
                         const TPtrC8& url,
                         const TPtrC8& baseUrl,
                         TBool canInteract);


    public:


        /**
        * PluginWin
        * Returns a pointer to the CPluginWin object.
        * @since 3.1
        * @return The CPluginWin object
        */
        PluginWin* pluginWin() { return m_pluginwin;}


        /**
        * Frame
        * Returns a pointer to WebKitFrame object.
        * @since 3.1
        * @return The WebKitControl object
        */
        WebFrame* frame() const { return m_frame; } 
        
        
        /**
        * 
        * 
        * @since 3.1
        * @return 
        */
        void createPluginWinL(TDesC8& url, TDesC& mimetype);


        /**
        * 
        * 
        * @since 3.1
        * @return 
        */
        void loadPluginL( const TDesC8& mimetype );

        /**
        * 
        * 
        * @since 3.1
        * @return 
        */
        NPP getNPP(){return m_instance;}

        /**
        * 
        * 
        * @since 3.1
        * @return 
        */
        NPPluginFuncs* getNPPluginFucs() {return m_pluginfuncs;}

        /**
        * 
        * 
        * @since 3.1
        * @return 
        */
        void forceRedraw(bool drawNow = true);

        /**
        * 
        * 
        * @since 3.1
        * @return 
        */        
        TBool isBrowserScheme(const TPtrC8& url);
        
        /**
        *
        * GetLoadMode
        * Returns the load mode associated with the "window-type".
        */
        TPluginLoadMode GetLoadMode(const TDesC* aWindowType);
        
        /**
        *
        * GetWindowType
        * Resolves the "window-type" string into a TWindowType.
        */
        TWindowType GetWindowType(const TDesC* aWindowType);

    public: // Methods From MWebCoreObjectWidget

        void setNeedsDisplay( TBool ) {}
        
        void setCursor( CursorTypes ) {}
        /**
        * Draw
        * From MWebCoreObjectWidget
        * Repaints the frame.
        * @since 3.1
        * @param TRect& aRect
        * @return void
        */
        void draw( WebCoreGraphicsContext& gc,
                   const TRect& rect );


        /**
        * IsVisible
        * From MWebCoreObjectWidget
        * Purpose:
        * @since 3.1
        * @return void
        */
        TBool isVisible() const;

        /**
        * SizeHint
        * From MWebCoreObjectWidget
        * Purpose
        * @since 3.1
        * @return TSize
        */
        TSize sizeHint() const;

        /**
        * IsFocused
        * From MWebCoreObjectWidget
        * Purpose
        * @since 3.1
        * @return TBool
        */
        TBool isFocused() const { return ETrue; }

        /**
        * MakeVisible
        * From MWebCoreObjectWidget
        * Purpose
        * @since 3.0
        * @param TBool aVisible
        * @return void
        */
        void makeVisible(TBool visible) ;

        /**
        * SetFocus
        * From MWebCoreObjectWidget
        * Purpose
        * @since 3.1
        * @param TBool aFoucs
        * @return void
        */
        void setFocus(TBool focus) ;

        /**
        * Activate
        * From MWebCoreObjectWidget
        * Purpose Activated the plugin
        * @since 3.1
        * @param TBool aFoucs
        * @return void
        */
        void activate();

        /**
        * Rect
        * From MWebCoreObjectWidget
        * Purpose
        * @since 3.1
        * @return TRect
        */
        TRect rect() const;

        /**
        * SetRect
        * From MWebCoreObjectWidget
        * Purpose Sets the Rect
        * @since 3.1
        * @param TRect aRect
        * @return void
        */
        void setRect(const TRect& rect);


        /**
        * IsFocusable
        * From MWebCoreObjectWidget
        * Purpose Check if the plugin is focusable
        * @since 3.2
        * @param TRect aRect
        * @return void
        */
        TBool isFocusable() const;

       /**
        * SetParent
        * From MWebCoreObjectWidget
        * Set the CCoeControl parent for this WebKitBridge
        * @since 3.0
        * @return void
        */
        void setParent( MScrollView* /*aParent*/){/*iParent = aBrCtlInterface*/};

       /**
        * IsActive
        * From MWebCoreObjectWidget
        * Returns ETrue is plugin is active and receiving events
        * @since 3.1
        * @return void
        */
        TBool isActive() const { return m_active; }

        /**
        * Deactivate
        * From MWebCoreObjectWidget
        * Removes the plugin from control stack
        * @since 3.1
        * @return void
        */
        void deActivate();

        /**
        * SetFont
        * From MWebCoreObjectWidget
        * Sets the font
        * @since 3.1
        * @return void
        */
        void setFont(CFont* font);
        
        /**
        * CanInteract
        * Returns the m_canInteract flag
        * @since 5.0
        * @return TBool
        */
        TBool CanInteract() const 
        {
            return m_canInteract;
        }
        
        /**
        * pluginPlayer
        * Returns the plugin player
        * @since 5.0
        * @return PluginPlayer*
        */
        PluginPlayer* pluginPlayer() const {
            return m_frame->frameView()->topView()->pluginPlayer();
        }
        
        
        HBufC8* url() { return m_url; }
        
        void playPluginContent(const TDesC8& aRequestUrl) {};

        void positionChanged();

        void stopPluginContent(const TDesC8& aRequestUrl) {};

        void *pluginScriptableObject(); 
        
        void pluginFocusChanged(TBool focus);
        
        TBool hasResized () {return m_resized; }

        void resized();
        
        void ref() {++m_ref;};
        
        void deref() {if (--m_ref <= 0) delete this;};

        TBool loadPluginL( const TDesC& mimetype);
        
        void openInViewerL();
        
        // stream manipulation
        void addPluginStream(PluginStream*);
        void removePluginStream(PluginStream*);      
        
  public:
        //callbacks from NpnImplementation  
        int getRequestL(const TDesC8& url, bool notify, void* notifydata,const TDesC* aWindowType);                          
        int postRequestL(const TDesC8& url, const TDesC& buffer, bool fromfile, bool notify, void* notifydata,const TDesC* aWindowType);
        bool isInteractive();
        TRect getClipRect() const;
        TRect getPluginWinRect() const;
        void setClipRect(TRect rect);
        TInt handleNetworkAccess() const;
        WebCore::Element* getElement() {return m_element;}
        void setElement(WebCore::Element* aElement) {m_element = aElement;}

  public:  // from MMemoryCollector
        TUint Collect(unsigned int aRequired);
        void Restore()                          {}
        TOOMPriority Priority()                 { return EOOM_PriorityLow; }
        

        /**
        * Run javascript, possibly asynchronously.
        *
        * @since 3.2
        * @return Bool.
        */
        TBool RunScript(); 
        RArray<NPN_GenericElement>*  genericElementArray(){
            return iGenericElementArray;
        }
  private:  // private member data
        void setPluginWinClipedRect();
        TRect frameVisibleRect() const;
        void Close();
        
        const TDesC& GetExecutionMode();
        void setupGenericElementArrrayL();
        void addWidgetAttributesL();
        // Window-owning CoeControl which wraps the CoeControl created by the plugin
        PluginWin* m_pluginwin;
        WebFrame* m_frame; // not owned
        CFont* m_font;
        HBufC8* m_url;
        WebCore::Element* m_element;
        
        TBool m_visible;
        TBool m_pluginSupported;
        TBool m_pluginClosed;
        TBool m_canInteract;
        TBool m_focused;
        TBool m_active;
        TBool m_flashContent;
        
        TRect m_rect;
        TRect m_pluginWinClipRect;        
        int m_ref;

        TCannedImageData m_cannedimg;
        // Attribute lists - The plugin API expects a separate array for
        // attribute names and values
        CDesCArrayFlat *m_attributeNames;
        CDesCArrayFlat *m_attributeValues;
        //Active Object to run script
        CIdle* iJavascriptTimer;
        
        //Javascript: url 
        RPointerArray<HBufC> m_JSUrls;

        WTF::HashSet<PluginStream*>  m_streams;
        
        RArray<NPN_GenericElement>*  iGenericElementArray;
        TRect   m_oldRect;
        TRect    m_oldViewport;
        

public:
        TInt m_handle;
        NPP m_instance;    
        NPPluginFuncs* m_pluginfuncs;
        bool m_resized;
        RPointerArray<HBufC> m_tempFilesArray;
};

#endif //  __PLUGINSKIN_H__
