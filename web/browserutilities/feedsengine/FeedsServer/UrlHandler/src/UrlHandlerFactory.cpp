/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A factory class used to create UrlHandlers.
*
*/



#include <uri16.h>
#include <uricommon.h>

#include "FileHandler.h"
#include "HttpHandler.h"
#include "UrlHandler.h"
#include "UrlHandlerFactory.h"


// -----------------------------------------------------------------------------
// UrlHandlerFactory::NewL
// 
// Given the schema of the url it creates an url handler instance
// capable of fetching the url.
// -----------------------------------------------------------------------------
//
CUrlHandler* UrlHandlerFactory::NewUrlHandlerL(CHttpConnection& aHttpConnection,
        const TDesC& aUrl)
{
    _LIT(KHttp, "http");
    _LIT(KHttps, "https");
    _LIT(KFile, "file");
    
    CUrlHandler*   handler = NULL;
    TUriParser16   uriParser;

    // Get the url's scheme.
    User::LeaveIfError(uriParser.Parse(aUrl));
    const TPtrC16 schema(uriParser.Extract(EUriScheme));

    // Create a http handler. 
    if ((schema.CompareF(KHttp) == 0) || (schema.CompareF(KHttps) == 0))
        {
        handler = CHttpHandler::NewL(aHttpConnection);
        }

    // Create a file handler.
    else if (schema.CompareF(KFile) == 0)
        {
        handler = CFileHandler::NewL();
        }

    // Otherwise leave
    else
        {
        User::Leave(KErrBadName);
        }

    return handler;
}
