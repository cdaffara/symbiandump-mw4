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


#ifndef __STREAMSYMBIAN_H__
#define __STREAMSYMBIAN_H__

namespace WebCore {

class Streamable;
class StreamInput;
class StreamOutput;
class String;

struct StreamEndOfLine {
};

class Streamable
{
public:
    virtual void write(StreamOutput& os) const = 0;
    virtual void read(StreamInput& is) = 0;
};

class StreamOutput
{
public:
    virtual ~StreamOutput() {}

    virtual StreamOutput& operator<<(char) = 0;
    virtual StreamOutput& operator<<(unsigned char) = 0;
    virtual StreamOutput& operator<<(short) = 0;
    virtual StreamOutput& operator<<(unsigned short) = 0;
    virtual StreamOutput& operator<<(int) = 0;
    virtual StreamOutput& operator<<(unsigned int) = 0;
    virtual StreamOutput& operator<<(const String&) = 0;
    virtual StreamOutput& operator<<(const Streamable&) = 0;
    virtual StreamOutput& operator<<(const StreamEndOfLine&) = 0;
};

class StreamInput
{
public:
    virtual ~StreamInput() {}

    virtual StreamInput& operator>>(char&) = 0;
    virtual StreamInput& operator>>(unsigned char&) = 0;
    virtual StreamInput& operator>>(short&) = 0;
    virtual StreamInput& operator>>(unsigned short&) = 0;
    virtual StreamInput& operator>>(int&) = 0;
    virtual StreamInput& operator>>(unsigned int&) = 0;
    virtual StreamInput& operator>>(String&) = 0;
    virtual StreamInput& operator>>(Streamable&) = 0;
};

}

#endif // !__STREAMOUTPUT_H__

