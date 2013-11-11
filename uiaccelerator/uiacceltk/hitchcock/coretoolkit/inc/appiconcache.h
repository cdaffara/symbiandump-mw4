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


#ifndef __APPICONCACHE_H__
#define __APPICONCACHE_H__

#include <e32base.h>
#include <e32cmn.h>
#include <fbs.h>

NONSHARABLE_STRUCT(TRasterizedBitmap)
    {
    TRasterizedBitmap(TInt64 aBitmapSerial, CFbsBitmap* aBitmap) : iBitmapSerial(aBitmapSerial), iBitmap(aBitmap)
        {
        }
    TRasterizedBitmap() : iBitmapSerial(0), iBitmap(NULL) {}
    TInt64 iBitmapSerial;
    CFbsBitmap* iBitmap;
    };


NONSHARABLE_CLASS(CAppIconCache) : public CBase
    {
    public:
        static CAppIconCache* NewL(TInt aCacheLimit);
        virtual ~CAppIconCache();
        
        IMPORT_C CFbsBitmap* Find(TInt64 aSerialNumber, TSize aSize);
        IMPORT_C TInt Insert(TRasterizedBitmap& aBitmap);
        IMPORT_C void Clear();
        
    protected:
        CAppIconCache(TInt aCacheLimit);
        void ConstructL();
    
    private:
        RArray<TRasterizedBitmap> iCache;
        TInt iCacheLimit;
    };



#endif
