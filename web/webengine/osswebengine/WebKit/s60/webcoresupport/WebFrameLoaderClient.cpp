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
* Description:   Implementation of WebFrameLoaderClient
*
*/

#include <Browser_platform_variant.hrh>
#include "config.h"
#include <../bidi.h>
#include "WebFrameLoaderClient.h"
#include "ResourceError.h"
#include "DebugStream.h"
#include "DocumentLoader.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "HTMLFrameOwnerElement.h"
#include "HTMLNames.h"
#include "HTMLFrameElement.h"
#include "WebCoreFrameBridge.h"
#include "HTMLFormElement.h"
#include "HTMLObjectElement.h"
#include "FrameView.h"
#include "Document.h"
#include "WebCoreWidget.h"
#include "WebFrame.h"
#include "WebView.h"
#include "WebFrameView.h"
#include "WebFrameBridge.h"
#include "WebDocumentLoader.h"
#include "BrCtl.h"
#include "Page.h"
#include "ProgressTracker.h"
#include "WebPolicyManager.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "BrCtlWindowObserver.h"
#include "WmlDispatcher.h"
#include "WebUtil.h"
#include "ErrorDefs.h"
#include "WebFepTextEditor.h"
#include "SettingsContainer.h"
#include "WebTabbedNavigation.h"
#include "StaticObjectsContainer.h"
#include "WebCursor.h"
#include "WebCharsetData.h"
#include "WebKitLogger.h"
#include "PluginHandler.h"
#include "MimeTypeRegistry.h"
#include "WidgetExtension.h"

using namespace WebCore;
using namespace HTMLNames;

// this should be changed if we wanna have a counter per window.
static int total_bytes = 0;


static CBrCtl* brctl(WebFrame* webFrame)
{
    ASSERT(hasWebView());
    ASSERT(hasFrameView());
    return webFrame->frameView()->topView()->brCtl();
}

WebFrameLoaderClient::WebFrameLoaderClient(WebFrame* webFrame)
    : m_webFrame(webFrame)
{
    m_webPolicyManager = new WebPolicyManager(this);
    m_WmlContentListener = 0;
}

WebFrameLoaderClient::~WebFrameLoaderClient()
{
    delete m_webPolicyManager;
    if (m_WmlContentListener) delete m_WmlContentListener;
}

void WebFrameLoaderClient::frameLoaderDestroyed() 
{ 
    delete this;
}

bool WebFrameLoaderClient::hasWebView() const 
{
    return m_webFrame != NULL;; 
}

bool WebFrameLoaderClient::hasFrameView() const 
{
    return m_webFrame->frameView() != NULL; 
}

bool WebFrameLoaderClient::privateBrowsingEnabled() const
{
//    return [[getWebView(m_webFrame.get()) preferences] privateBrowsingEnabled];
    return false; 
}

void WebFrameLoaderClient::makeDocumentView()
{
    WebFrameView *v = m_webFrame->frameView();
    //WebDataSource *ds = [m_webFrame.get() _dataSource];

    //NSView <WebDocumentView> *documentView = [v _makeDocumentViewForDataSource:ds];
    //if (!documentView)
    //    return;

    if (m_webFrame && 
        m_webFrame->frameView() && 
        m_webFrame->frameView()->topView()&&
        !m_webFrame->parentFrame()) {

        m_webFrame->frameView()->topView()->setEditable(false);
    }
    WebFrameBridge *bridge = m_webFrame->bridge();

    // FIXME: We could save work and not do this for a top-level view that is not a WebHTMLView.
    //(MWebCoreWidget* view, int marginWidth, int marginHeight)    
    bridge->createFrameViewWithScrollView(v, v->marginWidth(), v->marginHeight());
    //m_webFrame.get() _updateBackground];
    bridge->installInFrame(v);

    // Call setDataSource on the document view after it has been placed in the view hierarchy.
    // This what we for the top-level view, so should do this for views in subframes as well.
    //[documentView setDataSource:ds];
}

void WebFrameLoaderClient::makeRepresentation(DocumentLoader* docLoader)
{
    if (isWMLContent(m_response.mimeType())) {
        // call the WmlDispatcher Headers routine to set the WML flag
        if (! m_WmlContentListener) {
            TRAPD(err, m_WmlContentListener = CWmlDispatcher::NewL(brctl(m_webFrame), core(m_webFrame) ));
            if((err == KErrNone) && (m_WmlContentListener != NULL))
                {
                brctl(m_webFrame)->setWmlDispatcher(m_WmlContentListener);
                }
        }
        TRAPD(err,m_WmlContentListener->HeadersL( 0, m_response ));
    }
    else{
        if (m_WmlContentListener) {
            m_WmlContentListener->MakeWmlVisible(EFalse);
        brctl(m_webFrame)->setWmlMode(EFalse);
            brctl(m_webFrame)->MakeVisible(ETrue);
        }
        if (FrameLoadTypeRedirectWithLockedHistory == core(m_webFrame)->loader()->loadType())
            m_webFrame->frameView()->topView()->setRedirectWithLockedHistory(true);
        if (docLoader->isLoadingMainResource()&&
            core(m_webFrame)->loader()->isLoadingMainFrame() &&
            FrameLoadTypeStandard == core(m_webFrame)->loader()->loadType()){
            m_webFrame->frameView()->topView()->resetZoomLevel();
        }

        if (!core(m_webFrame)->ownerElement()) {
            brctl(m_webFrame)->settings()->setNavigationType((brctl(m_webFrame)->capabilities()&TBrCtlDefs::ECapabilityCursorNavigation) ? SettingsContainer::NavigationTypeCursor : SettingsContainer::NavigationTypeTabbed);
            StaticObjectsContainer::instance()->webCursor()->cursorUpdate(true);
            m_webFrame->frameView()->topView()->tabbedNavigation()->clear();
		}        
    }
}

void WebFrameLoaderClient::setDocumentViewFromCachedPage(CachedPage*)
{
    /*
    DocumentLoader* cachedDocumentLoader = cachedPage->documentLoader();
    ASSERT(cachedDocumentLoader);
    cachedDocumentLoader->setFrame(core(m_webFrame.get()));
    NSView <WebDocumentView> *cachedView = cachedPage->documentView();
    ASSERT(cachedView != nil);
    [cachedView setDataSource:dataSource(cachedDocumentLoader)];
    [m_webFrame->_private->webFrameView _setDocumentView:cachedView];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::forceLayout()
{
    /*
    NSView <WebDocumentView> *view = [m_webFrame->_private->webFrameView documentView];
    if ([view isKindOfClass:[WebHTMLView class]])
        [(WebHTMLView *)view setNeedsToApplyStyles:YES];
    [view setNeedsLayout:YES];
    [view layout];
    */
    if (core(m_webFrame)->document()->isHTMLDocument())
    {
        core(m_webFrame)->view()->layout();
    }
}

void WebFrameLoaderClient::forceLayoutForNonHTML() 
{
    /*
    WebFrameView *thisView = m_webFrame->_private->webFrameView;
    NSView <WebDocumentView> *thisDocumentView = [thisView documentView];
    ASSERT(thisDocumentView != nil);
    
    // Tell the just loaded document to layout.  This may be necessary
    // for non-html content that needs a layout message.
    if (!([[m_webFrame.get() _dataSource] _isDocumentHTML])) {
        [thisDocumentView setNeedsLayout:YES];
        [thisDocumentView layout];
        [thisDocumentView setNeedsDisplay:YES];
    }
    */
    if (core(m_webFrame)->document()->isImageDocument())
    {
        core(m_webFrame)->view()->layout();
    }
}

void WebFrameLoaderClient::setCopiesOnScroll()
{
    m_webFrame->frameView()->setMayUseCopyScroll(true);
}

void WebFrameLoaderClient::detachedFromParent2()
{
    // m_webFrame->_private->inspectors makeObjectsPerformSelector:@selector(_webFrameDetached:) withObject:m_webFrame.get()];
    if (m_webFrame->frameView())
        m_webFrame->frameView()->setWebFrame(NULL);
}

void WebFrameLoaderClient::detachedFromParent3()
{
    m_webFrame->setFrameView(NULL);
}

void WebFrameLoaderClient::detachedFromParent4()
{
    m_webFrame->setBridge(NULL);
}

void WebFrameLoaderClient::download(ResourceHandle* handle, const ResourceRequest& request, const ResourceRequest& initialRequest, const ResourceResponse& response) 
{
    /*
    id proxy = handle->releaseProxy();
    ASSERT(proxy);
    
    WebView *webView = getWebView(m_webFrame.get());
    WebDownload *download = [WebDownload _downloadWithLoadingConnection:handle->connection()
                                                                request:request.nsURLRequest()
                                                               response:response.nsURLResponse()
                                                               delegate:[webView downloadDelegate]
                                                                  proxy:proxy]; 
    NSURL *originalURL = nil;
    WebBackForwardList *history = [webView backForwardList];
    WebHistoryItem *currentItem = nil;
    int backListCount = [history backListCount];
    int backCount = 0;
    
    // find the first item in the history that was originated
    // by the user
    while (backListCount > 0 && !originalURL) {
        currentItem = [history itemAtIndex:backCount--];
        
        if (![currentItem respondsToSelector:@selector(_wasUserGesture)] || [currentItem _wasUserGesture])
            originalURL = [currentItem URL];
    }

    if (originalURL && [download respondsToSelector:@selector(_setOriginatingURL:)])
        [download _setOriginatingURL:originalURL];
    */
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->download(handle, request, response);
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventNewUrlContentArrived, 0, 0 ); // must send this for progress bar to go away
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventUrlLoadingFinished, 0, 0 );
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventContentFinished, 0, 0 );
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadFinished, 0, 0 );
    );
}

void WebFrameLoaderClient::assignIdentifierToInitialRequest(unsigned long identifier, DocumentLoader*, const ResourceRequest&)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    id object;
    BOOL shouldRelease = NO;
    if (implementations.delegateImplementsIdentifierForRequest)
        object = implementations.identifierForRequestFunc(resourceLoadDelegate, @selector(webView:identifierForInitialRequest:fromDataSource:), webView, request.nsURLRequest(), dataSource(loader));
    else {
        object = [[NSObject alloc] init];
        shouldRelease = YES;
    }
    [webView _addObject:object forIdentifier:identifier];
    if (shouldRelease)
        [object release];
    */

    notImplemented(); 
}

void WebFrameLoaderClient::dispatchWillSendRequest(DocumentLoader* loader, unsigned long identifier, ResourceRequest&, const ResourceResponse& redirectResponse)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);
    */
    if (redirectResponse.isNull())
        static_cast<WebDocumentLoader*>(loader)->increaseLoadCount(identifier);
    /*
    if (implementations.delegateImplementsWillSendRequest)
        request = implementations.willSendRequestFunc(resourceLoadDelegate, @selector(webView:resource:willSendRequest:redirectResponse:fromDataSource:), webView, [webView _objectForIdentifier:identifier], request.nsURLRequest(), redirectResponse.nsURLResponse(), dataSource(loader));
    */
}

void WebFrameLoaderClient::dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long identifier, const AuthenticationChallenge&)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = [webView resourceLoadDelegate];
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    NSURLAuthenticationChallenge *webChallenge = mac(challenge);

    if (implementations.delegateImplementsDidReceiveAuthenticationChallenge) {
        [resourceLoadDelegate webView:webView resource:[webView _objectForIdentifier:identifier] didReceiveAuthenticationChallenge:webChallenge fromDataSource:dataSource(loader)];
        return;
    }

    NSWindow *window = [webView hostWindow] ? [webView hostWindow] : [webView window];
    [[WebPanelAuthenticationHandler sharedHandler] startAuthentication:webChallenge window:window];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long identifier, const AuthenticationChallenge&)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = [webView resourceLoadDelegate];
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    NSURLAuthenticationChallenge *webChallenge = mac(challenge);

    if (implementations.delegateImplementsDidCancelAuthenticationChallenge) {
        [resourceLoadDelegate webView:webView resource:[webView _objectForIdentifier:identifier] didCancelAuthenticationChallenge:webChallenge fromDataSource:dataSource(loader)];
        return;
    }

    [(WebPanelAuthenticationHandler *)[WebPanelAuthenticationHandler sharedHandler] cancelAuthentication:webChallenge];
    */
    notImplemented(); 
}       

void WebFrameLoaderClient::dispatchDidReceiveResponse(DocumentLoader* loader, unsigned long identifier, const ResourceResponse& response) 
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    if (implementations.delegateImplementsDidReceiveResponse)
        implementations.didReceiveResponseFunc(resourceLoadDelegate, @selector(webView:resource:didReceiveResponse:fromDataSource:), webView, [webView _objectForIdentifier:identifier], response.nsURLResponse(), dataSource(loader));
    */
    
    m_firstData = true;
    m_response = response;
}

void WebFrameLoaderClient::dispatchDidReceiveContentLength(DocumentLoader*, unsigned long identifier, int lengthReceived)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    if (implementations.delegateImplementsDidReceiveContentLength)
        implementations.didReceiveContentLengthFunc(resourceLoadDelegate, @selector(webView:resource:didReceiveContentLength:fromDataSource:), webView, [webView _objectForIdentifier:identifier], (WebNSUInteger)lengthReceived, dataSource(loader));
    */
    // tot:fixme lemgthReceived is actually the content length, not how much was received. The logic has to change
    total_bytes+=lengthReceived;
}

void WebFrameLoaderClient::dispatchDidFinishLoading(DocumentLoader* loader, unsigned long identifier)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);
    
    if (implementations.delegateImplementsDidFinishLoadingFromDataSource)
        implementations.didFinishLoadingFromDataSourceFunc(resourceLoadDelegate, @selector(webView:resource:didFinishLoadingFromDataSource:), webView, [webView _objectForIdentifier:identifier], dataSource(loader));
    [webView _removeObjectForIdentifier:identifier];

    static_cast<WebDocumentLoaderMac*>(loader)->decreaseLoadCount(identifier);
    */
    static_cast<WebDocumentLoader*>(loader)->decreaseLoadCount(identifier);
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidFailLoading(DocumentLoader* loader, unsigned long identifier, const ResourceError& error)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    if (implementations.delegateImplementsDidFailLoadingWithErrorFromDataSource)
        implementations.didFailLoadingWithErrorFromDataSourceFunc(resourceLoadDelegate, @selector(webView:resource:didFailLoadingWithError:fromDataSource:), webView, [webView _objectForIdentifier:identifier], error, dataSource(loader));
    [webView _removeObjectForIdentifier:identifier];

    static_cast<WebDocumentLoaderMac*>(loader)->decreaseLoadCount(identifier);
    */  
    //
    if (brctl(m_webFrame)->wmlMode() && m_WmlContentListener) {
        m_WmlContentListener->HandleError(identifier,error.errorCode());
    }
    //
    static_cast<WebDocumentLoader*>(loader)->decreaseLoadCount(identifier);
}

bool WebFrameLoaderClient::dispatchDidLoadResourceFromMemoryCache(DocumentLoader*, const ResourceRequest&, const ResourceResponse&, int length)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);

    if (!implementations.delegateImplementsDidLoadResourceFromMemoryCache)
        return false;

    implementations.didLoadResourceFromMemoryCacheFunc(resourceLoadDelegate, @selector(webView:didLoadResourceFromMemoryCache:response:length:fromDataSource:), webView, request.nsURLRequest(), response.nsURLResponse(), length, dataSource(loader));
    return true;
    */
    notImplemented(); 
    return false; 
}

void WebFrameLoaderClient::dispatchDidHandleOnloadEvents()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidHandleOnloadEventsForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didHandleOnloadEventsForFrameFunc(frameLoadDelegate, @selector(webView:didHandleOnloadEventsForFrame:), webView, m_webFrame.get());
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidReceiveServerRedirectForProvisionalLoad()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidReceiveServerRedirectForProvisionalLoadForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didReceiveServerRedirectForProvisionalLoadForFrameFunc(frameLoadDelegate, @selector(webView:didReceiveServerRedirectForProvisionalLoadForFrame:), webView, m_webFrame.get());
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidCancelClientRedirect()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidCancelClientRedirectForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didCancelClientRedirectForFrameFunc(frameLoadDelegate, @selector(webView:didCancelClientRedirectForFrame:), webView, m_webFrame.get());
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchWillPerformClientRedirect(const KURL&, double interval, double fireDate)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsWillPerformClientRedirectToURLDelayFireDateForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.willPerformClientRedirectToURLDelayFireDateForFrameFunc(frameLoadDelegate, @selector(webView:willPerformClientRedirectToURL:delay:fireDate:forFrame:), webView, URL.getNSURL(), delay, [NSDate dateWithTimeIntervalSince1970:fireDate], m_webFrame.get());
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidChangeLocationWithinPage()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidChangeLocationWithinPageForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didChangeLocationWithinPageForFrameFunc(frameLoadDelegate, @selector(webView:didChangeLocationWithinPageForFrame:), webView, m_webFrame.get());
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchWillClose()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());   
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsWillCloseFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.willCloseFrameFunc(frameLoadDelegate, @selector(webView:willCloseFrame:), webView, m_webFrame.get());
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidReceiveIcon()
{
    /*
    ASSERT([m_webFrame.get() _isMainFrame]);
    WebView *webView = getWebView(m_webFrame.get());   

    // FIXME: This willChangeValueForKey call is too late, because the icon has already changed by now.
    [webView _willChangeValueForKey:_WebMainFrameIconKey];

    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidReceiveIconForFrame) {
        Image* image = iconDatabase()->iconForPageURL(core(m_webFrame.get())->loader()->url().url(), IntSize(16, 16));
        if (NSImage *icon = webGetNSImage(image, NSMakeSize(16, 16))) {
            id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
            implementations.didReceiveIconForFrameFunc(frameLoadDelegate, @selector(webView:didReceiveIcon:forFrame:), webView, icon, m_webFrame.get());
        }
    }

    [webView _didChangeValueForKey:_WebMainFrameIconKey];
    */
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventFaviconAvailable, 0, 0 );
    );
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidStartProvisionalLoad() 
{
    /*
    WebView *webView = getWebView(m_webFrame.get());   
    [webView _didStartProvisionalLoadForFrame:m_webFrame.get()];

    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidStartProvisionalLoadForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didStartProvisionalLoadForFrameFunc(frameLoadDelegate, @selector(webView:didStartProvisionalLoadForFrame:), webView, m_webFrame.get());
    }
    */
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventNewContentStart, 0, 0 );
    );
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidReceiveTitle(const String& title) 
{
    /*
    WebView *webView = getWebView(m_webFrame.get());   
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidReceiveTitleForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didReceiveTitleForFrameFunc(frameLoadDelegate, @selector(webView:didReceiveTitle:forFrame:), webView, title, m_webFrame.get());
    }
    */
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventTitleAvailable, 0, 0 );
    );
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidCommitLoad() 
{ 
    /*
    // Tell the client we've committed this URL.
    ASSERT([m_webFrame->_private->webFrameView documentView] != nil);
    
    WebView *webView = getWebView(m_webFrame.get());   
    [webView _didCommitLoadForFrame:m_webFrame.get()];

    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidCommitLoadForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didCommitLoadForFrameFunc(frameLoadDelegate, @selector(webView:didCommitLoadForFrame:), webView, m_webFrame.get());
    }
    */
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventTitleAvailable, 0, 0 );
    );
}

void WebFrameLoaderClient::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());   
    [webView _didFailProvisionalLoadWithError:error forFrame:m_webFrame.get()];
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidFailProvisionalLoadWithErrorForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didFailProvisionalLoadWithErrorForFrameFunc(frameLoadDelegate, @selector(webView:didFailProvisionalLoadWithError:forFrame:), webView, error, m_webFrame.get());
    }
    [m_webFrame->_private->internalLoadDelegate webFrame:m_webFrame.get() didFinishLoadWithError:error];
    */
    //
    if (brctl(m_webFrame)->wmlMode() && m_WmlContentListener) {
        m_WmlContentListener->HandleError(0,error.errorCode());
    }
    //
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadError, 0, 0 );
    );
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidFailLoad(const ResourceError& error)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());   
    [webView _didFailLoadWithError:error forFrame:m_webFrame.get()];

    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidFailLoadWithErrorForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didFailLoadWithErrorForFrameFunc(frameLoadDelegate, @selector(webView:didFailLoadWithError:forFrame:), webView, error, m_webFrame.get());
    }

    [m_webFrame->_private->internalLoadDelegate webFrame:m_webFrame.get() didFinishLoadWithError:error];
    */
    //
    if (brctl(m_webFrame)->wmlMode() && m_WmlContentListener) {
        m_WmlContentListener->HandleError(0,error.errorCode());
    }
    //
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadError, 0, 0 );
    );
    m_webFrame->frameView()->topView()->pageLoadFinished();
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchDidFinishDocumentLoad() // called when parsing is finished
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidFinishDocumentLoadForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didFinishDocumentLoadForFrameFunc(frameLoadDelegate, @selector(webView:didFinishDocumentLoadForFrame:), webView, m_webFrame.get());
    }
    */
}

void WebFrameLoaderClient::dispatchDidFinishLoad()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());   
    [webView _didFinishLoadForFrame:m_webFrame.get()];

    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidFinishLoadForFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didFinishLoadForFrameFunc(frameLoadDelegate, @selector(webView:didFinishLoadForFrame:), webView, m_webFrame.get());
    }

    [m_webFrame->_private->internalLoadDelegate webFrame:m_webFrame.get() didFinishLoadWithError:nil];
    */
    if(  core(m_webFrame)->loader()->isLoadingMainFrame() )
        m_webFrame->frameView()->topView()->pageLoadFinished();
}

void WebFrameLoaderClient::dispatchDidFirstLayout()
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    WebFrameLoadDelegateImplementationCache implementations = WebViewGetFrameLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsDidFirstLayoutInFrame) {
        id frameLoadDelegate = WebViewGetFrameLoadDelegate(webView);
        implementations.didFirstLayoutInFrameFunc(frameLoadDelegate, @selector(webView:didFirstLayoutInFrame:), webView, m_webFrame.get());
    }
    */
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventNewContentDisplayed, 0, 0 );
    );
}

Frame* WebFrameLoaderClient::dispatchCreatePage()
{
    /*
    WebView *currentWebView = getWebView(m_webFrame.get());
    id wd = [currentWebView UIDelegate];
    if ([wd respondsToSelector:@selector(webView:createWebViewWithRequest:)])
        return core([[wd webView:currentWebView createWebViewWithRequest:nil] mainFrame]);
    return 0;
    */
    CBrCtl* newBrCtl = NULL;
    TRAP_IGNORE(newBrCtl = brctl(m_webFrame)->getWindowL(*m_webPolicyManager->newWindowTargetName(),
        m_webPolicyManager->newWindowuserGesture()));
    if (newBrCtl) {
        return core(newBrCtl->webView()->mainFrame());
    }
    return NULL; 
}

void WebFrameLoaderClient::dispatchShow() 
{
    brctl(m_webFrame)->showWindow();
}

//dl mgr
void WebFrameLoaderClient::dispatchDecidePolicyForMIMEType(FramePolicyFunction function, const String& MIMEType, const ResourceRequest& request) 
{ 
    /*
        WebView *webView = getWebView(m_webFrame.get());

    [[webView _policyDelegateForwarder] webView:webView
                        decidePolicyForMIMEType:MIMEType
                                        request:request.nsURLRequest()
                                          frame:m_webFrame.get()
                               decisionListener:setUpPolicyListener(function).get()];

    */
    if (!function) 
        return;
    m_webPolicyManager->dispatchDecidePolicyForMIMEType(function, MIMEType, request);
}

void WebFrameLoaderClient::dispatchDecidePolicyForNewWindowAction(FramePolicyFunction function, const NavigationAction& action, const ResourceRequest& request, const String& frameName)
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    [[webView _policyDelegateForwarder] webView:webView
            decidePolicyForNewWindowAction:actionDictionary(action)
                                   request:request.nsURLRequest()
                              newFrameName:frameName
                          decisionListener:setUpPolicyListener(function).get()];
    */
    if (!function) 
        return;
    m_webPolicyManager->dispatchDecidePolicyForNewWindowAction(function, action, request, frameName);
}

void WebFrameLoaderClient::dispatchDecidePolicyForNavigationAction(FramePolicyFunction function, const NavigationAction& action, const ResourceRequest& request) 
{ 
    /*
    WebView *webView = getWebView(m_webFrame.get());
    [[webView _policyDelegateForwarder] webView:webView
                decidePolicyForNavigationAction:actionDictionary(action)
                                        request:request.nsURLRequest()
                                          frame:m_webFrame.get()
                               decisionListener:setUpPolicyListener(function).get()];
    */
    if (!core(m_webFrame) || !function)
        return;
    m_webPolicyManager->dispatchDecidePolicyForNavigationAction(function, action, request);
}

void WebFrameLoaderClient::cancelPolicyCheck() 
{
    /*
    [m_policyListener.get() invalidate];
    m_policyListener = nil;
    m_policyFunction = 0;
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchUnableToImplementPolicy(const ResourceError&) 
{
    /*
    WebView *webView = getWebView(m_webFrame.get());
    [[webView _policyDelegateForwarder] webView:webView
        unableToImplementPolicyWithError:error frame:m_webFrame.get()];    
    */
    notImplemented(); 
}

void WebFrameLoaderClient::dispatchWillSubmitForm(FramePolicyFunction function, PassRefPtr<FormState> formState)
{
    if (!function)
        return;
    m_webFrame->frameView()->topView()->willSubmitForm(formState.get());
    m_webPolicyManager->dispatchWillSubmitForm(function, formState);
}

void WebFrameLoaderClient::dispatchDidLoadMainResource(DocumentLoader*) 
{
    /*
    if ([WebScriptDebugServer listenerCount])
        [[WebScriptDebugServer sharedScriptDebugServer] webView:getWebView(m_webFrame.get())
            didLoadMainResourceForDataSource:dataSource(loader)];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::revertToProvisionalState(DocumentLoader*)
{
    //    [dataSource(loader) _revertToProvisionalState];
    notImplemented(); 
}

void WebFrameLoaderClient::setMainDocumentError(DocumentLoader* loader, const ResourceError& error) 
{
    //    [dataSource(loader) _setMainDocumentError:error];
    if (brctl(m_webFrame)->brCtlDialogsProvider()) {
        int err = mapHttpErrors(error.errorCode());
        if (err <= KBrowserHTTP100 && err > KBrowserFailure) {
            if (total_bytes == 0 || err == KBrowserHTTP502 ) {
                HBufC* url = NULL;
                url = HBufC::New(loader->originalURL().des().Length());
                if (url) {
                    url->Des().Copy(loader->originalURL().des());
                    TRAP_IGNORE(brctl(m_webFrame)->brCtlDialogsProvider()->DialogNotifyHttpErrorL(err, *url));
                    delete url;
                }
            }
        }
        else {
            if (err != KBrowserCancelled) {
                TRAP_IGNORE(brctl(m_webFrame)->brCtlDialogsProvider()->DialogNotifyErrorL(err));
            }
        }
    }
}

void WebFrameLoaderClient::clearUnarchivingState(DocumentLoader*) 
{
    notImplemented(); 
}

// Maybe these should go into a ProgressTrackerClient some day
void WebFrameLoaderClient::willChangeEstimatedProgress() 
{
    //    [dataSource(loader) _clearUnarchivingState];
}

void WebFrameLoaderClient::didChangeEstimatedProgress() 
{
    //    [getWebView(m_webFrame.get()) _willChangeValueForKey:_WebEstimatedProgressKey];
    notImplemented(); 
}

void WebFrameLoaderClient::postProgressStartedNotification() 
{
    //    [[NSNotificationCenter defaultCenter] postNotificationName:WebViewProgressStartedNotification object:getWebView(m_webFrame.get())];
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventUrlLoadingStart, 0, 0 );
    );
    total_bytes = 0;
#ifdef PERF_REGRESSION_LOG
        wkDebug()<<"Load started"<<mem<<flush;
#endif // PERF_REGRESSION_LOG
}

void WebFrameLoaderClient::postProgressEstimateChangedNotification()
{
#ifdef PERF_REGRESSION_LOG
    wkDebug()<<"Loading..."<<total_bytes<<"bytes"<<mem<<flush;
#endif // PERF_REGRESSION_LOG
    //    [[NSNotificationCenter defaultCenter] postNotificationName:WebViewProgressEstimateChangedNotification object:getWebView(m_webFrame.get())];
    double estimatedProgress = core(m_webFrame)->page()->progress()->estimatedProgress();
    TRAP_IGNORE(
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventNewUrlContentArrived, total_bytes / estimatedProgress, 0 );
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventMoreUrlContentArrived, total_bytes, 0 );
    );
}

void WebFrameLoaderClient::postProgressFinishedNotification() 
{
    TRAP_IGNORE(postProgressFinishedNotificationL());
}

void WebFrameLoaderClient::postProgressFinishedNotificationL() 
{
#ifdef PERF_REGRESSION_LOG
    wkDebug()<<"Load finished "<<core(m_webFrame)->page()->mainFrame()->document()->documentURI().des()<<mem<<flush;
#endif // PERF_REGRESSION_LOG
    //    [[NSNotificationCenter defaultCenter] postNotificationName:WebViewProgressFinishedNotification object:getWebView(m_webFrame.get())];
    //core(m_webFrame)->bridge()->Callback().postProgressFinishedNotification();
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventMoreUrlContentArrived, total_bytes, 0 );
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventUrlLoadingFinished, 0, 0 );
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventContentFinished, 0, 0 );
    brctl(m_webFrame)->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadFinished, 0, 0 );
    m_webFrame->frameView()->topView()->pageLoadFinished();
}

void WebFrameLoaderClient::setMainFrameDocumentReady(bool) 
{
    //    [getWebView(m_webFrame.get()) setMainFrameDocumentReady:ready];
    notImplemented(); 
}

void WebFrameLoaderClient::startDownload(const ResourceRequest&) 
{
    /*
    // FIXME: Should download full request.
    [getWebView(m_webFrame.get()) _downloadURL:request.url().getNSURL()];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::willChangeTitle(DocumentLoader*) 
{
    /*
    // FIXME: Should do this only in main frame case, right?
    [getWebView(m_webFrame.get()) _willChangeValueForKey:_WebMainFrameTitleKey];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::didChangeTitle(DocumentLoader*)
{
    /*
    // FIXME: Should do this only in main frame case, right?
    [getWebView(m_webFrame.get()) _didChangeValueForKey:_WebMainFrameTitleKey];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::committedLoad(DocumentLoader* loader, const char* data, int length) 
{ 
    if (brctl(m_webFrame)->wmlMode()) {
        TRAP_IGNORE(
        m_WmlContentListener->ResponseL( data, length );
        );
        return;
    }
    m_webFrame->bridge()->receivedData(data, length, m_response.textEncodingName());
}

void WebFrameLoaderClient::finishedLoading(DocumentLoader* loader) 
{ 
    if (!brctl(m_webFrame)->wmlMode()) {
        m_webFrame->bridge()->receivedData(NULL, 0, m_response.textEncodingName());
    }
}

void WebFrameLoaderClient::finalSetupForReplace(DocumentLoader*)
{
    //    [dataSource(loader) _clearUnarchivingState];
    notImplemented(); 
}

void WebFrameLoaderClient::updateGlobalHistoryForStandardLoad(const KURL& url) 
{ 
    /*
    NSURL *nsurl = url.getNSURL();
    WebHistoryItem *entry = [[WebHistory optionalSharedHistory] addItemForURL:nsurl];
    String pageTitle = core(m_webFrame.get())->loader()->documentLoader()->title();
    if (pageTitle.length())
        [entry setTitle:pageTitle];
    */

    if (!core(m_webFrame)->ownerElement()) {
        ResourceRequest* r = &(core(m_webFrame)->loader()->activeDocumentLoader()->request());
        TPtrC formContentType(KNullDesC);
        WebCore::FormData* formData = NULL;

        if (equalIgnoringCase(r->httpMethod(), "POST")) {
            formContentType.Set(r->httpContentType().des());
            formData = r->httpBody();
        }
        TPtrC8 responseUrl(r->url().des());
        TPtrC8 requestUrl(url.des());

        brctl(m_webFrame)->historyHandler()->updateGlobalHistoryForStandardLoad(responseUrl, requestUrl, formContentType, formData);
    }
}

void WebFrameLoaderClient::updateGlobalHistoryForReload(const KURL&)
{
    /*
    WebHistory *sharedHistory = [WebHistory optionalSharedHistory];
    WebHistoryItem *item = [sharedHistory itemForURL:url.getNSURL()];
    if (item)
        [sharedHistory setLastVisitedTimeInterval:[NSDate timeIntervalSinceReferenceDate] forItem:item];
    */
     brctl(m_webFrame)->historyHandler()->updateGlobalHistoryForReload();
}

bool WebFrameLoaderClient::shouldGoToHistoryItem(HistoryItem*) const 
{
    /*
    WebView* view = getWebView(m_webFrame.get());
    WebHistoryItem *webItem = kit(item);
    
    return [[view _policyDelegateForwarder] webView:view shouldGoToHistoryItem:webItem];
    */
    notImplemented();
    return false; 
}

ResourceError WebFrameLoaderClient::cancelledError(const ResourceRequest& request_)
{
    //    return [NSError _webKitErrorWithDomain:NSURLErrorDomain code:NSURLErrorCancelled URL:request.url().getNSURL()];
    return ResourceError(String(), KErrCancel, request_.url().url(), String());
}

ResourceError WebFrameLoaderClient::blockedError(const ResourceRequest&)
{
    //    return [NSError _webKitErrorWithDomain:WebKitErrorDomain code:WebKitErrorCannotUseRestrictedPort URL:request.url().getNSURL()];
    notImplemented();
    return ResourceError(); 
}

ResourceError WebFrameLoaderClient::cannotShowURLError(const ResourceRequest&)
{
    //    return [NSError _webKitErrorWithDomain:WebKitErrorDomain code:WebKitErrorCannotShowURL URL:request.url().getNSURL()];
    notImplemented();
    return ResourceError(); 
}

ResourceError WebFrameLoaderClient::interruptForPolicyChangeError(const ResourceRequest& request)
{
    //    return [NSError _webKitErrorWithDomain:WebKitErrorDomain code:WebKitErrorFrameLoadInterruptedByPolicyChange URL:request.url().getNSURL()];
    return ResourceError(String(), KErrCancel, request.url().url(), String());
}

ResourceError WebFrameLoaderClient::cannotShowMIMETypeError(const ResourceResponse&) 
{
    //    return [NSError _webKitErrorWithDomain:NSURLErrorDomain code:WebKitErrorCannotShowMIMEType URL:response.url().getNSURL()];
    notImplemented();
    return ResourceError(); 
}

ResourceError WebFrameLoaderClient::fileDoesNotExistError(const ResourceResponse&)
{
//        return [NSError _webKitErrorWithDomain:NSURLErrorDomain code:NSURLErrorFileDoesNotExist URL:response.url().getNSURL()];    
    notImplemented();
    return ResourceError(); 
}

bool WebFrameLoaderClient::shouldFallBack(const ResourceError&)
{
    /*
    // FIXME: Needs to check domain.
    // FIXME: WebKitErrorPlugInWillHandleLoad is a workaround for the cancel we do to prevent
    // loading plugin content twice.  See <rdar://problem/4258008>
    return error.errorCode() != NSURLErrorCancelled && error.errorCode() != WebKitErrorPlugInWillHandleLoad;
    */
    notImplemented(); 
    return false; 
}

void WebFrameLoaderClient::setDefersLoading(bool defers)
{
    /*
    if (!defers)
        deliverArchivedResourcesAfterDelay();
    */
    notImplemented(); 
}

bool WebFrameLoaderClient::willUseArchive(ResourceLoader*, const ResourceRequest&, const KURL& originalURL) const
{
    /*
    if (request.url() != originalURL)
        return false;
    if (!canUseArchivedResource(request.nsURLRequest()))
        return false;
    WebResource *resource = [dataSource(core(m_webFrame.get())->loader()->activeDocumentLoader()) _archivedSubresourceForURL:originalURL.getNSURL()];
    if (!resource)
        return false;
    if (!canUseArchivedResource([resource _response]))
        return false;
    m_pendingArchivedResources.set(loader, resource);
    // Deliver the resource after a delay because callers don't expect to receive callbacks while calling this method.
    deliverArchivedResourcesAfterDelay();
    return true;
    */
    notImplemented(); 
    return false; 
}

bool WebFrameLoaderClient::isArchiveLoadPending(ResourceLoader*) const 
{
    //    return m_pendingArchivedResources.contains(loader);
    notImplemented(); 
    return false; 
}

void WebFrameLoaderClient::cancelPendingArchiveLoad(ResourceLoader*) 
{
    /*
    if (m_pendingArchivedResources.isEmpty())
        return;
    m_pendingArchivedResources.remove(loader);
    if (m_pendingArchivedResources.isEmpty())
        m_archivedResourcesDeliveryTimer.stop();
    */
    notImplemented(); 
}

void WebFrameLoaderClient::clearArchivedResources() 
{
    /*
    m_pendingArchivedResources.clear();
    m_archivedResourcesDeliveryTimer.stop();
    */
    notImplemented(); 
}

bool WebFrameLoaderClient::canHandleRequest(const ResourceRequest&) const
{
    //    return [WebView _canHandleRequest:request.nsURLRequest()];
    notImplemented(); 
    return true; 
}

bool WebFrameLoaderClient::canShowMIMEType(const String& MIMEType) const 
{
    //    return [WebView canShowMIMEType:MIMEType];
    notImplemented(); 
    return true; 
}

bool WebFrameLoaderClient::representationExistsForURLScheme(const String& URLScheme) const 
{
    //    return [WebView _representationExistsForURLScheme:URLScheme];
    notImplemented();
    return false; 
}

String WebFrameLoaderClient::generatedMIMETypeForURLScheme(const String& URLScheme) const 
{
    //    return [WebView _generatedMIMETypeForURLScheme:URLScheme];
    notImplemented();
    return String(); 
}

void WebFrameLoaderClient::frameLoadCompleted()
{
    if (brctl(m_webFrame)->wmlMode()) {
        TRAP_IGNORE(
		if (m_WmlContentListener)
        	m_WmlContentListener->CompleteL( 0, 0 );
        );
        return;
    }
    /*
    // Note: Can be called multiple times.
    // Even if already complete, we might have set a previous item on a frame that
    // didn't do any data loading on the past transaction. Make sure to clear these out.
    NSScrollView *sv = [m_webFrame->_private->webFrameView _scrollView];
    if ([getWebView(m_webFrame.get()) drawsBackground])
        [sv setDrawsBackground:YES];
    core(m_webFrame.get())->loader()->setPreviousHistoryItem(0);
    */
    notImplemented(); 
}

void WebFrameLoaderClient::saveViewStateToItem(HistoryItem*)
{
    /*
    if (!item)
        return;
    
    NSView <WebDocumentView> *docView = [m_webFrame->_private->webFrameView documentView];

    // we might already be detached when this is called from detachFromParent, in which
    // case we don't want to override real data earlier gathered with (0,0)
    if ([docView superview] && [docView conformsToProtocol:@protocol(_WebDocumentViewState)])
        item->setViewState([(id <_WebDocumentViewState>)docView viewState]);
    */
    notImplemented(); 
}

void WebFrameLoaderClient::restoreViewState()
{
    /*
    HistoryItem* currentItem = core(m_webFrame.get())->loader()->currentHistoryItem();
    ASSERT(currentItem);

    // FIXME: As the ASSERT attests, it seems we should always have a currentItem here.
    // One counterexample is <rdar://problem/4917290>
    // For now, to cover this issue in release builds, there is no technical harm to returning
    // early and from a user standpoint - as in the above radar - the previous page load failed 
    // so there *is* no scroll state to restore!
    if (!currentItem)
        return;
    
    NSView <WebDocumentView> *docView = [m_webFrame->_private->webFrameView documentView];
    if ([docView conformsToProtocol:@protocol(_WebDocumentViewState)]) {        
        id state = currentItem->viewState();
        if (state) {
            [(id <_WebDocumentViewState>)docView setViewState:state];
        }
    }
    */
    notImplemented(); 
}

void WebFrameLoaderClient::provisionalLoadStarted()
{ 
    /*
    // FIXME: This is OK as long as no one resizes the window,
    // but in the case where someone does, it means garbage outside
    // the occupied part of the scroll view.
    [[m_webFrame->_private->webFrameView _scrollView] setDrawsBackground:NO];
    */
    notImplemented(); 
}

void WebFrameLoaderClient::didFinishLoad() 
{
    //    [m_webFrame->_private->internalLoadDelegate webFrame:m_webFrame.get() didFinishLoadWithError:nil];    
    notImplemented(); 
}

void WebFrameLoaderClient::prepareForDataSourceReplacement() 
{
    /*
    if (![m_webFrame.get() _dataSource]) {
        ASSERT(!core(m_webFrame.get())->tree()->childCount());
        return;
    }
    
    // Make sure that any work that is triggered by resigning first reponder can get done.
    // The main example where this came up is the textDidEndEditing that is sent to the
    // FormsDelegate (3223413).  We need to do this before _detachChildren, since that will
    // remove the views as a side-effect of freeing the bridge, at which point we can't
    // post the FormDelegate messages.
    //
    // Note that this can also take FirstResponder away from a child of our frameView that
    // is not in a child frame's view.  This is OK because we are in the process
    // of loading new content, which will blow away all editors in this top frame, and if
    // a non-editor is firstReponder it will not be affected by endEditingFor:.
    // Potentially one day someone could write a DocView whose editors were not all
    // replaced by loading new content, but that does not apply currently.
    NSView *frameView = m_webFrame->_private->webFrameView;
    NSWindow *window = [frameView window];
    NSResponder *firstResp = [window firstResponder];
    if ([firstResp isKindOfClass:[NSView class]] && [(NSView *)firstResp isDescendantOf:frameView])
        [window endEditingFor:firstResp];
    */
    notImplemented(); 
}

PassRefPtr<DocumentLoader> WebFrameLoaderClient::createDocumentLoader(const ResourceRequest& request, const SubstituteData& substituteData) 
{
//    RefPtr<WebDocumentLoader> loader = new WebDocumentLoader(request, substituteData);
//    WebDataSource *dataSource = new WebDataSource;
//    dataSource->initWithDocumentLoader(loader.get());
//    loader->setDataSource(dataSource, getWebView(m_webFrame.get()));
//    dataSource->release();
//    return loader.release();
    RefPtr<DocumentLoader> loader = new WebDocumentLoader(request, substituteData);

    return loader.release();
}

void WebFrameLoaderClient::setTitle(const String& title, const KURL&) 
{ 
    /*
    NSURL* nsURL = canonicalURL(URL.getNSURL());
    if(!nsURL)
        return;
    NSString *titleNSString = title;
    [[[WebHistory optionalSharedHistory] itemForURL:nsURL] setTitle:titleNSString];
    */
    brctl(m_webFrame)->historyHandler()->setCurrentEntryTitle(title.des());    
}

String WebFrameLoaderClient::userAgent(const KURL&) 
{
    //    return [getWebView(m_webFrame.get()) _userAgentForURL:url];
    return m_webFrame->frameView()->topView()->userAgent();
}

TInt WebFrameLoaderClient::widgetNetworkConstants(TInt aId)
{
    CWidgetExtension* widgetExt = m_webFrame->frameView()->topView()->widgetExtension();
    if ( widgetExt ) {
        return widgetExt->widgetNetworkConstants(aId);
    }
    
    notImplemented();
    return 0;
}

TInt WebFrameLoaderClient::widgetNetworkState()
{
    CWidgetExtension* widgetExt = m_webFrame->frameView()->topView()->widgetExtension();
    if ( widgetExt ) {
        return widgetExt->widgetNetworkState();
    }
    
    notImplemented();
    return 0;
}

void WebFrameLoaderClient::saveDocumentViewToCachedPage(CachedPage*) 
{
    //    cachedPage->setDocumentView([m_webFrame->_private->webFrameView documentView]);
    notImplemented(); 
}

bool WebFrameLoaderClient::canCachePage() const 
{
    /*
    // We can only cache HTML pages right now
    return [[[m_webFrame.get() _dataSource] representation] isKindOfClass:[WebHTMLRepresentation class]];
    */
    notImplemented();
    return false; 
}


PassRefPtr<Frame> WebFrameLoaderClient::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
    const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
    /*
    WebFrameBridge* bridge = m_webFrame->_private->bridge;
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    
    return [bridge createChildFrameNamed:name
                   withURL:url.getNSURL()
                   referrer:referrer 
                   ownerElement:ownerElement
                   allowsScrolling:allowsScrolling
                   marginWidth:marginWidth
                   marginHeight:marginHeight];

    END_BLOCK_OBJC_EXCEPTIONS;
    return 0;
    */
    return m_webFrame->bridge()->createChildFrameNamed(&name, url.des(), referrer.des(),
        ownerElement, allowsScrolling, marginWidth, marginHeight);
}

Widget* WebFrameLoaderClient::createPlugin(const WebCore::IntSize&, Element* pluginElement, const KURL& url, const Vector<String>& paramNames,
                                               const Vector<String>& paramValues, const String& mimeType, bool pluginDocument)
{
    bool canLoad = true;
    
    if ( pluginElement ) {
        HTMLObjectElement *objectNode = static_cast<HTMLObjectElement* >(pluginElement);
        canLoad = objectNode->canLoadPlugin();
    }
    
    return new Widget( m_webFrame->bridge()->viewForPluginWithURL( url.des(), paramNames, paramValues,
            core(m_webFrame)->loader()->baseURL(), mimeType, canLoad, pluginElement ));
}

void WebFrameLoaderClient::redirectDataToPlugin(Widget* pluginWidget) 
{
    /*
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    [m_webFrame->_private->bridge redirectDataToPlugin:pluginWidget->getView()];
    END_BLOCK_OBJC_EXCEPTIONS;
    */
//    core(m_webFrame)->bridge()->Callback().RedirectDataToPlugin( pluginWidget->getView() );
}

Widget* WebFrameLoaderClient::createJavaAppletWidget(const IntSize&, Element*, const KURL& baseURL, const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    /*
    Widget* result = new Widget;
    
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    WebFrameBridge* bridge = m_webFrame->_private->bridge;
    result->setView([bridge viewForJavaAppletWithFrame:NSMakeRect(0, 0, size.width(), size.height())
                            attributeNames:nsArray(paramNames)
                            attributeValues:nsArray(paramValues)
                            baseURL:baseURL.getNSURL()
                            DOMElement:[DOMElement _wrapElement:element]]);    
    END_BLOCK_OBJC_EXCEPTIONS;
    
    return result;
    */
    notImplemented();
    return 0; 
}

ObjectContentType WebFrameLoaderClient::objectContentType(const KURL& url, const String& mimeType)
{
    /*
    WebFrameBridge* bridge = m_webFrame->_private->bridge;
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    return (ObjectContentType)[bridge determineObjectFromMIMEType:mimeType URL:url.getNSURL()];
    END_BLOCK_OBJC_EXCEPTIONS;
    return ObjectContentNone;
    */

    ObjectContentType contentType = ObjectContentNone;
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();     
    TInt  handle = pluginHandler->findPlugin( mimeType.des() );        
    
    if (handle == KErrNotFound) {                        
      handle = pluginHandler->findPluginByExtension( url.des() );        
    }
    
    if (handle != KErrNotFound) {
        contentType = ObjectContentNetscapePlugin;
    }

    if (MIMETypeRegistry::isSupportedImageMIMEType(mimeType))
        return WebCore::ObjectContentImage;
    return contentType; 
}

String WebFrameLoaderClient::overrideMediaType() const
{
    /*
    NSString* overrideType = [m_webFrame->_private->bridge overrideMediaType];
    if (overrideType)
        return overrideType;
    return String();
    */
    notImplemented();
    return String(); 
}

void WebFrameLoaderClient::windowObjectCleared() const
{
    if (m_webFrame && !m_webFrame->parentFrame()) {
        m_webFrame->frameView()->topView()->windowObjectCleared();
    }
}

void WebFrameLoaderClient::didPerformFirstNavigation() const
{
    notImplemented();
}

void WebFrameLoaderClient::registerForIconNotification(bool listen)
{
//    [[m_webFrame.get() webView] _registerForIconNotification:listen];
//tot:fixme Do this when SQLite is capable to support multithreading.
/*
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_receivedIconChangedNotification:) name:WebIconDatabaseDidAddIconNotification object:nil];        
    else
        [[NSNotificationCenter defaultCenter] removeObserver:self name:WebIconDatabaseDidAddIconNotification object:nil];

  */
    notImplemented();
}

void WebFrameLoaderClient::ignore(WebCore::FramePolicyFunction function)
{
    (core(m_webFrame)->loader()->*function)(WebCore::PolicyIgnore);
}

void WebFrameLoaderClient::download(WebCore::FramePolicyFunction function)
{
    (core(m_webFrame)->loader()->*function)(WebCore::PolicyDownload);
}

void WebFrameLoaderClient::use(WebCore::FramePolicyFunction function)
{
    (core(m_webFrame)->loader()->*function)(WebCore::PolicyUse);
}

bool WebFrameLoaderClient::isWMLContent(const String& MIMEType)
{
    bool found = false;
#ifndef BRDO_WML_DISABLED_FF    
    if ( MIMEType == "text/vnd.wap.wml" || MIMEType == "text/vnd.wap.wmlc" || MIMEType == "application/vnd.wap.wmlc" || MIMEType == "application/vnd.wap.wmlscriptc") {
        found = true;
    }
#endif    
    return found;
}




/*
static inline WebDataSource *dataSource(DocumentLoader* loader)
{
    return loader ? static_cast<WebDocumentLoaderMac*>(loader)->dataSource() : nil;
}

WebFrameLoaderClient::WebFrameLoaderClient(WebFrame *webFrame)
    : m_webFrame(webFrame)
    , m_policyFunction(0)
    , m_archivedResourcesDeliveryTimer(this, &WebFrameLoaderClient::deliverArchivedResources)
{
}

NSCachedURLResponse* WebFrameLoaderClient::willCacheResponse(DocumentLoader* loader, unsigned long identifier, NSCachedURLResponse* response) const
{
    WebView *webView = getWebView(m_webFrame.get());
    id resourceLoadDelegate = WebViewGetResourceLoadDelegate(webView);
    WebResourceDelegateImplementationCache implementations = WebViewGetResourceLoadDelegateImplementations(webView);
    if (implementations.delegateImplementsWillCacheResponse)
        return implementations.willCacheResponseFunc(resourceLoadDelegate, @selector(webView:resource:willCacheResponse:fromDataSource:), webView, [webView _objectForIdentifier:identifier], response, dataSource(loader));

    return response;
}

// The following 2 functions are copied from [NSHTTPURLProtocol _cachedResponsePassesValidityChecks] and modified for our needs.
// FIXME: It would be nice to eventually to share this logic somehow.
bool WebFrameLoaderClient::canUseArchivedResource(NSURLRequest *request) const
{
    NSURLRequestCachePolicy policy = [request cachePolicy];
    if (policy == NSURLRequestReturnCacheDataElseLoad)
        return true;
    if (policy == NSURLRequestReturnCacheDataDontLoad)
        return true;
    if ([request valueForHTTPHeaderField:@"must-revalidate"] != nil)
        return false;
    if ([request valueForHTTPHeaderField:@"proxy-revalidate"] != nil)
        return false;
    if ([request valueForHTTPHeaderField:@"If-Modified-Since"] != nil)
        return false;
    if ([request valueForHTTPHeaderField:@"Cache-Control"] != nil)
        return false;
    if ([@"POST" _webkit_isCaseInsensitiveEqualToString:[request HTTPMethod]])
        return false;
    return true;
}

bool WebFrameLoaderClient::canUseArchivedResource(NSURLResponse *response) const
{
    if (WKGetNSURLResponseMustRevalidate(response))
        return false;
    if (WKGetNSURLResponseCalculatedExpiration(response) - CFAbsoluteTimeGetCurrent() < 1)
        return false;
    return true;
}

void WebFrameLoaderClient::deliverArchivedResourcesAfterDelay() const
{
    if (m_pendingArchivedResources.isEmpty())
        return;
    if (core(m_webFrame.get())->page()->defersLoading())
        return;
    if (!m_archivedResourcesDeliveryTimer.isActive())
        m_archivedResourcesDeliveryTimer.startOneShot(0);
}

void WebFrameLoaderClient::deliverArchivedResources(Timer<WebFrameLoaderClient>*)
{
    if (m_pendingArchivedResources.isEmpty())
        return;
    if (core(m_webFrame.get())->page()->defersLoading())
        return;

    const ResourceMap copy = m_pendingArchivedResources;
    m_pendingArchivedResources.clear();

    ResourceMap::const_iterator end = copy.end();
    for (ResourceMap::const_iterator it = copy.begin(); it != end; ++it) {
        RefPtr<ResourceLoader> loader = it->first;
        WebResource *resource = it->second.get();
        NSData *data = [[resource data] retain];
        loader->didReceiveResponse([resource _response]);
        loader->didReceiveData((const char*)[data bytes], [data length], [data length], true);
        [data release];
        loader->didFinishLoading();
    }
}

RetainPtr<WebFramePolicyListener> WebFrameLoaderClient::setUpPolicyListener(FramePolicyFunction function)
{
    ASSERT(!m_policyListener);
    ASSERT(!m_policyFunction);

    [m_policyListener.get() invalidate];

    WebFramePolicyListener *listener = [[WebFramePolicyListener alloc] initWithWebCoreFrame:core(m_webFrame.get())];
    m_policyListener = listener;
    [listener release];
    m_policyFunction = function;

    return listener;
}

void WebFrameLoaderClient::receivedPolicyDecison(PolicyAction action)
{
    ASSERT(m_policyListener);
    ASSERT(m_policyFunction);

    FramePolicyFunction function = m_policyFunction;

    m_policyListener = nil;
    m_policyFunction = 0;

    (core(m_webFrame.get())->loader()->*function)(action);
}

String WebFrameLoaderClient::userAgent(const KURL& url)
{
    return [getWebView(m_webFrame.get()) _userAgentForURL:url];
}

static const MouseEvent* findMouseEvent(const Event* event)
{
    for (const Event* e = event; e; e = e->underlyingEvent())
        if (e->isMouseEvent())
            return static_cast<const MouseEvent*>(e);
    return 0;
}

NSDictionary *WebFrameLoaderClient::actionDictionary(const NavigationAction& action) const
{
    unsigned modifierFlags = 0;
    const Event* event = action.event();
    if (const UIEventWithKeyState* keyStateEvent = findEventWithKeyState(const_cast<Event*>(event))) {
        if (keyStateEvent->ctrlKey())
            modifierFlags |= NSControlKeyMask;
        if (keyStateEvent->altKey())
            modifierFlags |= NSAlternateKeyMask;
        if (keyStateEvent->shiftKey())
            modifierFlags |= NSShiftKeyMask;
        if (keyStateEvent->metaKey())
            modifierFlags |= NSCommandKeyMask;
    }
    if (const MouseEvent* mouseEvent = findMouseEvent(event)) {
        IntPoint point(mouseEvent->clientX(), mouseEvent->clientY());
        WebElementDictionary *element = [[WebElementDictionary alloc]
            initWithHitTestResult:core(m_webFrame.get())->eventHandler()->hitTestResultAtPoint(point, false)];
        NSDictionary *result = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithInt:action.type()], WebActionNavigationTypeKey,
            element, WebActionElementKey,
            [NSNumber numberWithInt:mouseEvent->button()], WebActionButtonKey,
            [NSNumber numberWithInt:modifierFlags], WebActionModifierFlagsKey,
            action.URL().getNSURL(), WebActionOriginalURLKey,
            nil];
        [element release];
        return result;
    }
    return [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithInt:action.type()], WebActionNavigationTypeKey,
        [NSNumber numberWithInt:modifierFlags], WebActionModifierFlagsKey,
        action.URL().getNSURL(), WebActionOriginalURLKey,
        nil];
}

static NSArray* nsArray(const Vector<String>& vector)
{
    unsigned len = vector.size();
    NSMutableArray* array = [NSMutableArray arrayWithCapacity:len];
    for (unsigned x = 0; x < len; x++)
        [array addObject:vector[x]];
    return array;
}

- (id)initWithWebCoreFrame:(Frame*)frame
{
    self = [self init];
    if (!self)
        return nil;
    frame->ref();
    m_frame = frame;
    return self;
}

- (void)invalidate
{
    if (m_frame) {
        m_frame->deref();
        m_frame = 0;
    }
}

- (void)dealloc
{
    if (m_frame)
        m_frame->deref();
    [super dealloc];
}

- (void)finalize
{
    ASSERT_MAIN_THREAD();
    if (m_frame)
        m_frame->deref();
    [super finalize];
}


*/



