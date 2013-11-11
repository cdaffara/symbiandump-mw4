/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "appiconcache.h"

CAppIconCache::~CAppIconCache()
    {
    Clear();
    }
    
CAppIconCache::CAppIconCache(TInt aCacheLimit) : iCache(32), iCacheLimit(aCacheLimit)
    {
    }

CAppIconCache* CAppIconCache::NewL(TInt aCacheLimit)
    {
    CAppIconCache* self = new (ELeave) CAppIconCache(aCacheLimit);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


void CAppIconCache::ConstructL()
    {
    }

EXPORT_C CFbsBitmap* CAppIconCache::Find(TInt64 aSerialNumber, TSize aSize)
    {
    TRasterizedBitmap rb;
    for (TInt index = 0; index < iCache.Count(); index++)
        {
        if (iCache[index].iBitmapSerial == aSerialNumber && iCache[index].iBitmap->SizeInPixels() == aSize)
            {
            // if we found a match, move the entry to the beginning of
            // the cache
            rb = iCache[index];
            iCache.Remove(index);
            iCache.Insert(rb, 0);
            return rb.iBitmap;
            }
        }
    return NULL;   
    }
    
EXPORT_C TInt CAppIconCache::Insert(TRasterizedBitmap& aBitmap)
    {
    TSize bitmapsize = aBitmap.iBitmap->SizeInPixels();
    // assume 32bpp for the icon
    TInt bitmapBytes = bitmapsize.iWidth*bitmapsize.iHeight*4;
    if (bitmapBytes > 128*128*4)
        {
        return KErrTooBig;
        }
        
    if (iCache.Count() > iCacheLimit)
        {
        // cache limit exceeded, remove
        // the least recently used entry
        delete iCache[iCache.Count()-1].iBitmap;
        iCache.Remove(iCache.Count()-1);
        }
    return iCache.Insert(aBitmap, 0);
    }

EXPORT_C void CAppIconCache::Clear()
    {
    for (TInt index = 0; index < iCache.Count(); index++)
        {
        delete iCache[index].iBitmap;
        }
    iCache.Reset();
    }
