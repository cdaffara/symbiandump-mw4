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



#ifndef __HUIROSTER_H__
#define __HUIROSTER_H__


#include <e32base.h>
#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/huieventhandler.h>


/* Forward declarations. */
class MHuiFrameBuffer;
class CHuiControlGroup;
class CHuiControl;
class CHuiVisual;
class CHuiGc;
class THuiEvent;
class CHuiRosterEntry;
struct THuiXYMetric;
class MHuiEffectable;
class CHuiRenderBuffer;

/** Predefined positions for showing groups. */
const TInt KHuiRosterShowAtTop = -2;
const TInt KHuiRosterShowAtBottom = -1;


/** Input event types for observing. */
enum THuiInputType
    {
    EHuiInputPointerDrag,
    EHuiInputPointerLongTap,
    EHuiInputPointerUnhandled
    };


/**
 * Roster defines a priority order of control groups shown an a display.
 * The roster also offers input events to the groups, and handles the
 * drawing of the groups' root visuals.
 *
 * The roster is responsible for managing the logical state of the UI: which
 * controls are active, who has input focus, and which controls receive input
 * events. In the roster, the logical structure and state of
 * the user interface is represented by control groups and controls.
 *
 * This is an abstract base class. The methods here are accessible from
 * the outside.
 *
 * There is a roster owned by the environment (a shared roster), if this is not
 * passed to a display's ConstructL, it will create its own private roster automatically.
 *
 * @see HUITK Programmer's Guide for more information on using rosters.
 */
NONSHARABLE_CLASS(CHuiRoster) : public CBase
	{
public:

	/* Destructor. */
	virtual ~CHuiRoster();
	

	/* Methods. */
	
	/**
	 * Shows a control group.
	 *
	 * @param aGroup  Control group.
	 * @param aWhere  Where to show the group. Index, <code>KHuiRosterShowAtTop</code>,
	 *                or <code>KHuiRosterShowAtBottom</code>.
	 */
	virtual void ShowL(CHuiControlGroup& aGroup, TInt aWhere = KHuiRosterShowAtTop) = 0;
		
	/**
	 * Hides a control group.
	 * Hides all the controls within a control group. Also disconnects the
	 * control from its host.
	 *
	 * @param aGroup  Control group to be hidden.
	 *
	 * @see CHuiControl::Hide()
	 */
    virtual void Hide(CHuiControlGroup& aGroup) = 0;

    /**
     * Finds the index of a control group in the roster.
     *
     * @param aGroup  Control group. Should return KErrNotFound if not found.
     */
    virtual TInt Find(CHuiControlGroup* aGroup) const = 0;
    
    /**
     * Returns the number of control groups.
     */
    virtual TInt Count() const = 0;
    
    /**
     * Returns a control group.
     *
     * @see Count()
     */
    virtual CHuiControlGroup& ControlGroup(TInt aIndex) const = 0;
    
    /**
     * Finds a control inside the roster.
     *
     * @param aControlId  Identifier of the control to find.
     *
     * @return  Pointer to the control.  <code>NULL</code>, if not found.
     */
    virtual CHuiControl* FindControl(TInt aControlId) const = 0;
   
    /**
     * Gives input focus to a control.
     */
    virtual void SetFocus(CHuiControl& aControl) = 0;
    
    /**
     * Clears the input focus so that no control has focus.
     */
    virtual void ClearFocus() = 0;
                
    /**
     * Called when a key event occurs. The input event is offered to the 
     * controls in the order defined by the groups.
     */    
    virtual TBool HandleEventL(const THuiEvent& aEvent) = 0;
    
    /**
     * Show an individual visual in the roster. The visual is appended to the
     * root level visuals of its group, at the end of the list of root visuals.
     *
     * @param aVisual  Visual to show.
     */
    IMPORT_C virtual void ShowVisualL(CHuiVisual* aVisual) = 0;

    /**
     * Hide an individual visual in the roster.
     *
     * @param aVisual  Visual to hide.
     */
    virtual void HideVisual(CHuiVisual* aVisual) = 0;

    /**
     * Moves a root visual to the front. Causes layout recalculation.
     */
    virtual void MoveVisualToFront(CHuiVisual* aRootVisual) = 0;
    
    /**
     * Returns the array of input observers for a specific type of input 
     * event.
     *
     * @param aInputType  Type of input being observed.
     */
    virtual RHuiObserverArray<MHuiEventHandler>& Observers(THuiInputType aInputType) = 0;

    /**
     * Sets treshold which pointer needs to be moved before starting to send 
     * drag events.Default treshold is 4 pixels in both X and Y direction.
     * Event handler must be added to some pointer event observer array, before
     * calling this function. Also removing event handler from observer will 
     * reset this treshold.
     *
     * NOTE: Not every unit type is feasible. Only these types are supported:
     * EHuiUnitPixel, EHuiUnitRelativeToDisplay, EHuiUnitS60
     *
     * \code
     *  Display()->Roster().Observers(EHuiInputPointerDrag).AppendL(*this);
     *  Display()->Roster().SetPointerDragThresholdL(*this,THuiXYMetric(THuiMetric(0.2,EHuiUnitRelativeToDisplay),THuiMetric(0.1,EHuiUnitRelativeToDisplay)));
     * \endcode
     *
     * @param aEventHandler  Event handler to which treshold affects.
     * @param aXYMetric  Treshold in metric units.
     */
    virtual void SetPointerDragThresholdL(const MHuiEventHandler& aEventHandler, const THuiXYMetric& aXYMetric) = 0;

    /**
     * Disables long tap events when dragging is going on. Default 
     * functionality is that long tap event will be delivered simultaneously
     * with drag events. Event handler must be added to some pointer event 
     * observer array, before calling this function. Also removing event 
     * handler from observer will reset this feature.
     *
     * \code
     *  Display()->Roster().Observers(EHuiInputPointerDrag).AppendL(*this);
     *  Display()->Roster().DisableLongTapEventsWhenDraggingL(*this);
     * \endcode
     *
     * @param aEventHandler  Event handler to which disabling affects.
     * @param aDisable  Boolean to indicate whether feature is disabled or not.
     */
    virtual void  DisableLongTapEventsWhenDraggingL(const MHuiEventHandler& aEventHandler, TBool aDisable = ETrue) = 0;
    
    /**
     * Moves a root visual to the given position.
     *
     * @param aRootVisual  Visual to move.
     */
    virtual void Move(CHuiVisual* aVisual, TInt aPos) = 0;

    /**
     * Returns effectable-interface if it is supported by the implementation.
     *
     */
    virtual MHuiEffectable* Effectable() = 0;
        
    /**
     * Takes screenshot from the roster content and keeps drawing that until
     * this method is called again to unfreeze content.
     * 
     * @param aFreeze if ETrue, further frames are drawn from cached image of the
     * roster content. If EFalse, normal drawing is used.
     *
     */
    virtual void FreezeVisibleContentL(TBool aFreeze = ETrue) = 0;
    
    virtual TBool IsVisibleContentFrozen() const = 0;
    
    virtual void SetAlfEventWindow(CHuiVisual* aVisual) = 0;
    
protected:

    /* Constructors. */

	/**
	 * Constructor.
	 */
	CHuiRoster();
    
	};

#endif  // __HUIROSTER_H__
