/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Scroller event class.
*
*/



#include <alf/alfscrollerevent.h>

// ======== MEMBER FUNCTIONS ========
   
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfScrollerEvent::TAlfScrollerEvent( TCode aCode, TOrientation aOrientation, TInt aMagnitude )
    :
    iCode(aCode),
    iOrientation(aOrientation),
    iMagnitude(aMagnitude)
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TAlfScrollerEvent::TAlfScrollerEvent()
    :
    iCode(TAlfScrollerEvent::EMoveByItem),
    iOrientation(TAlfScrollerEvent::EVertical),
    iMagnitude(0)
    {
    }

// ---------------------------------------------------------------------------
// Access the event code
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfScrollerEvent::TCode TAlfScrollerEvent::Code() const
    {
    return iCode;
    }
    
// ---------------------------------------------------------------------------
// Access the orientation
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfScrollerEvent::TOrientation TAlfScrollerEvent::Orientation() const
    {
    return iOrientation;
    }
    
// ---------------------------------------------------------------------------
// Access the magnitude
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfScrollerEvent::Magnitude() const
    {
    return iMagnitude;
    }
    
  
