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
* Description:   CHuiControl provides a base class for a generic logical control 
*                element in the HUITK UI.
*
*/



#ifndef __HUICONTROL_H__
#define __HUICONTROL_H__


#include <e32base.h>
#include <barsread.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiLayout.h>
#include <uiacceltk/HuiSessionObject.h>
#include <uiacceltk/huieventhandler.h>

/* Forward declarations. */
class CHuiEnv;
class CHuiTextureManager;
class CHuiDisplay;
class THuiEvent;
class CHuiControlGroup;


/**
 * CHuiControl provides a base class for gathering logical functionality in
 * the UI.
 *
 * For example a list control could gather (and _control_) all visuals that form
 * a listbox UI element. A thumbnail view control could be used to control visuals
 * that form a thumbnail view in an image gallery application.
 *
 * Their main use is to handle interaction with the user - Controls handle input
 * events by overriding OfferEventL().
 *
 * Controls are not visible by themselves, but controls create and manipulate
 * visuals (@see CHuiVisual) - set target values for visual properties and/or
 * schedule animation commands.
 *
 * Controls are collected in groups (e.g., one group per view), but do not have a
 * hierarchical organization. However controls can be connected to each other
 * for collaboration between multiple controls. @see AddConnectionL()
 */
class CHuiControl : public CBase, 
                    public MHuiVisualOwner, 
                    public MHuiEventHandler,
                    public MHuiSessionObject
	{
public:

	/** @beginAPI */
	
	/* Constructors and destructor. */

	/**
	 * Constructs this control. An unique ID for this control is
	 * generated for this control using CHuiStatic::GenerateId();
	 * @see Id()
	 * 
	 * @todo This constructor is public, explain rationale.
	 */
	IMPORT_C CHuiControl(CHuiEnv& aEnv);

	/**
	 * Destructs this control. Cancels all scheduled commands for this control.
     * Removes the host and any other connections (if any) and destroys
     * the visuals owned by this control.
	 */
	IMPORT_C ~CHuiControl();

    void ConstructL()
        {
        BaseConstructL();
        }

	/* Methods. */

	/**
	 * Set an id value for this control.
	 */
	IMPORT_C void SetId(TInt aId);

	/**
	 * Returns the id of this control. Zero (0) id the control has no id.
	 */
	IMPORT_C TInt Id() const;

	/**
	 * From MHuiVisualOwner. Returns the environment of the visual.
	 */
	IMPORT_C CHuiEnv& Env() const;

	/**
	 * From MHuiVisualOwner. Returns the control group of the visual's owner control.
	 *
	 * @return  Control group, or <code>NULL</code> if there isn't one.
	 */
	IMPORT_C CHuiControlGroup* ControlGroup() const;

    /**
     * Returns the texture manager of the environment of the visual.
     */
    IMPORT_C CHuiTextureManager& TextureManager() const;

    /**
     * Returns the display this control is bound to, or <code>NULL</code>.
     */
    IMPORT_C CHuiDisplay* Display() const;

    /**
     * Binds the control to a display. Called automatically by 
     * CHuiControl::ShowL().
     */
    IMPORT_C void BindDisplay(CHuiDisplay& aDisplay);

	/**
	 * From MHuiVisualOwner. Appends a visual to the control. The control also receives ownership
	 * of the visual. This method is specifically intended to be used when
	 * adding previously created visuals to the control.
	 *
	 * @param aVisual  Visual to add. Control gets ownership.
	 */
    IMPORT_C void AppendL(CHuiVisual* aVisual);

	/**
	 * Appends a new visual to the list of visuals owned by the control.
	 * This method is specifically intended to be used during construction
	 * of visual trees.
	 *
	 * @param aVisual  Visual to append.
	 */
	IMPORT_C void AppendL(CHuiVisual* aVisual, CHuiLayout* aParentLayout);

	/**
	 * From MHuiVisualOwner. Removes a visual from the control. The caller also receives ownership
	 * of the visual.
	 *
	 * @param aVisual  Visual to remove. Caller gets ownership.
	 */
	IMPORT_C void Remove(CHuiVisual* aVisual);

	/**
	 * Creates a new visual using the visual factory and appends it.
	 *
	 * @param aVisualType  Type of the visual to create.
	 *
	 * @return  Pointer to the created visual.
	 */
    IMPORT_C CHuiVisual* AppendVisualL(THuiVisualType aVisualType,
                                       CHuiLayout* aParentLayout=NULL);

    /**
     * Creates a new layout using the visual factory and appends it.
     *
     * @param aLayoutType  Type of the layout to create.
     *
     * @return  Pointer to the created layout.
     */
    IMPORT_C CHuiLayout* AppendLayoutL(THuiLayoutType aLayoutType,
                                       CHuiLayout* aParentLayout=NULL);

    /**
     * Returns one of the visuals owned by the control.
     *
     * @param aIndex  Index number of the visual to return.
     *
     * @return  Visual.
     */
    IMPORT_C CHuiVisual& Visual(TInt aIndex) const;

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
    IMPORT_C CHuiVisual* FindTag(const TDesC8& aTag) const;
      
	/**
	 * Returns the visual host control.
	 *
	 * @return  Host control. NULL if not connected.
	 */
	IMPORT_C CHuiControl* Host() const;

	/**
	 * New virtual method. Establishes a connection between this control and another control.
	 * Instead of supporting a treelike parent-child control hierarchy, Hitchcock
	 * allows defining a set of links between any controls. These links are called
	 * connections. Connections can be used to construct a parent-child
     * based hierarchy of controls, if that is considered necessary from the point
     * of view of input event handling or some other data passing scenario. They
     * can be utilized for observation purposes or keeping track of associated controls.
	 *
	 * @param aConnectedControl  Control to connect.
	 * @param aRole              Role of the control. Interpretation depends on
	 *                           the host.
	 * @see HUITK Programmer's Guide for examples of how to connect controls.
	 */
	IMPORT_C virtual void AddConnectionL(CHuiControl* aConnectedControl, TInt aRole=0);

	/**
	 * Removes a client.
	 *
	 * @param aConnectedControl  Connected control to remove.
	 */
	IMPORT_C void RemoveConnection(CHuiControl* aConnectedControl);

    /**
     * Find a client's index number.
     *
     * @param aConnected  Connected control to find.
     *
     * @return  Index number.
     */
    IMPORT_C TInt FindConnection(const CHuiControl* aConnected) const;

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
    IMPORT_C CHuiControl& Connection(TInt aIndex) const;

    /**
     * @deprecated
     *
     * Returns a client control.
     *
     * @param aOrdinal  Role or index.
     */
    IMPORT_C CHuiControl& ConnectionByOrdinal(TInt aOrdinal) const;

	/**
	 * Returns the role of a client control.
	 *
	 * Roles are defined by the application developer - a user interface framework
	 * built on top of Hitchcock can define a suitable set of roles for the controls
	 * implemented in the framework.
	 *
	 * @param aIndex  Index of the client.
	 *
	 * @return  Role of the client control.
	 * @see AddConnectionL()
	 */
    IMPORT_C TInt ConnectionRole(TInt aIndex) const;

    /**
     * @deprecated
     *
     * Ordinal is the effective index. Role overrides index.
     * @see AddConnectionL()
     */
    IMPORT_C TInt ConnectionOrdinal(TInt aIndex) const;

    /**
     * Returns the control's role.
     * 
     * @return Role. Zero by default.
     *
     * @see AddConnectionL()
     */
    IMPORT_C TInt Role() const;

    /**
     * Sets the control's role.
     * 
     * @param aRole Connection role.
     *
     * @see AddConnectionL()
     */
    IMPORT_C void SetRole(TInt aRole);

    /**
     * Returns the automatic visual host identification.
     *
     * @return Automatic visual host identification. Zero id not set.
     */
    IMPORT_C TInt HostId() const;

    /**
     * Sets the automatic visual host identification (non zero).
     * Needs to be unique in the CHuiEnv. If using this automatic mechanism, 
     * do not use the AddConnectionL() function - it will be automatically 
     * called when this control is shown.
     *
     * @return Automatic visual host identification.
     */
    IMPORT_C void SetHostId(TInt aHostId);

	/**
	 * @internal
	 *
	 * Do not call outside the toolkit library!
	 *
	 * Sets the host of the control. This will be called by the AddConnectioL()
	 *
	 * @param aHost Host control.
	 */
    IMPORT_C void SetHost(CHuiControl* aHost);

    /**
     * Returns a container layout for the specified child control. 
     * A container layout contains all the visuals of a child control.
     *
     * @param aConnected Child connection, which is querying for the visual host.
     * 
     * @return Container layout for the child controls' root visuals.
     */
    IMPORT_C virtual CHuiLayout* ContainerLayout(const CHuiControl* aConnected) const;

    /**
     * Coordinate conversion.
     */
    IMPORT_C TPoint HostToDisplay(const TPoint& aPoint) const;

    /**
     * Coordinate conversion.
     */
    IMPORT_C TPoint DisplayToHost(const TPoint& aPoint) const;

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
    IMPORT_C TBool HitTest(const TPoint& aPoint) const;

    /**
     * Acquires focus for this control.
     */
    IMPORT_C void AcquireFocus();

    /**
     * Determines whether the control has input focus.
     */
    IMPORT_C TBool Focus() const;

    /**
     * New virtual method. Determines whether the control wants to receive focus.
     */
    IMPORT_C virtual TBool IsFocusing() const;

    /**
     * New virtual method. Called when the control's focus state changes.
     */
    IMPORT_C virtual void FocusChanged(CHuiDisplay& aDisplay, TBool iFocused);

    /**
     * Determines whether the control accepts input events.
     */
    IMPORT_C TBool AcceptInput() const;

    /**
     * From MHuiEventHandler. Called when an input event is being offered to the control.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  <code>ETrue</code>, if the event was handled.
     *          Otherwise <code>EFalse</code>.
     */
    IMPORT_C TBool OfferEventL(const THuiEvent& aEvent);

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
    IMPORT_C THuiRealPoint DisplayCenter() const __SOFTFP;

    /**
     * Cancel all scheduled commands related to this control and the control's
     * visuals.
     */
    IMPORT_C void CancelAllCommands();

    /**
     * From MHuiVisualOwner. Notifies the control that one of the visuals it ows has been laid out.
     * Called automatically from CHuiVisual::UpdateChildrenLayout().
     * For example, occurs when a parent layout is resized.
     *
     * @param aVisual  Visual whose layout was updated.
     */
    IMPORT_C void VisualLayoutUpdated(CHuiVisual& aVisual);
    
    /**
     * From MHuiVisualOwner. Called when a visual owned by the control is being destroyed.
     * Visual is removed from the control.
     *
     * @param aVisual  Visual about to the destroyed.
     */
    IMPORT_C void VisualDestroyed(CHuiVisual& aVisual);

	/**
	 * From MHuiVisualOwner. Called when there was an error (a leave happened)
	 * while preparing one of the controls visual for drawing. 
	 * Typically the resource alloc errors such as out of 
	 * memory is escalated through this callback.
	 * 
	 * By default this callback causes a panic with the
	 * error code, so you should override this if you
	 * want different behavior.
	 * 
     * @param aVisual    Visual that caused the error.
     * @param aErrorCode Symbian KErr error code for the failure.
	 */
    IMPORT_C void VisualPrepareDrawFailed(CHuiVisual& aVisual, TInt aErrorCode); 

    /**
     * From MHuiVisualOwner. Returns a pointer to the event handler responsible for handling
     * events related this control's visuals (i.e. the handler is this
     * control).
     */
    IMPORT_C MHuiEventHandler* EventHandler();

    /** @endAPI */


    /**
     * Sets the group that owns the control. Called by the control group
     * when the control is added to the group.
     *
     * @param aOwnerGroup  Owner group.
     */
    void SetControlGroup(CHuiControlGroup& aOwnerGroup);

    /**
     * Shows all the unshown visuals of the control on the specified display.
     * Visuals that are currently shown on another display are not affected.
     *
     * @param aDisplay  Display to add visuals to.
     *
     * @see CHuiControl::BindDisplay()
     */
    void ShowL(CHuiDisplay& aDisplay);

    /**
     * Hides those visuals that are displayed on the specified display. Other
     * visuals are not affected.
     *
     * @param aDisplay  Display to remove visuals from.
     */
    void Hide(CHuiDisplay& aDisplay);

    /**
     * Changes the control's focus state.
     */
    void SetFocus(CHuiDisplay& aDisplay, TBool aHasFocus);

    /**
     * Sets the flag that tells whether the control wants to receive focus.
     */
    void SetFocusing(TBool aFocusing);

    /**
     * Clears the change flags of the control.
     */
    void ClearChanged();


protected:

    /** @beginAPI */

    /* Methods. */

    /**
     * New virtual method. Notifies the control that its visible has been changed on a display.
     * This is the earliest time when the control knows the dimensions of
     * the display it is being shown on.
     *
     * @param aIsVisible  ETrue, if the control is now visible on the display.
     *                    EFalse, if the control is about to the hidden on the display.
     * @param aDisplay    The display on which the control's visibility is changing.
     */
    IMPORT_C virtual void NotifyControlVisibility(TBool aIsVisible, CHuiDisplay& aDisplay);


    /* Utility methods. */

    /**
     * New virtual method. Called when a visual has been added to the control.
     *
     * @param aVisual  Visual that was added.
     */
    IMPORT_C virtual void VisualAddedL(CHuiVisual* aVisual);

    /**
     * New virtual method. Called when a visual has been removed from the control.
     *
     * @param aVisual  Visual that was removed.
     */
    IMPORT_C virtual void VisualRemoved(CHuiVisual* aVisual);

    /**
     * New virtual method. Called when a client control is added to the control.
     * 
     * @see AddConnectionL()
     */
    IMPORT_C virtual void ConnectionAddedL(CHuiControl* aConnectedControl, TInt aRole);

    /**
     * New virtual method. Called when a client control is removed from the control.
     *
     * @see RemoveConnection()
     */
    IMPORT_C virtual void ConnectionRemoved(CHuiControl* aConnectedControl, TInt aRole);

    /**
     * New virtual method. The host control is about to be changed. The control needs to
     * add its visuals to the container layout provided by the new host.
     *
     * @see SetHost()
     */
    IMPORT_C virtual void HostChangingL(CHuiControl* aNewHost);

    /**
     * Virtual second phase constructor. Must be base called in deriving class.
     */
    IMPORT_C virtual void BaseConstructL();

    IMPORT_C virtual void ControlExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

   /** @endAPI */
 

protected: // from MHuiSessionObject
    
    /**
     * ! Gets the object type.
     */
    IMPORT_C TType Type() const;

    /**
     * ! Gets the session id for object.
     */
    IMPORT_C TInt SessionId() const;

    /**
     * ! Sets the session id for object.
     */
    IMPORT_C void SetSessionId(TInt aSessionId);


public: // internal utilities    
    
    /**
     * Remove this control's visuals from the specified container layout.
     *
     * @param aHostControl Host control.
     */
    void RemoveVisualsFromHostControl( CHuiControl& aHostControl );

private:

    /** The environment where this visual belong to. */
    CHuiEnv& iEnv;

    /** The group into which this control belongs to. */
    CHuiControlGroup* iOwnerGroup;

    /** The display this control is currently bound to. */
    CHuiDisplay* iBoundDisplay;

    /** Identifier of the control. */
    TInt iId;

    /** Identifier of the host control. */
    TInt iHostId;

    /** Control's role. */
    TInt iRole;

    /** Visuals owned by the control. */
    RPointerArray<CHuiVisual> iVisuals;

    /** Host control. The host provides a container visual for this control's
        visuals. */
    CHuiControl* iHost;

    /** Holds information about a client control. */
    struct SConnection
        {
        /** Connected control. */
        CHuiControl* iControl;

        /** Role of the client. */
        /** @todo The role here is redundant because each control knows
                  its role? */
        TInt iRole;
        };

    /** Connected controls and their roles. */
    RArray<SConnection> iConnections;

    /** ETrue, if the control wants to receive input focus. */
    TBool iFocusing;

    /** Control has input focus. */
    TBool iHasFocus;

    /** Session id */
    TInt iSessionId;

    TAny* iSpare;
	};

#endif  // __HUICONTROL_H__
