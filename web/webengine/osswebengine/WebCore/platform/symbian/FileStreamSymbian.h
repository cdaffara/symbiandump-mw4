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


#ifndef __FILESTREAMSYMBIAN_H__
#define __FILESTREAMSYMBIAN_H__

#include "StreamSymbian.h"
#include "PlatformString.h"
#include <s32file.h>

namespace WebCore {


class FileStreamOutput : public StreamOutput
{
public:
    FileStreamOutput(const String& path);
    ~FileStreamOutput();

    bool isOpened() const       { return m_isOpened; }

    StreamOutput& operator<<(char);
    StreamOutput& operator<<(unsigned char);
    StreamOutput& operator<<(short);
    StreamOutput& operator<<(unsigned short);
    StreamOutput& operator<<(int);
    StreamOutput& operator<<(unsigned int);
    StreamOutput& operator<<(const String&);
    StreamOutput& operator<<(const Streamable&);
    StreamOutput& operator<<(const StreamEndOfLine&);

    RFileWriteStream& platformStream()          { return m_stream; }

private:
    RFileWriteStream m_stream;
    bool m_isOpened;
};

class FileStreamInput : public StreamInput
{
public:
    FileStreamInput(const String& path);
    ~FileStreamInput();

    bool isOpened() const       { return m_isOpened; }

    StreamInput& operator>>(char&);
    StreamInput& operator>>(unsigned char&);
    StreamInput& operator>>(short&);
    StreamInput& operator>>(unsigned short&);
    StreamInput& operator>>(int&);
    StreamInput& operator>>(unsigned int&);
    StreamInput& operator>>(String&);
    StreamInput& operator>>(Streamable&);

    RFileReadStream& platformStream()           { return m_stream; }

private:
    RFileReadStream m_stream;
    bool m_isOpened;
};

class TextFileStreamOutput : public StreamOutput
{
public:

    TextFileStreamOutput(const String& path);
    ~TextFileStreamOutput();

    bool isOpened() const       { return m_isOpened; }

    StreamOutput& operator<<(char);
    StreamOutput& operator<<(unsigned char);
    StreamOutput& operator<<(short);
    StreamOutput& operator<<(unsigned short);
    StreamOutput& operator<<(int);
    StreamOutput& operator<<(unsigned int);
    StreamOutput& operator<<(const String&);
    StreamOutput& operator<<(const Streamable&);
    StreamOutput& operator<<(const StreamEndOfLine&);

    RFileWriteStream& platformStream()          { return m_stream; }

private:
    RFileWriteStream m_stream;
    bool m_isOpened;
    String m_data;
};

}

#endif //!__FILESTREAMSYMBIAN_H__
