/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 2006 Lars Knoll <lars@trolltech.com>
 * Copyright (C) 2007 Nokia Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "TextBreakIteratorSymbian.h"
#include "TextBreakIterator.h"
#include <e32cmn.h>
#include <kjs/ustring.h>

namespace WebCore {

    class TextBreakIterator
    {
    public:
        virtual int first() = 0;
        virtual int next() = 0;
        virtual int previous() = 0;
        inline int following(int pos) {
            currentPos = pos;
            return next();
        }
        inline int preceding(int pos) {
            currentPos = pos;
            return previous();
        }
        int currentPos;
        const UChar *string;
        int length;
    };

    class WordBreakIteratorSymbian : public TextBreakIterator
    {
    public:
        virtual int first();
        virtual int next();
        virtual int previous();
    };

    class CharBreakIteratorSymbian : public TextBreakIterator
    {
    public:
        virtual int first();
        virtual int next();
        virtual int previous();
    };
    
    class LineBreakIteratorSymbian : public TextBreakIterator
    {
    public:
        virtual int first();
        virtual int next();
        virtual int previous();
    };

    int WordBreakIteratorSymbian::first() {
        currentPos = 0;
        return currentPos;
    }

    int WordBreakIteratorSymbian::next() {
        if (currentPos == length) {
            currentPos = -1;
            return currentPos;
        }
        bool haveSpace = false;
        while (currentPos < length) {
            if (haveSpace && !TChar(string[currentPos]).IsSpace())
                break;
            if (TChar(string[currentPos]).IsSpace())
                haveSpace = true;
            ++currentPos;
        }
        return currentPos;
    }

    int WordBreakIteratorSymbian::previous() {
        if (currentPos == 0) {
            currentPos = -1;
            return currentPos;
        }
        bool haveSpace = false;
        while (currentPos > 0) {
            if (haveSpace && !TChar(string[currentPos]).IsSpace())
                break;
            if (TChar(string[currentPos]).IsSpace())
                haveSpace = true;
            --currentPos;
        }
        return currentPos;
    }

    int CharBreakIteratorSymbian::first() {
        currentPos = 0;
        return currentPos;
    }

    int CharBreakIteratorSymbian::next() {
        if (currentPos == length)
            return -1;
        
        // FIXME: symbian doesn't have Grapheme Cluster support
        // currentPos = layout.nextCursorPosition(currentPos);

        currentPos++;
        return currentPos;
    }
    int CharBreakIteratorSymbian::previous() {
        if (currentPos == 0)
            return -1;

        // FIXME: symbian doesn't have Grapheme Cluster support
        // currentPos = layout.nextCursorPosition(currentPos);

        currentPos--;

        //currentPos = layout.previousCursorPosition(currentPos);
        return currentPos;
    }

    int LineBreakIteratorSymbian::first() {
        currentPos = 0;
        return currentPos;
    }

   int LineBreakIteratorSymbian::next() 
   {
        if (currentPos == length)
            return -1;
        
      // Asian line breaking. Everywhere allowed except directly in front of a punctuation character.
        int iRet = -1;
        currentPos++;   
        unsigned char row = (KJS::UChar(string[currentPos])).high();
        
        if ( row > 0x2d && row < 0xfb || row == 0x11 )
            iRet = currentPos;
        else
           iRet = -1;
        
       return iRet;
 }
    
    int LineBreakIteratorSymbian::previous() 
    {
       //not implemented
       return -1;
    }
    
static WordBreakIteratorSymbian *wordIterator = 0;
static CharBreakIteratorSymbian *charIterator = 0;
static LineBreakIteratorSymbian *lineIterator = 0;
void cleanupIterators() 
{
    delete wordIterator;
    wordIterator = NULL;
    delete charIterator;
    charIterator = NULL;
    delete lineIterator;
    lineIterator = NULL;
}

TextBreakIterator* wordBreakIterator(const UChar* string, int length)
{
    if (!wordIterator)
        wordIterator = new WordBreakIteratorSymbian;

    wordIterator->string = string;
    wordIterator->length = length;
    wordIterator->currentPos = 0;

    return wordIterator;
}

TextBreakIterator* characterBreakIterator(const UChar* string, int length)
{
    if (!charIterator)
        charIterator = new CharBreakIteratorSymbian;

    charIterator->string = string;
    charIterator->length = length;
    charIterator->currentPos = 0;

    return charIterator;
}

TextBreakIterator* lineBreakIterator(const UChar* string, int length)
{
    if (!lineIterator)
        lineIterator = new LineBreakIteratorSymbian;

    lineIterator->string = string;
    lineIterator->length = length;
    lineIterator->currentPos = 0;

    return lineIterator;
}

TextBreakIterator* sentenceBreakIterator(const UChar*, int)
{
    // not yet implemented
    return 0;
}


int textBreakFirst(TextBreakIterator* bi)
{
    return bi->first();
}

int textBreakNext(TextBreakIterator* bi)
{
    return bi->next();
}

int textBreakPreceding(TextBreakIterator* bi, int pos)
{
    return bi->preceding(pos);
}

int textBreakFollowing(TextBreakIterator* bi, int pos)
{
    return bi->following(pos);
}

int textBreakCurrent(TextBreakIterator* bi)
{
    return bi->currentPos;
}

bool isTextBreak(TextBreakIterator*, int)
{
    return true;
}

}



