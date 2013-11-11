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
#include "WebIconDatabase.h"

#include "IconDatabase.h"
#include "MaskedBitmap.h"
#include "Image.h"
#include "PlatformString.h"
#include "KURL.h"
#include "IntRect.h"
#include <gulicon.h>
#include <fbs.h>
#include "WebKitUtilsSqlite.h"

using namespace WebCore;

_LIT(KDBFile, "c:\\private\\%08x\\");

WebIconDatabase::WebIconDatabase()
{
}

WebIconDatabase::~WebIconDatabase()
{
    iconDatabase()->close();
    sqlite3SymbianLibFinalize();
}

void WebIconDatabase::openSharedDatabase()
{
    if (!iconDatabase()->isOpen()) {
        sqlite3SymbianLibInit();
        // tot fixme: get process path from some utilities
        iconDatabase()->setEnabled(true);
	    RProcess myProcess;	
	    TBuf <256>dirName;    
        dirName.Format(KDBFile, myProcess.Identity());
        String s(dirName.Ptr(), dirName.Length());
        iconDatabase()->open(s);
    }
}

CGulIcon* WebIconDatabase::iconForPageURL(const TDesC8& url)
{
    // make sure url has a trailing /
    KURL _url(url);
    iconDatabase()->retainIconForPageURL(String(_url));
    Image* image = iconDatabase()->iconForPageURL(String(_url), IntSize(TSize(16, 16)));
    if (image) {
        CMaskedBitmap* maskedBitmap = image->getMaskedBitmap();
        if (!maskedBitmap)
            return NULL;
        CGulIcon* icon = NULL;
        CFbsBitmap* bitmap = new CFbsBitmap();
        CFbsBitmap* mask = new CFbsBitmap();

        if (bitmap && mask) {
            int err( BitmapUtil::CopyBitmap(maskedBitmap->Bitmap(), *bitmap));
            int errMask( BitmapUtil::CopyBitmap(maskedBitmap->Mask(), *mask));
            if (err == KErrNone && errMask == KErrNone) {
                TRAP_IGNORE(icon = CGulIcon::NewL());
                if (icon) {
                    icon->SetBitmap( bitmap );
                    icon->SetMask( mask );
                }
            }
        }
        return icon;
    }
    return NULL;
}

void WebIconDatabase::retainIconForURL(const TDesC8& url)
{   
    iconDatabase()->retainIconForPageURL(String(url));
}

void WebIconDatabase::releaseIconForURL(const TDesC8& url)
{   
    iconDatabase()->releaseIconForPageURL(String(url));
}


