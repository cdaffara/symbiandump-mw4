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
#include "FontCache.h"
#include "FontPlatformData.h"
#include "Font.h"
#include "StaticObjectsContainer.h"
#include "PlatformFontCache.h"
#include <../bidi.h>            // work around for multiple bidi.h files
#include "ZoomFactor.h"
#include <e32base.h>
#include <e32std.h>
#include <w32std.h>
#include <coemain.h>
#include <fbs.h>
#include <featmgr.h>
#include <languages.hrh>

#include <AknFontAccess.h>
#include <AknUtils.h>
#include <AknLayoutFont.h>
#include <AknFontSpecification.h>

// CONSTANTS
_LIT( KPlatformDefaultFontFamily,   "S60 WebKit" ); 

namespace WebCore
{

void FontCache::platformInit()
{
    // don't support Glphy caching in S60, so force to use the complex path
    Font::setCodePath( Font::Complex );
    StaticObjectsContainer::instance()->fontCache()->Initialize();
}

const FontData* FontCache::getFontDataForCharacters(const Font& font, const UChar* characters, int length)
{
    // this function shouldn't be called.
    __ASSERT_ALWAYS( 0, User::Panic(_L("FontCache:"), 0) );
    return 0;
}

FontPlatformData* FontCache::getSimilarFontPlatformData(const Font& font)
{
    return 0;
}

FontPlatformData* FontCache::getLastResortFallbackFont(const Font& font)
{
   // the default fallback font
   return getCachedFontPlatformData(font.fontDescription(), StaticObjectsContainer::instance()->fontCache()->DeviceDefaultFontFamilies());
}

FontPlatformData* FontCache::getDeviceDefaultFont(float size)
{
    FontDescription fd;
    fd.setComputedSize(size);
    return getCachedFontPlatformData(fd, StaticObjectsContainer::instance()->fontCache()->DeviceDefaultFontFamilies());
}

FontPlatformData* FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& family)
{
    PlatformFontCache* platformCache = StaticObjectsContainer::instance()->fontCache();
    return new FontPlatformData(platformCache->zoomedFont(fontDescription, 100));
}

const AtomicString& FontCache::systemFontFamilyName( const AtomicString& familyName )
{
    return StaticObjectsContainer::instance()->fontCache()->SystemFontFamily( familyName );
}

void PlatformFontCache::Initialize()
    {
    iScreenDevice = CCoeEnv::Static()->ScreenDevice();
    iFontCache.clear();

    // search for scalable fonts only to make lookups faster
    TInt count(iScreenDevice->NumTypefaces());
    for (TInt n=0; n<count; ++n)
        {
        TTypefaceSupport typeface;
        iScreenDevice->TypefaceSupport(typeface, n);
        if (typeface.iIsScalable)
            {
            // count the scalable typefaces
            ++iTypefaceCount;
            }
        }

    // populate typeface array
    TInt i=0;
    iSupportedNames = new AtomicString[iTypefaceCount];

    for (TInt n=0; n<count; ++n)
        {
        TTypefaceSupport typeface;
        iScreenDevice->TypefaceSupport(typeface, n);
        if (typeface.iIsScalable)
            {
            iSupportedNames[i] = typeface.iTypeface.iName;
            ++i;
            }

        }

}

PlatformFontCache::~PlatformFontCache()
{
    Vector<ZoomedSpec>::iterator it = iFontCache.begin();
    Vector<ZoomedSpec>::iterator end = iFontCache.end();
    for(;it != end; ++it) {
        ReleaseFont( it->m_font );
        it->m_font = 0;
    }        
    iFontCache.clear();
    delete [] iSupportedNames;
    iDeviceDefaultFont = "";
}

const AtomicString& PlatformFontCache::SystemFontFamily( const AtomicString& familyName )
{
    // first check if we support this font
    for (TInt n=0; n<iTypefaceCount; ++n)
    {
        if (iSupportedNames[n] == familyName )
        {
            return iSupportedNames[n];
        }
    }

    return DeviceDefaultFontFamilies();
}

const AtomicString& PlatformFontCache::DeviceDefaultFontFamilies()
    {
    TPtrC font(KPlatformDefaultFontFamily);
    iDeviceDefaultFont = font;
    return iDeviceDefaultFont;
    }

CFont* PlatformFontCache::CreateFont(const ZoomedSpec& spec)
{    
    CFont* font = 0;
    
    Vector<ZoomedSpec>::const_iterator it = iFontCache.begin();
    Vector<ZoomedSpec>::const_iterator end = iFontCache.end();
    for (;it != end; ++it) {
        if ((*it) == spec) {
            font = it->m_font; 
            return font;
        }
    }
    
    
    if (spec.m_spec.iTypeface.iName == KPlatformDefaultFontFamily) { 
        // fall back to platform default fonts by using TAknFontFamily 
        font = AknFontAccess::GetClosestFont(*iScreenDevice, spec.m_spec.iFontStyle, spec.m_spec.iHeight * spec.m_zoom/100, (AknFontAccess::TAknFontFamily)0); 
    } 
    else { 
        font = AknFontAccess::GetClosestFont(*iScreenDevice, spec.m_spec.iFontStyle, spec.m_spec.iHeight * spec.m_zoom/100, spec.m_spec.iTypeface.iName); 
    } 
        
    
    if (font) {
        ZoomedSpec newSpec(spec);
        newSpec.m_font = font;
        iFontCache.append(newSpec);
    }   
	 
    return font;
}

void PlatformFontCache::ReleaseFont(CFont* font)
{
    iScreenDevice->ReleaseFont(font);
}

void PlatformFontCache::setFontZoomFactor(int zoom)
{
    // adjust the zoom factor based on fonts
    iFontZoomFactor = zoom;
}

TFontSpec PlatformFontCache::fontSpecInTwips(const FontDescription& fontDescription)
{    
    TInt fSize = fontDescription.computedPixelSize();

    if (fSize == 0) fSize = 12;
    TInt twipSize = iScreenDevice->VerticalPixelsToTwips(fSize);

    // convert to platform-supported font family
    TPtrC fPtr( SystemFontFamily( fontDescription.family().family() ) );
    

    TFontSpec fontSpec(fPtr, twipSize);
    fontSpec.iFontStyle.SetStrokeWeight(fontDescription.bold() ? EStrokeWeightBold : EStrokeWeightNormal);
    fontSpec.iFontStyle.SetPosture(fontDescription.italic() ? EPostureItalic : EPostureUpright);
    fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap); // enable anti-aliasing
    
    if (fontDescription.bold() && fPtr==KPlatformDefaultFontFamily) {
        fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    }

    return fontSpec;
}


CFont* PlatformFontCache::zoomedFont(const FontDescription& fontDescription, int zoom)
{
    ZoomedSpec spec(fontSpecInTwips(fontDescription), zoom, 0);
    return CreateFont(spec);
}

}

