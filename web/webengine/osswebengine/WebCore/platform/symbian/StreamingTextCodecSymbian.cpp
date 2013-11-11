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
#include "StreamingTextCodecSymbian.h"
#include "CharsetData.h"
#include "CString.h"
#include "PlatformString.h"
#include "TextEncoding.h"
#include "TextEncodingRegistry.h"
#include <eikenv.h>
#include <wtf/Assertions.h>

using std::auto_ptr;
using std::min;

static const size_t ConversionBufferSize = 16384;

namespace WebCore {
    
// static data
CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* TextCodecSymbian::s_availableCharsets = 0;

static auto_ptr<TextCodec> newTextCodecSymbian(const TextEncoding&, const void* additionalData)
{
    return auto_ptr<TextCodec>(new TextCodecSymbian(*static_cast<const TUint*>(additionalData)));
}

void TextCodecSymbian::registerEncodingNames(EncodingNameRegistrar registrar)
{
    TInt lastEncoding = -1;
    const char* lastName = 0;

    for (size_t i = 0; CharsetTable[i].name; ++i) {
        if (CharsetTable[i].encoding != lastEncoding) {
            lastEncoding = CharsetTable[i].encoding;
            lastName = CharsetTable[i].name;
        }
        registrar(CharsetTable[i].name, lastName);
    }
}

void TextCodecSymbian::registerCodecs(TextCodecRegistrar registrar)
{
    TInt lastEncoding = -1;

    for (size_t i = 0; CharsetTable[i].name; ++i)
        if (CharsetTable[i].encoding != lastEncoding) {
            registrar(CharsetTable[i].name, newTextCodecSymbian, &CharsetTable[i].encoding);
            lastEncoding = CharsetTable[i].encoding;
        }
}

TextCodecSymbian::TextCodecSymbian(TUint encoding)
    : m_converter(0)
    , m_state(CCnvCharacterSetConverter::KStateDefault)
    , m_encoding(encoding)
    , m_firstChunkDecoded(EFalse)
    , m_numBufferedBytes(0)
{
}

TextCodecSymbian::~TextCodecSymbian()
{
    delete m_converter;
    m_fallbackCodec.clear();
}

bool TextCodecSymbian::initializeConverter()
{
    if( m_firstChunkDecoded ) return true;

    // Removed the call to CEikonEnv::Static()->FsSession() which should not be
    // called in a thread where UiKon static env does not exist. This gets called
    // by the icon database thread which does not have it. Also, the file
    // server session is not needed anymore. It exists only for backward compatibility
    RFs fsSession;

    // FIXME: find a way to free the global availableCharsets array
    if( !m_converter ) {
        // first time call to charconv
        if( ( m_converter = 
            CCnvCharacterSetConverter::NewL() ) == NULL ) 
            return false;

        // caching the available charsets table
        if( !s_availableCharsets )
            s_availableCharsets = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL( fsSession );
    }
   
    // check decoder avialability
    checkSupportedEncoding();

    // prepare character conversion
    m_converter->PrepareToConvertToOrFromL( m_encoding, *s_availableCharsets, fsSession );

    return true;
}

String TextCodecSymbian::decode(const char* chs, size_t length, bool flush)
{
    if( !initializeConverter() )
        return String();

    // FIXME: how to handle flush flag in Symbian converter?
    if( m_encoding == KCharacterSetIdentifierIso88591 ) {
        if( !m_fallbackCodec ) {
            m_fallbackCodec.set(newTextCodec(Latin1Encoding()).release());
        }

        return m_fallbackCodec->decode( chs, length, flush );
    }

    Vector<UChar> result;

    // prepare the input buffer for character converter,
    // the leftover bytes need to be decoded first.
    TPtrC8 inBuf(0,0);
    char* inputString = 0;
    if( m_numBufferedBytes != 0 ) {
        const int fullLength = m_numBufferedBytes + length;
        inputString = (char*)malloc( fullLength );
        if( !inputString ) return String();
        memcpy( inputString, m_bufferedBytes, m_numBufferedBytes );
        memcpy( inputString + m_numBufferedBytes, chs, length );
        inBuf.Set( (const TUint8*)inputString, fullLength );
        m_numBufferedBytes = 0;
    }
    else {
        inBuf.Set( (const TUint8*)chs, length );
    }

    // output buffer
    UChar* buffer = (UChar*)malloc( ConversionBufferSize*sizeof(UChar) );
    if( !buffer ) return String();
    TPtr16 outBuf( (TUint16*)buffer, 0, ConversionBufferSize );
    
    for(;;) {
        int rep = 0;
        int rIndx = 0;
        
        int retValue = m_converter->ConvertToUnicode( outBuf, inBuf, m_state, rep, rIndx );

        if( retValue == CCnvCharacterSetConverter::EErrorIllFormedInput ) {
            if (inBuf.Length() < strlen((char*) m_bufferedBytes))
            {
            memcpy( m_bufferedBytes, inBuf.Ptr(), inBuf.Length() );
            m_numBufferedBytes = inBuf.Length();
            }
            break;
        }
        else if( retValue == 0 ) {
            // decoded fully, exit
            appendOmittingBOM( result, buffer, outBuf.Length() );
            break;
        }
        else {
            // prepare for next round
            appendOmittingBOM( result, buffer, outBuf.Length() );
            outBuf.SetLength(0);
            inBuf.Set( inBuf.Right(retValue) );
        }
    }
    
    free( buffer );
    free( inputString );

    if(!m_firstChunkDecoded )
        m_firstChunkDecoded = true;

    String resultString = String::adopt(result);
    return resultString;
}

CString TextCodecSymbian::encode(const UChar* data, size_t length, bool allowEntities)
{
    if( !initializeConverter() )
        return CString();

    // FIXME: how to handle allowEntities flag in Symbian converter?
    // We should figure out a good way to use SetReplacementForUnconvertibleUnicodeCharactersL()
    if( m_encoding == KCharacterSetIdentifierIso88591 ) {
        if( !m_fallbackCodec ) {
            m_fallbackCodec.set(newTextCodec(Latin1Encoding()).release());
        }

        return m_fallbackCodec->encode( data, length, allowEntities );
    }
    
    Vector<char> result;
    
    // input buffer
    TPtrC inBuf( (const TUint16*)data, length );

    // output buffer
    char* buffer = (char*)malloc( ConversionBufferSize*sizeof(char) );
    if( !buffer ) return CString();
    size_t size = 0;
    TPtr8 outBuf( (TUint8*)buffer, 0, ConversionBufferSize );

    for(;;) {
        TInt retValue = m_converter->ConvertFromUnicode( outBuf, inBuf );
        const int olen = outBuf.Length();
        
        if( retValue == CCnvCharacterSetConverter::EErrorIllFormedInput )
            break;
        
        result.resize( size + olen );
        memcpy( result.data() + size, buffer, olen );
        size += olen;

        if( retValue == 0 )
            break;
        else {
            outBuf.SetLength(0);
            inBuf.Set( inBuf.Right(retValue) );
        }
    }

    free( buffer );

    if( !m_firstChunkDecoded )
        m_firstChunkDecoded = true;

    return CString( result.data(), size );
}

void TextCodecSymbian::checkSupportedEncoding()
{
    int count = s_availableCharsets->Count();

    for (int index = 0; index < count; index++) {
        if (m_encoding == ((CCnvCharacterSetConverter::SCharacterSet)(s_availableCharsets->At(index))).Identifier())
            return;
    }

    // otherwise fall back to iso88591
    m_encoding = KCharacterSetIdentifierIso88591;
}


void TextCodecSymbian::deleteStatAvailCharsets()
{
    delete s_availableCharsets;
    s_availableCharsets = 0;
}

}

// END OF FILE
