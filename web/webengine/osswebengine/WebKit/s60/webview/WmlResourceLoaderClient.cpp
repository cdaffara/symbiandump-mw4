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
* Description:   
*
*/


//  INCLUDES
#include "WmlResourceLoaderClient.h"
#include "WmlResourceLoader.h"
#include "WmlDispatcher.h"
#include "Page.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "ProgressTracker.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "StaticObjectsContainer.h"


#include "WebKitLogger.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
using namespace WebCore;


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
WmlResourceLoaderClient* WmlResourceLoaderClient::CreateResourceLoaderClient(CWmlDispatcher* wmldispatcher)
{
    //this object get's deleted by the WmlStreamLoader. 
    return new WmlResourceLoaderClient(wmldispatcher);    
}

// -----------------------------------------------------------------------------
//   C++ default constructor.
// -----------------------------------------------------------------------------
WmlResourceLoaderClient::WmlResourceLoaderClient(CWmlDispatcher* wmldispatcher) : 
    m_wmldispatcher(wmldispatcher),    
    m_loader(0),
    m_contentType(0),
    m_charset(0),
    m_url(0),        
    m_error(0),
    m_contentLength(0),
    m_restLength(0),
    m_httpStatus (0)
{
}

// -----------------------------------------------------------------------------
//   C++ destructor.
// -----------------------------------------------------------------------------  
WmlResourceLoaderClient::~WmlResourceLoaderClient( )
{
    delete m_url;
    delete m_contentType;
    delete m_charset;   
}

void WmlResourceLoaderClient::start(TDesC8& url, Frame* frame)
{

    WebCore::ResourceRequest* request = new WebCore::ResourceRequest(WebCore::String(url));    
    m_loader = WebCore::WmlResourceLoader::create(frame, this);
    
    if (m_loader) {
        m_loader->documentLoader()->addSubresourceLoader(m_loader);
        m_loader->load(*request);            
    }
}

void WmlResourceLoaderClient::stop()
{
    if (m_loader && !m_loader->isDone())
        m_loader->cancel();
}

void WmlResourceLoaderClient::cancelWithError(const WebCore::ResourceError& error)
{
    if (m_loader && !m_loader->isDone()) 
        m_loader->cancel(error); 
}

void WmlResourceLoaderClient::didReceiveResponse(const WebCore::ResourceResponse& response)
{ 
    int status  = 200;
    if(response.isHTTP()){
        status = response.httpStatusCode();
    }
    
    if (status < 200 || status >= 300) {          
        cancelWithError(WebCore::ResourceError(WebCore::String(response.url()), KErrGeneral, WebCore::String(response.url()), WebCore::String(response.httpStatusText())));        
        return;
    }
    
    delete m_contentType;
    m_contentType = NULL;
    
    delete m_charset;
    m_charset = NULL;
    
    delete m_url;
    m_url = NULL;
    
    m_contentType = response.mimeType().des().AllocL();
    m_charset = response.textEncodingName().des().AllocL();
    
    TPtrC myUrl = response.url().url().des();
    m_url = HBufC::New(myUrl.Length() + 1);
    m_url->Des().Copy(myUrl);
    m_url->Des().ZeroTerminate();
    
    m_contentLength = response.expectedContentLength();
    m_httpStatus = status; 
    m_restLength = 0;
}

void WmlResourceLoaderClient::didReceiveData(const char* data, int length, long long lengthReceived)
{
	   //send data to wmlcontrol
    TInt chunkIndex = 0;     
    if(m_contentLength > length && m_restLength){
        chunkIndex++;     
    }

    if (m_contentType && m_charset && m_url && m_wmldispatcher) {
        m_wmldispatcher->WmlParameters( data, length, *m_contentType, *m_charset, m_httpStatus, chunkIndex, *m_url);        
    }
    
    m_restLength = m_contentLength - length;   
}

void WmlResourceLoaderClient::didFinishLoading()
{
    TInt chunkIndex = -1;      // last chunk
    if (m_contentType && m_charset && m_url && m_wmldispatcher) {
        m_wmldispatcher->WmlParameters( NULL, 0, *m_contentType, *m_charset, m_httpStatus, chunkIndex, *m_url );
    }
    if ( m_wmldispatcher ) {
        m_wmldispatcher->HandleError(0,KErrCompletion);
    }
}

void WmlResourceLoaderClient::didFail(const WebCore::ResourceError& error)
{    
    if ( m_wmldispatcher ) {
        m_wmldispatcher->HandleError(0,error.errorCode());
    }
}

void WmlResourceLoaderClient::didCancel(const WebCore::ResourceError& error)
{
    if ( m_wmldispatcher ) {
        m_wmldispatcher->HandleError(0,error.errorCode());
    }
}

// End of File

