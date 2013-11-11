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

#ifndef __HTTPSESSIONMANAGER_H__
#define __HTTPSESSIONMANAGER_H__

#include <wtf/HashMap.h>
#include "HttpUiCallbacks.h"
#include "HttpCallbacks.h"
#include "HttpDownload.h"
#include <../bidi.h>
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include <es_sock.h> // RConnection

namespace WebCore {
    class ResourceHandle;
    struct ResourceRequest;
    class ResourceResponse;
};
class HttpCallbacks;
class HttpConnection;
class CHttpCacheManager;
class HttpRequestHeaderManager;
class CookieHandler;
class HttpDlConnection;
class RHTTPTransaction;
class SelfDownloadContentHandler;

const int KHttpMaxConnectionNum = 7;
const int KHttpMaxTransactionNumPerConnection = 2;
const int KHttpBatchingBuffSize = 2*1400;
const int KHttpReceiveBuffSize = 32*1024;

class HttpSessionManager
{
friend class HttpDownload;
public:
    HttpSessionManager();
    ~HttpSessionManager();
    void openHttpSessionIfNeededL();
    void closeHttpSession();
    bool isSessionRunning() const { return m_sessionRunning; }
    RHTTPSession& httpSession() { return m_httpSession; }
    CookieHandler* cookieHandler() const { return m_cookieHandler; }
    void enableCookies(int cookiesEnabled);
    int cookiesEnabled() const { return m_cookiesEnabled; }
    void enableImei(bool imeiEnabled) { m_imeiEnabled = imeiEnabled; }
    bool imeiEnabled() const { return m_imeiEnabled; }
    void setEmbedded(int embedded) { m_embedded = embedded; }
    int isEmbedded() const { return m_embedded; }
    void enableReferer(bool referer) { m_sendReferer = referer; }
    bool refererEnabled() const { return m_sendReferer; }
    void enableSecurityDialogs(bool showDialogs) {m_securityWarningsEnabled = showDialogs; }
    bool securityDialogsEnabled() const { return m_securityWarningsEnabled; }
    void enableAutoOpenDownloads(bool autoOpen);
    bool autoOpenDownloads() { return m_autoOpenDownloads; }
    void setAp(unsigned int ap) { m_ap = ap; }
    unsigned int ap() const { return m_ap; }
    void setCharset(unsigned int charset) { m_charset = charset; }
    unsigned int charset() const { return m_charset; }
    void addRequest(HttpConnection* connection, WebCore::ResourceHandle* handle);
    void removeRequest(HttpConnection* connection);
    void addDlRequest(HttpDlConnection* dlConnection);
    bool findDownloadConnection (HttpDlConnection* dlConnection, int& position);
    void removeDlRequest(HttpDlConnection* dlConnection); 
    void addAuthRequest(HttpConnection* connection, WebCore::ResourceHandle* handle); 
    void removeAuthRequest(HttpConnection* connection); 
    HttpUiCallbacks* uiCallback() { return &m_uiCallbacks; }
    MHTTPTransactionCallback* transactionCallback() { return &m_transactionCallbacks; }
    void handleError(int error);
    CHttpCacheManager* cacheManager();
    HttpDownload* httpDownload(bool aCreate=true);
    HttpConnection* firstHttpConnection();
	void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response, HttpConnection* connection);
    HttpRequestHeaderManager* requestHeaderManager() { return m_ReqHdrManager; }
	void setSelfDownloadContentTypes(const TDesC& types);
	void setClientAcceptHeadersL(const TDesC& headers);
	bool httpPipelining() {return m_httpPipelining;}
    void UpdateCacheL( const WebCore::String& url, const WebCore::String &equiv, const WebCore::String &content );
	void ResetOutstandingSelfDl() {m_OutstandingSelfDl = false; }
    const RPointerArray<HBufC8>& ClientAcceptHeaders() const { return m_ClientAcceptHeaders; }
    
private:
    void updateFilters(bool initializing = false);
    int count();
    HttpDlConnection* CreateHttpDlConnection();
	void downloadL(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response, HttpConnection* connection);

private:
    RHTTPSession m_httpSession;
    bool m_sessionRunning;
    int m_cookiesEnabled;
    bool m_cookieFilterLoaded;
    bool m_imeiEnabled;
    int m_embedded;
    bool m_sendReferer;
    bool m_securityWarningsEnabled;
    bool m_autoOpenDownloads;
	bool m_httpPipelining;
    unsigned int m_ap;
    unsigned int m_charset;
    HashMap<HttpConnection *, WebCore::ResourceHandle *> m_pendingHttpRequests;
    HashMap<HttpConnection *, WebCore::ResourceHandle *> m_pendingHttpAuthRequests;
    HttpUiCallbacks m_uiCallbacks;
    HttpCallbacks m_transactionCallbacks;
    CHttpCacheManager* m_cacheManager;
    HttpRequestHeaderManager* m_ReqHdrManager;
    CookieHandler* m_cookieHandler;
    Vector<HttpDlConnection*> m_pendingHttpDownloadRequests;
    HttpDownload* m_httpDownload;
    SelfDownloadContentHandler* m_SelfDownloadContentHandler;
    HBufC* m_SelfDownloadContentTypes;
	bool m_OutstandingSelfDl;
	RPointerArray<HBufC8> m_ClientAcceptHeaders;
};
#endif // __HTTPSESSIONMANAGER_H__
// end of file
