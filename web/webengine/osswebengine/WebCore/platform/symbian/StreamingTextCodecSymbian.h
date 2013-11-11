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


#ifndef StreamingTextCodecSymbian_H
#define StreamingTextCodecSymbian_H

#include "TextDecoder.h"
#include "CharsetDataSymbian.h"
#include <wtf/OwnPtr.h>

namespace WebCore {

    class TextCodecSymbian : public TextCodec {
    public:
        static void registerEncodingNames(EncodingNameRegistrar);
        static void registerCodecs(TextCodecRegistrar);

        explicit TextCodecSymbian(TUint aEncoding);
        virtual ~TextCodecSymbian();

        virtual String decode(const char*, size_t length, bool flush = false);
        virtual CString encode(const UChar*, size_t length, bool allowEntities = false);

        static void deleteStatAvailCharsets();
    
    private:
        bool initializeConverter();
        void checkSupportedEncoding();

        CCnvCharacterSetConverter* m_converter;
        TInt    m_state;
        TUint   m_encoding;
        TBool   m_firstChunkDecoded;
        unsigned m_numBufferedBytes;
        unsigned char m_bufferedBytes[16]; // bigger than any single multi-byte character
        OwnPtr<TextCodec> m_fallbackCodec;

        // static data shared by all converters
        static CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* s_availableCharsets;
    };
}

#endif
