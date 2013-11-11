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
#include "Font.h"
#include "TextStyle.h"
#include "FontData.h"
#include "FontFallbackList.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "WebCoreGraphicsContext.h"
#include <e32std.h>
#include <bitstd.h>
#include <gdi.h>
#include "StaticObjectsContainer.h"
#include "PictographSymbian.h"
#include "ZoomFactor.h"
#include "PlatformFontCache.h"

namespace WebCore {

const UChar UnicodeZeroWidthSpace = 0x200b;

static void notSupported() { __ASSERT_ALWAYS( 0, User::Panic( _L("Font:"), 1 ) ); }

void Font::drawGlyphs(GraphicsContext* graphicsContext, const FontData* font, const GlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
    notSupported();
}

FloatRect Font::selectionRectForComplexText(const TextRun& run, const TextStyle& style, const IntPoint& point, int h, int from, int to) const
{
    float b = 0;
    if (from>0) {
        TextRun begin(run.characters(), from);
        b = floatWidthForComplexText(begin, style);
    }

    TextRun end(run.characters() + from, to - from);
    float w = floatWidthForComplexText(end, style);

    FloatRect rect(b + point.x(), point.y(), w, h);

    return rect;
}

void Font::drawComplexText(GraphicsContext* graphicsContext, const TextRun& run, const TextStyle& style, const FloatPoint& point, int from, int to) const
{
    PlatformFontCache* cache = StaticObjectsContainer::instance()->fontCache();
    CFont* font = cache->zoomedFont(m_fontDescription, cache->fontZoomFactor());
    // RenderBlock::layoutColumns create a 0 platform context to do a faked paint
    // during layout - weird stuff :)
    if (!graphicsContext->platformContext())
        return;

    CFbsBitGc& bitgc = graphicsContext->platformContext()->gc();
    Color c = graphicsContext->fillColor();
    bitgc.SetPenColor( TRgb( c.red(), c.green(), c.blue(), c.alpha() ) );
    bitgc.UseFontNoDuplicate( static_cast<const CFbsBitGcFont*>(font) );

    TPoint startPos = point;
    if (from) {

        // the text run before the selection
        TextRun leftRun(run.characters(), from);
        // the selection start point
        startPos.iX += floatWidthForComplexText(leftRun, style);
    }

    TPtrC str( (const TUint16*)(run.characters() + from), to - from);

    TInt numSpaces(0);
    TInt indexOfFirstNonRegularSpace(-1);
    TInt strLength(str.Length());

    // a) search for nbsp and similar
    // b) count the spaces for word spacing
    for( TInt n = 0; n<strLength; ++n) {

        if ( str[n] == ' ' ) {
            numSpaces++;
        }
        else if( TChar(str[n]).IsSpace() ) {

            if (indexOfFirstNonRegularSpace==-1) {
                indexOfFirstNonRegularSpace = n;
            }
            numSpaces++;
            if( m_wordSpacing==0 || style.padding()==0 ) {
                break;
            }
        }
    }

    // letter spacing
    bitgc.SetCharJustification(m_letterSpacing*str.Length(),str.Length());
    // word spacing
    bitgc.SetWordJustification(m_wordSpacing*numSpaces + style.padding(), numSpaces);

#if PLATFORM(SYMBIAN)
    TLanguage uilang = User::Language();
    if(uilang == ELangKorean)
    {
        startPos.iY -= 1;
    }
#endif
    // see if we need a temporary buffer
    if( indexOfFirstNonRegularSpace > -1 || isSmallCaps() || style.rtl()){
        HBufC* text = 0;
        text = str.Alloc();
        if (text){

            TPtr newStr(text->Des());
            if (isSmallCaps()) {
                // proper small caps implementation would use smaller font, but that get complicated
                // just upper case for now
                newStr.UpperCase();
            }

            // if we have non-regular spaces (nbsp, tab, etc.) we need to make a copy
            // and replace them with regular spaces. otherwise they show up as boxes.
            if (indexOfFirstNonRegularSpace > -1) {
                for(; indexOfFirstNonRegularSpace<strLength; ++indexOfFirstNonRegularSpace ) {
                    if( TChar(newStr[indexOfFirstNonRegularSpace]).IsSpace()) {
                       if (newStr[indexOfFirstNonRegularSpace] != UnicodeZeroWidthSpace) {
                           newStr[indexOfFirstNonRegularSpace] = ' ';
                       }
                    }
                }
            }

            if (style.rtl()) {
                TBidiText* bidiText = TBidiText::NewL( newStr, 1, TBidiText::ERightToLeft );
                bidiText->WrapText(xForm(width(run)), *font );
                bitgc.DrawText( bidiText->DisplayText(),xForm(startPos));
                delete bidiText;
            }
            else{
                bitgc.DrawText( newStr, xForm(startPos) );
            }

            if (graphicsContext && StaticObjectsContainer::instance()->pictograph())
                StaticObjectsContainer::instance()->pictograph()->DrawPictographsInText(graphicsContext->platformContext(), bitgc, *font, newStr, startPos );

            delete text;
        }

    }else {
        bitgc.DrawText( str, xForm(startPos) );
        if (graphicsContext && StaticObjectsContainer::instance()->pictograph())
            StaticObjectsContainer::instance()->pictograph()->DrawPictographsInText(graphicsContext->platformContext(), bitgc, *font, str, startPos );
    }

    bitgc.DiscardFont();
}

inline float floatWidthForComplexTextSymbian(const Font* f, const TextRun& run,  const TextStyle& style, int maxWidth, int& chars){
    const CFont& font( f->primaryFont()->platformData() );
    TPtrC str( (const TUint16*)(run.characters()), run.length());

    TInt numSpaces(0);
    TInt indexOfFirstNonRegularSpace(-1);
    TInt strLength(str.Length());
    // count the spaces for word spacing
    // a) search for nbsp and similar
    // b) count the spaces for word spacing
    for( TInt n = 0; n<strLength; ++n)
        {
        if ( str[n] == ' ' )
                {
                numSpaces++;
                }
        else if( TChar(str[n]).IsSpace() )
            {
            if (indexOfFirstNonRegularSpace==-1)
                {
                indexOfFirstNonRegularSpace = n;
                }
            numSpaces++;
            if ( f->wordSpacing() == 0 || style.padding() == 0 )
                break;
            }
        }

    CFont::TMeasureTextInput input;
    input.iCharJustExcess = str.Length() * f->letterSpacing();
    input.iCharJustNum = str.Length();
    input.iWordJustExcess = numSpaces * f->wordSpacing();
    input.iWordJustNum = numSpaces;
    if (maxWidth>0)
        input.iMaxAdvance = maxWidth;
    CFont::TMeasureTextOutput output;
    //input.iMaxAdvance = aMaxAdvance;

    // see if we need a temporary buffer
    if( indexOfFirstNonRegularSpace > -1 || f->isSmallCaps() ){
        HBufC* text = str.Alloc();
        // don't bother to reverse order for right-to-left, assume the length will be the same
        if (text){
            TPtr newStr(text->Des());
            if ( f->isSmallCaps() )
                {
                // proper small caps implementation would use smaller font, but that get complicated
                // just upper case for now
                newStr.UpperCase();
                }
            // if we have non-regular spaces (nbsp, tab, etc.) we need to make a copy
            // and replace them with regular spaces. otherwise they show up as boxes.
            if (indexOfFirstNonRegularSpace > -1)
                {
                for(; indexOfFirstNonRegularSpace<strLength; ++indexOfFirstNonRegularSpace )
                    {
                    if( TChar(newStr[indexOfFirstNonRegularSpace]).IsSpace() )
                        {
                        newStr[indexOfFirstNonRegularSpace] = ' ';
                        }
                    }
                }
            // FIXME: should use smallcap font, which has smaller font size than the current font
            TInt w = font.MeasureText(*text,&input,&output);
            delete text;
            chars = output.iChars;
            return w;
            }
        }
    TInt w = font.MeasureText(str,&input,&output);
    chars = output.iChars;
    return w;
}

float Font::floatWidthForComplexText(const TextRun& run, const TextStyle& style) const
{
    int chars;
    return floatWidthForComplexTextSymbian(this, run, style, -1, chars);
}


int Font::offsetForPositionForComplexText(const TextRun& run, const TextStyle& style, int x, bool includePartialGlyphs) const
{
    int chars;
    floatWidthForComplexTextSymbian(this, run, style, x, chars);
    return chars;
}

}
