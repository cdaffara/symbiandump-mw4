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
* Description:   Implementation of WebPolicyManager
*
*/

#include "config.h"
#include <../bidi.h>
#include "WebPolicyManager.h"
#include "WebFrameLoaderClient.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "BrCtl.h"
#include "WebUtil.h"
#include "StaticObjectsContainer.h"
#include "PlugInInfoStore.h"
#include "MimeTypeRegistry.h"
#include <uri8.h>
#include <badesca.h>

using namespace WebCore;

// CONSTANTS
const char* typeTextHtml = "text/html";
const char* typeTextXml = "text/xml";
const char* typeApplicationXhtml = "application/xhtml+xml";
const char* typeTextPlain = "text/plain";
const char* typeApplicationWapXhtml = "application/vnd.wap.xhtml+xml";
const char* typeMultipartMixed = "multipart/mixed";
const char* typeApplicationOctetStream = "application/octet-stream";
const char* typeImageSlash = "image/";
const char* typeSvg = "svg";
_LIT(KPathBegin,"<!--framePathBegin ");
_LIT(KPathEnd," framePathEnd --!>");

WebPolicyManager::WebPolicyManager(WebFrameLoaderClient* webFrameLoaderClient) :
m_webFrameLoaderClient(webFrameLoaderClient)
{
    m_newWindowUserGesture = false;
    m_newWindowTargetName = NULL;
}

WebPolicyManager::~WebPolicyManager()
{
    delete m_newWindowTargetName;
}

void WebPolicyManager::dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction function,
                                                       const WebCore::String& MIMEType,
                                                       const WebCore::ResourceRequest& /*request*/)
{
    if ( m_webFrameLoaderClient->isWMLContent(MIMEType) ) {
        // check for wml limitations
        const Vector<CBrCtl*>& ctrls = StaticObjectsContainer::instance()->activeBrowserControls();
        for (int i=0; i<ctrls.size(); ++i) {
            // check non-current BrCtls to see if wml engine is loaded
            // if it is then we need to ignore this url since only one BrCtl can load the wml engine
            if (ctrls[i]->wmlEngineInterface() &&
                ctrls[i] != control(m_webFrameLoaderClient->webFrame())) {
                m_webFrameLoaderClient->ignore(function);
                return;
            }
        }
        m_webFrameLoaderClient->use(function);
    }
    else if ( canShowMIMEType(MIMEType) || 
              ((m_webFrameLoaderClient->webFrame()->parentFrame()) && (PlugInInfoStore::supportsMIMEType(MIMEType)) && 
              !(MIMEType.startsWith("audio/", false)) && !(MIMEType.startsWith("video/", false)))) {
        m_webFrameLoaderClient->use(function);
    }
    else {
        m_webFrameLoaderClient->download(function);
    }
}

void WebPolicyManager::dispatchDecidePolicyForNewWindowAction(WebCore::FramePolicyFunction function, const WebCore::NavigationAction& action, const WebCore::ResourceRequest& request, const WebCore::String& frameName)
{
    switch (action.type())
    {
        case NavigationTypeLinkClicked:
        case NavigationTypeFormSubmitted:
        case NavigationTypeBackForward:
        case NavigationTypeReload:
        case NavigationTypeFormResubmitted:
            m_newWindowUserGesture = true;
            break;
        case NavigationTypeOther:
        default:
            m_newWindowUserGesture = false;
            break;
    }
    delete m_newWindowTargetName;
    m_newWindowTargetName = NULL;
    if (frameName.des().Length()) {
        if (frameName == "_blank" || frameName == "_new") {
            m_newWindowTargetName = generateFrameName();
        }
        else { // frame has a name
            m_newWindowTargetName = frameName.des().Alloc();
        }
    }
    if (m_newWindowTargetName) {
            m_webFrameLoaderClient->use(function);
    }
    else {
        m_webFrameLoaderClient->ignore(function);
    }
}

void WebPolicyManager::dispatchDecidePolicyForNavigationAction(WebCore::FramePolicyFunction function, const WebCore::NavigationAction&, const WebCore::ResourceRequest& request)
{
    // ResourceLoadDelegate does all the checks now.
    m_webFrameLoaderClient->use(function);
}

void WebPolicyManager::cancelPolicyCheck()
{
}

void WebPolicyManager::dispatchUnableToImplementPolicy(const WebCore::ResourceError&)
{
}

void WebPolicyManager::dispatchWillSubmitForm(WebCore::FramePolicyFunction function, PassRefPtr<WebCore::FormState>)
{
    m_webFrameLoaderClient->use(function);
}

WebCore::ResourceError WebPolicyManager::interruptForPolicyChangeError(const WebCore::ResourceRequest& request)
{
    return ResourceError(); 
}

bool WebPolicyManager::canShowMIMEType(const String& MIMEType) const 
{
    bool found = false;
    
    //RFC 2183: "Content-Disposition: attachment" means downloaded by DM
    FrameLoader* frameLoader = m_webFrameLoaderClient->webFrame()->frameLoader();
    if(frameLoader && frameLoader->activeDocumentLoader()) {
    	ResourceResponse r = frameLoader->activeDocumentLoader()->response();
    	if (r.isAttachment())
    		return found;
    }

    if (MIMEType == typeTextHtml ||
        MIMEType == typeApplicationXhtml ||
        MIMEType == typeApplicationWapXhtml ||
        MIMEType == typeMultipartMixed || 
        MIMEType == typeTextXml) {
        found = true;
    }
    //Check if the image type can be handled by the browser. If not
    //forward download manager downloads the content
    else if (MIMETypeRegistry::isSupportedImageMIMEType(MIMEType)){
        found = true;
    }

    // special case for application/octet-stream. some web servers
    // are miscofigured and send application/octet-stream even for html
    // page so we need to pass it to webcore as if it was text/html. however
    // some binary content also comes as application/octet-stream (such as
    // sisx files ). so here we che(ha)ck if the response url has .html .htm .asp
    // extension. though luck for the rest. feel free to extend the list.
    // some text/plain is actually not (example: cacert.org DER root certificate)
    else if( MIMEType == typeTextPlain ||
             MIMEType == typeApplicationOctetStream ) {
        TPtrC8 url = (core(m_webFrameLoaderClient->webFrame()))->loader()->activeDocumentLoader()->responseURL().des();
        TUriParser8 parser;
        if( parser.Parse(url) == KErrNone ) {
            TPtrC8 path = parser.Extract( EUriPath );
            // path == 1 means only / (no filename)
            if( path.Length() > 1 ) {
                found = (path.Find(_L8(".html")) != KErrNotFound ||
                         path.Find(_L8(".htm"))  != KErrNotFound ||
                         path.Find(_L8(".asp"))  != KErrNotFound ||
                         path.Find(_L8(".php"))  != KErrNotFound ||
                         path.Find(_L8(".jsp"))  != KErrNotFound ||
                         path.Find(_L8(".txt"))  != KErrNotFound);
            }
        }
    }
    // tot:fixme defaultcontenthandler is only for selfdownloadable, go through the list
    return found;
}


