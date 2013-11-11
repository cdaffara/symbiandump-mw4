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
* Description:   Definition of CHuiRosterImpl. CHuiRosterImpl implements the interface
*                defined by CHuiRoster.
*
*/



#ifndef __HUIROSTERIMPL_H__
#define __HUIROSTERIMPL_H__


#include "uiacceltk/HuiRoster.h"
#include "uiacceltk/huieventhandler.h"
#include "uiacceltk/HuiEvent.h"
#include "uiacceltk/HuiRealPoint.h"
#include "uiacceltk/HuiRealPoint.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/huilowmemoryobserver.h"
#include "huieffectable.h"

/* Forward declarations. */
class CHuiDisplay;
class CHuiLongTapDetector;
class CHuiLayout;
struct THuiPointerEventDetails;

class CHuiFxEngine;
class CHuiFxEffect;

class CHuiCanvasGc;
class CHuiCanvasRenderBuffer;

const TReal32 KDefaultDragTresholdInPixelsX = 4.0;
const TReal32 KDefaultDragTresholdInPixelsY = 4.0;

/**
 * Roster defines a priority order of control groups shown an a display.
 * The roster also offers input events to the groups, and handles the
 * drawing of the groups' root visuals.
 * Implements the CHuiRoster abstract base class.
 *
 * @see CHuiRoster
 */
NONSHARABLE_CLASS(CHuiRosterImpl) : public CHuiRoster, private MHuiEffectable, 
        private MHuiMemoryLevelObserver
	{
public:

	/* Constructors and destructor. */

	/**
	 * Constructor.
	 *
	 * @param aDisplay  Initial display associated with the roster.
	 */
	CHuiRosterImpl(CHuiDisplay* aDisplay);

	/**
	 * Second-phase constructor.
	 */
	void ConstructL();

	/**
	 * Destructor.
	 */
	virtual ~CHuiRosterImpl();


	/* Methods. */
	
	/**
	 * Sets the current display that is using the roster. Multiple displays 
	 * may share a roster, but only one of the them uses it at a time.
	 *
	 * @param aDisplay  Display. Can be <code>NULL</code>.
	 */
	void SetDisplay(CHuiDisplay* aDisplay);
	
    /**
     * Recalculates layout starting from root visuals.
     */
    void SetRect(const TRect& aRect);	

    TRect Rect() const;	

    void ShowL(CHuiControlGroup& aGroup, TInt aWhere = KHuiRosterShowAtTop);
	
	void Hide(CHuiControlGroup& aGroup);
	    
    TInt Find(CHuiControlGroup* aGroup) const;
    
    /**
     * Returns the number of control groups.
     */
    TInt Count() const;    
    
    /**
     * Returns a control group.
     */
    CHuiControlGroup& ControlGroup(TInt aIndex) const;
    
    /**
     * Finds a control inside the roster.
     *
     * @param aControlId  Identifier of the control to find.
     *
     * @return  Pointer to the control.  <code>NULL</code>, if not found.
     */
    CHuiControl* FindControl(TInt aControlId) const;

    /**
     * Prepares the visuals for drawing. All visuals
     * are prepared before the actual drawing begins.
     * 
     * The visuals are prepared in the same
     * order than they get drawn.
     * 
     * @see Draw()
     * 
	 * This default implementation is not
	 * leaving, since it traps the prepare errors
	 * inside and handles the errors by calling
	 * the visual owner prepare draw failed callback.
	 * 
	 * @see MHuiVisualOwner::VisualPrepareDrawFailed()
	 * 
	 * @return ETrue if the prepare succeeded, or EFail if 
	 * the prepare failed. Failing to prepare a draw 
	 * terminates the screen refresh and drawing 
	 * for the current frame.
	 * 
     */
	TBool PrepareDraw() const;
    
    /**
     * Draw the visuals tree. This is done by
     * recursively drawing the root visuals
     * in the order they have been added.
     * 
     * Note that you can affect the drawing order
     * by calling the MoveVisualToFront - methods.
     *
     * @param aGc           Graphics context to draw into.
     */
    void Draw(CHuiGc& aGc) const;

    /**
     * Draw the visuals tree. This is done by
     * recursively drawing the root visuals
     * in the order they have been added.
     * 
     * Note that you can affect the drawing order
     * by calling the MoveVisualToFront - methods.
     *
     * @param aGc           Graphics context to draw into.
     * @param aDisplay Alterative display where visual tree 
     * is to be drawn.
     */
    void Draw(CHuiGc& aGc, CHuiDisplay* aDisplay) const;
    void DrawSelf(CHuiGc& aGc, CHuiDisplay* aDisplay) const;
    void DrawSelfFrozen(CHuiGc& aGc, CHuiDisplay* aDisplay) const;
    
    /**
     * Assigns the visual as a root visual and makes
     * the area of the visual to cover the entire
     * display.
     * 
     * Also, associates Rosters display with the visual.
     * @see HideVisual()
     * @see SetVisualToFullArea()
     */
    void ShowVisualL(CHuiVisual* aVisual);

    /**
     * Adds the visual to the list of root visuals. Also
     * also sets the visual to the full area.
     * 
     * @see ShowVisualL()
     */
    void HideVisual(CHuiVisual* aVisual);
   
    /**
     * Gives input focus to a control.
     */
    void SetFocus(CHuiControl& aControl);
    
    /**
     * Clears the input focus so that no control has focus.
     */
    void ClearFocus();
                
    /**
     * Called when a key event occurs. The input event is offered to the 
     * controls in the order defined by the groups.
     */    
    TBool HandleEventL(const THuiEvent& aEvent);
    
    /**
     * Tell all visuals to post their change notifications.
     */
    void ScanDirty();

    /**
     * Tell all visuals to post their change notifications.
     */
    void ScanTransformedDirty(CHuiGc& aGc, CHuiDisplay* aDisplay);

    /**
     * Moves a root visual to the front inside its roster entry.
     *
     * @param aRootVisual  Visual to move.
     */
    void MoveVisualToFront(CHuiVisual* aRootVisual);

    /**
     * Moves a root visual to the front inside its roster entry.
     *
     * @param aRootVisual  Visual to move.
     */
    void MoveVisualToFront(const CHuiVisual& aRootVisual);

    /**
     * Moves a root visual to the given position.
     *
     * @param aRootVisual  Visual to move.
     */
    void Move(CHuiVisual* aVisual, TInt aPos);
    
    /**
     * Updates the layout positions of all the root visuals in this roster.
     * Typically called on a resolution change.
     */
    void UpdateLayout();
    
    /**
     * Notifies all visuals of a change in the environment's skin.
     */
    void NotifySkinChangedL();
    
    /**
     * Debug help for dumping the visual trees to the console.
     */
    void DumpRootLevelTrees() const;
    /**
     * Clear change flags of the visuals in the roster. This is called by the
     * refresh loop after the frame has been fully drawn.
     */    
    void ClearChanged();
    
    RHuiObserverArray<MHuiEventHandler>& Observers(THuiInputType aInputType);
    
    /**
     * Sets treshold which pointer needs to be moved before starting to send 
     * drag events.Default treshold is zero pixels, so any movement will 
     * trigger drag events.
     *
     * NOTE: Not every unit type is feasible. Use only types like:
     * EHuiUnitPixel, EHuiUnitRelativeToDisplay, EHuiUnitS60
     *
     * @param aEventHandler  Event handler to which treshold affects.
     * @param aXYMetric  Treshold in metric units.
     */
    void SetPointerDragThresholdL(const MHuiEventHandler& aEventHandler, const THuiXYMetric& aXYMetric);

    /**
     * Disables long tap events when dragging is going on. Default 
     * functionality is that long tap event will be delivered simultaneously
     * with drag events. 
     *
     * @param aEventHandler  Event handler to which disabling goes.
     * @param aDisable  Boolean to indicate whether feature is disabled or not.
     */
    void  DisableLongTapEventsWhenDraggingL(const MHuiEventHandler& aEventHandler, TBool aDisable = ETrue);
	
    /**
     * Tells if this roster has control groups which have transformations to be applied.
     * @return ETrue if transformations exist for any control group, otherwise EFalse
     */
	TBool HasTransformedControlGroups() const;
	
    /**
     * Enables or disbles drawing external content to canvas visuals. This can be set during drawing the visual hierarchy.
     *
     * @param aEnable Enable or disable external content. 
     */
	void SetDrawingExternalContent(TBool aEnable);
	
    /**
     * Returns whether external content can be drawn to canvas visuals.
     *
     * @return Content drawing state. 
     */
	TBool IsDrawingExternalContent() const;

    /**
     * Removes the external content visual from canvas visuals.
     *
     * @param aExternalContentVisual External content visual.
     */
	void RemoveExternalContentVisualFromParentL(CHuiVisual* aExternalContentVisual);

	/**
	 * Removes the external content visual from canvas visuals. Recursive method.
	 *
	 * @param aExternalContentVisual External content visual.
	 */
	void RemoveExternalContentVisualFromParentL(CHuiVisual* aExternalContentVisual, CHuiLayout* aLayout);
	
	void FreezeVisibleContentL(TBool aFreeze = ETrue);
	
	TBool IsVisibleContentFrozen() const;
	
	// From MHuiEffectable
    void EffectSetEffect(CHuiFxEffect* aEffect);
    TReal32 EffectOpacityTarget() const;
    void EffectSetOpacity(TReal32 aOpacity);
    void EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const;
    THuiRealRect EffectDisplayRect() const __SOFTFP;
    void SetLoadingEffect(TBool aLoading);
    void EffectSetSource( TBool aIsInput1 );
    TBool EffectGetSource() const;	
    
    // From MHuiMemoryLevelObserver
    void SetMemoryLevel(THuiMemoryLevel aLevel);
    
	// For handling native ALF applications drawing 
    void SetAlfEventWindow(CHuiVisual* aVisual);
    const CHuiVisual* AlfEventWindow();
    TBool NativeAppsContentChanged();
    void DrawNativeAppsContent( CHuiGc &aGc, CHuiDisplay* aDisplay) const;
    
protected:

    /* Methods. */

	/**
	 * Appends a new control group on top of the roster.
	 *
	 * @param aGroup  Control group to add.
	 */
	void AppendL(CHuiControlGroup* aGroup);
	
	/** 
	 * Inserts a control group into a specific position in the roster.
	 *
	 * @param aGroup  Control group.
	 * @param aPos    Position to insert into.
	 *
	 * @leave KErrAlreadyExists  The control group is already in the roster.
	 */
    void InsertL(CHuiControlGroup* aGroup, TInt aPos);

    /**
     * Moves an existing control group into a new position in the roster.
     *
     * @param aPos  Position to move into.
     */
    void Move(CHuiControlGroup* aGroup, TInt aPos);

	/**
	 * Removes a control group from the roster.
	 * 
	 * @param aGroup  Control group to remove.
	 */
	void Remove(CHuiControlGroup* aGroup);

    /**
     * Update positions of root visuals to fit the entire display area.
     */
    void SetVisualToFullArea(CHuiVisual* aVisual);
    
    TBool OfferPointerEventWithinTreeL(CHuiVisual& aVisual, const THuiEvent& aEvent, TBool aUseDispRect = EFalse);
	
    MHuiEffectable* Effectable();

private:

    /* Private data structures. */


private:
    
    /* Private methods. */

    /**
     * Finds the entry for the visual's control group.
     *
     * @return  Reference to roster entry.
     *
     * @panic  
     */
    CHuiRosterEntry* EntryForVisual(const CHuiVisual* aVisual);

    /**
     * Changes the control that has input focus.
     */
    void ChangeInputFocus(CHuiControl* aControl);
 
    /**
     * Determines if the roster has changed so much that everything needs to
     * be redrawn when the next refresh occurs.
     */
    TBool Changed() const;
    
    /**
     * Flags the roster as changed or not changed.
     *
     * @param aIsChanged  ETrue, if the roster should be marked as changed.
     */
    void SetChanged(TBool aIsChanged = ETrue);
    
    /**
     * Utilizes tactile feedbacks. Called on the pointer event.
     *
     * @param aEvent Pointer event.
     */
    void HandleTactileFeedback( const THuiEvent& aEvent );



	/**
	 * Adds new event detail struct to the iPointerEventDetails
	 */
	void AddEventDetailsL(MHuiEventHandler* aEventHandler, const THuiXYMetric& aXYMetric, TBool aDisable);

	/**
	 * Cleans iPointerEventDetails from details that are not used by
	 * any observer
	 */
	void CleanEventDetails();
	
	/**
	 * Checks if event handler is within threshold range.
	 */
	TBool DragEventOutOfRangeL(MHuiEventHandler* aEventHandler, const THuiEvent& aEvent);
	
	/**
	 * Checks if long tap event allowed in current state. Checks if there is already
	 * drag events sent and if long tap event are disabled during dragging.
	 */
	TBool IsLongTapEventAllowed(MHuiEventHandler* aEventHandler);
	
	/**
	 * Cleans dragging flags from iPointerEventDetails
	 */
	void CleanDraggingFlags();
	
    /**
     * Traverses through visual tree and draws outline for visual
     */
	void DrawBoundaries( CHuiGc& aGc, CHuiVisual* aVisual, TBool aDrawOutline ) const;

    /**
     * Returns true if render buffer can be used for freeze.
     */
    TBool UseRenderBufferForFreeze() const;
    
private:

    /** Area occupied by the roster. */
    TRect iRect;

    CHuiDisplay* iDisplay;

    /** List of control groups. None of the groups is owned by the roster. */
    //RPointerArray<CHuiControlGroup> iGroups;
    
    RPointerArray<CHuiRosterEntry> iEntries;
    
    /** One of the controls in the roster may have input focus. */
    CHuiControl* iInputFocus;
    
    /** Array of root visuals. All the visuals of the visible controls 
        are added here (as direct children or indirect descendants). */
    //RPointerArray<CHuiVisual> iRootVisuals;

    /** Changed flag for the roster itself. */
    TBool iChanged;
    
    RHuiObserverArray<MHuiEventHandler> iPointerDragObservers;
    RHuiObserverArray<MHuiEventHandler> iUnhandledPointerObservers;
    RHuiObserverArray<MHuiEventHandler> iLongTapObservers;
    
    /** For detecting long pointer events. */
    CHuiLongTapDetector* iLongTapDetector;
    
    TBool iTactileFeedbackHandled;

    /** List of event details how to handle events */
    RPointerArray<THuiPointerEventDetails> iPointerEventDetails;
    /** Initial event when starting dragging */    
    THuiEvent iInitialDownEvent;    
    
    TBool iDrawEvenIfDisabledDrawInRoster; 

    /** Effect */
    CHuiFxEffect* iEffect;
    TReal32 iEffectOpacity;
    TBool iIsInput1;

	/** Render buffer */
    CHuiCanvasGc* iCanvasGc;
    CHuiCanvasRenderBuffer* iCanvasRenderBuffer;
    CHuiGc::TOrientation iCanvasRenderBufferOrientation;
	
	TBool iRosterIsFrozen;
	TBool iMonitorMemoryLevel;
	
	/* not owned */
	CHuiVisual* iAlfEventWindowVisual;
	TBool iPreviousAlfContentDrawn;
	};

#endif  // __HUIROSTERIMPL_H__
