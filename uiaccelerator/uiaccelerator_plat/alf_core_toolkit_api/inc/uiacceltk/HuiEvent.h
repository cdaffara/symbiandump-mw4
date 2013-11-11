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



#ifndef __HUIEVENT_H__
#define __HUIEVENT_H__


#include <e32base.h>
#include <coedef.h>
#include <w32std.h>


/* Forward declarations. */
class CHuiDisplay;
class CHuiVisual;


/**
 * An input event. This class would typically be passed to a control.
 * One example of an event would be a keypress,
 * or a custom message arriving from another control.
 * You cannot derive from this class.
 */
NONSHARABLE_CLASS(THuiEvent)
	{
public:

    /** @beginAPI */

    /** Event types. This is the most general type for the message.*/
    enum TType
        {
        ETypeCustom, 		//!< a message passed from one control to another
        ETypeKey,			//!< a message regarding a keypress
        ETypePointer,		//!< a message regarding a touch screen interface
        ETypeIdleBegin,	    //!< hui has entered the idle state
        ETypeIdleEnd		//!< hui has left the idle state
        };
        

	/* Constructors. */
   
    /**
     * Constructor.
     */
    THuiEvent(CHuiDisplay* aDisplay, TType aType);
    
    /**
     * Constructor for custom events (not bound to any display).
     */
    IMPORT_C THuiEvent(TInt aCustomParam);
   
    /**
     * Constructor for pointer events.
     */
    IMPORT_C THuiEvent(CHuiDisplay* aDisplay, const TPointerEvent& aPointerEvent);
    
	/* Methods. */

    /**
     * Returns the type of the event.
     * @see TType
     */     
    IMPORT_C TType Type() const;
    
    /**
     * Determines whether the event is a custom event.
     */     
    IMPORT_C TBool IsCustomEvent() const;
    
    /**
     * Determines whether the event is a key event, for example, a key was pressed.
     */     
    IMPORT_C TBool IsKeyEvent() const;
    
    /**
     * Determines whether the event is a pointer event.
     * A pointer event is caused by a pointing device, such as a click, move or drag.
     * @return true if the event is a pointer event.
     */     
    IMPORT_C TBool IsPointerEvent() const;
    
    /**
     * Returns a reference to the display which the event originated from.
     */
    IMPORT_C CHuiDisplay& Display() const;

    /**
     * Pointer is down.
     * @return ETrue if this is a pointer event and the pointer has been pressed down.
     */
    IMPORT_C TBool PointerDown() const;

    /**
     * Pointer is up.
     * @return ETrue if this is a pointer event and the pointer has been lifted up.
     */
    IMPORT_C TBool PointerUp() const;
    
    /**
     * Pointer event is a long press.
     * @return ETrue if this is a pointer event and the pointer has been pressed for long tap duration.
     */
    IMPORT_C TBool PointerLongTap() const;

    IMPORT_C void SetVisual(CHuiVisual* aVisual);
    
    IMPORT_C CHuiVisual* Visual() const;

    /** @endAPI */	

    /**
     * Constructor for key events.
     */
    THuiEvent(CHuiDisplay* aDisplay, const TKeyEvent& aKeyEvent, TEventCode aType);
    

private:
    
    /* Private methods */


public:

    /* Public member variables. */

    /** The display from where the event originated. */
    CHuiDisplay* iDisplay;

    /** Type of this event, if it is a Symbian S60 event code.
    *
    * For example: EEventKeyUp, EEventKeyDown
    *
    * @see Symbian event codes.
    */    
    TEventCode iCode;

    /** The S60 struct that contains scancode, key event information, etc.
    * @see Symbian event codes.
    */        
    TKeyEvent iKeyEvent;


    /** The S60 struct that contains pointer device event info, which would be
    * triggered by touch screen interaction.
    * @see Symbian event codes.
    */            
    TAdvancedPointerEvent iPointerEvent;
    
    /** Parameter for custom events. Alignment not BC between TB9.1 and 9.2 */
    TInt iParam;


private:

    /** Type of the event. */
    TType iType;
    
    /** Visual which the event is associated with. */
    CHuiVisual* iVisual;
    
    // occupied by Advanced pointer event, to keep at least the size as same
    //TInt iSpare1;
    //TInt iSpare2;
	};

#endif  // __HUIEVENT_H__
