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
#include <../bidi.h>
#include "FileStreamSymbian.h"
#include "PlatformString.h"
#include "StaticObjectsContainer.h"

namespace WebCore {

//------------------------------------------------------------------------------
// FileStreamOutput
//------------------------------------------------------------------------------

FileStreamOutput::FileStreamOutput(const String& path) : m_isOpened(false)
{
    if (m_stream.Replace(StaticObjectsContainer::instance()->fsSession(), path.des(), EFileWrite | EFileStream | EFileShareAny) == KErrNone)
        m_isOpened = true;
}

FileStreamOutput::~FileStreamOutput()
{
    m_stream.Close();
}

StreamOutput& FileStreamOutput::operator<<(char ch)
{
    m_stream.WriteInt8L(ch);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(unsigned char ch)
{
    m_stream.WriteUint8L(ch);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(short sh)
{
    m_stream.WriteInt16L(sh);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(unsigned short ush)
{
    m_stream.WriteUint16L(ush);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(int i)
{
    m_stream.WriteInt32L(i);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(unsigned int ui)
{
    m_stream.WriteUint32L(ui);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(const String& str)
{
    // we only write unicode data into the string
    int len = str.length();
    const UChar* chs = str.characters();
    m_stream.WriteInt32L(len);
    m_stream.WriteL((const TUint8*)(chs), len<<1);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(const Streamable& o)
{
    o.write(*this);
    return *this;
}

StreamOutput& FileStreamOutput::operator<<(const StreamEndOfLine&)
{
    return *this;
}

//------------------------------------------------------------------------------
// FileStreamInput
//------------------------------------------------------------------------------

FileStreamInput::FileStreamInput(const String& path) : m_isOpened(false)
{
    if (m_stream.Open(StaticObjectsContainer::instance()->fsSession(), path.des(), EFileShareReadersOnly | EFileRead | EFileStream) == KErrNone)
        m_isOpened = true;
}

FileStreamInput::~FileStreamInput()
{
    m_stream.Close();
}

StreamInput& FileStreamInput::operator>>(char& ch)
{
    ch = m_stream.ReadInt8L();
    return *this;
}


StreamInput& FileStreamInput::operator>>(unsigned char& uch)
{
    uch = m_stream.ReadUint8L();
    return *this;
}


StreamInput& FileStreamInput::operator>>(short& sh)
{
    sh = m_stream.ReadInt16L();
    return *this;
}


StreamInput& FileStreamInput::operator>>(unsigned short& ush)
{
    ush = m_stream.ReadUint16L();
    return *this;
}


StreamInput& FileStreamInput::operator>>(int& i)
{
    i = m_stream.ReadInt32L();
    return *this;
}


StreamInput& FileStreamInput::operator>>(unsigned int& ui)
{
    ui = m_stream.ReadUint32L();
    return *this;
}


StreamInput& FileStreamInput::operator>>(String& str)
{
    int len = m_stream.ReadInt32L();
    UChar* data = new UChar[len];
    m_stream.ReadL(data, len);
    str.insert(data, len, 0);
    delete data;
    return *this;
}


StreamInput& FileStreamInput::operator>>(Streamable& o)
{
    o.read(*this);
    return *this;
}

//------------------------------------------------------------------------------
// TextFileStreamOutput
//------------------------------------------------------------------------------

TextFileStreamOutput::TextFileStreamOutput(const String& path) : m_isOpened(false)
{
    if (m_stream.Replace(StaticObjectsContainer::instance()->fsSession(), path.des(), EFileWrite | EFileStream | EFileShareAny) == KErrNone)
        m_isOpened = true;
}

TextFileStreamOutput::~TextFileStreamOutput()
{
    m_stream.Close();
}

StreamOutput& TextFileStreamOutput::operator<<(char ch)
{
    m_data.append(String::number(ch));
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(unsigned char ch)
{
    m_data.append(String::number(ch));
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(short sh)
{
    m_data.append(String::number(sh));
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(unsigned short ush)
{
    m_data.append(String::number(ush));
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(int i)
{
    m_data.append(String::number(i));
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(unsigned int ui)
{
    m_data.append(String::number(ui));
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(const String& str)
{
    m_data.append(str);    
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(const Streamable& o)
{
    o.write(*this);
    return *this;
}

StreamOutput& TextFileStreamOutput::operator<<(const StreamEndOfLine&)
{
    String str("\n");
    int len = str.length();
    const UChar* chs = str.characters();
    m_stream.WriteL((const TUint8*)(chs), len<<1);
    return *this;
}



}
