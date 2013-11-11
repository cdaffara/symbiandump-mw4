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

#ifndef __PLATFORMFONTCACHE_H__
#define __PLATFORMFONTCACHE_H__

#include <gdi.h>
#include <wtf/Vector.h>
#include <wtf/HashSet.h>

class CWsScreenDevice;
class TFontSpec;
class CFont;
class AtomicString;

namespace WebCore
{
class FontDescription;
class PlatformFontCache
{
public:
    struct ZoomedSpec {
        TFontSpec m_spec;
        int m_zoom;
        CFont* m_font;
        ZoomedSpec() : m_zoom(100), m_font(0)                                                           {}
        ZoomedSpec(TFontSpec spec, int zoom, CFont* font) : m_spec(spec), m_zoom(zoom), m_font(font)    {}
    };

    PlatformFontCache() : iScreenDevice(0), iSupportedNames(0), iTypefaceCount(0), iFontZoomFactor(100) {}
    ~PlatformFontCache();

    void Initialize();
    const AtomicString& SystemFontFamily( const AtomicString& familyName );
    const AtomicString& DeviceDefaultFontFamilies();
    CFont* CreateFont( const ZoomedSpec& spec );
    void ReleaseFont(CFont* );

    // font-size corrected zooming
    CFont* zoomedFont(const FontDescription&, int zoom);
    int fontZoomFactor() const                  { return iFontZoomFactor; }
    void setFontZoomFactor(int zoom);
    TFontSpec fontSpecInTwips(const FontDescription& fontDescription);

    struct ZoomedSpecHash {
        static unsigned hash(const ZoomedSpec&);
        static bool equal(const ZoomedSpec& a, const ZoomedSpec& b);
    };

    struct ZoomedSpecKeyTraits : WTF::GenericHashTraits<ZoomedSpec> {
        static const bool emptyValueIsZero = false;
        static const bool needsDestruction = false;
        static ZoomedSpec deletedValue() { return ZoomedSpec(TFontSpec(), 100, 0); }
        static ZoomedSpec emptyValue() { return ZoomedSpec(TFontSpec(), 100, 0); }
    };

    struct FontTraits : WTF::GenericHashTraits<CFont*> {
        static const bool emptyValueIsZero = true;
        static const bool needsDestruction = false;
        static CFont* deletedValue() { return 0; }
    };

private:
    CWsScreenDevice* iScreenDevice;     // not owned
    AtomicString*    iSupportedNames;   // owned
    TInt             iTypefaceCount;
    AtomicString     iDeviceDefaultFont;
    TInt             iFontZoomFactor;
    // RVCT compiler doesn't like hashmap/hashset defined in this way, why???
    //WTF::HashMap<ZoomedSpec, CFont*, ZoomedSpecHash, ZoomedSpecKeyTraits, FontTraits> iFontCache;
    //WTF::HashSet<ZoomedSpec, ZoomedSpecHash, ZoomedSpecKeyTraits> iFontCache;
    WTF::Vector<ZoomedSpec> iFontCache;
};

static inline bool operator==(const PlatformFontCache::ZoomedSpec& a, const PlatformFontCache::ZoomedSpec& b) { return a.m_spec == b.m_spec && a.m_zoom == b.m_zoom; }
static inline bool operator!=(const PlatformFontCache::ZoomedSpec& a, const PlatformFontCache::ZoomedSpec& b) { return !(a==b); }

}
#endif// !__PLATFORMFONTCACHE_H__
