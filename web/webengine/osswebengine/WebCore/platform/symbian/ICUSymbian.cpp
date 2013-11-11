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

#include <config.h>
#include <e32base.h>
#include <unicode/uchar.h>

U_STABLE UChar32 U_EXPORT2
u_tolower(UChar32 c) {
    return User::LowerCase(c);
}

U_STABLE UChar32 U_EXPORT2
u_toupper(UChar32 c) {
    return User::UpperCase(c);
}

U_STABLE UCharDirection U_EXPORT2
u_charDirection(UChar32 c) {
    switch (TChar(c).GetBdCategory()) {
        default:
        case TChar::ELeftToRight:
            return U_LEFT_TO_RIGHT;
        case TChar::ELeftToRightEmbedding:
            return U_LEFT_TO_RIGHT_EMBEDDING;
        case TChar::ELeftToRightOverride:
            return U_LEFT_TO_RIGHT_OVERRIDE;
        case TChar::ERightToLeft:
            return U_RIGHT_TO_LEFT;
        case TChar::ERightToLeftArabic:
            return U_RIGHT_TO_LEFT_ARABIC;
        case TChar::ERightToLeftEmbedding:
            return U_RIGHT_TO_LEFT_EMBEDDING;
        case TChar::ERightToLeftOverride:
            return U_RIGHT_TO_LEFT_OVERRIDE;
        case TChar::EPopDirectionalFormat:
            return U_POP_DIRECTIONAL_FORMAT;
        case TChar::EEuropeanNumber:
            return U_EUROPEAN_NUMBER;
        case TChar::EEuropeanNumberSeparator:
            return U_EUROPEAN_NUMBER_SEPARATOR;
        case TChar::EEuropeanNumberTerminator:
            return U_EUROPEAN_NUMBER_TERMINATOR;
        case TChar::EArabicNumber:
            return U_ARABIC_NUMBER;
        case TChar::ECommonNumberSeparator:
            return U_COMMON_NUMBER_SEPARATOR;
        case TChar::ENonSpacingMark:
            return U_DIR_NON_SPACING_MARK;
        case TChar::EBoundaryNeutral:
            return U_BOUNDARY_NEUTRAL;
        case TChar::EParagraphSeparator:
            return U_BLOCK_SEPARATOR;
        case TChar::ESegmentSeparator:
            return U_SEGMENT_SEPARATOR;
        case TChar::EWhitespace:
            return U_WHITE_SPACE_NEUTRAL;
        case TChar::EOtherNeutral:
            return U_OTHER_NEUTRAL;
    }
}

U_STABLE UBool U_EXPORT2
u_ispunct(UChar32 c) {
    return TChar(c).IsPunctuation();
}

U_STABLE UChar32 U_EXPORT2
u_foldCase(UChar32 c, uint32_t options) {
    TChar t(c);
    t.Fold();
    return t;
}

U_STABLE UBool U_EXPORT2
u_islower(UChar32 c) {
    return TChar(c).IsLower();
}

U_STABLE int32_t U_EXPORT2
u_charDigitValue(UChar32 c) {
    return TChar(c).GetNumericValue();
}

U_STABLE int8_t U_EXPORT2
u_charType(UChar32 c) {
    switch (TChar(c).GetCategory()) {
        default:
        case TChar::ELlCategory:
            return U_LOWERCASE_LETTER; //Ll
        case TChar::ELuCategory:
            return U_UPPERCASE_LETTER; //Lu
        case TChar::ELoCategory:
            return U_OTHER_LETTER;     //Lo
        case TChar::ELtCategory:
            return U_TITLECASE_LETTER; //Lt
        case TChar::ENlCategory:
            return U_LETTER_NUMBER; //Nl
        case TChar::EMcCategory:
            return U_COMBINING_SPACING_MARK; //Mc
        case TChar::EMeCategory:
            return U_ENCLOSING_MARK;         //Me
        case TChar::EMnCategory:
            return U_NON_SPACING_MARK;       //Mn
        case TChar::ELmCategory:
            return U_MODIFIER_LETTER;       //Lm
        case TChar::ENdCategory:
            return U_DECIMAL_DIGIT_NUMBER;   //Nd
        case TChar::EPcCategory:
            return U_CONNECTOR_PUNCTUATION;
        case TChar::EPdCategory:
            return U_DASH_PUNCTUATION;
        case TChar::EPsCategory:
            return U_START_PUNCTUATION;
        case TChar::EPeCategory:
            return U_END_PUNCTUATION;
        case TChar::EPiCategory:
            return U_INITIAL_PUNCTUATION;
        case TChar::EPfCategory:
            return U_FINAL_PUNCTUATION;
        case TChar::EPoCategory:
            return U_OTHER_PUNCTUATION;
        case TChar::ESmCategory:
            return U_MATH_SYMBOL;
        case TChar::EScCategory:
            return U_CURRENCY_SYMBOL;
        case TChar::ESkCategory:
            return U_MODIFIER_SYMBOL;
        case TChar::ESoCategory:
            return U_OTHER_SYMBOL;
        case TChar::EZsCategory:
            return U_SPACE_SEPARATOR;
        case TChar::EZlCategory:
            return U_LINE_SEPARATOR;
        case TChar::EZpCategory:
            return U_PARAGRAPH_SEPARATOR;
        case TChar::ECcCategory:
            return U_CONTROL_CHAR;
        case TChar::ECfCategory:
            return U_FORMAT_CHAR;
        case TChar::ECsCategory:
            return U_SURROGATE;
        case TChar::ECoCategory:
            return U_PRIVATE_USE_CHAR;
        case TChar::ECnCategory:
            return U_GENERAL_OTHER_TYPES;
    };
}

U_STABLE UBool U_EXPORT2
u_isdigit(UChar32 c) {
    return TChar(c).IsDigit();
}

U_STABLE int32_t U_EXPORT2
u_getIntPropertyValue(UChar32 c, UProperty which) {
    // unimplemented
    return 0;
}

U_STABLE UChar32 U_EXPORT2
u_totitle(UChar32 c) {
    return User::TitleCase(c);
}

const char mirrored_chars[] = {
    '<','>',
    '(',')',
    '[',']',
    '{','}',
    0
};

U_STABLE UChar32 U_EXPORT2
u_charMirror(UChar32 c) {
    if (!TChar(c).IsMirrored())
        return c;
    int n=0;
    while(mirrored_chars[n] && mirrored_chars[n]!=c) n++;
    if (mirrored_chars[n]) {
        return mirrored_chars[n+(n%2?-1:1)];
    }
    return c;
}

U_STABLE UBool U_EXPORT2
u_isUUppercase(UChar32 c) {
    return TChar(c).IsUpper();
}

U_STABLE uint8_t U_EXPORT2
u_getCombiningClass(UChar32 c) {
    return TChar(c).GetCombiningClass();
}

#include <unicode/ustring.h>

U_STABLE int32_t U_EXPORT2
u_memcasecmp(const UChar *s1, const UChar *s2, int32_t length, uint32_t options) {
    return Mem::CompareF(s1,length,s2,length);
}

U_STABLE int32_t U_EXPORT2
u_strToLower(UChar *dest, int32_t destCapacity,
             const UChar *src, int32_t srcLength,
             const char *locale,
             UErrorCode *pErrorCode) {
    if (srcLength==-1) {
        srcLength = 0;
        const UChar* p = src;
        while(*p++) srcLength++;
    }
    if (!dest || !destCapacity)
        return srcLength;
    int l = destCapacity<srcLength?destCapacity:srcLength;
    for (int n=0; n<l;n++)
        dest[n] = User::LowerCase(src[n]);
    return srcLength;
}

U_STABLE int32_t U_EXPORT2
u_strToUpper(UChar *dest, int32_t destCapacity,
             const UChar *src, int32_t srcLength,
             const char *locale,
             UErrorCode *pErrorCode) {
    if (srcLength==-1) {
        srcLength = 0;
        const UChar* p = src;
        while(*p++) srcLength++;
    }
    if (!dest || !destCapacity)
        return srcLength;
    int l = destCapacity<srcLength?destCapacity:srcLength;
    for (int n=0; n<l;n++)
        dest[n] = User::UpperCase(src[n]);
    return srcLength;
}

U_STABLE int32_t U_EXPORT2
u_strFoldCase(UChar *dest, int32_t destCapacity,
              const UChar *src, int32_t srcLength,
              uint32_t options,
              UErrorCode *pErrorCode) {
    if (srcLength==-1) {
        srcLength = 0;
        const UChar* p = src;
        while(*p++) srcLength++;
    }
    if (!dest || !destCapacity)
        return srcLength;
    int l = destCapacity<srcLength?destCapacity:srcLength;
    for (int n=0; n<l;n++)
        dest[n] = User::Fold(src[n]);
    return srcLength;
}

#include <unicode/ubrk.h>

struct UBreakIteratorImpl {
    int textLength;
    int pos;
};

U_STABLE UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      const char *locale,
      const UChar *text,
      int32_t textLength,
      UErrorCode *status) {
    UBreakIteratorImpl* bi = new UBreakIteratorImpl;
    bi->textLength = textLength;
    bi->pos = 0;
    return (UBreakIterator*)bi;
}

U_STABLE void U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             const UChar*    text,
             int32_t         textLength,
             UErrorCode*     status) {
    UBreakIteratorImpl* i = (UBreakIteratorImpl*)bi;
    i->textLength = textLength;
    i->pos = 0;
}

U_STABLE void U_EXPORT2
ubrk_close(UBreakIterator *bi) {
    UBreakIteratorImpl* i = (UBreakIteratorImpl*)bi;
    delete i;
}

U_STABLE int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi) {
    UBreakIteratorImpl* i = (UBreakIteratorImpl*)bi;
    i->pos = 0;
    return 0;
}

U_STABLE int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset) {
    return offset?offset-1:UBRK_DONE;
}

U_STABLE int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset) {
    UBreakIteratorImpl* i = (UBreakIteratorImpl*)bi;
    return offset+1>=i->textLength?UBRK_DONE:offset+1;
}

U_STABLE int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi) {
    UBreakIteratorImpl* i = (UBreakIteratorImpl*)bi;
    i->pos++;    
    return i->pos>=i->textLength?UBRK_DONE:i->pos;
}

U_STABLE int32_t U_EXPORT2
ubrk_current(const UBreakIterator *bi) {
    UBreakIteratorImpl* i = (UBreakIteratorImpl*)bi;
    return i->pos;
}

#include <unicode/ucnv.h>

U_STABLE void U_EXPORT2
ucnv_toUnicode(UConverter *converter,
               UChar **target,
               const UChar *targetLimit,
               const char **source,
               const char *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err) {
    return;
}

U_STABLE const char * U_EXPORT2
ucnv_getStandardName(const char *name, const char *standard, UErrorCode *pErrorCode) {
    return 0;
}

U_STABLE void U_EXPORT2
ucnv_setFromUCallBack (UConverter * converter,
                       UConverterFromUCallback newAction,
                       const void *newContext,
                       UConverterFromUCallback *oldAction,
                       const void **oldContext,
                       UErrorCode * err) {
    return;
}

U_STABLE void U_EXPORT2
ucnv_setSubstChars(UConverter *converter,
                   const char *subChars,
                   int8_t len,
                   UErrorCode *err) {
    return;
}

U_STABLE void U_EXPORT2
ucnv_fromUnicode (UConverter * converter,
                  char **target,
                  const char *targetLimit,
                  const UChar ** source,
                  const UChar * sourceLimit,
                  int32_t* offsets,
                  UBool flush,
                  UErrorCode * err) {
    return;
}

U_STABLE UConverter* U_EXPORT2
ucnv_open(const char *converterName, UErrorCode *err) {
    return 0;
}

U_STABLE void  U_EXPORT2
ucnv_close(UConverter * converter) {
    return;
}

#include <unicode/ucnv_err.h>

U_STABLE void U_EXPORT2 UCNV_FROM_U_CALLBACK_ESCAPE (
                  const void *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  const UChar* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err) {
    return;
}

U_STABLE void U_EXPORT2 UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  const void *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  const UChar* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err) {
    return;
}


#include <unicode/uidna.h>

U_STABLE int32_t U_EXPORT2
uidna_IDNToASCII(  const UChar* src, int32_t srcLength,
                   UChar* dest, int32_t destCapacity,
                   int32_t options,
                   UParseError* parseError,
                   UErrorCode* status) {
    return 0;
}

#include <unicode/unorm.h>

U_STABLE UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(const UChar *source, int32_t sourcelength,
                 UNormalizationMode mode,
                 UErrorCode *status) {
    return UNORM_YES;
}


U_STABLE int32_t U_EXPORT2
unorm_normalize(const UChar *source, int32_t sourceLength,
                UNormalizationMode mode, int32_t options,
                UChar *result, int32_t resultLength,
                UErrorCode *status) {
    return 0;
}

U_STABLE UChar* U_EXPORT2
u_memset(UChar *dest, UChar c, int32_t count)
{
    while( count-- ) {
        (*dest++) = c;
    }
    return dest;
}
