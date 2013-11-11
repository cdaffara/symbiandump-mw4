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



#ifndef T_ALFEVENT_H
#define T_ALFEVENT_H

#include <w32std.h>
#include <alf/alfscrollerevent.h>

class CAlfDisplay;
class CAlfVisual;

/**
 *  Event class.
 *  This class would typically be passed to a control.
 *  One example of an event would be a keypress,
 *  or a custom message arriving from another control.
 *  You cannot derive from this class.
 *  Usage:
 *  A custom control, derived from Control, overwrides, OfferEventL()
 *  @code
 *  TBool LayoutControl::OfferEventL(const TAlfEvent& aEvent)
 *  {
 * 	TBool handled = EFalse;
 *  if ( aEvent.IsKeyEvent() )
 *       {
 *        / Do something
 *        handled = ETrue;         
 *       }
 *   else if ( aEvent.IsPointerEvent() )
 *        {
 *         switch ( aEvent.PointerEvent().iType )
 *           {
 *           case TPointerEvent::EButton1Down:
 *              / Do something
 *               break;
 *           case TPointerEvent::EButton1Up:
 *               / Do something
 *               break;
 *           case TPointerEvent::EDrag:
 *              / Do something
 *               break;
 *           case TPointerEvent::EButtonRepeat:
 *               / Do something
 *               break;
 *           default:
 *              / Do something
 *               break;
 * 			 }
 *          handled = ETrue;
 *       }
 *    }
 *  @endcode
 *  @see MAlfEventHandler::OfferEventL()
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TAlfEvent )
    {
    
public:

    /** Event types. This is the most general type for the message.*/
    enum TType
        {
        ETypeCustom, 		//!< a message passed from one control to another
        ETypeKey,			//!< a message regarding a keypress
        ETypePointer,		//!< a message regarding a touch screen interface
        ETypeIdleBegin,	    //!< the toolkit has just entered the idle state
        ETypeIdleEnd,		//!< the toolkit has just left the idle state
        ETypeScroller,	    //!< a message sent from a scroller
        
        EAlfEventTypeLast = 1024
        };
        
    /* Constructors. */
   
    /**
     * Constructor.
     */
    TAlfEvent(CAlfDisplay& aDisplay, TType aType);
    
    /**
     * Constructor for custom events (not bound to any display).
     */
    IMPORT_C TAlfEvent(TInt aCustomParam);

    /**
     * Constructor for custom events (not bound to any display).
     */
    IMPORT_C TAlfEvent(TInt aCustomParam, TInt aCustomEventData);
   
    /**
     * Constructor for scroller event bound to a display.
     */
    IMPORT_C TAlfEvent(CAlfDisplay&, const TAlfScrollerEvent& aEvent );
   
    /**
     * Constructor for key events.
     *
     * @param aDisplay Display where the event is received
     * @param aKeyEvent Key event definition
     * @param aType Event code
     */
    IMPORT_C TAlfEvent(CAlfDisplay& aDisplay, const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Constructor for pointer events.
     *
     * @param aDisplay Display where the event is received
     * @param aPointerEvent Pointer event definition
     *          iPosition Contains the coordinates relative to the hit visual
     *          iParentPosition Contains the coordinates relative to the used display area.
     */
    IMPORT_C TAlfEvent(CAlfDisplay& aDisplay, const TPointerEvent& aPointerEvent );
    
    /**
     * Determines whether the event is a pointer event.
     *
     * @todo needs more description
     * @return ETrue if pointer event.
     */
    IMPORT_C TBool IsPointerEvent() const;
    
    /**
     * Returns pointer event. Panics is not pointer event.
     *
     * On the stylus up and down events, the hit leaf visual can be fetched with
     * Visual() function. If the function returns a visual, the iPosition member
     * variable of the TPointerEvent states the event coordinates relative to that 
     * visual area. If the visual is not found, the iPosition is the same as iParentPosition.
     *
     * The iParentPosition memeber variable of the TPointerEvent states the event
     * coordinates relative to the used display visible area. See, that this is usually
     * the main pane area and not the physical screen dimensions.
     *
     * The iParentPosition coornidates can be mapped to any visual's local coordinates by
     * visual->DisplayToLocal( aEvent.PointerEvent().iParentPosition );
     *
     * If the visual is set, the following applies:
     * aEvent.Visual()->LocalToDisplay( aEvent.PointerEvent().iPosition ) == aEvent.PointerEvent().iParentPosition
     * aEvent.Visual()->DisplayToLocal( aEvent.PointerEvent().iParentPosition ) == aEvent.PointerEvent().iPosition
     *
     * @return Pointer event.
     */
    IMPORT_C const TPointerEvent& PointerEvent() const;

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
     * Returns whether the event is a scroller event.
     *
     * @return EFalse iff the event is not a scroller event
     */     
    IMPORT_C TBool TAlfEvent::IsScrollerEvent() const;

    /**
     * Access an ALF scroller event object. 
     *
     * Only can be called if IsScrollerEvent() is not EFalse
     *
     * @return a structure containing the scroller event
     */
    IMPORT_C TAlfScrollerEvent ScrollerEvent() const;
    
    /**
     * Determines whether the event is a key event, for example, a key was pressed.
     *
     * @return ETrue is key event.
     */ 
    IMPORT_C TBool IsKeyEvent() const;
    
    /**
     * Returns key event code. Panics if not key event.
     *
     * @return Key event code.
     */
    IMPORT_C const TEventCode& Code() const;
    
    /**
     * Returns key event. Panics if not key event.
     *
     * @return Key event.
     */
    IMPORT_C const TKeyEvent& KeyEvent() const;
    
    /**
     * Returns custom paramter. Panics if not custom event.
     *
     * @return Custom parameter.
     */
    IMPORT_C TInt CustomParameter() const;
    
    /**
     * Returns additional custom event data. Panics if not a custom event.
     * This can be either an identifier to find out the actual event data or
     * it can contain the data directly.
     *
     * @return Custom event data.
     */
    IMPORT_C TInt CustomEventData() const;    
    
    /**
     * Sets display
     *
     * @param aDisplay New display. Ownership not transferred.
     */
    void SetDisplay( CAlfDisplay* aDisplay );
    
    /**
    * Returns the display that the event comes from, if any.
    * NULL is returned if the event has not had a display set.  Custom events are
    * likely not to have a display set. Key events may have a display set 
    *
    * @return The display the event is associated with
    */
    IMPORT_C CAlfDisplay* Display() const;
    
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
    
    /**
     * Sets visual associated with the event
     *
     * @param aVisual Associated visual.
     */
    IMPORT_C void SetVisual(CAlfVisual* aVisual);
    
    /**
     * Returns visual associated with the event
     *
     * @return Associated visual. NULL if not set.
     */
    IMPORT_C CAlfVisual* Visual() const;

private: // data

    /** The display from where the event originated. */
    CAlfDisplay* iDisplay;

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
    
    /**
     * Holds an alf scroller event object
     */
    TAlfScrollerEvent iScrollerEvent;
       
    /** Parameter for custom events. */
    TInt iParam;
    
    /** Additional event data for custom events. */
    TInt iCustomEventData;

    /** Type of the event. */
    TType iType;
    
    /** Visual which the event is associated with. */
    CAlfVisual* iVisual;

private:
    // Advanced pointer event occupies the extra space
    //TInt iSpare1;
    //TInt iSpare2;
    };



#endif // T_ALFEVENT_H
