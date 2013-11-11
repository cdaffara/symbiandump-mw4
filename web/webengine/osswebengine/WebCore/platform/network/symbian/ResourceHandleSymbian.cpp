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
* Description:  ResourceHandle
*
*/

#include "config.h"

#include "Frame.h"
#include "DocLoader.h"
#include "ResourceHandle.h"
#include "DeprecatedString.h"
#include "ResourceHandleManagerSymbian.h"
#include "ResourceHandleInternal.h"
#include "WebCoreFrameBridge.h"
#include "UrlConnection.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"

#include "SynchRequest.h"

#include "BrCtl.h"
#include "FrameTree.h"
#include "Page.h"
#include "WebFrame.h"
#include "WebFrameBridge.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "kjs_window.h"

#define notImplemented() {}

namespace WebCore {


using namespace KJS;
using namespace std;

class EventDeferrer : Noncopyable {
public:
    EventDeferrer(Frame*, bool deferSelf, bool pauseTimeouts);
    ~EventDeferrer();
private:
    Vector<RefPtr<Frame>, 16> m_deferredFrames;
    Vector<pair<RefPtr<Frame>, PausedTimeouts*>, 16> m_pausedTimeouts;
};


class AsyncCancel : public MUrlConnection, public CAsyncOneShot
{
public:
    AsyncCancel(ResourceHandle* _handle) : MUrlConnection(_handle), CAsyncOneShot(CActive::EPriorityHigh + 1)  {}
    
    virtual ~AsyncCancel() {}
    virtual int submit() {Call(); return KErrNone;}
    virtual void cancel() { CActive::Cancel(); }
    virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response) {}
    virtual int totalContentSize() { return 0; }
    void response() {}
    void complete(int error) {}
private:
    void RunL() 
    {
        CResourceHandleManager::self()->receivedFinished(m_handle, KErrCancel, this);
        derefHandle();
    }
};


ResourceHandleInternal::~ResourceHandleInternal()
{
}

ResourceHandle::~ResourceHandle()
{
    cancel();
    delete d->m_connection;
}

bool ResourceHandle::start(Frame* frame)
{
    ASSERT(frame);

    if (!frame)
        return false;

    // If we are no longer attached to a Page, this must be an attempted load from an
    // onUnload handler, so let's just block it.
    if (!frame->page())
        return false;

    //  
    d->m_connection = StaticObjectsContainer::instance()->resourceLoaderDelegate()->CreateUrlConnection(this, frame);
    int error = CResourceHandleManager::self()->add(this);
    if (d->m_connection == NULL || error != KErrNone) {
        // create fake connection to issue async cancel
        delete d->m_connection;
        d->m_connection = 0;
        d->m_connection = new AsyncCancel(this);
        if (d->m_connection){
            d->m_connection->submit();
            HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
            // do not report errors on resource loads
            if (httpSessionMgr && httpSessionMgr->uiCallback() && request().mainLoad() && error != KErrNone) {
                httpSessionMgr->uiCallback()->reportError(error);
            }
        }
    }
    return (d->m_connection != NULL);
}

void ResourceHandle::cancel()
{
    CResourceHandleManager::self()->cancel(this);
}

bool ResourceHandle::loadsBlocked()
{
    // tot:fixme loads should be blocked when a modal dialog is open
    notImplemented();
    return false;
}

bool ResourceHandle::willLoadFromCache(ResourceRequest& request)
{
    // tot:fixme should set a flag is the content is coming from cache, so that we don't load favicons from the network
    notImplemented();
    return false;
}

bool ResourceHandle::supportsBufferedData()
{
    notImplemented();
    return false;
}

PassRefPtr<SharedBuffer> ResourceHandle::bufferedData()
{
    notImplemented();
    return 0;
}

void ResourceHandle::loadResourceSynchronously(const ResourceRequest& request, 
                                               ResourceError& e, 
                                               ResourceResponse& r, 
                                               Vector<char>& data, 
                                               Frame* frame)
{
    ASSERT(frame);
    // Call control() in WebFrame to get current webView
    control(frame)->webView()->synchRequestPending(true);

    EventDeferrer deferrer(frame, false, true); // Need to suspend timer events
    
    CSynchRequest* synchXHR = CSynchRequest::NewL();
    CleanupStack::PushL(synchXHR);
    synchXHR->CreateResource(request, e, r, data, frame);
    CleanupStack::PopAndDestroy(synchXHR);
    
    if (frame) { // Frame may be gone by now? (in case of user's navigation)
        control(frame)->webView()->synchRequestPending(false);
    }
}
 
void ResourceHandle::setDefersLoading(bool defers)
{
    d->m_defersLoading = defers;
    d->m_connection->setDefersLoading(defers);

}

EventDeferrer::EventDeferrer(Frame* thisFrame, bool deferSelf, bool pauseTimeouts)
{
    const HashSet<Page*>* group = thisFrame->page()->frameNamespace();

    if (!group)
        return;

    HashSet<Page*>::const_iterator end = group->end();
    for (HashSet<Page*>::const_iterator it = group->begin(); it != end; ++it) {
        Page* otherPage = *it;
        if ((deferSelf || pauseTimeouts)) {
            if (!otherPage->defersLoading() && (deferSelf || otherPage != thisFrame->page())) {
                m_deferredFrames.append(otherPage->mainFrame());
            }

            if (pauseTimeouts) {
                for (Frame* frame = otherPage->mainFrame(); frame; frame = frame->tree()->traverseNext()) {
                    if (Window* window = Window::retrieveWindow(frame)) {
                        PausedTimeouts* timeouts = window->pauseTimeouts();
                        m_pausedTimeouts.append(make_pair(frame, timeouts));
                    }
                }
            }
        }
    }

    size_t count = m_deferredFrames.size();
    for (size_t i = 0; i < count; ++i) {
        if (Page* page = m_deferredFrames[i]->page()) {
            page->setDefersLoading(true);
        }
    }
}

EventDeferrer::~EventDeferrer()
{
    size_t count = m_deferredFrames.size();
    for (size_t i = 0; i < count; ++i) {
        if (Page* page = m_deferredFrames[i]->page()) {
            page->setDefersLoading(false);
        }
    }

    count = m_pausedTimeouts.size();

    for (size_t i = 0; i < count; i++) {
        Window* window = Window::retrieveWindow(m_pausedTimeouts[i].first.get());
        if (window) {
            window->resumeTimeouts(m_pausedTimeouts[i].second);
        }
        delete m_pausedTimeouts[i].second;
    }
}

} // namespace WebCore
