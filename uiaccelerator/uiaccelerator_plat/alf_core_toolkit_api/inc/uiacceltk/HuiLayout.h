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
* Description:   Defines CHuiLayout class. Layouts are visuals that manage 
*                the placement of a set of child visuals.
*
*/



#ifndef __HUILAYOUT_H__
#define __HUILAYOUT_H__


#include <e32base.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/huimetric.h>

/* Forward declarations */
class CHuiEnv;
class CHuiControl;


/** Layout types. */
enum THuiLayoutType
    {
    EHuiLayoutTypeLayout,
    EHuiLayoutTypeFlow,
    EHuiLayoutTypeGrid,
    EHuiLayoutTypeApp,
    EHuiLayoutTypeAnchor
    };

/**
 * Layouts are visuals that manage the placement of a set of child visuals
 * (or other layouts).
 *
 * Layouts are responsible for setting the positions and sizes of their
 * child visuals. Each layout can define a position, a size, or both
 * (or neither) for their children.
 *
 * Layouts use the children's ordinal numbers to determine where each of
 * them should be placed. For example, a grid layout could define that the
 * top row contains children with ordinals 0..3, from left to right. The
 * second row could then contain the children 4..7. When the order of the
 * child visuals changes within their parent layout, the parent layout
 * will recalculate the positions and sizes of the affected children.
 *
 * When the size of a layout changes, it will immediately recalculate the
 * layout of its children. The transition time that is specified in the
 * size transition of the layout is used when setting the targets of the
 * children's new positions and sizes.
 *
 * Layouts are able to notify their controls when their size is changed.
 * These notifications are needed because the size of a layout is not known
 * until the display is refreshed. This means that the <strong>size of a layout
 * is not available when your controls ConstructL is executed</strong>, unless
 * it is explicitly set. The notifications are also handy because they allow
 * the controls to respond to dynamic screen size changes. So unless if the
 * whole screen is not used for the layout, the notifications are the only way
 * to do calculations based on the layout size.
 *
 * To make the control receive layout update notifications call:
 *
 * <code>
 * layout->SetFlags(EHuiVisualFlagLayoutUpdateNotification);
 * </code>
 *
 * .. and override VisualLayoutUpdated() method in your control.
 *
 * @see SetSize()
 * @see SetFlags()
 * @see CHuiControl::VisualLayoutUpdated()
 *
 * Each layout defines its own local coordinate system used for interpreting
 * the positions and sizes of its child visuals. The base unit of the 
 * coordinate system is defined with the SetBaseUnit() method, which sets a
 * THuiXYMetric to use as the basis for all coordinate calculations. The default
 * metric is [1px, 1px] which corresponds to pixels on the display.
 *
 * The local coordinate system of a layout can be mirrored horizontally and/or
 * vertically by using a negative magnitude in the relevant THuiMetric of the
 * base unit. Note that this will only affect the local coordinate system 
 * of the layout: the contents of the child visuals or child layouts will not 
 * be mirrored. Visual mirroring should be done with a CHuiTransformation,
 * and child layouts can be mirrored by further application of the negative
 * base unit metrics. In order to enable a layout to automatically mirror the 
 * horizontal component of the base unit based on locale mirroring, set the visual flag 
 * @c EHuiVisualFlagAutomaticLocaleMirroringEnabled, note that it is necessary
 * to set this flag for each layout for which this feature is required.
 *
 * To create to a fully scalable UI, the appropriate base unit metrics and
 * layout visuals must be used. For example, the EHuiUnitS60 metric unit is
 * dependent on the platform-specified measurement units for layout. 
 * CHuiGridLayout and CHuiAnchorLayout provide support for pixel-independent 
 * placement of their children.
 *
 * @see CHuiAnchorLayout
 * @see CHuiGridLayout
 *
 * Applications that need more complex layouts should derive their own
 * CHuiLayout based classes that implement the necessary layout calculations.
 * In many cases, however, a combination of some of the built-in layout classes
 * such as anchor and grid layouts will be sufficient.
 */
class CHuiLayout : public CHuiVisual
    {
public:

    enum TExpansionFlags
        {
        /** Not expanding in any direction. */
        EExpandNone = 0,

        /** Expands horizontally. */
        EExpandHorizontally = 1,

        /** Expands vertically. */
        EExpandVertically = 2
        };


    /** @beginAPI */

    /* Constructors and destructor. */

    /**
     * Construct a new basic layout and give its ownership to a control.
     * Note that CHuiLayout has no real layout functionality: its children
     * must be laid out manually.
     *
     * @param aOwnerControl  Owner control.
     * @param aParentLayout  Parent layout for the new layout.
     */
    IMPORT_C static CHuiLayout* AddNewL(CHuiControl& aOwnerControl,
                                        CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CHuiLayout(MHuiVisualOwner& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiLayout();

    /**
     * Second-phase constructor from CHuiVisual, must be called in deriving class
     */
    IMPORT_C void ConstructL();



    /* Methods. */

    /**
     * From CHuiVisual. Removes this layout from its owner and and destroys the layout.
     * Also removes the entire hierarchy of child visuals and destroys all
     * of them.
     */
    IMPORT_C void RemoveAndDestroyAllD();

    /**
     * From CHuiVisual. Finds a visual whose tag descriptor contains a specific tag.
     * Checks self first, then looks through the child visuals.
     *
     * CHuiLayout::FindTag() extends upon the basic implementation
     * in CHuiVisual::FindTag() by adding recursive visual hierarchy
     * searching.
     *
     * @param aTag  Tag to look for. If this tag is found as a part of a
     *              visual's colon-separated tag descriptor, it is treated
     *              as a match and the visual is returned.
     *              The tag to look for cannot contain separator characters (:).
     *
     * @return  Visual whose tag descriptor matches the tag. <code>NULL</code>,
     *          if no such visual could be found. If a visual is returned,
     *          it will either be this visual or one of its descendants.
     *
     * @see CHuiVisual::Tag()
     * @see CHuiVisual::SetTagL()
     * @see CHuiVisual::IsTagged()
     * @see CHuiVisual::FindTag()
     */
    IMPORT_C CHuiVisual* FindTag(const TDesC8& aTag);

    /**
     * Append a new visual to the layout. If the visual is already a member of
     * some other layout, it will be removed from the old layout first.
     *
     * @param aVisual  Visual to append.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.     
     *
     * @todo  Add a method for appending multiple visuals in one set, so that
     *        they only cause a single layout update.
     */
    IMPORT_C void AppendL(CHuiVisual* aVisual, TInt aLayoutTransitionTime = 0);

    /**
     * Insert a new visual to the layout, at specified position (position 0 is the first,
     * Count() -1 is the last). If the visual is already a member of
     * some other layout, it will be removed from the old layout first.
     *
     * @param aVisual  Visual to append.
     * @param TInt     Position to insert. This is the new ordinal number of the visual.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.     
     */
    IMPORT_C void InsertL(CHuiVisual* aVisual, TInt aPosition, TInt aLayoutTransitionTime = 0);

    /**
     * Remove a visual from the layout.
     *
     * @param aVisual  Visual to remove.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.     
     * @todo  Add a method for removing multiple visuals in one set, so that
     *        they only cause a single layout update.
     */
    IMPORT_C void Remove(CHuiVisual* aVisual, TInt aLayoutTransitionTime = 0);

    /**
     * From CHuiVisual. Returns the number of child visuals in the layout.
     */
    IMPORT_C TInt Count() const;

    /**
     * From CHuiVisual. Returns a child visual.
     *
     * @param aIndex  Index of the child visual.
     *
     * @return  Reference to a child visual.
     */
    IMPORT_C CHuiVisual& Visual(TInt aIndex) const;

    /**
     * Finds a visual's index.
     *
     * @param aVisual  Visual to look for.
     *
     * @return  Index of the visual, or <code>KErrNotFound</code>.
     */
    IMPORT_C TInt FindVisual(const CHuiVisual* aVisual) const;

    
     /**
     * From CHuiVisual. Sets the position of the layout.
     * @param aPos             Target position, in the local coordinate system as
     *                         defined by the parent layout (or the display, 
     *                         if there is no parent).
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C void SetPos(const THuiRealPoint& aPos, TInt aTransitionTime = 0);

    /**
     * From CHuiVisual. Change the size of the layout. Children's positions and sizes are
     * updated accordingly. Each layout class is responsible for determining
     * how to update children's layout.
     *
     * @param aSize                 Size to set the layout to.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.     
     */
    IMPORT_C void SetSize(const THuiRealSize& aSize, TInt aTransitionTime = 0);

    /**
     * From CHuiVisual. Update the layout of all children. Subclasses can call this to
     * recalculate the layout of all children.
     *
     * @param aTransitionTime  Time for layout transition.
     */
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);


    /**
     * New virtual method. Update the layout of one child visual.
     *
     * @param aIndex           Child visual index.
     * @param aTransitionTime  Time for layout transition.
     */
    IMPORT_C virtual void UpdateChildLayout(TInt aIndex, TInt aTransitionTime = 0);

    /**
     * Sets the inner padding of the layout. This is typically the amount
     * of empty space between child visuals in the layout.
     *
     * @param aInnerPadding  Inner padding (e.g., column, row separation).
     */
    IMPORT_C void SetInnerPadding(const TPoint& aInnerPadding);

    /**
     * Sets the inner padding of the layout. This is typically the amount
     * of empty space between child visuals in the layout. 
     *
     * @note as paddings are relative to the layout visual, if relative units are required, 
     * it is advisable to use EHuiUnitRelativeToMySize so that the paddings are relative
     * to the size of this layout visual
     *
     * @param aInnerPadding  Inner padding, using metrics.
     */
    IMPORT_C void SetInnerPadding(const THuiXYMetric& aInnerPadding);

    /**
     * Returns the current inner padding.
     */
    IMPORT_C const THuiXYMetric& InnerPadding() const;

    /**
     * Returns the inner horizontal padding. 
     * @deprecated This is useless, use InnerPaddingInBaseUnits() instead.
     */
    IMPORT_C TInt HorizontalInnerPadding() const;

    /**
     * Returns the inner vertical padding.
     * @deprecated This is useless, use InnerPaddingInBaseUnits() instead.
     */
    IMPORT_C TInt VerticalInnerPadding() const;

    /**
     * Returns the inner padding converted to the base units of this layout.
     * These can be summed with the positions and sizes of the layout's children.
     */
    IMPORT_C THuiRealPoint InnerPaddingInBaseUnits() const __SOFTFP;

    /**
     * Enables or disables scrolling in the layout. An offset
     * for the origin of the layout can be defined using 
     * ScrollOffset() - method and manipulating the THuiTimedPoint
     * returned by that method.
     * 
     * Scroll offset, unlike other layout parameters, is not directly applied 
     * to layout children when UpdateChildrenLayout() is called. Instead, the 
     * scroll offset is taken into consideration when individual child visual 
     * display rectangles are calculated. This way the scroll offset is directly 
     * applied to dirty rectangle calculations as well as to visual content
     * rectangle calculations when visuals are drawn.
     *
     * @param aScrolling  <code>ETrue</code> to enable scrolling,
     *                    <code>EFalse</code> to disable.
     */
    IMPORT_C void EnableScrollingL(TBool aScrolling = ETrue);

    /**
     * Indicates whether scrolling is enabled in this layout.
     * @see EnableScrolling()
     *
     * @return ETrue if scrolling is enabled, EFalse otherwise.
     */
    IMPORT_C TBool Scrolling() const;

    /**
     * Returns the scroll offset of the layout. The scroll offset is in
     * layout's base units. The scroll offset has to be enabled before this 
     * method is called.
     * @see EnableScrollingL()
     *
     * @return  Reference to timed point that defines the scroll offset.
     *
     * @panic THuiPanic::ELayoutNotScrolling  Scrolling is not enabled.
     */
    IMPORT_C THuiTimedPoint& ScrollOffset();
    IMPORT_C const THuiTimedPoint& ScrollOffset() const;
    
    /**
     * Specifies the scroll offset of this layout. The scroll offset
     * is defined in layout's base units. The scroll offset has to be 
     * enabled before this method is called.
     * @see BaseUnit()
     * @see EnableScrollingL()
     *
     * @param aOffset Offset in layout's base units.
     * @param aTransitionTime Time when the offset is to be reached in milliseconds.
     */
    IMPORT_C void SetScrollOffsetInBaseUnits(const THuiRealPoint& aOffset, TInt aTransitionTime = 0);

    /**
     * Determines the virtual size of the layout. The virtual size may be
     * larger than the actual size. The virtual size is updated automatically
     * when the layout's children are positioned.
     *
     * @return  Virtual size.
     *
     * @panic THuiPanic::ELayoutNotScrolling  Scrolling is not enabled.
     */
    IMPORT_C TSize VirtualSize() const;

    /**
     * Layout is calculated according the ordinals. The effective ordinal
     * may not be the same as the real child ordinal, because some visuals
     * are laid out manually.
     *
     * @return  Effective ordinal for the child visual.
     *
     * @panic  THuiPanic::ELayoutChildVisualNotFound  aVisual is not a child
     *         of this layout.
     */
    IMPORT_C TInt EffectiveLayoutOrdinal(const CHuiVisual& aVisual) const;

    /**
     * Moves the given child visual to specific position in the layout's list
     * of child visuals. Affects the drawing order (last child is drawn last),
     * and possibly the final laid out position and size of the visual.
     * This is due to that many layout implementations calculate the
     * positioning of their children based on child's order number
     * in the list.
     *
     * Causes layout recalculation.
     *
     * @param aVisual    Child visual to move.
     * @param aPosition  New position (and ordinal number)
     * 					 in the child list (0 = first, Count()-1 = last).
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.
     * @see MoveVisualToBack()
     * @see MoveVisualToFront()
     */
    IMPORT_C void Reorder(CHuiVisual& aVisual, TInt aPosition, TInt aLayoutTransitionTime = 0);

    /**
     * Moves a child visual to the front of the other children. In other words
     * the visual is drawn last (on top of the other child visuals). In practise
     * this affects the order of the visuals whithin this layout so that the
     * visual is moved at the end of the list.
     *
     * Causes layout recalculation.
     *
     * @param aVisual  Child visual to move.
     * @see MoveVisualToBack()
     * @see Reorder()
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.     
     *
     * @see MoveVisualToFront() - Use this instead!
     */
    IMPORT_C void MoveVisualToFront(CHuiVisual& aVisual, TInt aLayoutTransitionTime = 0);


    /**
     * Moves a child visual behind the other children (in drawing order).
     * In other words the visual is drawn first (the other child visuals will
     * be drawn on top of this visual).
     *
     * In practise this affects the order
     * of the visuals whithin this layout so that the visual is moved at
     * the beginning of the list (ordinal 0).
     *
     * @param aVisual  Child visual to move.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.     
     * @see MoveVisualToFront()
     * @see Reorder()
     */
    IMPORT_C void MoveVisualToBack(CHuiVisual& aVisual, TInt aLayoutTransitionTime = 0);

    /**
     * Notifies children of the change in the environment's skin.
     */
    IMPORT_C void NotifySkinChangedL();

    /**
     * Sets the transition time for the recalculation of this layout's
     * contents. This is used when a visual is added or removed or when
     * the order of the children is changed.
     *
     * You need to set the EHuiVisualFlagManualTransitionTime flag of this
     * visual before this setting has any effect. This flag overrides the
     * layout transition time hierarchy and uses a local value instead.
     *
     * @param aTransitionTime  Transition time in milliseconds.
     *
     * @see CHuiLayout::SetTransitionTime()     
     * @see CHuiVisual::SetFlag()     
     * @see EHuiVisualFlagManualTransitionTime
     */
    IMPORT_C void SetTransitionTime(TInt aTransitionTime);

    /**
     * Returns the transition time of this layout.
     *
     * @return  Transition time in milliseconds, or
     *          KHuiLayoutDefaultTransitionTime.
     *
     * @see CHuiLayout::SetTransitionTime()
     * @see CHuiVisual::SetFlag()     
     * @see EHuiVisualFlagManualTransitionTime     
     */
    IMPORT_C TInt TransitionTime() const;

    /**
     * Determines the layout ordinal for a child visual. The child's placement
     * depends on the ordinal number.
     *
     * @param aIndex  Index of the child visual.
     */
    IMPORT_C virtual TInt ChildOrdinal(TInt aIndex);

	/**
	 * From CHuiVisual. Prepares all children of this layout for 
	 * drawing. The preparation is done in drawing 
	 * order.
	 * 
	 * Actually this default implementation is not
	 * leaving, since it traps the prepare errors
	 * inside and handles the errors by calling
	 * the visual owners "prepare draw failed" 
	 * -callback.
	 * 
	 * @return ETrue if the prepare succeeded, or EFail if 
	 * the prepare failed. Failing to prepare a draw 
	 * terminates the screen refresh and drawing 
	 * for the current frame.
	 * 
	 * @see MHuiVisualOwner::VisualPrepareDrawFailed()
	 */
	IMPORT_C TBool PrepareDrawL();	
    /**
     * From CHuiVisual. Draws (renders) this layout. 
     * 
     * The default implementataion applies transformations 
     * (affects the positions of the layout), draws brushes
     * and of course children.
     * 
     * Layout clipping is not affected by transformation.
     * 
     * @todo Clipping doesn't work if there is a transformation?
     * @todo No need to draw children that won't be visible.
     */
    IMPORT_C void Draw(CHuiGc& aGc) const;

    /**
     * From CHuiVisual. Called to inform the display that a dirty region has to be defined.
     *
     * Forward the notification request to all the child visuals.
     * Scrolling causes changed reports from the layout itself.
     */
    IMPORT_C void ReportChanged();

    /** From CHuiVisual */
    IMPORT_C void ClearChanged();

    /** From CHuiVisual */
    IMPORT_C TBool Changed() const;

    /** From CHuiVisual */
    IMPORT_C void DumpTree() const;

    /**
     * Defines the base unit used for positioning and sizing children
     * within this layout. For example, the actual X position of a child is 
     * determined by <tt>aBaseUnit.iX * childX</tt>. The effect of the 
     * base unit is limited to the children of this layout -- the 
     * grandchildren use the base units of their parents, not this base unit.
     * 
     * @note It is recommended to use EHuiUnitNormalized if proportional base units are 
     * required. For example, EHuiUnitRelativeToMySize would not make sense as a base 
     * unit.
     *
     * @param aMetric  Metric that will be used for both X and Y axes.
     */
    IMPORT_C void SetBaseUnit(const THuiMetric& aBaseUnit);

    /**
     * Defines the base unit used for positioning and sizing children
     * within this layout. For example, the actual X position of a child is 
     * determined by <tt>aBaseUnit.iX * childX</tt>. The effect of the 
     * base unit is limited to the children of this layout -- the 
     * grandchildren use the base units of their parents, not this base unit.
     * 
     * @param aMetric  Metric to use as the base unit.
     */
    IMPORT_C void SetBaseUnit(const THuiXYMetric& aBaseUnit);
    
    /**
     * Returns the layout's base measurement unit. The positions and sizes
     * of the children of this layout are interpreted as multiples of this
     * base unit.  For example, the actual X position of a child is 
     * determined by <tt>aBaseUnit.iX * childX</tt>. 
     *
     * The effect of the base unit is limited to the children of this layout 
     * -- the grandchildren use the base units of their parents, not this 
     * base unit.
     *
     * @note that if the visual flag @c EHuiVisualFlagAutomaticLocaleMirroringEnabled
     * is set, the value returned by this method will change based on the current platform
     * locale mirroring in the X-axis. So for example if the current layout is mirrored, then
     * the value returned by this method will have the X magnitude set to the negative
     * of the value that was set using @c SetBaseUnit.
     *
     * @return Metric used as the base unit for child coordinates and sizes.
     */
    IMPORT_C virtual THuiXYMetric BaseUnit() const;


    /** @endAPI */


    /**
     * Calculate the bounding rectangle for the layout. The rectangle is
     * calculated in the base units of the layout, i.e. it is calculated
     * using unconverted the child coordinates and sizes.
     *
     * @return  Bounding rectangle: the smallest rectangle inside which
     *          all the children of this layout can fit.
     */
    THuiRealRect BoundingRect() const;

    /**
     * Calculate the size of the inner area (bounds without outer padding).
     * The size is calculated in the base units of this layout, so that
     * children can be positioned within the layout based on this size.
     *
     * @return  Width and height of the inner area, in base units of 
     *          this layout.
     */
    IMPORT_C THuiRealSize InnerSize() const __SOFTFP;
    
    /**
     * Calculates the top left corner of the inner area (bounds without
     * outer padding). The point is calculated in the base units of this
     * layout, so that the children can be positioned within the layout
     * based on this point.
     *
     * @return  Top left corner point of the inner area, in base units
     *          of this layout.
     */
    IMPORT_C THuiRealPoint InnerTopLeft() const __SOFTFP; 

    /** @beginAPI */

    /**
     * New virtual method. Determines the size of a child visual according to the layout.
     * The child size is determined using the base units of the layout.
     * Derived layout classes should refer to InnerSize() and InnerTopLeft()
     * to determine the area suitable for placing children into.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aSize     New target size for the child. This goes to 
     *                  CHuiVisual::SetSize() with no conversions.
     *
     * @return  <code>ETrue</code>, if a new size was determined. Otherwise,
     *          <code>EFalse</code> is returned and no size changes are
     *          needed.
     */
    IMPORT_C virtual TBool ChildSize(TInt aOrdinal, TSize& aSize);

    /**
     * New virtual method. Determines the position of a child visual according to the layout.
     * The child position is determined using the base units of the layout.
     * Derived layout classes should refer to InnerSize() and InnerTopLeft()
     * to determine the area suitable for placing children into.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aPos      New position for the child. This goes to 
     *                  CHuiVisual::SetPos() with no conversions.
     *
     * @return  <code>ETrue</code>, if a new position was determined.
     *          Otherwise, <code>EFalse</code> is returned and no position
     *          changes are needed.
     */
    IMPORT_C virtual TBool ChildPos(TInt aOrdinal, TPoint& aPos);


    enum EHuiLayoutChildRect
        {
        THuiLayoutChildRectNotImplemented = -1,
        THuiLayoutChildRectUpdateNotNeeded = 0,
        THuiLayoutChildRectPosUpdateNeeded = 1,
        THuiLayoutChildRectSizeUpdateNeeded = 2,
        THuiLayoutChildRectLayoutUpdateNeeded = THuiLayoutChildRectPosUpdateNeeded | THuiLayoutChildRectSizeUpdateNeeded
        };
        
    /**
     * New virtual method.  Determines the position and size of a child visual according to the layout.
     * The child position is determined using the base units of the layout.
     * Derived layout classes should refer to InnerSize() and InnerTopLeft()
     * to determine the area suitable for placing children into.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aRect      New position for the child. This is used to set
     *                          the child size and position with no conversions.
     *
     * @return one of the values belonging to EHuiLayoutChildRect as follows:
     *      THuiLayoutChildRectNotImplemented -  method not implemented, ChildPos and ChildSize to be called as previously
     *      THuiLayoutChildRectUpdateNotNeeded - method is implemented, but aRect was not calculated; child pos/size are not modified
     *      THuiLayoutChildRectPosUpdateNeeded - method is implemented, but only iTl was calculated (pos)
     *      THuiLayoutChildRectSizeUpdateNeeded - method is implemented, but only size was calculated (iTl will be returned as 0)
     *      THuiLayoutChildRectLayoutUpdateNeeded - method is implemented and aRect was calculated
     *
     */
    IMPORT_C virtual TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);


    /** @endAPI */


public: // From CHuiVisual. 


    IMPORT_C TReal32 BrushOpacity() const __SOFTFP;

    IMPORT_C THuiRealRect BrushRect() const __SOFTFP;

    IMPORT_C CHuiSkin& BrushSkin() const;

protected:
     /**
     * From CHuiVisual. Overridden by subclasses to draw the content of the visual.
     * By default DrawSelf does not draw anything.
     */
    IMPORT_C void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;

    /**
     * Calculate the metric reference for this layout, relative to this layout's 
     * base units.
     *
     * @param aLayout this parameter is no longer used, an alternative method is provided that does not take this parameter
     * @param aMetric the metric for which we need a reference value
     */
    IMPORT_C THuiRealPoint MetricReferenceForLayoutInPixels(const CHuiLayout* aLayout, const THuiXYMetric& aMetric) const __SOFTFP;
    IMPORT_C THuiRealPoint MetricReferenceForLayoutInPixels(const THuiXYMetric& aMetric) const __SOFTFP;
    
    IMPORT_C void ExpandRectWithContent(TRect& aRect) const;

    /** ! From HuiVisual */
    IMPORT_C void VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

    TBool EffectIsAppliedToChildren() const;

public: // RnD utilities from CHuiVisual
    IMPORT_C void GetInstanceName(TDes& aName) const;
    IMPORT_C void GetClassName(TDes& aName) const;
    IMPORT_C TType Type() const;
    IMPORT_C TInt SessionId() const;
    IMPORT_C void SetSessionId(TInt aSessionId);

private:
     
    TBool HasActiveChilderen() const;
    void DrawStoredBitmap(CHuiGc &aGc) const;
protected:
    struct THuiLayoutPrivateData;
    THuiLayoutPrivateData* iHuiLayoutPrivateData; // owned
    };

#endif  // __HUILAYOUT_H__
