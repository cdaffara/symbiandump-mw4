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
#include "PlatformMouseEvent.h"

#include <w32std.h>
#include <stdio.h>
#include <stdlib.h>

namespace WebCore {


static MouseButton mouseButtonForEvent(TPointerEvent::TType aType)
{
    switch (aType)
    {
    case TPointerEvent::EButton1Down:
    case TPointerEvent::EButton1Up:
    case TPointerEvent::EDrag:
        {
            return LeftButton;
        }
    case TPointerEvent::EButton2Down:
    case TPointerEvent::EButton2Up:
        {
            return RightButton;
        }
    case TPointerEvent::EButton3Down:
    case TPointerEvent::EButton3Up:
        {
            return MiddleButton;
        }
    default:
        {
            return LeftButton;
        }
    }
    
}

static MouseEventType mouseEventForEventType(TPointerEvent::TType aType)
{
    switch (aType)
    {
        default:
        case TPointerEvent::EButton1Down:
        case TPointerEvent::EButton2Down:
            return MouseEventPressed;
        case TPointerEvent::EButton1Up:
        case TPointerEvent::EButton2Up:
            return MouseEventReleased;
        case TPointerEvent::EDrag:
            return MouseEventMoved;
    }
}


PlatformMouseEvent::PlatformMouseEvent( TPointerEvent aEvent )
{
    m_position = aEvent.iPosition;
    m_globalPosition = m_position;
    m_button = mouseButtonForEvent( aEvent.iType );
    m_clickCount = aEvent.iType == TPointerEvent::EMove ? 0 : 1;
    m_shiftKey = aEvent.iModifiers & EModifierShift;
    m_ctrlKey = aEvent.iModifiers & EModifierCtrl;
    m_altKey = aEvent.iModifiers & EModifierAlt;
    // m_metaKey = ???
    m_metaKey = EFalse;
    m_eventType = mouseEventForEventType(aEvent.iType);
}
}
