/*
* ==============================================================================
*  Name        : BrCtl.h
*  Part of     : WebKit \ BrowserControl
*  Interface   : Browser Control API
*  Description : Implementation of the Browser Control
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


#ifndef BRCTL_H
#define BRCTL_H

//  INCLUDES
#include <BrCtlInterface.h>

#include "BrCtlDefs.h"
#include "HistoryInterface.h"

#include "WmlInterface.h"
//  CONSTANTS

// DATA TYPES
enum TBrCtlLoadState
    {
    ELoadStateIdle,
    ELoadStateRequestSent,
    ELoadStateResponseInProgress,
    ELoadStateLoadingChildren
    };

const int KMainScrollbarWidth = 5;

// FORWARD DECLARATIONS
class CBrCtl;
class WebView;
class HistoryHandler;
class MWmlEngineInterface;
class MWmlInterface;
class MWidgetExtension;
class MWidgetCallback;
class SettingsContainer;
//class ScrollingProvider;
class CPeriodic;
class WebDataLoadConsumer;
class HistoryHandler;
class HttpDownload;
class CPageScaler;
class CWmlDispatcher;
class CWmlContentInterface;

namespace WebCore {
    class FormData;
}


class HistoryHandler : public HistoryCallback
{
public:
    static HistoryHandler* initWithHandle(CBrCtl* brctl);
    ~HistoryHandler();
    void updateGlobalHistoryForStandardLoad(const TPtrC8& url, const TPtrC8& requestUrl,
        TPtrC& formContentType, WebCore::FormData* formData);
    void setCurrentEntryTitle(TPtrC title);
    void updateGlobalHistoryForReload ();
    HistoryHandler(CBrCtl* brctl) { m_brctl = brctl; m_historyController = NULL; }
public: // from HistoryCallback
    virtual CCoeControl* parent();
    virtual TPoint currentPosition(); 
    virtual int currentZoomLevel(); 
    virtual int minZoomLevel(); 
    virtual bool wmlMode();
    virtual void setWmlMode(bool aWml);
    virtual void doHistoryGet(TPtrC8 url, TBrCtlDefs::TBrCtlCacheMode cacheMode);
    virtual int doHistoryPost(TPtrC8 url, TBrCtlDefs::TBrCtlCacheMode cacheMode, TPtrC contentType, WebCore::FormData* formData);
    virtual void stateChanged(bool enter);
    virtual void navigationStateChanged(TBrCtlDefs::TBrCtlState state, bool value);
    virtual CFbsBitmap* scaledPage() ;
    virtual void makeVisible(bool visible);
    virtual WebSurface* surface();
    virtual void handleWmlBackL();
    virtual bool dialogSelectOption(SelectArray* historyList);
    virtual void setUse(bool use, WebCore::FormData* formData);
    HistoryControllerInterface* historyController() { return m_historyController; }
    virtual void deferTimers(bool option);

private:
    CBrCtl* m_brctl;
    HistoryControllerInterface* m_historyController;
};


/**
*  Implementation of the Browser Control
*
*  @lib webkit.dll
*  @since 3.0
*/
class CBrCtl : public CBrCtlInterface, public MBrCtlLoadEventObserver
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
          MBrCtlSoftkeysObserver       // Default: browser does not update softkeys
          MBrCtlLinkResolver           // Must be provided if ECapabilityClientResolveEmbeddedURL or ECapabilityClientNotifyURL is set
          MBrCtlSpecialLoadObserver    // Default: DocumentHandler and SchemeDispatcher are not called by browser and connection created by Proxy Filter
          MBrCtlLayoutObserver         // Default: Browser does not update scroll location
          MBrCtlDialogsProvider        // Default: Use Browser's own dialogs
          MBrCtlWindowObserver         // Default:
          MBrCtlDownloadObserver     // Default:
        */
        static CBrCtlInterface* NewL(CCoeControl* aParent,
                                     TRect aRect,
                                     TUint aBrCtlCapabilities,
                                     TUint aCommandIdBase,
                                     MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver,
                                     MBrCtlLinkResolver* aBrCtlLinkResolver,
                                     MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver,
                                     MBrCtlLayoutObserver* aBrCtlLayoutObserver,
                                     MBrCtlDialogsProvider* aBrCtlDialogsProvider,
                                     MBrCtlWindowObserver* aBrCtlWindowObserver,
                                     MBrCtlDownloadObserver* aBrCtlDownloadObserver );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CBrCtl();

    public: // Class methods

        /**
        * Returns the parent CCoeControl
        * @since 3.0
        */
        CCoeControl* CCoeControlParent()        { return m_coectrlparent; }
        WebView* webView()              const   { return m_webView; } 
        void LoadDllWmlEngineL(); 
        void UnloadDllWmlEngine();
        MWmlEngineInterface* wmlEngineInterface() const { return m_wmlEngineInterface; }
        void setWmlMode (bool aWml);
        bool wmlMode() const {return m_wmlMode;}
        void resetWmlServiceOptions();
        void wmlServiceOptionsAddItemL( const TText* aText, TUint32 aActualElemID, TInt aIsPrev );
        void postUrlL(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType);
        void setWmlPageScaler( CPageScaler& aPageScaler ) { m_wmlPageScaler = &aPageScaler; }
        CPageScaler& wmlPageScaler() const { return *m_wmlPageScaler; }
        CWmlDispatcher* wmlDispatcher() const {return m_wmlDispatcher;}
        void setWmlDispatcher(CWmlDispatcher* dispatcher){m_wmlDispatcher = dispatcher;}
        void loadResource(TDesC8& url, CWmlContentInterface* content );
        TBrCtlWmlServiceOption* firstPrevDoElement() const { return m_firstPrevDoElement; }
        HBufC* fileNameToUrlLC(const TDesC& aFileName);
        void SetScriptLogMode(TInt aMode);
        
    public: // Methods from CBrCtlInterface

        /**
        * From CBrCtlInterface
        * Pass a command to the Browser Control
        * @since 3.0
        * @param aCommand The command that the Browser Control should process
        * @return void
        */
        IMPORT_C void HandleCommandL(TInt aCommand);

        /**
        * From CBrCtlInterface Pass a command to the Browser Control
        * @since 2.8
        * @param aCommand The command that the Browser Control should process
        * @param aTransId The transaction Id associated with this command
        * @return void
        */
        IMPORT_C void HandleDownloadCommandL(TUint16 aTransId, TBrCtlDefs::TBrCtlDownloadCmd aCommand);

        /**
        * From CBrCtlInterface
        * Return the display size that would be needed to see all the content
        * @since 3.0
        * @return The display size that would be needed to see all the content
        */
        IMPORT_C TSize ContentSize();

        /**
        * From CBrCtlInterface
        * Update a Browser Control setting
        * @since 3.0
        * @param aSetting The setting to update
        * @param aValue The new value of the setting
        * @return void
        */
        IMPORT_C void SetBrowserSettingL(TUint aSetting, TUint aValue);

        /**
        * From CBrCtlInterface
        * Get a setting from the Browser Control
        * @since 3.0
        * @param aSetting The setting to get
        * @return The value of the setting
        */
        IMPORT_C TUint BrowserSettingL(TUint aSetting);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to display the content of a buffer
        * @since 3.0
        * @param aUrl A url that refers to the data. If a url scheme is used, it must be "data:"
        * @param aData The content to display. Browser Control does not take ownership
        * @param aDataType The data type of the data
        * @param aCharsetUid The UID of the character converter of the content's charset 0 assumed Latin-1
        * @return void
        */
        IMPORT_C void LoadDataL(const TDesC& aUrl,
                                const TDesC8& aData,
                                const TDataType& aDataType,
                                TUid aCharsetUid);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to display content incrementaly
        * @since 3.0
        * @param aUrl A url that refers to the data. If a url scheme is used, it must be "data:"
        * @param aDataType The data type of the data
        * @param aCharsetUid The UID of the character converter of the content's charset 0 assumed Latin-1
        * @param aContentLength The length of the content.
        * @param aBrCtlDataLoadSupplier A callback interface to stop passing data if an error occured in the Browser Control
        * @param aBrCtlDataLoadConsumer A callback interface returned by the browser. Data should be passed to that interface
        * @return void
        */
        IMPORT_C void InitLoadDataL(const TDesC& aUrl,
                                    const TDataType& aDataType,
                                    TUid aCharsetUid,
                                    TUint aContentLength,
                                    MBrCtlDataLoadSupplier* aBrCtlDataLoadSupplier,
                                    MBrCtlDataLoadConsumer** aBrCtlDataLoadConsumer);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to load a url
        * @since 3.0
        * @param aUrl The url to load
        * @param aApId The Access Point Id to use
        * @param aBrCtlCacheMode The cache mode to use
        * @return void
        */
        IMPORT_C void LoadUrlL(const TDesC& aUrl,
                               TInt aApId = -1,
                               TBrCtlDefs::TBrCtlCacheMode aBrCtlCacheMode = TBrCtlDefs::ECacheModeNormal);

        /**
        * Requests the Browser Control to POST to a URL
        * @since 5.0
        * @param aUrl URL to which the Browser Control sends the POST request
        * @param aContentType Content type of the data to be posted
        * @param aPostData Data to be posted
        * @param aBoundary Boundary for form-data or multi-part content. 
        * @param aReserved For future use only
        * @return void
        */        
        IMPORT_C void PostUrlL (const TDesC& aUrl, 
                                const TDesC8& aContentType, 
                                const TDesC8& aPostData, 
                                const TDesC8* aBoundary, 
                                TAny* aReserved);
        /**
        * From CBrCtlInterface
        * Request the Browser Control to load and display a file from the file system
        * @since 3.0
        * @param aFileName The name of the file
        * @return void
        */
        IMPORT_C void LoadFileL(const TDesC& aFileName);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to load and display a file from the file system
        * @since 3.0
        * @param aFileHandle The handle of the file
        * @return void
        */
        IMPORT_C void LoadFileL(RFile& aFileHandle);

        /**
        * From CBrCtlInterface
        * Return the WML Options for the current page
        * @since 3.0
        * @param
        * @return The WML Options for the current page
        */
        IMPORT_C RPointerArray<TBrCtlWmlServiceOption>* WMLOptionMenuItemsL();

        /**
        * From CBrCtlInterface
        * Request the Browser Control to add its own options to the option menu
        * @since 3.0
        * @param aMenuPane Handle of the menu that will contain the options
        * @param aResourceId The resource ID of the menu
        * @param aAfter WML options should be added after that index
        * @return void
        */
        IMPORT_C void AddOptionMenuItemsL(CEikMenuPane& aMenuPane, TInt aResourceId, TInt aAfter=-1);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to clear the cache
        * @since 3.0
        * @return The number of bytes that were freed from cache
        */
        IMPORT_C TUint ClearCache();

        /**
        * From CBrCtlInterface
        * Ask the Browser Control if a URL is in cache
        * @since 3.0
        * @param aUrl The Url
        * @return ETrue if a specific Url is in cache
        */
        IMPORT_C TBool IsUrlInCache(TDesC& aUrl);

        /**
        * From CBrCtlInterface
        * Ask the Browser Control to clear a URL from the cache
        * @since 3.0
        * @param aUrl The Url
        * @return KErrNone, or an error code
        */
        IMPORT_C TInt ClearItemInCache(TDesC& aUrl);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to return info about the page. The info is returned on the Cleanup Stack.
        * @since 3.0
        * @param aBrCtlPageInfo The type of requested info
        * @return void
        */
        IMPORT_C HBufC* PageInfoLC(TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo);

        /**
        * From CBrCtlInterface
        * Query the Browser Control if there is a previous or next
        * (depending on aDirection) element in history stack
        * @since 3.0
        * @return ETrue if can go aDirection, EFalse otherwise
        */
        IMPORT_C TBool NavigationAvailable(TBrCtlDefs::TBrCtlNavigationDirection aDirection);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to save the current content to a file
        * @since 3.0
        * @param aFileName The name of the file to use for saving the content
        * @return The number of bytes in the saved file
        */
        IMPORT_C TUint16 SaveToFileL(const TDesC& aFileName);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to pass the server's certificate info for the current page
        * @since 3.0
        * @return Certificate info of the server of the current page
        */
        IMPORT_C const TCertInfo* CertInfo();

        /**
        * From CBrCtlInterface
        * Request the Browser Control to register for load events
        * @since 3.0
        * @param aBrCtlLoadEventObserver A load events observer to add
        * @return void
        */
        IMPORT_C void AddLoadEventObserverL(MBrCtlLoadEventObserver* aBrCtlLoadEventObserver);

        /**
        * From CBrCtlInterface
        * Request the Browser Control to unregister for load events
        * @since 3.0
        * @param aBrCtlLoadEventObserver A load events observer to remove
        * @return void
        */
        IMPORT_C void RemoveLoadEventObserver(MBrCtlLoadEventObserver* aBrCtlLoadEventObserver);

        /**
        * From CBrCtlInterface
        * Return the number of images in the current page
        * @since 3.0
        * @param
        * @return The number of images in the current page
        */
        IMPORT_C TInt ImageCountL();

        /**
        * From CBrCtlInterface
        * Returns the focused element
        * @since 3.0
        * @return The type of the focused element
        */
        IMPORT_C TBrCtlDefs::TBrCtlElementType FocusedElementType();

        /**
        * From CBrCtlInterface
        * Register an observer for state change changes
        * @since 3.0
        * @param The observer to register
        * @return void
        */
        IMPORT_C void AddStateChangeObserverL(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver);

        /**
        * From CBrCtlInterface
        * Unregister an observer for state changes
        * @since 3.0
        * @param The observer to unregister
        * @return void
        */
        IMPORT_C void RemoveStateChangeObserver(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver);

        /**
        * From CBrCtlInterface
        * Returns the version info requested
        * @since 3.0
        * @param The observer to unregister
        * @return The version control info
        */
        IMPORT_C HBufC* VersionInfoLC(TBrCtlDefs::TBrCtlVersionInfo aVersionInfo);

        /**
        * Pass a list of content types to the Browser Control.
        * These content types will not be using the download manager
        * The list must be separated by ';' e.g. "text/html;text/wml"
        * @since 2.8
        * @param aContentTypes The content types that should not use Download Manager
        * @return void
        */
        IMPORT_C void SetSelfDownloadContentTypesL(TDesC& aContentTypes);

        /**
        * Update a Browser Control parameter.
        * @since 2.8
        * @param aParam The parameter to update
        * @param aValue The new value of the parameter
        * @return void
        */
        IMPORT_C void SetParamL(TBrCtlDefs::TBrCtlParams aParam, const TDesC& aValue);

        /**
        * Request the Browser Control to load a saved page.
        * @since 3.0
        * @param aUid a unique identifier for the saved page
        * @param aSavedPageBuffer buffer which contains the content of the saved page
        * @return void
        */
        IMPORT_C void LoadSavedPageL(const TDesC& aUid, TDesC& aSavedPageBuffer);

        /**
        * Return the zoom levels index
        * @since 3.0
        * @return const RArray<TUint>&
        */
        IMPORT_C RArray<TUint>* ZoomLevels() const;

        /**
        * Return information about the linked feeds in the current page - used to
        * build a "Subscribe to" submenu.
        * @since 3.0
        * @param aItems Information about the linked feeds in the current page.
        * @return void
        */
        IMPORT_C const RPointerArray<TBrCtlSubscribeTo>& SubscribeToMenuItemsL();

        /**
        * Request icon for the given url
        * @since 3.1
        * @param aUrl url to a favicon or a page
        * @param aBitmapInfo info on whether to retun a favicon or a page thumbnail
        * @return favion icon or page icon
        */
        IMPORT_C CGulIcon* GetBitmapData(const TDesC& aUrl, TBrCtlDefs::TBrCtlBitmapInfo aBitmapInfo);

        /**
        * Register a client interface for the browser engine
        * @since 2.8
        * @param The observer to register
        * @return void
        */
        IMPORT_C void AddCommandObserverL(MBrCtlCommandObserver* aBrCtlCommandObserver);

        /**
        * Unregister a client interface for the browser engine
        * @since 2.8
        * @param The observer to unregister
        * @return void
        */
        IMPORT_C void RemoveCommandObserver(MBrCtlCommandObserver* aBrCtlCommandObserver);


   public: // Methods from CCoeControl

        /**
        * From CCoeControl
        * Return the minimum size the Browser Control can use
        * @since 3.0
        * @return The minimum size that the Browser Control can use
        */
        IMPORT_C TSize MinimumSize();

        /**
        * From CCoeControl
        * Pass key events to the Browser Control
        * @since 3.0
        * @param aKeyEvent The key event
        * @param aType The key code
        * @return If the Browser Control consumed the key event or not
        */
        IMPORT_C TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aEventCode);

        /**
        * From CCoeControl
        * Sets that the Browser Control has keyboard focus.
        * @since 3.0
        * @param aFocus If the Browser Control gets or loses focus
        * @param ENoDrawNow If the Browser Control should redraw itself as result of focus change
        * @return void
        */
        IMPORT_C void SetFocus(TBool aFocus, TDrawNow aDrawNow = ENoDrawNow);
        
        /*
        * This method calls to download manager in order to display query "Cancel
        * outstanding downloads?" and cancel if it requested by user
        * @param none
        * @return ETrue if it is OK to exit; EFalse - if it is not OK to exit
        */
        IMPORT_C TBool OkToExit();

        /**
        * Perform the incremental find operation
        * @since 3.0
        * @return TBrCtlFindResponse
        */
        IMPORT_C int FindKeyword( const TDesC& aKeyword );

        /**
        * Continue the incremental find operation
        * @since 3.0
        * @return TBrCtlFindResponse
        */
        IMPORT_C TBrCtlDefs::TBrCtlFindResponse FindKeywordAgain( TBool aFowardDirection );
        
        IMPORT_C MWidgetExtension* CreateWidgetExtensionL(MWidgetCallback& aWidgetCallback);
  
        /**
        * Get information about the focused image. The function leaves if the focus is not on an image
        * @since 3.2
        * @param 
        * @return The requested information
        */
        IMPORT_C TBrCtlImageCarrier* FocusedImageLC();
        
        /**
        * From CCoeControl
        * Sets that the Browser Control visible.
        * @since 3.0
        * @param aVisible If the Browser Control is visible or invisible
        * @return void
        */
        void MakeVisible(TBool aVisible);

        /**
        * From CCoeControl
        * Handle changes in size of the display.
        */
        void Draw(const TRect& aRect) const;

        MBrCtlSpecialLoadObserver* brCtlSpecialLoadObserver() const { return m_brCtlSpecialLoadObserver; }
        MBrCtlDownloadObserver* brCtlDownloadObserver();
        MBrCtlLinkResolver* brCtlLinkResolver() const { return m_brCtlLinkResolver; }
        MBrCtlDialogsProvider* brCtlDialogsProvider() const { return m_brCtlDialogsProvider; }
        MBrCtlWindowObserver* brCtlWindowObserver() const { return m_brCtlWindowObserver; }
        MBrCtlLayoutObserver* brCtlLayoutObserver() const { return m_brCtlLayoutObserver; }
        MBrCtlSoftkeysObserver* brCtlSoftkeysObserver() const { return m_brCtlSoftkeysObserver; }
        
        unsigned int commandIdBase() const { return m_commandIdBase; }
        unsigned int capabilities() const { return m_capabilities; }
        SettingsContainer* settings() const { return m_settingsContainer; }
        CBrCtl* getWindowL(TDesC& windowName, bool userGesture);
        
        void reportStateChanged(TBrCtlDefs::TBrCtlState state, bool enter);
        void showHorizontalScrollbar(bool show);
        void drawScrollbar();
        void showWindow();
        void closeWindowSoon();
        void doCloseWindowSoon();
        TBool sendCommandsToClient(TBrCtlDefs::TBrCtlClientCommands aCommand,
                                    const CArrayFix<TPtrC>& aAttributesNames,
                                    const CArrayFix<TPtrC>& aAttributeValues );
        void endLoadData();
        void updateDefaultSoftkeys();
        void setCertInfo(TCertInfo* aCertInfo) { m_certInfo = aCertInfo; }
        
        void updateScrollbars(int documentHeight, int displayHeight, int displayPosY,
            int documentWidth, int displayWidth, int displayPosX);
        bool hasHorizontalScrollbar() { return m_hasHorizontalScrollbar; }

    public: // from MBrCtlLoadEventObserver

        void HandleBrowserLoadEventL( TBrCtlDefs::TBrCtlLoadEvent aBrCtlLoadEvent,
            TUint aSize, TUint16 aTransactionId );

        HistoryHandler* historyHandler() { return m_historyHandler; }

    protected:  // Functions from CCoeControl

        /**
        * From CCoeControl
        * Handle changes in size of the display.
        */
        void SizeChanged();

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
        void doEnableFavicon();
        static TInt doCloseCb(TAny* aPtr);
        static TInt doEnableFaviconCb(TAny* aPtr);

    private:    // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CBrCtl( CCoeControl* aParent,
        TUint aBrCtlCapabilities,
        TUint aCommandIdBase,
        MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver,
        MBrCtlLinkResolver* aBrCtlLinkResolver,
        MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver,
        MBrCtlLayoutObserver* aBrCtlLayoutObserver,
        MBrCtlDialogsProvider* aBrCtlDialogsProvider,
        MBrCtlWindowObserver* aBrCtlWindowObserver,
        MBrCtlDownloadObserver* aBrCtlDownloadObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CCoeControl* aParent, TRect& aRect);

    private:
        void LoadResourceFileL();
        int getMainScrollbarWidth() const;

    private:    // Class data
        CCoeControl* m_coectrlparent;
        TRect        m_rect;
        WebView*     m_webView;
        HistoryHandler* m_historyHandler;
        SettingsContainer* m_settingsContainer;
        WebDataLoadConsumer* m_dataLoadConsumer;        
        
        RPointerArray<TBrCtlSubscribeTo> m_subscribeToItems;
        RPointerArray<HBufC> m_subscribeToUrls;
        RPointerArray<MBrCtlLoadEventObserver> iLoadEventObserversArray;
        RPointerArray<MBrCtlCommandObserver> m_commandObserverArray;

        // tot:fixme
        MBrCtlSpecialLoadObserver* m_brCtlSpecialLoadObserver;
        MBrCtlLinkResolver* m_brCtlLinkResolver;
        MBrCtlDialogsProvider* m_brCtlDialogsProvider;
        MBrCtlWindowObserver* m_brCtlWindowObserver;
        MBrCtlSoftkeysObserver* m_brCtlSoftkeysObserver;
        MBrCtlLayoutObserver* m_brCtlLayoutObserver;
        RPointerArray<MBrCtlStateChangeObserver> m_stateChangeObserverArray;
        unsigned int m_commandIdBase;
        unsigned int m_capabilities;
        TInt m_resourceFile;
        CPeriodic* m_timer;
        bool m_wmlMode;
        bool m_suspendTimers;
        MWmlEngineInterface* m_wmlEngineInterface;
        RLibrary  m_Library;
        MWmlInterface* m_WmlInterface;
        TCertInfo* m_certInfo;
        int m_documentHeight;
        int m_displayHeight;
        int m_displayPosY;
        int m_documentWidth;
        int m_displayWidth;
        int m_displayPosX;
        bool m_hasHorizontalScrollbar;
        bool m_wmlUnloadPending;
        bool m_ownsLayoutObserver;
        bool m_ownsDialogsProvider;
        CPageScaler* m_wmlPageScaler;
        CWmlDispatcher* m_wmlDispatcher;
        RPointerArray<TBrCtlWmlServiceOption>   m_wmlServiceOption;
        TBrCtlWmlServiceOption*                 m_firstPrevDoElement;
		MBrCtlDownloadObserver*                 m_brCtlDownloadObserver;

};

#endif      // BRCTL_H

// End of File
