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

#include "ResourceHandle.h"
#include "ResourceHandleManagerSymbian.h"
#include "ResolvedConnection.h"
#include "ResourceRequest.h"

#include "BrCtl.h"
#include "WebFrame.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

using namespace WebCore;

TInt processResponseCb(TAny* ptr)
{
    ResolvedConnection* rc = static_cast<ResolvedConnection*>(ptr);
    rc->processResponse();
    return KErrNone;
}

ResolvedConnection::ResolvedConnection(ResourceHandle* _handle, Frame* frame) : MUrlConnection(_handle)
{
    m_frame = frame;
    m_didSubmit = false;
    m_scheduler = NULL;
    m_contentType = NULL;
    m_charset = NULL;
    m_contentBuf = NULL;
    m_maxSize = 0;
}

ResolvedConnection::~ResolvedConnection()
{
    if (m_scheduler) {
        m_scheduler->Cancel();
        delete m_scheduler;
    }
    delete m_contentType;
    delete m_charset;
    delete m_contentBuf;
}

int ResolvedConnection::submit()
{
    m_didSubmit = true;
    if (m_contentBuf) {
        m_scheduler = CPeriodic::NewL( CActive::EPriorityStandard );
        m_scheduler->Start( 0, 0, TCallBack( &processResponseCb, this ) );
    }
    return KErrNone;
}

void ResolvedConnection::cancel()
{
    control(m_frame)->brCtlLinkResolver()->CancelAll();
}

void ResolvedConnection::download(ResourceHandle* handle,
                                  const ResourceRequest& request,
                                  const ResourceResponse& response)
{
    __ASSERT_ALWAYS(EFalse, User::Panic(_L("Resource Loader"), KErrArgument));
}

void ResolvedConnection::HandleResolveComplete(const TDesC& aContentType,
                                               const TDesC& aCharset,
                                               const HBufC8* aContentBuf)
{
    if (aContentType.Length() == 0 || aContentBuf == NULL) {
        complete(KErrArgument);
    }
    else {
        m_contentType = aContentType.Alloc();
        m_charset = aCharset.Alloc();
        m_contentBuf = aContentBuf->Alloc();
        if (!(m_contentType && m_charset && m_contentBuf)) {
            complete(KErrNoMemory);
        }
        else {
            m_maxSize = aContentBuf->Length();
            if (m_didSubmit) {
                m_scheduler = CPeriodic::NewL( CActive::EPriorityStandard );
                m_scheduler->Start( 0, 0, TCallBack( &processResponseCb, this ) );
            }
        }
    }
}

void ResolvedConnection::processResponse()
{
    if (m_scheduler) {
        m_scheduler->Cancel();
        delete m_scheduler;
        m_scheduler = NULL;
    }
    __ASSERT_DEBUG(m_didSubmit && m_contentBuf, User::Panic(_L(""), KErrGeneral));
    ResourceResponse response(m_handle->request().url().des(), m_contentType->Des(), m_contentBuf->Length(), m_charset->Des(), String() );
    response.setHTTPStatusCode(200);
    CResourceHandleManager::self()->receivedResponse(m_handle, response, this);
    CResourceHandleManager::self()->receivedData(m_handle, m_contentBuf->Des(), m_contentBuf->Length(), this);
    CResourceHandleManager::self()->receivedFinished(m_handle, KErrNone, this);
    derefHandle();
}

void ResolvedConnection::HandleResolveError(TInt aError)
{
    complete(aError);
}

void ResolvedConnection::complete(int error)
{
    CResourceHandleManager::self()->receivedFinished(m_handle, error, this);
    derefHandle();
}

// end of file
