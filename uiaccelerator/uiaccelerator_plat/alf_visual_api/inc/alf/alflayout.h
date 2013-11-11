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
* Description:   Layout base class
*
*/



#ifndef C_ALFLAYOUT_H
#define C_ALFLAYOUT_H

#include <alf/alfvisual.h> 
#include <alf/alftimedvalue.h>
#include <alf/alfmetric.h>

/** Layout types. */
enum TAlfLayoutType
    {
    EAlfLayoutTypeLayout,
    EAlfLayoutTypeFlow,
    EAlfLayoutTypeGrid,
    EAlfLayoutTypeLCTGrid,
    EAlfLayoutTypeApp,
    EAlfLayoutTypeAnchor,
    EAlfLayoutTypeLCTAnchor,
    EAlfLayoutTypeDeck,
    EAlfLayoutTypeViewport,
    EAlfLayoutTypeCurvePath
    };

/** The default transition time is queried from CAlfStatic. */
const TInt KAlfLayoutDefaultTransitionTime = -1;

class CAlfControl;

/**
 *  Base class for all layouts.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 *
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
 * layout->SetFlags(EAlfVisualFlagLayoutUpdateNotification);
 * </code>
 *
 * .. and override VisualLayoutUpdated() method in your control.
 *
 * @see SetSize()
 * @see SetFlags()
 * @see CAlfControl::VisualLayoutUpdated()
 *
 * Each layout defines its own local coordinate system used for interpreting
 * the positions and sizes of its child visuals. The base unit of the 
 * coordinate system is defined with the SetBaseUnit() method, which sets a
 * TAlfXYMetric to use as the basis for all coordinate calculations. The default
 * metric is [1px, 1px] which corresponds to pixels on the display.
 *
 * The local coordinate system of a layout can be mirrored horizontally and/or
 * vertically by using a negative magnitude in the relevant TAlfMetric of the
 * base unit. Note that this will only affect the local coordinate system 
 * of the layout: the contents of the child visuals or child layouts will not 
 * be mirrored. Visual mirroring should be done with a CAlfTransformation,
 * and child layouts can be mirrored by further application of the negative
 * base unit metrics. In order to enable a layout to automatically mirror the 
 * horizontal component of the base unit based on locale mirroring, set the visual flag 
 * @c EAlfVisualFlagAutomaticLocaleMirroringEnabled, note that it is necessary
 * to set this flag for each layout for which this feature is required.
 *
 * To create to a fully scalable UI, the appropriate base unit metrics and
 * layout visuals must be used. For example, the EAlfUnitS60 metric unit is
 * dependent on the platform-specified measurement units for layout. 
 * CAlfGridLayout and CAlfAnchorLayout provide support for pixel-independent 
 * placement of their children.
 *
 * @see CAlfAnchorLayout
 * @see CAlfGridLayout
 *
 * Applications that need more complex layouts should derive their own
 * CAlfLayout based classes that implement the necessary layout calculations.
 * In many cases, however, a combination of some of the built-in layout classes
 * such as anchor and grid layouts will be sufficient.
 */
class CAlfLayout : public CAlfVisual
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


    /**
     * 2-phased constructor which also gives ownedship to control.
     *
     * @param aOwnerControl Control which owns new layout
     * @param aParentLayout If given, has the parent layout.
     * @return New object. Ownership NOT returned. Instead given to control.
     */
    IMPORT_C static CAlfLayout* AddNewL( CAlfControl& aOwner,
                                         CAlfLayout* aParentLayout = 0 );

    /**
     * Constructor.
     */
    IMPORT_C CAlfLayout();

    /**
     * From CAlfVisual. Second-phase constructor.
     */
    IMPORT_C void ConstructL( CAlfControl& aOwner );

    /**
     * Virtual destructor.
     */
    IMPORT_C ~CAlfLayout();
    
    /**
     * From CAlfVisual. Removes this layout from its owner and and destroys the layout.
     * Also removes the entire hierarchy of child visuals and destroys all
     * of them.
     */
    IMPORT_C void RemoveAndDestroyAllD();

    /**
     * Append a new visual to the layout.
     *
     * @param aVisual  Visual to append.
     * @param aConstructedWithParentInformation Whether visual already knows its parent
     * @return Error code.
     *
     * @todo  Add a method for appending multiple visuals in one set, so that
     *        they only cause a single layout update.
     */
    IMPORT_C TInt Append( CAlfVisual* aVisual, TBool aConstructedWithParentInformation = EFalse, TInt aLayoutTransitionTime = 0 );
 
    /**
     * Remove a visual from the layout.
     *
     * @param aVisual  Visual to remove.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.          
     *
     * @todo  Add a method for removing multiple visuals in one set, so that
     *        they only cause a single layout update.
     */
    IMPORT_C void Remove( CAlfVisual* aVisual, TInt aLayoutTransitionTime = 0 );
    
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
     * @see MoveVisualToBack()
     * @see MoveVisualToFront()
     */
    IMPORT_C void Reorder(CAlfVisual& aVisual, TInt aPosition, TInt aLayoutTransitionTime = 0 );

    /**
     * Moves a child visual to the front of the other children. In other words
     * the visual is drawn last (on top of the other child visuals). In practise
     * this affects the order of the visuals whithin this layout so that the
     * visual is moved at the end of the list.
     *
     * Causes layout recalculation.
     *
     * @param aVisual  Child visual to move.
     * @param aLayoutTransitionTime How long the transition should take visually, in milliseconds.          
     * @see MoveVisualToBack()
     * @see Reorder()
     *
     * @see MoveVisualToFront() - Use this instead!
     */
    IMPORT_C void MoveVisualToFront(CAlfVisual& aVisual, TInt aLayoutTransitionTime = 0 );


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
    IMPORT_C void MoveVisualToBack(CAlfVisual& aVisual, TInt aLayoutTransitionTime = 0 );


    /**
     * Returns the number of child visuals in the layout.
     *
     * @return Child visual count.
     */
    IMPORT_C TInt Count() const;
    
    /**
     * Returns a child visual.
     *
     * @param aIndex  Index of the child visual.
     *
     * @return  Reference to a child visual.
     */
    IMPORT_C CAlfVisual& Visual( TInt aIndex ) const;
    
    /**
     * Enables or disables scrolling in the layout.
     *
     * @param aScrolling  ETrue to enable scrolling,
     *                    EFalse to disable.
     */
    IMPORT_C void EnableScrollingL(TBool aScrolling = ETrue);
    
    /**
     * Is scrolling in the layout.
     *
     * @return ETrue if scrolling is enabled.
     */
    IMPORT_C TBool Scrolling() const;

    /**
     * Returns the scroll offset of the layout. The scroll offset is in
     * layout's base units.
     *
     * @return  Reference to timed point that defines the scroll offset.
     */
    IMPORT_C const TAlfTimedPoint& ScrollOffset() const;
    
    /**
     * Set the scroll offset
     *
     * @param aPoint Sets the point.
     */
    IMPORT_C void SetScrollOffset(const TAlfTimedPoint& aPoint);
     

    /**
     * Determines the layout ordinal for a child visual. The child's placement
     * depends on the ordinal number.
     *
     * @param aIndex  Index of the child visual.
     */
    IMPORT_C TInt ChildOrdinal(TInt aIndex);

    /**
     * Determines the position of a child visual according to the layout.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aPos      New position for the child.
     *
     * @return  <code>ETrue</code>, if a new position was determined.
     *          Otherwise, <code>EFalse</code> is returned and no position
     *          changes are needed.
     */
    IMPORT_C TBool ChildPos(TInt aOrdinal, TPoint& aPos);

    /**
     * Determines the size of a child visual according to the layout.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aSize     New target size for the child.
     *
     * @return  <code>ETrue</code>, if a new size was determined. Otherwise,
     *          <code>EFalse</code> is returned and no size changes are
     *          needed.
     */
    IMPORT_C TBool ChildSize(TInt aOrdinal, TSize& aSize);
    
     /**
     * @from CAlfVisual
     * Update the layout of all children. Subclasses can call this to
     * recalculate the layout of all children.
     *
     * @param aTransitionTime  Time for layout transition in milliseconds.
     *                         Any negative value uses value given in 
     *                         CAlfEnv::StaticSetLayoutTransitionTime()
     */
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0); 
    
    /**
     * @from CAlfVisual
     * Finds a visual whose tag descriptor contains a specific tag.
     *
     * CAlfLayout::FindTag() extends the search into child visuals,
     * covering recursively entire visual hierarchies.
     * 
     * @param aTag  Tag to look for. If this tag is found as a part of a
     *              visual's colon-separated tag descriptor, it is treated
     *              as a match and the visual is returned. The tag to look
     *              for cannot contain separator characters (:).
     *
     * @return  Visual whose tag descriptor matches the tag. <code>NULL</code>,
     *          if no such visual could be found. 
     *
     * Todo: how should several visuals with matching tag to be handled ??
     *
     * @see CAlfVisual::Tag()
     * @see CAlfVisual::SetTagL()
     * @see CAlfVisual::IsTagged()
     * @see CAlfVisual::FindTag()
     */
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);

    /**
     * Sets the transition time for the recalculation of this layout's
     * contents. This is used when a visual is added or removed or when
     * the order of the children is changed.
     *
     * Set to KAlfLayoutDefaultTransitionTime to make the layout use the
     * default transition time queried from CAlfStatic.
     *
     * @param aTransitionTime  Transition time in milliseconds, or
     *                         KAlfLayoutDefaultTransitionTime.
     */
    IMPORT_C void SetTransitionTime(TInt aTransitionTime);

    /**
     * Returns the transition time of this layout.
     *
     * @return  Transition time in milliseconds, or
     *          KAlfLayoutDefaultTransitionTime.
     *
     * @see CAlfLayout::SetTransitionTime()
     */
    IMPORT_C TInt TransitionTime() const;
    
    /**
     * Determines the virtual size of the layout. The virtual size may be
     * larger than the actual size. The virtual size is updated automatically
     * when the layout's children are positioned.
     *
     * @return  Virtual size.
     */
    IMPORT_C TSize VirtualSize() const;

    /**
     * Sets the inner padding of the layout. This is typically the amount
     * of empty space between child visuals in the layout.
     *
     * @note that the left and right paddings will be the same, and the top and bottom
     *      paddings will be the same.
     * @deprecated migrate to using metrics instead
     * @param aInnerPadding  Inner padding (e.g., column, row separation).
     */
    IMPORT_C void SetInnerPadding(const TPoint& aInnerPadding);

    /**
     * Sets the inner padding of the layout. This is typically the amount
     * of empty space between child visuals in the layout.
     *
     * @note as paddings are relative to the layout visual, if relative units are required, 
     * it is advisable to use EAlfUnitRelativeToMySize so that the paddings are relative
     * to the size of this layout visual
     *
     * @param aInnerPadding  Inner padding, using metrics.
     */
    IMPORT_C void SetInnerPadding(const TAlfXYMetric& aInnerPadding);

    /**
     * Returns the inner padding.
     * @deprecated this will return correct pixel values only if paddings were set 
     *      using deprecated pixel based APIs, so migrate to using metrics instead
     */
    IMPORT_C TPoint InnerPadding() const;

    /**
     * Returns the inner padding metric
     * @see InnerPadding() which is superceded by this method
     */
    IMPORT_C const TAlfXYMetric& InnerPaddingAsMetric() const;

    /**
     * Returns the inner horizontal padding.
     *
     * @deprecated this will return correct pixel values only if paddings were set 
     *      using deprecated pixel based APIs, so migrate to using metrics instead
     */
    IMPORT_C TInt HorizontalInnerPadding() const;

    /**
     * Returns the inner vertical padding.
     *
     * @deprecated this will return correct pixel values only if paddings were set 
     *      using deprecated pixel based APIs, so migrate to using metrics instead
     */
    IMPORT_C TInt VerticalInnerPadding() const;
    
    /**
     * Returns the inner padding converted to the base units of this layout.
     * These can be summed with the positions and sizes of the layout's children.
     */
    IMPORT_C TAlfRealPoint InnerPaddingInBaseUnits() const;
    
    /**
     * Finds a visual's index.
     *
     * @param aVisual  Visual to look for.
     *
     * @return  Index of the visual, or <code>KErrNotFound</code>.
     */
    IMPORT_C TInt FindVisual(CAlfVisual* aVisual) const;

    /**
     * Insert a new visual to the layout, at specified position (position 0 is the first,
     * Count() -1 is the last). If the visual is already a member of
     * some other layout, it will be removed from the old layout first.
     *
     * @param aVisual  Visual to append.
     * @param TInt     Position to insert. This is the new ordinal number of the visual.
     * @return Error code
     */
    IMPORT_C TInt Insert(CAlfVisual* aVisual, TInt aPosition);
    
    /**
     * Layout is calculated according the ordinals. The effective ordinal
     * may not be the same as the real child ordinal, because some visuals
     * are laid out manually.
     *
     * @return  Effective ordinal for the child visual.
     */
    IMPORT_C TInt EffectiveLayoutOrdinal(const CAlfVisual& aVisual) const;

    /**
     * Defines the base unit used for positioning and sizing children
     * within this layout. For example, the actual X position of a child is 
     * determined by <tt>aBaseUnit.iX * childX</tt>. The effect of the 
     * base unit is limited to the children of this layout -- the 
     * grandchildren use the base units of their parents, not this base unit.
     * 
     * @note It is recommended to use EAlfUnitNormalized if proportional base units are 
     * required. For example, EAlfUnitRelativeToMySize would not make sense as a base 
     * unit.
     *
     * @param aMetric  Metric that will be used for both X and Y axes.
     */
    IMPORT_C void SetBaseUnit(const TAlfMetric& aBaseUnit);

    /**
     * Defines the base unit used for positioning and sizing children
     * within this layout. For example, the actual X position of a child is 
     * determined by <tt>aBaseUnit.iX * childX</tt>. The effect of the 
     * base unit is limited to the children of this layout -- the 
     * grandchildren use the base units of their parents, not this base unit.
     * 
     * @param aMetric  Metric to use as the base unit.
     */
    IMPORT_C void SetBaseUnit(const TAlfXYMetric& aBaseUnit);
       
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
     * @note that if the visual flag @c EAlfVisualFlagAutomaticLocaleMirroringEnabled
     * is set, the value returned by this method will change based on the current platform
     * locale mirroring in the X-axis. So for example if the current layout is mirrored, then
     * the value returned by this method will have the X magnitude set to the negative
     * of the value that was set using @c SetBaseUnit.
     *
     * @return Metric used as the base unit for child coordinates and sizes.
     */
    IMPORT_C virtual TAlfXYMetric BaseUnit() const;

protected:

    /**
     * @from CAlfVisual
     * Deletes the child visuals
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    
    // Private structure. Owned.
    struct TLayoutPrivateData;
    TLayoutPrivateData* iLayoutData;

    };


#endif // C_ALFLAYOUT_H
