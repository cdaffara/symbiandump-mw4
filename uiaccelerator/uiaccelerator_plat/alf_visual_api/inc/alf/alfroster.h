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
* Description:   Roster
*
*/



#ifndef C_ALFROSTER_H
#define C_ALFROSTER_H

#include <e32base.h>

class CAlfDisplay;
class CAlfControlGroup;
class TAlfEvent;
class CAlfControl;
class CAlfVisual;
struct TAlfXYMetric;


/** Predefined positions for showing groups. */
const TInt KAlfRosterShowAtTop = -2;
const TInt KAlfRosterShowAtBottom = -1;

enum TAlfPointerEventFlags
    {
    EAlfPointerEventReportDrag      = 0x01, 
    EAlfPointerEventReportLongTap   = 0x02,
    EAlfPointerEventReportUnhandled = 0x04
    };

class CAlfRoster;

/**
 * Observer class that can be used to notify users about changes in the
 * relative order of control groups in the roster.
 *
 * The NotifyControlGroupOrderChanged() method will be called when control
 * groups are shown or hidden and as a result the relative order the control groups
 * are in is modified. The notification is also called when the first control
 * group is shown or when the last control group is hidden.
 *
 * It is on the user's responsibility to remove the callback object from the 
 * CAlfRoster objects it has been added into before destroying the callback
 * object.
 * Usage:
 * @code
 *  // Create a control group.
 *  CAlfControlGroup& group = iEnv->NewControlGroupL( KPrimaryGroup );
 *
 *  // Add a control to the group.
 *  AnchorControl* hello = AnchorControl::ConstructL(*iEnv);
 *  group.AppendL(*hello);
 * 
 *  // Show the control group on the display.
 * display.Roster().ShowL(group);
 * @endcode
 * @see CAlfRoster::AddControlGroupOrderChangedObserverL()
 * @see CAlfRoster::RemoveControlGroupOrderChangedObserver()
 *
 * @lib alfclient.lib
 * @since S60 v5.0
 */
class MAlfControlGroupOrderChangedObserver
    {
public:
    
    /**
     * Callback method that is called whenever relative order of the control 
     * groups is modified. This is a result of calls to CAlfRoster::ShowL()
     * or CAlfRoster::Hide(). Notification is called after relative order of control
     * groups has changed. User can analyze the observed CAlfRoster object for more 
     * information about the changes. This observer has to be registered to a CAlfRoster
     * object to acquire notifications through this method.
     */
    virtual void NotifyControlGroupOrderChanged() = 0;   

    };

/**
 *  Roster keeps track of the visible control groups.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfRoster ): public CBase
    {

public:

    /**
     * Constructor
     */
    CAlfRoster();

    /**
     * ConstructL
     * 
     * @param aDisplay. Associated display. NULL for shared roster.
     */
    void ConstructL(CAlfDisplay* aDisplay);

    /**
     * Destructor
     */
    virtual ~CAlfRoster();

    /**
	 * Shows a control group.
	 *
	 * @param aGroup  Control group.
	 * @param aWhere  Where to show the group. Index, <code>KAlfRosterShowAtTop</code>,
	 *                or <code>KAlfRosterShowAtBottom</code>.
	 */
    IMPORT_C void ShowL( CAlfControlGroup& aGroup, 
                         TInt aWhere = KAlfRosterShowAtTop);
                         
    /**
	 * Hides a control group.
	 *
	 * @param aGroup  Group.
	 */
    IMPORT_C void Hide(CAlfControlGroup& aGroup);

    /**
     * Finds the index of a control group in the roster.
     *
     * @param aGroup  Control group.
     * @return Index. KErrNotFound if not found.
     */
    IMPORT_C TInt Find(const CAlfControlGroup& aGroup) const;
    
    /**
	 * Removes a control group from the roster.
	 * 
	 * @param aGroup  Control group to remove. NULL does nothing.
	 */
    void Remove(CAlfControlGroup* aGroup);
    
    /**
     * Returns the number of control groups.
     *
     * @return Number of control groups.
     */
    IMPORT_C TInt Count() const;
    
    /**
     * Returns a control group.
     */
    IMPORT_C CAlfControlGroup& ControlGroup(TInt aIndex) const;
    
    /**
     * Finds a control inside the roster.
     *
     * @param aControlId  Identifier of the control to find.
     *
     * @return  Pointer to the control.  <code>NULL</code>, if not found.
     */
    IMPORT_C CAlfControl* FindControl(TInt aControlId) const;
    
    /**
     * Gives input focus to a control.
     */
    IMPORT_C void SetFocus(CAlfControl& aControl);
    
    /**
     * Clears the input focus so that no control has focus.
     */
    IMPORT_C void ClearFocus();
    
    /**
     * Moves an existing control group into a new position in the roster.
     *
     * @param aGroup Moved group.
     * @param aPos  Position to move into.
     */
    TBool Move(CAlfControlGroup& aGroup, TInt aPos);
    
    /**
	 * Appends a new control group on top of the roster.
	 *
	 * @param aGroup  Control group to add.
	 */
    void AppendL(CAlfControlGroup& aGroup);
    
    	/** 
	 * Inserts a control group into a specific position in the roster.
	 *
	 * @param aGroup  Control group.
	 * @param aPos    Position to insert into.
	 *
	 * @leave KErrAlreadyExists  The control group is already in the roster.
	 */
    void InsertL(CAlfControlGroup& aGroup, TInt aPos);
    
    /**
     * Called when a key event occurs. The input event is offered to the 
     * controls in the order defined by the groups.
     * 
     * @param aEvent Received event.
     * @return ETrue if event was consumed.
     */
    TBool HandleEventL(const TAlfEvent& aEvent);
    
    /**
     * Show an individual visual in the roster. The visual is appended to the
     * root level visuals of its group.
     *
     * @param aVisual  Visual to show.
     */
    IMPORT_C void ShowVisualL(CAlfVisual& aVisual);

    /**
     * Hide an individual visual in the roster.
     *
     * @param aVisual  Visual to hide.
     */
    IMPORT_C void HideVisual(CAlfVisual& aVisual);
    
    /**
     * Moves a root visual to the front. Causes layout recalculation.
     */
    IMPORT_C void MoveVisualToFront(CAlfVisual& aRootVisual);
    
    /**
     * Adds a pointer event observer.
     * 
     * @param aObserver Observer type.
     * @param aControl Control which receives the events.
     * @return Error code.
     */
    IMPORT_C TInt AddPointerEventObserver( 
        TAlfPointerEventFlags aObserver, 
        CAlfControl& aControl );
    
    /**
     * Removes a pointer event observer.
     * 
     * @param aObserver Observer type.
     * @param aControl Control which receives the events.
     * @return Error code.
     */
    IMPORT_C TInt RemovePointerEventObserver( 
        TAlfPointerEventFlags aObserver, 
        CAlfControl& aControl );
    
    /**
     * Sets pointer event observer flags. See TAlfPointerEventFlags for
     * possible combinations.
     * 
     * @param aPointerEventFlags Flags. 
     * @param aControl Control which receives the events.
     * @return Error code.
     */
    IMPORT_C TInt SetPointerEventObservers( 
        TUint aPointerEventFlags, 
        CAlfControl& aControl );

    /**
    * To be called on the display's roster when the display's focus changes state.
    * The roster will remove focus from controls if NULL is passed in. It does this without
    * changing its FocusedControl(), which becomes a latent focus.
    * When ETrue is passed in, the FocusedConrol() is again given control focus.
    *
    * @param aDisplay The display losing its focus as a whole
    * @param aNewFocusState The new focus state
    */
    void DisplayFocusChanged( CAlfDisplay& aDisplay, TBool aNewFocusState );
        
    /**
    * Access to the control to which the roster will send events first.
    *
    * @return the roster's currently focused control
    */
    CAlfControl* FocusedControl() const;

    /**
     * Sets treshold that pointer needs to be moved before starting to send 
     * drag events. Default treshold is 4 pixels in both X and Y direction.
     * Control must be added to some pointer event observer array, before
     * calling this function. Also removing control from observer will 
     * reset this treshold.
     *
     * NOTE: Not every unit type is feasible. Only these types are supported:
     * EAlfUnitPixel, EAlfUnitRelativeToDisplay, EAlfUnitS60
     *
     * \code
     *  Display()->Roster().SetPointerEventObservers( EAlfPointerEventReportDrag, *this );     
     *  Display()->Roster().SetPointerDragThreshold(*this,TAlfXYMetric(TAlfMetric(0.2,EAlfUnitRelativeToDisplay),TAlfMetric(0.1,EAlfUnitRelativeToDisplay)));     
     * \endcode
     *
     * @param aControl  Control to which treshold affects.
     * @param aXYMetric  Treshold in metric units.
     * @return Error code.
     */
    IMPORT_C TInt SetPointerDragThreshold(const CAlfControl& aControl, const TAlfXYMetric& aXYMetric);

    /**
     * Disables long tap events when dragging is going on. Default 
     * functionality is that long tap event will be delivered simultaneously
     * with drag events. Control must be added to some pointer event observer 
     * array, before calling this function. Also removing control from 
     * observer will reset this feature.
     *
     * \code
     *  Display()->Roster().SetPointerEventObservers( EAlfPointerEventReportDrag, *this );     
     *  Display()->Roster().DisableLongTapEventsWhenDragging(*this);
     * \endcode
     *
     * @param aControl  Control to which disabling affects.
     * @param aDisable  Boolean to indicate whether feature will be disabled 
     *                  or not.
     * @return Error code.
     */
    IMPORT_C TInt DisableLongTapEventsWhenDragging(const CAlfControl& aControl, TBool aDisable = ETrue);

    /**
     * Adds the given MAlfControlGroupOrderChangedObserver object to the observer array of this CAlfRoster.
     *
     * The observer object will be notified whenever relative order of control groups changes in this CAlfRoster.
     *
     * @see RemoveControlGroupOrderChangedObserver()
     * @see MAlfControlGroupOrderChangedObserver
     *
     * @param aObserver The MAlfControlGroupOrderChangedObserver object to add to the array of active observers.
     *
     * @since S60 v5.0
     */
    IMPORT_C void AddControlGroupOrderChangedObserverL(MAlfControlGroupOrderChangedObserver& aObserver);
    
    /**
     * Removes the given MAlfControlGroupOrderChangedObserver object from the observer array of this CAlfRoster.
     *
     * If the MAlfControlGroupOrderChangedObserver object is not found from the array of observers the call
     * is ignored.
     *
     * @see AddControlGroupOrderChangedObserverL()
     * @see MAlfControlGroupOrderChangedObserver
     *
     * @param aObserver MAlfControlGroupOrderChangedObserver object to be searched from the array of observers
     *                  and removed if found.
     *
     * @since S60 v5.0     
     */
    IMPORT_C void RemoveControlGroupOrderChangedObserver(MAlfControlGroupOrderChangedObserver& aObserver);
        
private:
    
    void ChangeInputFocus(CAlfControl* aControl);
    void ReportHostsAboutChangingInputFocus( CAlfControl* aOldFocusControl, CAlfControl* aNewFocusControl );
    void NotifyControlGroupOrderChangedObservers();

private:
    
    // Private data. Owned.
    struct TPrivateData;
    TPrivateData* iData;

    };


#endif // C_ALFROSTER_H
