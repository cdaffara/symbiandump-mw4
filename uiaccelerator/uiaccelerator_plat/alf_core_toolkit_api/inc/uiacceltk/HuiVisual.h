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
* Description:   Defines CHuiVisual. Visuals are graphical entities visible on a display
*
*/



#ifndef __HUIVISUAL_H__
#define __HUIVISUAL_H__


#include <e32base.h>
#include <e32std.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/huimetric.h>
#include <uiacceltk/HuiRealRect.h>
#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiRealSize.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiBrushArray.h>
#include <uiacceltk/HuiSessionObject.h>


/* Forward declarations. */
class CHuiEnv;
class CHuiControlGroup;
class CHuiSkin;
class CHuiControl;
class MHuiEventHandler;
class CHuiDisplay;
class CHuiLayout;
class CHuiGc;
class CHuiVisual;
class CHuiTransformation;
class CHuiDropShadow;
class CHuiFxEffect;
class CHuiFxEffectParser;
class MHuiEffectable;
class CHuiCanvasRenderBuffer;
/** Visual types. */
enum THuiVisualType
    {
    EHuiVisualTypeVisual,
    EHuiVisualTypeLine,
    EHuiVisualTypeText,
    EHuiVisualTypeImage,
    EHuiVisualTypeMesh,
    EHuiVisualTypeCanvas
    };


/**
 * Visual flags.
 *
 * @see CHuiVisual::SetFlag()
 * @see CHuiVisual::SetFlags()
 * @see CHuiVisual::Flags() 
 * @see CHuiVisual::ClearFlag() 
 * @see CHuiVisual::ClearFlags()  
 */
enum THuiVisualFlags
    {
    /** Manual layout means that layouts can't change the position and
        size of the visual; the owner of the visual is expected to
        manually maintain the visual's placement. */
    EHuiVisualFlagManualSize = 0x1,
    EHuiVisualFlagManualPosition = 0x2,
    EHuiVisualFlagManualLayout = EHuiVisualFlagManualSize |
                                 EHuiVisualFlagManualPosition,

    EHuiVisualFlagBlendAdd = 0x4,

    /** Notify owner control when the visual's layout has been updated.
        Notication is sent after all children have been updated. */
    EHuiVisualFlagLayoutUpdateNotification = 0x8,

    /** When the visual is drawn it will use a 3D projection. */
    EHuiVisualFlag3DProjection = 0x10,

    /** No depth test when drawing with a 3D projection. */
    EHuiVisualFlagNoDepthOcclusion = 0x20,

    /** Use a local horizon with a 3D projection. Otherwise, the global
        display horizon is used instead. */
    EHuiVisualFlagLocalHorizon = 0x40,

    /** Contents of the visual are clipped to its display rectangle. */
    EHuiVisualFlagClipping = 0x80,

    /** Visual is using a horizontal center origin, i.e., the X coordinate
        of the visual's position defines its horizontal center point. */
    EHuiVisualFlagHOriginCenter = 0x100,

    /** Visual is using a right edge origin, i.e., the X coordinate
        of the visual's position defines its right edge. */
    EHuiVisualFlagHOriginRight = 0x200,

    /** Visual is using a vertical center origin, i.e., the Y coordinate
        of the visual's position defines its vertical center point. */
    EHuiVisualFlagVOriginCenter = 0x400,

    /** Visual is using a bottom edge origin, i.e., the Y coordinate
        of the visual's position defines its bottom edge. */
    EHuiVisualFlagVOriginBottom = 0x800,

    /** Visual has changed and needs refresh. */
    EHuiVisualChanged = 0x1000,

    /** When the visual is drawn, the depth buffer shouldn't be updated. */
    EHuiVisualFlagNoDepthUpdate = 0x2000,

    /** Use the visual's local layout transition time, do not inherit from parent. */
    EHuiVisualFlagManualTransitionTime = 0x4000,

    /** Draw above the normal drawing order. */
    EHuiVisualFlagDrawAfterOthers = 0x10000,
    
    /** Do not automatically update the layout of children when doing 
        changes in a layout (e.g., when reordering children). */
    EHuiVisualFlagFreezeLayout = 0x20000,

    /** The visual will be ignored when checking for possible pointer event
        interaction. */
    EHuiVisualFlagIgnorePointer = 0x40000,

    /** 
     * automatically reverse the base unit sign in the X axis if the current locale setting is 
     * for mirrored layout
     *
     * @note this will only affect the positions of visuals contained
     * within layout visuals, the contents within a visual such as a text visual will not be 
     * affected. 
     */
    EHuiVisualFlagAutomaticLocaleMirroringEnabled = 0x80000,
    
    /** 
     * RnD: if set, some of the debug features are enabled. See programmers'
     * guide for more info.
     *
     * If the central repository has the setting to draw selected visual outline, 
     * setting this flag will draw outline for the visual and its child visuals
     */    
    EHuiVisualFlagEnableDebugMode  = 0x100000,

    /** 
     * Setting this flag makes visual and its children Inactive i.e. it is ignored from roster operations 
     * - Draw, Handle event, change notifications etc.
     * Unsetting flag can again make visual tree active.
     * Note that flag change does not take effect unless that display area is refreshed.
     **/
    EHuiVisualFlagInactive = 0x200000,

    /** 
     * Setting this flag informs renderer that visual is to be considered as opaque and thus
     * renderer may choose not to draw visuals behind opaque visual. Note that visual
     * attribute "opacity" is not affected by this flag.
     **/
    EHuiVisualFlagOpaqueHint = 0x400000,
    
    /** 
     * \internal
     * Setting this flag means that visual is not drawn to the normal place in visual tree.
     * If some canvas visual has set this visual as an external content visual, it is drawn there.
     * This is used only for canvas visuals and this is for internal use only.
     **/    
    EHuiVisualFlagDrawOnlyAsExternalContent = 0x800000,
    
    EHuiVisualFlagShouldBeHidden = 0x1000000,
    
    EHuiVisualFlagShouldDestroy = 0x2000000,
            
    EHuiVisualFlagShouldBeInactive = 0x4000000,
    
	EHuiVisualFlagAlwaysDraw = 0x8000000,

    /**
     * This indicates that this visual is completely under another opaque visual.
     * So this is a hint, set by client, that the toolkit can use to optimize drawing.
     */
    EHuiVisualFlagUnderOpaqueHint = 0x10000000,

    EHuiVisualFlagShouldBeUnderOpaqueHint = 0x20000000,
    
    EHuiVisualFlagShouldBeShown = 0x40000000,
    // visual that is used only by wserv
    EHuiVisualFlagWserv = 0x80000000,
    EHuiVisualFlagLast = 0xFFFFFFFF
    };


/** Horizontal origin of a visual. */
enum THuiVisualHOrigin
    {
    EHuiVisualHOriginLeft = 0,
    EHuiVisualHOriginCenter = 1,
    EHuiVisualHOriginRight = 2
    };


/** Vertical origin of a visual. */
enum THuiVisualVOrigin
    {
    EHuiVisualVOriginTop = 0,
    EHuiVisualVOriginCenter = 1,
    EHuiVisualVOriginBottom = 2
    };

enum THuiReferenceState
 	{
  	EHuiReferenceStateNow = 0,
  	EHuiReferenceStateTarget = 1
  	};
  	
const TInt KHuiTactileFeedbackEventTypeNone = -1;

/**
 * Interface for visual owners. A class that owns a visual must implement 
 * this interface. Visual owners are expected to destroy their visuals when
 * the owner is destroyed.
 */
class MHuiVisualOwner
    {
public:

    IMPORT_C virtual ~MHuiVisualOwner();

    /**
     * Returns the environment where this visual belongs into.
     *
     * @return  Environment.
     */
    virtual CHuiEnv& Env() const = 0;

    /**
     * Determines the control group in which the owner belong to. If the owner
     * does not belong to a control group, <code>NULL</code> is returned.
     *
     * @return Control group or <code>NULL</code>.
     */
    virtual CHuiControlGroup* ControlGroup() const = 0;
    
    /**
     * Returns the event handler resposible for handling events related to 
     * the visual, for example when the visual receives a pointer down event.
     */
    virtual MHuiEventHandler* EventHandler() = 0;

	/**
	 * Appends a visual to the visual owner. 
	 *
	 * @param aVisual  Visual to append. The owner receives ownership
	 * of the visual.
	 *
     * @return Reference to appended visual.
	 */
    virtual void AppendL(CHuiVisual* aVisual) = 0;

	/**
	 * Removes a visual from the owner. 
	 *
	 * @param aVisual  Visual to remove. 
	 *
	 * @return Pointer to removed visual. Caller gets ownership.
	 */
	virtual void Remove(CHuiVisual* aVisual) = 0;

    /**
     * Removes a visual from the ownership of the owner. This is called when
     * the visual is being destroyed.
     *
     * @param aVisual  Visual to remove.
     */
    virtual void VisualDestroyed(CHuiVisual& aVisual) = 0;

    /**
     * Notifies the owner that the layout of a visual has been recalculated.
     * Called only when the EHuiVisualFlagLayoutUpdateNotification flag has
     * been set for the visual.
     *
     * @param aVisual  Visual that has been laid out.
     */
    virtual void VisualLayoutUpdated(CHuiVisual& aVisual) = 0;
   
	/**
	 * Notifies the visual owner if there was an error (a leave happened)
	 * when preparing the visual for drawing. Typically the resource alloc
	 * errors such as out of memory is escalated through this callback.
	 * 
     * @param aVisual    Visual that caused the error.
     * @param aErrorCode Symbian KErr error code for the failure.
	 */
    virtual void VisualPrepareDrawFailed(CHuiVisual& aVisual, TInt aErrorCode) = 0;

    };


/**
 * Visuals are graphical entities visible on a display. They are responsible
 * for maintaining their state independently.
 *
 * Visuals use a hierarchical coordinate system. All coordinate values are
 * specified in the base unit of the parent layout, relative to the parent 
 * visual's upper left corner. This is called the local coordinate system of
 * the visual. Visuals that have no parent are relative to the upper left 
 * corner of the HuiDisplay.
 *
 * In practice, this means that when a layout is moved 10 pixels to the right,
 * all of its children are automatically also moved 10 pixels to the right
 * on the screen.
 *
 * Visuals can be tagged with human-readable descriptors. @see SetTagL()
 *
 * Note that all layouts are derived from CHuiVisual. Layout visuals are
 * used for building visual trees. @see CHuiLayout
 */
class CHuiVisual : public CBase, public MHuiBrushGuide, public MHuiSessionObject
    {
public:

    /* Constructors and destructor. */

    /** @beginAPI */

    /**
     * Constructs and appends a new visual to the owner control.
     *
     * @param aOwnerControl  Control that will own the new visual.
     *
     * @return  The new visual.
     */
    IMPORT_C static CHuiVisual* AddNewL(CHuiControl& aOwnerControl,
                                        CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     *
     * @param aEnv    The environment where this visuals belongs to.
     * @param aOwner  Owner control.
     */
    IMPORT_C CHuiVisual(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor, must be called in deriving class.
     */
    IMPORT_C virtual void ConstructL();

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiVisual();


    /* New Methods. */

    /**
     * Destroys a hierarchy of visuals, along with this visual. The visuals
     * are first removed from their owner.
     */
    IMPORT_C virtual void RemoveAndDestroyAllD();

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
     * @see CHuiVisual::Tag()
     * @see CHuiVisual::IsTagged()
     * @see CHuiVisual::FindTag()
     */
    IMPORT_C void SetTagL(const TDesC8& aTag);

    /**
     * Determines the tag of the visual.
     *
     * @return  Tag descriptor of the visual.
     *
     * @see CHuiVisual::SetTagL()
     * @see CHuiVisual::IsTagged()
     * @see CHuiVisual::FindTag()
     */
    IMPORT_C const TDesC8& Tag() const;

    /**
     * Determines if the visual has a tag.
     *
     * @return  <code>ETrue</code>, if the visual has been tagged with SetTagL().
     *          <code>EFalse</code> otherwise.
     *
     * @see CHuiVisual::Tag()
     * @see CHuiVisual::SetTagL()
     * @see CHuiVisual::FindTag()
     */
    IMPORT_C TBool IsTagged() const;

    /**
     * Finds a visual whose tag descriptor contains a specific tag.
     *
     * CHuiVisual::FindTag() is the basic implementation that checks if
     * this visual's tag matches the specified tag. CHuiLayout::FindTag()
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
     * @see CHuiVisual::Tag()
     * @see CHuiVisual::SetTagL()
     * @see CHuiVisual::IsTagged()
     * @see CHuiLayout::FindTag()
     */
    IMPORT_C virtual CHuiVisual* FindTag(const TDesC8& aTag);

    /**
     * Set a flag that affects the visual's behaviour.
     *
     * @param aFlag  Flag to set.
     */
    IMPORT_C void SetFlag(THuiVisualFlags aFlag);

    /**
     * Sets one or more flags that affect the visual's behaviour.
     *
     * @param aAnyFlags  One or more flags to set.
     */
    IMPORT_C void SetFlags(TInt aAnyFlags);

    /**
     * Unset flags that affect the visual's behaviour.
     *
     * @param aFlag  Flag to clear.
     */
    IMPORT_C void ClearFlag(THuiVisualFlags aFlag);

    /**
     * Clear one or more flags that affect the visual's behaviour.
     *
     * @param aAnyFlags  One or more flags to clear.
     */
    IMPORT_C void ClearFlags(TInt aAnyFlags);

    /**
     * Returns the flags of the visual.
     */
    inline TInt Flags() const;

    /**
     * Returns the owner of the visual.
     */
    IMPORT_C MHuiVisualOwner& Owner() const;

    /**
     * Sets the owner of the visual.
     *
     * @param aOwner  New owner of the visual.
     */
    IMPORT_C void SetOwner(MHuiVisualOwner& aOwner);
    
    /**
     * Returns the number of child visuals.
     */
    IMPORT_C virtual TInt Count() const;

    /**
     * Returns a child visual.
     *
     * @param aIndex  Index of the child visual.
     *
     * @return  Reference to a child visual.
     */
    IMPORT_C virtual CHuiVisual& Visual(TInt aIndex) const;

    /**
     * Returns the layout this visual belongs to.
     */
    IMPORT_C CHuiLayout* Layout() const;

    /**
     * Sets the layout this visual belongs to.
     */
    void SetLayout(CHuiLayout* aLayout);

    /**
     * Returns the environment of the visual.
     */
    IMPORT_C CHuiEnv& Env() const;

    /**
     * Returns the skin of the owner control's environment.
     */
    IMPORT_C CHuiSkin& Skin() const;

    /**
     * Returns the display of the visual (if shown).
     *
     * @return  Pointer to <code>CHuiDisplay</code> instance, or
     * <code>NULL</code> if no display is associated with the visual.
     */
    IMPORT_C CHuiDisplay* Display() const;

    /**
     * Sets the display on which the visual is being shown.
     */
    void SetDisplay(CHuiDisplay* aDisplay);

    /**
     * Sets the origin of the visual. The origin defines the location of the
     * visual's position in relation to its DisplayRect. By default the visual's
     * position defines its top left corner.
     *
     * Calling this method is equivalent to setting/clearing the corresponding
     * flags.
     */
    IMPORT_C void SetOrigin(THuiVisualHOrigin aHoriz, THuiVisualVOrigin aVert);

    /**
     * Determines the horizontal origin.
     */
    IMPORT_C THuiVisualHOrigin HorizontalOrigin() const;

    /**
     * Determines the vertical origin.
     */
    IMPORT_C THuiVisualVOrigin VerticalOrigin() const;
    
    /**
     * Moves a point according to the visual's origin position. If this is called
     * on both the iTl and iBr corners of a rectangle, the result is that the
     * rectangle is aligned by the origin. This method is used during the calculations 
     * to determine a visual's display rectangle.
     *
     * @param aPoint  Point to move.
     * @param aUndo   Which way to offset the point (apply or undo the alignment).
     */
    IMPORT_C void AlignByOrigin(THuiRealPoint& aPoint, TBool aUndo = EFalse) const;

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
    IMPORT_C void SetMinSize(const THuiRealSize& aMinSize);

    /**
     * @deprecated  Use base unit version instead
     *
     * Returns the minimum size of the visual.
     */
    IMPORT_C TSize MinSize() const;

    /**
     * Returns the minimum size of the visual in base units.
     */
    IMPORT_C THuiRealSize MinSizeInBaseUnits() const __SOFTFP;

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
     * @param aMaxSize  Maximum size of the visual in base units.
     */
    IMPORT_C void SetMaxSize(const THuiRealSize& aMaxSize);

    /**
     * @deprecated  Use base unit version instead
     *    
     * Returns the maximum size of the visual.
     */
    IMPORT_C TSize MaxSize() const;

    /**
     * Returns the maximum size of the visual in base units.
     */
    IMPORT_C THuiRealSize MaxSizeInBaseUnits() const __SOFTFP;

    /**
     * Returns the position of the visual. This is set by UpdateLayout().
     */
    IMPORT_C const THuiTimedPoint& Pos() const;

    /**
     * Returns the size of the visual. This is set by UpdateLayout().
     */
    IMPORT_C const THuiTimedPoint& Size() const;

    inline THuiTimedPoint& Size();
 
    /**
     * Sets the position of the visual.
     * 
     * This method should be used only when doing manual layouting. 
     * If the visuals are under a layout that defines positions for its children (such
     * as CHuiGridLayout), then the layout class's facilities should be used instead.
     * 
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using the SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to assign the position yourself.
     * 
     * @param aPos             Target position, in the local coordinate system as
     *                         defined by the parent layout (or the display, 
     *                         if there is no parent).
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C virtual void SetPos(const THuiRealPoint& aPos, TInt aTransitionTime = 0);

    /**
     * Sets the size of the visual.
     * 
     * This method should be used only when doing manual layouting. 
     * If the visuals are under a layout that defines sizes for its children (such
     * as CHuiGridLayout), then the layout class's facilities should be used instead.
     *
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to assign the size yourself.
     * 
     * @param aSize            New size for this visual, in the local coordinate 
     *                         system as defined by the parent layout (or the display, 
     *                         if there is no parent).
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C virtual void SetSize(const THuiRealSize& aSize, TInt aTransitionTime = 0);

    /**
     * Sets the position and size of the visual.
     *
     * This method should be used only when doing manual layouting. 
     * If the visuals are under a layout that defines positions and sizes for 
     * its children (such as CHuiGridLayout), then the layout class's facilities 
     * should be used instead.
     *
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to assign the rectangle yourself.
     * 
     * @param aRect            Target rectangle, in the local coordinate 
     *                         system as defined by the parent layout (or the display, 
     *                         if there is no parent).
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C void SetRect(const THuiRealRect& aRect, TInt aTransitionTime = 0);

    /**
     * @deprecated  Use base unit version instead
     *    
     * Moves the visual's position.
     * 
     * Move() should be used only when doing manual layouting (same goes for 
     * SetPos() and SetSize()). 
     * If the visuals are under a layout that defines positions for its children (such
     * as CHuiGridLayout), then the layout class's facilities should be used instead.
     * 
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to move this visual.
     *
     * @param aOffset          Offset from the current target.
     * @param aTransitionTime  Transition time in milliseconds.
     */
    IMPORT_C void Move(const TPoint& aOffset, TInt aTransitionTime = 0);

    /**
     * Moves the visual's position.
     * 
     * Move() should be used only when doing manual layouting (same goes for 
     * SetPos() and SetSize()). 
     * If the visuals are under a layout that defines positions for its children (such
     * as CHuiGridLayout), then the layout class's facilities should be used instead.
     * 
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to move this visual.
     *
     * @param aOffset          Offset from the current target.
     * @param aTransitionTime  Transition time in milliseconds.     
     */
    IMPORT_C void Move(const THuiRealPoint& aOffset, TInt aTransitionTime = 0);

    /**
     * Positions and sizes this visual to match another.
     * 
     * This method should be used only when doing manual layouting. If the visuals are under 
     * a layout, then the layout class's facilities should be used instead.
     *
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to mimic other visuals.
     * 
     * @param aVisual  Visual whose position and size to copy.
     */
    IMPORT_C void Mimic(const CHuiVisual& aVisual, TInt aTransitionTime = 0);

    /**
     * @deprecated  Use base unit version instead
     *        
     * Sets the position and size of the visual.
     * 
     * This method should be used only when doing manual layouting. If the visuals are under 
     * a layout, then the layout class's facilities should be used instead.
     *
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to assign the position and size yourself.
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
     * This method should be used only when doing manual layouting. If the visuals are under 
     * a layout, then the layout class's facilities should be used instead.
     *
     * If you want to move or otherwise layout the visual manually, you should 
     * set the EHuiVisualFlagManualLayout flag using SetFlag() method.
     * 
     * @note Even if the visual has no parent -- the roster will automatically 
     * lay it out to cover the entire display, so the EHuiVisualFlagManualLayout
     * will be needed if you want to assign the position and size yourself.
     *
     * @param aCenterPoint     New center point of the visual in metric units.
     * @param aSize            New size of the visual in metric units.
     * @param aTransitionTime  Number of milliseconds for transitioning to new values.     
     */
    IMPORT_C void SetCenteredPosAndSize(const THuiRealPoint& aCenterPoint,
                                        const THuiRealSize& aSize,
                                        TInt aTransitionTime = 0);

    /**
     * Update the layout of all children. Subclasses can call this to
     * recalculate the layout of all children.
     *
     * @param aTransitionTime  Time for layout transition.
     *      CHuiStatic::LayoutTransitionTime() should be used
     *      as the default value, if an immediate transition is not required.
     *
     * @see CHuiStatic::LayoutTransitionTime()
     */
    IMPORT_C virtual void UpdateChildrenLayout(TInt aTransitionTime = 0);

    /**
     * For convenience. Updates the layout of the siblings of this visual. In other words,
     * asks the parent layout to do a children layout.
     *
     * @param aTransitionTime  Time for layout transition.
     */
    IMPORT_C void UpdateSiblingLayout(TInt aTransitionTime = 0);

    /**
     * Calculates the visual's bounding rectangle in display coordinates.
     * Display coordinates are defined relative to the display's visible area.
     * Uses the current values of the position and size. Only calculated once 
     * per frame. The rect is always in pixels.
     */
    IMPORT_C THuiRealRect DisplayRect() const __SOFTFP;

    /**
     * Calculates the visual's bounding rectangle animation target 
     * in display coordinates. Display coordinates are defined 
     * relative to the display's visible area. Uses the target 
     * values of the rectangle position and size, which means 
     * transitions are always immediate in the returned rectangle. 
     * Only calculated once per frame. The rect is always in pixels.
     */
    IMPORT_C THuiRealRect DisplayRectTarget() const __SOFTFP;
    
    /**
     * Determines the base unit of the local coordinate system, in which
     * this visual's position and size are stored. The position and size
     * of the visual are multiplied by this metric to get the actual
     * position and size.
     *
     * @return  Base unit of the local coordinate system, as defined by the
     *          parent layout, or the display if there is no parent.
     */
    IMPORT_C THuiXYMetric LocalBaseUnit() const;

    /**
     * ! deprecated, use method with THuiReferenceState instead
     */
    IMPORT_C THuiRealPoint LocalPointInPixels(const THuiRealPoint& aLocalPoint) const __SOFTFP;

    /**
     * Converts a point in the local coordinate system to pixels in the 
     * local coordinate system. The local coordinate system is relative to 
     * the top left corner of the parent. This method only
     * does a unit conversion from the base units to pixels.
     *
     * @param aLocalPoint  Point to convert, which is using the local
     *                     base unit.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     *
     * @return  @c aLocalPoint converted to pixels in the local coordinate system.
     */
    IMPORT_C THuiRealPoint LocalPointInPixels(const THuiRealPoint& aLocalPoint, 
                                              THuiReferenceState aReferenceState) const __SOFTFP;
    

    /**
     * ! deprecated, use method with THuiReferenceState instead
     */
    IMPORT_C THuiRealPoint PixelPointInLocal(const THuiRealPoint& aPixelPoint) const __SOFTFP;

    /**
     * Converts a pixel point in the local coordinate system to a point that
     * uses the base unit of the local coordinate system. 
     * The local coordinate system is relative to the top left corner 
     * of the parent. This method only does a unit conversion from pixels to
     * the base units.
     *
     * @param aPixelPoint  Point to convert, which is in pixels.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     *
     * @return @c aPixelPoint converts to the local base unit.
     */
    IMPORT_C THuiRealPoint PixelPointInLocal(const THuiRealPoint& aPixelPoint,
                                             THuiReferenceState aReferenceState) const __SOFTFP;


    /**
     * Converts a pixel point to a specific unit.
     *
     * @param aPixelPoint  Point to convert, in pixels.
     * @param aUnit        Unit to which the point is to be converted.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     *
     * @return  The point as a multiple of @c aUnit.
     */
    THuiRealPoint PixelPointInUnits(const THuiRealPoint& aPixelPoint, 
        const THuiXYMetric& aUnit, THuiReferenceState aReferenceState) const __SOFTFP;


    /**
     * ! deprecated, use method with THuiReferenceState instead
     */
    IMPORT_C THuiRealPoint MetricToPixels(const THuiXYMetric& aMetric) const __SOFTFP;

    /**
     * Converts an XY metric to a pixel point.
     *
     * @param aMetric  Metric to convert.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     *
     * @return  The metric in pixels.
     */
    IMPORT_C THuiRealPoint MetricToPixels(const THuiXYMetric& aMetric,
                                          THuiReferenceState aReferenceState) const __SOFTFP;

    /**
     * ! deprecated, use method with THuiReferenceState instead
     */
    IMPORT_C void PixelsToMetric(const THuiRealPoint& aPixels, THuiXYMetric& aMetric) const;

    /**
     * Converts a pixel point to an XY metric. The destination unit of the 
     * conversion is taken from the @c aMetric parameter.
     *
     * @param aPixels  Pixel point to convert.
     * @param aMetric  The result is written here. Only the magnitudes of the
     *                 metric are modified. The @c iUnit of the conversion 
     *                 is read from this metric.
     */
    IMPORT_C void PixelsToMetric(const THuiRealPoint& aPixels, THuiXYMetric& aMetric,
                                 THuiReferenceState aReferenceState) const;
    
 
     /**
     * ! deprecated, use method with THuiReferenceState instead
     */
    IMPORT_C THuiRealPoint MetricReferenceInPixels(const THuiXYMetric& aMetric) const __SOFTFP;               
   
    
    /**
     * Calculates the reference size for a metric. The reference size is, for
     * example, the size of the parent layout in the case of the 
     * EHuiUnitNormalized unit.
     * 
     * @param aMetric  Metric whose reference size is to be determined.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     * 
     * @return  Reference size.
     */
    IMPORT_C THuiRealPoint MetricReferenceInPixels(const THuiXYMetric& aMetric,               
                                 THuiReferenceState aReferenceState) const __SOFTFP;

    /**
     * Converts local coordinates to display coordinates. This method is used
     * for calculating the position of the visual on the display when the visual
     * is about to be drawn. Display coordinates are defined relative to the 
     * display's visible area.
     *
     * @param aPoint  Point in the local coordinate system. This might be,
     *                for example, the position or size of this visual.
     *
     * @return  The point in the display's coordinate system, in pixels.
     */
    IMPORT_C TPoint LocalToDisplay(const TPoint& aPoint) const;

    /**
     * Converts local coordinates to display coordinates. This method is used
     * for calculating the position of the visual on the display when the visual
     * is about to be drawn. Display coordinates are defined relative to the 
     * display's visible area.
     *
     * @param aPoint  Point in the local coordinate system. This might be,
     *                for example, the position or size of this visual.
     *
     * @return  The point in the display's coordinate system, in pixels.
     */
    IMPORT_C THuiRealPoint LocalToDisplay(const THuiRealPoint& aPoint) const __SOFTFP;

    /**
     * Converts display coordinates to local coordinates. This method can be
     * used when determining where a particular point on the display is 
     * in the local coordinate system of a layout, for instance when handling
     * pointer input events. Display coordinates are defined relative to 
     * the display's visible area.
     *
     * @param aPoint  Point in the display coordinate system. 
     *
     * @return  The point in this visual's local coordinate system, in the
     *          base units of the parent layout (or display, if there is
     *          no parent layout).
     */
    IMPORT_C TPoint DisplayToLocal(const TPoint& aPoint) const;

    /**
     * Converts display coordinates to local coordinates. This method can be
     * used when determining where a particular point on the display is 
     * in the local coordinate system of a layout, for instance when handling
     * pointer input events. Display coordinates are defined relative to 
     * the display's visible area.
     *
     * @param aPoint  Point in the display coordinate system. 
     *
     * @return  The point in this visual's local coordinate system, in the
     *          base units of the parent layout (or display, if there is
     *          no parent layout).
     */
    IMPORT_C THuiRealPoint DisplayToLocal(const THuiRealPoint& aPoint) const __SOFTFP;


    IMPORT_C void BoxMetricToPixelRect(const THuiBoxMetric& aBox, 
        THuiRealRect& aRectInPixels) const;

    /**
     * Converts a box metric to a pixel rectangle. This method is used for
     * converting visual padding boxes to pixels.
     *
     * @param aBox  Box metric to be interpreted in the context of this visual.
     * @param aRectInPixels  Resulting rectangle in pixels. Note that the width
     *                       and height of this box are meaningless, because 
     *                       each of the four sides is defined independently of
     *                       each other.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     */
    IMPORT_C void BoxMetricToPixelRect(const THuiBoxMetric& aBox, 
        THuiRealRect& aRectInPixels, THuiReferenceState aReferenceState) const;

    /**
     * Converts a box metric to a rectangle in local coordinates. This method
     * is used for converting visual padding boxes to local coordinate units
     * when doing layout.
     *
     * @param aBox  Box metric to be interpreted in the context of this visual.
     * @param aLocalRect  Top, left, right, and bottom values of the box in 
     *                    the visual's local coordinate system.
     *                    Note that the width and height of this box are meaningless, 
     *                    because each of the four sides is defined independently of
     *                    each other.
     */
    /*IMPORT_C void BoxMetricToLocalRect(const THuiBoxMetric& aBox,
        THuiRealRect& aLocalRect) const;*/

    /**
     * Moves the visual to the front inside its parent layout. 
     * In other words, the visual becames the last child of the parent.
     * Causes layout recalculation.
     */
    IMPORT_C void MoveToFront(TInt aTransitionTime = 0);

    /**
     * Returns the position of the visual.
     */
    inline THuiTimedPoint& Pos();

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
     * Enables or disables transformation of the visual.
     */
    IMPORT_C void EnableTransformationL(TBool aIsTransformed = ETrue);

    /**
     * Returns the transformation of the visual.
     *
     * @panic  THuiPanic::EVisualTransformNotEnabled
     */
    IMPORT_C CHuiTransformation& Transformation();

    /**
     * Sets the number of pixels that the visual uses for padding its contents.
     * Sets both the horizontal and vertical padding.
     *
     * @param aPadding  Number of padding pixels.
     */
    IMPORT_C void SetPadding(TInt aPadding);

    /**
     * Sets the number of pixels that the visual uses for padding its contents.
     * Sets the horizontal and vertical padding separately.
     *
     * @param aPadding  Number of padding pixels.
     */
    IMPORT_C void SetPadding(const TPoint& aPadding);

    /**
     * Sets the amount of padding that the visual uses for padding its contents.
     * The box metric can have a different amount on all sides.
     *
     * @note as paddings are relative to the visual, if relative units are required for a layout, 
     * it is advisable to use EHuiUnitRelativeToMySize so that the paddings are relative
     * to the size of this layout visual
     *
     * @param aPadding  Amount of padding.
     */
    IMPORT_C void SetPadding(const THuiBoxMetric& aPadding);

    /**
     * Returns the padding of the visual.
     *
     * @return  Padding as a box metric.
     */
    IMPORT_C const THuiBoxMetric& Padding() const;

     /**
     * ! deprecated, use method with THuiReferenceState instead
     */
    IMPORT_C THuiRealRect PaddingInPixels() const __SOFTFP;

    /**
     * Returns the padding of the visual, in pixels.
     *
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     * @return  Padding, in pixels. iTl.iX is the left padding, iTl.iY is 
     *          the top padding, iBr.iX is the right padding, and iBr.iY
     *          is the bottom padding. The width and height of the returned 
     *          rectangle are meaningless.
     */
    IMPORT_C THuiRealRect PaddingInPixels(THuiReferenceState aReferenceState) const __SOFTFP;

	/**
	 * PrepareDrawL prepares the visual for drawing (screen refresh).
	 * Can be used to update the visual or execute any Leaving 
	 * operations such as resource allocation needed for the 
	 * actual drawing.
	 * 
	 * @return ETrue if the prepare succeeded, or EFail if 
	 * the prepare failed. Failing to prepare a draw 
	 * terminates the screen refresh and drawing 
	 * for the frame that is currently been drawn.
	 * 
	 * By default the base class PrepareDrawL does not do anything.
	 */
	IMPORT_C virtual TBool PrepareDrawL();

    /**
     * Draws the visual. 
     * 
     * This default implementation applies local transformation,
     * draws foreground brushes (over the content), 
     * background brushes (under the content) and 
     * content itself (by calling DrawSelf()) with clipping.
     * 
     * @note DrawSelf() should bee overridden by subclasses to 
     * draw the content of the visual.
     * 
     * @see DrawSelf()
     * @see PrepareDrawL()
     */
    IMPORT_C virtual void Draw(CHuiGc& aGc) const;

    /**
     * Draws a layer of brushes for the visual.
     *
     * @param aGc     Graphics context.
     * @param aLayer  Brush layer.
     */
    IMPORT_C void DrawBrushes(CHuiGc& aGc, THuiBrushLayer aLayer) const;

    /**
     * Overridden by subclasses to draw the content of the visual.
     * By default the base class DrawSelf does not draw anything.
     * 
     * @param aGc 			The graphics context to be used to draw the visual.
     * @param aDisplayRect  The bounding rectangle of the visual content,
     * 						in display coordinates. Same as calling 
     *                      CHuiVisual::DisplayRect().
     * 
     * @note DrawSelf won't be called for fully transparent visuals.
     * @see Draw()
     * @see PrepareDrawL()
     */
    IMPORT_C virtual void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;

    /**
     * Raises the change flag for this visual. Visual will be updated as soon
     * as possible.
     */
    IMPORT_C virtual void SetChanged();

    /**
     * Determines if the visual has changed since the last change
     * notification. Changed means becoming dirty, so that the area
     * of the visual has to be redrawn.
     */
    IMPORT_C virtual TBool Changed() const;

    /**
     * Called to inform the display that a dirty region has to be defined.
     */
    IMPORT_C virtual void ReportChanged();

    /**
     * Clears the changed status of the visual. This is called automatically
     * by the refresh loop after a frame has been drawn.
     */
    IMPORT_C virtual void ClearChanged();

    /**
     * Enables or disables brushes for the visual.
     */
    IMPORT_C void EnableBrushesL(TBool aEnabled = ETrue);

    /**
     * Returns the brushes of the visual.
     *
     * @return  Brush array. <code>NULL</code>, if not enabled.
     */
    IMPORT_C CHuiBrushArray* Brushes();

    /**
     * Debug method for dumping the structure of a visual tree into the
     * console.
     */
    IMPORT_C virtual void DumpTree() const;

    IMPORT_C virtual void GetInstanceName(TDes& aName) const;

    /**
     * Returns the name of the class.
     *
     * @param aName  Name of the visual class.
     */
    IMPORT_C virtual void GetClassName(TDes& aName) const;

    /**
     * Applies or restores the local projection of the visual.
     */
    IMPORT_C void EnterLocalProjection(CHuiGc& aGc, TBool aApply,
                                       TReal32 aExtraOffset = 0.0,
                                       const THuiRealRect* aVisibleRect = 0) const __SOFTFP;

    /**
     * Notifies the visual that the environment's skin has been changed
     * or updated. The visual should update its state (e.g., fonts)
     * accordingly.
     */
    IMPORT_C virtual void NotifySkinChangedL();
    
    
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
     *        If KHuiTactileFeedbackEventTypeNone, the event type is irrelevant.
     * 
     * @return ETrue if the visual has a tactile feedback defined.
     */
    IMPORT_C TBool HasTactileFeedback( 
        TInt aEventType = KHuiTactileFeedbackEventTypeNone ) const;
       
    /**
     * Removes tactile feedback for the visual.
     * 
     * @param aEventType Event type which feedback to remove.
     *        If KHuiTactileFeedbackEventTypeNone, all the feedbacks are removed.
     */
    IMPORT_C void RemoveTactileFeedback( TInt aEventType = KHuiTactileFeedbackEventTypeNone );
    
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
    IMPORT_C CHuiDropShadow* DropShadowHandler() const;

public: // From MHuiBrushGuide. 

    IMPORT_C TReal32 BrushOpacity() const __SOFTFP;

    IMPORT_C THuiRealRect BrushRect() const __SOFTFP;

    IMPORT_C CHuiSkin& BrushSkin() const;

public: // From MHuiSessionObject
    
    /**
     * Gets the object type.
     */
    IMPORT_C TType Type() const;

    /**
     * Gets the session id for object.
     */
    IMPORT_C TInt SessionId() const;

    /**
     * Sets the session id for object.
     */
    IMPORT_C void SetSessionId(TInt aSessionId);

    /** @endAPI */

public: // Internal utilities

    /**
     * Reports a layout update to the owner. The owner is notified of
     * the layout update if the layout update notification flag is set
     * for the visual.
     *
     * @see EHuiVisualFlagLayoutUpdateNotification
     */
    void ReportLayoutUpdate();


    /* Static member functions. */

    /**
     * Calculate dual alpha levels. Used when crossfading images.
     */
    IMPORT_C static void GetDualAlpha(TReal32 aOpacity, TReal32 aSecondary,
                             TInt& aPrimaryAlpha, TInt& aSecondaryAlpha);
                             
   /**
    * Return tactile feedback of the event. Checks the parent layouts
    * if this visual has not defined a feedback.
    * @param aEventType Event type 
    * @return Feedback type. KErrNotFound if not found.
    */
   TInt TactileFeedback( TInt aEventType ) const;
   
      
   /**
    * Finds a tacticon from this visual.
    * @param aEventType Event type to match
    * @return Index of the found event. KErrNotFound if not found-
    */
   TInt FindTacticon( TInt aEventType ) const;

    /**
     * Gets the visual's bounding rectangle in display coordinates that were
     * calculated during previous frame.
     * Display coordinates are defined relative to the display's visible area.
     * Uses the current values of the position and size. Only calculated once 
     * per frame. The rect is always in pixels.
     */
   THuiRealRect CachedDisplayRect() const;

    /**
     * Returns the brushes of the visual.
     *
     * @return  Brush array. <code>NULL</code>, if not enabled.
     */
    CHuiBrushArray* Brushes() const; 

    /**
     *  Returns the currently active effect for the visual
     *  or NULL if no effect is active.
     *
     *  @return CEffect instance or NULL.
     */
    IMPORT_C CHuiFxEffect* Effect() const;

    IMPORT_C MHuiEffectable *Effectable() const;

    IMPORT_C CHuiCanvasRenderBuffer *StoredRenderBuffer() const;
    IMPORT_C void SetStoredRenderBuffer(CHuiCanvasRenderBuffer *aRenderBuffer);
    IMPORT_C void SetStoredRenderBufferModificationsEnabled(TBool aEnabled);
    IMPORT_C void SetFreezeState(TBool aEnabled);
    IMPORT_C TBool Freezed() const;

    /**
     *  Assign an effect for the visual. The ownership of the
     *  effect is transferred to the visual.
     *
     *  @param aEffect Effect instance.
     */
    IMPORT_C void SetEffect(CHuiFxEffect* aEffect);

    /**
     * Assign the effect parser to the visual. The ownership is transferred to the visual
     * This must be done because the parsing is asynchronous, and we must be sure that the 
     * visual is still alive when the parsing completes.
     * 
     * @param aEffectParser Effect parser instance
     * 
     */
    void SetEffectParser( CHuiFxEffectParser* aEffectParser );
    
#ifdef HUI_DEBUG_TRACK_DRAWING
    inline void SetTracking( TBool aValue )
        {
        iTrackVisual = aValue;
        }
       
    inline TBool Tracking( ) const
        {
        return iTrackVisual;
        }
#endif      

    inline TBool LoadingEffect() const
        {
        return iLoadingEffect;
        };
    inline void SetLoadingEffect( TBool aLoading )
        {
        iLoadingEffect = aLoading;
        };

    /**
     * Queries canvas flags of the visual, returns result
     * @internal
     */    
    TInt QueryCanvasFlags();

    /**
     * Checks if external content drawing is enabled for this visual, returns result
     * @internal
     */    
    TBool QueryExternalContentDrawingEnabled();
    
    /**
     * Checks if visual has something to draw, returns result
     * @internal
     */    
    TBool QueryHasDrawableContent();
    
protected:

    /* Transformation. */

    /** @beginAPI */

    /**
     * Applies or restores the local transformation matrix of the visual.
     * Should be called before and after drawing the visual.
     *
     * @param aGc     Graphics context.
     * @param aApply  <code>ETrue</code>, if the local transformation should
     *                be applied; <code>EFalse</code>, if it should be
     *                removed.
     */
    IMPORT_C void Transform(CHuiGc& aGc, TBool aApply,
                            const THuiRealRect* aVisibleRect = 0) const;

    /** @endAPI */


protected:

    /* Protected methods. */

    /**
     * Expands the dirty region of the visual depending on the content of
     * the visual. This includes any brushes attached to the visual. Used
     * during dirty region reporting.
     *
     * @param aRect  Dirty region.
     */
    IMPORT_C virtual void ExpandRectWithContent(TRect& aRect) const;

    /**
     * Applies mirroring to a coordinate point according to the base unit
     * of the local coordinate system. 
     *
     * @param aPixels  Point to mirror in pixels. The mirrored result is
     *                 returned here.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     */
    void ApplyMirroring(THuiRealPoint& aPixels, THuiReferenceState aReferenceState) const;

    IMPORT_C virtual void VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
   
   /*
    * Convert a metric, measured in its own units, using a metric reference value suitable for 
    * that unit, into pixels.
    *
    * @note aReference will usually be supplied by calling MetricReferenceInPixels, or in the case of
    * a layout that needs to convert from pixels into the metric for its child visuals, will be supplied by 
    * calling CHuiLayout::MetricReferenceForLayoutInPixels
    *
    * @param aResult output the result of the calculation in pixels
    * @param aMetric the metric to convert
    * @param aReference the reference value, which must correspond to the unit type in the 
    *       provided metric, and which must always be supplied in pixels
    *
    */
   static void ConvertMetricLengthToPixels(TReal32& aResult, const THuiMetric& aMetric, TReal32 aReference);
   
   /*
    * Convert a value measured in pixels, into a metric value (whose unit type is supplied), using a 
    * metric reference value suitable for that unit type.
    *
    * @note aReference will usually be supplied by calling MetricReferenceInPixels, or in the case of
    * a layout that needs to convert from pixels into the metric for its child visuals, will be supplied by 
    * calling CHuiLayout::MetricReferenceForLayoutInPixels
    *
    * @param aResult when input, the metric unit type is used as the required unit type, and is not modified.
    *       The metric magnitude is replaced with the result of the calculation in that unit type
    * @param aPixles the pixel quantity to convert
    * @param aReference the reference value, which must correspond to the unit type in the 
    *       provided metric, and which must always be supplied in pixels
    */
   static void ConvertPixelsToMetricLength(THuiMetric& aResult, TReal32 aPixels, TReal32 aReference);

   TBool IsDelayedEffectSource() const;

private:

    /**
     * Converts local coordinates to display coordinates. This method is used
     * for calculating the position of the visual on the display when the visual
     * is about to be drawn. Display coordinates are defined relative to the 
     * display's visible area.
     *
     * @param aPoint  Point in the local coordinate system. This might be,
     *                for example, the position or size of this visual.
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     *
     * @return  The point in the display's coordinate system, in pixels.
     */
    THuiRealPoint LocalToDisplay(const THuiRealPoint& aPoint,
        THuiReferenceState aReferenceState) const;


    /**
     * Converts display coordinates to local coordinates. This method can be
     * used when determining where a particular point on the display is 
     * in the local coordinate system of a layout, for instance when handling
     * pointer input events. Display coordinates are defined relative to 
     * the display's visible area.
     *
     * @param aPoint  Point in the display coordinate system. 
     * @param aReferenceState Defines whether now or target value is used in case 
     *                        there are metrics transformations taking place
     *
     * @return  The point in this visual's local coordinate system, in the
     *          base units of the parent layout (or display, if there is
     *          no parent layout).
     */
    THuiRealPoint DisplayToLocal(const THuiRealPoint& aPoint,
            THuiReferenceState aReferenceState) const;


    /**
     * Determines if the visuals parent has changed since the last change
     * notification so that it may require also this visual to report itself
     * as changed.
     */
    TBool ParentChanged() const;

    /**
     * Checks if this visual is transformed.
     */
    TBool IsTransformed() const;
    
    /**
     * Retruns pointer to transformation if this visual is transformed. Otherwise NULL is returned.
     */
    CHuiTransformation* Transformation() const;

    /**
     * Handle flags changes.
     * @param aOldFlags flags previous to the changes.
     */
    void HandleFlagsChanged( TInt aOldFlags );

protected:

    /** Display area that was in effect during the previous draw operation.
        This will be included in the dirty rectangle. */
    mutable TRect iPreviousDrawn; 

public:

    /* Public properties. */
    /** ! Position / order / size of public variables must not change */

    /** Depth offset. */
    THuiTimedValue iDepthOffset;

    /**
     * Opacity of this visual.
     * The interpolation style of opacity values by default is linear.
     * A value of 0 is fully transparent, a value of 1 is fully opaque.
     */
    THuiTimedValue iOpacity;

private:

    /** ! Following three members are accessed by inline methods:
     * - Their location (at the beginning of member data) must not change
     * - Their order and size must not change
     */ 
    
    /** Flags. */
    TInt iFlags;

    /** Position of the visual. */
    THuiTimedPoint iPos;

    /** Size of the visual. */
    THuiTimedPoint iSize;

    /** ! ------------------------------- */

protected: // RnD utils
    /** Session id */
    TInt iSessionId;
    

    /** Owner of the visual. */
    MHuiVisualOwner* iOwner;

#ifdef HUI_DEBUG_TRACK_DRAWING
    /** Used for tracking the visual in different stages of rendering */
	TBool iTrackVisual;
#endif
    
private:

    /** The display on which the visual is being shown. */
    CHuiDisplay* iDisplay;

    /** Layout this visual belongs to (may be <code>NULL</code>). */
    CHuiLayout* iLayout;

    /**
     * Minimum allowed size for the visual. Layouts will respect this value
     * when layout is being updated.
     *
     * @todo  Needs to be implemented.
     */
    THuiRealSize iMinSize;

    /**
     * Maximum allowed size for the visual. Layouts will respect this value
     * when layout is being updated.
     *
     * @todo  Needs to be implemented.
     */
    THuiRealSize iMaxSize;

    /**
     * Padding between visual edges and content. The box metric can have a 
     * different amount of padding on each of the four sides.
     */
    //TPoint iPadding;
    THuiBoxMetric iPadding;

    /** If not <code>NULL</code>, this transformation is used when the
        content visual is drawn. */
    CHuiTransformation* iTransform;

    /** Background brushes. */
    CHuiBrushArray* iBrushes;

    /** If effect is still being loaded (parsing), visual may need to be hided */
    TBool iLoadingEffect;

    /** Tag. */
    HBufC8* iTag;

	/** 
	 * Cached display rectangle for the current frame. Defines 
	 * Calculates the visual's bounding rectangle in display coordinates. 
	 * The rect is always in pixels.
	 * @see DisplayRect()
	 */
	mutable THuiRealRect iDisplayRect;

	/**
	 * Describes during which frame the display rectance cache (iDisplayRect)
	 * is valid.
	 */
	mutable TUint iDisplayRectFrameNumber;

	/**	
     * Cached visual's bounding rectangle animation target in display 
     * coordinates. Uses the target values of the position and size, 
     * which means transitions are always immediate in the returned 
     * rectangle.
     */
	mutable THuiRealRect iDisplayRectTarget;

	/**
	 * Describes during which frame the display rectance target cache 
	 * (iDisplayRectTarget) is valid.
	 */
	mutable TUint iDisplayRectTargetFrameNumber;
    
    // Rest of the private data
    struct THuiVisualPrivateData;
    THuiVisualPrivateData* iVisualData;
    };


/* Inline methods. */

TInt CHuiVisual::Flags() const
    {
    return iFlags;
    }

inline THuiTimedPoint& CHuiVisual::Pos()
    {
    return iPos;
    }

inline THuiTimedPoint& CHuiVisual::Size()
    {
    return iSize;
    }

#endif  // __HUIVISUAL_H__
