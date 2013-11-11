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
#include "KURL.h"
#include <string.h>
#include <wtf/Assertions.h>
#include <wtf/Vector.h>

using namespace WebCore;

KURL::KURL( const TDesC8& aUrl )
{
    TInt bytesLength(aUrl.Length());
    if (bytesLength ) {
        Vector<char, 2048> buffer(bytesLength + 6);  // 5 for "file:", 1 for NUL terminator
        char *bytes = &buffer[5];
        memcpy(bytes, aUrl.Ptr(), bytesLength);
        bytes[bytesLength] = '\0';
        if (bytes[0] == '/') {
            buffer[0] = 'f';
            buffer[1] = 'i';
            buffer[2] = 'l';
            buffer[3] = 'e';
            buffer[4] = ':';
            parse(buffer.data(), 0);
        } else
            parse(bytes, 0);
    } else
        parse("", 0);
}

TPtrC8 KURL::des() const
{
    return urlString.des8();
}
