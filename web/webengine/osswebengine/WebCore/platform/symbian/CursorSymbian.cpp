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
#include "Cursor.h"
#include "PlatformCursorSymbian.h"
#include "WebCoreWidget.h"

namespace WebCore {
static const Cursor gCursor;

Cursor::Cursor(const Cursor& other)
: m_impl(other.m_impl)
    {
    }

Cursor::~Cursor()
    {
    }

Cursor& Cursor::operator=(const Cursor& other)
    {
    m_impl = other.m_impl;
    return *this;
    }

Cursor::Cursor(SymbianCursor* c)
    : m_impl(c)
    {
    }


const Cursor& crossCursor()
    {
    return gCursor;
    }

const Cursor& handCursor()
    {
    static Cursor g = Cursor( new SymbianCursor( HandCursor ) );
    return g;
    }

const Cursor& iBeamCursor()
    {
    static Cursor g = Cursor( new SymbianCursor( IBeamCursor ) );
    return g;
    }

const Cursor& waitCursor()
    {
    return gCursor;
    }

const Cursor& helpCursor()
    {
    return gCursor;
    }

const Cursor& eastResizeCursor()
    {
    return gCursor;
    }

const Cursor& northResizeCursor()
    {
    return gCursor;
    }

const Cursor& northEastResizeCursor()
    {
    return gCursor;
    }

const Cursor& northWestResizeCursor()
    {
    return gCursor;
    }

const Cursor& southResizeCursor()
    {
    return gCursor;
    }

const Cursor& southEastResizeCursor()
    {
    return gCursor;
    }

const Cursor& southWestResizeCursor()
    {
    return gCursor;
    }

const Cursor& westResizeCursor()
    {
    return gCursor;
    }

const Cursor& northSouthResizeCursor()
    {
    return gCursor;
    }

const Cursor& eastWestResizeCursor()
    {
    return gCursor;
    }

const Cursor& northEastSouthWestResizeCursor()
    {
    return gCursor;
    }

const Cursor& northWestSouthEastResizeCursor()
    {
    return gCursor;
    }

const Cursor& columnResizeCursor()
    {
    return gCursor;
    }

const Cursor& rowResizeCursor()
    {
    return gCursor;
    }

const Cursor& pointerCursor()
    {
    return gCursor;
    }

const Cursor& verticalTextCursor()
    {
    return gCursor;
    }

const Cursor& cellCursor()
    {
    return gCursor;
    }

const Cursor& contextMenuCursor()
    {
    return gCursor;
    }

const Cursor& noDropCursor()
    {
    return gCursor;
    }

const Cursor& aliasCursor()
    {
    return gCursor;
    }

const Cursor& zoomInCursor()
{
    return gCursor;
}

const Cursor& zoomOutCursor()
{
    return gCursor;
}

const Cursor& copyCursor()
    {
    return gCursor;
    }

const Cursor& noneCursor()
    {
    return gCursor;
    }

const Cursor& notAllowedCursor()
    {
    return gCursor;
    }

const Cursor& progressCursor()
    {
    return gCursor;
    }


}
