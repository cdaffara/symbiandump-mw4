/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Event class.
*
*/



#include "alf/alfevent.h"
#include "alf/alfvisual.h"
#include <alf/alfscrollerevent.h>

#include <uiacceltk/HuiUtil.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TAlfEvent::TAlfEvent(CAlfDisplay& aDisplay, TType aType)
        : iDisplay(&aDisplay),
          iCode( EEventNull ),
          iParam(KErrNotFound),
          iType(aType),
          iVisual( NULL )
          
    {
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfEvent::TAlfEvent(TInt aCustomParam)
        : iDisplay(NULL),
          iCode( EEventNull ),
          iParam(aCustomParam),
          iCustomEventData(0),
          iType(ETypeCustom),
          iVisual( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfEvent::TAlfEvent(TInt aCustomParam, TInt aCustomEventData)
        : iDisplay(NULL),
          iCode( EEventNull ),
          iParam(aCustomParam),
          iCustomEventData(aCustomEventData),
          iType(ETypeCustom),
          iVisual( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfEvent::TAlfEvent( 
    CAlfDisplay& aDisplay, 
    const TAlfScrollerEvent& aEvent )
        : iDisplay(&aDisplay),
          iCode( EEventNull ),
          iScrollerEvent(aEvent),
          iParam(0),
          iCustomEventData(0),
          iType(ETypeScroller),
          iVisual( NULL )
    {
    }

   
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfEvent::TAlfEvent(
    CAlfDisplay& aDisplay, const TKeyEvent& aKeyEvent, TEventCode aType)
        : iDisplay(&aDisplay),
          iCode(aType),
          iKeyEvent(aKeyEvent),
          iParam(KErrNotFound),
          iType(ETypeKey),
          iVisual( NULL )
    {
    }
  
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfEvent::TAlfEvent( CAlfDisplay& aDisplay, 
                      const TPointerEvent& aPointerEvent )
        : iDisplay(&aDisplay),
          iCode( EEventNull ),
          /*iPointerEvent(*aPointerEvent.AdvancedPointerEvent()),*/
          iParam( KErrNotFound ),
          iType(ETypePointer),
          iVisual( NULL )
    {
     if ( aPointerEvent.iModifiers &EModifierAdvancedPointerEvent )
         {
         iPointerEvent = *aPointerEvent.AdvancedPointerEvent();
         }
     else
         {
         iPointerEvent.iType=aPointerEvent.iType;       
         iPointerEvent.iPosition=aPointerEvent.iPosition;
         iPointerEvent.iParentPosition=aPointerEvent.iParentPosition;
         }
    }

// ---------------------------------------------------------------------------
// Is pointer event?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAlfEvent::IsPointerEvent() const
    {
    return iType == ETypePointer;
    }
   
// ---------------------------------------------------------------------------
// Returns pointer event
// ---------------------------------------------------------------------------
// 
EXPORT_C const TPointerEvent& TAlfEvent::PointerEvent() const
    {
    __ASSERT_ALWAYS( IsPointerEvent(), USER_INVARIANT() );
    return iPointerEvent;
    }

// ---------------------------------------------------------------------------
// Returns event type.
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfEvent::TType TAlfEvent::Type() const
    {
    return iType;    
    }
    
// ---------------------------------------------------------------------------
// Is custom event?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAlfEvent::IsCustomEvent() const
    {
    return iType == ETypeCustom;        
    }

// ---------------------------------------------------------------------------
// Is it a scroller input event?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAlfEvent::IsScrollerEvent() const
    {
    return iType == ETypeScroller; 
    }

// ---------------------------------------------------------------------------
// Returns scroller event
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfScrollerEvent TAlfEvent::ScrollerEvent() const
    {
    __ASSERT_ALWAYS( IsScrollerEvent(), USER_INVARIANT() );
    TAlfScrollerEvent scrollerEvent = iScrollerEvent;
    return scrollerEvent;
    }
    
// ---------------------------------------------------------------------------
// Is key event?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAlfEvent::IsKeyEvent() const
    {
    return iType == ETypeKey;
    }

// ---------------------------------------------------------------------------
// Return key event code.
// ---------------------------------------------------------------------------
//
EXPORT_C const TEventCode& TAlfEvent::Code() const
    {
    __ASSERT_ALWAYS( IsKeyEvent(), USER_INVARIANT() );
    return iCode;
    }

// ---------------------------------------------------------------------------
// Return key event.
// ---------------------------------------------------------------------------
//
EXPORT_C const TKeyEvent& TAlfEvent::KeyEvent() const
    {
    __ASSERT_ALWAYS( IsKeyEvent(), USER_INVARIANT() );
    return iKeyEvent;
    }
    
// ---------------------------------------------------------------------------
// Return custom event parameter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfEvent::CustomParameter() const
    {
    __ASSERT_ALWAYS( IsCustomEvent(), USER_INVARIANT() );
    return iParam;
    }

// ---------------------------------------------------------------------------
// Return custom event data
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfEvent::CustomEventData() const
    {
    __ASSERT_ALWAYS( IsCustomEvent(), USER_INVARIANT() );
    return iCustomEventData;
    }

// ---------------------------------------------------------------------------
// Sets display
// ---------------------------------------------------------------------------
//    
void TAlfEvent::SetDisplay( CAlfDisplay* aDisplay )
    {
    iDisplay = aDisplay;
    }
    
// ---------------------------------------------------------------------------
// Returns display or NULL
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDisplay* TAlfEvent::Display( ) const
    {
    return iDisplay;
    }

// ---------------------------------------------------------------------------
// Check if the event is pointer down event.
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool TAlfEvent::PointerDown() const
    {
    return iType == ETypePointer &&
           iPointerEvent.iType == TPointerEvent::EButton1Down;
    }

// ---------------------------------------------------------------------------
// Check if the event is pointer up event.
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool TAlfEvent::PointerUp() const
    {
    return iType == ETypePointer &&
           iPointerEvent.iType == TPointerEvent::EButton1Up;
    }

// ---------------------------------------------------------------------------
// Check if the event is long pointer event.
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool TAlfEvent::PointerLongTap() const
    {
    return iType == ETypePointer &&
           iPointerEvent.iType == TPointerEvent::EButtonRepeat;
    }

// ---------------------------------------------------------------------------
// Sets the associated visual
// ---------------------------------------------------------------------------
//     
EXPORT_C void TAlfEvent::SetVisual(CAlfVisual* aVisual)
    {
    iVisual = aVisual;
    }

// ---------------------------------------------------------------------------
// Returns the visual
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfVisual* TAlfEvent::Visual() const
    {
    return iVisual;
    }
    
  
