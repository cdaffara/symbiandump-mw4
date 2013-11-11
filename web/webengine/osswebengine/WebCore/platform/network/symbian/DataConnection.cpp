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

#include <f32file.h>
#include <uri8.h>
#include <escapeutils.h>
#include <apmrec.h>
#include <apgcli.h>
//#include <imcvcodc.h>
#include <tconvbase64.h>
#include "ResourceHandle.h"
#include "ResourceRequest.h"
#include "DataConnection.h"
#include "FileReader.h"
#include "ResourceHandleManagerSymbian.h"
#include "StaticObjectsContainer.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT8( KDefaultCharset, "iso-8859-1" );
_LIT8( KDefaultContentType, "text/plain" );
_LIT8( KDataScheme, "data:" );
_LIT8( KContentTypeKey, "content type:" );
_LIT8( KCharsetKey, "charset=" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

static int s_dataTransactionsCount = 0;

using namespace WebCore;

DataConnection::DataConnection(ResourceHandle* _handle) : MUrlConnection(_handle)
{
    s_dataTransactionsCount++;
    m_scheduler = NULL;
    m_maxSize = 0;
}

DataConnection::~DataConnection()
{
    s_dataTransactionsCount--;
    if (m_scheduler) {
        m_scheduler->Cancel();
        delete m_scheduler;
    }
}

int DataConnection::submit()
{
    TRAPD(error, submitL());
    return error;
}

void DataConnection::submitL()
{
    m_scheduler = CPeriodic::NewL( CActive::EPriorityStandard );
    m_scheduler->Start( 0, 0, TCallBack( &sendResponseCb, this ) );
}

void DataConnection::parseUrlLC(HBufC8*& contentType, HBufC8*& encoding, HBufC8*& body)
{
    TPtrC8 url = m_handle->request().url().des();
    // set content type and content encoding
    // parse url to get header information
    // http://www.faqs.org/rfcs/rfc2397.html
    // 3. Syntax

    // dataurl    := "data:" [ mediatype ] [ ";base64" ] "," data
    // mediatype  := [ type "/" subtype ] *( ";" parameter )
    // data       := *urlchar
    // parameter  := attribute "=" value
    //
    // data:text/plain;charset=iso-8859-7,%be%fg%be

    TPtrC8 urlPtr8 =  m_handle->request().url().des();
    TInt colonPos( urlPtr8.Locate( ',' ) );
    if( colonPos != KErrNotFound ) {
        // set header including ,
        TPtrC8 headersStr( urlPtr8.Left( colonPos + 1 ) );
        // cut off scheme data:
        headersStr.Set( headersStr.Mid( KDataScheme().Length() ) );
        // check for ;base64
        TBool base64( headersStr.Find( _L8( ";base64" ) ) != KErrNotFound );
        // check for ;
        TInt semicolonPos;
        do {
            semicolonPos = headersStr.Locate( ';' );
            // no semicolon means one mediatype. set semiPos to the end of the str
            // unless we are at the end of the string
            if( headersStr.Length() > 1 ) {
                semicolonPos = semicolonPos == -1 ? headersStr.Length() - 1 : semicolonPos;
            }
            if( semicolonPos != -1 ) {
                // str = text/plain
                TPtrC8 str( headersStr.Left( semicolonPos ) );
                if( str.Locate( '/' ) != KErrNotFound ) {
                    contentType = str.AllocLC();
                }
                else if( str.Find( KCharsetKey ) == 0 ) {
                    encoding = (str.Mid( KCharsetKey().Length()).AllocLC());
                }
                // headersStr = charset=iso-8859-7,%be%fg%be
                headersStr.Set( headersStr.Mid( semicolonPos + 1 ) );
            }
        }
        while( semicolonPos != KErrNotFound );
        // simulate header strings
        // content type: text/html
        // charset: iso-8859-1
        if (contentType == NULL) {
            contentType = KDefaultContentType().AllocLC();
        }
        if (encoding == NULL) {
            encoding = KDefaultCharset().AllocLC();
        }

        // make sure there is stuff to decode
        if( ( colonPos + 1 ) < urlPtr8.Length() ) {
            TBool ok( EFalse );
            TPtrC8 data( urlPtr8.Mid( colonPos + 1 ) );
            // decode body
            if( base64 ) {
                // allocate twice as big buffer for decoded string
                body = HBufC8::NewLC( 2 * data.Length() );
                TPtr8 decodedBody( body->Des() );
                // urlPtr8
                //TImCodecB64 codec;
                //codec.Initialise();
                TBase64 codec;
                ok = codec.Decode( data, decodedBody );
            }
            if( !ok ) { // if not base64, simple copy
                if( !body ) {
                    body = HBufC8::NewLC( data.Length() );
                    body->Des().Copy( data );
                }
            }
        }
    }
    m_maxSize = body->Length();
}

void DataConnection::cancel()
{
    if (m_scheduler) {
        m_scheduler->Cancel();
        delete m_scheduler;
        m_scheduler = NULL;
    }
}

void DataConnection::download(WebCore::ResourceHandle* handle,
                              const WebCore::ResourceRequest& request,
                              const WebCore::ResourceResponse& response)
{
    __ASSERT_ALWAYS(EFalse, User::Panic(_L("Resource Loader"), KErrArgument));
}

TInt DataConnection::sendResponseCb(TAny* aPtr)
{
    DataConnection* self = static_cast<DataConnection*>(aPtr);
    TRAPD(error, self->sendResponseL());
    if (error != KErrNone) {
        CResourceHandleManager::self()->receivedFinished(self->m_handle, error, self);
    }
    return KErrNone;
}

void DataConnection::sendResponseL()
{
    m_scheduler->Cancel();
    delete m_scheduler;
    m_scheduler = NULL;

    HBufC8* contentType = NULL;
    HBufC8* encoding = NULL;
    HBufC8* body = NULL;
    parseUrlLC(contentType, encoding, body);

    ResourceResponse response(m_handle->request().url().des(), contentType->Des(), body->Length(), encoding->Des(), String() );
    CResourceHandleManager::self()->receivedResponse(m_handle, response, this);
    CResourceHandleManager::self()->receivedData(m_handle, body->Des(), body->Length(), this);
    CResourceHandleManager::self()->receivedFinished(m_handle, KErrNone, this);
    CleanupStack::PopAndDestroy(3); // contentType, encoding, body
    derefHandle();
    }

// end of file
