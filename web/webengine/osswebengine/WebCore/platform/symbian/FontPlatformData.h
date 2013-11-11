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

#ifndef __FONTPLATFORMDATA_H__
#define __FONTPLATFORMDATA_H__

#include "StringImpl.h"

class CFont;

namespace WebCore {

class FontDescription;

class FontPlatformData
{
public:
    struct Deleted {};

    // Used for deleted values in the font cache's hash tables.
    FontPlatformData(Deleted) : iFont(reinterpret_cast<CFont*>(-1))                {}

    FontPlatformData() : iFont(0)                       {}
    FontPlatformData( CFont* aFont ) : iFont( aFont )   {}
    ~FontPlatformData()                                 {}

    operator const CFont& () const                      { return *iFont; }
    CFont* Font()                                       { return iFont; }

    bool operator==( const FontPlatformData& other ) const   { return iFont == other.iFont; }
    unsigned hash() const    { return StringImpl::computeHash((UChar*)(iFont), sizeof(CFont*) / sizeof(UChar)); }

private:
    CFont* iFont;
};

}

#endif// !__FONTPLATFORMDATA_H__
