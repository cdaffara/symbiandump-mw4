/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef T_ALFCOMMAND_H
#define T_ALFCOMMAND_H

#include <alf/alfimage.h>

class CAlfControl;
class TAlfObjectCommand;
class CAlfEnv;
class CAlfVisual;
class CAlfControlGroup;
class CAlfTextVisual;
class MAlfEventHandler;
class CAlfDisplay;
class CAlfImageVisual;
class CAlfTransformation;

/**
 * Command types.
 * All the commands that can be executed by the toolkit are enumerated here.
 */
enum TAlfCommandType
    {
  
    /** None corresponds the base class (TAlfCommand). */
    EAlfCommandTypeNone = 0,
    
    EAlfCommandTypeObject,
    
    EAlfCommandTypeAction,
    
    EAlfCommandTypeGroup,
    
    EAlfCommandTypeControl,
    
    EAlfCommandTypeVisual,
    
    EAlfCommandTypeImage,
    
    EAlfCommandTypeText,
    
    EAlfCommandTypeValue,
    
    EAlfCommandTypePoint,
    
    EAlfCommandTypeTransformation,
    
    EAlfCommandTypeCustomEvent,

    /** Marker command type. Types up from here will be used for markers only. */    
    EAlfCommandTypeMarkerFirst = 10000,
    EAlfCommandTypeMarker0 = EAlfCommandTypeMarkerFirst,
    EAlfCommandTypeMarker1,
    EAlfCommandTypeMarker2,
    EAlfCommandTypeMarker3,
    EAlfCommandTypeMarker4,
    EAlfCommandTypeMarker5,
    EAlfCommandTypeMarker6,
    EAlfCommandTypeMarker7,
    EAlfCommandTypeMarker8,
    EAlfCommandTypeMarker9,
    EAlfCommandTypeMarker10
    };


/** 
 * Command operations. 
 */
enum TAlfOp
    {
    EAlfOpNone = 0,
    																
    /* Control Group operations. */
    EAlfOpHide = 1000,
    EAlfOpShow,
    EAlfOpEnableInput,
    EAlfOpDisableInput,
    
    /* Control operations. */
    /* (start from 2000) */  // None defined yet.
    
    /* Visual operations. */
    EAlfOpRemoveAndDestroyAll = 3000,
    EAlfOpModifyFlags,
    EAlfOpSetPrimaryImage,
    EAlfOpSetSecondaryImage,
    EAlfOpSetText,
    EAlfOpEnableTransformation,
    EAlfOpDisableTransformation,
    
    /* Timed value operations. */
    EAlfOpSet = 4000,
    EAlfOpSetWithSpeed,
    
    /* Timed value style operations. The actual is:
       TAlfInterpolationStyle(iOperation - EAlfOpSetStyle) */
    EAlfOpSetStyle = 5000,
    
    /* Transformation operations. */
    EAlfOpLoadIdentity = 6000,
    EAlfOpTranslate,
    EAlfOpScale,
    EAlfOpRotate,
    EAlfOpReplaceStep
    };

/**
 * Object types.
 * These are the different types of objects which can issue and receive commands.
 */
enum TAlfCommandObjectType
    {
    EAlfCommandObjectTypeNone,

    /** This object is a visual. */
    EAlfCommandObjectTypeVisual,

    /** This object is a control. */
    EAlfCommandObjectTypeControl,

    /** This object is a control group. */
    EAlfCommandObjectTypeControlGroup
    };

/**
 * Value command target
 * 
 */
enum TAlfValueCommandTarget
    {
    EAlfValueCommandVisualSetOpacity
    };
    
/**
 * Point command target
 * 
 */
enum TAlfPointCommandTarget
    {
    EAlfPointCommandVisualSetPos,
    EAlfPointCommandVisualSetSize
    };

/**
 * TAlfCommand is the base class of all commands. A command is an
 * instruction to some class that can be set to occur some time in the future.
 * Each command stores information about an operation. Each command is also responsible for
 * the execution of the command. 
 * Commands are processed by the class's OfferEventL() method.
 */
class TAlfCommand
    {
public:

    /* Destructor. */

    IMPORT_C virtual ~TAlfCommand();
    

    /* Methods. */
    
    IMPORT_C TAlfCommandType Type() const;
    
    /**
     * Returns a pointer to the command casted to TAlfObjectCommand.
     * If this is not possible (the command is not derived from 
     * TAlfObjectCommand), returns NULL instead.
     */
    IMPORT_C virtual const TAlfObjectCommand* ObjectCommand() const;

    /**
     * Executes the command.
     *
     * @param aEnv  The environment in which the command is being executed.
     */
    virtual void ExecuteL(CAlfEnv& aEnv) const = 0;
    
    /**
     * Returns the size of the command.
     */
    virtual TInt Size() const = 0;
    
protected:

    /* Constructor. */

    /**
     * Protected constructor.
     *
     * @param aType  Type of the command.
     */
    IMPORT_C TAlfCommand(TAlfCommandType aType);
    
    IMPORT_C virtual void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /* These member variables are shared by all commands. */
    
    /** Type of the command. Depends on the command class. */
    TAlfCommandType iType;

private:
    TInt iSpare1;
    TInt iSpare2;
    };
    
/**
 * TAlfActionCommand is a simple action that triggers an action notification.
 * 
 * They are reserved for notifying observers of user-initiated actions in the user
 * interface. 
 * For example, a button control might send an action command when the user clicks on
 * the button.
 * 
 * Usage:
 * @code
 * 
 * //Candidate, interested in action notification, should implement 
 * // MAlfActionObserver interface
 * class ActionCommandTest : public MAlfActionObserver
 *  {
 *   public:
 * 	 .....
 * 	 .....
 * 	 .....
 *   void HandleAction( const TAlfActionCommand& aActionCommand );
 *  };
 *   
 * // From MAlfActionObserver
 * void ActionCommandTest::HandleAction( const TAlfActionCommand& aActionCommand )
 *    {
 *    //Perform required action on notification
 *    }
 * -------------------------------------------------------------------------------
 *  //Observer registrartion
 *  ActionCommandTest* testClass = new ActionCommandTest();
 * 
 *  //Create command
 *  TAlfActionCommand command( 1 );
 * 
 *  //Register test command as observer to environment
 *  iEnv->AddActionObserverL( testClass );
 *    
 *   //Send command
 *   iEnv->Send( command, 1 );
 * 
 *  //Cleanup
 *  iEnv->RemoveActionObserver( testClass );
 *  delete testClass;
 * 
 * @endcode
 * 
 * @see MAlfActionObserver
 * @see CAlfEnv::AddActionObserverL()
 * @see CAlfEnv::RemoveActionObserver()
 */  
 class TAlfActionCommand : public TAlfCommand
    {
public:
    
    /* Constructor. */

    /**
     * Constructs a new action command.
     *
     * @param aId  Identifier of the action.
     */
    IMPORT_C TAlfActionCommand(TInt aId);    

    /* Methods. */

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;

    /**
     * Determines the action identifier of the command.
     *
     * @param  Action identifier.
     */     
    IMPORT_C TInt Id() const;


protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /** Action identifier. */
    TInt iId;

private:
    TInt iSpare1;
    TInt iSpare2;
    };

/** 
 * TAlfObjectCommand is the base class for commands that modify data owned by 
 * an object. If the object gets deleted and the scheduler is notified of this, 
 * the command will be automatically canceled.
 *
 * Deriving class needs to override Clone() and Execute() functions from the Command
 * base class and NOT to call the base class implementation.
 */
class TAlfObjectCommand : public TAlfCommand
    {
public:

    /* Methods. */

    IMPORT_C const TAlfObjectCommand* ObjectCommand() const;

    /**
     * Returns the data owner object of the command.
     */
    IMPORT_C TAny* Object() const;
    
    /**
     * Returns the operation of the command.
     */
    IMPORT_C TAlfOp Operation() const;
    
    
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
     
     
    IMPORT_C TInt Size() const; 
     
protected:     

    /* Constructor. */

    /**
     * Protected constructor.
     *
     * @param aType             Type of the command.
     * @param aDataOwnerObject  Object that owns the data this command handles.
     * @param aOperation        Operation being done on the object.
     */
    IMPORT_C TAlfObjectCommand(TAlfCommandType aType, TAny* aDataOwnerObject, TAlfOp aOperation);
    
    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /** Object that owns the data this command handles. */
    TAny* iObject;

    /** Operation being done on the object. */
    TAlfOp iOperation;

private:
    TInt iSpare1;
    TInt iSpare2;
    };
    
/**
 * TAlfGroupCommand is an object command that operates on a control group.
 * Usage:
 * 
 * @code
 *  //Create control group command to show contro group on display
 *  TAlfGroupCommand command( *iControlGroup, EAlfOpShow, &display );
 *  
 *  //Send command , control group will be shown
 *   iEnv->Send( command, 1 );
 * 
 * //Create control group command to hide control group on display
 *  TAlfGroupCommand command1( *iControlGroup, EAlfOpHide, &display );
 *  
 *  //Send command , control group will be hidden
 *   iEnv->Send( command, 1 );
 * @endcode
*/    
class TAlfGroupCommand : public TAlfObjectCommand
    {
public:

    /* Constructor. */

    /**
     * Constructs a new control group command.
     *
     * @param aGroup      Control group.
     * @param aDisplay    Display associated with the command.
     * @param aOperation  Operation to do on the group.
     */
    IMPORT_C TAlfGroupCommand(CAlfControlGroup& aGroup, TAlfOp aOperation, 
                              CAlfDisplay* aDisplay = NULL);    

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the control group that the command operates on.
     */
    IMPORT_C CAlfControlGroup& ControlGroup() const;
    
    /**
     * Returns the display associated with the command.
     */
    IMPORT_C CAlfDisplay* Display() const;

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /** Display associated with the command. */
    CAlfDisplay* iDisplay;

private:
    TInt iSpare1;
    TInt iSpare2;
    };
    
/**
 * TAlfControlCommand is an object command that operates on a control.
 */    
class TAlfControlCommand : public TAlfObjectCommand
    {
public:

    /* Constructor. */

    /**
     * Constructs a new control command.
     *
     * @param aControl    Control.
     * @param aOperation  Operation to do on the control.
     */
    IMPORT_C TAlfControlCommand(CAlfControl& aControl, TAlfOp aOperation);    

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;

    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;   

    /* Methods. */
    
    /**
     * Returns the control that the command operates on.
     */
    IMPORT_C CAlfControl& Control() const;
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TInt iSpare1;
    TInt iSpare2;
    }; 
    
/**
 * TAlfVisualCommand is an object command that operates on a visual.
 * Usage:
 * 
 * @code
 *  //Create Visual command to move visual to front
 *   TAlfVisualCommand command( *iVisual, EAlfOpEnableTransformation );
 *  
 *  //Send command , visual will be moved to front
 *   iEnv->Send( command, 1 );
 * 
 * @endcode
 * 
 */    
class TAlfVisualCommand : public TAlfObjectCommand
    {
public:

    /* Constructor. */

    /**
     * Constructs a new visual command.
     *
     * @param aVisual     Visual.
     * @param aOperation  Operation to do on the visual.
     */
    IMPORT_C TAlfVisualCommand(CAlfVisual& aVisual, TAlfOp aOperation,
                               TInt aSetFlags = 0, TInt aClearFlags = 0);    

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;
    
    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C CAlfVisual& Visual() const;
    
    /**
     * Returns the flags that will be set by the command.
     */
    IMPORT_C TInt FlagsToSet() const;
    
    /**
     * Returns the flags that will be cleared by the command.
     */
    IMPORT_C TInt FlagsToClear() const;

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /** Flags to set. */
    TInt iSetFlags;
    
    /** Flags to clear. */
    TInt iClearFlags;

private:
    TInt iSpare1;
    TInt iSpare2;

    };
    
/**
 * TAlfImageCommand is a command that operaters on an image visual.
 * Usage:
 * 
 * @code
 * 
 *  //Create Image command to set primary image for imagevisual
 *   TAlfImageCommand command( *imageVisual, *image, EAlfOpSetPrimaryImage );
 *  
 *  //Send command , Primary image will be set
 *   iEnv->Send( command, 1 );
 * 
 * @endcode
 */    
class TAlfImageCommand : public TAlfObjectCommand
    {
public:    

    /* Constructor. */

    /**
     * Constructs a new image command.
     *
     * @param aImageVisual  Image visual.
     * @param aImage        Image.
     * @param aOperation    Operation to do on the visual.
     */
    IMPORT_C TAlfImageCommand(CAlfImageVisual& aImageVisual, const TAlfImage& aImage, 
                              TAlfOp aOperation = EAlfOpSetPrimaryImage);

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C CAlfImageVisual& ImageVisual() const;
    
    /**
     * Returns a reference to the image of the visual.
     */
    IMPORT_C const TAlfImage& Image() const;
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Image of the command. */
    TAlfImage iImage;    

private:
    TInt iSpare1;
    TInt iSpare2;
    };  
    
/**
 * TAlfTextCommand is a command that operaters on a text visual.
 *    
 * Usage:
 * 
 * @code
 * 
 *  //Create text command to set text in textvisual
 *   TAlfTextCommand command( *textVisual, _L( "Text" ) );
 *  
 *  //Send command , text will be set to textvisual
 *   iEnv->Send( command, 1 );
 * @endcode
 */    
class TAlfTextCommand : public TAlfObjectCommand
    {
public:    

    /* Constructor. */

    /**
     * Constructs a new text command.
     *
     * @param aTextVisual  Text visual.
     * @param aText        Text.
     * @param aOperation   Operation to perform.
     */
    IMPORT_C TAlfTextCommand(CAlfTextVisual& aTextVisual, const TDesC& aText, 
                             TAlfOp aOperation = EAlfOpSetText);

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C CAlfTextVisual& TextVisual() const;
    
    /**
     * Returns a reference to the text of the visual.
     */
    IMPORT_C const TDesC& Text() const;
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Text of the command. */
    const TDesC& iText;

private:
    TInt iSpare1;
    TInt iSpare2;
    };
    
    
/**
 * TAlfValueCommand is a command that operates on a single timed value.
 *
 * @code
 * 
 *  //Create timed value command to set visualopacity of a visual
 *  TAlfValueCommand command( visual, EAlfValueCommandVisualSetOpacity, 0.2, 15 );
 *  
 *  //Send command , visualopacity will be set
 *   iEnv->Send( command, 1 );
 * 
 * @endcode
 */    
class TAlfValueCommand : public TAlfObjectCommand 
    {
public:

    /* Constructor. */

    /**
     * Constructs a new value command.
     *
     * @param aObject          Object. Needs to match with aObjectType
     * @param aObjectType      Timed value to modify.
     * @param aTarget          Target value
     * @param aTransitionTime  Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C TAlfValueCommand(
        TAny* aObject, 
        TAlfValueCommandTarget aObjectType, 
        TReal32 aTarget, 
        TInt aTransitionTime = 0 ) __SOFTFP;  
        
    /**
     * Constructs a new value command. This constructor takes an operation 
     * parameter in addition to the target and timing values.
     *
     * @param aObject          Object. Needs to match with aObjectType
     * @param aObjectType      Timed value to modify.
     * @param aOperation  Operation to perform.
     * @param aTarget     Target value
     * @param aTiming     Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C TAlfValueCommand(TAny* aObject, TAlfValueCommandTarget aObjectType, 
                              TAlfOp aOperation = EAlfOpSet,
                              TReal32 aTarget = 0.0, TReal32 aTiming = 0.0) __SOFTFP;
                             
    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */

    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C TAlfValueCommandTarget TimedValue() const;
    
    /**
     * Returns the target value.
     */
    IMPORT_C TReal32 Target() const __SOFTFP;
    
    /**
     * Returns the transition time.
     */
    IMPORT_C TInt TransitionTime() const;
    
    /**
     * Returns the transition speed. Zero, if not set.
     */
    IMPORT_C TReal32 Speed() const __SOFTFP;
       
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Timed value being operated on. */
    TAlfValueCommandTarget iTimedValue;

    /** Target value. */
    TReal32 iTarget;
    
    /** Transition time in milliseconds. */
    TInt iTransitionTime;
    
    /** Optionally, transition speed in units/second. */
    TReal32 iSpeed; 

private:
    TInt iSpare1;
    TInt iSpare2;
    };

/**
 * TAlfPointCommand is a command that operates on a timed point.
 * Usage:
 * @code
 * 
 *  //Create timed point command to set visual size
 *   TAlfRealPoint p1( 0.1, 0.1 );
 *   TAlfPointCommand command1( iVisual, EAlfValueCommandVisualSize, p1, 1 );
 *  
 *  //Send command , Primary image will be set
 *   iEnv->Send( command, 1 );
 * 
 * @endcode
 */    
class TAlfPointCommand : public TAlfObjectCommand 
    {
public:

    enum TParameterTarget
        {
        EBoth = 0, // default
        EXOnly,
        EYOnly
        };

    /* Constructor. */

    /**
     * Constructs a new timed point command.
     *
     * @param aObject     Object. Needs to match with aObjectType
     * @param aObjectType Timed value to modify.
     * @param aOperation  Operation to perform.
     */
    IMPORT_C TAlfPointCommand(TAny* aObject, TAlfPointCommandTarget aObjectType, 
                              TAlfOp aOperation);

    /**
     * Constructs a new timed point command.
     *
     * @param aObject          Object. Needs to match with aObjectType
     * @param aObjectType      Timed value to modify.
     * @param aTarget          Target value
     * @param aTransitionTime  Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C TAlfPointCommand(TAny* aObject, TAlfPointCommandTarget aObjectType, 
                              TAlfRealPoint aTarget, TInt aTransitionTime = 0,
                              TParameterTarget aParameterTarget = EBoth ); 
    
    /**
     * Constructs a new timed point command. This constructor takes an operation 
     * parameter in addition to the target and timing values.
     *
     * @param aObject     Object. Needs to match with aObjectType
     * @param aObjectType Timed value to modify.
     * @param aOperation  Operation to perform.
     * @param aTarget     Target value.
     * @param aTiming     Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C TAlfPointCommand(TAny* aObject, TAlfPointCommandTarget aObjectType, 
                              TAlfOp aOperation,
                              TAlfRealPoint aTarget, TReal32 aTiming = 0.0,
                              TParameterTarget aParameterTarget = EBoth) __SOFTFP;
                             
    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the object type that the command operates on.
     */
    IMPORT_C TAlfPointCommandTarget TimedPoint() const;
    
    /**
     * Returns the target value.
     */
    IMPORT_C TAlfRealPoint Target() const;
    
    /**
     * Returns the transition time.
     */
    IMPORT_C TInt TransitionTime() const;
    
    /**
     * Returns the transition speed. Zero, if not set.
     */
    IMPORT_C TReal32 Speed() const __SOFTFP;

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Timed point being operated on. */
    TAlfPointCommandTarget iTimedPoint;

    /** Target value. */
    TAlfRealPoint iTarget;
    
    /** Transition time in milliseconds. */
    TInt iTransitionTime;
    
    /** Optionally, transition speed in units/second. */
    TReal32 iSpeed;

    TParameterTarget iParameterTarget;

private:
    TInt iSpare1;
    TInt iSpare2;
    };

/**
 * TAlfTransformationCommand is a command that operates on a transformation.
 * 
 * Transformation command can generate one of the following operations:
 * @li EAlfOpLoadIdentity - Loads an identity matrix to the transformation.
 * @li EAlfOpTranslate - Applies a translation to the object affected by the transformation.
 * @li EAlfOpScale - Applies scaling to the object affected by the transformation.
 * @li EAlfOpRotate - Applies a rotation to the object that is affected by the transformation.
 * @li EAlfOpReplaceStep - Replaces the whole transformation step values with the 
 * ones given as command parameters
 *
 * If the transformation operation is rotation the default rotation is around z-axis.
 * If the transformation operation is scaling the default scale factors are 1.0 on all axis.
 * On any other transformation the x,y and z components are zero by default.
 * 
 * Usage:
 * @code
 * 	
 * 	//Enable transformation
 *  iVisual->EnableTransformationL( );
 * 
 * //Get transformation object
 *  CAlfTransformation* transformation = &iVisual->Transformation();
 * 
 *  //Set transformation properties
 *   const TInt rotateAngle = 23.67;
 *   transformation->Rotate( rotateAngle );
 * 
 * //Create transformation command
 * TAlfTransformationCommand command( iVisual,transformation, EAlfOpRotate, 0 );
 * 
 * //Properties can also be set through command
 *  command.SetAngle( TAlfTimedValue( 56 ) );
 * 
 * //Send command
 *  iEnv->Send( command, 1 );
 * 
 * @endcode
 */    
class TAlfTransformationCommand : public TAlfObjectCommand
    {
public:    

    enum 
        {
        /** Used in the aStep parameter to specify that no step index is 
            specified. */
        EStepNone = -1
        };


    /* Constructor. */

    /**
     * Constructs a new transformation command.
     *
     * @param aOwner      Owner of the transformation. Allowed be NULL.
     * @param aTransformation  Transformation to operate on.
     * @param aOperation  Operation to perform.
     * @param aStep       Step index in the transformation.
     */
    IMPORT_C TAlfTransformationCommand(TAny* aOwner, CAlfTransformation& aTransformation,
                                       TAlfOp aOperation = EAlfOpLoadIdentity,
                                       TInt aStep = EStepNone);

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the transformation that the command operates on.
     */
    IMPORT_C CAlfTransformation& Transformation() const;
    
    /**
     * Returns the transformation step index.
     */
    IMPORT_C TInt Step() const;
    
    /**
     * Sets the X component.
     * - If transformation is translation this sets the amount of translation on X axis.
     * - If transformation is rotation this sets the X component of rotation axis vector.
     * - If transformation is scaling this sets the scaling factor for the X axis.
     *
     * @param aValue Timed value to set to the X component.
     */
    IMPORT_C void SetX(const TAlfTimedValue& aValue);

    /**
     * Sets the Y component.
     * - If transformation is translation this sets the amount of translation on Y axis.
     * - If transformation is rotation this sets the Y component of rotation axis vector.
     * - If transformation is scaling this sets the scaling factor for the Y axis.
     *
     * @param aValue Timed value to set to the Y component.
     */
    IMPORT_C void SetY(const TAlfTimedValue& aValue);

    /**
     * Sets the Z component.
     * - If transformation is translation this sets the amount of translation on Z axis.
     * - If transformation is rotation this sets the Z component of rotation axis vector.
     * - If transformation is scaling this sets the scaling factor for the Z axis.
     *
     * @param aValue Timed value to set to the Z component.
     */
    IMPORT_C void SetZ(const TAlfTimedValue& aValue);
    
    /**
     * Set the angle of rotation if the transformation is rotation.
     */
    IMPORT_C void SetAngle(const TAlfTimedValue& aValue);
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Image of the command. */
    CAlfTransformation& iTransformation;

    /** Step index. */    
    TInt iStep;

    /** X component. Suspended. */
    TAlfTimedValue iX;
    
    /** Y component. Suspended. */
    TAlfTimedValue iY;
    
    /** Z component. Suspended. */
    TAlfTimedValue iZ;
    
    /** Angle component. Suspended. */
    TAlfTimedValue iAngle;
    
private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * TAlfMarkerCommand is a special marker command that has no functionality, 
 * but can be entered into the scheduler to mark a specific point in time.
 */
class TAlfMarkerCommand : public TAlfCommand
    {
public:
    
    /* Constructor. */

    /**
     * Constructs a new marker command.
     *
     * @param aId  Identifier of the marker.
     */
    IMPORT_C TAlfMarkerCommand(TInt aId);    


    /* Methods. */

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;
    
    /**
     * Determines the identifier of the marker.
     *
     * @param  Marker identifier.
     */     
    IMPORT_C TInt Id() const;
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:
    TInt iSpare1;
    TInt iSpare2;
    };

    
    
/**
 * TAlfCustomEventCommand is a command that sends a custom event. The command
 * can be sent to any individual MAlfEventHandler derived object or broadcasted to 
 * all Control derived objects.
 * 
 * Usage
 * 
 * @code
 *  //Create custom Event command, as iControl implements, MAlfEventHandler,
 *  // it will recieve custom event
 *  TAlfCustomEventCommand command( KCustomCommandIdDefault, iControl );
 * 
 * //Send command
 *  iEnv->Send( command, 1 );
 * 
 * @endcode
 */    
class TAlfCustomEventCommand : public TAlfObjectCommand
    {
public:
    
    /* Constructor. */

    /**
     * Constructs a new custom event command.
     *
     * @param aEventParam  Parameter of the custom event.
     * @param aRecipient   Optional recipient of the event.
     * @param aEventData   Optional event data identifier.
     *
     * @todo  It would be good to be able to specify more information to 
     *        be sent with the custom event, e.g., a pointer. Also check
     *        out TAlfEvent.
     */
    IMPORT_C TAlfCustomEventCommand(
        TInt aEventParam, MAlfEventHandler* aRecipient = NULL, TInt aEventData = 0 );

    /* Methods. */

    /* Implementation of TAlfCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CAlfEnv& aEnv) const;
    
    /* Implementation of TAlfCommand::Size(). */
    IMPORT_C TInt Size() const;

    /**
     * Returns the recipient of the event, or NULL if none defined.
     */
    IMPORT_C MAlfEventHandler* Recipient() const;
    
    /**
     * Determines the custom event parameter of the command.
     *
     * @return  Parameter.
     */     
    IMPORT_C TInt Param() const;
    
    /**
     * Returns the event data of the command.
     *
     * @return  Event data.
     */     
    IMPORT_C TInt EventData() const;    
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Custom event parameter.  @todo Could be a pointer as well? */
    TInt iEventParam;
        
    /** Event data. */
    TInt iEventData;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


#endif // T_ALFCOMMAND_H
