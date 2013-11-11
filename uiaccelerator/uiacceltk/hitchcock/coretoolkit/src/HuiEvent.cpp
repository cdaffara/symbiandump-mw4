/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include "uiacceltk/HuiEvent.h"  // Class definition


THuiEvent::THuiEvent(CHuiDisplay* aDisplay, TType aType)
        : iDisplay(aDisplay),
          iCode(EEventNull),
          iParam( KErrNotFound),
          iType(aType),
          iVisual(0)
    {
    }


EXPORT_C THuiEvent::THuiEvent(TInt aCustomParam)
        : iDisplay(0),
          iCode(EEventNull),
          iParam(aCustomParam),
          iType(ETypeCustom),
          iVisual(0)
    {
    }


EXPORT_C THuiEvent::THuiEvent(CHuiDisplay* aDisplay, const TPointerEvent& aPointerEvent)
        : iDisplay(aDisplay),
          iCode(EEventNull),         
          iParam( KErrNotFound),
          iType(ETypePointer),
          iVisual(0)          
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


THuiEvent::THuiEvent(CHuiDisplay* aDisplay,
                     const TKeyEvent& aKeyEvent,
                     TEventCode aType)
        : iDisplay(aDisplay),
          iCode(aType),
          iKeyEvent(aKeyEvent),
          iParam( KErrNotFound ),
          iType(ETypeKey),
          iVisual(0)
    {
    }



EXPORT_C THuiEvent::TType THuiEvent::Type() const
    {
    return iType;
    }


EXPORT_C TBool THuiEvent::IsCustomEvent() const
    {
    return iType == ETypeCustom;
    }


EXPORT_C TBool THuiEvent::IsKeyEvent() const
    {
    return iType == ETypeKey;
    }


EXPORT_C TBool THuiEvent::IsPointerEvent() const
    {
    return iType == ETypePointer;
    }


EXPORT_C CHuiDisplay& THuiEvent::Display() const
    {
    ASSERT( iDisplay );
    return *iDisplay;
    }


EXPORT_C TBool THuiEvent::PointerDown() const
    {
    return iType == ETypePointer &&
           iPointerEvent.iType == TPointerEvent::EButton1Down;
    }


EXPORT_C TBool THuiEvent::PointerUp() const
    {
    return iType == ETypePointer &&
           iPointerEvent.iType == TPointerEvent::EButton1Up;
    }
    
EXPORT_C TBool THuiEvent::PointerLongTap() const
    {
    return iType == ETypePointer &&
           iPointerEvent.iType == TPointerEvent::EButtonRepeat;
    }


EXPORT_C void THuiEvent::SetVisual(CHuiVisual* aVisual)
    {
    iVisual = aVisual;
    }
   
    
EXPORT_C CHuiVisual* THuiEvent::Visual() const
    {
    return iVisual;
    }
