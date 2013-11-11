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

#include "httpfiltercommonstringsext.h"
#include "httpfiltercommonstringsaddition.h"
#include "httpfilterconnhandlerinterface.h"
#include "httpfilteriopinterface.h"
#include "httpfilterauthenticationinterface.h"
#include "cookiefilterinterface.h"
#include "uaproffilter_interface.h"
#include "deflatefilterinterface.h"
#include "HttpSessionManager.h"
#include "HttpUiCallbacks.h"
#include "httpcachemanager.h"
#include "HttpConnection.h"
#include "HttpRequestHeaderManager.h"
#include "CookieHandler.h"
#include "HttpDlConnection.h"
#include "SelfDownloadContentHandler.h"
#include "BrCtl.h"
#include "ResourceHandleManagerSymbian.h"
#include "StaticObjectsContainer.h"
#include "WebFrame.h"

// CONSTANTS
_LIT8( KHttpProtString, "HTTP/TCP" );
_LIT (KNullStr, "");

class MBrCtlSpecialLoadObserver;

using namespace WebCore;

HttpSessionManager::HttpSessionManager()
{
    m_sessionRunning = false;
    m_cookiesEnabled = false;
    m_cookieFilterLoaded = false;
    m_httpPipelining = true;
    m_cacheManager = NULL;
    m_ReqHdrManager = NULL;
    m_cookieHandler = NULL;
    m_httpDownload = NULL;
    m_SelfDownloadContentHandler = NULL;
    m_SelfDownloadContentTypes = KNullStr().Alloc();
}

HttpSessionManager::~HttpSessionManager()
{
    delete m_cacheManager;
    m_cacheManager = NULL;
    delete m_ReqHdrManager;
    m_ReqHdrManager = NULL;
    delete m_cookieHandler;
    delete m_httpDownload;
    m_httpDownload = NULL;
    delete m_SelfDownloadContentHandler;
    m_SelfDownloadContentHandler = NULL;
    delete m_SelfDownloadContentTypes;
    m_SelfDownloadContentTypes = NULL;
    m_ClientAcceptHeaders.ResetAndDestroy();
    m_ClientAcceptHeaders.Close();
    closeHttpSession();
}

void HttpSessionManager::openHttpSessionIfNeededL()
{
    if (!m_sessionRunning) {
        m_httpSession.OpenL( KHttpProtString );
        m_sessionRunning = true;
        // get the connection info
        RStringPool strP = m_httpSession.StringPool();
        const TStringTable& stringTable = RHTTPSession::GetTable();
        RHTTPConnectionInfo connInfo = m_httpSession.ConnectionInfo();

        // set shutdown
        THTTPHdrVal immediateShutdown = strP.StringF( HTTP::ESocketShutdownImmediate, stringTable );
        connInfo.SetPropertyL ( strP.StringF( HTTP::ESocketShutdownMode, stringTable ), immediateShutdown );

        // set pipelining
        RStringF maxConnection = strP.StringF( HTTP::EMaxNumTransportHandlers , stringTable );
        connInfo.SetPropertyL( maxConnection, THTTPHdrVal( KHttpMaxConnectionNum ) );

        RStringF maxToPipeline = strP.StringF(HTTP::EMaxNumTransactionsToPipeline, stringTable );
        connInfo.SetPropertyL( maxToPipeline, THTTPHdrVal( KHttpMaxTransactionNumPerConnection ) );
        
#ifdef BRDO_HTTP_STACK_93
        // Set optimal pipelining
        RStringF strOptimalPipelineVal = strP.StringF(HTTP::EHttpEnableOptimalPipelining, RHTTPSession::GetTable());
        connInfo.SetPropertyL(strP.StringF(HTTP::EHttpOptimalPipelining, RHTTPSession::GetTable()), strOptimalPipelineVal);
#endif
		//set HTTP receive Buffer Size property
		RStringF receiveBuffSize = strP.StringF(HTTP::ERecvBufferSize, stringTable );
		connInfo.SetPropertyL( receiveBuffSize, THTTPHdrVal(KHttpReceiveBuffSize));

		//set HTTP batching enable
		THTTPHdrVal batchingSupport(strP.StringF(HTTP::EEnableBatching,RHTTPSession::GetTable()));
		connInfo.SetPropertyL( strP.StringF( HTTP::EHttpBatching, RHTTPSession::GetTable() ), batchingSupport );

		//set HTTP batching Buffer Size property
		RStringF batchingBuffSize = strP.StringF(HTTP::EBatchingBufferSize, stringTable );
		connInfo.SetPropertyL( batchingBuffSize, THTTPHdrVal(KHttpBatchingBuffSize));

		//set HTTP socket prioritis to high
		THTTPHdrVal enableTranspHndlrPriority(strP.StringF(HTTP::EEnableTranspHndlrPriority, RHTTPSession::GetTable()));
		connInfo.SetPropertyL(strP.StringF(HTTP::ETranspHndlrPriority, RHTTPSession::GetTable()), enableTranspHndlrPriority);

        strP.OpenL( HttpFilterCommonStringsExt::GetTable() );
        strP.OpenL( HttpFilterCommonStringsExt::GetLanguageTable() );
        strP.OpenL( HttpFilterCommonStringsAddition::GetTable() );

        CHttpFilterAuthenticationInterface::InstallFilterL( m_httpSession, true );
        CHttpFilterAuthenticationInterface::InstallFilterL( m_httpSession, false);
        CHttpFilterConnHandlerInterface::InstallFilterL( m_httpSession, &m_uiCallbacks );
        CHttpUAProfFilterInterface::InstallFilterL( m_httpSession );
        CHttpDeflateFilter::InstallFilterL( m_httpSession );
        CHttpFilterIopInterface::InstallFilterL( m_httpSession, iopOptionHostHeader );
        // cache manager
        if (!m_cacheManager) {
            m_cacheManager = CHttpCacheManager::NewL();
        }
        // http request header manager
        if (!m_ReqHdrManager) {
            m_ReqHdrManager = HttpRequestHeaderManager::NewL(m_httpSession);
        }
        if (!m_cookieHandler) {
            m_cookieHandler = CookieHandler::init();
        }
    }
    updateFilters(true);
}

CHttpCacheManager* HttpSessionManager::cacheManager()
{ 
    if (!m_cacheManager)
        TRAP_IGNORE(m_cacheManager = CHttpCacheManager::NewL());
    return m_cacheManager; 
}

HttpDownload* HttpSessionManager::httpDownload(bool aCreate)
{
    if (!m_httpDownload && aCreate){
        m_httpDownload = new HttpDownload(this);
    }
    return m_httpDownload;
}

void HttpSessionManager::closeHttpSession()
{
    if (m_sessionRunning) {
        this->handleError(KErrCancel);

        // disconnect the Dl Mgr
        if (m_httpDownload){
            m_httpDownload->disconnect();
		}
        m_httpSession.Close();
        m_sessionRunning = false;

        delete m_ReqHdrManager;
        m_ReqHdrManager = NULL;
    }
}

void HttpSessionManager::enableCookies(int cookiesEnabled)
{
    m_cookiesEnabled = cookiesEnabled;
    updateFilters();
}

void HttpSessionManager::addRequest(HttpConnection* connection, ResourceHandle* handle)
{
    m_pendingHttpRequests.add(connection, handle);
}

void HttpSessionManager::removeRequest(HttpConnection* connection)
{
    m_pendingHttpRequests.remove(connection);
}

void HttpSessionManager::addAuthRequest(HttpConnection* connection, ResourceHandle* handle)
{
    m_pendingHttpAuthRequests.add(connection, handle);
}

void HttpSessionManager::removeAuthRequest(HttpConnection* connection)
{
    m_pendingHttpAuthRequests.remove(connection);
}

bool HttpSessionManager::findDownloadConnection (HttpDlConnection* dlConnection, int& position)
{
    // check if this connection is in the list
    HttpDlConnection* dlConnLocal = NULL;
    bool found = false;
    int index = 0;
    int size = m_pendingHttpDownloadRequests.size();
    while (index < size){
        dlConnLocal = m_pendingHttpDownloadRequests[index];
        if (dlConnection == dlConnLocal){
            position = index;
            found = true;
            break;
        }
        index++;
    }
    return found;
}

void HttpSessionManager::addDlRequest(HttpDlConnection* dlConnection)
{
    m_pendingHttpDownloadRequests.append(dlConnection);
}

void HttpSessionManager::removeDlRequest(HttpDlConnection* dlConnection)
{
    int position = 0;
    bool found = findDownloadConnection (dlConnection, position);
    if (found){
        m_pendingHttpDownloadRequests.remove(position);
    }
}

void HttpSessionManager::handleError(int error)
{
    Vector<HttpConnection *> requests;
    
    for(HashMap<HttpConnection *, ResourceHandle *>::iterator tmpit = m_pendingHttpRequests.begin();
        tmpit != m_pendingHttpRequests.end(); ++tmpit)
        {
            requests.append(tmpit->first);
        }
    
    for (int i=0; i<requests.size(); ++i)
        {
            requests[i]->handleError(error);
        }
}

HttpConnection* HttpSessionManager::firstHttpConnection()
{
    HashMap<HttpConnection *, ResourceHandle *>::const_iterator it = m_pendingHttpRequests.begin();
    return it->first;
}

void HttpSessionManager::updateFilters(bool initializing)
{
    if (!m_sessionRunning || count() > 1 || (count() == 1 && !initializing)) {
        return;
    }
    // Cookie Filter
    if (m_cookiesEnabled != m_cookieFilterLoaded) {
        if (m_cookiesEnabled) {
            TRAP_IGNORE(
                CHttpCookieFilter::InstallFilterL( m_httpSession );
                m_cookieFilterLoaded = true;
            );
        }
        else {
            RHTTPFilterCollection filterColl = m_httpSession.FilterCollection();
            filterColl.RemoveFilter(m_httpSession.StringPool().StringF(HTTP::ECookieFilter, RHTTPSession::GetTable()));
            m_cookieFilterLoaded = false;
        }
		// inform the download manager
        if(httpDownload(false)) {
            httpDownload()->enableCookies(m_cookiesEnabled);
        }
    }    
}

int HttpSessionManager::count()
{
    return m_pendingHttpRequests.size() + !httpDownload(false) ? 0 : httpDownload()->numOfDownloads();
}

HttpDlConnection* HttpSessionManager::CreateHttpDlConnection()
{
    HttpDlConnection* dlConnection = new HttpDlConnection();
    return dlConnection;
}

void HttpSessionManager::downloadL(ResourceHandle* handle, const ResourceRequest& request,
                                  const ResourceResponse& response, HttpConnection* connection)
{
    RHTTPTransaction* connTransaction = connection->takeOwnershipHttpTransaction();
    WebFrame* webFrame = kit(connection->frame());
    if (!m_SelfDownloadContentHandler) {
        m_SelfDownloadContentHandler = SelfDownloadContentHandler::NewL(
            webFrame, *m_SelfDownloadContentTypes);
    } else  { //m_SpecialLoadObserver in static object m_SelfDownloadContentHandler has to reinitialize
    	m_SelfDownloadContentHandler->ReinitializeSpecialLoadObserver(webFrame);
    }
    if(m_SelfDownloadContentHandler->IsSupported(request, response, *connTransaction)
		!= KErrNotSupported) {
		if (m_OutstandingSelfDl) {
			// only one outstanding self download is supported
			User::Leave(KErrCancel);
		}
		else {
			m_OutstandingSelfDl = true;
			m_SelfDownloadContentHandler->HandleResponseHeadersL(request, response, *connTransaction);
			RHTTPTransactionPropertySet propSet = connTransaction->PropertySet();
			RStringPool stringPool = m_httpSession.StringPool();

			// Add own adress to the transaction properties		
			propSet.RemoveProperty( stringPool.StringF(HttpFilterCommonStringsExt::ESelfDownloadCallback,
                                    HttpFilterCommonStringsExt::GetTable()) );
			propSet.SetPropertyL( stringPool.StringF(HttpFilterCommonStringsExt::ESelfDownloadCallback,
                                    HttpFilterCommonStringsExt::GetTable()), 
                                    ((TInt) (MHTTPTransactionCallback*)m_SelfDownloadContentHandler));
            if (connection->totalContentSize()) {
				m_SelfDownloadContentHandler->HandleResponseBodyL(*connTransaction);
			}
		}
    } else {
        m_httpPipelining = false;
        HttpDlConnection* dlConnection = CreateHttpDlConnection();
        httpDownload()->continueDownloadL(connTransaction, dlConnection);
        if (connection->totalContentSize()) {
            THTTPEvent ev(THTTPEvent::EGotResponseBodyData);
            m_transactionCallbacks.MHFRunL(*connTransaction, ev);
        }
    }
    // Note: connection is  cleanuped in the HttpConnection::MHFRunL() THTTPEvent::EGotResponseHeaders
}

void HttpSessionManager::download(ResourceHandle* handle, const ResourceRequest& request,
                                  const ResourceResponse& response, HttpConnection* connection)
{
		TRAPD(ret, downloadL(handle, request,response, connection));
		if (ret != KErrNone) {
           MBrCtlDialogsProvider* dialogsProvider = StaticObjectsContainer::instance()->brctl()->brCtlDialogsProvider();
           if ( dialogsProvider && (ret == KErrNoMemory)) {
              TRAP_IGNORE(dialogsProvider->DialogNotifyErrorL(ret));
		   }
		}
}

void HttpSessionManager::setSelfDownloadContentTypes(
    const TDesC& types)
{
    delete m_SelfDownloadContentTypes;
    m_SelfDownloadContentTypes = types.Alloc();
}

void HttpSessionManager::setClientAcceptHeadersL(
    const TDesC& headers)
{
    // cleanup pervious accept headers
    m_ClientAcceptHeaders.ResetAndDestroy();

    TInt endName = 0;
    TInt startValue = 0;
    TInt endValue = 0;
    TInt consumed = 0;
    TInt len = headers.Length();    
    
    HBufC8* acceptHeaders = HBufC8::NewLC(headers.Length());
    acceptHeaders->Des().Copy(headers);
    while (consumed < len)
        {
        TPtrC8 ptr(acceptHeaders->Ptr() + consumed, len - consumed);
        // find the headers separator first
        endValue = ptr.Locate('\r');
        if (endValue == 0)
            {
            // skip empty headers
            consumed++;
            continue;
            }
        if (endValue == KErrNotFound)
            {
            endValue = ptr.Length();
            }
        TPtrC8 header(ptr.Ptr(), endValue);

        endName = header.Locate(':');
        if (endName <= 0)
            {
            // No separator in the header, or it is the first character
            User::Leave(KErrArgument);
            }
        // Skip leading spaces in header value
        for (startValue = endName + 1; startValue < endValue && header[startValue] == ' '; startValue++)
            {
            }

        HBufC8* headerBuf = HBufC8::NewLC(endName);
        HBufC8* valueBuf = HBufC8::NewLC(endValue-startValue);

        headerBuf->Des().Copy(ptr.Left(endName));
        valueBuf->Des().Copy(ptr.Mid(startValue, endValue - startValue));
        
        User::LeaveIfError(m_ClientAcceptHeaders.Append(headerBuf)); 
        User::LeaveIfError(m_ClientAcceptHeaders.Append(valueBuf));
        CleanupStack::Pop(2); // headerBuf, valueBuf   
        consumed += (endValue + 1);
        }
    CleanupStack::PopAndDestroy(); // acceptHeaders
}

void HttpSessionManager::enableAutoOpenDownloads(bool autoOpen)
{
    m_autoOpenDownloads = autoOpen;
    if(httpDownload(false)) {
        httpDownload()->updateDownloadsOpenEnabled();
    }
}

void HttpSessionManager::UpdateCacheL(const String& url, const String &equiv, const String &content)
{
    CHttpCacheManager* cache = cacheManager();
    if( cache ) {
        TPtrC urlPtr( url );
        HBufC8* url8 = HBufC8::NewLC( urlPtr.Length() );
        url8->Des().Copy( urlPtr );
        if (equalIgnoringCase(equiv, "expires")) {
            TPtrC expPtr( content );
            if(expPtr.Length()) {
                HBufC8* exp8 = HBufC8::NewLC( expPtr.Length() );
                exp8->Des().Copy( expPtr );
                cache->AddHeaderL( *url8, _L8("Expires"), *exp8 );
                CleanupStack::PopAndDestroy(); // exp8
            }
        }
        else {
            String str = content.lower().stripWhiteSpace();
            if (str.contains("no-cache") || str.contains("no-store")) {
                cache->RemoveL( *url8 );
            }
            else if (equalIgnoringCase(equiv, "cache-control") && str.contains("max-age")) {
                HBufC8* name8 = HBufC8::NewLC( equiv.length() );
                name8->Des().Copy( equiv );
                HBufC8* value8 = HBufC8::NewLC( content.length() );
                value8->Des().Copy( content );
                cache->AddHeaderL( *url8, *name8, *value8 );
                CleanupStack::PopAndDestroy(2); // name8, value8
            }
        }
        CleanupStack::PopAndDestroy(); // url8
    }
}

// end of file
