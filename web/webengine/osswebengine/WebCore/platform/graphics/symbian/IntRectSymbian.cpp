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
#include "IntRect.h"
#include <e32std.h>

namespace WebCore {

IntRect::IntRect(const TRect& r)
    : m_location(IntPoint(r.iTl.iX, r.iTl.iY))
    , m_size(IntSize(r.Size().iWidth, r.Size().iHeight))
{
}

IntRect::operator TRect() const
{
    return TRect(x(), y(), right(), bottom());
}

TRect IntRect::Rect() const
{
    return TRect(x(), y(), right(), bottom());
}

}
