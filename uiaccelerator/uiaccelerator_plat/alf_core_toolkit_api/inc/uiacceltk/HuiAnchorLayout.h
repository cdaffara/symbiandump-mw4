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
* Description:   Definition for CHuiAnchorLayout. Anchor layouts allow specifying the 
*                placement of child visuals using anchors, that can be relative or absolute
*                coordinates and attached to a specific edge of the layout.
*
*/



#ifndef __HUIANCHORLAYOUT_H__
#define __HUIANCHORLAYOUT_H__


#include <e32base.h>
#include <uiacceltk/HuiLayout.h>
#include <uiacceltk/HuiTimedPoint.h>


/* Forward declarations */
class CHuiEnv;
class CHuiLayout;
class CHuiControl;


/** for internal use only */
enum THuiAnchorBitmask
    { 
    EHuiAnchorBitmaskHorizontal = 0x000F,
    EHuiAnchorBitmaskVertical = 0x00F0
    };

/** for internal use only */
enum THuiAnchorBitmaskShift
   {
    EHuiAnchorBitmaskShiftHorizontal = 0,
    EHuiAnchorBitmaskShiftVertical = 4
    };

/** for internal use only */
enum THuiAnchorAxis
    {
    EHuiAnchorAxisHorizontal = 0,
    EHuiAnchorAxisVertical = 1
    };

/** for internal use only */
enum THuiAnchorProximity
    {
    EHuiAnchorProximityNone = 0,    
    EHuiAnchorProximitySize,
    EHuiAnchorProximityNear,
    EHuiAnchorProximityCenter,
    EHuiAnchorProximityFar
    };

/** 
 * Anchor type. Used to determine which edge or corner of a child visual is 
 * defined by an anchor.
 *
 * @note that if width or height are used as one anchor, they must be combined 
 * with an appropriate anchor in order to unambiguously determine the size and 
 * position of the child.
 */
enum THuiAnchorType
    {
    EHuiAnchorTypeNone = EHuiAnchorProximityNone,
    
    EHuiAnchorTypeWidth = EHuiAnchorProximitySize << EHuiAnchorBitmaskShiftHorizontal,
    EHuiAnchorTypeLeft = EHuiAnchorProximityNear << EHuiAnchorBitmaskShiftHorizontal,
    EHuiAnchorTypeHCenter = EHuiAnchorProximityCenter << EHuiAnchorBitmaskShiftHorizontal,
    EHuiAnchorTypeRight = EHuiAnchorProximityFar << EHuiAnchorBitmaskShiftHorizontal,
    
    EHuiAnchorTypeHeight = EHuiAnchorProximitySize << EHuiAnchorBitmaskShiftVertical,
    EHuiAnchorTypeTop = EHuiAnchorProximityNear << EHuiAnchorBitmaskShiftVertical,
    EHuiAnchorTypeVCenter = EHuiAnchorProximityCenter << EHuiAnchorBitmaskShiftVertical,
    EHuiAnchorTypeBottom = EHuiAnchorProximityFar << EHuiAnchorBitmaskShiftVertical,
    
    
    EHuiAnchorTypeSize = EHuiAnchorTypeHeight | EHuiAnchorTypeWidth,

    EHuiAnchorTypeTopLeft = EHuiAnchorTypeTop | EHuiAnchorTypeLeft,
    EHuiAnchorTypeTopCenter = EHuiAnchorTypeTop | EHuiAnchorTypeHCenter,
    EHuiAnchorTypeTopRight = EHuiAnchorTypeTop | EHuiAnchorTypeRight,

    EHuiAnchorTypeCenterLeft = EHuiAnchorTypeVCenter | EHuiAnchorTypeLeft,
    EHuiAnchorTypeCenter = EHuiAnchorTypeVCenter | EHuiAnchorTypeHCenter,
    EHuiAnchorTypeCenterRight = EHuiAnchorTypeVCenter | EHuiAnchorTypeRight,
    
    EHuiAnchorTypeBottomLeft = EHuiAnchorTypeBottom | EHuiAnchorTypeLeft,
    EHuiAnchorTypeBottomCenter = EHuiAnchorTypeBottom | EHuiAnchorTypeHCenter,
    EHuiAnchorTypeBottomRight = EHuiAnchorTypeBottom | EHuiAnchorTypeRight
    };

/** 
 * Anchor origin, relative to a particular edge or corner. Direction is 
 * measured along positive x and y axes, as modified by the metric used.
 */
enum THuiAnchorAttachmentOrigin
    {
    EHuiAnchorAttachmentOriginNone = EHuiAnchorTypeNone,
    
    EHuiAnchorAttachmentOriginLeft = EHuiAnchorTypeLeft,
    EHuiAnchorAttachmentOriginHCenter = EHuiAnchorTypeHCenter,
    EHuiAnchorAttachmentOriginRight = EHuiAnchorTypeRight,
    
    EHuiAnchorAttachmentOriginTop = EHuiAnchorTypeTop,
    EHuiAnchorAttachmentOriginVCenter = EHuiAnchorTypeVCenter,
    EHuiAnchorAttachmentOriginBottom = EHuiAnchorTypeBottom,
    
    EHuiAnchorAttachmentOriginTopLeft = EHuiAnchorTypeTopLeft, 
    EHuiAnchorAttachmentOriginTopCenter = EHuiAnchorTypeTopCenter, 
    EHuiAnchorAttachmentOriginTopRight = EHuiAnchorTypeTopRight, 

    EHuiAnchorAttachmentOriginCenterLeft = EHuiAnchorTypeCenterLeft,
    EHuiAnchorAttachmentOriginCenter = EHuiAnchorTypeCenter, 
    EHuiAnchorAttachmentOriginCenterRight = EHuiAnchorTypeCenterRight, 
    
    EHuiAnchorAttachmentOriginBottomLeft = EHuiAnchorTypeBottomLeft, 
    EHuiAnchorAttachmentOriginBottomCenter = EHuiAnchorTypeBottomCenter, 
    EHuiAnchorAttachmentOriginBottomRight = EHuiAnchorTypeBottomRight
    };
    
/** 
 * Type of attachment point used when attaching an anchor to another
 * anchor in the same layout.
 */
enum THuiAnchorAttachmentType
    {
    EHuiAnchorAttachToParent = -1
    };
    
    
/** 
 * Determines which corner of a child visual is positioned by the anchor
 * @deprecated, only retained for use with deprecated SetAnchor routines, use
 *                        THuiAnchorAttachmentOrigin instead with new APIs
 */
enum THuiAnchor
    {
    /** Anchor that defines the top left corner of a visual. */
    EHuiAnchorTopLeft,

    /** Anchor that defines the bottom right corner of a visual. */
    EHuiAnchorBottomRight
    };


/** 
 * Anchor origins.
 * @deprecated, only retained for use with deprecated SetAnchor routines, use
 *                        THuiAnchorAttachmentOrigin instead with new APIs
 */
enum THuiAnchorOrigin
    {
    EHuiAnchorOriginLeft,
    EHuiAnchorOriginHCenter,
    EHuiAnchorOriginRight,
    EHuiAnchorOriginTop,
    EHuiAnchorOriginVCenter,
    EHuiAnchorOriginBottom
    };


/** 
 *Anchor metric types, for offsets and sizes.
 * @deprecated, only retained for use with deprecated SetAnchor routines, use
 *                        THuiMetric instead with new APIs
 */
enum THuiAnchorMetric
    {
    /** Use absolute pixel coordinates to specify the values. */        
    EHuiAnchorMetricAbsolute,
    /** 
     * Use the size of the visual 
     * either normalized between 0.0 (at origin) and 1.0 (farthest away edge)
     * or -0.5 (min) and +0.5 (0.0 being always at the origin). For example if you 
     * use EHuiAnchorOriginLeft with EHuiAnchorMetricRelativeToSize, the 
     * left edge is at 0.0, and the right edge at 1.0. But if you use any of 
     * the center origins (EHuiAnchorOriginHCenter or EHuiAnchorOriginVCenter),
     * then the left/top edge is at -0.5 and right/top edge is at +0.5.
     */
    EHuiAnchorMetricRelativeToSize
    };


/**
 * CHuiAnchorLayout is a layout that allows visuals to be positioned with 
 * flexible relative anchor points. 
 *
 * To use this class, an anchor layout is created, and then a number of anchor
 * points are added to it using calls to the SetAnchor() method. The visuals
 * inside the layout will be stretched between these anchor points.
 *
 * By default, each child is set to cover the entire anchor layout area.
 * @todo This seems to be incorrect. Default behaviour seems to be that
 * a child is invisible until its anchor points have been set.
 * 
 * @see CHuiAnchorLayout::SetAnchor() for more information about anchors.
 */
NONSHARABLE_CLASS(CHuiAnchorLayout) : public CHuiLayout
    {
public:

    /* Constructors and destructor. */

    /**
     * Construct a new anchor layout and give its ownership to a control.
     *
     * @param aOwnerControl  Owner control.
     * @param aParentLayout  Parent layout for the new anchor layout.
     */
    IMPORT_C static CHuiAnchorLayout* AddNewL(CHuiControl& aOwnerControl,
                                              CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    CHuiAnchorLayout(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHuiAnchorLayout();


    /* Methods. */

    /**
     * Resets all the anchors.
     */
    IMPORT_C void Reset();

    /**
     * Anchors a child visual's corner to specific position within this layout.
     *
     * @deprecated, use new @c Attach APIs with THuiMetric support.
     * See the relevant API documentation for an example of how to migrate your code
     *
     * Each child can have two anchors: the top left and bottom right corners 
     * of the child visual. These anchors can be fastened to any point within 
     * this layout, as specified by the other parameters.
     *
     * In a typical case you set two anchors (both EHuiAnchorTopLeft and 
     * EHuiAnchorBottomRight), which specifies both size and position for the 
     * child. However it is also possible to set only the EHuiAnchorTopLeft 
     * anchor, which only positions the child without affecting it's size.
     * 
     * @see RemoveAnchor() to remove an achor definition.
     * 
     * @note  After updating anchors, you should call 
     *        CHuiLayout::UpdateChildrenLayout(), to cause the anchors to be 
     *        applied to child positions and/or sizes.
     * 
     * For example the following code will anchor child visual number 0 in 
     * the middle of the layout, with a 25% border around the outside:
     *
     * \code
     *      // set top left corner position of 1st child ..
     *	    iLayout->SetAnchor(EHuiAnchorTopLeft, 0,
     *	        EHuiAnchorOriginLeft, 
     *          EHuiAnchorOriginTop,
     *          EHuiAnchorMetricRelativeToSize, // With EHuiAnchorOriginLeft means that 0.0 goes to left edge, and 1.0 to the right
     *          EHuiAnchorMetricRelativeToSize, // With EHuiAnchorOriginTop means that 0.0 goes to top edge, and 1.0 to the bottom
     *          THuiTimedPoint(0.25, 0.25));
     *      // .. and set the bottom right corner also to fix the size
     * 	    iLayout->SetAnchor(EHuiAnchorBottomRight, 0,
     *	        EHuiAnchorOriginRight, 
     *          EHuiAnchorOriginBottom,
     *   	EHuiAnchorMetricRelativeToSize, 
     *          EHuiAnchorMetricRelativeToSize,
     *          THuiTimedPoint(-0.25, -0.25)); 
     * \endcode
     *
     * @todo  Specifying a transition in the timed point may be useless:
     *        layouts are evaluated pretty seldom.
     *
     * @param aAnchor       Type of the anchor (either EHuiAnchorTopLeft for the 
     *                      top left corner or EHuiAnchorBottomRight for bottom right
     *                      corner).
     * @param aOrdinal      Ordinal of the child visual that this anchor applies to.
     * @param aHorizOrigin  Horizontal origin for the anchor position (aPosition). 
     *                      Defines the point inside the layout on which the 
     *                      evaluation of the child's placement is based. May be 
     *                      EHuiAnchorOriginLeft (left edge), 
     *                      EHuiAnchorOriginHCenter (center) or
     *                      EHuiAnchorOriginRight (right edge).
     * @param aVertOrigin   Vertical origin for the anchor position (aPosition). 
     *                      Defines the point inside the layout on which the 
     *                      evaluation of the child's placement is based. Possible options 
     *                      are EHuiAnchorOriginTop (top), EHuiAnchorOriginVCenter 
     *                      (center) and EHuiAnchorOriginBottom (bottom).
     * @param aHorizMetric  Specifies the metric to use to define the
     *                      horizontal position: EHuiAnchorMetricAbsolute to use 
     *                      absolute pixels or EHuiAnchorMetricRelativeToSize to 
     *                      use normalized values relative to layout size.
     * @param aVertMetric   Specifies the metric used in the vertical position:
     *                      EHuiAnchorMetricAbsolute to use absolute pixels or 
     *                      EHuiAnchorMetricRelativeToSize to use normalized 
     *                      values relative to layout size.     
     * @param aPosition     The achor position as an offset from the origin point, 
     *                      applied using the metric specified with aHorizMetric 
     *                      and aVertMetric.
     * @return                   Error code
     *
     */
    IMPORT_C TInt SetAnchor(THuiAnchor aAnchor, TInt aOrdinal,
                            THuiAnchorOrigin aHorizOrigin,
                            THuiAnchorOrigin aVertOrigin,
                            THuiAnchorMetric aHorizMetric,
                            THuiAnchorMetric aVertMetric,
                            const THuiTimedPoint& aPosition);
    /**
     * Convenience method for setting top left and bottom right anchors in a single call.
     *
     * This method sets size-relative anchors for a single child visual.
     *
     * @param aOrdinal           Ordinal of child visual to set anchors for.
     * @param aTlHorizOrigin     Horizontal origin for the top left anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. May be 
     *                           EHuiAnchorOriginLeft (left edge), 
     *                           EHuiAnchorOriginHCenter (center) or
     *                           EHuiAnchorOriginRight (right edge).
     * @param aTlVertOrigin      Vertical origin for the top left anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. Possible options 
     *                           are EHuiAnchorOriginTop (top), EHuiAnchorOriginVCenter 
     *                           (center) and EHuiAnchorOriginBottom (bottom).
     * @param aTopLeftOffset     The achor position as an offset from the origin point, 
     * @param aBrHorizOrigin     Horizontal origin for the bottom right anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. May be 
     *                           EHuiAnchorOriginLeft (left edge), 
     *                           EHuiAnchorOriginHCenter (center) or
     *                           EHuiAnchorOriginRight (right edge).
     * @param aBrVertOrigin      Vertical origin for the bottom right anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. Possible options 
     *                       	 are EHuiAnchorOriginTop (top), EHuiAnchorOriginVCenter 
     *                           (center) and EHuiAnchorOriginBottom (bottom).     
     * @param aBottomRightOffset The achor position as an offset from the origin point,      
     * @return                   Error code
     *
     * @see CHuiAnchorLayout::SetAnchor() for more information about anchors.
     */
    IMPORT_C TInt SetRelativeAnchorRect(TInt aOrdinal,
                                        THuiAnchorOrigin aTlHorizOrigin,
                                        THuiAnchorOrigin aTlVertOrigin,
                                        const THuiRealPoint& aTopLeftOffset,
                                        THuiAnchorOrigin aBrHorizOrigin,
                                        THuiAnchorOrigin aBrVertOrigin,
                                        const THuiRealPoint& aBottomRightOffset);
                                  
    /**
     * Undefines an anchor.
     * @see SetAnchor()
     * @deprecated instead use @c Detach and see relevant API documentation for
     *      example code
     */
    IMPORT_C void RemoveAnchor(THuiAnchor aAnchor, TInt aOrdinal);

                                  
    /**
     * Anchors one or more edges of a child visual to a specific position 
     * within this layout. When combined together, a correctly specified group 
     * of anchors provide sufficient information for the anchor layout to 
     * determine the size and position of the child visual.
     *
     * Each child can have up to four anchors, each of which correspond to an 
     * edge. A corner anchor is equivalent to supplying two similar orthogonal 
     * edge anchors, so typically it is easier to use corners. Therefore, each 
     * child can have up to two corner anchors, or a mixture of corners and 
     * edges. Alternatively, four similar edges can be specified in one call by 
     * supplying a box metric.
     *
     * In a typical case you set two different corner anchors (for example both 
     * EHuiAnchorTypeTopLeft and EHuiAnchorTypeBottomRight), taking care
     * to ensure that they are sufficiently orthogonal in order to define all needed
     * coordinates. However it is also possible to set only one corner anchor, 
     * which only positions the child without affecting it's size. Alternatively,
     * one anchor can be specified as a corner, and then the other can be 
     * specified as EHuiAnchorTypeSize; similarly the coordinates along
     * one axis can be specified by an appropriate combination of edge and 
     * width or height anchors.
     *
     * The position of each anchor relative to this layout is determined
     * by the choice of origin (i.e. which point of the layout the 
     * child's is attached to), and the offset position, which is specified
     * using a THuiMetric or related type. All available features of 
     * THuiMetric may be used to affect the resulting position, providing the 
     * benefit that different metrics can be used for each child. Also note 
     * that the metric used will override the base unit that has been set in 
     * this layout.
     * 
     * @note if called again to specify the same anchor types
     *      that have been previous set, those definitions will be updated. 
     *      However, in order to change type, it is necessary to detach a 
     *      previously defined anchor first, as there can only be a maximum 
     *      of two anchors along each axis (for a given child).
     * @note that size anchor type and attachment origins are not 
     *      currenly supported. 
     * @note that center anchor type is not currently supported.
     * @note if an anchor of similar type has already been defined it will be 
     *      replaced, however in general it is easier to manage anchors by 
     *      removing them all and starting again.
     * @note  After updating anchors, you should call 
     *      @c CHuiLayout::UpdateChildrenLayout(), to cause the anchors to be 
     *      applied to child positions and/or sizes.
     * 
     * @see @c Detach() to remove achor definitions for a given ordinal
     *
     * For example the following code will anchor three child visuals 0, 1, 2  in 
     * the middle of the layout, with a 25% border around the outside, such that 
     * they all occupy the same position:
     *
     * \code
     *      // using edges...
     *	    iLayout->Attach(0, 
     *          EHuiAnchorTypeLeft,
     *          THuiXYMetric(0.25, EHuiMetricNormalized),
     *          EHuiAnchorAttachmentOriginLeft);
     *	    iLayout->Attach(0, 
     *          EHuiAnchorTypeRight,
     *          THuiXYMetric(-0.25, EHuiMetricNormalized),
     *          EHuiAnchorAttachmentOriginRight);
     *	    iLayout->Attach(0, 
     *          EHuiAnchorTypeTop,
     *          THuiXYMetric(0.25, EHuiMetricNormalized),
     *          EHuiAnchorAttachmentOriginTop);
     *	    iLayout->Attach(0, 
     *          EHuiAnchorTypeBottom,
     *          THuiXYMetric(-0.25, EHuiMetricNormalized),
     *          EHuiAnchorAttachmentOriginBottom);
     *
     *      // using corners...
     *	    iLayout->Attach(1, 
     *          EHuiAnchorTypeTopLeft,
     *          THuiXYMetric(0.25, EHuiMetricNormalized),
     *          EHuiAnchorAttachmentOriginTopLeft);
     * 	    iLayout->Attach(1, 
     *	        EHuiAnchorTypeBottomRight, 
     *         THuiXYMetric(-0.25, EHuiMetricNormalized),
     *	        EHuiAnchorAttachmentOriginBottomRight);
     * 
     *      // .. and using a box
     * 	    iLayout->Attach(2, THuiBoxMetric(
     *          THuiMetric(0.25, EHuiMetricNormalized),
     *          THuiMetric(-0.25, EHuiMetricNormalized),
     *          THuiMetric(0.25, EHuiMetricNormalized),
     *          THuiMetric(-0.25, EHuiMetricNormalized)));
     * \endcode
     *
     * @param aOrdinal      Ordinal of the child visual that this anchor applies to
     * @param aType       Type of the anchor, which determines how and where the anchor attaches to this layout
     * @param aOffset     The anchor position as an offset from the origin point,
     *      specifying both the magnitude and units of the offset for the relevant axes.
     * @param aAttachmentOrigin  Origin for the anchor offset.
     *      Defines the point inside the layout from which the 
     *      evaluation of the child's placement is based. 
     * @param aAttachmentOrdinal    the ordinal of the anchor to which this anchor is attached, 
     *      use the default value of EHuiAnchorAttachToParent in order to attach to parent
     * @return Error code
     *
     */
    IMPORT_C TInt Attach(
        TInt aOrdinal,
        THuiAnchorType aType, 
        const THuiMetric& aOffset,
        THuiAnchorAttachmentOrigin aAttachmentOrigin,
        TInt aAttachmentOrdinal = EHuiAnchorAttachToParent);

    /** 
     * Attach a corner of a child visual  to a specific position 
     * within this layout. 
     *
     * @see Attach() for all API documentation
     */
    IMPORT_C TInt Attach(
        TInt aOrdinal,
        THuiAnchorType aType, 
        const THuiXYMetric& aOffset,
        THuiAnchorAttachmentOrigin aAttachmentOrigin = EHuiAnchorAttachmentOriginTopLeft,
        TInt aAttachmentOrdinal = EHuiAnchorAttachToParent);

    /** 
     * Attach all four edges of a child visual  to a specific position 
     * within this layout.
     *
     * @note, will detach all existing edges first. In case of error, all
     * edges that were successfull added before the error occurred will be 
     * detached.
     *
     * @see Attach() for all API documentation
     * @param aAttachmentOrigin if set to EHuiAnchorAttachmentOriginNone, each edge will 
     *        be attached to the corresponding edge
     */
    IMPORT_C TInt Attach(
        TInt aOrdinal,
        const THuiBoxMetric& aOffset,
        THuiAnchorAttachmentOrigin aAttachmentOrigin = EHuiAnchorAttachmentOriginNone,
        TInt aAttachmentOrdinal = EHuiAnchorAttachToParent);


    /**
     * Undefines all anchors for a specified ordinal. Has no effect if the specified ordinal
     * cannot be found.
     *
     * @see Attach()
     * 
     * @param aOrdinal the ordinal of the anchor to remove
     */
    IMPORT_C void Detach(TInt aOrdinal);
    
    /**
     * Undefines an anchor. Has no effect if the specified anchor type 
     * cannot be found. For example to detach a corner anchor with ordinal
     * equal to zero:
     *
     * \code
     *      iLayout->Detach(0, EAlfAnchorTypeTopLeft);
     * \endcode
     *
     * @see Attach()
     * @note can be used to separately remove the edges 
     * that were attached using different combinations of types, 
     * but try to be consistent in using either edges or corners to avoid 
     * confusion.
     * 
     * @param aOrdinal the ordinal of the anchor to remove
     * @param aType the edge relating to the anchor to remove
     */    
    IMPORT_C void Detach(TInt aOrdinal, THuiAnchorType aType);
    
    /**
     * Overridden version of size changing updates the layout of child visuals.
     *
     * @param aSize  New size for the layout.
     * @param aTransitionTime  Time for the transition to new size to take
     *               place.
     */
    IMPORT_C void SetSize(const THuiRealSize& aSize, TInt aTransitionTime=0);

    /**
     * Determines the size of a child visual according to the layout.
     *
     * @note if an anchor is defined at the specified ordinal position, the
     *      metric belonging to the anchor will override the metric specified
     *      in this layout's base unit. 
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aSize     New target size for the child.
     *
     * @return  <code>ETrue</code>, if a new size was determined. Otherwise,
     *          <code>EFalse</code> is returned and no size changes are
     *          needed.
     */
    IMPORT_C virtual TBool ChildSize(TInt aOrdinal, TSize& aSize);

    /**
     * Determines the position of a child visual according to the layout. 
     *
     * @note if an anchor is defined at the specified ordinal position, the
     *      metric belonging to the anchor will override the metric specified
     *      in this layout's base unit. 
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aPos      New position for the child.
     *
     * @return  <code>ETrue</code>, if a new position was determined.
     *          Otherwise, <code>EFalse</code> is returned and no position
     *          changes are needed.
     */
    IMPORT_C virtual TBool ChildPos(TInt aOrdinal, TPoint& aPos);

    /**
     * Determines the position and size of a child visual according to the layout.
     * The child position is determined using the base units of the layout. If a 
     * valid anchor has been defined then the size and or position of the 
     * corresponding child will be determined.
     *
     * @see CHuiLayout::ChildRect for full documentation
     */
    IMPORT_C virtual TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);
    
    /**
     * Returns the name of the class.
     *
     * @param aName  Name of the layout class.
     */
    void GetClassName(TDes& aName) const
        {
        aName = _L("CHuiAnchorLayout");
        }


private: // internal data types
    struct TCornerAnchor;
    struct TAnchor_deprecated;
    struct TEdgeAnchor;
    struct TAnchor;
    struct TAxis;

private: // methods

    /** 
     * Find the anchor at the given index in this layout's anchor list.
     *
     * @param aOrdinal Index into the anchor list.
     * @return Anchor at the requested ordinal. NULL if ordinal is out of range.
     *
     */
    const TAnchor* Anchor(TInt aOrdinal) const;

    /**
     * the axis structure specified
     * @param aAxis the index of the axis required
     * @return the axis specified
     */
    inline TAxis& Axis(THuiAnchorAxis aAxis) const;

    /**
     * Evaluate anchor along an axis
     *
     * @param aAxisId the axis id required
     * @param aAxis the axis
     * @param aNear outputs the near side value along the specified axis
     * @param aFar outputs the far side value along the specified axis
     * @return a value selected from CHuiLayout::EHuiLayoutChildRect, depending on which sides were updated
     */
    TInt EvaluateAxis(THuiAnchorAxis aAxisId, const TAxis& aAxis, TReal32& aNear, TReal32& aFar) const;

    /*
     * Get the metric reference for an anchor's offset along a specified axis
     *
     * @param aAxisID the axis needed
     * @param aEdge the edge
     * @return the metric reference value in pixels
     */
    TReal32 EdgeOffsetInPixels(THuiAnchorAxis aAxisId, const TEdgeAnchor& aEdge) const;
    
    /**
     * Calculate the actual pixel value for the supplied edge's position and size
     * relative to this layout.
     *
     * @param aAxisID the axis needed
     * @param aEdge the edge to be evaluated
     * @param aInnerPaddingPx the inner padding for this axis in pixels
     * @param aOffset the offset in pixels
     * @param aNearPx near coordinate of the inner rect
     * @param aNearCalculatedPx returns the near coordinate which will be modified if the edge was relevant
     * @param aFarPx far coordinate of the inner rect
     * @param aFarCalculatedPx returns the far coordinate which will be modified if the edge was relevant
     * @return a value selected from CHuiLayout::EHuiLayoutChildRect, depending on which sides were updated
     */
    TInt EvaluateEdgeAnchorInPixels(
        THuiAnchorAxis aAxisId,
        const TEdgeAnchor& aEdge,
        TReal32 aInnerPaddingPx,
        TReal32 aOffsetPx,
        TReal32 aNearPx, TReal32& aNearCalculatedPx, 
        TReal32 aFarPx, TReal32& aFarCalculatedPx) const;

    /**
     * Calculate the actual pixel value for the attached edge's position and size
     * relative to this layout.
     *
     * @param aAxisID the axis needed
     * @param aEdge the edge to be evaluated
     * @param aInnerPaddingPx the inner padding for this axis in pixels
     * @param aNearPx near coordinate of the inner rect
     * @param aNearCalculatedPx returns the near coordinate which will be modified if the edge was relevant
     * @param aFarPx far coordinate of the inner rect
     * @param aFarCalculatedPx returns the far coordinate which will be modified if the edge was relevant
     * @return a value selected from CHuiLayout::EHuiLayoutChildRect, depending on which sides were updated
     */
    TInt EvaluateEdgeAttachmentInPixels(
        THuiAnchorAxis aAxisId,
        const TEdgeAnchor& aEdge,
        TReal32 aInnerPaddingPx,
        TReal32 aNearPx, TReal32& aNearCalculatedPx, 
        TReal32 aFarPx, TReal32& aFarCalculatedPx) const;

    /**  
     * Detach edge
     *
     * @param aOrdinal the anchor to detach
     * @param aType the type of the edge to detach, must correspond to an edge
     *      type only (i.e. must not be a corner type).
     */
    void DetachEdge(TInt aOrdinal, THuiAnchorType aType);

    /*
     * Look for a cycle in the graph, starting with a specified edge
     *
     * @param aStartAxisId the axis of the specified starting edge
     * @param aStartAnchorProximity the proximity of the specified starting edge
     * @param aStartOrdinal the ordinal of the specified starting edge
     * @param aEdge the edge under consideration
     * @return ETrue if a cycle was found
     */
    TBool CheckForCycles(
        THuiAnchorAxis aStartAxisId,
        THuiAnchorProximity aStartAnchorProximity,
        TInt aStartOrdinal, 
        const TEdgeAnchor* aEdge) const;

private: // helper methods for dealing with internal structures

    /** 
     * convert an edge into a proximity. 
     *
     * @note if the edge is of type EHuiAnchorEdgeNone, then 
     * EHuiAnchorProximityNone will be returned, and aAxis will not be modified.
     *
     * @param aType the type
     * @param aAxis output parameter specifying along which axis the edge was defined
     * @return the proximity
     */    
    THuiAnchorProximity ConvertTypeToProximity(THuiAnchorType aType, THuiAnchorAxis& aAxis) const;

    /** 
     * convert an attachment origin into a proximity. 
     *
     * @note if the edge is of type EHuiAnchorAttachmentOriginNone, then 
     * EHuiAnchorProximityNone will be returned, and aAxis will not be modified.
     *
     * @param aAttachmentOrigin the origin
     * @param aAxis output parameter specifying along which axis the edge was defined
     * @return the proximity
     */    
    THuiAnchorProximity ConvertAttachmentOriginToProximity(THuiAnchorAttachmentOrigin aAttachmentOrigin, THuiAnchorAxis& aAxis) const;

    /** 
     * split a corner up into its respective edges
     *
     * @param aCorner the corner to convert
     * @param aEdgeH the horizontal edge
     * @param aEdgeV the vertical edge
     */
    void ConvertCornerToEdges(THuiAnchorType aCorner, THuiAnchorType& aEdgeH, THuiAnchorType& aEdgeV) const;
    
    /** 
     * split a corner attachment origin up into its respective edge origins
     *
     * @param aCornerOrigin the corner origin to convert
     * @param aEdgeH the horizontal origin
     * @param aEdgeV the vertical origin
     */
    void ConvertCornerAttachmentOriginToEdgeAttachmentOrigins(THuiAnchorAttachmentOrigin aCornerOrigin, THuiAnchorAttachmentOrigin& aEdgeOriginH, THuiAnchorAttachmentOrigin& aEdgeOriginV) const;
    
private: // helper methods for compatibility with deprecated APIs

    THuiAnchorType ConvertDeprecatedAnchorTypeToType(THuiAnchor aAnchor) const;
    
    THuiAnchorAttachmentOrigin ConvertDeprecatedOrginToAttachmentOrigin(THuiAnchorOrigin aOrigin) const;
    THuiAnchorAttachmentOrigin ConvertDeprecatedOriginsToAttachmentOrigin(THuiAnchorOrigin aHorizOrigin, THuiAnchorOrigin aVertOrigin) const;
    
    THuiMetric ConvertDeprecatedAnchorMetricAndTargetToHuiMetric(THuiAnchorMetric aMetric, TReal32 aTarget) const;  
    THuiXYMetric ConvertDeprecatedAnchorMetricsAndOffsetToHuiMetric(THuiAnchorMetric aHorizMetric, THuiAnchorMetric aVertMetric, const THuiTimedPoint& aOffset) const;

private: // data

    struct THuiAnchorLayoutPrivateData;
    THuiAnchorLayoutPrivateData* iHuiAnchorLayoutPrivateData; // owned
    };

#endif  // __HUIANCHORLAYOUT_H__


