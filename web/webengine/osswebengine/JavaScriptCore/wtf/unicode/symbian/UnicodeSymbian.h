// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2006 George Staikos <staikos@kde.org>
 *  Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef KJS_UNICODE_SYMBIAN_H
#define KJS_UNICODE_SYMBIAN_H

#include <config.h>
#include <stdint.h>
#include <e32cmn.h>

#include <ctype.h>
#include <unicode/uchar.h>

//typedef uint16_t UChar;
//typedef uint32_t UChar32;

// some defines from ICU needed one or two places

#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)
#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((UChar32)(lead)<<10UL)+(UChar32)(trail)-U16_SURROGATE_OFFSET)

#define U16_LEAD(supplementary) (UChar)(((supplementary)>>10)+0xd7c0)
#define U16_TRAIL(supplementary) (UChar)(((supplementary)&0x3ff)|0xdc00)

#define U_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define U16_IS_SURROGATE(c) U_IS_SURROGATE(c)
#define U16_IS_SURROGATE_LEAD(c) (((c)&0x400)==0)

#define U16_NEXT(s, i, length, c) { \
    (c)=(s)[(i)++]; \
    if(U16_IS_LEAD(c)) { \
        uint16_t __c2; \
        if((i)<(length) && U16_IS_TRAIL(__c2=(s)[(i)])) { \
            ++(i); \
            (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
        } \
    } \
}

#define U_MASK(x) ((uint32_t)1<<(x))

#include "symbian/UnicodeCategory.h"


namespace WTF {
  namespace Unicode {

    enum Direction {
      LeftToRight = TChar::ELeftToRight,
      RightToLeft = TChar::ERightToLeft,
      EuropeanNumber = TChar::EEuropeanNumber,
      EuropeanNumberSeparator = TChar::EEuropeanNumberSeparator,
      EuropeanNumberTerminator = TChar::EEuropeanNumberTerminator,
      ArabicNumber = TChar::EArabicNumber,
      CommonNumberSeparator = TChar::ECommonNumberSeparator,
      BlockSeparator = TChar::EParagraphSeparator,
      SegmentSeparator = TChar::ESegmentSeparator,
      WhiteSpaceNeutral = TChar::EWhitespace,
      OtherNeutral = TChar::EOtherNeutral,
      LeftToRightEmbedding = TChar::ELeftToRightEmbedding,
      LeftToRightOverride = TChar::ELeftToRightOverride,
      RightToLeftArabic = TChar::ERightToLeftArabic,
      RightToLeftEmbedding = TChar::ERightToLeftEmbedding,
      RightToLeftOverride = TChar::ERightToLeftOverride,
      PopDirectionalFormat = TChar::EPopDirectionalFormat,
      NonSpacingMark = TChar::ENonSpacingMark,
      BoundaryNeutral = TChar::EBoundaryNeutral
    };

    inline UChar32 toLower(UChar32 ch)
    {
      return TChar(ch).GetLowerCase();
    }

    inline int toLower(UChar* str, int strLength, UChar*& destIfNeeded)
    {
      destIfNeeded = 0;

      for (int i = 0; i < strLength; ++i)
        str[i] = (uint16_t)toLower(str[i]);

      return strLength;
    }

    inline int toLower(UChar* result, int resultLength, const UChar* src, int srcLength,  bool* error)
    {
      // FIXME: handle special casing. Easiest with some low level API in Qt
      *error = false;
      if (resultLength < srcLength) {
        *error = true;
        return srcLength;
      }
      for (int i = 0; i < srcLength; ++i)
        result[i] = toLower(src[i]);
      return srcLength;
    }

    inline UChar32 toUpper(UChar32 ch)
    {
      return TChar(ch).GetUpperCase();
    }

    inline int toUpper(uint16_t* str, int strLength, uint16_t*& destIfNeeded)
    {
      destIfNeeded = 0;

      for (int i = 0; i < strLength; ++i)
        str[i] = (uint16_t)toUpper(str[i]);

      return strLength;
    }

    inline int toUpper(UChar* result, int resultLength, const UChar* src, int srcLength,  bool* error)
    {
      // FIXME: handle special casing. Easiest with some low level API in Qt
      *error = false;
      if (resultLength < srcLength) {
        *error = true;
        return srcLength;
      }
      for (int i = 0; i < srcLength; ++i)
        result[i] = toUpper(src[i]);
      return srcLength;
    }

    inline bool isFormatChar(int32_t c)
    {
      return (c & 0xffff0000) == 0 && TChar((unsigned short)c).GetCategory() == TChar::ECfCategory;
    }

    inline bool isSeparatorSpace(int32_t c)
    {
      return (c & 0xffff0000) == 0 && TChar((unsigned short)c).GetCategory() == TChar::EZsCategory;
    }

    inline bool isPrintableChar(int32_t c)
    {
      return (c & 0xffff0000) == 0 && TChar((unsigned short)c).IsPrint();
    }

    inline UChar32 foldCase(UChar32 c)
    {
      // do not modify the original char
      UChar32 c_ = c;
      TChar _c(c);
      _c.Fold(TChar::EFoldCase);
      c_ = _c;
      return c_;
    }

    inline int foldCase(UChar* result, int resultLength, UChar* src, int srcLength,  bool* error)
    {
      // FIXME: handle special casing. Easiest with some low level API in Qt
      *error = false;
      if (resultLength < srcLength) {
        *error = true;
        return srcLength;
      }
      for (int i = 0; i < srcLength; ++i)
        result[i] = foldCase(src[i]);
      return srcLength;
    }

    inline int toTitleCase(UChar32 c)
    {
      return TChar(c).GetTitleCase();
    }

    inline uint8_t combiningClass(UChar32 c)
    {
      return TChar(c).GetCombiningClass();
    }


    inline UChar32 mirroredChar(UChar32 c)
    {
      // needs fixing
      return c;
    }

    inline bool isDigit(UChar32 c)
    {
      return (c & 0xffff0000) == 0 && TChar(c).IsDigit();
    }

    inline bool isLower(UChar32 c)
    {
      return (c & 0xffff0000) == 0 && TChar(c).IsLower();
    }

    inline bool isUpper(UChar32 c)
    {
      return (c & 0xffff0000) == 0 && TChar(c).IsUpper();
    }

    inline Direction direction(UChar32 c)
    {
      return (Direction)TChar(c).GetBdCategory();
    }

    inline int umemcasecmp(const UChar* a, const UChar* b, int len)
    {
      // handle surrogates correctly
      for (int i = 0; i < len; ++i) {
        TChar c1 = TChar(a[i]);
        TChar c2 = TChar(b[i]);
        c1.Fold();
        c2.Fold();
        if (c1 != c2)
          return c1 - c2;
      }
      return 0;
    }

    inline CharCategory category(int32_t c)
    {
      // FIXME: implement support for non-BMP code points
      if ((c & 0xffff0000) != 0)
        return NoCategory;

      switch (TChar((unsigned short)c).GetCategory()) {
        case TChar::EMnCategory:
          return Mark_NonSpacing;
        case TChar::EMcCategory:
          return Mark_SpacingCombining;
        case TChar::EMeCategory:
          return Mark_Enclosing;
        case TChar::ENdCategory:
          return Number_DecimalDigit;
        case TChar::ENlCategory:
          return Number_Letter;
        case TChar::ENoCategory:
          return Number_Other;
        case TChar::EZsCategory:
          return Separator_Space;
        case TChar::EZlCategory:
          return Separator_Line;
        case TChar::EZpCategory:
          return Separator_Paragraph;
        case TChar::ECcCategory:
          return Other_Control;
        case TChar::ECfCategory:
          return Other_Format;
        case TChar::ECsCategory:
          return Other_Surrogate;
        case TChar::ECoCategory:
          return Other_PrivateUse;
        case TChar::ECnCategory:
          return Other_NotAssigned;
        case TChar::ELuCategory:
          return Letter_Uppercase;
        case TChar::ELlCategory:
          return Letter_Lowercase;
        case TChar::ELtCategory:
          return Letter_Titlecase;
        case TChar::ELmCategory:
          return Letter_Modifier;
        case TChar::ELoCategory:
          return Letter_Other;
        case TChar::EPcCategory:
          return Punctuation_Connector;
        case TChar::EPdCategory:
          return Punctuation_Dash;
        case TChar::EPsCategory:
          return Punctuation_Open;
        case TChar::EPeCategory:
          return Punctuation_Close;
        case TChar::EPiCategory:
          return Punctuation_InitialQuote;
        case TChar::EPfCategory:
          return Punctuation_FinalQuote;
        case TChar::EPoCategory:
          return Punctuation_Other;
        case TChar::ESmCategory:
          return Symbol_Math;
        case TChar::EScCategory:
          return Symbol_Currency;
        case TChar::ESkCategory:
          return Symbol_Modifier;
        case TChar::ESoCategory:
          return Symbol_Other;
        default:
          return NoCategory;
      }
    }

    enum DecompositionType {
      DecompositionNone = 0,
      DecompositionCanonical,
      DecompositionCompat,
      DecompositionCircle,
      DecompositionFinal,
      DecompositionFont,
      DecompositionFraction,
      DecompositionInitial,
      DecompositionIsolated,
      DecompositionMedial,
      DecompositionNarrow,
      DecompositionNoBreak,
      DecompositionSmall,
      DecompositionSquare,
      DecompositionSub,
      DecompositionSuper,
      DecompositionVertical,
      DecompositionWide
    };

    inline DecompositionType decompositionType(UChar32 c)
    {
        // FIXME: no corresponding function in TChar
        return DecompositionNone;
    }
    
    inline int digitValue(UChar32 c)
    {
        return isDigit(c) ? (c - UChar32('0')) : -1;
    }

    inline bool isPunct(UChar32 c)
    {
        return TChar(c).IsPunctuation();
    }
  }
}

#endif
