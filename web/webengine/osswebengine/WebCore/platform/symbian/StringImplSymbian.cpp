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
#include "StringImpl.h"
#include <e32std.h>

namespace WebCore {

StringImpl::StringImpl(const TDesC& des)
{
    init(des.Ptr(), des.Length());
}

StringImpl::StringImpl(const TDesC8& des)
{
    init((const char*)des.Ptr(), des.Length());
}

TPtrC StringImpl::des() const
{
    TPtrC tstr((const TUint16 *)m_data, m_length);
    return tstr;
}

}
