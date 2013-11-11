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
* Description:   ?Description
*
*/



#ifndef __HUICONTROLGROUP_H__
#define __HUICONTROLGROUP_H__


#include <e32base.h>
#include <barsread.h>

#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/huieventhandler.h>
#include <uiacceltk/HuiSessionObject.h>



enum THuiControlGroupRole
    {
    /** Unknown */
    EHuiUnknownContainer,
    
    /** Alf application control group */
    EHuiSessionContainer,
    
    /** WSERV window group */
    EHuiWindowGroupContainer,
    
    /** WSERV floating sprite group */
    EHuiWindowFloatingSpriteContainer,

    /** Group for showing effects */
    EHuiFullScreenEffectContainer,
    
    /** Group for FPS indicator */
    EHuiFpsIndicatorContainer
    };


/* Forward declarations. */
class CHuiEnv;
class CHuiControl;
class CHuiTransformation;


/**
 * CHuiControlgroup instances are used to organize controls into logical
 * groups (based on, e.g., UI views or functionality).
 *
 * Control groups have ownership of CHuiControl instances in the toolkit.
 *
 * During the application startup the application programmer sets up
 * controls into control groups. Typically the initialization phase ends
 * up showing one or more control groups on a display:
 *
 * <code>
 * iHuiDisplay->Roster().ShowL(group);
 * </code>
 */
NONSHARABLE_CLASS(CHuiControlGroup) : public CBase, public MHuiSessionObject
	{
public:

	/* Constructors and destructor. */

	/**
	 * Constructor.
	 */
	CHuiControlGroup(TInt aResourceId, CHuiEnv& aEnv);

	/**
	 * Second-phase constructor.
	 */
	void ConstructL();

    /**
	 * Second-phase constructor that reads definitions using a resource
	 * reader.
	 *
	 * @param aReader  Resource reader initialized with the group definition.
	 */
	void ConstructFromResourceL(TResourceReader& aReader);

	/**
	 * Destructor.
	 */
	virtual ~CHuiControlGroup();


	/* Methods. */

	/**
	 * Determines the identifier of the control group (from a resource file).
	 *
	 * @return  Resource identifier.
	 */
	IMPORT_C TInt ResourceId() const;

	/**
	 * Adds a new control into the group. The group takes ownership of the
	 * control.
	 *
	 * @param aControl  Control to insert into the group.
	 */
	IMPORT_C void AppendL(CHuiControl* aControl);

	/**
	 * Adds a new control into the group and pops it off of the cleanup
	 * stack.
	 *
	 * @param aControl  Control to insert into the group and pop.
	 */
	IMPORT_C void AppendAndPopL(CHuiControl* aControl);

    /**
     * Removes a control from the control group.
     *
     * @param aControl  Control to remove from the group. The control's
     *                  ownership is transferred to the caller. Note that
     *                  controls should never be used unless they are
     *                  owned by a control group.
     *
     * @return  KErrNone, if successful. KErrNotFound, if the control is not
     *          in the group.
     */
    IMPORT_C TInt Remove(CHuiControl* aControl);

    /**
     * Returns the number of controls within this group.
     * 
     * @return Number of controls.
     */
    IMPORT_C TInt Count() const;

    /**
     * Access a control within the group, by its index into the
     * internal list of controls. Will panic if an out-of-bounds
     * index is passed.
     * 
     * @param aIndex  Index of the control to return.
     * 
     * @return Reference to the requested control. Always
     *         returns a valid control.
     */
    IMPORT_C CHuiControl& Control(TInt aIndex) const;

    /**
     * @return  Pointer to the control, or <code>NULL</code> if the control
     *          is not in the group.
     */
    IMPORT_C CHuiControl* FindControl(TInt aId) const;

    /**
     * Enables or disables transformation of the control group.
     */
    IMPORT_C void EnableTransformationL(TBool aIsTransformed = ETrue);

    /**
     * Determines if the control group has a transformation.
     */
    IMPORT_C TBool IsTransformed() const;

    /**
     * Returns the transformation of the control group.
     *
     * @return  Transformation steps.
     *
     * @panic  THuiPanic::EVisualTransformNotEnabled
     */
    IMPORT_C CHuiTransformation& Transformation();

	/**
	 * Determines whether the group accepts input events.
	 */
	IMPORT_C TBool AcceptInput() const;

    /**
     * Sets the input event acceptance of the group.
     *
     * @param aAcceptInput  Accept input events.
     */
    IMPORT_C void SetAcceptInput(TBool aAcceptInput);

    /**
     * Determines if the control group has been changed. The roster calls this
     * to determine if larger updates are necessary.
     */
    TBool Changed() const;

    /**
     * Clears the change flags of the control group and its member controls.
     */
    void ClearChanged();


    // Implementation of MHuiSessionObject
    
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

    IMPORT_C void SetSecureId(TInt aSecureId);
    IMPORT_C TInt SecureId();

private:

    /* Private methods */


public:

    /** Events that are not handled by any controls in the control group
        are offered to these event handlers. */
    RHuiObserverArray<MHuiEventHandler> iUnhandledEventObservers;


private:

	/** Resource ID that was passed in the constructor. */
    TInt iResourceId;

    /** Environment. */
    CHuiEnv& iEnv;

    /** Accepts input events. */
    TBool iAcceptInput;

    /** Controls owned by the group. */
    RPointerArray<CHuiControl> iControls;

    /** Common transformation for all the root visuals of the
        control group. */
    CHuiTransformation* iTransform;

    /** Session id */
    TInt iSessionId;
    
    TInt iSecureId;

public:

    /** 
     * Common opacity for all the controls of the group.
     *    
     * @deprecated  Do not use control and control group opacities.
     *              Instead, set up common parent layouts and adjust their
     *              opacities. Logic objects should not affect the visible
     *              elements. 
     */
    THuiTimedValue iOpacity;
    TBool iAlfApp;
  
	};

#endif  // __HUICONTROLGROUP_H__
