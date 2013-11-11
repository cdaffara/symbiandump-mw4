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
#include "IntSize.h"
#include <e32std.h>

namespace WebCore {

IntSize::IntSize(const TSize& s)
    : m_width(s.iWidth)
    , m_height(s.iHeight)
{
}

IntSize::operator TSize() const
{
    return TSize(m_width, m_height);
}

}
