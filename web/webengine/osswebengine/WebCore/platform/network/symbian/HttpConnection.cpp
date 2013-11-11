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

#include <uri8.h>
#include <escapeutils.h>
#include <http/rhttpheaders.h>
#include <http/mhttpdatasupplier.h>
#include <thttpfields.h>
#include "ResourceHandle.h"
#include "ResourceHandleInternal.h"
#include "ResourceRequest.h"
#include "HttpConnection.h"
#include "ResourceHandleManagerSymbian.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "HttpCacheSupply.h"
#include "HttpPostDataSupplier.h"
#include <httpfiltercommonstringsext.h>
#include <BrCtlDefs.h>
#include "BrCtl.h"
#include "BrCtlSpecialLoadObserver.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "HttpUiCallbacks.h"
#include "HttpRequestHeaderManager.h"
#include "HttpConnUtils.h"
#include "MultipartContentHandler.h"
#include "UnknownContentHandler.h"
#include <wtf/Vector.h>

_LIT8(KHttps, "https");
_LIT8( KAppUid, "Appuid" );

using namespace WebCore;

DefersData::DefersData(void* ctx, DefersDataCallback callback) : CActive(CActive::EPriorityStandard)
{
    m_ctx = ctx;
    m_callback = callback;
    CActiveScheduler::Add(this);
}

DefersData::~DefersData()
{
    Cancel();
    delete m_response;

    Vector<HBufC8*>::const_iterator it = m_bodyParts.begin();
    Vector<HBufC8*>::const_iterator end = m_bodyParts.end();
    while (it != end) {
        HBufC8* buf = m_bodyParts.first();
        m_bodyParts.remove(0);
        delete buf;
        it = m_bodyParts.begin();
        end = m_bodyParts.end();
    }
}

void DefersData::RunL()
{
    m_callback(m_ctx);
}

TInt DefersData::RunError(TInt aError)
{
    return KErrNone;
}

void DefersData::Activate()
{
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
}


ReceivedFinished::ReceivedFinished(void* ctx, ReceivedFinishedCallback callback) : CActive(CActive::EPriorityStandard)
{
    m_ctx = ctx;
    m_callback = callback;
    m_done = true;
    m_error = KErrNone;
    CActiveScheduler::Add(this);
}

ReceivedFinished::~ReceivedFinished()
{
    Cancel();
}

void ReceivedFinished::RunL()
{
    m_done = true;    
    /*
     * this callback will call deRef on HttpConnection which
     * will delete ReceivedFinished object since it's a 
     * member of HttpConnection. Therefore this call has to 
     * the last one.
     */
    m_callback(m_ctx, m_error);
}

TInt ReceivedFinished::RunError(TInt aError)
{
    return KErrNone;
}

void ReceivedFinished::Activate(TInt errorCode)
{
    m_done = false;
    m_error = errorCode;
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
}

void ReceivedFinished::done(bool status)
{
    m_done = status;
}

HttpConnection::HttpConnection(ResourceHandle* _handle, Frame* _frame) : MUrlConnection(_handle)
{
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    httpSessionMgr->addRequest(this, m_handle);
    m_frag = NULL;
    m_transaction = NULL;
    m_urlResponse = NULL;
    m_contentType = NULL;
    m_encoding = NULL;
    m_cacheSupply = NULL;
    m_postDataSupplier = NULL;
    m_maxSize = 0;
    m_frame = _frame;
    m_IsMultipart = false;
    m_isDone = false;
    m_certInfo = NULL;
    m_accumulatedSize = 0;
    m_defersData = NULL; // requests will not be propagated if loading is blocked
    m_receivedFinished = NULL;
    m_unknownContentHandler = NULL;
}

HttpConnection::~HttpConnection()
{
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    if (m_transaction) {
        RHTTPSession& session = httpSessionMgr->httpSession();
        // remove own address from transaction properties
        m_transaction->PropertySet().RemoveProperty( session.StringPool().StringF(HttpFilterCommonStringsExt::ESelfPtr,
            HttpFilterCommonStringsExt::GetTable()));
    }
    httpSessionMgr->removeRequest(this);
    delete m_frag;
    delete m_urlResponse;
    delete m_contentType;
    delete m_encoding;
    delete m_defersData;
    delete m_receivedFinished;
    delete m_unknownContentHandler;
    delete m_cacheSupply;
    delete m_postDataSupplier;
    delete m_transaction;
}

HttpConnection* HttpConnection::connectionFromTransaction(RHTTPTransaction& transaction)
{
    RHTTPTransactionPropertySet propSet = transaction.PropertySet();
    THTTPHdrVal propRetVal;
    // Get the name of the property
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    RHTTPSession& session = httpSessionMgr->httpSession();
    RStringF propName = session.StringPool().StringF( HttpFilterCommonStringsExt::ESelfPtr, HttpFilterCommonStringsExt::GetTable() );
    // if it is set . .
    if( propSet.Property( propName, propRetVal ) ){
        return (HttpConnection*)(TInt(propRetVal));
    }
    return NULL;
}

int HttpConnection::submit()
{
    TRAPD(error, submitL());
    return error;
}

void HttpConnection::submitL()
{
    __ASSERT_DEBUG( !m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    User::LeaveIfNull(httpSessionMgr);
    httpSessionMgr->openHttpSessionIfNeededL();
    TPtrC8 urlPtr( m_handle->request().url().des() );
    m_transaction = new (ELeave) RHTTPTransaction;
    RHTTPSession& session = httpSessionMgr->httpSession();
    RStringPool stringPool = session.StringPool();
    const TStringTable& stringTable = RHTTPSession::GetTable();
    TUriParser8 uriParser;
    uriParser.Parse(urlPtr);
    // fragment
    if(uriParser.IsPresent(EUriFragment)) {
        m_frag = uriParser.Extract(EUriFragment).AllocL();
        // url without frag
        uriParser.UriWithoutFragment(urlPtr);
        // and reparse url
        uriParser.Parse(urlPtr);
        }

    // open transaction
    RStringF method;
    if (m_handle->request().httpMethod() == "GET") {
        method = stringPool.StringF( HTTP::EGET, stringTable );
    }
    else if (m_handle->request().httpMethod() == "POST") {
        method = stringPool.StringF( HTTP::EPOST, stringTable );
    }
    else if (m_handle->request().httpMethod() == "PUT") {
        method = stringPool.StringF( HTTP::EPUT, stringTable );
    }
    else if (m_handle->request().httpMethod() == "DELETE") {
        method = stringPool.StringF( HTTP::EDELETE, stringTable );
    }
    else {
        User::Leave(KErrArgument);
    }
    *m_transaction = session.OpenTransactionL( uriParser, *(httpSessionMgr->transactionCallback()), method );
    // add transaction attributes such as default headers, cache properties
    addRequestHeadersL();
    RHTTPTransactionPropertySet propSet = m_transaction->PropertySet();
    
    RStringF appuid = stringPool.OpenFStringL( KAppUid );
    
    TUint appuidValue = control(m_frame)->webView()->getWidgetId();
    if(appuidValue){
        propSet.SetPropertyL(appuid,THTTPHdrVal(appuidValue));
    }
    
    appuid.Close();
    // Add IMEI Notify property to the transaction
    // It is not needed, as it is done in UA-Prof filter
    // addIMEINotifyPropertiesL();

    // add http request headers
    RHTTPHeaders httpHeaders = m_transaction->Request().GetHeaderCollection();
    httpSessionMgr->requestHeaderManager()->AddAllHeadersL(httpHeaders, m_handle->request());
    // Create a dataSupplier object to supply the request body to http stack.
    if ((m_handle->request().httpMethod() == "POST" ||
        m_handle->request().httpMethod() == "PUT") && m_handle->request().httpBody()){
        // 2 steps constructor
        m_postDataSupplier = new(ELeave)HttpPostDataSupplier( m_transaction, control(m_frame));
        m_postDataSupplier->initL(m_handle->request().httpBody());
        m_transaction->Request().SetBody( *m_postDataSupplier);
    }
    // create cache supplier
    m_cacheSupply = CHttpCacheSupply::NewL( this );
    TBrCtlDefs::TBrCtlCacheMode cacheMode;
    switch (m_handle->request().cachePolicy())
    {
    default:
    case UseProtocolCachePolicy:
        cacheMode = TBrCtlDefs::ECacheModeNormal;
        break;
    case ReloadIgnoringCacheData:
        cacheMode = TBrCtlDefs::ECacheModeNoCache;
        break;
    case ReturnCacheDataElseLoad:
        cacheMode = TBrCtlDefs::ECacheModeHistory;
        break;
    case ReturnCacheDataDontLoad:
        cacheMode = TBrCtlDefs::ECacheModeOnlyCache;
        break;
    };
	IsUrlInCacheL(m_transaction->Request().URI().UriDes() );
    if( m_cacheSupply->StartRequestL( cacheMode ) != KErrNone ) {
        // check if the response must come from the cache
        if( cacheMode != TBrCtlDefs::ECacheModeOnlyCache ) {
            m_transaction->SubmitL();
        }
        else {
            // cache failed
            complete( KErrGeneral );
        }
    }
}

void HttpConnection::cancel()
{
    complete( KErrCancel );
}

void HttpConnection::download(WebCore::ResourceHandle* handle,
                              const WebCore::ResourceRequest& request,
                              const WebCore::ResourceResponse& response)
{
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->download(handle,
        request, response, this);
}

void HttpConnection::setDefersLoading(bool defers)
{
    if (m_receivedFinished && !m_receivedFinished->isDone()) {
        // Don't set defer loading when in ReceivedFinished process
        return;
    }

    if (defers) {
        if (m_defersData) {
            m_defersData ->Cancel(); // This would happen if we did not finish sending the accumulated content, and a second JavaScript dialog is displayed.
        } else {
            m_defersData = new DefersData(this, processDefersData);
        }
        if (m_cacheSupply) {
            m_cacheSupply->PauseSupply();
        }
    }
    else {
        if (m_defersData) {
            m_defersData->Activate();
        }
        if (m_cacheSupply) {
            m_cacheSupply->ResumeSupply();
        }
    }
}

void HttpConnection::handleError(int error)
{
    complete(error);
}

void HttpConnection::MHFRunL(const THTTPEvent &aEvent)
    {
    if (m_cancelled) {
        return;
        }

    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    // Using this flag to prevent caching of secure items
    TBool nonsecure = ETrue;
    TUriParser8 requestedParser;
    if(requestedParser.Parse(m_transaction->Request().URI().UriDes()) == KErrNone) {
        if( requestedParser.Extract( EUriScheme ).Compare(KHttps) == 0 ) {
            nonsecure = EFalse;
            }
        }
    switch( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            if ( nonsecure ) {
                // pass headers to the cache first
                TRAP_IGNORE(m_cacheSupply->HeadersReceivedL());
                }
            int httpStatus( m_transaction->Response().StatusCode() );
            if (httpStatus == KErrCompletion) {
                return;
            }

            // Add certificate only if https, and top level request
            if( m_handle->request().mainLoad() ) {
                if ( m_transaction->Request().URI().Extract( EUriScheme ).FindF( KHttps ) == 0 && !m_isInCache ) {
                    m_certInfo = new(ELeave)TCertInfo;
                    m_transaction->ServerCert( *m_certInfo );
                    }
                // certinfo will be set/overwritten only if top load request
                control(m_frame)->setCertInfo( m_certInfo );
            }

            // authentication
            bool handled( EFalse );
            //in case of bad Auth Header(aError = -7276), transaction is cancelled already in the filter,
            //should be passed to Error Handler, otherwise handle Auth request normally.
            if( ( httpStatus == 401 /*EHttpUnauthorized*/ ) ||
                ( httpStatus == 407 /*EHttpProxyAuthenticationRequired*/ ) )
                {
                // Move the transaction to the Auth queue,
                // and wait for Authentication callback
                // This is proventing the transaction from being deleted
                // in case the network dropped automatically for example
                // during the data call connection.
                m_transaction->Cancel();
                HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
                httpSessionMgr->removeRequest(this);
                httpSessionMgr->addAuthRequest(this, m_handle);
                TInt authRet = handleAuthRequestL( httpStatus );
                handled = (authRet == KErrNone);
                // trans is cancelled at this point. if
                // authentication is failed (not handled) then
                // just complete this transaction
                if( !handled )
                    {
                    //
                    complete( authRet );
                    return;
                    }
                }
            if ( !handled )
                {
                // url
                m_urlResponse = NULL;
                if(m_frag) {
                    m_urlResponse = HBufC8::NewL(m_transaction->Request().URI().UriDes().Length() + m_frag->Length() + 1);
                    TPtr8 responsePtr(m_urlResponse->Des());
                    responsePtr.Copy(m_transaction->Request().URI().UriDes());
                    responsePtr.Append(_L("#"));
                    responsePtr.Append(*m_frag);
                }
                else {
                    m_urlResponse = HBufC8::NewL(m_transaction->Request().URI().UriDes().Length());
                    m_urlResponse->Des().Copy(m_transaction->Request().URI().UriDes());
                }
                // content type
                THTTPHdrVal hdrVal;
                RHTTPHeaders httpHeaders = m_transaction->Response().GetHeaderCollection();
                RStringPool stringPool = m_transaction->Session().StringPool();
                
                THTTPHdrFieldIter it = httpHeaders.Fields();  
                
                HBufC8* headerStr;  
                TBool isRefresh = EFalse;
                TPtrC8 headerValue;   ;
                while( it.AtEnd() == EFalse )
                {
                // Get name of next header field
                RStringTokenF fieldName = it();
                RStringF fieldNameStr = stringPool.StringF( fieldName );
                httpHeaders.GetRawField( fieldNameStr, headerValue );
                headerStr = HBufC8::NewLC( fieldNameStr.DesC().Length() );
                TPtr8 headerPtr( headerStr->Des() );
                headerPtr.Copy( fieldNameStr.DesC() );
                if (equalIgnoringCase(headerPtr, "refresh"))
                {
                  isRefresh = ETrue;
                	CleanupStack::PopAndDestroy(); // headerStr
                	break;
                }

                CleanupStack::PopAndDestroy(); // headerStr
                ++it;
                }            
         
                const TStringTable& stringTable = RHTTPSession::GetTable();
                if( httpHeaders.GetField( stringPool.StringF( HTTP::EContentType, stringTable ), 0,
                    hdrVal) == KErrNone ) {
                    m_contentType = hdrVal.StrF().DesC().AllocL();
                }
                else {
                    m_contentType = KNullDesC8().AllocL();
                }
                // content encoding
                if( httpHeaders.GetParam( stringPool.StringF( HTTP::EContentType, stringTable ),
                    stringPool.StringF( HTTP::ECharset, stringTable ), hdrVal ) == KErrNone ) {
                    m_encoding = hdrVal.StrF().DesC().AllocL();
                }
                else {
                    m_encoding = KNullDesC8().AllocL();
                }
                // content length
                if( httpHeaders.GetField( stringPool.StringF( HTTP::EContentLength,
                    stringTable ), 0, hdrVal ) == KErrNone ) {
                    m_maxSize = hdrVal.Int();
                }
                String encoding;
                if (m_encoding && m_encoding->Length()) {
                    encoding = m_encoding->Des();
                }
                ResourceResponse response(m_urlResponse->Des(), m_contentType->Des(), m_maxSize, encoding, String() );
                response.setHTTPStatusCode(m_transaction->Response().StatusCode());
                //HTTP status text
                response.setHTTPStatusText(((m_transaction->Response()).StatusText().DesC()));
                
                

                if (m_contentType && m_contentType->Length()) {
                    response.setHTTPHeaderField("Content-Type", *m_contentType);
                }
                
                // Add Refresh field only when Refresh existed in recieved header. 
                if (isRefresh)
                    {
                        response.setHTTPHeaderField("Refresh",headerValue);
                    }
				
                TPtrC8 result;

                if( httpHeaders.GetRawField( stringPool.StringF( HTTP::EContentDisposition, stringTable ), result) == KErrNone )
                	response.setHTTPHeaderField("Content-Disposition", result);

                if (m_handle->request().mainLoad()) {
                    if(m_handle->request().url() != response.url()) {
                        // Relative URLs are resolved based on the request URL, not response URL.
                        // If a redirect happens, the request URL must be updated.
                        m_handle->getInternal()->m_request.setURL(response.url());
                        m_frame->loader()->activeDocumentLoader()->request().setURL(response.url());
                    }
                    if(MultipartContentHandler::IsSupported(response)) {
                        m_IsMultipart = true;
                        m_MultipartContentHandler = MultipartContentHandler::NewL();
                        m_MultipartContentHandler->HandleResponseHeadersL(response, *m_transaction);
                        return;
                    }
                    if (UnknownContentHandler::isUnknownContent(response)) {
                        m_unknownContentHandler = UnknownContentHandler::NewL(this, response);
                        return;
                    }
                }
                // If loading is defered, don't send the headers now
                if (m_defersData) {
                    ResourceResponse* resp = new ResourceResponse(m_urlResponse->Des(), m_contentType->Des(), m_maxSize, encoding, String());
                    if (resp == NULL) {
                        complete(KErrNoMemory);
                    }
                    else {
                        m_defersData->m_response = resp;
                    }
                }
                else {
                    CResourceHandleManager::self()->receivedResponse(m_handle, response, this);
                }
                // transaction is complete (must have been cancelled in receivedResponse call
                if (m_isDone)
                    return;
                // transaction is taken, we need to cleanup this resource.
                if (!m_transaction) {
                    derefHandle();
                    // this object might be invalid at this point
                    return;
                    }
                }
            break;
        }
        case THTTPEvent::EGotResponseBodyData:
            {
            if ( nonsecure ) {
                // pass chunk to the cache first
                TRAP_IGNORE( m_cacheSupply->BodyReceivedL() );
                }
            MHTTPDataSupplier* body = m_transaction->Response().Body();
            // get it from the transaction
            TPtrC8 chunkPtr;
            body->GetNextDataPart( chunkPtr );
            m_accumulatedSize += chunkPtr.Length();

            if (chunkPtr.Length()) {
                if(m_IsMultipart) {
                    m_MultipartContentHandler->HandleResponseBodyL(chunkPtr);
                } else {
                    if (m_unknownContentHandler) {
                        m_unknownContentHandler->updateContentTypeL(chunkPtr);
                        if (m_defersData) {
                            m_defersData ->m_response = m_unknownContentHandler->handOffResponse();
                        }
                        else {
                            ResourceResponse* response = m_unknownContentHandler->handOffResponse();
                            CResourceHandleManager::self()->receivedResponse(m_handle, *response, this);
                            delete response;
                            // transaction is complete (must have been cancelled in receivedResponse call
                            if (m_isDone)
                                return;
                            // transaction is taken, we need to cleanup this resource.
                            if (!m_transaction) {
                                derefHandle();
                                // this object might be invalid at this point
                                return;
                            }
                        }
                        delete m_unknownContentHandler;
                        m_unknownContentHandler = NULL;
                    }
                    // If loading is defered, don't send the body now
                    if (m_defersData) {
                        HBufC8* buf = NULL;
                        buf = chunkPtr.AllocL(); // ok to leave on error because it will trigger HandleError
                        m_defersData->m_bodyParts.append(buf);
                    }
                    else {
                        WebCore::ResourceHandle* rh = handle();
                        if (rh) { rh->ref(); }
                            
                        CResourceHandleManager::self()->receivedData(m_handle, chunkPtr, 
                        m_maxSize == 0 ? chunkPtr.Length() : m_maxSize, this);
                        
                        if ( m_transaction) 
                            body->ReleaseData();
                                    
                        if (rh) { rh->deref(); }
                        
                        // this object might be invalid at this point
                        return;
                    }
                }
            }
            /* If the transaction is closed - through extensive processing through receivedData(), above - the transaction may
               already have been deleted (by HttpConnection::complete).  Check for existence of transaction
               before releasing data (ReleaseData()) to prevent attempts to double delete memory. */
            if ( m_transaction)
                body->ReleaseData();
            break;
            }
        case THTTPEvent::EResponseComplete:
            {
            // do not mix it up with the ESucceeded
            // The transaction's response is complete. An incoming event.
            if ( nonsecure ) {
                TRAP_IGNORE( m_cacheSupply->ResponseCompleteL() );
                }
            break;
            }
        case THTTPEvent::ESucceeded:
            {
            complete(KErrNone);
            break;
            }
        case THTTPEvent::ERequestComplete:
            {
            // request is all set
            if ( nonsecure ) {
                m_cacheSupply->CloseRequest();
                }
            break;
            }
        case THTTPEvent::EFailed:
        case THTTPEvent::EMoreDataReceivedThanExpected:
        case THTTPEvent::EUnrecoverableError:
        case KErrAbort:
            {
                if (!m_transaction) {
                    // this object might be invalid at this point.
                    return;
                    }
                int statusCode = m_transaction->Response().StatusCode();
                if ( statusCode != 200) {
                    complete(-25000 - m_transaction->Response().StatusCode());
                }
                else if (statusCode == 200 && aEvent.iStatus == THTTPEvent::EFailed) {
                    // this is a weird combination, it is caused by cached supplier when a response only has the
                    // response header, but no response body is sent by the HTTP stack.  It is a legitimate senario though.
                    complete(KErrNone);
                }
                else {
                    complete(aEvent.iStatus);
                }

            break;
            }
        case THTTPEvent::ERedirectRequiresConfirmation:
            {
            TInt method = m_transaction->Request().Method().Index(RHTTPSession::GetTable());
            TInt statusCode = m_transaction->Response().StatusCode();

            if( (statusCode == 301 ) &&
                !((method==HTTP::EGET) || (method==HTTP::EHEAD)) )
                 {
                    RHTTPRequest request = m_transaction->Request();
                    RHTTPHeaders requestHeaders(request.GetHeaderCollection());
                    RStringPool p = m_transaction->Session().StringPool();
                    requestHeaders.RemoveField(p.StringF(HTTP::EContentLength,RHTTPSession::GetTable()));
                    requestHeaders.RemoveField(p.StringF(HTTP::EContentType,RHTTPSession::GetTable()));
                    m_transaction->Request().RemoveBody();
                    m_transaction->Response().RemoveBody();
                    m_transaction->Request().SetMethod(p.StringF(HTTP::EGET, RHTTPSession::GetTable()));
                }
            int ret = HandleSpecialEvent(THTTPEvent::ERedirectRequiresConfirmation);
            if (ret != KErrNone) {
                handleError(ret);
                }
            break;
            }
        case THTTPEvent::ERedirectedPermanently:
        case THTTPEvent::ERedirectedTemporarily:
            {
            // for redirects, we must ensure that we properly handle
            // any new or changed uri fragment
            TUriParser8 uriParser;
            uriParser.Parse( m_transaction->Request().URI().UriDes() );
            // check for fragment
            if(uriParser.IsPresent(EUriFragment)) {
                // first extract the portion without fragment to
                // return to Http framework
                TPtrC8 uriNoFrag;
                // extract non-fragment portion into uriNoFrag
                uriParser.UriWithoutFragment( uriNoFrag );
                TUriParser8 parserNoFrag;
                parserNoFrag.Parse( uriNoFrag );
                m_transaction->Request().SetURIL( parserNoFrag );
                // now save the fragment for later use
                const TDesC8& fragment = uriParser.Extract( EUriFragment );
                delete m_frag;
                m_frag = NULL;
                m_frag = fragment.AllocL();
                }
            HandleSpecialEvent(aEvent.iStatus);
            break;
            }
        case THTTPEvent::EGotResponseTrailerHeaders:
            {
            break;// We don't process this event
            }
        default:
            {
            // error handling
            handleError(aEvent.iStatus);
            break;
            }
        }
    }

int HttpConnection::HandleSpecialEvent(int event)
{
    int ret( KErrNone );
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    switch( event )
        {
        case THTTPEvent::ERedirectRequiresConfirmation:
            {
                // Display a Redirect Confirmation dialog, before proceeding
                ret = httpSessionMgr->uiCallback()->aboutToLoadPage(control(m_frame), HttpUiCallbacks::ERedirectConfirmation);
                if (ret == KErrNone) {
                    // submit redirected transaction
                    TRAP(ret, m_transaction->SubmitL());
                    return ret;
                }
                else {
                    return KErrCancel;
                }
                break;
            }
        case THTTPEvent::ERedirectedPermanently:
        case THTTPEvent::ERedirectedTemporarily:
            {
            // Cancel transaction first and resubmit it if the user accepted a secure connection
            m_transaction->Cancel();
            if (CheckForSecurityStatusChange() != KErrNone) {
                return KErrCancel;
                }
            else {
                ret = CheckForNonHttpRedirect();
                if (ret == KErrNone) {
                    // submit redirected transaction only in case of http request.
                    TRAP(ret, m_transaction->SubmitL());
                    return ret;
                    }
                else {
                    return KErrCancel;
                    }
                }
            break;
            }
        case KErrNotSupported:
        default:
            {
                break;
            }
        }
    return ret;
}

void HttpConnection::complete(int error)
{
    if (m_defersData) {
        m_defersData->m_done = true;
        m_defersData->m_error = error;
        return;
    }
    
    // protect this function from re-entry
    if (m_isDone)
        return;
    m_isDone = true;

    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    if(m_IsMultipart && error == KErrNone) {
        TInt status = m_MultipartContentHandler->ResponseComplete();
        if(status == KErrNone) {
            TPtrC8 encoding(*m_encoding);
            if (m_MultipartContentHandler->MarkupCharset().Length()) {
                encoding.Set(m_MultipartContentHandler->MarkupCharset());
            }
            ResourceResponse response(m_urlResponse->Des(), m_MultipartContentHandler->MarkupContentType(),
                m_maxSize, encoding, String() );
            response.setHTTPStatusCode(m_transaction->Response().StatusCode());
            response.setHTTPStatusText(m_transaction->Response().StatusText().DesC());
            if(m_handle->request().url() != response.url()) {
                // Relative URLs are resolved based on the request URL, not response URL.
                // If a redirect happens, the request URL must be updated.
                m_handle->getInternal()->m_request.setURL(response.url());
                m_frame->loader()->activeDocumentLoader()->request().setURL(response.url());
            }
            CResourceHandleManager::self()->receivedResponse(m_handle, response, this);
            CResourceHandleManager::self()->receivedData(m_handle,
                m_MultipartContentHandler->MarkupContent(), m_maxSize, this);
        }
        delete m_MultipartContentHandler;
    }
    if (!error) {
        // Spawn active object for call to return immediately, to avoid blocking
        activateReceivedFinished(error);  
    }

    if (m_cacheSupply) {
        delete m_cacheSupply;
        m_cacheSupply = NULL;
    }
    if (m_postDataSupplier) {
        delete m_postDataSupplier;
        m_postDataSupplier = NULL;
    }
    if (m_transaction) {
        m_transaction->Cancel();
        m_transaction->Close();
        delete m_transaction;
        m_transaction = NULL;
    }
    if (error) {
        CResourceHandleManager::self()->receivedFinished(m_handle, error, this);
        derefHandle();
    }
}

// -----------------------------------------------------------------------------
// HttpConnection::handleAuthRequestL
//
// -----------------------------------------------------------------------------
//
int HttpConnection::handleAuthRequestL(
    int status )
    {
    int ret( KErrNone );
    THTTPHdrVal usernameVal;
    THTTPHdrVal passwordVal;
    THTTPHdrVal realmVal;
    THTTPHdrVal staleVal;
    RHTTPTransactionPropertySet propSet = m_transaction->PropertySet();
    RHTTPSession& session = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpSession();
    RStringPool strP = session.StringPool();
    const TStringTable& stringTable = RHTTPSession::GetTable();

    bool isProxy( EFalse );
    bool needClose( EFalse );
    bool pwdClose( EFalse );
    bool realmClose( EFalse );
    bool stale( propSet.Property( strP.StringF( HTTP::EStale, stringTable ), staleVal ) );

    _LIT8( KStrNull, "" );
    //
    switch( status )
        {
        case EHttpUnauthorized:
            {
            if( !propSet.Property( strP.StringF( HTTP::ERealm, stringTable ), realmVal ) )
                {
                realmClose = ETrue;
                realmVal = strP.OpenStringL( KStrNull );
                }
            if( !propSet.Property( strP.StringF( HTTP::EUsername, stringTable ), usernameVal ) )
                {
                needClose = ETrue;
                usernameVal = strP.OpenStringL( KStrNull );
                }
            if( !propSet.Property(strP.StringF( HTTP::EPassword, stringTable ), passwordVal ) )
                {
                pwdClose = ETrue;
                passwordVal = strP.OpenStringL( KStrNull );
                }
            break;
            }
        case EHttpProxyAuthenticationRequired:
            {
            const TStringTable& stringTableEx = HttpFilterCommonStringsExt::GetTable();
            //
            isProxy = ETrue;
            if( !propSet.Property(strP.StringF( HttpFilterCommonStringsExt::EProxyRealm,
                stringTableEx ), realmVal ) )
                {
                return KErrHttpDecodeUnknownAuthScheme;
                }
            if( !propSet.Property( strP.StringF( HttpFilterCommonStringsExt::EProxyUsername,
                stringTableEx ), usernameVal ) )
                {
                needClose = ETrue;
                usernameVal = strP.OpenStringL( KStrNull );
                }
            if( !propSet.Property( strP.StringF( HTTP::EPassword, stringTable ), passwordVal ) )
                {
                pwdClose = ETrue;
                passwordVal = strP.OpenStringL( KStrNull );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, THttpConnUtils::PanicLoader( KErrArgument ) );
            break;
            }
        }

    TRAP( ret, SendAuthRequestL( usernameVal, realmVal, isProxy, stale, passwordVal ) );
    if (realmClose)
      {
        realmVal.Str().Close();
        }
    if( needClose )
        {
        usernameVal.Str().Close();
        }
    if( pwdClose )
        {
        passwordVal.Str().Close();
        }
    return(ret);
    }

// -----------------------------------------------------------------------------
// HttpConnection::SendAuthRequestL
//
// -----------------------------------------------------------------------------
//
void HttpConnection::SendAuthRequestL(
    THTTPHdrVal& aUsernameVal,
    THTTPHdrVal& aRealmVal,
    bool aIsProxy,
    bool aStale,
    THTTPHdrVal& aPasswordVal )
    {

    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    RHTTPSession& session = httpSessionMgr->httpSession();
    int popCount( 0 );

    // prepare uri param
    int len = m_transaction->Request().URI().UriDes().Length() + 1;
    TUint16* uri = new(ELeave) TUint16 [ len ];
    CleanupStack::PushL( uri );
    popCount++;
    TPtr uriPtr( uri, len );
    uriPtr.Copy( m_transaction->Request().URI().UriDes() );
    uriPtr.ZeroTerminate();

    // prepare username param
    len = aUsernameVal.Str().DesC().Length() + 1;
    TUint16* username = new(ELeave) TUint16 [ len ];
    CleanupStack::PushL( username );
    popCount++;
    TPtr usernamePtr( username, len );
    usernamePtr.Copy( aUsernameVal.Str().DesC() );
    usernamePtr.ZeroTerminate();

    // prepare realm param
    len = aRealmVal.Str().DesC().Length() + 1;
    TUint16* realm = new(ELeave) TUint16 [ len ];
    CleanupStack::PushL( realm );
    popCount++;
    TPtr realmPtr( realm, len );
    realmPtr.Copy( aRealmVal.Str().DesC() );
    realmPtr.ZeroTerminate();

    // prepare password param
    len = aPasswordVal.Str().DesC().Length() + 1;
    TUint16* password = new(ELeave) TUint16 [ len ];
    CleanupStack::PushL( password );
    popCount++;
    TPtr passwordPtr( password, len );
    passwordPtr.Copy( aPasswordVal.Str().DesC() );
    passwordPtr.ZeroTerminate();

    // Get the authentication type
    bool basicAuthentication( EFalse );
    RHTTPTransactionPropertySet propSet = m_transaction->PropertySet();
    THTTPHdrVal propRetVal;

    // Get the name of the property
    RStringF propName = session.StringPool().StringF( HTTP::EBasic, RHTTPSession::GetTable() );

    // if it is set . .
    if( propSet.Property( propName, propRetVal ) )
        {
        // . . then we have basic auth
        basicAuthentication = ETrue;
        }

    // send data to dialog layer for user input.  Data from user is provided
    // back to http transaction. by UiCallbacks calling HttpConnection::AuthenticationResponse
    // Completion of this authentication process will therefore occur before
    // the call below returns.
    httpSessionMgr->uiCallback()->AuthenticationRequest( this, uriPtr, usernamePtr,
        realmPtr, aIsProxy, aStale, passwordPtr, basicAuthentication );
    CleanupStack::PopAndDestroy( popCount ); // password, realm, username, uri
    }

// -----------------------------------------------------------------------------
// HttpConnection::AuthenticationResponse
//
//
// -----------------------------------------------------------------------------
//
void HttpConnection::AuthenticationResponse(
    TPtr& aUsername,
    TPtr& aPassword,
    bool aProxy,
    int aError)
    {
    TUint16* username = (TUint16*) aUsername.Ptr();
    TUint16* password = (TUint16*) aPassword.Ptr();

    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    int ret = KErrNone;

    // Execution is back from the dialog, move the transaction
    // from the authentication listback into active list
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    httpSessionMgr->removeAuthRequest(this);
    httpSessionMgr->addRequest(this, m_handle);

    switch (aError)
        {
        case KErrNone:
            {
            TRAP( ret, this->AddAuthenticationPropertiesL( aUsername, aPassword, aProxy) );
            if (ret == KErrNone)
                {
                m_transaction->SubmitL();
                break;
                }
            aError = ret;
            }
        case KErrCancel:
        case KErrNoMemory:
        default:
            {
            delete username;
            delete password;
            this->complete( aError );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// HttpConnection::AddAuthenticationPropertiesL
// Add username and password properties to the transaction.
// -----------------------------------------------------------------------------
//
void HttpConnection::AddAuthenticationPropertiesL(
    TPtr& aUsername,
    TPtr& aPassword,
    bool aProxy )
    {
    TUint16* username = (TUint16*) aUsername.Ptr();
    TUint16* password = (TUint16*) aPassword.Ptr();
    __ASSERT_DEBUG( username != NULL && password != NULL, THttpConnUtils::PanicLoader( KErrArgument ) );

    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    RString usernameStr;
    RString passwordStr;
    RStringPool stringPool = m_transaction->Session().StringPool();
    RHTTPTransactionPropertySet propSet = m_transaction->PropertySet();
    //
    User::LeaveIfError( THttpConnUtils::OpenStringFromUnicode( stringPool, username, usernameStr ) );
    CleanupClosePushL( usernameStr );
    User::LeaveIfError( THttpConnUtils::OpenStringFromUnicode( stringPool, password, passwordStr ) );
    CleanupClosePushL( passwordStr );
    // set proxy authentication
    if( aProxy )
        {
        propSet.RemoveProperty( stringPool.StringF( HttpFilterCommonStringsExt::EProxyUsername,
            HttpFilterCommonStringsExt::GetTable() ) );

        propSet.SetPropertyL( stringPool.StringF( HttpFilterCommonStringsExt::EProxyUsername,
            HttpFilterCommonStringsExt::GetTable() ), usernameStr);

        propSet.RemoveProperty( stringPool.StringF( HttpFilterCommonStringsExt::EProxyPassword,
            HttpFilterCommonStringsExt::GetTable() ) );

        propSet.SetPropertyL( stringPool.StringF( HttpFilterCommonStringsExt::EProxyPassword,
            HttpFilterCommonStringsExt::GetTable()), passwordStr );
        }
    else
        {
        propSet.RemoveProperty( stringPool.StringF( HTTP::EUsername, RHTTPSession::GetTable() ) );

        propSet.SetPropertyL( stringPool.StringF( HTTP::EUsername, RHTTPSession::GetTable() ),
            usernameStr );

        propSet.RemoveProperty( stringPool.StringF( HTTP::EPassword, RHTTPSession::GetTable() ) );

        propSet.SetPropertyL( stringPool.StringF( HTTP::EPassword, RHTTPSession::GetTable() ),
            passwordStr );
        }
    CleanupStack::PopAndDestroy( 2 ); // passwordStr, usernameStr
    }

// -----------------------------------------------------------------------------
// HttpConnection::CheckForSecurityStatusChange
//
// -----------------------------------------------------------------------------
//
int HttpConnection::CheckForSecurityStatusChange()
    {
    int error(KErrNone);
    bool requestedSecScheme(EFalse);
    bool redirectedSecScheme(EFalse);

    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    if (m_handle->request().mainLoad() && !m_frame->ownerElement())
        {
        TUriParser8 redirectedParser;
        if(redirectedParser.Parse( m_transaction->Request().URI().UriDes() ) == KErrNone)
            {
            //
            if( redirectedParser.Extract( EUriScheme ).Compare(KHttps) == 0 )
                {
                    redirectedSecScheme = ETrue;
                }
            }
        TUriParser8 requestedParser;
        if(m_handle->request().url().des().Length() != 0 )
            {
            error = requestedParser.Parse(m_handle->request().url().des());
            }
        else
            {
            error = requestedParser.Parse(m_transaction->Request().URI().UriDes());
            }
        if(error == KErrNone)
            {
            //
            if( requestedParser.Extract( EUriScheme ).Compare(KHttps) == 0 )
                {
                requestedSecScheme = ETrue;
                }
            }
        //When submitting the request iSecurePage was set based on the request url
        //Check the redirect url and see if the scheme has changed
        HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
        if(requestedSecScheme && !redirectedSecScheme) //redirection from a secure page to an unsecure one
            {
            error = httpSessionMgr->uiCallback()->aboutToLoadPage(control(m_frame), HttpUiCallbacks::EExitingSecurePage);
            }
        else if(redirectedSecScheme && !requestedSecScheme) //redirection to unsecurepage when secure page was requested
            {
            error = httpSessionMgr->uiCallback()->aboutToLoadPage(control(m_frame), HttpUiCallbacks::EEnteringSecurePage );
            }
        }
    return error;
    }

// -----------------------------------------------------------------------------
// HttpConnection::CheckForNonHttpRedirect
//
// -----------------------------------------------------------------------------
//
TInt HttpConnection::CheckForNonHttpRedirect()
    {
    TUriParser8 uriParser;

    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    if(uriParser.Parse( m_transaction->Request().URI().UriDes() ) == KErrNone)
        {
        if (uriParser.IsPresent(EUriHost) && uriParser.IsPresent(EUriScheme)) // looking only for absolue Url path and schemes other than http(s)
            {
            const TDesC8& scheme = uriParser.Extract(EUriScheme);
            if (scheme.FindF(_L8("http")) == KErrNotFound) // everything but http(s)
                {
                TPtrC8 ptr(uriParser.UriDes());
                // these arrays are pushed into CleanupStack in case leave
                // if no leave, they will be freed below
                RArray<TUint>* typeArray = new (ELeave) RArray<TUint>(1);
                CleanupStack::PushL(typeArray);

                CDesCArrayFlat* desArray = new (ELeave) CDesCArrayFlat(1);
                CleanupStack::PushL(desArray);

                User::LeaveIfError(typeArray->Append(EParamRequestUrl));

                HBufC16* urlbuf = HBufC16::NewLC( ptr.Length()  + 1); // +1 for zero terminate
                urlbuf->Des().Copy( ptr );
                TPtr16 bufDes16 = urlbuf->Des();
                bufDes16.ZeroTerminate();

                desArray->AppendL(bufDes16);
                CleanupStack::Pop();

                MBrCtlSpecialLoadObserver* loadObserver = control(m_frame)->brCtlSpecialLoadObserver();

                if (loadObserver)
                    {
                    TRAP_IGNORE(loadObserver->HandleRequestL(typeArray, desArray));
                    }

                // No leave, so pop here and clean up
                CleanupStack::Pop(desArray);
                CleanupStack::Pop(typeArray);

                // cleanup arrays
                if (typeArray)
                    {
                    // Closes the array and frees all memory allocated to the array
                    typeArray->Close();
                    delete typeArray;
                    }

                if (desArray)
                    {
                    // Deletes all descriptors from the array and frees the memory allocated to the array buffer
                    desArray->Reset();
                    delete desArray;
                    }

                return KErrCancel;
                }
            }
        }
    return KErrNone;
    }

RHTTPTransaction* HttpConnection::takeOwnershipHttpTransaction()
{
    __ASSERT_DEBUG( m_transaction, THttpConnUtils::PanicLoader( KErrArgument ) );

    RHTTPTransaction* trans = m_transaction;
    RHTTPSession& session = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpSession();
    // remove own address from transaction properties
    m_transaction->PropertySet().RemoveProperty(session.StringPool().StringF(HttpFilterCommonStringsExt::ESelfPtr,
        HttpFilterCommonStringsExt::GetTable()));
    m_transaction = NULL;
    return trans;
}

// -----------------------------------------------------------------------------
// HttpConnection::addIMEINotifyPropertiesL
// Keeping for completeness, but currently is not needed as functionality is implemented
// in User-Agent Profile Filter
// Add IMEINotify properties to the transaction.
// -----------------------------------------------------------------------------
//
/*
void HttpConnection::addIMEINotifyPropertiesL()
{
    // Get IMEI Notify enable/disable setting
    HttpSessionManager* httpSessionMgr = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    bool imeiNotify = httpSessionMgr->imeiEnabled();

    RHTTPTransactionPropertySet propSet = m_transaction->PropertySet();

    RStringPool stringPool = m_transaction->Session().StringPool();
    propSet.RemoveProperty( stringPool.StringF(
        HttpFilterCommonStringsExt::EIMEINotify, HttpFilterCommonStringsExt::GetTable() ) );
    //
    propSet.SetPropertyL( stringPool.StringF( HttpFilterCommonStringsExt::EIMEINotify,
        HttpFilterCommonStringsExt::GetTable() ), THTTPHdrVal( imeiNotify ) );
}
*/

// -----------------------------------------------------------------------------
// HttpLoaderUtils::IsUrlInCache
//
// Returns ETrue if Cache Manager finds the Url in cache, EFalse otherwise
// -----------------------------------------------------------------------------
//
bool HttpConnection::IsUrlInCacheL(
    const TDesC8& aUrl )
{
    TBool inCache( EFalse );  // not in cache by default
	m_isInCache = EFalse;

    CHttpCacheManager* cache = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cacheManager();

    if ( cache )
    {
        // call cache manager to check for url in cache
        inCache = cache->Find( aUrl );
    }
	m_isInCache = inCache;
    return inCache;
}

void HttpConnection::processDefersData(void* ctx)
{
    HttpConnection* self = static_cast<HttpConnection*>(ctx);
    DefersData* defersData = self->m_defersData;
    if (self->m_cancelled)
        return;
    if (self->m_defersData->m_response) {
        CResourceHandleManager::self()->receivedResponse(self->m_handle, *(self->m_defersData->m_response), self);
        // transaction is complete (must have been cancelled in receivedResponse call
        if (self->m_isDone)
            return;
        // transaction is taken, we need to cleanup this resource.
        if (!self->m_transaction) {
            self->derefHandle();
            // this object might be invalid at this point
            return;
        }
        delete self->m_defersData->m_response;
        self->m_defersData->m_response = NULL;
        self->m_defersData->Activate();
        return;
    }
    if (self->m_defersData->m_bodyParts.size()) {
        HBufC8* buf = self->m_defersData->m_bodyParts.first();
        self->m_defersData->m_bodyParts.remove(0);
        CResourceHandleManager::self()->receivedData(self->m_handle, *buf, self->m_maxSize, self);
        delete buf;
        self->m_defersData->Activate();
        return;
    }
    if (self->m_defersData->m_done) {
        self->m_defersData = NULL;
        self->complete(defersData->m_error);
    }
    self->m_defersData = NULL;
    delete defersData;
}


void HttpConnection::activateReceivedFinished(TInt errorCode) 
{
        if (m_receivedFinished) {
            if (m_receivedFinished->isDone()) {
                delete m_receivedFinished;
            } else {
                // This would happen if processing of a previous transaction is hung.
                // We should never get in here.
                m_receivedFinished ->Cancel();
                return;
            }
        }
            
        m_receivedFinished = new ReceivedFinished(this, processReceivedFinished);
        m_receivedFinished->Activate(errorCode);
}

void HttpConnection::processReceivedFinished(void* ctx, TInt errorCode)
{
    HttpConnection* self = static_cast<HttpConnection*>(ctx);
    CResourceHandleManager::self()->receivedFinished(self->m_handle, errorCode, self);

    self->derefHandle();
}


// end of file
