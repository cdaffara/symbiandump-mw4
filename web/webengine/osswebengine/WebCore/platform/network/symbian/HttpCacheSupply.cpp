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
* Description:  Implementation of CHttpCacheSupply
*
*/


// INCLUDE FILES
#include "HttpCacheSupply.h"
#include "httpcachemanager.h"
#include "HttpCacheDataSupplier.h"
#include "HttpConnection.h"
#include "ResourceLoaderDelegate.h"
#include "StaticObjectsContainer.h"

#include <http/rhttptransaction.h>
#include <http/mhttpdatasupplier.h>
#include <http/rhttpsession.h>
#include <httperr.h>


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KResponseTimeout = 0;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheSupply::CHttpCacheSupply
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheSupply::CHttpCacheSupply(HttpConnection* aHttpTransaction)
    : m_httpTransaction( aHttpTransaction ),
      m_reponseState( THTTPEvent::EClosed )
{
    m_cacheEntry.iCacheHandler = NULL;
    m_cacheEntry.iCacheEntry = NULL;
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::ConstructL()
{
    m_cacheManager = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cacheManager();
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheSupply* CHttpCacheSupply::NewL(HttpConnection* aHttpTransaction)
{
    CHttpCacheSupply* self = new( ELeave ) CHttpCacheSupply( aHttpTransaction );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
}

// Destructor
CHttpCacheSupply::~CHttpCacheSupply()
{
    delete m_dataSupplier;
    CloseRequest();
    delete m_responseTimer;
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::StartRequestL
//
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheSupply::StartRequestL(TBrCtlDefs::TBrCtlCacheMode aCacheMode)
{
    TInt status;
    status = m_cacheManager->RequestL( *(m_httpTransaction->HttpTransaction()), aCacheMode, m_cacheEntry );
    // start a timer that feeds the content to the transaction
    if( status == KErrNone ) {
        m_closed = EFalse;
        // set response state. start with the http headers.
        m_reponseState = THTTPEvent::EGotResponseHeaders;
        m_responseTimer = CPeriodic::NewL( CActive::EPriorityHigh );
        m_responseTimer->Start( KResponseTimeout, KResponseTimeout, TCallBack( &ResponseCallbackL, this ) );
    }
    return status;
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::CloseRequest
//
//
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::CloseRequest()
{
    if( !m_closed) {
        m_cacheManager->RequestClosed( (m_httpTransaction->HttpTransaction()), m_cacheEntry );
        m_closed = ETrue;
    }
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::HeadersReceived
//
//
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::HeadersReceivedL()
{
    // do not cache content we just sent off
    if( m_reponseState == THTTPEvent::EClosed ) {
        RHTTPTransaction* trans = m_httpTransaction->HttpTransaction();
        m_cacheManager->ReceivedResponseHeadersL( *(m_httpTransaction->HttpTransaction()), m_cacheEntry );
        //
        m_notModified = trans->Response().StatusCode() == HTTPStatus::ENotModified;
        if( m_notModified ) {
            // change from 304 to 200 -otherwise trans exits with EFailed
            trans->Response().SetStatusCode( HTTPStatus::EOk );
        }
    }
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::BodyReceivedL
//
//
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::BodyReceivedL()
{
    // do not cache content we just sent off
    if( m_reponseState == THTTPEvent::EClosed ) {
        RHTTPTransaction* trans = m_httpTransaction->HttpTransaction();
        MHTTPDataSupplier* supplier = trans->Response().Body();
        //
        m_cacheManager->ReceivedResponseBodyDataL( *trans, *supplier, m_cacheEntry );
    }
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::ResponseComplete
//
//
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::ResponseCompleteL()
{
    if( m_reponseState == THTTPEvent::EClosed ) {
        RHTTPTransaction* trans = m_httpTransaction->HttpTransaction();
        // not modified needs body before response complete
        if( m_notModified ) {
            // use cache
            // close response first
            m_cacheManager->ResponseComplete( *trans, m_cacheEntry );
            // request the item from cache
            if( m_cacheManager->RequestL( *trans, TBrCtlDefs::ECacheModeOnlyCache, m_cacheEntry ) == KErrNone ) {
                // ser
                m_reponseState = THTTPEvent::EGotResponseBodyData;
                //
                SendBodyL();
                CloseRequest();
            }
        }
        else {
            // normal response complete
            m_cacheManager->ResponseComplete( *trans, m_cacheEntry );
        }
    }
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::ResponseCallbackL
//
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheSupply::ResponseCallbackL(TAny* aAny)
{
    //
    CHttpCacheSupply* thisObj = (CHttpCacheSupply*)aAny;
    thisObj->SupplyResponseL();
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::SupplyResponseL
//
//
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::SupplyResponseL()
{
    RHTTPTransaction* trans = m_httpTransaction->HttpTransaction();
    //
    switch( m_reponseState )
    {
        case THTTPEvent::EGotResponseHeaders:
            {
                m_failed = EFalse;
                if( m_cacheManager->RequestHeadersL( *trans, m_cacheEntry ) == KErrNone ) {
                    //
                    trans->Response().SetStatusCode( HTTPStatus::EOk );
                    //
                    m_httpTransaction->MHFRunL( THTTPEvent::EGotResponseHeaders );
                    // move to the next state
                    m_reponseState = THTTPEvent::EGotResponseBodyData;
                }
                else {
                    // move to the next state
                    m_failed = ETrue;
                    // move to the next state
                    m_reponseState = THTTPEvent::EResponseComplete;
                }
                break;
            }
        case THTTPEvent::EGotResponseBodyData:
            {
                SendBodyL();
                // move to the next state
                m_reponseState = THTTPEvent::EResponseComplete;
                break;
            }
        case THTTPEvent::EResponseComplete:
            {
                m_httpTransaction->MHFRunL(THTTPEvent::EResponseComplete );
                // move to the next state
                m_reponseState = !m_failed ? THTTPEvent::ESucceeded : THTTPEvent::EFailed;
                break;
            }
        case THTTPEvent::ESucceeded:
            {
                // move to the next state
                m_reponseState = THTTPEvent::EClosed;
                // cancel timer
                m_responseTimer->Cancel();
                //
                m_httpTransaction->MHFRunL(THTTPEvent::ESucceeded );
                // this obj is destroyed at this point
                break;
            }
        case THTTPEvent::EFailed:
            {
                // move to the next state
                m_reponseState = THTTPEvent::EClosed;
                // cancel timer
                m_responseTimer->Cancel();
                m_httpTransaction->MHFRunL(THTTPEvent::EFailed );
                // this obj is destroyed at this point
                break;
            }
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::SendBodyL
//
//
// -----------------------------------------------------------------------------
//
void CHttpCacheSupply::SendBodyL()
{
    RHTTPTransaction* trans = m_httpTransaction->HttpTransaction();
    //
    TBool lastChunk;
    // currently it is always the last chunk
    HBufC8* body = m_cacheManager->RequestNextChunkL( *trans, lastChunk, m_cacheEntry );
    if( body ) {
        CleanupStack::PushL( body );
        // create datasupplier and attach it to the transaction
        if( !m_dataSupplier )
            m_dataSupplier = CHttpCacheDataSupplier::NewL( body );
        trans->Response().SetBody( *m_dataSupplier );
        CleanupStack::Pop(); // body
        //
        m_httpTransaction->MHFRunL(THTTPEvent::EGotResponseBodyData );
    }
    else {
        // move to the next state
        m_failed = ETrue;
    }
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::PauseSupply
// -----------------------------------------------------------------------------
void CHttpCacheSupply::PauseSupply()
{
    if( m_responseTimer && m_responseTimer->IsActive() )
        m_responseTimer->Cancel();
}

// -----------------------------------------------------------------------------
// CHttpCacheSupply::ResumeSupply
// -----------------------------------------------------------------------------
void CHttpCacheSupply::ResumeSupply()
{
    if( m_responseTimer && !m_responseTimer->IsActive() )
        m_responseTimer->Start( KResponseTimeout, KResponseTimeout, TCallBack( &ResponseCallbackL, this ) );
}

//  End of File
