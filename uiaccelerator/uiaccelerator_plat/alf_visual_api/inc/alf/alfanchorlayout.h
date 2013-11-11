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
* Description:   Anchor layout definition.
*
*/



#ifndef C_ALFANCHORLAYOUT_H
#define C_ALFANCHORLAYOUT_H

#include <alf/alflayout.h> 

/** for internal use only */
enum TAlfAnchorBitmask
    { 
    EAlfAnchorBitmaskHorizontal = 0x000F,
    EAlfAnchorBitmaskVertical = 0x00F0
    };

/** for internal use only */
enum TAlfAnchorBitmaskShift
   {
    EAlfAnchorBitmaskShiftHorizontal = 0,
    EAlfAnchorBitmaskShiftVertical = 4
    };

/** for internal use only */
enum TAlfAnchorAxis
    {
    EAlfAnchorAxisHorizontal = 0,
    EAlfAnchorAxisVertical = 1
    };

/** for internal use only */
enum TAlfAnchorProximity
    {
    EAlfAnchorProximityNone = 0,    
    EAlfAnchorProximitySize,
    EAlfAnchorProximityNear,
    EAlfAnchorProximityCenter,
    EAlfAnchorProximityFar
    };

/** 
 * Anchor type. Used to determine which edge or corner of a child visual is 
 * defined by an anchor.
 *
 * @note that if width or height are used as one anchor, they must be combined 
 * with an appropriate anchor in order to unambiguously determine the size and 
 * position of the child.
 */
enum TAlfAnchorType
    {
    EAlfAnchorTypeNone = EAlfAnchorProximityNone,
    
    EAlfAnchorTypeWidth = EAlfAnchorProximitySize << EAlfAnchorBitmaskShiftHorizontal,
    EAlfAnchorTypeLeft = EAlfAnchorProximityNear << EAlfAnchorBitmaskShiftHorizontal,
    EAlfAnchorTypeHCenter = EAlfAnchorProximityCenter << EAlfAnchorBitmaskShiftHorizontal,
    EAlfAnchorTypeRight = EAlfAnchorProximityFar << EAlfAnchorBitmaskShiftHorizontal,
    
    EAlfAnchorTypeHeight = EAlfAnchorProximitySize << EAlfAnchorBitmaskShiftVertical,
    EAlfAnchorTypeTop = EAlfAnchorProximityNear << EAlfAnchorBitmaskShiftVertical,
    EAlfAnchorTypeVCenter = EAlfAnchorProximityCenter << EAlfAnchorBitmaskShiftVertical,
    EAlfAnchorTypeBottom = EAlfAnchorProximityFar << EAlfAnchorBitmaskShiftVertical,
    
    
    EAlfAnchorTypeSize = EAlfAnchorTypeHeight | EAlfAnchorTypeWidth,

    EAlfAnchorTypeTopLeft = EAlfAnchorTypeTop | EAlfAnchorTypeLeft,
    EAlfAnchorTypeTopCenter = EAlfAnchorTypeTop | EAlfAnchorTypeHCenter,
    EAlfAnchorTypeTopRight = EAlfAnchorTypeTop | EAlfAnchorTypeRight,

    EAlfAnchorTypeCenterLeft = EAlfAnchorTypeVCenter | EAlfAnchorTypeLeft,
    EAlfAnchorTypeCenter = EAlfAnchorTypeVCenter | EAlfAnchorTypeHCenter,
    EAlfAnchorTypeCenterRight = EAlfAnchorTypeVCenter | EAlfAnchorTypeRight,
    
    EAlfAnchorTypeBottomLeft = EAlfAnchorTypeBottom | EAlfAnchorTypeLeft,
    EAlfAnchorTypeBottomCenter = EAlfAnchorTypeBottom | EAlfAnchorTypeHCenter,
    EAlfAnchorTypeBottomRight = EAlfAnchorTypeBottom | EAlfAnchorTypeRight
    };

/** 
 * Anchor origin, relative to a particular edge or corner. Direction is 
 * measured along positive x and y axes, as modified by the metric used.
 */
enum TAlfAnchorAttachmentOrigin
    {
    EAlfAnchorAttachmentOriginNone = EAlfAnchorTypeNone,
    
    EAlfAnchorAttachmentOriginLeft = EAlfAnchorTypeLeft,
    EAlfAnchorAttachmentOriginHCenter = EAlfAnchorTypeHCenter,
    EAlfAnchorAttachmentOriginRight = EAlfAnchorTypeRight,
    
    EAlfAnchorAttachmentOriginTop = EAlfAnchorTypeTop,
    EAlfAnchorAttachmentOriginVCenter = EAlfAnchorTypeVCenter,
    EAlfAnchorAttachmentOriginBottom = EAlfAnchorTypeBottom,
    
    EAlfAnchorAttachmentOriginTopLeft = EAlfAnchorTypeTopLeft, 
    EAlfAnchorAttachmentOriginTopCenter = EAlfAnchorTypeTopCenter, 
    EAlfAnchorAttachmentOriginTopRight = EAlfAnchorTypeTopRight, 

    EAlfAnchorAttachmentOriginCenterLeft = EAlfAnchorTypeCenterLeft,
    EAlfAnchorAttachmentOriginCenter = EAlfAnchorTypeCenter, 
    EAlfAnchorAttachmentOriginCenterRight = EAlfAnchorTypeCenterRight, 
    
    EAlfAnchorAttachmentOriginBottomLeft = EAlfAnchorTypeBottomLeft, 
    EAlfAnchorAttachmentOriginBottomCenter = EAlfAnchorTypeBottomCenter, 
    EAlfAnchorAttachmentOriginBottomRight = EAlfAnchorTypeBottomRight
    };
    
/** 
 * Type of attachment point used when attaching an anchor to another
 * anchor in the same layout.
 */
enum TAlfAnchorAttachmentType
    {
    EAlfAnchorAttachToParent = -1
    };
    
    
/** 
 * Determines which corner of a child visual is positioned by the anchor
 * @deprecated, only retained for use with deprecated SetAnchor routines, use
 *                        TAlfAnchorAttachmentOrigin instead with new APIs
 */
enum TAlfAnchor
    {
    /** Anchor that defines the top left corner of a visual. */
    EAlfAnchorTopLeft,

    /** Anchor that defines the bottom right corner of a visual. */
    EAlfAnchorBottomRight
    };

/** 
 * Anchor origins.
 * @deprecated, only retained for use with deprecated SetAnchor routines, use
 *                        TAlfAnchorAttachmentOrigin instead with new APIs
 */
enum TAlfAnchorOrigin
    {
    EAlfAnchorOriginLeft,
    EAlfAnchorOriginHCenter,
    EAlfAnchorOriginRight,
    EAlfAnchorOriginTop,
    EAlfAnchorOriginVCenter,
    EAlfAnchorOriginBottom
    };


/** 
 *Anchor metric types, for offsets and sizes.
 * @deprecated, only retained for use with deprecated SetAnchor routines, use
 *                        TAlfMetric instead with new APIs
 */
enum TAlfAnchorMetric
    {
    /** Use absolute pixel coordinates to specify the values. */        
    EAlfAnchorMetricAbsolute,
    /** 
     * Use the size of the visual 
     * either normalized between 0.0 (at origin) and 1.0 (farthest away edge)
     * or -0.5 (min) and +0.5 (0.0 being always at the origin). For example if you 
     * use EAlfAnchorOriginLeft with EAlfAnchorMetricRelativeToSize, the 
     * left edge is at 0.0, and the right edge at 1.0. But if you use any of 
     * the center origins (EAlfAnchorOriginHCenter or EAlfAnchorOriginVCenter),
     * then the left/top edge is at -0.5 and right/top edge is at +0.5.
     */
    EAlfAnchorMetricRelativeToSize
    };

class TAlfTimedPoint;
class CAlfControl;

/**
 * CAlfAnchorLayout is a layout that allows visuals to be positioned with 
 * flexible relative anchor points. 
 *
 * To use this class, an anchor layout is created, and then a number of anchors
 * are added to it using calls to the Attach() methods. The visuals
 * inside the layout will be stretched between these anchor points.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfAnchorLayout : public CAlfLayout
    {

public:

    /**
     * Two-phased constructor, which attach new object into the control.
     * @param aOwnerControl Control that owns this new obejct.
     * @param aParentLayout If given, the new object is set as child.
     */
    IMPORT_C static CAlfAnchorLayout* AddNewL(CAlfControl& aOwnerControl,
                                              CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfAnchorLayout();

    /**
     * From CAlfLayout. Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Virtual destructor.
     */
    IMPORT_C ~CAlfAnchorLayout();
    
    /**
     * Resets all the anchors.
     */
    IMPORT_C void Reset();

    /**
     * Anchors a child visual's corner to specific position within this layout.
     *
     * @deprecated, use new @c Attach APIs with TAlfMetric support.
     * See the relevant API documentation for an example of how to migrate your code
     *
     * Each child can have two anchors: the top left and bottom right corners 
     * of the child visual. These anchors can be fastened to any point within 
     * this layout, as specified by the other parameters.
     *
     * In a typical case you set two anchors (both EAlfAnchorTopLeft and 
     * EAlfAnchorBottomRight), which specifies both size and position for the 
     * child. However it is also possible to set only the EAlfAnchorTopLeft 
     * anchor, which only positions the child without affecting it's size.
     * 
     * @see RemoveAnchor() to remove an achor definition.
     * 
     * @note  After updating anchors, you should call 
     *        CAlfLayout::UpdateChildrenLayout(), to cause the anchors to be 
     *        applied to child positions and/or sizes.
     * 
     * For example the following code will anchor child visual number 0 in 
     * the middle of the layout, with a 25% border around the outside:
     *
     * \code
     *      // set top left corner position of 1st child ..
     *	    iLayout->SetAnchor(EAlfAnchorTopLeft, 0,
     *	        EAlfAnchorOriginLeft, 
     *          EAlfAnchorOriginTop,
     *          EAlfAnchorMetricRelativeToSize, // With EAlfAnchorOriginLeft means that 0.0 goes to left edge, and 1.0 to the right
     *          EAlfAnchorMetricRelativeToSize, // With EAlfAnchorOriginTop means that 0.0 goes to top edge, and 1.0 to the bottom
     *          TAlfTimedPoint(0.25, 0.25));
     *      // .. and set the bottom right corner also to fix the size
     * 	    iLayout->SetAnchor(EAlfAnchorBottomRight, 0,
     *	        EAlfAnchorOriginRight, 
     *          EAlfAnchorOriginBottom,
     *   	EAlfAnchorMetricRelativeToSize, 
     *          EAlfAnchorMetricRelativeToSize,
     *          TAlfTimedPoint(-0.25, -0.25)); 
     * \endcode
     *
     * @todo  Specifying a transition in the timed point may be useless:
     *        layouts are evaluated pretty seldom.
     *
     * @param aAnchor       Type of the anchor (either EAlfAnchorTopLeft for the 
     *                      top left corner or EAlfAnchorBottomRight for bottom right
     *                      corner).
     * @param aOrdinal      Ordinal of the child visual that this anchor applies to.
     * @param aHorizOrigin  Horizontal origin for the anchor position (aPosition). 
     *                      Defines the point inside the layout on which the 
     *                      evaluation of the child's placement is based. May be 
     *                      EAlfAnchorOriginLeft (left edge), 
     *                      EAlfAnchorOriginHCenter (center) or
     *                      EAlfAnchorOriginRight (right edge).
     * @param aVertOrigin   Vertical origin for the anchor position (aPosition). 
     *                      Defines the point inside the layout on which the 
     *                      evaluation of the child's placement is based. Possible options 
     *                      are EAlfAnchorOriginTop (top), EAlfAnchorOriginVCenter 
     *                      (center) and EAlfAnchorOriginBottom (bottom).
     * @param aHorizMetric  Specifies the metric to use to define the
     *                      horizontal position: EAlfAnchorMetricAbsolute to use 
     *                      absolute pixels or EAlfAnchorMetricRelativeToSize to 
     *                      use normalized values relative to layout size.
     * @param aVertMetric   Specifies the metric used in the vertical position:
     *                      EAlfAnchorMetricAbsolute to use absolute pixels or 
     *                      EAlfAnchorMetricRelativeToSize to use normalized 
     *                      values relative to layout size.     
     * @param aPosition     The achor position as an offset from the origin point, 
     *                      applied using the metric specified with aHorizMetric 
     *                      and aVertMetric.
     * @return                   Error code
     *
     */
    IMPORT_C TInt SetAnchor(
        TAlfAnchor aAnchor, 
        TInt aOrdinal,
        TAlfAnchorOrigin aHorizOrigin,
        TAlfAnchorOrigin aVertOrigin,
        TAlfAnchorMetric aHorizMetric,
        TAlfAnchorMetric aVertMetric,
        const TAlfTimedPoint& aOffset);
        
    /**
	 * Convenience method for setting top left and bottom right anchors in a single call.
	 *
	 * This method sets size-relative anchors for a single child visual.
	 *
	 * @param aOrdinal           Ordinal of child visual to set anchors for.
     * @param aTlHorizOrigin     Horizontal origin for the top left anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. May be 
     *                           EAlfAnchorOriginLeft (left edge), 
     *                           EAlfAnchorOriginHCenter (center) or
     *                           EAlfAnchorOriginRight (right edge).
     * @param aTlVertOrigin      Vertical origin for the top left anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. Possible options 
     *                           are EAlfAnchorOriginTop (top), EAlfAnchorOriginVCenter 
     *                           (center) and EAlfAnchorOriginBottom (bottom).
     * @param aTopLeftOffset     The achor position as an offset from the origin point, 
     * @param aBrHorizOrigin     Horizontal origin for the bottom right anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. May be 
     *                           EAlfAnchorOriginLeft (left edge), 
     *                           EAlfAnchorOriginHCenter (center) or
     *                           EAlfAnchorOriginRight (right edge).
     * @param aBrVertOrigin      Vertical origin for the bottom right anchor position (aPosition). 
     *                           Defines the point inside the layout on which the 
     *                           evaluation of the child's placement is based. Possible options 
     *                       	 are EAlfAnchorOriginTop (top), EAlfAnchorOriginVCenter 
     *                           (center) and EAlfAnchorOriginBottom (bottom).     
     * @param aBottomRightOffset The achor position as an offset from the origin point,      
     * @return                   Error code.
     * 
	 *
	 * @see CAlfAnchorLayout::SetAnchor() for more information about anchors.
	 */
    IMPORT_C TInt SetRelativeAnchorRect(TInt aOrdinal,
                                        TAlfAnchorOrigin aTlHorizOrigin,
                                        TAlfAnchorOrigin aTlVertOrigin,
                                        const TAlfRealPoint& aTopLeftOffset,
                                        TAlfAnchorOrigin aBrHorizOrigin,
                                        TAlfAnchorOrigin aBrVertOrigin,
                                        const TAlfRealPoint& aBottomRightOffset);
                                        
    /**
     * Undefines an anchor.
     * @see SetAnchor()
     * @deprecated instead use @c Detach and see relevant API documentation for
     *      example code
     */
    IMPORT_C void RemoveAnchor(TAlfAnchor aAnchor, TInt aOrdinal);

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
     * EAlfAnchorTypeTopLeft and EAlfAnchorTypeBottomRight), taking care
     * to ensure that they are sufficiently orthogonal in order to define all needed
     * coordinates. However it is also possible to set only one corner anchor, 
     * which only positions the child without affecting it's size. Alternatively,
     * one anchor can be specified as a corner, and then the other can be 
     * specified as EAlfAnchorTypeSize; similarly the coordinates along
     * one axis can be specified by an appropriate combination of edge and 
     * width or height anchors.
     *
     * The position of each anchor relative to this layout is determined
     * by the choice of origin (i.e. which point of the layout the 
     * child's is attached to), and the offset position, which is specified
     * using a TAlfMetric or related type. All available features of 
     * TAlfMetric may be used to affect the resulting position, providing the 
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
     *      @c CAlfLayout::UpdateChildrenLayout(), to cause the anchors to be 
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
     *          EAlfAnchorTypeLeft,
     *          TAlfXYMetric(0.25, EAlfMetricNormalized),
     *          EAlfAnchorAttachmentOriginLeft);
     *	    iLayout->Attach(0, 
     *          EAlfAnchorTypeRight,
     *          TAlfXYMetric(-0.25, EAlfMetricNormalized),
     *          EAlfAnchorAttachmentOriginRight);
     *	    iLayout->Attach(0, 
     *          EAlfAnchorTypeTop,
     *          TAlfXYMetric(0.25, EAlfMetricNormalized),
     *          EAlfAnchorAttachmentOriginTop);
     *	    iLayout->Attach(0, 
     *          EAlfAnchorTypeBottom,
     *          TAlfXYMetric(-0.25, EAlfMetricNormalized),
     *          EAlfAnchorAttachmentOriginBottom);
     *
     *      // using corners...
     *	    iLayout->Attach(1, 
     *          EAlfAnchorTypeTopLeft,
     *          TAlfXYMetric(0.25, EAlfMetricNormalized),
     *          EAlfAnchorAttachmentOriginTopLeft);
     * 	    iLayout->Attach(1, 
     *	        EAlfAnchorTypeBottomRight, 
     *         TAlfXYMetric(-0.25, EAlfMetricNormalized),
     *	        EAlfAnchorAttachmentOriginBottomRight);
     * 
     *      // .. and using a box
     * 	    iLayout->Attach(2, TAlfBoxMetric(
     *          TAlfMetric(0.25, EAlfMetricNormalized),
     *          TAlfMetric(-0.25, EAlfMetricNormalized),
     *          TAlfMetric(0.25, EAlfMetricNormalized),
     *          TAlfMetric(-0.25, EAlfMetricNormalized)));
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
     *      use the default value of EAlfAnchorAttachToParent in order to attach to parent
     * @return Error code
     *
     */
    IMPORT_C TInt Attach(
        TInt aOrdinal,
        TAlfAnchorType aType, 
        const TAlfMetric& aOffset,
        TAlfAnchorAttachmentOrigin aAttachmentOrigin,
        TInt aAttachmentOrdinal = EAlfAnchorAttachToParent);

    /** 
     * Attach a corner of a child visual  to a specific position 
     * within this layout. 
     *
     * @see Attach() for all API documentation
     */
    IMPORT_C TInt Attach(
        TInt aOrdinal,
        TAlfAnchorType aType, 
        const TAlfXYMetric& aOffset,
        TAlfAnchorAttachmentOrigin aAttachmentOrigin = EAlfAnchorAttachmentOriginTopLeft,
        TInt aAttachmentOrdinal = EAlfAnchorAttachToParent);

    /** 
     * Attach all four edges of a child visual  to a specific position 
     * within this layout.
     *
     * @note, will detach all existing edges first. In case of error, all
     * edges that were successfull added before the error occurred will be 
     * detached.
     *
     * @see Attach() for all API documentation
     * @param aAttachmentOrigin if set to EAlfAnchorAttachmentOriginNone, each edge will 
     *        be attached to the corresponding edge
     */
    IMPORT_C TInt Attach(
        TInt aOrdinal,
        const TAlfBoxMetric& aOffset,
        TAlfAnchorAttachmentOrigin aAttachmentOrigin = EAlfAnchorAttachmentOriginTopLeft,
        TInt aAttachmentOrdinal = EAlfAnchorAttachToParent);


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
    IMPORT_C void Detach(TInt aOrdinal, TAlfAnchorType aType);

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);

    /** 
     * From CAlfLayout
     * @see CAlfLayout
     */
    IMPORT_C TAlfXYMetric BaseUnit() const; 


protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

                                
private:

    // Private data structure. Owned.
    struct TAnchorLayoutPrivateData;
    TAnchorLayoutPrivateData* iAnchorLayoutData;

    };



#endif // C_ALFANCHORLAYOUT_H
