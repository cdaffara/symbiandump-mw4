/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine source file
 *
*/


#include "SVGEvent.h"


// ---------------------------------------------------------------------------
// Constructor
// Initialized with x, y coordinates info and event type
// ---------------------------------------------------------------------------
TSvgUiMouseEvent::TSvgUiMouseEvent( TSvgEngineEventType aEtype, TFloatFixPt aX, TFloatFixPt aY ) :
         iEtype( aEtype ), iX( aX ), iY( aY )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint8 TSvgUiMouseEvent::EventMask()
    {
    return KSvgEventMaskExternalUI;
    }

// ---------------------------------------------------------------------------
// Accessor for the event type
// ---------------------------------------------------------------------------
TSvgEngineEventType TSvgUiMouseEvent::EventType()
    {
    return iEtype;
    }

// ---------------------------------------------------------------------------
// Accessor for x coordinate
// ---------------------------------------------------------------------------
TFloatFixPt TSvgUiMouseEvent::X()
    {
    return iX;
    }
// ---------------------------------------------------------------------------
// Accessor for y coordinate
// ---------------------------------------------------------------------------
TFloatFixPt TSvgUiMouseEvent::Y()
    {
    return iY;
    }

/*********************************************/
// ---------------------------------------------------------------------------
// Constructor
// Initialized for the key code
// ---------------------------------------------------------------------------
TSvgUiKeyEvent::TSvgUiKeyEvent( TUint32 aKeyCode ) : iKeyCode( aKeyCode )
    {
    }

// ---------------------------------------------------------------------------
// Accessor for the event mask
// ---------------------------------------------------------------------------
TUint8 TSvgUiKeyEvent::EventMask()
    {
    return KSvgEventMaskExternalUI;
    }

// ---------------------------------------------------------------------------
// Accessor for the event type
// ---------------------------------------------------------------------------
TSvgEngineEventType TSvgUiKeyEvent::EventType()
    {
    return ESvgEngineEventKeyPress;
    }

// ---------------------------------------------------------------------------
// Accessor for the key code of this event
// ---------------------------------------------------------------------------
TUint32 TSvgUiKeyEvent::KeyCode()
    {
    return iKeyCode;
    }

// ---------------------------------------------------------------------------
// Accessor for the time of the event
// ---------------------------------------------------------------------------
TInt32 TSvgUiKeyEvent::Time()
    {
    return  iTime;
    }

// ---------------------------------------------------------------------------
// Set accessor for the time of the event
// ---------------------------------------------------------------------------
void TSvgUiKeyEvent::SetTime( TInt32 aTime )
    {
    iTime = aTime;
    }

// ---------------------------------------------------------------------------
// Accessor for the begin time of the event
// ---------------------------------------------------------------------------
TInt32 TSvgUiKeyEvent::BeginTime()
    {
    return iBeginTime;
    }

// ---------------------------------------------------------------------------
// Set accessor for the begin time of the event
// ---------------------------------------------------------------------------
void TSvgUiKeyEvent::SetBeginTime( TInt32 aTime )
    {
    iBeginTime = aTime;
    }

/**********************************************/
// ---------------------------------------------------------------------------
// Constructor
// Initialized for the time of the event
// ---------------------------------------------------------------------------
TSvgTimerEventPrep::TSvgTimerEventPrep( TUint32 aTime ) : iTime( aTime )
    {
    }

// ---------------------------------------------------------------------------
// Accessor for the event mask
// ---------------------------------------------------------------------------
TUint8 TSvgTimerEventPrep::EventMask()
    {
    return KSvgEventMaskTimer;
    }

// ---------------------------------------------------------------------------
// Accessor for the event type
// ---------------------------------------------------------------------------
TSvgEngineEventType TSvgTimerEventPrep::EventType()
    {
    return ESvgEngineEventTimerPrep;
    }

// ---------------------------------------------------------------------------
// Accessor for the time of the event
// ---------------------------------------------------------------------------
TUint32 TSvgTimerEventPrep::Time()
    {
    return (TUint32) iTime;
    }

/*********************************************/

// ---------------------------------------------------------------------------
// Constructor
// Initialized for the time span of the event
// ---------------------------------------------------------------------------
TSvgTimerEvent::TSvgTimerEvent( TUint32 aTime ) : iTime( aTime )
    {
    }

// ---------------------------------------------------------------------------
// Accessor for the event mask
// ---------------------------------------------------------------------------
TUint8 TSvgTimerEvent::EventMask()
    {
    return KSvgEventMaskTimer;
    }

// ---------------------------------------------------------------------------
// Accessor for the event type
// ---------------------------------------------------------------------------
TSvgEngineEventType TSvgTimerEvent::EventType()
    {
    return ESvgEngineEventTimer;
    }

// ---------------------------------------------------------------------------
// Accessor for the time of the event
// ---------------------------------------------------------------------------
TUint32 TSvgTimerEvent::Time()
    {
    return (TUint32)iTime;
    }


// ---------------------------------------------------------------------------
// Accessor for the time of the event
// ---------------------------------------------------------------------------
void TSvgTimerEvent::SetTime(TInt32 aTime)
    {
    iTime = aTime;
    }


// ---------------------------------------------------------------------------
// Accessor for the begin time of the event
// ---------------------------------------------------------------------------
TInt32 TSvgTimerEvent::BeginTime()
    {
    return iBeginTime;
    }

// ---------------------------------------------------------------------------
// Set accessor for the begin time of the event
// ---------------------------------------------------------------------------
void TSvgTimerEvent::SetBeginTime( TInt32 aTime )
    {
    iBeginTime = aTime;
    }


/*********************************************/

// ---------------------------------------------------------------------------
// Constructor
// Initialized with the event and the SVG element that is registered for this
// event
// ---------------------------------------------------------------------------
TSvgInternalEvent::TSvgInternalEvent( TSvgEvent aEvent, CSvgElementImpl* aElement ) :
                            iEvent( aEvent ),
							iElement( aElement ),
							IsUserSeek( EFalse)
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// Initialized with the event, the SVG element that is registered for this
// event and the eventtime
// For SetMediaTime implementation.
// ---------------------------------------------------------------------------
TSvgInternalEvent::TSvgInternalEvent( TSvgEvent aEvent, CSvgElementImpl* aElement, TInt32 aTime ) :
                            iEvent( aEvent ),
							iElement( aElement ),
							iTime(aTime),
							IsUserSeek(ETrue)
    {
    }


// ---------------------------------------------------------------------------
// is the event generated in a user seek
// ---------------------------------------------------------------------------
TBool TSvgInternalEvent::UserSeek()
    {
    return IsUserSeek;
    }

// ---------------------------------------------------------------------------
// Accessor for the event mask
// ---------------------------------------------------------------------------
TUint8 TSvgInternalEvent::EventMask()
    {
    return KSvgEventMaskInternal;
    }

// ---------------------------------------------------------------------------
// Accessor for the event type
// ---------------------------------------------------------------------------
TSvgEngineEventType TSvgInternalEvent::EventType()
    {
    return ESvgEngineInternalEvent;
    }

// ---------------------------------------------------------------------------
// Accessor for the registered object (a SVG element)
// ---------------------------------------------------------------------------
CSvgElementImpl* TSvgInternalEvent::ObjectAddress()
    {
    return iElement;
    }

// ---------------------------------------------------------------------------
// Accessor for the SVG Event
// ---------------------------------------------------------------------------
TSvgEvent TSvgInternalEvent::SvgEvent()
    {
    return iEvent;
    }

// ---------------------------------------------------------------------------
// Accessor for the time of the event
// ---------------------------------------------------------------------------
TInt32 TSvgInternalEvent::Time()
    {
    return iTime;
    }

// ---------------------------------------------------------------------------
// Set accessor for the time of the event
// ---------------------------------------------------------------------------
void TSvgInternalEvent::SetTime( TInt32 aTime )
    {
    iTime = aTime;
    }

// ---------------------------------------------------------------------------
// Accessor for the begin time of the event
// ---------------------------------------------------------------------------
TInt32 TSvgInternalEvent::BeginTime()
    {
    return iBeginTime;
    }

// ---------------------------------------------------------------------------
// Set accessor for the begin time of the event
// ---------------------------------------------------------------------------
void TSvgInternalEvent::SetBeginTime( TInt32 aTime )
    {
    iBeginTime = aTime;
    }

// ---------------------------------------------------------------------------
// Accessor for the event mask
// ---------------------------------------------------------------------------
TUint8 TSvgScreenUpdateEvent::EventMask()
    {
    return KSvgEventMaskNone;
    }

// ---------------------------------------------------------------------------
// Accessor for the event type
// ---------------------------------------------------------------------------
TSvgEngineEventType TSvgScreenUpdateEvent::EventType()
    {
    return ESvgEngineEventScreenUpdate;
    }
