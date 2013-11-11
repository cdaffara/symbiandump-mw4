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
#include "../../bidi.h"

#include "WebTextFormatMask.h"

#include "Frame.h"
#include "Editor.h"
#include "string.h"
#include "HTMLNames.h"
#include "HTMLInputElement.h"

#include "Text.h"
#include "CString.h"
#include "DeprecatedCString.h"
#include "SelectionController.h"

using namespace WebCore;
static const int kInfinite = -1;

// help functions
inline bool isPunctuation(TChar::TCategory category)       { return (category & 0xF0) == TChar::EPunctuationGroup; }
inline bool isSymbol(TChar::TCategory category)            { return (category & 0xF0) == TChar::ESymbolGroup; }
inline bool isLowerCase(TChar::TCategory category)         { return category == TChar::ELlCategory; }
inline bool isUpperCase(TChar::TCategory category)         { return category == TChar::ELuCategory; }
inline bool isDigit(TChar::TCategory category)             { return category == TChar::ENdCategory; }

// NOTE: it seems more logical to put WebTextFormatMask as a member of 
// RenderStyle.  This way we just need to parse the format mask string once
// when parsing CSS.  However, for now we don't want to mess up with css code
// in webcore, and anyway wap css is not used very often.

WebTextFormatMask::WebTextFormatMask(const String& str, bool required) 
    : m_masks(0), m_currentMask(0), m_acceptAll(false), m_inputRequired(required)
{
    buildMaskList(str);
    m_currentMask = m_masks;
}

WebTextFormatMask::~WebTextFormatMask()
{
    clearMaskList();
}

void WebTextFormatMask::buildMaskList(const String& str)
{
    // *M or *m
    if (str.isEmpty() || str=="*M" || str=="*m") {
        m_acceptAll = true;
        return;
    }

    // parse the string
    const UChar* p = String(str).charactersWithNullTermination();
    UChar ch;
    int mul = 1;
    bool result = true;
    while( ch = *p++ ) {
        switch(ch) {
            case 'a': result = createMask( ELeLoSymPuc, mul );      break;
            case 'A': result = createMask( ELeUpSymPuc, mul );      break;
            case 'n': result = createMask( ENumSymPuc, mul );       break;
            case 'N': result = createMask( ENumChar, mul );         break;
            case 'x': result = createMask( ELeLoNumSymPuc, mul );   break;
            case 'X': result = createMask( ELeUpNumSymPuc, mul );   break;
            case 'm': result = createMask( EAnyLow, mul );          break;
            case 'M': result = createMask( EAnyUpper, mul );        break;
            case '*': mul = kInfinite;                              break;
            case '\\':result = createStaticMask(p);                 break;
            default: {
                // 'nf'
                if( isdigit(ch) )
                    mul = parseMultitude(--p, result);
                else 
                    result = false;
                break;
           }
        }

        if(!result) {
            // something wrong with the format string, fallback to
            // accept all characters.
            clearMaskList();
            m_acceptAll = true;
            return;
        }
    }
    
}

void WebTextFormatMask::clearMaskList()
{
    MaskBase* m = m_masks;
    MaskBase* p = m;
    while(p) {
        p = m->m_next;
        delete m;
        m = p;
    }
    m_masks = NULL;
}

bool WebTextFormatMask::createMask( TInputFormatMaskType type, int& multi )
{
    MaskBase* m = NULL;
    if( multi == 1 )
        m = new MaskSingle( type );
    else
        m = new MaskComposite( type, multi );
    multi = 1;
    return appendMask(m);
}

int WebTextFormatMask::parseMultitude( const UChar*& p, bool& result )
{
    // start from p, search all digits
    String dstr;
    while(isdigit(*p))
        dstr.append(*p++);    
    
    // there should be one valid character after digits
    if(*p == 0) {
        result = false;
        return 0;
    }

    // parse the digit string
    int multi = atoi( dstr.latin1().deprecatedCString().data() );
    if(multi>0)
        result = true;
    return multi;        
}

bool WebTextFormatMask::createStaticMask( const UChar*& p )
{
    MaskBase* m = new MaskStatic( *p++ );
    return appendMask(m);
}

bool WebTextFormatMask::checkText( const String& text, ErrorBlock& eb )
{
    // "-wap-input-required" takes precedence
    if(text.length() == 0) {
        return !m_inputRequired;
    }

    if(m_acceptAll) return true;

    m_currentMask = m_masks;

    // we have no masks left
    if( !m_currentMask && text.length()>0 ) {
        eb.set(0, text.length());
        return false;
    }

    for(int i=0; i<text.length(); ++i) {
        if( !m_currentMask || !m_currentMask->check(text[i]) ) {
            // search all illegal characters in this run
            if( m_currentMask ) {
                if( eb.m_start == -1 )
                    eb.set( i, 1 );
                else
                    eb.m_extent++;
            }
            else {
                eb.m_extent += text.length() - i;
                return false;
            }
        }
        else if( eb.m_start != -1 ) {
            // the previous check failed
            return false;
        }

        m_currentMask = m_currentMask->nextMask();
    }

    // did we use up all the masks?
    if(m_currentMask && m_currentMask->multitude() != kInfinite)
        return false;

    return (eb.m_start == -1);
}

MaskBase* WebTextFormatMask::getMask(int aOffset)
{
    MaskBase* m = m_masks;
    int i = 0; 
    while(m) {
        if (i == aOffset) {
            return m;    
        }
        m = m->nextMask();
        ++i;              
    }
    return NULL;
}

int WebTextFormatMask::getMultitude()
{
    int count = 0;
    MaskBase* m = m_masks;
    while (m) {

        if (m->multitude() == kInfinite){
            return kInfinite;    
        }        
        else {
            count += m->multitude();           
        }
        m = m->nextMask();                
    }
        
    return (count)?count:kInfinite;    
}


TInputFormatMaskType WebTextFormatMask::getInputFormatMaskType(Frame* frame, int aOffset)
{ 

    int i = 0;
    MaskBase* m = m_masks;
    while (m) {
        if (m->isComposite()){
            return m->getInputFormatMaskType();    
        }
        else if (i==aOffset) {

            TInputFormatMaskType ifmt = m->getInputFormatMaskType();
            return ifmt;            
        }
        m = m->nextMask();
        ++i;                
    }

    return ENoFormat;
}
  
bool WebTextFormatMask::appendMask(MaskBase* m)
{
    // build the mask chain
    if( !m_masks ) {
        m_masks = m;
    }
    else {
        MaskBase* msk = m_masks;
        while( msk->m_next )
            msk = msk->m_next;

        // composite mask only exists at the end of mask chain.
        if( msk->isComposite() ) {
            delete m;
            return false;
        }
       
        msk->m_next = m;
    }

    return true;
}

MaskComposite::MaskComposite(TInputFormatMaskType t, int mul) 
            : MaskSingle(t), m_offset(0), m_length(mul)
{
}

MaskBase* MaskComposite::nextMask()
{
    if( ++m_offset < m_length || m_length == kInfinite )
        return this;
    m_offset = 0;
    return NULL;
}

bool MaskSingle::check(UChar ch)
{

    TChar::TCategory c = TChar(ch).GetCategory();

    switch(m_type) {
        case ELeLoSymPuc:       return isLowerCase(c) || isSymbol(c) || isPunctuation(c);
        case ELeUpSymPuc:       return isUpperCase(c) || isSymbol(c) || isPunctuation(c);
        case ENumSymPuc:        return isDigit(c) || isSymbol(c) || isPunctuation(c);
        case ENumChar:          return isDigit(c);
        case ELeLoNumSymPuc:    return isLowerCase(c) || isDigit(c) || isSymbol(c) || isPunctuation(c);
        case ELeUpNumSymPuc:    return isUpperCase(c) || isDigit(c) || isSymbol(c) || isPunctuation(c);
        case EAnyLow:
        case EAnyUpper: 
        default:
            return true;
    }

}

bool MaskStatic::check(UChar ch)
{
    return m_char == ch;
}
    
// END OF FILE

