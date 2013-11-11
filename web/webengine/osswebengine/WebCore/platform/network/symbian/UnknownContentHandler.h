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

#ifndef __UNKNOWNCONTENTHANDLER_H__
#define __UNKNOWNCONTENTHANDLER_H__

#include <e32base.h>
#include <http/rhttptransaction.h>
#include "HttpConnection.h"
#include "ResourceResponse.h"

class UnknownContentHandler
{
public:
    /**
    */
    virtual ~UnknownContentHandler();

    static UnknownContentHandler* NewL(HttpConnection* connection, WebCore::ResourceResponse& response);

    static bool isUnknownContent(WebCore::ResourceResponse& response);
    
    void updateContentTypeL(TPtrC8& chunkPtr);

    WebCore::ResourceResponse* handOffResponse();

private:
    /**
    */
    UnknownContentHandler(HttpConnection* connection);
    void ConstructL(WebCore::ResourceResponse& response);

private: // data
    HttpConnection* m_connection;
    WebCore::ResourceResponse* m_response;

};


#endif // __UNKNOWNCONTENTHANDLER_H__
// end of file
