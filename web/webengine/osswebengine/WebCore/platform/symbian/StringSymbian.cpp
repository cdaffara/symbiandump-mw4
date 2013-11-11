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
#include "PlatformString.h"
#include <e32std.h>

namespace WebCore {

String::String(const TDesC& des)
{
    if (!des.Length())
        m_impl = StringImpl::empty();
    else
        m_impl = new StringImpl(des);
}

String::String(const TDesC8& des)
{
    if (!des.Length())
        m_impl = StringImpl::empty();
    else
        m_impl = new StringImpl(des);
}

}
