/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "Font.h"
#include "FontData.h"
#include "FontCache.h"
#include "FloatRect.h"
#include "FontDescription.h"
#include "StaticObjectsContainer.h"
#include "PlatformFontCache.h"
#include <wtf/MathExtras.h>
#include <e32base.h>
#include <e32std.h>
#include <fbs.h>

// CONSTATNS
_LIT(KX,"x");
_LIT(KSpace, " ");

#define NOT_IMPLEMENTED __ASSERT_ALWAYS( 0, User::Panic( _L("FontData:"), 0 ) )

namespace WebCore
{

void FontData::platformInit()
{
    const CFont& font( m_font );
    m_ascent = font.AscentInPixels() + 2;
    m_descent = font.DescentInPixels() - 2;
    m_lineGap = font.FontLineGap();
    m_lineSpacing = m_ascent + m_descent + 1;

    CFont::TMeasureTextOutput output;
    font.MeasureText( TPtrC(KX), 0, &output );
    m_xHeight = output.iMaxGlyphSize.iHeight;

    font.MeasureText( TPtrC(KSpace), 0, &output );
    m_spaceWidth = output.iMaxGlyphSize.iWidth;
    m_adjustedSpaceWidth = m_spaceWidth;
}

void FontData::platformDestroy()
{
    // Do Nothing. m_font is owned by the Font Cache 
    // and maybe shared with other FontData instances
}

FontData* FontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_smallCapsFontData) {
        // FIXME: we just use the current font for smallcap font
        m_smallCapsFontData = new FontData( m_font );
    }
    return m_smallCapsFontData;
}

bool FontData::containsCharacters(const UChar* characters, int length) const
{
    return true;
}

void FontData::determinePitch()
{
    NOT_IMPLEMENTED;
}

float FontData::platformWidthForGlyph(Glyph glyph) const
{
    NOT_IMPLEMENTED;
    return -1;
}

}
