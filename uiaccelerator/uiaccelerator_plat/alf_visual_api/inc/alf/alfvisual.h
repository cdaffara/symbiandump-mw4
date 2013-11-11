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
* Description:   Base class for visuals
*
*/



#ifndef C_ALFVISUAL_H
#define C_ALFVISUAL_H

class CAlfEnv;
class CAlfVisual;
class CAlfControlGroup;
class CAlfControl;
class MAlfEventHandler;
struct TAlfBoxMetric;
class CAlfDropShadow;

#include <e32base.h>
#include <alf/alftimedvalue.h>
#include <alf/alfpropertyowner.h>

/** Visual types. */
enum TAlfVisualType
    {
    EAlfVisualTypeVisual,
    EAlfVisualTypeLine,
    EAlfVisualTypeText,
    EAlfVisualTypeLCTText,
    EAlfVisualTypeImage,
    EAlfVisualTypeMesh,
    EAlfVisualTypeCanvas
    };

/** Visual flags. */
enum TAlfVisualFlags
    {
    /** Manual layout means that layouts can't change the position and
        size of the visual; the owner of the visual is expected to
        manually maintain the visual's placement. */
    EAlfVisualFlagManualSize = 0x1,
    EAlfVisualFlagManualPosition = 0x2,
    EAlfVisualFlagManualLayout = EAlfVisualFlagManualSize |
                                 EAlfVisualFlagManualPosition,

    /** 
     * This enum is retained for source compatibility.
     * @deprecated 
     * @note See EAlfVisualFlagClipping
     */
    AlfVisualFlagClipping = 0x4,
    
    /** Notify owner control when the visual's layout has been updated.
        Notication is sent after all children have been updated. */
    EAlfVisualFlagLayoutUpdateNotification = 0x8,

    /** When the visual is drawn it will use a 3D projection. */
    EAlfVisualFlag3DProjection = 0x10,

    /** No depth test when drawing with a 3D projection. */
    EAlfVisualFlagNoDepthOcclusion = 0x20,

    /** Use a local horizon with a 3D projection. Otherwise, the global
        display horizon is used instead. */
    EAlfVisualFlagLocalHorizon = 0x40,

    /** Contents of the visual are clipped to its display rectangle. */
    EAlfVisualFlagClipping = 0x80,

    EAlfVisualFlagHOriginCenter = 0x100,

    EAlfVisualFlagHOriginRight = 0x200,

    EAlfVisualFlagVOriginCenter = 0x400,

    EAlfVisualFlagVOriginBottom = 0x800,

    /** Visual has changed and needs refresh. */
    EAlfVisualChanged = 0x1000,
    
    /** When the visual is drawn, the depth buffer shouldn't be updated. */
    EAlfVisualFlagNoDepthUpdate = 0x2000,

    /** Use the visual's local layout transition time, do not inherit from parent. */
    EAlfVisualFlagManualTransitionTime = 0x4000,

    /** Draw above the normal drawing order. */
    EAlfVisualFlagDrawAfterOthers = 0x10000,
    
    /** Do not automatically update the layout of children when doing 
        changes in a layout (e.g., when reordering children). */
    EAlfVisualFlagFreezeLayout = 0x20000,
    
    /** The visual will be ignored when checking for possible pointer event
        interaction. */
    EAlfVisualFlagIgnorePointer = 0x40000,

    /** 
     * automatically reverse the base unit sign in the X axis if the current locale setting is 
     * for mirrored layout
     *
     * @note this will only affect the positions of visuals contained
     * within layout visuals, the contents within a visual such as a text visual will not be 
     * affected. 
     */
    EAlfVisualFlagAutomaticLocaleMirroringEnabled = 0x80000,
    
    /** 
     * RnD: if set, some of the debug features are enabled. See programmers'
     * guide for more info.
     *
     * If the central repository has the setting to draw selected visual outline, 
     * setting this flag will draw outline for the visual and its child visuals
     */    
    EAlfVisualFlagEnableDebugMode  = 0x100000,

    //Code added by Devayani
    /** 
     * Setting this flag makes visual and its children Inactive i.e. it is ignored from roster operations 
     * - Draw, Handle event, change notifications etc.
     * Unsetting flag can again make visual tree active.
     * Note that flag change does not take effect unless that display area is refreshed.
     **/
    EAlfVisualFlagInactive = 0x200000,

    /** 
     * Setting this flag informs renderer that visual is to be considered as opaque and thus
     * renderer may choose not to draw visuals behind opaque visual. Note that visual
     * attribute "opacity" is not affected by this flag.
     **/
    EAlfVisualFlagOpaqueHint = 0x400000,
    
    EAlfVisualFlagLast = 0xFFFFFFFF
    };

/** Horizontal origin of a visual. */
enum TAlfVisualHOrigin
    {
    EAlfVisualHOriginLeft = 0,
    EAlfVisualHOriginCenter = 1,
    EAlfVisualHOriginRight = 2
    };


/** Vertical origin of a visual. */
enum TAlfVisualVOrigin
    {
    EAlfVisualVOriginTop = 0,
    EAlfVisualVOriginCenter = 1,
    EAlfVisualVOriginBottom = 2
    };

const TText KAlfTagSeparator = ':';

const TInt KAlfTactileFeedbackEventTypeNone = -1;

class CAlfControl;
class CAlfLayout;
class CAlfDisplay;
class CAlfEnv;
class CAlfBrushArray;
class CAlfGenComponent;
class MAlfMappingFunction;
class CAlfTransformation;

/** Fixed properties for visuals, Use these for efficient copying of visual properties. */ 
class AlfVisualProperties
    {
    public:
    enum 
        {
        ENone = 0x0,
        ESize = 0x1,
        EPosition = 0x2,
        EOpacity = 0x4,
        EDepthOffset = 0x8,
        ESecondaryImageAlpha = 0x10, 
        EScale = 0x20,
        EPosOffset = 0x40,
        ETextVisShadowOpacity = 0x80,
        EImageVisTurnAngle = 0x100,
        ELineVisThickness = 0x200,
        ELineVisShadowThickness = 0x400,
        ELineVisStartPos = 0x800,
        ELineVisEndPos = 0x1000,
        EMeshVisYawAngle = 0x2000, 
        EMeshVisPitchAngle = 0x4000,
        EMeshVisMaterial = 0x8000,
        ELayoutScrollOffset = 0x10000,
        // Add new properties here (next value: 0x20000)
        
        EAll = 0xFFFFFFFF
        };
    };


/**
 *  Visual base class
 *
 *  All visuals and layout are derived from this.
 *  Usage:
 *  @code
 *    // #1 Visual object usage
 *    CAlfVisual* newVisual = CAlfVisual::AddNewL( *myMainControl, mMainGridLayout );
 *    newVisual->EnableBrushesL();
 *    newVisual->Brushes->AppendL( borderBrush, EAlfHasOwnership );
 *    
 *   // #2 as a base class
 *   class CMyVisual : public CAlfVisual
 *       {    
 *   public:
 *       // allocation function
 *       CMyVisual(CAlfControl& aOwnerControl, CAlfLayout* aParentLayout = 0);  
 *    
 *       // Function documentation
 *       void MyVisualSpecificFunction();
 *   protected:
 *
 *       ~CAlfVisual();
 *   private:
 *       TInt mMyPrivateData;    
 *       };
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfVisual : public CAlfPropertyOwner
    {

public:

    /**
     * Constructor
     */
    IMPORT_C CAlfVisual();

    /**
     * ConstructL
     */
    IMPORT_C virtual void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor
     */
    IMPORT_C virtual ~CAlfVisual();
    
    /**
     * Destroys a hierarchy of visuals, along with this visual. The visuals
     * are first removed from their owner.
     */
    IMPORT_C virtual void RemoveAndDestroyAllD();
    
    /**
     * Return the server side handle
     *
     * @return Handle to the server side CAlfVisual object. 0 if not set.
     */
    IMPORT_C TInt Identifier() const;
     
    /**
     * Sets the layout this visual belongs to.
     *
     * @param aLayout Parent layout
     */
    void SetLayout(CAlfLayout* aLayout);
    
    /**
     * Set a flag that affects the visual's behaviour.
     *
     * @param aFlag  Flag to set.
     */
    IMPORT_C void SetFlag(TAlfVisualFlags aFlag);

    /**
     * Sets one or more flags that affect the visual's behaviour.
     *
     * @param aAnyFlags  One or more flags to set.
     */
    IMPORT_C void SetFlags(TUint aAnyFlags);

    /**
     * Unset flags that affect the visual's behaviour.
     *
     * @param aFlag  Flag to clear.
     */
    IMPORT_C void ClearFlag(TAlfVisualFlags aFlag);

    /**
     * Clear one or more flags that affect the visual's behaviour.
     *
     * @param aAnyFlags  One or more flags to clear.
     */
    IMPORT_C void ClearFlags(TUint aAnyFlags);

    /**
     * Returns the flags of the visual.
     * @return Flags.
     */
    IMPORT_C TUint Flags() const;
    
    /**
     * Returns the layout this visual belongs to.
     *
     * @return Parent layout. NULL if not set.
     */
    IMPORT_C CAlfLayout* Layout() const;
    
    /**
     * Returns the position of the visual. This is set by UpdateLayout().
     *
     * @return Position.
     */
    IMPORT_C TAlfTimedPoint Pos() const;

    /**
     * Sets the position of the visual.
     */
    IMPORT_C void SetPos(const TAlfRealPoint& aPos, TInt aTransitionTime = 0);
    
    /**
     * Returns the size of the visual. This is set by UpdateLayout().
     */
    IMPORT_C const TAlfTimedPoint Size() const;

    /**
     * Sets the size of the visual.
     */
    IMPORT_C void SetSize(const TAlfRealSize& aSize, TInt aTransitionTime = 0);
    
    /**
     * Returns the overall opacity affecting the owner's visuals. All visuals 
     * owned by this object are affected by this opacity.
     *
     * @return  Timed value.
     */
    IMPORT_C const TAlfTimedValue& Opacity() const;
    
    /**
     * Set opacity
     *
     * @param aOpacity Opacity value.
     */
    IMPORT_C void SetOpacity( const TAlfTimedValue& aOpacity );
    
    /**
     * Returns the environment where this visual belongs into.
     *
     * @return  Environment.
     */
    IMPORT_C CAlfEnv& Env() const;
    
    /**
     * Returns the display of the visual (if shown).
     *
     * @return  Pointer to <code>CAlfDisplay</code> instance, or
     * <code>NULL</code> if no display is associated with the visual.
     */
    IMPORT_C CAlfDisplay* Display() const;
    
    /**
     * Sets the display on which the visual is being shown.
     *
     * @param aDisplay  Pointer to display onto which to attach the visual
     */
    void SetDisplay(CAlfDisplay* aDisplay);
        
    /**
     * Determines the tag of the visual.
     *
     * @return  Tag descriptor of the visual.
     *
     * @see CAlfVisual::SetTagL()
     * @see CAlfVisual::IsTagged()
     * @see CAlfVisual::FindTag()
     */
    IMPORT_C const TDesC8& Tag() const;
    
    /**
     * Update the layout of all children. Subclasses can call this to
     * recalculate the layout of all children.
     *
     * @param aTransitionTime  Time for layout transition in milliseconds.
     *                         Any negative value uses value given in 
     *                         CAlfEnv::StaticSetLayoutTransitionTime()
     */
    IMPORT_C virtual void UpdateChildrenLayout(TInt aTransitionTime = 0);
    
    /**
     * Calculates the visual's bounding rectangle in display coordinates.
     * Display coordinates are defined relative to the display's visible area.
     * Uses the target values of the position and size, which means transitions
     * are always immediate in the returned rectangle.
     *
     * @return Rectange.
     */
    IMPORT_C TAlfRealRect DisplayRectTarget() const;
    
    /**
     * Convert local coordiantes to display coordinates.
     * Display coordinates are defined relative to the display's visible area.
     *
     * @return Converted point.
     */
    IMPORT_C TPoint LocalToDisplay(const TPoint& aPoint) const;
    
    /**
     * Convert local coordinates to display coordinates.
     * Display coordinates are defined relative to the display's visible area.
     *
     * @param aPoint  Point in local coordinates.
     * @return  Point in display coordinates.
     */
    IMPORT_C TAlfRealPoint LocalToDisplay(const TAlfRealPoint& aPoint) const;
    
    /*
     * Convert display coordinates to local coordinates.
     * Display coordinates are defined relative to the display's visible area.
     *
     * @param aPoint  Point in display coordinates.
     * @return  Point in local coordinates.
     */
    IMPORT_C TPoint DisplayToLocal(const TPoint& aPoint) const;
    
    /**
     * Convert display coordinates to local coordinate.
     * Display coordinates are defined relative to the display's visible area.
     *
     * @param aPoint  Point in display coordinates.
     * @return  Point in local coordinates.
     */
    IMPORT_C TAlfRealPoint DisplayToLocal(const TAlfRealPoint& aPoint) const;
    
    /**
     * Enables or disables brushes for the visual.
     */
    IMPORT_C void EnableBrushesL(TBool aEnabled = ETrue);
    
    /**
     * Returns the brushes of the visual.
     *
     * @return  Brush array. <code>NULL</code>, if not enabled.
     */
    IMPORT_C CAlfBrushArray* Brushes();
  
    /**
     * Moves the visual to the front inside its parent layout. 
     * In other words, the visual becames the last child of the parent.
     * Causes layout recalculation.
     */
    IMPORT_C void MoveToFront(TInt aTransitionTime = 0);
    
    /**
     * Enables or disables transformation of the control group.
     *
     * @param aIsTransformed ETrue for enabling.
     */
    IMPORT_C void EnableTransformationL( TBool aIsTransformed = ETrue );
    
    /**
     * Returns the transformation of the control group.
     *
     * @return  Transformation steps.
     */
    IMPORT_C CAlfTransformation& Transformation();
    
    /**
     * Sets the number of pixels that the visual uses for padding its contents.
     * Sets both the horizontal and vertical padding.
     *
     * @note that the left and right paddings will be the same, and the top and bottom
     *      paddings will be the same.
     * @deprecated migrate to using metrics instead
     *
     * @param aPadding  Number of padding pixels.
     */
    IMPORT_C void SetPadding(TInt aPadding);

    /**
     * Sets the number of pixels that the visual uses for padding its contents.
     * Sets the horizontal and vertical padding separately.
     *
     * @note that the left and right paddings will be the same, and the top and bottom
     *      paddings will be the same.
     * @deprecated migrate to using metrics instead
     *
     * @param aPadding  Number of padding pixels.
     */
    IMPORT_C void SetPadding(const TPoint& aPadding);

    /**
     * Sets the amount of padding that the visual uses for padding its contents.
     * The box metric can have a different amount on all sides.
     *
     * @note as paddings are relative to the layout visual, if relative units are required, 
     * it is advisable to use EAlfUnitRelativeToMySize so that the paddings are relative
     * to the size of this layout visual
     *
     * @param aPadding  Amount of padding.
     */
    IMPORT_C void SetPadding(const TAlfBoxMetric& aPadding);

    /**
     * Returns the padding of the visual, in pixels.
     *
     * @return  Padding, in pixels. iTl.iX is the left padding, iTl.iY is 
     *          the top padding, iBr.iX is the right padding, and iBr.iY
     *          is the bottom padding. The width and height of the returned 
     *          rectangle are meaningless.
     */
    IMPORT_C TAlfRealRect PaddingInPixels() const;

    /**
     * Sets the tag of the visual.
     *
     * The tag is a descriptor, in the form of one or more colon-separated
     * character sequences. By default, visuals have no tag (""). Normally,
     * a visual could have a single tag, for example ("caption").
     * The tag descriptor of the visual may contain multiple tags,
     * for example: "tag1:tag2:tag3".
     *
     * Tags can be used for identifying visuals. In a typical use case, a control
     * could identify which of its visuals represent certain elements of the
     * control based on the tag descriptors. In this case, the tags would be
     * set by an outside party that also sets the tags of the visuals.
     *
     * @param aTag  Tag descriptor for the visual.
     *
     * @see CAlfVisual::Tag()
     * @see CAlfVisual::IsTagged()
     * @see CAlfVisual::FindTag()
     */
    IMPORT_C void SetTagL(const TDesC8& aTag);

    /**
     * Determines if the visual has a tag.
     *
     * @return  <code>ETrue</code>, if the visual has been tagged with SetTagL().
     *          <code>EFalse</code> otherwise.
     *
     * @see CAlfVisual::Tag()
     * @see CAlfVisual::SetTagL()
     * @see CAlfVisual::FindTag()
     */
    IMPORT_C TBool IsTagged() const;

    /**
     * Finds a visual whose tag descriptor contains a specific tag.
     *
     * CAlfVisual::FindTag() is the basic implementation that checks if
     * this visual's tag matches the specified tag. CAlfLayout::FindTag()
     * extends the search into child visuals, covering recursively entire
     * visual hierarchies.
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
     * @see CAlfLayout::FindTag()
     */
    IMPORT_C virtual CAlfVisual* FindTag(const TDesC8& aTag);

    /**
     * Construct a visual.
     *
     * @todo This method is probably to allow extended visual types to be created.
     *
     * @param aOwner Who owns this new control.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner, TInt aImplementationId, TInt aImplementationUid, const TDesC8& aConstructionParams );

    IMPORT_C CAlfGenComponent* Comms() const;

    
    /**
     * Gets the depth offset.
     *
     * @return Depth offset.
     */
    IMPORT_C TAlfTimedValue DepthOffset() const;

    /**
     * Sets the depth offset.
     *
     * @param aDepthOffset Depth offset.
     */
    IMPORT_C void SetDepthOffset(const TAlfTimedValue& aDepthOffset);

    /**
     * Returns the owner of the visual.
     */
    IMPORT_C CAlfControl& Owner() const;

    /**
     * Sets the owner of the visual.
     *
     * @param aOwner  New owner of the visual.
     */
    IMPORT_C void SetOwner(CAlfControl& aOwner);

    /**
     * Sets the origin of the visual. The origin defines the location of the
     * visual's position in relation to its DisplayRect. By default the visual's
     * position defines its top left corner.
     *
     * Calling this method is equivalent to setting/clearing the corresponding
     * flags.
     */
    IMPORT_C void SetOrigin(TAlfVisualHOrigin aHoriz, TAlfVisualVOrigin aVert);

    /**
     * Determines the horizontal origin.
     */
    IMPORT_C TAlfVisualHOrigin HorizontalOrigin() const;

    /**
     * Determines the vertical origin.
     */
    IMPORT_C TAlfVisualVOrigin VerticalOrigin() const;

    /**
     * Aligns the rectangle using the visual's origin position.
     *
     * @param aRect  A rectangle (origin at top left). Modified to suit the
     *               visual's origin.
     */
    IMPORT_C void AlignByOrigin(TAlfRealRect& aRect) const;

    /**
     * @deprecated  Use base unit version instead
     *
     * Sets the minimum size of the visual.
     *
     * @param aMinSize  Minimum size of the visual.
     */
    IMPORT_C void SetMinSize(const TSize& aMinSize);

    /**
     * Sets the minimum size of the visual.
     *
     * @param aMinSize  Minimum size of the visual in base units.
     */
    IMPORT_C void SetMinSize(const TAlfRealSize& aMinSize);

    /**
     * @deprecated  Use base unit version instead
     *
     * Returns the minimum size of the visual.
     */
    IMPORT_C TSize MinSize() const;

    /**
     * Returns the minimum size of the visual in base units.
     */
    IMPORT_C TAlfRealSize MinSizeInBaseUnits() const;

    /**
     * @deprecated  Use base unit version instead
     *
     * Sets the maximum size of the visual.
     *
     * @param aMaxSize  Maximum size of the visual.
     */
    IMPORT_C void SetMaxSize(const TSize& aMaxSize);

    /**
     * Sets the maximum size of the visual.
     *
     * @param aMaxSize  Maximum size of the visual.
     */
    IMPORT_C void SetMaxSize(const TAlfRealSize& aMaxSize);

    /**
     * @deprecated  Use base unit version instead
     *
     * Returns the maximum size of the visual.
     */
    IMPORT_C TSize MaxSize() const;

    /**
     * Returns the maximum size of the visual in base units.
     */
    IMPORT_C TAlfRealSize MaxSizeInBaseUnits() const;

    /**
     * Sets the position and size of the visual.
     *
     * @param aRect            Target rectangle.
     * @param aTransitionTime  Transition timein milliseconds.
     */
    IMPORT_C void SetRect(const TAlfRealRect& aRect, TInt aTransitionTime = 0);

    /**
     * @deprecated  Use base unit version instead
     *
     * Moves the visual's position.
     *
     * @param aOffset          Offset from the current target.
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C void Move(const TPoint& aOffset, TInt aTransitionTime = 0);

    /**
     * Moves the visual's position.
     *
     * @param aOffset          Offset from the current target in base units.
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C void Move(const TAlfRealPoint& aOffset, TInt aTransitionTime = 0);

    /**
     * Positions and sizes this visual to match another.
     *
     * @param aVisual  Visual whose position and size to copy.
     */
    IMPORT_C void Mimic(CAlfVisual& aVisual, TInt aTransitionTime = 0);

    /**
     * @deprecated  Use base unit version instead
     *
     * Sets the position and size of the visual.
     *
     * @param aCenterPoint     New center point of the visual.
     * @param aSize            New size of the visual.
     * @param aTransitionTime  Number of milliseconds for transitioning to new values.
     */
    IMPORT_C void SetCenteredPosAndSize(const TPoint& aCenterPoint,
                                        const TSize& aSize,
                                        TInt aTransitionTime = 0);

    /**
     * Sets the position and size of the visual.
     *
     * @param aCenterPoint     New center point of the visual in base units.
     * @param aSize            New size of the visual in base units.
     * @param aTransitionTime  Number of milliseconds for transitioning to new values.
     */
    IMPORT_C void SetCenteredPosAndSize(const TAlfRealPoint& aCenterPoint,
                                        const TAlfRealSize& aSize,
                                        TInt aTransitionTime = 0);

    /**
     * Calculates the visual's bounding rectangle in display coordinates.
     * Display coordinates are defined relative to the display's visible area.
     * Uses the current values of the position and size. Only calculated once 
     * per frame.
     */
    IMPORT_C TAlfRealRect DisplayRect() const;

    /**
     * Returns the opacity of the visual, taking into account the opacity
     * of parent visuals.
     *
     * @return  Effective opacity level of the visual.
     */
    IMPORT_C TReal32 EffectiveOpacity() const __SOFTFP;

    /**
     * Sets the clipping mode of the layout.
     *
     * @param aClipping  <code>ETrue</code> to hide children that are outside
     *                   the layout's real size.
     */
    IMPORT_C void SetClipping(TBool aClipping = ETrue);

    /**
     * Determines whether the visual is clipping its contents.
     */
    IMPORT_C TBool Clipping() const;

    /**
     * Returns the padding pixels. Visuals can use this to determine the
     * amount of padding that should be applied to the content.
     *
     * @deprecated this will return correct pixel values only if paddings were set 
     *      using deprecated pixel based APIs, so migrate to using metrics instead
     *
     * @return  Horizontal and vertical padding, in pixels.
     */
    IMPORT_C TPoint Padding() const;

    /**
     * Returns the padding of the visual.
     *
     * @return  Padding as a box metric.
     */
    IMPORT_C const TAlfBoxMetric& PaddingAsMetric() const;
    
    /**
     * Returns the horizontal padding pixels. Visuals can use this to
     * determine the amount of padding that should be applied to the content.
     *
     * @deprecated this will return correct pixel values only if paddings were set 
     *      using deprecated pixel based APIs, so migrate to using metrics instead
     *
     * @return  Horizontal padding, in pixels.
     */
    IMPORT_C TInt HorizontalPadding() const;

    /**
     * Returns the vertical padding pixels. Visuals can use this to
     * determine the amount of padding that should be applied to the content.
     *
     * @deprecated this will return correct pixel values only if paddings were set 
     *      using deprecated pixel based APIs, so migrate to using metrics instead
     *
     * @return  Vertical padding, in pixels.
     */
    IMPORT_C TInt VerticalPadding() const;

    /**
     * Sets the position of the visual.
     */
    IMPORT_C void SetPos(const TAlfTimedPoint& aPos);

    /**
     * Sets the size of the visual.
     */
    IMPORT_C void SetSize(const TAlfTimedPoint& aSize);

    /**
    * Copies selected, basic types of properties from another visual
    * If another visual is in the middle of interpolating selected value(s), target value is used
    * with given transition time.
    *
    * For dynamic properties, use CAlfPropertyOwner::CopyProperty()
    * 
    * @param aSourceVisual   Visual, from which the values are copied
    * @param aBitmask        Defines what properties to copy
    * @param aTransitionTime Number of milliseconds for transitioning to new values.     
    * 
    * @return System wide error codes
    */
    IMPORT_C TInt CopyValues(const CAlfVisual& aSourceVisual, TInt aBitmask = AlfVisualProperties::EAll, TInt aTransitionTime = 0);

    /**
    * Pauses selected, basic types of timed properties of this visual
    *
    * @param aPropertiesBitmask   defines what properties are paused,  combination of AlfVisualProperties
    */
    IMPORT_C void Pause(TInt aPropertiesBitmask = AlfVisualProperties::EAll);

    /**
    * Resumes selected basic types of timed properties of this visual
    * that were paused using CAlfVisual::Pause().
    * If a property was not paused before calling Resume, call does not have effect
    *
    * @param aPropertiesBitmask   defines what properties are paused, combination of AlfVisualProperties
    */
    IMPORT_C void Resume(TInt aPropertiesBitmask = AlfVisualProperties::EAll);

    // internal
    TBool TagMatches(const TDesC8& aTag) const;
    
    /** ! Deletes the visual */
    IMPORT_C virtual void DoRemoveAndDestroyAllD();
    
    
    /**
     * Sets an automatic tactile feedback. If this visual is a layout,
     * the feedback affects to the child visuals as well. This can 
     * be overridden in child visuals.
     * 
     * @param aEventType Event which triggers the automatic feedback.
     * @param aFeedbackType Feedback type.
     *
     * @see TTouchEventType from touchlogicalfeedback.h
     * @see TTouchLogicalFeedback from touchlogicalfeedback.h
     */
    IMPORT_C void SetTactileFeedbackL( TInt aEventType, TInt aFeedbackType );
        
    /**
     * Checks whether a tactile feedback has been assigned.
     * Eventhough, this returns EFalse the layout may have feedback defined.
     * @param aEventType Event type which needs to match
     *        If KAlfTactileFeedbackEventTypeNone, the event type is irrelevant.
     * 
     * @return ETrue if the visual has a tactile feedback defined.
     */
    IMPORT_C TBool HasTactileFeedback( 
        TInt aEventType = KAlfTactileFeedbackEventTypeNone ) const;
       
    /**
     * Removes tactile feedback for the visual.
     * 
     * @param aEventType Event type which feedback to remove.
     *        If KAlfTactileFeedbackEventTypeNone, all the feedbacks are removed.
     */
    IMPORT_C void RemoveTactileFeedback( TInt aEventType = KAlfTactileFeedbackEventTypeNone );
    
    /**
     * Enables the drop shadow. Currently implemented for the image and text visuals.
     * 
     * @param aEnable ETrue to enable, EFalse to disable.
     */
    IMPORT_C void EnableDropShadowL( TBool aEnable = ETrue );
    
    /**
     * Returns the drop shadow parameter handler.
     *
     * @return Drop shadow handler. NULL if drop shadow is disabled.
     */
    IMPORT_C CAlfDropShadow* DropShadowHandler() const;
    IMPORT_C void CAlfVisual::SetEffectL( const TDesC& aEffectFile );
    IMPORT_C void CAlfVisual::SetGroupEffectL( const TDesC& aEffectFile, TInt aGroupHandle );

protected:

    /** ! future proofing */
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private:

    // Private data. Own. 
    struct TPrivateData;
    TPrivateData* iData;

    };


#endif // C_ALFVISUAL_H
