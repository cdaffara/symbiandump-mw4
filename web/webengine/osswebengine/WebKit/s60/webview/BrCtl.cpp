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
* Description:   Implementation of the Browser Control
*
*/

// INCLUDE FILES
#include <Browser_platform_variant.hrh>
#include <../bidi.h>
#include <avkon.hrh>
#include <apmrec.h>
#include <apgcli.h>
#include <badesca.h>
#include <bautils.h>
#include <StringLoader.h>
#include <AknUtils.h>
#if defined(__BROWSER_SDK)
#include <webkit_sdk.rsg>
#else
#include <Webkit.rsg>
#endif
#include <gulicon.h>
#include <e32uid.h>
#include <BrowserDialogsProvider.h>
//tot:fixme
//#include <oom.h>

#include "config.h"
#include "BrCtl.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebDataLoadConsumer.h"
#include "WebFormFillPopup.h"
#include "WebPageFullScreenHandler.h"
#include "pagescaler.h"
#include "HistoryController.h"
#include "HistoryEntry.h"
#include "HistoryInterface.h"
#include "HistoryView.h"
#include "FormData.h"
#include "data_caging_path_literals.hrh"
#include "StaticObjectsContainer.h"
#include "WebIconDatabase.h"
#include "ResourceRequest.h"
#include "FormFillController.h"
#include "SettingsContainer.h"
#include "Frame.h"
#include "FrameTree.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "HTMLInputElement.h"
#include "Document.h"
#include "DocLoader.h"
#include "Node.h"
#include "Element.h"
#include "WebCursor.h"
#include "WebUtil.h"
#include "WebCharsetData.h"
#include "httpcachemanager.h"
#include "ResourceLoaderDelegate.h"
#include "EventHandler.h"
#include "Timer.h"
#include "Page.h"
#include "Range.h"
#include "FocusController.h"
#include "IconDatabase.h"
#include "HttpDownload.h"
#include "BrCtlSoftkeysObserverImpl.h"
#include "BrCtlSpecialLoadObserverImpl.h"
#include "BrCtlLayoutObserverImpl.h"
#include "BrCtlWindowObserverImpl.h"
#include "WidgetExtension.h"
#include "PluginSkin.h"
#include "HttpUiCallbacks.h"
#include "PluginWin.h"
#include <CUserAgent.h>

#ifndef BRDO_WML_DISABLED_FF
#include "WmlInterface.h"
#include "WmlInterfaceImpl.h"
#include "WmlEngineInterface.h"
#endif
#include "WmlDispatcher.h"

#ifndef BRDO_WML_DISABLED_FF
class CWmlContentInterface;
#endif
class TBrCtlWmlServiceOption;

const TUint KMinimumHeight = 20;
const TUint KMinimumWidth  = 20;
const TRgb KGray = TRgb(150, 150, 180);
const TRgb KDarkGray = TRgb(90, 90, 150);
const TRgb KBlack = TRgb(0x00, 0x00, 0x00);
const TRgb KWhite = TRgb(0xff, 0xff, 0xff);
const TInt KForward = 1;
const TInt KBack = -1;

_LIT( KDriveZ, "Z:");

typedef struct
{
    TBrCtlDefs::TBrCtlElementType iFocusedElementType;
    TInt iCommandId;
    TInt iResourceId;
} TCommandsArray;

static const TCommandsArray commandsArray[] =
    {
    {TBrCtlDefs::EElementFileSelectionBoxWithContent, TBrCtlDefs::ECommandRemoveFileName, R_TEXT_WML_LIST_REMOVE_FILE}
    };

using namespace WebCore;


void BrCtlPanic(TInt aError = KErrGeneral)
{
    User::Panic(_L("BrowserControl Panic"), aError);
}

TBrCtlWmlServiceOption::TBrCtlWmlServiceOption(HBufC* aText, TUint aElemID, TUint aActualElemID)
        : iText(aText), iElemID(aElemID), iActualElemID(aActualElemID)
{
}

TBrCtlWmlServiceOption::~TBrCtlWmlServiceOption()
{
    delete iText;
}

CCoeControl* HistoryHandler::parent()
{
    return m_brctl;
}

HistoryHandler* HistoryHandler::initWithHandle(CBrCtl* brctl)
{
    HistoryHandler* self = new HistoryHandler(brctl);
    if (self) {
        self->m_historyController = HistoryController::initWithCallback( self,
            self->m_brctl->capabilities() & TBrCtlDefs::ECapabilityGraphicalHistory, false);
    }
    return self;
}

HistoryHandler::~HistoryHandler()
{
    delete m_historyController;
}

void HistoryHandler::updateGlobalHistoryForStandardLoad(const TPtrC8& url, const TPtrC8& requestUrl,
        TPtrC& formContentType, WebCore::FormData* formData)
{
    m_historyController->insert(url, requestUrl, formContentType, formData);
}

void HistoryHandler::setCurrentEntryTitle(TPtrC title)
{
    m_historyController->setCurrentEntryTitle(title);
}

void HistoryHandler::updateGlobalHistoryForReload ()
{
    m_historyController->updateGlobalHistoryForReload();
}

TPoint HistoryHandler::currentPosition()
{
    return m_brctl->webView()->mainFrame()->frameView()->contentPos();
}

int HistoryHandler::currentZoomLevel()
{
    return m_brctl->webView()->scalingFactor();
}
int HistoryHandler::minZoomLevel()
{
    return m_brctl->webView()->minZoomLevel();
}
bool HistoryHandler::wmlMode()
{
    return m_brctl->wmlMode();
}

void HistoryHandler::setWmlMode(bool aWml)
{
    m_brctl->setWmlMode(aWml);
}

void HistoryHandler::doHistoryGet(TPtrC8 url, TBrCtlDefs::TBrCtlCacheMode cacheMode)
{
    m_brctl->webView()->setHistoryLoad(true);
    m_brctl->webView()->mainFrame()->loadURL(url, cacheMode, String());
}

int HistoryHandler::doHistoryPost(TPtrC8 url, TBrCtlDefs::TBrCtlCacheMode cacheMode,
                                   TPtrC contentType, WebCore::FormData* formData)
{
    ResourceRequestCachePolicy cachePolicy = ReturnCacheDataElseLoad;
    if (cacheMode == TBrCtlDefs::ECacheModeNoCache) {
        cachePolicy = ReloadIgnoringCacheData;
    }
    int ret = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->uiCallback()->aboutToLoadPage(m_brctl, HttpUiCallbacks::ERepostConfirmation);
    if (ret == KErrNone) {
        ResourceRequest request = (KURL(url));
        request.setMainLoad(true);
        request.setCachePolicy(cachePolicy);
        request.setHTTPMethod("POST");
        request.setHTTPContentType(contentType);
        request.setHTTPBody(formData);
        m_brctl->webView()->setHistoryLoad(true);
        m_brctl->webView()->mainFrame()->loadRequest(request);
    }
    return ret;
}

void HistoryHandler::stateChanged(bool enter)
{
    m_brctl->reportStateChanged(TBrCtlDefs::EStateHistoryView, enter);
}

void HistoryHandler::navigationStateChanged(TBrCtlDefs::TBrCtlState state, bool value)
{
    m_brctl->reportStateChanged(state, value);
}

CFbsBitmap* HistoryHandler::scaledPage()
{   
    CFbsBitmap* scaledPage = NULL;   
    if ( m_brctl->wmlMode() && &m_brctl->wmlPageScaler() ) {
        scaledPage = m_brctl->wmlPageScaler().ScaledPage();
    }
    else if( !m_brctl->wmlMode() && m_brctl->webView()->pageScaler() ) {
        scaledPage = m_brctl->webView()->pageScaler()->ScaledPage();
    }
    return scaledPage;    
}

void HistoryHandler::makeVisible(bool visible)
{
    if(wmlMode()) {
#ifndef BRDO_WML_DISABLED_FF
        m_brctl->wmlEngineInterface()->MakeVisible( visible );
#endif
    }
    else {
        m_brctl->webView()->MakeVisible( visible );
    }
}

void CBrCtl::updateScrollbars(int documentHeight, int displayHeight, int displayPosY,
                int documentWidth, int displayWidth, int displayPosX)
{
  if (!AknLayoutUtils::PenEnabled()) {
    m_documentHeight = documentHeight;
    m_displayHeight = displayHeight;
    m_displayPosY = displayPosY;
    m_documentWidth = documentWidth;
    m_displayWidth = displayWidth;
    m_displayPosX = displayPosX;
    if (displayWidth < documentWidth) {
        // add scrollbar if needed
        if (!m_hasHorizontalScrollbar) {
            m_hasHorizontalScrollbar = true;
            showHorizontalScrollbar(true);
        }
    }
    else {
        // remove scrollbar if not needed
        if (m_hasHorizontalScrollbar) {
            m_hasHorizontalScrollbar = false;
            showHorizontalScrollbar(false);
        }
    }
  }
}

void CBrCtl::Draw(const TRect& aRect) const
{
}


WebSurface* HistoryHandler::surface()
{
    return StaticObjectsContainer::instance()->webSurface();
}

void HistoryHandler::handleWmlBackL()
{
#ifndef BRDO_WML_DISABLED_FF
    // tot:fixme
  // First check if back should be handled by a DO element
  if ( m_brctl->firstPrevDoElement() )
    {
    m_brctl->wmlEngineInterface()->HandleDo( m_brctl->firstPrevDoElement()->ActualElemID() );
    }
  else
    {
    m_brctl->wmlEngineInterface()->ProcessBack();
    }
#endif
}

bool HistoryHandler::dialogSelectOption(SelectArray* historyList)
{
    HBufC* title = StringLoader::LoadLC( R_HISTORY_POPUP_HEADING_PANE );
    // Display history dialog
    TBool ret( m_brctl->brCtlDialogsProvider()->DialogSelectOptionL(*title, ESelectTypeNone, *historyList));
    CleanupStack::PopAndDestroy(); // title
    return ret;
}

void HistoryHandler::setUse(bool use, WebCore::FormData* formData)
{
    if (use) {
        formData->ref();
    }
    else {
        formData->deref();
    }
}

void HistoryHandler::deferTimers(bool option)
{
    setDeferringTimers(option);
}

TBrCtlSubscribeTo::TBrCtlSubscribeTo(const TPtrC& aTitle, const TPtrC& aUrl,
        TUint aCommandID)
        : iTitle(aTitle), iUrl(aUrl), iCommandID(aCommandID)
    {
    }

TBrCtlSubscribeTo::TBrCtlSubscribeTo()
: iTitle(KNullDesC()), iUrl(KNullDesC()), iCommandID(0)
    {
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBrCtl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBrCtlInterface* CBrCtl::NewL(
  CCoeControl* aParent,
    TRect aRect,
    TUint aBrCtlCapabilities,
    TUint aCommandIdBase,
    MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver,
    MBrCtlLinkResolver* aBrCtlLinkResolver,
    MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver,
    MBrCtlLayoutObserver* aBrCtlLayoutObserver,
    MBrCtlDialogsProvider* aBrCtlDialogsProvider,
    MBrCtlWindowObserver* aBrCtlWindowObserver,
    MBrCtlDownloadObserver* aBrCtlDownloadObserver )
{
    CBrCtl* self = new (ELeave) CBrCtl(aParent,
                                       aBrCtlCapabilities,
                                       aCommandIdBase,
                                       aBrCtlSoftkeysObserver,
                                       aBrCtlLinkResolver,
                                       aBrCtlSpecialLoadObserver,
                                       aBrCtlLayoutObserver,
                                       aBrCtlDialogsProvider,
                                       aBrCtlWindowObserver,
                                       aBrCtlDownloadObserver );
    CleanupStack::PushL(self);
    self->ConstructL(aParent, aRect);
    CleanupStack::Pop(); //self

    return self;
}

// -----------------------------------------------------------------------------
// CBrCtl::CBrCtl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBrCtl::CBrCtl(
  CCoeControl* aParent,
  TUint aBrCtlCapabilities,
  TUint aCommandIdBase,
  MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver,
  MBrCtlLinkResolver* aBrCtlLinkResolver,
  MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver,
  MBrCtlLayoutObserver* aBrCtlLayoutObserver,
    MBrCtlDialogsProvider* aBrCtlDialogsProvider,
    MBrCtlWindowObserver* aBrCtlWindowObserver,
    MBrCtlDownloadObserver* aBrCtlDownloadObserver )
   : m_coectrlparent(aParent)
   , m_brCtlSpecialLoadObserver(aBrCtlSpecialLoadObserver)
   , m_brCtlLinkResolver(aBrCtlLinkResolver)
   , m_brCtlDialogsProvider(aBrCtlDialogsProvider)
   , m_brCtlWindowObserver(aBrCtlWindowObserver)
   , m_brCtlSoftkeysObserver(aBrCtlSoftkeysObserver)
   , m_brCtlLayoutObserver(aBrCtlLayoutObserver)
   , m_commandIdBase(aCommandIdBase)
   , m_capabilities(aBrCtlCapabilities)
   , m_suspendTimers(false)
   , m_wmlEngineInterface(NULL)
   , m_brCtlDownloadObserver(aBrCtlDownloadObserver)
   , m_windoCloseTimer(NULL)
{
    m_documentHeight = 0;
    m_displayHeight = 0;
    m_displayPosY = 0;
    m_documentWidth = 0;
    m_displayWidth = 0;
    m_displayPosX = 0;
    m_hasHorizontalScrollbar = false;
    m_ownsSpecialLoadObserver = false;
    m_ownsSoftkeysObserver = false;    
    m_ownsLayoutObserver = false;
    m_ownsDialogsProvider = false;
    m_ownsWindowObserver = false;
}

// -----------------------------------------------------------------------------
// CBrCtl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBrCtl::ConstructL(
    CCoeControl* aParent,
    TRect& aRect)
{
    IconDatabase::delayDatabaseCleanup();
    // and enable cleanup
    m_timer = CPeriodic::NewL(CActive::EPriorityIdle);
    m_timer->Start(20000, 0, TCallBack(&doEnableFaviconCb,this));
    //
    StaticObjectsContainer::instance()->ref(*this);
    StaticObjectsContainer::instance()->setCapabilities(m_capabilities);
    StaticObjectsContainer::instance()->sharedIconDatabase();
    SetContainerWindowL( *aParent );
    m_rect = aRect;
    m_webView = WebView::NewL(*aParent, this);
    m_historyHandler = HistoryHandler::initWithHandle(this);
    m_settingsContainer = new (ELeave) SettingsContainer(m_webView, m_historyHandler->historyController());
    m_settingsContainer->setNavigationType((capabilities()&TBrCtlDefs::ECapabilityCursorNavigation) ? SettingsContainer::NavigationTypeCursor : SettingsContainer::NavigationTypeTabbed);

    m_usrAgnt = CUserAgent::NewL();

    // Create and initialize the Special Load Observer
    if (m_brCtlSpecialLoadObserver == NULL)
        {
        m_brCtlSpecialLoadObserver = new (ELeave) CBrCtlSpecialLoadObserver();
        m_ownsSpecialLoadObserver = true;
        }
        
        
    // Setup default support, if it wasn't passed into the constructor...
    // Create and initialize the Softkey Observer
    if (m_brCtlSoftkeysObserver == NULL)
        {
        m_brCtlSoftkeysObserver = new (ELeave) CBrCtlSoftkeysObserver();
        m_ownsSoftkeysObserver = true;
        }
        
    // Create and initialize the Layout Observer
    if (m_brCtlLayoutObserver == NULL)
        {
        m_brCtlLayoutObserver = new (ELeave) CBrCtlLayoutObserver();
        m_ownsLayoutObserver = true;
        }
    // Create and initialize the Dialog Provider
    if (m_brCtlDialogsProvider == NULL)
        {
        m_brCtlDialogsProvider = CBrowserDialogsProvider::NewL(NULL);
        m_ownsDialogsProvider = true;
        }
        
  // window observer
    if(m_brCtlWindowObserver == NULL)
        {
        m_brCtlWindowObserver = new (ELeave) CBrCtlWindowObserver();
        m_ownsWindowObserver = true;
        }
    
    LoadResourceFileL();
    // Set the rect for BrowserControl (a CCoeControl).
    SetRect(aRect);
    CCoeEnv::Static()->DisableExitChecks(true);
    Window().AllocPointerMoveBuffer(256, 0);
    Window().DisablePointerMoveBuffer();
    Window().PointerFilter(EPointerFilterMove | EPointerFilterDrag | EPointerFilterEnterExit, 0);
    ActivateL();
}

// -----------------------------------------------------------------------------
// CBrCtl::~CBrCtl
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CBrCtl::~CBrCtl()
{
    iLoadEventObserversArray.Close();
    iLoadEventObserversArray.Close();
    m_stateChangeObserverArray.Close();
    m_commandObserverArray.Close();
    m_subscribeToItems.ResetAndDestroy();
    m_subscribeToItems.Close();
    m_subscribeToUrls.ResetAndDestroy();
    m_subscribeToUrls.Close();

    delete m_webView;
    delete m_historyHandler;
    delete m_settingsContainer;
    delete m_usrAgnt;
    
    if (m_ownsSpecialLoadObserver) {
        delete (CBrCtlSpecialLoadObserver*)m_brCtlSpecialLoadObserver;
    }
    
    if (m_ownsSoftkeysObserver) {
        delete (CBrCtlSoftkeysObserver*)m_brCtlSoftkeysObserver;
    }
    
    if (m_ownsWindowObserver) {
        delete (CBrCtlWindowObserver*)m_brCtlWindowObserver;
    }

    if (m_ownsLayoutObserver) {
        delete (CBrCtlLayoutObserver*)m_brCtlLayoutObserver;
    }
    if (m_ownsDialogsProvider) {
        delete (CBrowserDialogsProvider*)m_brCtlDialogsProvider;
    }
    if (m_timer) {
        m_timer->Cancel();
        delete m_timer;
    }
    if (m_windoCloseTimer) {
        m_windoCloseTimer->Cancel();
        delete m_windoCloseTimer;
    }
    
    if (m_dataLoadConsumer) {
        m_dataLoadConsumer->stopDataLoad();
        endLoadData();
    }
    m_brCtlLayoutObserver = NULL;
    m_brCtlDialogsProvider = NULL;
    m_brCtlDownloadObserver = NULL;
    m_brCtlWindowObserver = NULL;
    m_brCtlSoftkeysObserver = NULL; 
    m_brCtlSpecialLoadObserver = NULL;

    UnloadDllWmlEngine();

    // remove resource file
    CCoeEnv::Static()->DeleteResourceFile(m_resourceFile);
    StaticObjectsContainer::instance()->deref(*this);
}

// -----------------------------------------------------------------------------
// CBrCtl::HandleBrowserLoadEventL
// Public Class Method
// Update the load event observers with a new event
// -----------------------------------------------------------------------------
//
void CBrCtl::HandleBrowserLoadEventL( TBrCtlDefs::TBrCtlLoadEvent aLoadEvent, TUint aSize, TUint16 aTransactionId )
{
    for (int i = 0; i < iLoadEventObserversArray.Count(); i++)
        iLoadEventObserversArray[i]->HandleBrowserLoadEventL(aLoadEvent, aSize, aTransactionId);

    switch (aLoadEvent) {
        case TBrCtlDefs::EEventNewContentStart:
            if (m_webView->pageScalerEnabled())
                m_webView->pageScaler()->DocumentStarted();
            if (m_webView->formFillPopup() && m_webView->formFillPopup()->IsVisible()) 
                m_webView->formFillPopup()->handleCommandL(TBrCtlDefs::ECommandCancel);            
            break;
        case TBrCtlDefs::EEventContentFinished:
        case TBrCtlDefs::EEventUploadFinished:
            if (m_suspendTimers) {
                m_suspendTimers = false;
                HandleCommandL(TBrCtlDefs::ECommandAppBackground);
            }
#ifndef BRDO_WML_DISABLED_FF
            if (m_wmlUnloadPending)
                {
                UnloadDllWmlEngine();
            }
#endif
            if (m_webView->formFillPopup() && m_webView->formFillPopup()->IsVisible()) 
                m_webView->formFillPopup()->handleCommandL(TBrCtlDefs::ECommandCancel);    
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// -----------------------------------------------------------------------------
// CBrCtl::HandleCommandL
// From CBrCtlInterface
// Pass a command to the Browser Control
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::HandleCommandL(TInt aCommand)
{
    switch (aCommand - m_commandIdBase) {
        case TBrCtlDefs::ECommandReload:
        case TBrCtlDefs::ECommandBack:
        case TBrCtlDefs::ECommandOneStepBack:
        case TBrCtlDefs::ECommandForward:
        case TBrCtlDefs::ECommandShowHistory:
        case TBrCtlDefs::ECommandClearHistory:
            {
#ifndef BRDO_WML_DISABLED_FF
                if (wmlMode()) {
                    m_wmlEngineInterface->InputElementEditComplete(EFalse);
                }
#endif
                m_historyHandler->historyController()->handleHistoryCommandL(aCommand - m_commandIdBase);
                break;
            }
        case TBrCtlDefs::ECommandCancel:
            {
                if (m_webView->pageView())
                    m_webView->cancelPageView();
                else if(m_webView->pluginPlayer())
                    m_webView->closePluginPlayer();
                else if (m_historyHandler->historyController()->historyView())
                    m_historyHandler->historyController()->handleHistoryCommandL(aCommand - m_commandIdBase);
                else if (m_webView->inFindState())
                    m_webView->exitFindState();
                else if (FocusedElementType() == TBrCtlDefs::EElementActivatedInputBox) {
                    if (m_webView->formFillPopup() && m_webView->formFillPopup()->IsVisible()) {
                        m_webView->formFillPopup()->handleCommandL(TBrCtlDefs::ECommandCancel);
                    }
                    m_webView->setFocusNone();
                    //m_webView->setEditable(EFalse);
                }
                else if (FocusedElementType() == TBrCtlDefs::EElementActivatedObjectBox) {
                    Frame* coreFrame = m_webView->page()->focusController()->focusedOrMainFrame();
                    if (coreFrame)
                        coreFrame->eventHandler()->deactivatedEvent();
                }
                break;
            }
        case TBrCtlDefs::ECommandCancelFetch:
            {
                m_webView->mainFrame()->stopLoading();
                break;
            }

        case TBrCtlDefs::ECommandShowThumbnailView:
            {
                if (m_webView->pageView()) {
                    m_webView->closePageView();
                }
                else if(capabilities() & TBrCtlDefs::ECapabilityGraphicalPage) {
                    m_webView->openPageViewL();
                }
                break;
            }

        case TBrCtlDefs::ECommandDisconnect:
            {
                StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->closeHttpSession();
                break;
            }

        case TBrCtlDefs::ECommandAccept:
            {
                if (m_webView->pageView()) {
                    m_webView->closePageView();
                } else if (FocusedElementType() == TBrCtlDefs::EElementActivatedInputBox) {
                    m_webView->setFocusNone();
                    //m_webView->setEditable();
                }
                break;
            }
        case TBrCtlDefs::ECommandOpen:
            {
                if (m_webView->pageView()) {
                    m_webView->closePageView();
                } else {
                    if (m_historyHandler->historyController()->historyView()) {
                        // this is a weird way of managing history view. needs fixing
                        TKeyEvent keyEvent;
                        keyEvent.iCode = EKeyDevice3;
                        OfferKeyEventL(keyEvent, EEventKey);
                    } else {
                        Frame* coreFrame = core(m_webView->mainFrame());
                        if (coreFrame) {
                            // send an activate event to the focused node
                            TPointerEvent event;
                            event.iPosition = StaticObjectsContainer::instance()->webCursor()->position();
                            event.iType = TPointerEvent::EButton1Down ;
                            coreFrame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(event));
                            event.iType = TPointerEvent::EButton1Up ;
                            coreFrame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(event));
                        }
                    }
                }
                break;
            }
        case TBrCtlDefs::ECommandShowImages:
            {
                CArrayFixFlat<TBrCtlImageCarrier>* imageList = NULL;
                if(wmlMode()){
#ifndef BRDO_WML_DISABLED_FF
                    imageList = m_wmlEngineInterface->GetPageImagesL();
#endif
                }
                else{
                    imageList = m_webView->mainFrame()->imageData(false);
                }
                m_brCtlDialogsProvider->DialogDisplayPageImagesL(*imageList);
                imageList->Reset();
                delete imageList;
                break;
            }
        case TBrCtlDefs::ECommandClearFind:
            {
                m_webView->exitFindState();
                break;
            }
        case TBrCtlDefs::ECommandRemoveFileName:
            {
                IntPoint p;
                WebFrame* wf = frameAndPointUnderCursor(p, *m_webView);
                if (wf) {
                    Frame* coreFrame = core(wf);
                    if (!coreFrame->renderer())
                        break;
                    Element* node = coreFrame->document()->elementFromPoint(p.x(), p.y());
                    for (Node* n = node; n; n = n->parentNode())
                        if (n->isFocusable() && nodeTypeB(n, coreFrame) == TBrCtlDefs::EElementFileSelectionBoxWithContent)
                            ((HTMLInputElement*)n)->setValue(String());
                }
                break;
            }
        case TBrCtlDefs::ECommandLoadImages:
            {
                Frame* mainFrame = core(m_webView->mainFrame());
                for (Frame* frame = mainFrame; frame; frame = frame->tree()->traverseNext(mainFrame))
                    frame->document()->docLoader()->setAutoLoadImages(true);
                break;
            }
        case TBrCtlDefs::ECommandAppForeground:
            {
#ifndef PERF_REGRESSION_LOG
                if (isDeferringTimers())
                    setDeferringTimers(false);
                m_suspendTimers = false;
#endif

                break;
            }
        case TBrCtlDefs::ECommandAppBackground:
            {
#ifndef PERF_REGRESSION_LOG
                if (m_webView->isLoading())
                    m_suspendTimers = true;
                else if (!isDeferringTimers())
                    setDeferringTimers(true);
#endif

                //Disable the zooming bar when it goes to background
                m_webView->hideZoomSliderL();
                break;
            }
        case TBrCtlDefs::ECommandClearAutoFormFillData:
            {
                StaticObjectsContainer::instance()->formFillController()->clearFormData();
                break;
            }
        case TBrCtlDefs::ECommandClearAutoFormFillPasswordData:
            {
                StaticObjectsContainer::instance()->formFillController()->clearLoginData();
                break;
            }
        case TBrCtlDefs::ECommandOpenNewWindow:
            {
                IntPoint p;
                WebFrame* f = frameAndPointUnderCursor(p, *m_webView);
                String url = getNodeUrlAtPointInFrame(*f, p);
                if (url.length() == 0)
                    break;
                DeprecatedString resource = DeprecatedString::fromDes(url.des());
                KURL docUrl = core(m_webView->mainFrame())->loader()->documentLoader()->URL();
                KURL absUrl(docUrl, resource);
                TPtrC _url = absUrl.url().des();
                CBrCtl* brCtl = static_cast<CBrCtl*>(m_brCtlWindowObserver->OpenWindowL(_url, const_cast<TDesC*>(&KNullDesC()), true, NULL));
                if (brCtl)
                    brCtl->LoadUrlL(_url);
                break;
            }
        case TBrCtlDefs::ECommandShowToolBar:
            {
                // deactivate input first
                if (FocusedElementType() == TBrCtlDefs::EElementActivatedInputBox) {
                    m_webView->setFocusNone();
                    //m_webView->setEditable();
                }
                m_webView->launchToolBarL();
                break;
            }
        case TBrCtlDefs::ECommandShowDownloads:
            {
                WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpDownload()->ShowDownloadsL();
                break;
            }
        case TBrCtlDefs::ECommandSaveLaunchParams:
            {
                WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpDownload()->prepareToExit(
                    BrowserSettingL( TBrCtlDefs::ESettingsLaunchAppUid ),
                    BrowserSettingL( TBrCtlDefs::ESettingsLaunchViewId ),
                    BrowserSettingL( TBrCtlDefs::ESettingsLaunchCustomMessageId ) );
                break;
            }
        case TBrCtlDefs::ECommandShowAnchorHref:
            {
                if ( wmlMode() ) {
                    User::Leave(KErrNotSupported);
                }
                m_webView->handleShowAnchorHrefL();
                break;
            }
        case TBrCtlDefs::ECommandLoadFocusedImage:
            {
                if ( wmlMode() ) {
                    User::Leave(KErrNotSupported);
                }
                m_webView->loadFocusedImageL();
                break;
            }
        case TBrCtlDefs::ECommandAddToPhoneBook:
            {
                addFocusedUrlToContacts(m_webView);
                break;
            }
        case TBrCtlDefs::ECommandMakeCall:
            {
                if (!(m_webView->pageView() || m_historyHandler->historyController()->historyView())) {
                    Frame* coreFrame = core(m_webView->mainFrame());
                    if (coreFrame) {
                        // send an activate event to the focused node
                        TKeyEvent keyEvent;
                        keyEvent.iCode = EKeyDevice3; //0x0000f842;
                        keyEvent.iScanCode = EStdKeyDevice3; //0x000000a7;
                        keyEvent.iModifiers = 0;
                        keyEvent.iRepeats = 0;
                        OfferKeyEventL( keyEvent, EEventKeyDown );
                        OfferKeyEventL( keyEvent, EEventKey );
                        OfferKeyEventL( keyEvent, EEventKeyUp );
                    }
                }
                break;
            }
        case TBrCtlDefs::ECommandUnloadWMLEngine:
            {
            UnloadDllWmlEngine();
            break;
            }

        case TBrCtlDefs::ECommandOpenToViewer:
            {
            PluginSkin* plugin = m_webView->mainFrame()->focusedPlugin();
            if(plugin)
                plugin->openInViewerL();
            break;
            }

       case TBrCtlDefs::ECommandZoomSliderShow:
            {
            if ( WebCore::StaticObjectsContainer::instance()->formFillController())
                {
                // Close the form fill control when zoom slider being displayed
                WebCore::StaticObjectsContainer::instance()->formFillController()->destroyPopup();
                }

            m_webView->showZoomSliderL();
            break;
            }

       case TBrCtlDefs::ECommandZoomSliderHide:
            {
            m_webView->hideZoomSliderL();
            break;
            }

       case TBrCtlDefs::ECommandZoomIn:
            {
            // Only for non-touch, use cursor position to zoom
            if ( !AknLayoutUtils::PenEnabled() )
                {
                m_webView->setZoomCursorPosition( ETrue );
                }
             break;
            }

       case TBrCtlDefs::ECommandZoomOut:
            {
            // Only for non-touch, use cursor position to zoom
            if ( !AknLayoutUtils::PenEnabled() )
              {
              m_webView->setZoomCursorPosition( EFalse );
              }
             break;
            }

         case TBrCtlDefs::ECommandHistoryNavigateForward:
            {
            m_historyHandler->historyController()->performTransition(KForward);
            break;
            }
        case TBrCtlDefs::ECommandHistoryNavigateBack:
            {
            m_historyHandler->historyController()->performTransition(KBack);
            break;
            }

       case TBrCtlDefs::ECommandEnterFullscreenBrowsing:
            {
             m_webView->EnterFullscreenBrowsingL();
             break;
            }

       case TBrCtlDefs::ECommandLeaveFullscreenBrowsing:
            {
             m_webView->LeaveFullscreenBrowsingL();
             break;
            }

      default:
            {
            if ( m_wmlEngineInterface &&
                 (TUint)aCommand >= m_commandIdBase + TBrCtlDefs::ECommandIdWMLBase &&
                 (TUint)aCommand <= m_commandIdBase + TBrCtlDefs::ECommandIdRange )
                {
#ifndef BRDO_WML_DISABLED_FF
                // WML command IDs are generated from the base of BrowserControl commands (iCommandIdBase) +
                // the base of WML commands (TBrCtlDefs::ECommandIdWMLBase) + the index of the current WML element
                TInt command = (TInt)(aCommand - m_commandIdBase - TBrCtlDefs::ECommandIdWMLBase);
                if ( m_wmlServiceOption.Count() > (command) )
                    {
                    User::LeaveIfError( m_wmlEngineInterface->HandleDo( m_wmlServiceOption[command]->ActualElemID() ) );
                    }
#endif
                }
            else if ((TUint)aCommand >= TBrCtlDefs::ECommandIdPluginBase &&
                (TUint)aCommand < TBrCtlDefs::ECommandIdWMLBase) {
                // Handle Plug-in commands
                // The Plug-in Option Menu Items was setup using the command base of
                // TBrCtlDefs::ECommandIdPluginBase.  See the methods,
                PluginSkin* plugin = m_webView->mainFrame()->focusedPlugin();
                if(plugin && plugin->pluginWin())
                plugin->pluginWin()->handlePluginCommandL(aCommand);
                }
            if (m_webView && m_webView->widgetExtension()) {
                    m_webView->widgetExtension()->MenuItemSelected( aCommand );
                }
            }
        }
}

// -----------------------------------------------------------------------------
// CBrCtl::HandleDownloadCommandL
// This method calls to download manager in order to send commands to the DlMgr
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::HandleDownloadCommandL(
    TUint16 aTransId,
    TBrCtlDefs::TBrCtlDownloadCmd aCommand )
{
    WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpDownload()->HandleDownloadCommandL(aTransId, aCommand);
}

// -----------------------------------------------------------------------------
// CBrCtl::ContentSize
// From CBrCtlInterface
// Return the display size that would be needed to see all the content
// -----------------------------------------------------------------------------
//
EXPORT_C TSize CBrCtl::ContentSize()
{
    return m_webView->mainFrame()->frameView()->contentSize();
}

// -----------------------------------------------------------------------------
// CBrCtl::SetBrowserSettingL
// From CBrCtlInterface
// Update a Browser Control setting
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::SetBrowserSettingL(TUint aSetting, TUint aValue)
{
    m_settingsContainer->setBrctlSetting((TBrCtlDefs::TBrCtlSettings)aSetting, aValue);
}

// -----------------------------------------------------------------------------
// CBrCtl::BrowserSettingL
// From CBrCtlInterface
// Get a setting from the Browser Control
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CBrCtl::BrowserSettingL(TUint aSetting)
{
    return m_settingsContainer->brctlSetting((TBrCtlDefs::TBrCtlSettings)aSetting);
}

// -----------------------------------------------------------------------------
// CBrCtl::LoadDataL
// From CBrCtlInterface
// Request the Browser Control to display the content of a buffer
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::LoadDataL(const TDesC& aUrl, const TDesC8& aData,
                                const TDataType& aDataType,
                                TUid aCharsetUid)
{
    const TText* charset = charsetForUid(aCharsetUid.iUid);

    HBufC8* url = HBufC8::NewLC(aUrl.Length());
    url->Des().Copy(aUrl);
    PassRefPtr<SharedBuffer> data = new SharedBuffer((const char*)aData.Ptr(), aData.Length());
    String mimeTypeStr(aDataType.Des());
    String charsetStr(charset);
    SubstituteData substituteData(data, mimeTypeStr, charsetStr, url->Des(), url->Des());
    ResourceRequest request = (KURL(url->Des()));
    request.setMainLoad(true);
    request.setHTTPMethod("GET");
    m_webView->mainFrame()->loadData(request, substituteData);
    CleanupStack::PopAndDestroy(); // url
}

// -----------------------------------------------------------------------------
// CBrCtl::InitLoadDataL
// From CBrCtlInterface
// Request the Browser Control to display content incrementaly
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::InitLoadDataL(const TDesC& aUrl,
                                    const TDataType& aDataType,
                                    TUid aCharsetUid,
                                    TUint aContentLength,
                                    MBrCtlDataLoadSupplier* aBrCtlDataLoadSupplier,
                                    MBrCtlDataLoadConsumer** aBrCtlDataLoadConsumer)
{
    if (m_dataLoadConsumer) {
        m_dataLoadConsumer->stopDataLoad();
        endLoadData();
    }
    m_dataLoadConsumer = WebDataLoadConsumer::initWithMetaData(this, aUrl, aDataType,
        aCharsetUid, aContentLength, aBrCtlDataLoadSupplier);
    if (aBrCtlDataLoadConsumer == NULL) {
        User::Leave(KErrArgument);
    }
    *aBrCtlDataLoadConsumer = m_dataLoadConsumer;
}


// -----------------------------------------------------------------------------
// CBrCtl::LoadFileL
// From CBrCtlInterface
// Request the Browser Control to load and display a file from the file system
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::LoadFileL(const TDesC& aFileName)
{
    RFs& fs = StaticObjectsContainer::instance()->fsSession();
    RFile file;
    User::LeaveIfError(file.Open(fs, aFileName, EFileShareExclusive));
    CleanupClosePushL(file);
    LoadFileL(file);
    CleanupStack::PopAndDestroy(); // file
}

// -----------------------------------------------------------------------------
// CBrCtl::LoadFileL
// From CBrCtlInterface
// Request the Browser Control to load and display a file from the file system
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::LoadFileL(RFile& aFileHandle)
{
    int size;
    User::LeaveIfError(aFileHandle.Size(size));
    HBufC8* data = NULL;
    data = HBufC8::NewLC(size);
    TPtr8 dataPtr(data->Des());
    User::LeaveIfError(aFileHandle.Read(dataPtr));

    TFileName fileName;
    aFileHandle.FullName(fileName);

    RApaLsSession apaSession;

    TDataRecognitionResult dataType;
    User::LeaveIfError( apaSession.Connect() );
    TInt ret = apaSession.RecognizeData( TPtrC(fileName), data->Des(), dataType );
    apaSession.Close();

    TUid uid;
    uid.iUid = 0;

    HBufC* urlName = fileNameToUrlLC(fileName);
    LoadDataL(*urlName, data->Des(), dataType.iDataType, uid);
    CleanupStack::PopAndDestroy(urlName);

    CleanupStack::PopAndDestroy(data);
}

// -----------------------------------------------------------------------------
// RPointerArray
// From CBrCtlInterface
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<TBrCtlWmlServiceOption>* CBrCtl::WMLOptionMenuItemsL()
{
    if (!m_wmlMode) {
        resetWmlServiceOptions();
    }
    return &m_wmlServiceOption;
}

// -----------------------------------------------------------------------------
// CBrCtl::AddOptionMenuItemsL
// From CBrCtlInterface
// Request the Browser Control to add its own options to the option menu
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::AddOptionMenuItemsL(CEikMenuPane& aMenuPane, TInt aResourceId, TInt aAfter)
{
    //In widget mode, the widget menu items are populated only by Javascript Menu API
    if (m_webView && m_webView->widgetExtension()) {
        m_webView->widgetExtension()->AddOptionMenuItemsL( aMenuPane, aResourceId );
        return;
    }

    int after = aAfter == -1 ? aAfter :0;
    TBrCtlDefs::TBrCtlElementType focusedElementType = FocusedElementType();

    int count = sizeof(commandsArray) / sizeof(TCommandsArray);
    bool found = false;
    int i;
    for (i = count - 1; i >= 0; i--) {
        if (commandsArray[i].iFocusedElementType == focusedElementType) {
            found = true;
            break;
            }
    }
    if (found) {
        for (; i >= 0 ;i--) {
            if (commandsArray[i].iFocusedElementType != focusedElementType)
                break;
            if (after)
                insertOneMenuItem(aMenuPane, commandsArray[i].iCommandId, commandsArray[i].iResourceId, m_commandIdBase);
            else
                addOneMenuItemAfter(aMenuPane, aAfter, commandsArray[i].iCommandId, commandsArray[i].iResourceId, m_commandIdBase);
        }
    }
    // Give the plugin a chance to add its option menu items, send the Command Id
    // Base we're using for the plug-ins
    // tot fixme
    //TPluginControl pluginControl(*iWebKitControl);
    //pluginControl.AddPluginOptionMenuItemsL(aMenuPane, TBrCtlDefs::ECommandIdPluginBase, aAfter);
    PluginSkin* plugin = m_webView->mainFrame()->focusedPlugin();
    if(plugin && plugin->pluginWin())
        plugin->pluginWin()->addPluginOptionsL(aMenuPane, TBrCtlDefs::ECommandIdPluginBase, aAfter );

}

// -----------------------------------------------------------------------------
// CBrCtl::ClearCache
// From CBrCtlInterface
// Request the Browser Control to clear the cache. Returns the number of bytes
// cleared.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CBrCtl::ClearCache()
{
    int numOfBytes = 0;
    TRAP_IGNORE(numOfBytes = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cacheManager()->RemoveAllL());
    return numOfBytes;
}

// -----------------------------------------------------------------------------
// CBrCtl::IsUrlInCache
// From CBrCtlInterface
// Ask the Browser Control if a URL is in cache
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CBrCtl::IsUrlInCache(TDesC& aUrl)
{
    TBool ret = EFalse;
    HBufC8* url = HBufC8::New(aUrl.Length());
    if (url) {
        url->Des().Copy(aUrl);
        ret = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cacheManager()->Find(*url);
        delete url;
    }
    return ret;
}

// -----------------------------------------------------------------------------
// CBrCtl::ClearItemInCache
// From CBrCtlInterface
// Ask the Browser Control to clear a URL from the cache. Returns the number of
// bytes cleared.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CBrCtl::ClearItemInCache(TDesC& aUrl)
{
    TInt status = KErrNone;
    HBufC8* url = HBufC8::New(aUrl.Length());
    if (url) {
        url->Des().Copy(aUrl);
        TRAP_IGNORE(status = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cacheManager()->RemoveL(*url));
        delete url;
    }
    return status;
}

// -----------------------------------------------------------------------------
// CBrCtl::PageInfoLC
// From CBrCtlInterface
// Request the Browser Control to return info about the page.
// The info is returned on the Cleanup Stack, so the calling method has
// ownership of the memory.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CBrCtl::PageInfoLC(TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo)
{
    HBufC* pageInfo = NULL;
    switch(aBrCtlPageInfo)
        {
        case TBrCtlDefs::EPageInfoTitle:
            {
            if( wmlMode() ) {
#ifndef BRDO_WML_DISABLED_FF
                pageInfo = m_wmlEngineInterface->PageInfoLC(aBrCtlPageInfo);
                //pageInfo is already pushed
                CleanupStack::Pop(pageInfo);
#endif
            }
            else if ( m_historyHandler->historyController() && m_historyHandler->historyController()->historyView() ) {
                return m_historyHandler->historyController()->pageInfoLC( aBrCtlPageInfo );
            }

            if (core(m_webView->mainFrame())->loader()->documentLoader()->title().des().Length()) {
                pageInfo = core(m_webView->mainFrame())->loader()->documentLoader()->title().des().AllocL();
            }
            break;
            }
        case TBrCtlDefs::EPageInfoUrl:
            {
            pageInfo = core(m_webView->mainFrame())->loader()->documentLoader()->URL().url().des().AllocL();
            break;
            }
        case TBrCtlDefs::EPageInfoFocusedNodeUrl:
            {
            IntPoint p;
            WebFrame* f = frameAndPointUnderCursor(p, *m_webView);
            String url = getNodeUrlAtPointInFrame(*f, p);
            if (url.length() > 0)
                pageInfo = url.des().AllocL();
            else
                User::Leave(KErrNotSupported);
                break;
            }
        case TBrCtlDefs::EPageInfoContent:
            {
            Frame* mainFrame = core(m_webView->mainFrame());
            for (Frame* frame = mainFrame; frame; frame = frame->tree()->traverseNext(mainFrame)) {
                if (frame->document()) {
                    RefPtr<Range> textRange(rangeOfContents(frame->document()));
                    int ex = 0;
                    if (!textRange->collapsed(ex)) {
                        if (!pageInfo)
                            pageInfo = textRange->text().des().AllocL();
                        else {
                            pageInfo = pageInfo->ReAllocL(pageInfo->Length() + textRange->text().des().Length());
                            pageInfo->Des().Append(textRange->text().des());
                        }
                    }
                }
            }
            break;
            }
        default:
            {
            break;
            }

        }
    CleanupStack::PushL(pageInfo);
    return pageInfo;
}

// -----------------------------------------------------------------------------
// CBrCtl::NavigationAvailable
// From CBrCtlInterface
// Query the Browser Control if there is a previous or next
// (depending on aDirection) element in history stack
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CBrCtl::NavigationAvailable( TBrCtlDefs::TBrCtlNavigationDirection aDirection )
{
    return m_historyHandler->historyController()->canGoBackOrForward(aDirection == TBrCtlDefs::ENavigationForward ? 1 : -1);
}

// -----------------------------------------------------------------------------
// CBrCtl::SaveToFileL
// From CBrCtlInterface
// Request the Browser Control to save the current content to a file
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CBrCtl::SaveToFileL(const TDesC& aFileName)
{
    (void) aFileName;
    return 0;
}

// -----------------------------------------------------------------------------
// CBrCtl::CertInfo
// From CBrCtlInterface
// Request the Browser Control to pass the server's certificate info for the current page
// -----------------------------------------------------------------------------
//
EXPORT_C const TCertInfo* CBrCtl::CertInfo()
{
    const TCertInfo* certInfo = NULL;
    if( wmlMode() )
    {
#ifndef BRDO_WML_DISABLED_FF
    certInfo = m_certInfo;
#endif
    }
    else
    {
        certInfo = m_certInfo;
    }
    return certInfo;
}

// -----------------------------------------------------------------------------
// CBrCtl::AddLoadEventObserverL
// From CBrCtlInterface
// Request the Browser Control to register for load events
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::AddLoadEventObserverL(MBrCtlLoadEventObserver* aBrCtlLoadEventObserver)
{
    User::LeaveIfError(iLoadEventObserversArray.Append(aBrCtlLoadEventObserver));
}

// -----------------------------------------------------------------------------
// CBrCtl::RemoveLoadEventObserver
// From CBrCtlInterface
// Request the Browser Control to unregister for load events
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::RemoveLoadEventObserver(MBrCtlLoadEventObserver* aBrCtlLoadEventObserver)
{
    TInt index = iLoadEventObserversArray.Find(aBrCtlLoadEventObserver);
    if (index != KErrNotFound)
        iLoadEventObserversArray.Remove(index);
}

// -----------------------------------------------------------------------------
// CBrCtl::ImageCountL
// From CBrCtlInterface
// Return the number of images in the current page
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CBrCtl::ImageCountL()
{
    TInt cnt = 0;
    if (capabilities() & TBrCtlDefs::ECapabilitySavedPage){
        if( wmlMode() ){
#ifndef BRDO_WML_DISABLED_FF
            CArrayFixFlat<TBrCtlImageCarrier>* imageList = m_wmlEngineInterface->GetPageImagesL();
            cnt = imageList->Count();
            delete imageList;
#endif
        }
        else{
            cnt = m_webView->mainFrame()->imageCount(false);
        }
    }
    return cnt;
}


// -----------------------------------------------------------------------------
// CBrCtl::FocusedElementType
// From CBrCtlInterface
// Returns the focused element
// -----------------------------------------------------------------------------
//
EXPORT_C TBrCtlDefs::TBrCtlElementType CBrCtl::FocusedElementType()
{
    return m_webView->focusedElementType();
}

// -----------------------------------------------------------------------------
// CBrCtl::TakeSnapshot
// From CBrCtlInterface
// collects offscreen bitmap
// Returns CFbsBitmap*
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::TakeSnapshotL(CFbsBitmap& snapshot)
{
    if(m_webView){
        m_webView->collectOffscreenbitmapL(snapshot);
    }
}

// -----------------------------------------------------------------------------
// CBrCtl::AddStateChangeObserverL
// From CBrCtlInterface
// Register an observer for state changes
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::AddStateChangeObserverL(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver)
{
    User::LeaveIfError(m_stateChangeObserverArray.Append(aBrCtlStateChangeObserver));
}

// -----------------------------------------------------------------------------
// CBrCtl::RemoveStateChangeObserver
// From CBrCtlInterface
// Unregister an observer for state changes
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::RemoveStateChangeObserver(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver)
{
    TInt index = m_stateChangeObserverArray.Find(aBrCtlStateChangeObserver);
    if (index != KErrNotFound) {
        m_stateChangeObserverArray.Remove(index);
    }
}

// -----------------------------------------------------------------------------
// CBrCtl::AddCommandObserverL
// From CBrCtlInterface
// Register an observer for command interface
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::AddCommandObserverL(MBrCtlCommandObserver* aBrCtlCommandObserver)
{
    User::LeaveIfError(m_commandObserverArray.Append(aBrCtlCommandObserver));
}

// -----------------------------------------------------------------------------
// CBrCtl::RemoveCommandInterface
// From CBrCtlInterface
// Unregister an observer for command interface
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::RemoveCommandObserver(MBrCtlCommandObserver* aBrCtlCommandObserver)
{
    TInt index = m_commandObserverArray.Find(aBrCtlCommandObserver);
    if (index != KErrNotFound) {
        m_commandObserverArray.Remove(index);
    }
}

// -----------------------------------------------------------------------------
// CBrCtl::VersionInfoLC
// From CBrCtlInterface
// Request the Browser Control to return info about the Browser Control version.
// The info is returned on the Cleanup Stack.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CBrCtl::VersionInfoLC(TBrCtlDefs::TBrCtlVersionInfo aVersionInfo)
{
    switch (aVersionInfo)
      {
      case TBrCtlDefs::EVersionInfoName:
          {
          return StringLoader::LoadLC(R_QTN_BROWSERS_BROWSER_NAME_OSS);
          }

      case TBrCtlDefs::EVersionInfoVersion:
          {
          TPtr versionPtr = m_usrAgnt->BrowserVersionL()->Des();
          HBufC* versionInfo = HBufC::NewLC( versionPtr.Length() );
          versionInfo->Des().Copy(versionPtr);
          return versionInfo;
          }

      /*case TBrCtlDefs::EVersionInfoBuild:
          {
          return MobileBrowserBuild.AllocLC();
          }*/

        case TBrCtlDefs::EBrowserVersion:
          {
          TPtr nameAndVersionPtr = m_usrAgnt->BrowserNameAndVersionL()->Des();
          HBufC* nameAndVersionInfo = HBufC::NewLC( nameAndVersionPtr.Length() );
          nameAndVersionInfo->Des().Copy(nameAndVersionPtr);
          return nameAndVersionInfo;
          }

      default:
          {
          return NULL;
          }
      }   // end of switch
}

// -----------------------------------------------------------------------------
// CBrCtl::SetSelfDownloadContentTypesL
// Pass a list of content types to the Browser Control.
// These content types will not be using the download manager
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::SetSelfDownloadContentTypesL(
                                                   TDesC& aContentTypes )
{
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->
        httpSessionManager()->setSelfDownloadContentTypes(aContentTypes);
}

// -----------------------------------------------------------------------------
// CBrCtl::SetParamL
// Update a Browser Control parameter.
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::SetParamL(
                                TBrCtlDefs::TBrCtlParams aParam,
                                const TDesC& aValue )
{
    if (aParam == TBrCtlDefs::EParamsRequestHeaders){
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->
      httpSessionManager()->setClientAcceptHeadersL(aValue);
  }else if (aParam == TBrCtlDefs::EParamsSelfDownoadableTypes){
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->
      httpSessionManager()->setSelfDownloadContentTypes(aValue);
  }
  else {
    User::Leave(KErrNotSupported);
  }
}

// -----------------------------------------------------------------------------
// CBrCtl::LoadSavedPageL
// Request the Browser Control to load a saved page.
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::LoadSavedPageL(
                                     const TDesC& aUid,
                                     TDesC& aSavedPageBuffer )
{
}

// -----------------------------------------------------------------------------
// CBrCtl::FindKeyword
// Perform the incremental find operation
// -----------------------------------------------------------------------------
//
EXPORT_C int CBrCtl::FindKeyword(const TDesC& aKeyword )
{
    return m_webView->searchFor(aKeyword);
}

// -----------------------------------------------------------------------------
// CBrCtl::FindKeywordAgain
// Continue the incremental find operation
// -----------------------------------------------------------------------------
//
EXPORT_C TBrCtlDefs::TBrCtlFindResponse CBrCtl::FindKeywordAgain(TBool aFowardDirection )
{
    return (TBrCtlDefs::TBrCtlFindResponse)m_webView->searchAgain(aFowardDirection);
}

// -----------------------------------------------------------------------------
// CBrCtl::ZoomLevels
// Return the zoom levels array
// -----------------------------------------------------------------------------
//
EXPORT_C RArray<TUint>* CBrCtl::ZoomLevels() const
{
    return m_webView->zoomLevels();
}

// -----------------------------------------------------------------------------
// CBrCtl::SubscribeToItemsL
// Return the feeds' info for the current page.
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<TBrCtlSubscribeTo>& CBrCtl::SubscribeToMenuItemsL()
{
    m_subscribeToItems.ResetAndDestroy();
    m_subscribeToUrls.ResetAndDestroy();

    CArrayFixFlat<TBrCtlSubscribeTo>* links = m_webView->mainFrame()->findSubscribeTo();
    if (!links->Count()) {
        delete links;
        return m_subscribeToItems;
    }

    CleanupStack::PushL(links);
    KURL docUrl = core(m_webView->mainFrame())->loader()->documentLoader()->URL();
    int total = 0;
    for (int i = 0; i < links->Count(); i++) {
        TBrCtlSubscribeTo item = links->At(i);
        // replace url and add id
        DeprecatedString resource = DeprecatedString::fromDes(item.Url());
        KURL url(docUrl, resource);
        // TBrCtlSubscribeTo cannot hold heap variable, so we need an array to save it.
        HBufC* rssUrl = url.url().des().AllocL();
        m_subscribeToUrls.Append(rssUrl);
        //
        const TBrCtlSubscribeTo* newItem = new TBrCtlSubscribeTo(item.Title(), *rssUrl,
            TBrCtlDefs::ECommandIdBase + TBrCtlDefs::ECommandIdSubscribeToBase + total);
        if (m_subscribeToItems.Append(newItem) != KErrNone)
            delete newItem;
        else
            total++;
    }
    CleanupStack::PopAndDestroy(links);
    return m_subscribeToItems;
}


// -----------------------------------------------------------------------------
// CBrCtl::LoadUrlL
// From CBrCtlInterface
// Request the Browser Control to load a url
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::LoadUrlL(const TDesC& url, TInt apid,
                               TBrCtlDefs::TBrCtlCacheMode cachemode)
{
    // Check that we got a valid url
    if (url.Ptr() == NULL || url.Length() == 0)
        User::Leave(KErrArgument);
    // convert to 8

    _LIT(KJs, "javascript:");
    if (url.Length() > KJs().Length()) {
        if (url.Left(KJs().Length()).FindF(KJs) == 0) {
            core(m_webView->mainFrame())->loader()->executeScript(String(url.Mid(KJs().Length())), true);
            return;
        }
    }
    HBufC8* url8 = HBufC8::NewLC( url.Length() );
    url8->Des().Copy( url );
    m_webView->mainFrame()->loadURL( *url8, (TInt)cachemode, String() );
    CleanupStack::PopAndDestroy(); //url8
}

// -----------------------------------------------------------------------------
// CBrCtl::PostUrlL
// From CBrCtlInterface
// Requests the Browser Control to POST to a URL
// -----------------------------------------------------------------------------
EXPORT_C void CBrCtl::PostUrlL (const TDesC& aUrl,
                                const TDesC8& aContentType,
                                const TDesC8& aPostData,
                                const TDesC8* aBoundary,
                                TAny* aReserved)
{

    if (aUrl.Ptr() == NULL || aUrl.Length() == 0)
        User::Leave(KErrArgument);

    if (aContentType.Ptr() == NULL || aContentType.Length() == 0)
        User::Leave(KErrArgument);

    HBufC8* contenttype = NULL;
    if (aBoundary != NULL && aBoundary->Length()>0) {

        _LIT8(KPostBoundary, "; boundary=");
        contenttype = HBufC8::NewLC(aContentType.Length()+aBoundary->Length()+KPostBoundary().Length()+1);
        TPtr8 contentptr(contenttype->Des());
        contentptr.Copy(aContentType);
        contentptr.Append(KPostBoundary);
        contentptr.Append(*aBoundary);
        contentptr.ZeroTerminate();
    }
    else {
        contenttype = aContentType.AllocLC();
    }

    WebCore::FormData* formData = new (ELeave) FormData( (const void*) aPostData.Ptr(), aPostData.Length() );
    formData->ref();

    HBufC8* url8 = HBufC8::NewLC(aUrl.Length() + 1);
    TPtr8 url8ReqPtr(url8->Des());
    url8ReqPtr.Copy(aUrl);
    url8ReqPtr.ZeroTerminate();

    ResourceRequest request = (KURL(  url8ReqPtr ));
    request.setMainLoad(true);
    request.setCachePolicy(UseProtocolCachePolicy);
    request.setHTTPMethod("POST");
    request.setHTTPContentType(*contenttype);
    request.setHTTPBody(formData);
    webView()->mainFrame()->loadRequest(request);
    formData->deref();

    CleanupStack::PopAndDestroy(url8);
    CleanupStack::PopAndDestroy(contenttype);

}
// -----------------------------------------------------------------------------
// CBrCtl::MinimumSize
// From CCoeControl
// Return the minimum size the Browser Control can use
// -----------------------------------------------------------------------------
//
EXPORT_C TSize CBrCtl::MinimumSize()
{
    // Minimum size that can be supported.
    return TSize( KMinimumWidth, KMinimumHeight );
}

// -----------------------------------------------------------------------------
// CBrCtl::OfferKeyEventL
// From CCoeControl
// Pass key events into the Browser Control
// -----------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CBrCtl::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                             TEventCode aEventCode)
{
    return ComponentControl(0)->OfferKeyEventL(aKeyEvent, aEventCode);
}

// -----------------------------------------------------------------------------
// CBrCtl::SetFocus
// From CCoeControl
// Sets that the Browser Control has keyboard focus.
// -----------------------------------------------------------------------------
//
EXPORT_C void CBrCtl::SetFocus(TBool aFocus, TDrawNow aDrawNow)
{
    ComponentControl(0)->SetFocus(aFocus, aDrawNow);
}


// -----------------------------------------------------------------------------
// CBrCtl::MakeVisible
// makes the controls visible or invisible
// -----------------------------------------------------------------------------
//
void CBrCtl::MakeVisible(TBool visible)
{
    bool isvisible = IsVisible();
    CCoeControl::MakeVisible(visible);

#ifndef BRDO_WML_DISABLED_FF
    if (m_wmlEngineInterface) {
        m_wmlEngineInterface->MakeVisible(visible && wmlMode());
        }
#endif

    if (!wmlMode()) {
        if(!visible){
        m_webView->mainFrame()->frameLoader()->stopRedirectionTimerNow();
        }
        else if(m_webView->mainFrame()->frameLoader()->checkScheduledRedirection())
          m_webView->mainFrame()->frameLoader()->startRedirectionTimerNow();
        m_webView->MakeVisible(visible);
    }

    if( visible && !isvisible && m_webView ) {
        m_webView->checkForZoomChange();
    }

    if( m_webView->pageFullScreenHandler() && m_webView->pageFullScreenHandler()->isFullScreenMode() ) {
        if (visible)
            m_webView->pageFullScreenHandler()->showEscBtnL();
        else
            m_webView->pageFullScreenHandler()->hideEscBtnL();
    }
}

// -----------------------------------------------------------------------------
// CBrCtl::SizeChanged
// From CCoeControl
// Handle changes in size of the displayable area.  This is called by the
// CCoeControl framework, after SetRect(), SetSize(), SetExtent() is called on
// BrowserControl.
// -----------------------------------------------------------------------------
//
void CBrCtl::SizeChanged()
{
    if (StaticObjectsContainer::instance()->isPluginFullscreen()) return;
    TRect adjustedRect( Rect() );
    TBool callDoLayout = EFalse;
    TInt  scrollBarWidth = getMainScrollbarWidth();

    if ( (m_capabilities & TBrCtlDefs::ECapabilityDisplayScrollBar)) {
      if (scrollBarWidth > 0) {
        adjustedRect.iBr.iX -= scrollBarWidth;
        if (hasHorizontalScrollbar()) {
            adjustedRect.iBr.iY -= scrollBarWidth;
        }
        if (AknLayoutUtils::LayoutMirrored()) {
            adjustedRect.Move(scrollBarWidth, 0);
        }
      }
    }

    if( m_webView )
       callDoLayout = (adjustedRect.Width()!= m_webView->Rect().Width() ||
                       adjustedRect.Height()!= m_webView->Rect().Height())?ETrue:EFalse;

    if (wmlMode()) {
#ifndef BRDO_WML_DISABLED_FF
        if (m_wmlEngineInterface) {
            m_wmlEngineInterface->SetRect( adjustedRect );
            updateScrollbars(m_wmlEngineInterface->WMLDocumentSize().iHeight, m_wmlEngineInterface->WmlViewRectangle().Height(),
                m_wmlEngineInterface->WMLDocumentPosition().iY, m_wmlEngineInterface->WMLDocumentSize().iWidth, m_wmlEngineInterface->WmlViewRectangle().Width(),
                m_wmlEngineInterface->WMLDocumentPosition().iX);
        }
#endif
    }
    else {
        if (m_webView) {
            m_webView->SetRect(adjustedRect);
            updateScrollbars(m_webView->mainFrame()->frameView()->contentSize().iHeight, m_webView->Rect().Height(),
                m_webView->mainFrame()->frameView()->contentPos().iY, m_webView->mainFrame()->frameView()->contentSize().iWidth,
                m_webView->Rect().Width(), m_webView->mainFrame()->frameView()->contentPos().iX);
        }
    }
    if( m_historyHandler->historyController()->historyView() ) {
        m_historyHandler->historyController()->historyView()->SetRect( Rect() );
        m_historyHandler->historyController()->historyView()->DrawNow();
    }
    else {
        if(callDoLayout)
            m_webView->doLayout();
        if(m_webView)
            m_webView->syncRepaint(m_webView->Rect());
    }
}

// -----------------------------------------------------------------------------
// CBrCtl::CountComponentControls
// From CCoeControl
// Get the number of controls in the Browser Control
// -----------------------------------------------------------------------------
//
TInt CBrCtl::CountComponentControls() const
{
    if (m_historyHandler->historyController()->historyView()) {
        return 1;
    }
    TInt count(1); // iWebKitView + iWmlView
    if (m_webView->pageView()) {
        count++;
    }

    if ( m_webView->pluginPlayer() )
    {
        count++;
    }

    return count;
}

// -----------------------------------------------------------------------------
// CBrCtl::ComponentControl
// From CCoeControl
// Get a specified control with a given index
// -----------------------------------------------------------------------------
//
CCoeControl* CBrCtl::ComponentControl(TInt aIndex) const
{
    if (m_historyHandler->historyController()->historyView()) {
        return m_historyHandler->historyController()->historyView();
    }

    if ( m_webView->pluginPlayer() )
    {
        return (CCoeControl*)m_webView->pluginPlayer();
    }


    TInt count(0);
    if (count++==aIndex) {
        // tot:fixme
         if ( wmlMode() ) {
#ifndef BRDO_WML_DISABLED_FF
             return m_wmlEngineInterface->coeControl();
#endif
            }
        else {
            return m_webView;
        }
    }
    if (m_webView->pageView()) {
        if (count++==aIndex) {
            return (CCoeControl*)m_webView->pageView();
        }
    }

    return NULL;
}

void CBrCtl::reportStateChanged(TBrCtlDefs::TBrCtlState state, bool enter)
{
    for (TInt i=0; i < m_stateChangeObserverArray.Count(); i++) {
        m_stateChangeObserverArray[i]->StateChanged(state, enter);
    }
}

void CBrCtl::setWmlMode (bool mode)
{
#ifndef BRDO_WML_DISABLED_FF
    m_wmlMode = mode;
    reportStateChanged(TBrCtlDefs::EStateWmlView, mode);
    resetWmlServiceOptions();
#endif
}
//-------------------------------------------------------------------------------
// CBrCtl::LoadResourceFileL
// add webkit resource file to the coe environment
//
//-------------------------------------------------------------------------------
void CBrCtl::LoadResourceFileL()
    {
    // add webkit resource file to the coe environment
    // load resource file
    TFileName drive;
    TFileName resourceFileName;
    TParse parse;
    Dll::FileName( drive );
    parse.Set( drive, NULL, NULL );

    resourceFileName += parse.Drive();
    resourceFileName += KDC_RESOURCE_FILES_DIR;
    resourceFileName += _L("WebKit.RSC");

    CCoeEnv* coeEnv = CCoeEnv::Static();
    if( !BaflUtils::FileExists( coeEnv->FsSession(), resourceFileName ) )
        {
        TDriveName driveZ( KDriveZ );
        resourceFileName.Replace( 0, 2, driveZ);
        }

    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFileName  );
    m_resourceFile = coeEnv->AddResourceFileL(resourceFileName);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// CreateBrowserControlL
// From CBrCtlInterface
// Create CBrCtl. Entry point for Host Application
// -----------------------------------------------------------------------------
//
EXPORT_C CBrCtlInterface* CreateBrowserControlL(
  CCoeControl* aParent,
  TRect aRect,
  TUint aBrCtlCapabilities,
  TUint aCommandIdBase,
  MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver,
  MBrCtlLinkResolver* aBrCtlLinkResolver,
  MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver,
  MBrCtlLayoutObserver* aBrCtlLayoutObserver,
  MBrCtlDialogsProvider* aBrCtlDialogsProvider,
  MBrCtlWindowObserver* aBrCtlWindowObserver,
  MBrCtlDownloadObserver* aBrCtlDownloadObserver )
    {
    return CBrCtl::NewL(aParent, aRect, aBrCtlCapabilities,
                        aCommandIdBase, aBrCtlSoftkeysObserver,
                        aBrCtlLinkResolver, aBrCtlSpecialLoadObserver,
                        aBrCtlLayoutObserver, aBrCtlDialogsProvider,
                        aBrCtlWindowObserver, aBrCtlDownloadObserver );
    }

EXPORT_C MWidgetExtension* CBrCtl::CreateWidgetExtensionL(MWidgetCallback &aWidgetCallback)
{

    if (m_webView) {
        return m_webView->createWidgetExtension(aWidgetCallback);
    }

    return NULL;
}

EXPORT_C TBrCtlImageCarrier* CBrCtl::FocusedImageLC()
{
    if ( wmlMode() ) {
        User::Leave(KErrNotSupported);
    }
    return m_webView->focusedImageLC();
}

EXPORT_C TBool CBrCtl::OkToExit()
{
    return WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpDownload()->okToExit();
}

EXPORT_C CGulIcon* CBrCtl::GetBitmapData(const TDesC& aUrl, TBrCtlDefs::TBrCtlBitmapInfo aBitmapInfo)
{
    CGulIcon* icon = NULL;
    switch (aBitmapInfo)
        {
        case TBrCtlDefs::EBitmapThumbnail:
            {
                HistoryEntry* entry = m_historyHandler->historyController()->entry(EHistoryStackDirectionCurrent);
                if ( entry ) {
                    CFbsBitmap* bitmap = entry->thumbnail();
                    if(!bitmap) {
                        // get scaled page from PageScaler;
                        bitmap = m_webView->pageScaler()->ScaledPage();
                    }
                    if(bitmap) {
                        // update the history with new bitmap
                        TRAP_IGNORE(m_historyHandler->historyController()->updateHistoryEntryThumbnailL(bitmap));
                        TRAP_IGNORE(icon = CGulIcon::NewL(bitmap, NULL));
                        if(icon) {
                            icon->SetBitmapsOwnedExternally(ETrue);
                        }
                    }
                }
            break;
            }
        case TBrCtlDefs::EBitmapFavicon:
            {
            HBufC8* url = HBufC8::NewLC(aUrl.Length());
            url->Des().Copy(aUrl);
            icon = StaticObjectsContainer::instance()->sharedIconDatabase()->iconForPageURL(*url);
            CleanupStack::PopAndDestroy(); // url
            break;
            }
        }
    return icon;
}
void CBrCtl::showHorizontalScrollbar(bool show)
{
    TRect adjustedRect(Rect());
    TInt scrollBarWidth = getMainScrollbarWidth();

    if (scrollBarWidth > 0) {
      adjustedRect.iBr.iX -= scrollBarWidth;
      if (show) {
          adjustedRect.iBr.iY -= scrollBarWidth;
      }
      else {
          adjustedRect.iBr.iY += scrollBarWidth;
      }
    }
    m_webView->SetRect(adjustedRect);
}


CBrCtl* CBrCtl::getWindowL(TDesC& windowName, bool userGesture)
{
    CBrCtlInterface* newBrctl = NULL;
    TPtrC emptyUrl = KNullDesC();
    if (m_brCtlWindowObserver) {
        newBrctl = m_brCtlWindowObserver->FindWindowL(windowName);
        if (!newBrctl)
        {
         if(StaticObjectsContainer::instance()->isPluginFullscreen())
         {
            PluginSkin* plugin=m_webView->mainFrame()->focusedPlugin();
            plugin->deActivate();
         }
         newBrctl = m_brCtlWindowObserver->OpenWindowL(emptyUrl, &windowName, userGesture, 0);            
        }
    }
    return static_cast<CBrCtl*>(newBrctl);
}

void CBrCtl::showWindow()
{
    if (m_brCtlWindowObserver) {
        TRAP_IGNORE(m_brCtlWindowObserver->HandleWindowCommandL(KNullDesC(), EShowWindow));
    }
}

void CBrCtl::closeWindowSoon()
{
    if( m_windoCloseTimer ) {
        m_windoCloseTimer->Cancel();
    }
    else {
        m_windoCloseTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    }
    
    m_windoCloseTimer->Start( 20, 0, TCallBack( &doCloseCb, this ) );
}

void CBrCtl::doCloseWindowSoon()
{
    m_windoCloseTimer->Cancel();
    if (brCtlWindowObserver())
        TRAP_IGNORE(brCtlWindowObserver()->HandleWindowCommandL(KNullDesC(), ECloseWindow));
}

TBool CBrCtl::sendCommandsToClient(
    TBrCtlDefs::TBrCtlClientCommands aCommand,
    const CArrayFix<TPtrC>& aAttributesNames,
    const CArrayFix<TPtrC>& aAttributeValues )
{
    if (m_commandObserverArray.Count() == 0)
        return EFalse;
    for (TInt i = 0;i < m_commandObserverArray.Count(); i++)
        TRAP_IGNORE(m_commandObserverArray[i]->HandleCommandL(aCommand,aAttributesNames,aAttributeValues));
    return ETrue;
}

void CBrCtl::endLoadData()
{
    delete m_dataLoadConsumer;
    m_dataLoadConsumer = NULL;
}

void CBrCtl::setCertInfo(TCertInfo* aCertInfo)
{
	if (m_certInfo)
	{
		delete m_certInfo;
	}
	m_certInfo = aCertInfo;
}

void CBrCtl::updateDefaultSoftkeys()
{
    if (m_brCtlSoftkeysObserver) {
        TRAP_IGNORE(
        m_brCtlSoftkeysObserver->UpdateSoftkeyL(EKeyLeft, KNullDesC, 0, EChangeReasonIdle);
        m_brCtlSoftkeysObserver->UpdateSoftkeyL(EKeyRight, KNullDesC, 0, EChangeReasonIdle); );
    }
}

void CBrCtl::doEnableFavicon()
{
    m_timer->Cancel();
    IconDatabase::allowDatabaseCleanup();
}

TInt CBrCtl::doCloseCb(TAny* aPtr)
{
    static_cast<CBrCtl*>(aPtr)->doCloseWindowSoon();
    return KErrNone;
}

TInt CBrCtl::doEnableFaviconCb(TAny* aPtr)
{
    static_cast<CBrCtl*>(aPtr)->doEnableFavicon();
    return KErrNone;
}

//-------------------------------------------------
//
//  WML DISABLED
//
//-------------------------------------------------
#ifdef BRDO_WML_DISABLED_FF
void CBrCtl::resetWmlServiceOptions()
{ }
void CBrCtl::wmlServiceOptionsAddItemL( const TText* /*aText*/, TUint32 /*aActualElemID*/, TInt /*aIsPrev*/ )
{ }
void CBrCtl::postUrlL(TDesC& /*aUrl16*/, TDesC8& /*aPostData*/, TDesC& /*aContentType*/)
{ }
void CBrCtl::UnloadDllWmlEngine()
{ }
void CBrCtl::LoadDllWmlEngineL()
{ }
void CBrCtl::loadResource(TDesC8& /*url*/,  CWmlContentInterface* /*content*/ )
{ }
//-------------------------------------------------
//
//  WML ENABLED
//
//-------------------------------------------------
#else
void CBrCtl::resetWmlServiceOptions()
{
    m_wmlServiceOption.ResetAndDestroy();

    delete m_firstPrevDoElement;
    m_firstPrevDoElement = NULL;
}

void CBrCtl::wmlServiceOptionsAddItemL( const TText* aText, TUint32 aActualElemID, TInt aIsPrev )
{
    // does it fit in the range ?
    if ( m_wmlServiceOption.Count() > TBrCtlDefs::ECommandIdRange - TBrCtlDefs::ECommandIdWMLBase )
        {
        User::Leave( KErrGeneral );
        }

    // Add WML Elements to the array, only if the number of elements did not exceed the max.
    // WML command IDs are generated from the base of BrowserControl commands (iCommandIdBase) +
    // the base of WML commands (TBrCtlDefs::ECommandIdWMLBase) + the index of the current WML element
    TInt count = m_wmlServiceOption.Count();
    __ASSERT_DEBUG((aActualElemID >=  0 && count < TBrCtlDefs::ECommandIdRange), BrCtlPanic());
    TBrCtlWmlServiceOption *item = NULL;
    TUint elemID = m_commandIdBase + TBrCtlDefs::ECommandIdWMLBase + count;

    if (aText)
        {
        TPtrC ptr(aText);
        HBufC* text = ptr.AllocLC();
        TUint actualElem(aActualElemID);
        item = new TBrCtlWmlServiceOption( text, elemID, actualElem );
        }
    else
        {
        HBufC* doText = NULL;
        switch( aIsPrev )
            {
            case TWmlEngineDefs::WML_OPTION_UNKNOWN:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_UNKNOWN );
                break;
                }
            case TWmlEngineDefs::WML_OPTION_ACCEPT:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_ACCEPT );
                break;
                }
            case TWmlEngineDefs::WML_OPTION_PREV:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_PREV );
                break;
                }
            case TWmlEngineDefs::WML_OPTION_HELP:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_HELP );
                break;
                }
            case TWmlEngineDefs::WML_OPTION_RESET:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_RESET );
                break;
                }
            case TWmlEngineDefs::WML_OPTION_OPTIONS:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_OPTIONS );
                break;
                }
            case TWmlEngineDefs::WML_OPTION_DELETE:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_DELETE );
                break;
                }
            default:
                {
                doText = StringLoader::LoadLC( R_DO_ELEMENT_UNKNOWN );
                }
            }
        item = new TBrCtlWmlServiceOption( doText, elemID, aActualElemID );
        }
    User::LeaveIfNull(item);
    CleanupStack::Pop(); //text or dotext
    CleanupStack::PushL(item);
    if ( m_wmlEngineInterface && ( aIsPrev == TWmlEngineDefs::WML_OPTION_PREV)
     && ( m_firstPrevDoElement == NULL )  &&
        (m_wmlEngineInterface->HistoryStackPosition() != TWmlEngineDefs::EHistoryAtTheBeginning) )
        {
        // first DO element with PREV task is associated with the right softkey
        // and history is not empty
        m_firstPrevDoElement = item;
        }
    else
        {
        User::LeaveIfError(m_wmlServiceOption.Append( item ));
        }
    CleanupStack::Pop(); // item
}

void CBrCtl::postUrlL(TDesC& aUrl16, TDesC8& aPostData, TDesC& aContentType)
{
    //tot: fixme - WML use for sending post data
  //_LIT(KPostContentType, "multipart/form-data");

    WebCore::FormData* formData = new FormData( (const void*) aPostData.Ptr(), aPostData.Length() );
    formData->ref();
//
    TBrCtlDefs::TBrCtlCacheMode cacheMode = TBrCtlDefs::ECacheModeNormal;
//

    ResourceRequestCachePolicy cachePolicy = ReturnCacheDataElseLoad;

    HBufC8* url8 = HBufC8::NewL(aUrl16.Length() + 1);
    TPtr8 url8ReqPtr(url8->Des());
    url8ReqPtr.Copy(aUrl16);
    url8ReqPtr.ZeroTerminate();

    ResourceRequest request = (KURL(  url8ReqPtr ));
    request.setMainLoad(true);
    request.setCachePolicy(cachePolicy);
    request.setHTTPMethod("POST");
    request.setHTTPContentType(aContentType);
    request.setHTTPBody(formData);
    webView()->mainFrame()->loadRequest(request);
    formData->deref();
    delete url8;

}


// -----------------------------------------------------------------------------
// CBrCtl::UnloadDllWmlEngine
// Unload WML dll
// -----------------------------------------------------------------------------
//
void CBrCtl::UnloadDllWmlEngine()
    {

    if ( m_wmlEngineInterface )
        {
        delete m_wmlEngineInterface;
        m_wmlEngineInterface = NULL;
        m_Library.Close();
        }
    delete m_WmlInterface;
    m_WmlInterface = NULL;
    setWmlMode( EFalse );
    }

// -----------------------------------------------------------------------------
// CBrCtl::LoadDllWmlEngine
//
// -----------------------------------------------------------------------------
//
void CBrCtl::LoadDllWmlEngineL()
{
    if(!wmlEngineInterface()) {

#ifdef __WINSCW__
        _LIT( KBrowserWmlengineWINSCWName, "\\epoc32\\release\\winscw\\udeb\\wmlengine.dll" );
        User::LeaveIfError( m_Library.Load( KBrowserWmlengineWINSCWName ) );
#else
        _LIT( KBrowserWmlengineARMV5Name,  "\\system\\libs\\wmlengine.dll" );
        User::LeaveIfError( m_Library.Load( KBrowserWmlengineARMV5Name ) );
#endif //__WINSCW__

        TLibraryFunction entry = m_Library.Lookup(1);
        //Call the function to create a instance of class
        m_wmlEngineInterface = (MWmlEngineInterface*) entry();

        if( wmlEngineInterface() ) {

            m_WmlInterface = new(ELeave) CWmlInterfaceImpl(*this);
            TRect adjustedRect( Rect() );
            TInt scrollBarWidth = getMainScrollbarWidth();

            if ( (m_capabilities & TBrCtlDefs::ECapabilityDisplayScrollBar)) {
              if (scrollBarWidth > 0) {
                adjustedRect.iBr.iX -= scrollBarWidth;
                if (hasHorizontalScrollbar()) {
                    adjustedRect.iBr.iY -= scrollBarWidth;
                }

                if (AknLayoutUtils::LayoutMirrored()) {
                    adjustedRect.Move(scrollBarWidth, 0);
                }
              }
            }


           m_wmlEngineInterface->ConstructL( this, adjustedRect, m_WmlInterface );
           m_wmlEngineInterface->SetHistoryController(m_historyHandler->historyController() );
           m_wmlEngineInterface->SetRect( adjustedRect );

           const TUint settingSize = (sizeof(wmlSettingTable) / sizeof(TUint) );

           for(TUint i = 0, settingVal=0; i < settingSize; i++) {
              settingVal = BrowserSettingL(wmlSettingTable[i]);
              m_wmlEngineInterface->SetBrowserSettingL(wmlSettingTable[i], settingVal);
           }

           m_wmlEngineInterface->SetAppId(-1 );
        }
    }
}

void CBrCtl::loadResource(TDesC8& url,  CWmlContentInterface* content )
{
    m_wmlDispatcher->loadResource( url, content );
}
#endif  // BRDO_WML_DISABLED_FF

HBufC* CBrCtl::fileNameToUrlLC(const TDesC& aFileName)
{
    if (aFileName.Length() == 0 || aFileName.Ptr() == NULL) {
        User::Leave(KErrArgument);
    }

    // Size of "file://" and NULL terminator is 8
    _LIT(KFileScheme, "file:///");
    HBufC* urlName = HBufC::NewL(aFileName.Length() + KFileScheme().Length() );
    urlName->Des().Copy(KFileScheme); // Copy the file scheme
    urlName->Des().Append(aFileName); // Append the file name
    TInt index = 0;
    TPtrC ptr(*urlName);
    TUint16* str = (TUint16*)ptr.Ptr();

    // Replace all '\\' with '/'
    do {
        ptr.Set((ptr.Mid(index)));
        str = (TUint16*)ptr.Ptr();
        index = ptr.Locate('\\');
        if (index != KErrNotFound) {
            *(str + index) = '/';
        }

    } while (index != KErrNotFound);

    CleanupStack::PushL(urlName);
    return urlName;
}


void CBrCtl::SetScriptLogMode(TInt aMode)
{
    for (TInt i=0; i<m_stateChangeObserverArray.Count(); i++) {
        m_stateChangeObserverArray[i]->StateChanged(TBrCtlDefs::EStateScriptLog, aMode);
    }
}


int CBrCtl::getMainScrollbarWidth() const
{
  return 0;
}

MBrCtlDownloadObserver* CBrCtl::brCtlDownloadObserver()
{
    if (!m_brCtlDownloadObserver) {
        return WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->uiCallback();
    }
    return m_brCtlDownloadObserver;
}



void CBrCtl::HandlePointerBufferReadyL()
{
    m_webView->HandlePointerBufferReadyL();
}


