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


#include "config.h"
#include "MimeTypeRegistry.h"
#include <apgcli.h>

namespace WebCore {

struct ExtensionMap {
    const char* extension;
    const char* mimeType;
};

static const ExtensionMap extensionMap [] = {
    { "bmp", "image/bmp" },
    { "gif", "image/gif" },
    { "html", "text/html" },
    { "ico", "image/x-icon" },   
    { "jpeg", "image/jpeg" },
    { "jpg", "image/jpeg" },
    { "js", "application/x-javascript" },
    { "pdf", "application/pdf" },
    { "png", "image/png" },
    { "rss", "application/rss+xml" },
    { "svg", "image/svg+xml" },
    { "text", "text/plain" },
    { "txt", "text/plain" },
    { "xbm", "image/x-xbitmap" },
    { "xml", "text/xml" },
    { "xsl", "text/xsl" },
    { "xhtml", "application/xhtml+xml" },
    { 0, 0 }
};
    
String MIMETypeRegistry::getMIMETypeForExtension(const String &ext)
{
    String s = ext.lower();
    const ExtensionMap *e = extensionMap;
    while (e->extension) {
        if (s == e->extension)
            return e->mimeType;
        ++e;
    }
    // unknown, let's just assume plain text
    return "text/plain";
}

String MIMETypeRegistry::getDefaultMIMETypeForPath(const String& path)
{
    RApaLsSession apaSession;
    HBufC* mimeType = NULL;
    if (apaSession.Connect() == KErrNone) {
        TUid uid;
        TDataType dataType;
        // Ask the application architecture to find the file type.
        if (apaSession.AppForDocument(path.des(), uid, dataType ) == KErrNone) {
            // If no mime-type yet, then use the default.
            if ( dataType.Des8().Length()) {
                mimeType = HBufC::New(dataType.Des8().Length());
                if (mimeType) {
                    mimeType->Des().Copy(dataType.Des8());
                }
            }
        }
        apaSession.Close();
    }
    String ret;
    if (mimeType) {
        ret = String(*mimeType);
        delete mimeType;
        mimeType = NULL;
    }
    else {
        ret = "application/octet-stream";
    }
    return ret;
}

}
