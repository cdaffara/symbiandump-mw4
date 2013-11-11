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
* Description:   Control group
*
*/



#ifndef C_ALFCONTROLGROUP_H
#define C_ALFCONTROLGROUP_H

#include <e32base.h>

class CAlfEnv;
class CAlfControl;
class CAlfTransformation;
class CAlfDisplay;

/**
 *  A control group class is used to group controls that belong together.
 *
 *  Control group contains CAlfControl - derived classes. Control group can be
 *  used to create logical compounds of several controls. For instance all controls
 *  belonging to the same UI view can be grouped into a single control group.
 *
 *  Control groups should be instantiated through CAlfEnv::NewControlGroupL().
 *  Usage:
 *  @code
 * //Create a new control group
 *  CAlfControlGroup& group = iEnv->NewControlGroupL( KControlGroupIdDefault );
 * 
 * //Create control instance and append it to group
 *  CMyControl* control = CMyControl::ConstructL(&iEnv);
 *  group.AppendL( control );
 * 
 *  //Show control group on display
 *  iDisplay->Roster().ShowL( group );
 *  @endcode 
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfControlGroup ): public CBase
    {
public:

    /**
     * 1st phase constructor.
     */
    CAlfControlGroup();
    
    /**
     * 2nd phase constructor.
     * @param aResourceId Identifier.
     * @param aEnv The env.
     */
    void ConstructL( TInt aResourceId, CAlfEnv& aEnv );

    /**
     * Destructor.
     */
    virtual ~CAlfControlGroup();

    /**
     * Return the server side handle
     *
     * @return Handle to the server side CAlfSrvControlGroupSubSession object. 
     *         0 if not set.
     */
    TInt Identifier() const;
    
    /**
     * Binds a display to a control group. If a display is bound to a control group
     * it is being shown. Otherwise the control group is hidden. CAlfRoster uses
     * this method to notify control group about the display on which it is shown.
     * User should use methods in CAlfRoster to show or hide a control group.
     *
     * @see CAlfRoster
     *
     * @param aDisplay Pointer to a display on which this control group is shown,
     *                 or NULL if control group is hidden.
     */
    void BindDisplay(CAlfDisplay* aDisplay);
    
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
    IMPORT_C void AppendL( CAlfControl* aControl );
    
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
    IMPORT_C TInt Remove(CAlfControl* aControl);
    
    /**
	 * Determines whether the group accepts input events.
	 *
	 * @return ETrue is accepts input.
	 */
    IMPORT_C TBool AcceptInput() const;
    
    /**
     * Sets the input event acceptance of the group.
     *
     * @param aAcceptInput  Accept input events.
     */
    IMPORT_C void SetAcceptInput( TBool aAcceptInput );
    
    /**
     * Finds control
     * @see CAlfEnv::FindControl()
     * @return  Pointer to the control, or <code>NULL</code> if the control
     *          is not in the group.
     */
    IMPORT_C CAlfControl* FindControl( TInt aId,  TBool aUserId = ETrue ) const;
    
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
    IMPORT_C CAlfControl& Control( TInt aIndex ) const;
    
    /**
     * Enables or disables transformation of the control group.
     *
     * @param aIsTransformed ETrue for enabling.
     */
    IMPORT_C void EnableTransformationL( TBool aIsTransformed = ETrue );
    
    /**
     * Determines if the control group has a transformation.
     *
     * @return ETrue is transformation is enabled.
     */
    IMPORT_C TBool IsTransformed() const;
    
    /**
     * Returns the transformation of the control group.
     *
     * @return  Transformation steps.
     */
    IMPORT_C CAlfTransformation& Transformation();

private:
    
    // Private data. Owned.
    struct TPrivateData;
    TPrivateData* iData;

    };


#endif // C_ALFCONTROLGROUP_H
