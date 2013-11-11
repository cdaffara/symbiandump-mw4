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
* Description:   Definition of THuiCommand and derived command classes. 
*
*/



#ifndef __HUICOMMAND_H__
#define __HUICOMMAND_H__


#include <e32base.h>

#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiTimedPoint.h>


/* Forward declarations. */
class CHuiEnv;
class CHuiControlGroup;
class CHuiControl;
class CHuiDisplay;
class CHuiVisual;
class CHuiImageVisual;
class CHuiTextVisual;
class CHuiTransformation;
class THuiObjectCommand;
class THuiEvent;
class MHuiEventHandler;


/**
 * Command types.
 * All the commands that can be executed by the toolkit are enumerated here.
 */
enum THuiCommandType
    {
  
    /** None corresponds the base class (THuiCommand). */
    EHuiCommandTypeNone = 0,
    
    EHuiCommandTypeObject,
    
    EHuiCommandTypeAction,
    
    EHuiCommandTypeGroup,
    
    EHuiCommandTypeControl,
    
    EHuiCommandTypeVisual,
    
    EHuiCommandTypeImage,
    
    EHuiCommandTypeText,
    
    EHuiCommandTypeValue,
    
    EHuiCommandTypePoint,
    
    EHuiCommandTypeTransformation,
    
    EHuiCommandTypeCustomEvent,

    /** Marker command type. Types up from here will be used for markers only. */    
    EHuiCommandTypeMarkerFirst = 10000,
    EHuiCommandTypeMarker0 = EHuiCommandTypeMarkerFirst,
    EHuiCommandTypeMarker1,
    EHuiCommandTypeMarker2,
    EHuiCommandTypeMarker3,
    EHuiCommandTypeMarker4,
    EHuiCommandTypeMarker5,
    EHuiCommandTypeMarker6,
    EHuiCommandTypeMarker7,
    EHuiCommandTypeMarker8,
    EHuiCommandTypeMarker9,
    EHuiCommandTypeMarker10
    };


/** 
 * Command operations. 
 */
enum THuiOp
    {
    EHuiOpNone = 0,
    
    /* Control Group operations. */
    EHuiOpHide = 1000,
    EHuiOpShow,
    EHuiOpEnableInput,
    EHuiOpDisableInput,
    
    /* Control operations. */
    /* (start from 2000) */  // None defined yet.
    
    /* Visual operations. */
    EHuiOpRemoveAndDestroyAll = 3000,
    EHuiOpModifyFlags,
    EHuiOpSetPrimaryImage,
    EHuiOpSetSecondaryImage,
    EHuiOpSetText,
    EHuiOpEnableTransformation,
    EHuiOpDisableTransformation,
    
    /* Timed value operations. */
    EHuiOpSet = 4000,
    EHuiOpSetWithSpeed,
    
    /* Timed value style operations. The actual is:
       THuiInterpolationStyle(iOperation - EHuiOpSetStyle) */
    EHuiOpSetStyle = 5000,
    
    /* Transformation operations. */
    EHuiOpLoadIdentity = 6000,
    EHuiOpTranslate,
    EHuiOpScale,
    EHuiOpRotate
    };


/**
 * Object types.
 * These are the different types of objects which can issue and receive commands.
 */
enum THuiCommandObjectType
    {
    EHuiCommandObjectTypeNone,

    /** This object is a visual. */
    EHuiCommandObjectTypeVisual,

    /** This object is a control. */
    EHuiCommandObjectTypeControl,

    /** This object is a control group. */
    EHuiCommandObjectTypeControlGroup
    };



/**
 * THuiCommand is the base class of all commands. A command is an
 * instruction to some class that can be set to occur some time in the future.
 * Each command stores information about an operation. Each command is also responsible for
 * the execution of the command.
 * Commands are processed by the class's OfferEventL() method.
 */
class THuiCommand
    {
public:

    /* Destructor. */

    IMPORT_C virtual ~THuiCommand();
    

    /* Methods. */

    /**
     * Returns a pointer to the command casted to THuiObjectCommand.
     * If this is not possible (the command is not derived from 
     * THuiObjectCommand), returns NULL instead.
     */
    IMPORT_C virtual const THuiObjectCommand* ObjectCommand() const;

    /**
     * Executes the command.
     *
     * @param aEnv  The environment in which the command is being executed.
     */
    virtual void ExecuteL(CHuiEnv& aEnv) const = 0;
    
    /**
     * Returns the size of the command.
     */
    virtual TInt Size() const = 0;
    
    /**
     * Sets the sender object of the command.
     * The sender must not be deleted while the command is pending execution in a scheduler.
     *
     * @param aType    Sender type.
     * @param aSender  Pointer to sender.     
     */
    IMPORT_C void SetSender(THuiCommandObjectType aType, TAny* aSender);
       
    /**
     * Determines the type of the command.
     *
     * @return  Command type.
     */
    IMPORT_C THuiCommandType Type() const;
    
    /**
     * Determines the type of the object that sent the command.
     *
     * @return  Sender object type.
     */
    IMPORT_C THuiCommandObjectType SenderType() const;
    
    /**
     * Returns the sender object. This is the object which issued
     * the command originally.
     *
     * @return the object which sent the command.
     */
    IMPORT_C TAny* Sender() const;
    
    /**
     * Returns the sender visual. Returns NULL if the sender is not a visual.
     *
     * @return  Visual, or NULL.
     * @see Sender()
     */     
    IMPORT_C CHuiVisual* SenderVisual() const;
    
    /**
     * Returns the sender control. Returns NULL if the sender is not a control.
     *
     * @return  Control, or NULL.
     * @see Sender()
     */     
    IMPORT_C CHuiControl* SenderControl() const;
    
    /**
     * Returns the sender control group. Returns NULL if the sender is not a control group.
     *
     * @return  Control group, or NULL.
     * @see Sender()
     */     
    IMPORT_C CHuiControlGroup* SenderControlGroup() const;
    

protected:

    /* Constructor. */

    /**
     * Protected constructor.
     *
     * @param aType  Type of the command.
     */
    IMPORT_C THuiCommand(THuiCommandType aType);
    
    IMPORT_C virtual void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private:

    /* These member variables are shared by all commands. */
    
    /** Type of the command. Depends on the command class. */
    THuiCommandType iType;
    
    /** Type of the object that sent the command. */
    THuiCommandObjectType iSenderType;
    
    /** Object that sent this command. Must exist. */
    TAny* iSender;
    
private:
    TInt iSpare1;
    TInt iSpare2;
    
    };


/**
 * THuiActionCommand is a simple action that triggers an action notification.
 */
class THuiActionCommand : public THuiCommand
    {
public:
    
    /* Constructor. */

    /**
     * Constructs a new action command.
     *
     * @param aId  Identifier of the action.
     */
    IMPORT_C THuiActionCommand(TInt aId);    


    /* Methods. */

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
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
 * THuiObjectCommand is the base class for commands that modify data owned by 
 * an object. If the object gets deleted and the scheduler is notified of this, 
 * the command will be automatically canceled.
 */
class THuiObjectCommand : public THuiCommand
    {
public:

    /* Methods. */

    IMPORT_C const THuiObjectCommand* ObjectCommand() const;
    
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;

    IMPORT_C TInt Size() const;

    /**
     * Returns the data owner object of the command.
     */
    IMPORT_C TAny* Object() const;
    
    /**
     * Returns the operation of the command.
     */
    IMPORT_C THuiOp Operation() const;
     
     
protected:     

    /* Constructor. */

    /**
     * Protected constructor.
     *
     * @param aType             Type of the command.
     * @param aDataOwnerObject  Object that owns the data this command handles.
     * @param aOperation        Operation being done on the object.
     */
    IMPORT_C THuiObjectCommand(THuiCommandType aType, TAny* aDataOwnerObject, THuiOp aOperation);

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Object that owns the data this command handles. */
    TAny* iObject;

    /** Operation being done on the object. */
    THuiOp iOperation;
        
    };
    
    
/**
 * THuiGroupCommand is an object command that operates on a control group.
 */    
class THuiGroupCommand : public THuiObjectCommand
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
    IMPORT_C THuiGroupCommand(CHuiControlGroup& aGroup, THuiOp aOperation, 
                              CHuiDisplay* aDisplay = NULL);    

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the control group that the command operates on.
     */
    IMPORT_C CHuiControlGroup& ControlGroup() const;
    
    /**
     * Returns the display associated with the command.
     */
    IMPORT_C CHuiDisplay* Display() const;

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /** Display associated with the command. */
    CHuiDisplay* iDisplay;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * THuiControlCommand is an object command that operates on a control.
 */    
class THuiControlCommand : public THuiObjectCommand
    {
public:

    /* Constructor. */

    /**
     * Constructs a new control command.
     *
     * @param aControl    Control.
     * @param aOperation  Operation to do on the control.
     */
    IMPORT_C THuiControlCommand(CHuiControl& aControl, THuiOp aOperation);    

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;

    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;   


    /* Methods. */
    
    /**
     * Returns the control that the command operates on.
     */
    IMPORT_C CHuiControl& Control() const;
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * THuiVisualCommand is an object command that operates on a visual.
 */    
class THuiVisualCommand : public THuiObjectCommand
    {
public:

    /* Constructor. */

    /**
     * Constructs a new visual command.
     *
     * @param aVisual     Visual.
     * @param aOperation  Operation to do on the visual.
     */
    IMPORT_C THuiVisualCommand(CHuiVisual& aVisual, THuiOp aOperation,
                               TInt aSetFlags = 0, TInt aClearFlags = 0);    

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C CHuiVisual& Visual() const;
    
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
 * THuiImageCommand is a command that operaters on an image visual.
 */    
class THuiImageCommand : public THuiObjectCommand
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
    IMPORT_C THuiImageCommand(CHuiImageVisual& aImageVisual, const THuiImage& aImage, 
                              THuiOp aOperation = EHuiOpSetPrimaryImage);

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C CHuiImageVisual& ImageVisual() const;
    
    /**
     * Returns a reference to the image of the visual.
     */
    IMPORT_C const THuiImage& Image() const;
    
protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Image of the command. */
    THuiImage iImage;    

private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * THuiTextCommand is a command that operaters on a text visual.
 */    
class THuiTextCommand : public THuiObjectCommand
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
    IMPORT_C THuiTextCommand(CHuiTextVisual& aTextVisual, const TDesC& aText, 
                             THuiOp aOperation = EHuiOpSetText);

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C CHuiTextVisual& TextVisual() const;
    
    /**
     * Returns a reference to the image of the visual.
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
 * THuiValueCommand is a command that operates on a single timed value.
 */    
class THuiValueCommand : public THuiObjectCommand 
    {
public:

    /* Constructor. */

    /**
     * Constructs a new value command.
     *
     * @param aOwner           Owner of the timed value. Allowed to be NULL.
     * @param aValue           Timed value to modify (owned by aOwner).
     * @param aTarget          Target value
     * @param aTransitionTime  Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C THuiValueCommand(TAny* aOwner, THuiTimedValue& aValue, 
                              TReal32 aTarget, TInt aTransitionTime = 0) __SOFTFP; 
    
    /**
     * Constructs a new value command. This constructor takes an operation 
     * parameter in addition to the target and timing values.
     *
     * @param aOwner      Owner of the timed value. Allowed to be NULL.
     * @param aValue      Timed value to modify (owned by aOwner).
     * @param aOperation  Operation to perform.
     * @param aTarget     Target value
     * @param aTiming     Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C THuiValueCommand(TAny* aOwner, THuiTimedValue& aValue, 
                              THuiOp aOperation = EHuiOpSet,
                              TReal32 aTarget = 0.0, TReal32 aTiming = 0.0) __SOFTFP;
                             
    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the visual that the command operates on.
     */
    IMPORT_C THuiTimedValue& TimedValue() const;
    
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
    THuiTimedValue& iTimedValue;

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
 * THuiPointCommand is a command that operates on a timed point.
 */    
class THuiPointCommand : public THuiObjectCommand 
    {
public:

    /* Constructor. */

    /**
     * Constructs a new timed point command.
     *
     * @param aOwner      Owner of the timed point. Allowed to be NULL.
     * @param aValue      Timed point to modify.
     * @param aPoint      Point to execute the command on.
     * @param aOperation  Operation to perform.
     */
    IMPORT_C THuiPointCommand(TAny* aOwner, THuiTimedPoint& aPoint, 
                              THuiOp aOperation);

    /**
     * Constructs a new timed point command.
     *
     * @param aOwner           Owner of the timed value. Allowed to be NULL.
     * @param aPoint           Point to execute the command on.
     * @param aValue           Timed value to modify (owned by aOwner).
     * @param aTarget          Target value
     * @param aTransitionTime  Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C THuiPointCommand(TAny* aOwner, THuiTimedPoint& aPoint, 
                              THuiRealPoint aTarget, TInt aTransitionTime = 0) __SOFTFP; 
    
    /**
     * Constructs a new timed point command. This constructor takes an operation 
     * parameter in addition to the target and timing values.
     *
     * @param aOwner      Owner of the timed value. Allowed to be NULL.
     * @param aPoint      Point to execute the command on.
     * @param aValue      Timed value to modify (owned by aOwner).
     * @param aOperation  Operation to perform.
     * @param aTarget     Target value.
     * @param aTiming     Time (in milliseconds) for the transition to occur.
     */
    IMPORT_C THuiPointCommand(TAny* aOwner, THuiTimedPoint& aPoint, 
                              THuiOp aOperation,
                              THuiRealPoint aTarget, TReal32 aTiming = 0.0) __SOFTFP;
                             
    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;


    /* Methods. */
    
    /**
     * Returns the timed point that the command operates on.
     * @return The point that the command operates on.
     */
    IMPORT_C THuiTimedPoint& TimedPoint() const;
    
    /**
     * Returns the target value.
     * @return The real point that the command operates on.
     */
    IMPORT_C THuiRealPoint Target() const __SOFTFP;
    
    /**
     * Returns the transition time of this point command.
     * @return The transition time in milliseconds.
     */
    IMPORT_C TInt TransitionTime() const;
    
    /**
     * Returns the transition speed of this point command.
     * @return the transition speed. Zero, if not set.
     */
    IMPORT_C TReal32 Speed() const __SOFTFP;

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    /** Timed value being operated on. */
    THuiTimedPoint& iTimedPoint;

    /** Target value. */
    THuiRealPoint iTarget;
    
    /** Transition time in milliseconds. */
    TInt iTransitionTime;
    
    /** Optionally, transition speed in units/second. */
    TReal32 iSpeed;

private:
    TInt iSpare1;
    TInt iSpare2;
    };



/**
 * THuiTransformationCommand is a command that operaters on a transformation.
 */    
class THuiTransformationCommand : public THuiObjectCommand
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
    IMPORT_C THuiTransformationCommand(TAny* aOwner, CHuiTransformation& aTransformation,
                                       THuiOp aOperation = EHuiOpLoadIdentity,
                                       TInt aStep = EStepNone);

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;

    /* Methods. */
    
    /**
     * Returns the transformation that the command operates on.
     */
    IMPORT_C CHuiTransformation& Transformation() const;
    
    /**
     * Returns the transformation step index.
     */
    IMPORT_C TInt Step() const;
    
    /**
     * Sets the X component.
     *
     * @param aValue  Value.
     */
    IMPORT_C void SetX(const THuiTimedValue& aValue);

    /**
     * Returns a reference to the Y component.
     */
    IMPORT_C void SetY(const THuiTimedValue& aValue);

    /**
     * Returns a reference to the Z component.
     */
    IMPORT_C void SetZ(const THuiTimedValue& aValue);
    
    /**
     * Returns a reference to the angle component.
     */
    IMPORT_C void SetAngle(const THuiTimedValue& aValue);

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);   
    
private:

    /** Image of the command. */
    CHuiTransformation& iTransformation;

    /** Step index. */    
    TInt iStep;

    /** X component. Suspended. */
    THuiTimedValue iX;
    
    /** Y component. Suspended. */
    THuiTimedValue iY;
    
    /** Z component. Suspended. */
    THuiTimedValue iZ;
    
    /** Angle component. Suspended. */
    THuiTimedValue iAngle;

private:
    TInt iSpare1;
    TInt iSpare2;
    };


/**
 * THuiMarkerCommand is a special marker command that has no functionality, 
 * but can be entered into the scheduler to mark a specific point in time.
 */
class THuiMarkerCommand : public THuiCommand
    {
public:
    
    /* Constructor. */

    /**
     * Constructs a new marker command.
     *
     * @param aId  Identifier of the marker.
     */
    IMPORT_C THuiMarkerCommand(TInt aId);    


    /* Methods. */

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
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
 * THuiCustomEventCommand is a command that sends a custom event. The command
 * can be sent to any individual MHuiEventHandler derived object or broadcasted to 
 * all CHuiControl derived objects.
 */
class THuiCustomEventCommand : public THuiObjectCommand
    {
public:
    
    /* Constructor. */

    /**
     * Constructs a new custom event command.
     *
     * @param aEventParam  Parameter of the custom event.
     * @param aRecipient   Optional recipient of the event. If NULL then event is broadcasted.
     *
     * @todo  It would be good to be able to specify more information to 
     *        be sent with the custom event, e.g., a pointer. Also check
     *        out THuiEvent.
     */
    IMPORT_C THuiCustomEventCommand(TInt aEventParam, MHuiEventHandler* aRecipient = NULL);    


    /* Methods. */

    /* Implementation of THuiCommand::ExecuteL(). */
    IMPORT_C void ExecuteL(CHuiEnv& aEnv) const;
    
    /* Implementation of THuiCommand::Size(). */
    IMPORT_C TInt Size() const;

    /**
     * Returns the recipient of the event, or NULL if none defined.
     */
    IMPORT_C MHuiEventHandler* Recipient() const;

    /**
     * Determines the custom event parameter of the command.
     *
     * @return  Parameter.
     */     
    IMPORT_C TInt Param() const;

protected:

    IMPORT_C void CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:

    /** Custom event parameter. Application cau use re_interpret_cast to send pointer as param */
    TInt iEventParam;

    TInt iSpare1;
    TInt iSpare2;
    };

#endif  // __HUICOMMAND_H__
