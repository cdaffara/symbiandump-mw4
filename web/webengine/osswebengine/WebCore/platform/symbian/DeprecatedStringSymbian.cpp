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
#include "DeprecatedString.h"

namespace WebCore {

void DeprecatedString::setBufferFromDes(const TDesC& des)
{
    setUnicode((const DeprecatedChar *)des.Ptr(), (unsigned)des.Length());
}

void DeprecatedString::setBufferFromDes(const TDesC8& des)
{
    setLatin1((const char*)des.Ptr(), (unsigned)des.Length());
}

DeprecatedString DeprecatedString::fromDes(const TDesC& des)
{
    DeprecatedString s;
    s.setBufferFromDes(des);
    return s;
}

DeprecatedString DeprecatedString::fromDes(const TDesC8& des)
{
    DeprecatedString s;
    s.setBufferFromDes(des);
    return s;
}

TPtrC DeprecatedString::des() const
{
    if (!dataHandle[0]->_isUnicodeValid)
        dataHandle[0]->makeUnicode();

    TPtrC tstr((const TUint16*)unicode(), length() );

    return tstr;
}

TPtrC8 DeprecatedString::des8() const
{
    TPtrC8 tstr((const TUint8*)latin1(), length());

    return tstr;
}

}
