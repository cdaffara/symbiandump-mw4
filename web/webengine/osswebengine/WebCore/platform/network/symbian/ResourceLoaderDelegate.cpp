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
* Description:  
*
*/

#include "config.h"
#include <../bidi.h>
#include <uri8.h>
#include <badesca.h>

#include "ResourceLoaderDelegate.h"
#include "UrlConnection.h"
#include "DataConnection.h"
#include "FileConnection.h"
#include "HttpConnection.h"
#include "ResolvedConnection.h"

#include "ResourceHandleInternal.h"
#include "DocumentLoader.h"
#include "ResourceHandle.h"
#include "KURL.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "Frame.h"

#include "BrCtl.h"
#include "Page.h"
#include "WebUtil.h"
#include "WebFrame.h"
#include "SettingsContainer.h"
#include "BrCtlSpecialLoadObserver.h"
#include "BrCtlLinkResolver.h"
#include "WebDocumentLoader.h"
#include "WebView.h"

_LIT8(KHttp, "http");
_LIT8(KHttps, "https");

using namespace WebCore;

MUrlConnection* ResourceLoaderDelegate::CreateUrlConnection(ResourceHandle* handle, Frame* frame)
{
    MUrlConnection* connection = NULL;
    TInt err = KErrNone;
    ResourceRequest r = handle->request();
    TRAP(err, connection = checkLinkResolverL(handle, frame));
    if (err == KErrNone && !connection) {
        if (checkSecurityStatus(handle->request(), frame)) {
            TPtrC8 url(r.url().des());
            TUriParser8 parser;
            TInt status = parser.Parse( url );
            TInt ret = EFalse;
            if( status == KErrNone ) {
                TPtrC8 scheme = parser.Extract( EUriScheme );
                if (scheme.CompareF( KHttp ) == 0 || scheme.CompareF( KHttps ) == 0) {
                    connection = new HttpConnection(handle, frame);
                }
                else if (scheme.Length() == 1 || scheme.CompareF( _L8("file") ) == 0) {
                    connection = new FileConnection(handle);
                }
                else if (scheme.CompareF( _L8("data") ) == 0) {
                    connection = new DataConnection(handle);
                }
                else {
                    TRAP(err, handleSpecialSchemeL(r.url().des(), frame));
                    if (err == KErrNotFound) {
                        err = KErrHttpInvalidUri;
                    }
                }
            }
        }
    }
    if (r.mainLoad() && frame == frame->page()->mainFrame() && err != KErrNone && err != KErrCancel) {
		m_httpSessionManager.uiCallback()->reportError(mapHttpErrors(err));
    }
    CBrCtl* brctl = control(frame);
    m_httpSessionManager.uiCallback()->SetBrowserControl(brctl);
    return connection;
}

void ResourceLoaderDelegate::download(ResourceHandle* handle, const ResourceRequest& request,
                                      const WebCore::ResourceResponse& response)
{
    MUrlConnection* connection = handle->getInternal()->m_connection;
    connection->download(handle, request, response);
}

static void cleanupConnection( TAny* connection )
{
    delete (ResolvedConnection*) connection;
}

MUrlConnection* ResourceLoaderDelegate::checkLinkResolverL(ResourceHandle* handle, Frame* frame)
{
    CBrCtl* brctl = control(frame);
    ResolvedConnection* connection = NULL;
    int cnt = 0;
    if (brctl->brCtlLinkResolver()) {
        if (handle->request().mainLoad() && (brctl->capabilities() & TBrCtlDefs::ECapabilityClientNotifyURL)) {
            HBufC* url = HBufC::NewLC(handle->request().url().des().Length());
            cnt++;
            url->Des().Copy(handle->request().url().des());
            HBufC* currentUrl = NULL;
            TPtrC currentUrlPtr(KNullDesC);
            if (frame->loader() && frame->loader()->documentLoader()) {
                TPtrC8 currentUrl8 = frame->loader()->documentLoader()->URL().des();
                currentUrl = HBufC::NewLC(currentUrl8.Length());
                cnt++;
                if (currentUrl) {
                    currentUrl->Des().Copy(currentUrl8);
                    currentUrlPtr.Set(currentUrl->Des());
                }
            }            
            connection = new (ELeave) ResolvedConnection(handle, frame);
            TCleanupItem cleaner( cleanupConnection, connection );
            CleanupStack::PushL(cleaner);
            TBool ret = brctl->brCtlLinkResolver()->ResolveLinkL(url->Des(), currentUrlPtr, *connection);
            CleanupStack::Pop(); // cleaner( connection )
            if (!ret) {
                delete connection;
                connection = NULL;
            }
        }
        else if (!handle->request().mainLoad() && (brctl->capabilities() & TBrCtlDefs::ECapabilityClientResolveEmbeddedURL)) {
            HBufC* url = HBufC::NewLC(handle->request().url().des().Length());
            cnt++;
            url->Des().Copy(handle->request().url().des());
            HBufC* currentUrl = NULL;
            TPtrC currentUrlPtr(KNullDesC);
            if (frame->loader() && frame->loader()->documentLoader()) {
                TPtrC8 currentUrl8 = frame->loader()->documentLoader()->URL().des();
                currentUrl = HBufC::NewLC(currentUrl8.Length());
                cnt++;
                if (currentUrl) {
                    currentUrl->Des().Copy(currentUrl8);
                    currentUrlPtr.Set(currentUrl->Des());
                }
            }            
            connection = new (ELeave) ResolvedConnection(handle, frame);
            TCleanupItem cleaner( cleanupConnection, connection );
            CleanupStack::PushL(cleaner);
            TBool ret = brctl->brCtlLinkResolver()->ResolveEmbeddedLinkL(url->Des(), currentUrlPtr,
                ELoadContentTypeAny, *connection);
            CleanupStack::Pop(); // cleaner( connection )
            if (!ret) {
                delete connection;
                connection = NULL;
            }
        }
        CleanupStack::PopAndDestroy(cnt); // url, currentUrl
    }
    return connection;
}

bool ResourceLoaderDelegate::checkSecurityStatus(const ResourceRequest& request, Frame* frame)
{
    CBrCtl* brctl = control(frame);
    WebDocumentLoader* topDocumentLoader = static_cast<WebDocumentLoader*>(core(brctl->webView()->mainFrame())->loader()->documentLoader());
    HttpUiCallbacks::TEnterStatus enterStatus = HttpUiCallbacks::EEnterStatusNone;
    bool ret = true;
    
    TUriParser8 secureUrlParser;
    TUriParser8 currentSecureUrlParser;
    bool secureUrl = false;
    bool currentSecureUrl = false;
    if (secureUrlParser.Parse(request.url().des()) == KErrNone) {
        TPtrC8 scheme = secureUrlParser.Extract( EUriScheme );
        secureUrl = scheme.CompareF(KHttps) == 0;
    }
    if (frame->loader() && frame->loader()->documentLoader()) {
        TPtrC8 url = frame->loader()->documentLoader()->URL().des();
        if (currentSecureUrlParser.Parse(url) == KErrNone) {
            TPtrC8 scheme = currentSecureUrlParser.Extract( EUriScheme );
            currentSecureUrl = scheme.CompareF(KHttps) == 0;
        }
    }
    if (request.mainLoad()) {
        if (!frame->ownerElement()) {
            if (currentSecureUrl) {
                if (!secureUrl) {
                    // secure -> non secure
                    if (request.httpMethod() == "POST") {
                        enterStatus = HttpUiCallbacks::ESubmittingToNonSecurePage;
                    }
                    else {
                        enterStatus = HttpUiCallbacks::EExitingSecurePage;
                    }
                } // if (!secureUrl)
                else {
					// secure -> secure
                    if( secureUrlParser.Extract(EUriHost).Compare(
                        currentSecureUrlParser.Extract(EUriHost)) ) {
                        enterStatus = HttpUiCallbacks::EEnteringSecurePage;
                    }
                    else {
                        enterStatus = HttpUiCallbacks::EReEnteringSecurePage;
                    }
                }
            } // if (currentSecureUrl)
            else {
                if (secureUrl) {
                    // non secure -> secure
                    enterStatus = HttpUiCallbacks::EEnteringSecurePage;
                }
            }
        } // if (!frame->ownerElement())
    }
    else
    {
        if (currentSecureUrl) {
            if (!secureUrl) {
                // Ask once per page. If we already asked, just use the previous user decision
                if (topDocumentLoader->userWasAskedToLoadNonSecureItem()) {
                    ret = topDocumentLoader->userAgreedToLoadNonSecureItem();
                }
                else {
                    enterStatus = HttpUiCallbacks::ESomeItemsNotSecure;
                }
            }
        }
        else {
            if (secureUrl) {
                enterStatus = HttpUiCallbacks::ESecureItemInNonSecurePage;
            }
        }  
    }
    if (enterStatus != HttpUiCallbacks::EEnterStatusNone) {
        int err = m_httpSessionManager.uiCallback()->aboutToLoadPage(brctl, enterStatus);
        if (err != KErrNone) {
            ret = false;
        }
        if (enterStatus == HttpUiCallbacks::ESomeItemsNotSecure) {
            // Remember user decision
            topDocumentLoader->setUserAgreedToLoadNonSecureItem(ret);
        }
    }
    return ret;
}

bool ResourceLoaderDelegate::handleSpecialSchemeL(TPtrC8 url8, Frame* frame)
{
    // only the url is supported currently
    RArray<TUint> typeArray;
    CDesCArrayFlat* strArray = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( strArray );
    // add url id
    typeArray.Append( EParamRequestUrl );
    HBufC* url = NULL;
    url = HBufC::NewLC(url8.Length());
    TPtr urlPtr(url->Des());
    urlPtr.Copy(url8);
    // add url string
    strArray->AppendL( urlPtr );
    TBool handle( EFalse );
    CBrCtl* brctl = control(frame);
    if (brctl->brCtlSpecialLoadObserver()) {
        brctl->brCtlSpecialLoadObserver()->HandleRequestL( &typeArray, strArray );
    }
    CleanupStack::PopAndDestroy(2); // strArray, url
    return handle;
}

// end of file
