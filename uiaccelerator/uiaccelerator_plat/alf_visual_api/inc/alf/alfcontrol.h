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
* Description:   Base class for client side controls
*
*/



#ifndef C_ALFCONTROL_H
#define C_ALFCONTROL_H

#include <e32base.h>
#include <barsread.h>
#include <alf/alfvisual.h>  
#include <alf/alflayout.h>  
#include <alf/alfeventhandler.h>
#include <alf/alfpropertyowner.h>

/* Forward declarations. */
class CAlfEnv;
class CAlfDisplay;
class TAlfEvent;
class CAlfControlGroup;

/**
 * CAlfControl provides a base class for gathering logical functionality in
 * the UI.
 *
 * For example a list control could gather and control all visuals that form
 * a listbox UI element. A thumbnail view control could be used to control visuals
 * that form a thumbnail view in an image gallery application.
 *
 * Their main use is to handle interaction with the user by catching events through
 * overloaded OfferEventL() - method.
 *
 * Developers should derive their controls from this base class to implement the UI logic
 * of their application. Developers should remember to pass a reference to the CAlfEnv - object
 * to the CAlfControl - base class when derived class is constructed through the 
 * CAlfControl::ConstructL() - method. Failure to do this will result in KERN-EXEC 3 panic
 * when CAlfControl - derived object is added to a CAlfControlGroup. Usually you would do this
 * when ConstructL() - method of the derived class is called, like this:
 *
 * Usage: 
 * @code
 * class CMyControl : public CAlfControl
 *    {
 *  public:
 *     CMyControl( CAlfEnv& aEnv );
 *    ~CMyControl();
 * 
 *    //Handle interaction with the user by catching events
 *    virtual TBool OfferEventL(const TAlfEvent& aEvent);
 *    
 *    //Few other notofication methods, overloaded by this custom
 *    //control, to perform required action
 *    void NotifyControlVisibility( TBool aIsVisible, CAlfDisplay& aDisplay );
 *    void VisualLayoutUpdated( CAlfVisual& aVisual );
 *    void FocusChanged( CAlfDisplay& aDisplay, TBool aFocused );
 *    void HostChanging( CAlfControl* aNewHost );
 *    TBool Focusing()const;
 *   };
 * 
 *  void CMyControl::ConstructL(CAlfEnv& aEnv)
 * 	{
 *     CAlfControl::ConstructL(aEnv);
 * 	}
 * 
 *  //To show the control on display
 *  //Create a new control group
 *  CAlfControlGroup& group = iEnv->NewControlGroupL( KControlGroupIdDefault );
 * 
 *  //Create control instance and append it to group
 *  CMyControl* control = CMyControl::ConstructL(&iEnv);
 *  group.AppendL( control );
 * 
 *  //Show control group on display
 *  iDisplay->Roster().ShowL( group );
 * 
 * @endcode 
 * Controls are not visible by themselves, but controls create and manipulate
 * visuals (CAlfVisual - derived objects). Controls set the target values for visual properties and/or
 * schedule animation commands.
 *
 * Controls are collected in groups represented by CAlfControlGroup - objects (e.g., one group per view). 
 * Even though controls are contained within control groups they do not have a hierarchical organization.
 * Controls can be, however, connected to each other for collaboration.
 * This can be done using AddConnectionL() - method.
 */
class CAlfControl : public CAlfPropertyOwner, public MAlfEventHandler 
	{
public:

	/** @beginAPI */
	
	/* Constructors and destructor. */

	/**
	 * Constructor.
	 */
	IMPORT_C CAlfControl();

	/**
	 * Second-phase constructor.
	 */
	IMPORT_C void ConstructL( CAlfEnv& aEnv );

	/**
	 * Destructor.
	 */
	IMPORT_C ~CAlfControl();


	/* Methods. */
	

    IMPORT_C TInt Identifier() const;

	/**
	 * Set an id value for this control.
	 */
	IMPORT_C void SetId( TInt aId );

	/**
	 * Returns the id of this control. Zero (0) id the control has no id.
	 */
	IMPORT_C TInt Id() const;

	/**
	 * Returns the environment of the visual.
	 */
	IMPORT_C CAlfEnv& Env() const;

	/**
	 * Returns the control group of the visual's owner control.
	 *
	 * @return  Control group, or <code>NULL</code> if there isn't one.
	 */
	IMPORT_C CAlfControlGroup* ControlGroup() const;

    /**
     * Returns the display this control is bound to, or <code>NULL</code>.
     */
    IMPORT_C CAlfDisplay* Display() const;

    /**
     * Binds the control to a display. Called automatically by 
     * CAlfControl::ShowL().
     */
    IMPORT_C void BindDisplay( CAlfDisplay& aDisplay );

	/**
	 * Appends a new visual to the list of visuals owned by the control.
	 *
	 * @see CAlfVisual::Owner()
	 *
	 * @param aVisual        Visual to be appended.
	 * @param aParentLayout  Parent layout visual to which the visual will be appended to.
	 * @return Error code.   KErrNotSupported if some other control already owns the visual
	 *                       KErrAlreadyExists if visual was added to this control already
	 *                       System wide error codes in case of OOM   
	 */
	IMPORT_C TInt Append( CAlfVisual* aVisual,
                           CAlfLayout* aParentLayout = NULL);

	/**
	 * Removes a visual from the control. The caller also receives ownership
	 * of the visual.
	 *
	 * @param aVisual  Visual to remove. Caller gets ownership.
	 */
	IMPORT_C void Remove( CAlfVisual* aVisual );

	/**
	 * Creates a new visual using the visual factory and appends it.
	 *
	 * @param aVisualType  Type of the visual to create.
	 *
	 * @return  Pointer to the created visual.
	 */
    IMPORT_C CAlfVisual* AppendVisualL( TAlfVisualType aVisualType,
                                        CAlfLayout* aParentLayout = 0,
                                        TInt aImplementationUid = 0 );

    /**
     * Creates a new layout using the visual factory and appends it.
     *
     * @param aLayoutType  Type of the layout to create.
     *
     * @return  Pointer to the created layout.
     */
    IMPORT_C CAlfLayout* AppendLayoutL( TAlfLayoutType aLayoutType,
                                        CAlfLayout* aParentLayout = NULL,
                                        TInt aImplementationUid = 0 );

    /**
     * Returns one of the visuals owned by the control.
     *
     * @param aIndex  Index number of the visual to return.
     *
     * @return  Visual.
     */
    IMPORT_C CAlfVisual& Visual( TInt aIndex ) const;

    /**
     * Determines the number of visuals owned by the control. 
     *
     * Note that in visual hierarchies, child visuals are not owned by 
     * their parent visuals, but a control. This means that a control 
     * that owns a tree of visuals will return the total number of visuals 
     * in the tree, and not just the number of root visuals.
     *
     * @return  Number of visuals owned by the control.
     */
    IMPORT_C TInt VisualCount() const;

    /**
     * Finds the visual that matches a tag. Only the visuals owned by 
     * this control are searched.
     *
     * @param aTag  Tag descriptor to match against.
     *
     * @return  The first visual that matches the tag, or <code>NULL</code>
     *          if no visual matched the tag.
     */
    IMPORT_C CAlfVisual* FindTag( const TDesC8& aTag ) const;
      
	/**
	 * Returns the visual host control.
	 *
	 * @return  Host control. NULL if not set.
	 */
	IMPORT_C CAlfControl* Host() const;

	/**
	 * Establishes a manual connection between this host control and another control.
	 * 
	 * If setting manually the connection, do not use the SetHostId() function
	 * from the child control!
	 *
	 * Calls ConnectionAddedL when a connection has been created between the two
	 * controls
 	 *
	 * @param aConnectedControl  Control to connect.
	 * @param aRole              Role of the control. Interpretation depends on
	 *                           the host.
	 *
	 * @see ConnectionAddedL
	 */
	IMPORT_C virtual void AddConnectionL( CAlfControl* aConnectedControl, 
	                                      TInt aRole );

	/**
	 * Removes a client.
	 *
	 * Calls ConnectionRemoved when a connection has been broken between the two
	 * controls
	 *
	 * @param aConnectedControl  Connected control to remove.
	 *
	 * @see ConnectionRemoved
	 */
	IMPORT_C void RemoveConnection( CAlfControl* aConnectedControl );

    /**
     * Find a client's index number.
     *
     * @param aConnected  Connected control to find.
     *
     * @return  Index number.
     */
    IMPORT_C TInt FindConnection( const CAlfControl* aConnected ) const;

	/**
	 * Returns the number of clients.
	 *
	 * @return  Number of client controls.
	 */
    IMPORT_C TInt ConnectionCount() const;

	/**
	 * Returns a client control.
	 *
	 * @param aIndex  Index of the client.
	 *
	 * @return  Reference to the client control.
	 */
    IMPORT_C CAlfControl& Connection( TInt aIndex ) const;

    /**
     * @deprecated
     *  
     * Returns a client control.
     *
     * @param aOrdinal  Role or index.
     */
    IMPORT_C CAlfControl& ConnectionByOrdinal( TInt aOrdinal ) const;

	/**
	 * Returns the role of a client control.
	 *
	 * @param aIndex  Index of the client.
	 *
	 * @return  Role of the client control.
	 */
    IMPORT_C TInt ConnectionRole( TInt aIndex ) const;

    /**
     * @deprecated
     *
     * Ordinal is the effective index. Role overrides index.
     */
    IMPORT_C TInt ConnectionOrdinal( TInt aIndex ) const;

    /**
     * Returns the controls role.
     * 
     * @return Role.
     */
    IMPORT_C TInt Role() const;

    /**
     * Sets the controls role.
     * 
     * @param aRole New role.
     */
    IMPORT_C void SetRole( TInt aRole );

    /**
     * Returns the automatic visual host identification.
     * 
     * @return Identifier of the host control. Zero if not set,
     */
    IMPORT_C TInt HostId() const;

    /**
     * Sets the automatic visual host identification.
     * When the control is shown, the connection is created automatically
     * betweem this control and the host determined by the paramter.
     * 
     * If the automatic ID is set, do not use the manual AddConnectionL function!
     * 
     * @param aHostId Identifier of the host control. Zero if not set,
     */
    IMPORT_C void SetHostId( TInt aHostId );

	/**
	 * @internal
	 *
	 * Do not call this outside the library!
	 *
	 * Sets the host of the control. The AddConnectionL function will
	 * call this after the host side has been set-up for the connection.
	 *
	 * @param aHost  Host control.
	 */
    IMPORT_C void SetHost( CAlfControl* aHost );

    /**
     * Returns a container layout for the specified child control.
     *
     * By default this function returns NULL. Override this function
     * in your own control to select the container layout. 
     *
     * @param aConnected Child control, which is querying the layout.
     *
     * @return Container layout for the child controls' root visuals.
     */
    IMPORT_C virtual CAlfLayout* ContainerLayout(
        const CAlfControl* aConnected ) const;

    /**
     * Coordinate conversion.
     */
    IMPORT_C TPoint HostToDisplay( const TPoint& aPoint ) const;

    /**
     * Coordinate conversion.
     */
    IMPORT_C TPoint DisplayToHost( const TPoint& aPoint ) const;

    /**
     * Calculates the region of the screen where the control's visuals are.
     *
     * @return  Rectangle in display coordinates.
     */
    IMPORT_C TRect Bounds() const;

    /**
     * Tests whether a point is inside the control.
     *
     * @param aPoint  Point in display coordinates.
     *
     * @return  <code>ETrue</code>, if the point is inside one of the
     *          control's visuals. Otherwise <code>EFalse</code>.
     */
    IMPORT_C TBool HitTest( const TPoint& aPoint ) const;

    /**
     * Attempt to acquire focus for this control.
     * 
     * Actively attempt to acquire focus on all displays that contain visuals owned by this control.
     * This will set the focused control in the rosters of these displays, and then, in most situations, call 
     * <code>SetFocus</code> with a value of  <code>ETrue</code> on this control. The callback in 
     * <code>CAlfControl::FocusChanged</code> or its virtual override is then called. It is there that the control 
     * should perform whatever visual or functional changes that it performs upon change of focus.
     * 
     * If a display for this control is not currently focused (for example if another application is showing a popup or an element from
     * another UI library, for instance an Avkon menu is displayed ) then calling this method will only set a latent focus 
     * in that display's roster. That is, the focused control will be changed in the roster, but not in the control itself.
     * The control will not receive a <code>FocusChanged</code> callback. The call to this method "fails" only in the sense 
     * that there is no immediate setting of focus to the control by the environment.
     *
     * When focus is restored to the display, then the latent focus held in the roster will then be used to set focus again to the
     * control. Alf controls will thus experience loss and gain of focus when displays lose and gain focus, with the record of which 
     * control has the latent focus held in the roster.
     */
    IMPORT_C void AcquireFocus();

    /**
     * Attempt to give up focus.
     *
     * Actively give up focus on all displays containing visuals for this control. Whether the focus is 
     * actually changed in the associated roster(s) depends upon the focus state of the related displays.
     * @see CAlfControl::AcquireFocus
     *
     * @note If this control has latent focus in a roster then it is still cleared (i.e. is no longer even latent).
     * @note If this control is not the currently focused control in a roster (either active or latent) then calling 
     * this method has no effect on the focus state of that roster. 
     */
    IMPORT_C void RelinquishFocus();
    
    /**
     * Determines whether the control has input focus.
     */
    IMPORT_C TBool Focus() const;
    
    /**
     * Finds a child connection with focus or recursively one of its child
     * connections have focus. Use Focus() to find out if the returned
     * control instance has focus.
     * 
     * @return Child connection on a focus path. NULL if not found.
     */
    IMPORT_C CAlfControl* FocusedConnection() const;
    
    /**
     * Called when this control is included/excluded from the focus chain.
     *
     * Example:
     * There are controls c1, c2 and c3. The c3 is a child connection of the 
     * c2 (i.e. c2 is a host of c3). When the c1 is focused, the c2 is not 
     * part of the focus chain. Now, when c3 receives focus, the c2 becomes
     * part of the focus chain and this function will be called for c2. When the c3
     * loses focus (with RelinquishFocus() or back to c1), this function we be 
     * called again for c2 object.
     * 
     * This function calls PropertyOwnerExtension() with parameters:
     * aExtensionUid        KUidAlfPropOwnerExtControlFocusChainChanged
     * aExtensionParams     TBool** True if focus chain is gained.
     */
    void FocusChainChanged( TBool aInFocusChain );

    /**
     * Determines whether the control wants to receive focus.
     */
    IMPORT_C virtual TBool IsFocusing() const;

    /**
     * Called when the control's focus state changes.
     */
    IMPORT_C virtual void FocusChanged( CAlfDisplay& aDisplay, TBool aFocused );

    /**
     * Determines whether the control accepts input events.
     */
    IMPORT_C TBool AcceptInput() const;

    /**
     * Called when an input event is being offered to the control.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  <code>ETrue</code>, if the event was handled.
     *          Otherwise <code>EFalse</code>.
     */
    IMPORT_C virtual TBool OfferEventL( const TAlfEvent& aEvent );

    /**
     * Determines the display area.
     *
     * @return  Area of the display on which the control is shown.
     */
    IMPORT_C TRect DisplayArea() const;

    /**
     * Determines the coordinates of the center of the display.
     *
     * @return  Center of the display on which the control is shown.
     */
    IMPORT_C TAlfRealPoint DisplayCenter() const;

    /**
     * Cancel all scheduled commands related to this control and the control's
     * visuals.
     */
    IMPORT_C void CancelAllCommands();
    
    /**
     * Called when a visual owned by the control is being destroyed.
     * Visual is removed from the control.
     *
     * @param aVisual  Visual about to the destroyed.
     */
    IMPORT_C virtual void VisualDestroyed(CAlfVisual& aVisual);
    
    /**
     * Notifies the owner that the layout of a visual has been recalculated.
     * Called only when the EAlfVisualFlagLayoutUpdateNotification flag has
     * been set for the visual.
     *
     * @param aVisual  Visual that has been laid out.
     */
     IMPORT_C virtual void VisualLayoutUpdated(CAlfVisual& aVisual);
   
	/**
	 * Notifies the visual owner if there was an error (a leave happened)
	 * when preparing the visual for drawing. Typically the resource alloc
	 * errors such as out of memory is escalated through this callback.
	 * 
     * @param aVisual    Visual that caused the error.
     * @param aErrorCode Symbian KErr error code for the failure.
	 */
     IMPORT_C virtual void VisualPrepareDrawFailed(CAlfVisual& aVisual, TInt aErrorCode);

    /**
     * Shows all the unshown visuals of the control on the specified display.
     * Visuals that are currently shown on another display are not affected.
     *
     * @param aDisplay  Display to add visuals to.
     *
     * @see CAlfControl::BindDisplay()
     */
    IMPORT_C void ShowL( CAlfDisplay& aDisplay );

    /** @endAPI */

    /**
     * Sets the group that owns the control. Called by the control group
     * when the control is added to the group.
     *
     * @param aOwnerGroup  Owner group.
     */
    void SetControlGroup( CAlfControlGroup& aOwnerGroup );

    /**
     * Hides those visuals that are displayed on the specified display. Other
     * visuals are not affected.
     *
     * @param aDisplay  Display to remove visuals from.
     */
    void Hide( CAlfDisplay& aDisplay );

    /**
     * Changes the control's focus state.
     */
    void SetFocus( CAlfDisplay& aDisplay, TBool aHasFocus );

    /**
     * Sets the flag that tells whether the control wants to receive focus.
     */
    void SetFocusing( TBool aFocusing );

    /**
     * Clears the change flags of the control.
     */
    void ClearChanged();


protected:

    /** @beginAPI */

    /* Methods. */

    /**
     * Notifies the control that its visible has been changed on a display.
     * This is the earliest time when the control knows the dimensions of
     * the display it is being shown on.
     *
     * @param aIsVisible  ETrue, if the control is now visible on the display.
     *                    EFalse, if the control is about to the hidden on the display.
     * @param aDisplay    The display on which the control's visibility is changing.
     */
    IMPORT_C virtual void NotifyControlVisibility( TBool aIsVisible, 
                                                   CAlfDisplay& aDisplay );


    /* Utility methods. */

    /**
     * Called when a visual has been added to the control.
     *
     * @param aVisual  Visual that was added.
     */
    IMPORT_C virtual void VisualAddedL( CAlfVisual* aVisual );

    /**
     * Called when a visual has been removed from the control.
     *
     * @param aVisual  Visual that was removed.
     */
    IMPORT_C virtual void VisualRemoved( CAlfVisual* aVisual );

    /**
     * Called when a client control is added to the control.
     *
     * Override this function to get the notification.
     *
     * @param aConnectedControl Connected child control
     * @parar aRole Role of the child control.
     *
     * @see AddConnectionL
     */
    IMPORT_C virtual void ConnectionAddedL( CAlfControl* aConnectedControl, 
                                            TInt aRole );

    /**
     * Called when a client control is removed from the control.
     *
     * Override this function to get the notification.
     *
     * @param aConnectedControl Removed child control
     * @parar aRole Role of the child control.
     *
     * @see RemoveConnection
     */
    IMPORT_C virtual void ConnectionRemoved( CAlfControl* aConnectedControl, 
                                             TInt aRole );

    /**
     * The host control is about to be changed. The control needs to
     * add its visuals to the container layout provided by the new host.
     *
     * Override this function to get the notification. Base call is
     * mandatory. If this function leaves, the connection will not be
     * created.
     *
     * @param aNewHost New host control. If NULL, the host is removed.
     */
    IMPORT_C virtual void HostChangingL( CAlfControl* aNewHost );

    /** @endAPI */
    
    /**
     * @internal
     *
     * Remove this control's visuals from the specified container layout.
     *
     * @param aHostControl Host control.
     */
    void RemoveVisualsFromHostControl( CAlfControl& aHostControl );

protected:

    /** ! future proofing */
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private:
    
    // Private data. Owned.
    struct TPrivateData;
    TPrivateData* iData;

	};

#endif  // C_ALFCONTROL_H
