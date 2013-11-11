/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UnknownContentHandler
*
*/


#include "UnknownContentHandler.h"
#include "HttpConnection.h"
#include "PlatformString.h"
#include <uri8.h>
#include <apgcli.h>
using namespace WebCore;


UnknownContentHandler::UnknownContentHandler(HttpConnection* connection)
{
    m_connection = connection;
    m_response = NULL;
}

UnknownContentHandler::~UnknownContentHandler()
{
    delete m_response;
}

UnknownContentHandler* UnknownContentHandler::NewL(HttpConnection* connection, WebCore::ResourceResponse& response)
{
    UnknownContentHandler* self = NULL;
    self = new(ELeave) UnknownContentHandler(connection);
    CleanupStack::PushL(self);
    self->ConstructL(response);
    CleanupStack::Pop(self);
    return self;
}

bool UnknownContentHandler::isUnknownContent(ResourceResponse& response)
{
    const String& mimeType = response.mimeType();
    const KURL& url = response.url();

    if( equalIgnoringCase(mimeType, "application/octet-stream")) {
        TUriParser8 parser;
        if( parser.Parse(url.des()) == KErrNone ) {
            TPtrC8 path = parser.Extract( EUriPath );
            int length = path.Length();
            if( length > 4 ) {
                if (path.Find(_L8(".html")) != KErrNotFound || path.Find(_L8(".htm")) != KErrNotFound) {
                    response.setMimeType("text/html");
                    return false; // good chance of being html content
                }
                if (path.Find(_L8(".xhtml")) != KErrNotFound ) {
                    response.setMimeType("application/xhtml+xml");
                    return false; // good chance of being xhtml content
                }
                if (path.Find(_L8(".wml")) != KErrNotFound) {
                    response.setMimeType("text/vnd.wap.wml");
                    return false; // good chance of being wml content
                }
            }
        }
        return true; // could be html
    }
    return false;
}


void UnknownContentHandler::updateContentTypeL(TPtrC8& chunkPtr)
{
    RApaLsSession apaSession;
    TInt ret;
    User::LeaveIfError( apaSession.Connect() );
    TDataRecognitionResult dataType;
    ret = apaSession.RecognizeData( KNullDesC, chunkPtr, dataType );
    apaSession.Close();
    
    if( ret == KErrNone &&
      ( dataType.iConfidence == CApaDataRecognizerType::ECertain ) ||
      ( dataType.iConfidence == CApaDataRecognizerType::EProbable ) ) {       
        // "text/plain" shouldn't be treated as a strong type
        if (dataType.iDataType.Des8().FindF(_L8("text/plain")) == KErrNotFound) {
            m_response->setMimeType(dataType.iDataType.Des8());
        }
    }
}

ResourceResponse* UnknownContentHandler::handOffResponse()
{
    ResourceResponse* response = m_response;
    m_response = NULL;
    return response;
}

void UnknownContentHandler::ConstructL(ResourceResponse& response)
{
    m_response = new(ELeave) ResourceResponse(response.url(), response.mimeType(), response.expectedContentLength(), response.textEncodingName(), response.suggestedFilename());

}


// end of file
