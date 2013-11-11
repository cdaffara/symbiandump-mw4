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
* Description:   extension of WebCore::FrameLoaderClient
*
*/

#ifndef WebFrameLoaderClient_h
#define WebFrameLoaderClient_h

#include "FrameLoaderClient.h"
#include "ResourceResponse.h"
#include "Shared.h"

class WebPolicyManager;
class WebFrame;
class CWmlDispatcher;

namespace WebCore
{
    class Frame;
}

class WebFrameLoaderClient : public WebCore::FrameLoaderClient, public WebCore::Shared<WebFrameLoaderClient> 
    {
    public:
        WebFrameLoaderClient(WebFrame*);
        virtual ~WebFrameLoaderClient();
        
        virtual void frameLoaderDestroyed();
        
        virtual bool hasWebView() const; // mainly for assertions
        virtual bool hasFrameView() const; // ditto

        virtual bool privateBrowsingEnabled() const;

        virtual void makeDocumentView();
        virtual void makeRepresentation(WebCore::DocumentLoader*);
        virtual void setDocumentViewFromCachedPage(WebCore::CachedPage*);
        virtual void forceLayout();
        virtual void forceLayoutForNonHTML();

        virtual void setCopiesOnScroll();

        virtual void detachedFromParent2();
        virtual void detachedFromParent3();
        virtual void detachedFromParent4();


        virtual void assignIdentifierToInitialRequest(unsigned long identifier, WebCore::DocumentLoader*, const WebCore::ResourceRequest&);

        virtual void dispatchWillSendRequest(WebCore::DocumentLoader*, unsigned long identifier, WebCore::ResourceRequest&, const WebCore::ResourceResponse& redirectResponse);
        virtual void dispatchDidReceiveAuthenticationChallenge(WebCore::DocumentLoader*, unsigned long identifier, const WebCore::AuthenticationChallenge&);
        virtual void dispatchDidCancelAuthenticationChallenge(WebCore::DocumentLoader*, unsigned long identifier, const WebCore::AuthenticationChallenge&);        
        virtual void dispatchDidReceiveResponse(WebCore::DocumentLoader*, unsigned long identifier, const WebCore::ResourceResponse&);
        virtual void dispatchDidReceiveContentLength(WebCore::DocumentLoader*, unsigned long identifier, int lengthReceived);
        virtual void dispatchDidFinishLoading(WebCore::DocumentLoader*, unsigned long identifier);
        virtual void dispatchDidFailLoading(WebCore::DocumentLoader*, unsigned long identifier, const WebCore::ResourceError&);
        virtual bool dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int length);

        virtual void dispatchDidHandleOnloadEvents();
        virtual void dispatchDidReceiveServerRedirectForProvisionalLoad();
        virtual void dispatchDidCancelClientRedirect();
        virtual void dispatchWillPerformClientRedirect(const WebCore::KURL&, double interval, double fireDate);
        virtual void dispatchDidChangeLocationWithinPage();
        virtual void dispatchWillClose();
        virtual void dispatchDidReceiveIcon();
        virtual void dispatchDidStartProvisionalLoad();
        virtual void dispatchDidReceiveTitle(const WebCore::String& title);
        virtual void dispatchDidCommitLoad();
        virtual void dispatchDidFailProvisionalLoad(const WebCore::ResourceError&);
        virtual void dispatchDidFailLoad(const WebCore::ResourceError&);
        virtual void dispatchDidFinishDocumentLoad();
        virtual void dispatchDidFinishLoad();
        virtual void dispatchDidFirstLayout();

        virtual WebCore::Frame* dispatchCreatePage();
        virtual void dispatchShow();

        virtual void dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction, const WebCore::String& MIMEType, const WebCore::ResourceRequest&);
        virtual void dispatchDecidePolicyForNewWindowAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&, const WebCore::String& frameName);
        virtual void dispatchDecidePolicyForNavigationAction(WebCore::FramePolicyFunction, const WebCore::NavigationAction&, const WebCore::ResourceRequest&);
        virtual void cancelPolicyCheck();

        virtual void dispatchUnableToImplementPolicy(const WebCore::ResourceError&);

        virtual void dispatchWillSubmitForm(WebCore::FramePolicyFunction, PassRefPtr<WebCore::FormState>);

        virtual void dispatchDidLoadMainResource(WebCore::DocumentLoader*);
        virtual void revertToProvisionalState(WebCore::DocumentLoader*);
        virtual void setMainDocumentError(WebCore::DocumentLoader*, const WebCore::ResourceError&);
        virtual void clearUnarchivingState(WebCore::DocumentLoader*);

        // Maybe these should go into a ProgressTrackerClient some day
        virtual void willChangeEstimatedProgress();
        virtual void didChangeEstimatedProgress();
        virtual void postProgressStartedNotification();
        virtual void postProgressEstimateChangedNotification();
        virtual void postProgressFinishedNotification();
        
        virtual void setMainFrameDocumentReady(bool);

        virtual void startDownload(const WebCore::ResourceRequest&);

        virtual void willChangeTitle(WebCore::DocumentLoader*);
        virtual void didChangeTitle(WebCore::DocumentLoader*);

        virtual void committedLoad(WebCore::DocumentLoader*, const char*, int);
        virtual void finishedLoading(WebCore::DocumentLoader*);
        virtual void finalSetupForReplace(WebCore::DocumentLoader*);
        
        virtual void updateGlobalHistoryForStandardLoad(const WebCore::KURL&);
        virtual void updateGlobalHistoryForReload(const WebCore::KURL&);
        virtual bool shouldGoToHistoryItem(WebCore::HistoryItem*) const;

        virtual WebCore::ResourceError cancelledError(const WebCore::ResourceRequest&);
        virtual WebCore::ResourceError blockedError(const WebCore::ResourceRequest&);
        virtual WebCore::ResourceError cannotShowURLError(const WebCore::ResourceRequest&);
        virtual WebCore::ResourceError interruptForPolicyChangeError(const WebCore::ResourceRequest&);

        virtual WebCore::ResourceError cannotShowMIMETypeError(const WebCore::ResourceResponse&);
        virtual WebCore::ResourceError fileDoesNotExistError(const WebCore::ResourceResponse&);

        virtual bool shouldFallBack(const WebCore::ResourceError&);

        virtual void setDefersLoading(bool);

        virtual bool willUseArchive(WebCore::ResourceLoader*, const WebCore::ResourceRequest&, const WebCore::KURL& originalURL) const;
        virtual bool isArchiveLoadPending(WebCore::ResourceLoader*) const;
        virtual void cancelPendingArchiveLoad(WebCore::ResourceLoader*);
        virtual void clearArchivedResources();

        virtual bool canHandleRequest(const WebCore::ResourceRequest&) const;
        virtual bool canShowMIMEType(const WebCore::String& MIMEType) const;
        virtual bool representationExistsForURLScheme(const WebCore::String& URLScheme) const;
        virtual WebCore::String generatedMIMETypeForURLScheme(const WebCore::String& URLScheme) const;

        virtual void frameLoadCompleted();
        virtual void saveViewStateToItem(WebCore::HistoryItem*);
        virtual void restoreViewState();
        virtual void provisionalLoadStarted();
        virtual void didFinishLoad();
        virtual void prepareForDataSourceReplacement();

        virtual PassRefPtr<WebCore::DocumentLoader> createDocumentLoader(const WebCore::ResourceRequest&, const WebCore::SubstituteData&);
        virtual void setTitle(const WebCore::String& title, const WebCore::KURL&);

        virtual WebCore::String userAgent(const WebCore::KURL&);
        virtual TInt widgetNetworkConstants(TInt aId);
        virtual TInt widgetNetworkState();
        
        virtual void saveDocumentViewToCachedPage(WebCore::CachedPage*);
        virtual bool canCachePage() const;
        virtual void download(WebCore::ResourceHandle*, const WebCore::ResourceRequest&, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&);

        virtual PassRefPtr<WebCore::Frame> createFrame(const WebCore::KURL& url, const WebCore::String& name, WebCore::HTMLFrameOwnerElement* ownerElement,
                                   const WebCore::String& referrer, bool allowsScrolling, int marginWidth, int marginHeight);
        virtual WebCore::Widget* createPlugin(const WebCore::IntSize&, WebCore::Element*, const WebCore::KURL&, const Vector<WebCore::String>&, const Vector<WebCore::String>&, const WebCore::String&, bool loadManually);
        virtual void redirectDataToPlugin(WebCore::Widget* pluginWidget);
        
        virtual WebCore::Widget* createJavaAppletWidget(const WebCore::IntSize&, WebCore::Element*, const WebCore::KURL& baseURL, const Vector<WebCore::String>& paramNames, const Vector<WebCore::String>& paramValues);

        virtual WebCore::ObjectContentType objectContentType(const WebCore::KURL& url, const WebCore::String& mimeType);
        virtual WebCore::String overrideMediaType() const;

        virtual void windowObjectCleared() const;
        virtual void didPerformFirstNavigation() const; // "Navigation" here means a transition from one page to another that ends up in the back/forward list.
        
        virtual void registerForIconNotification(bool listen);

        WebFrame* webFrame() { return m_webFrame; }
        void ignore(WebCore::FramePolicyFunction function);
        void download(WebCore::FramePolicyFunction function);
        void use(WebCore::FramePolicyFunction function);
        bool isWMLContent(const WebCore::String& MIMEType);
        void postProgressFinishedNotificationL();

    public:
        WebFrame* m_webFrame;  // not owned
        WebCore::ResourceResponse m_response;
        bool m_firstData;
        WebPolicyManager* m_webPolicyManager;
        CWmlDispatcher* m_WmlContentListener;
    };


#endif // WebFrameLoaderClient_h
