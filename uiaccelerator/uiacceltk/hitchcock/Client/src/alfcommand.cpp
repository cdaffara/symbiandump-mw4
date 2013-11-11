/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the commands
*
*/



#include "alf/alfcommand.h"
#include "alf/alfevent.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alf/alftextvisual.h"
#include "alf/alfeventhandler.h"
#include "alf/alfdisplay.h"
#include "alf/alfroster.h"
#include "alf/alfcontrolgroup.h"
#include "alf/alfimagevisual.h"
#include "alf/alftransformation.h"
#include "alflogger.h"

// ======== MEMBER FUNCTIONS ========


EXPORT_C TAlfCommand::TAlfCommand(TAlfCommandType aType)
        : iType(aType)
    {
    }


EXPORT_C TAlfCommand::~TAlfCommand()
    {
    }


EXPORT_C TAlfCommandType TAlfCommand::Type() const
    {
    return iType;
    }
    
EXPORT_C void TAlfCommand::CommandExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    }


EXPORT_C const TAlfObjectCommand* TAlfCommand::ObjectCommand() const
    {
    return NULL;
    }

// GROUP COMMAND

EXPORT_C TAlfGroupCommand::TAlfGroupCommand(CAlfControlGroup& aGroup, TAlfOp aOperation,
                                            CAlfDisplay* aDisplay)
        : TAlfObjectCommand(EAlfCommandTypeGroup, &aGroup, aOperation),
          iDisplay(aDisplay)
    {
    }


EXPORT_C void TAlfGroupCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    __ALFLOGSTRING1( "TAlfGroupCommand::ExecuteL operation %d", Operation() )
    switch(Operation())
        {
        case EAlfOpShow:
            if(iDisplay)
                {
                iDisplay->Roster().ShowL(ControlGroup());
                }
            break;

        case EAlfOpHide:
            if(iDisplay)
                {
                iDisplay->Roster().Hide(ControlGroup());
                }
            break;

        case EAlfOpEnableInput:
            ControlGroup().SetAcceptInput(ETrue);
            break;

        case EAlfOpDisableInput:
            ControlGroup().SetAcceptInput(EFalse);
            break;

        default:
            __ALFLOGSTRING1( "TAlfGroupCommand::ExecuteL Unknown operation %d", Operation() )
            break;
        }
    }


EXPORT_C TInt TAlfGroupCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CAlfControlGroup& TAlfGroupCommand::ControlGroup() const
    {
    return *(CAlfControlGroup*) Object();
    }


EXPORT_C CAlfDisplay* TAlfGroupCommand::Display() const
    {
    return iDisplay;
    }
    
EXPORT_C void TAlfGroupCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }
    
// CONTROL COMMAND

EXPORT_C TAlfControlCommand::TAlfControlCommand(CAlfControl& aControl, TAlfOp aOperation)
        : TAlfObjectCommand(EAlfCommandTypeControl, &aControl, aOperation)
    {
    }


EXPORT_C void TAlfControlCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    __ALFLOGSTRING1( "TAlfControlCommand::ExecuteL Unknown operation %d", Operation() )
    }


EXPORT_C TInt TAlfControlCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CAlfControl& TAlfControlCommand::Control() const
    {
    return *(CAlfControl*) Object();
    }
    
EXPORT_C void TAlfControlCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// VISUAL COMMAND


EXPORT_C TAlfVisualCommand::TAlfVisualCommand(CAlfVisual& aVisual, TAlfOp aOperation,
                                              TInt aSetFlags, TInt aClearFlags)
        : TAlfObjectCommand(EAlfCommandTypeVisual, &aVisual, aOperation),
          iSetFlags(aSetFlags), iClearFlags(aClearFlags)
    {
    }


EXPORT_C void TAlfVisualCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EAlfOpRemoveAndDestroyAll:
            Visual().RemoveAndDestroyAllD();
            // The object is no longer valid after this.
            break;
        case EAlfOpEnableTransformation:
            Visual().EnableTransformationL(ETrue);
            break;

        case EAlfOpDisableTransformation:
            Visual().EnableTransformationL(EFalse);
            break;

        case EAlfOpModifyFlags:
            if(iSetFlags)
                {
                Visual().SetFlags(iSetFlags);
                }
            if(iClearFlags)
                {
                Visual().ClearFlags(iClearFlags);
                }
            break;

        default:
            __ALFLOGSTRING1( "TAlfVisualCommand::ExecuteL Unknown operation %d", Operation() )
            break;
        }
    }


EXPORT_C TInt TAlfVisualCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CAlfVisual& TAlfVisualCommand::Visual() const
    {
    return *(CAlfVisual*) Object();
    }


EXPORT_C TInt TAlfVisualCommand::FlagsToSet() const
    {
    return iSetFlags;
    }


EXPORT_C TInt TAlfVisualCommand::FlagsToClear() const
    {
    return iClearFlags;
    }
    
EXPORT_C void TAlfVisualCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// IMAGE COMMAND

EXPORT_C TAlfImageCommand::TAlfImageCommand(CAlfImageVisual& aImageVisual,
                                            const TAlfImage& aImage,
                                            TAlfOp aOperation)
        : TAlfObjectCommand(EAlfCommandTypeImage, &aImageVisual, aOperation),
          iImage(aImage)
    {
    }


EXPORT_C void TAlfImageCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EAlfOpSetPrimaryImage:
            ImageVisual().SetImage(iImage);
            break;

        case EAlfOpSetSecondaryImage:
            ImageVisual().SetSecondaryImage(iImage);
            break;

        default:
            __ALFLOGSTRING1( "TAlfImageCommand::ExecuteL Unknown operation %d", Operation() )
            break;
        }
    }


EXPORT_C TInt TAlfImageCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CAlfImageVisual& TAlfImageCommand::ImageVisual() const
    {
    return *(CAlfImageVisual*) Object();
    }


EXPORT_C const TAlfImage& TAlfImageCommand::Image() const
    {
    return iImage;
    }

EXPORT_C void TAlfImageCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

    
// ACTION COMMAND

EXPORT_C TAlfActionCommand::TAlfActionCommand(TInt aId)
        : TAlfCommand(EAlfCommandTypeAction), iId(aId)
    {
    }

EXPORT_C void TAlfActionCommand::ExecuteL(CAlfEnv& aEnv) const
    {
    TInt error = aEnv.ReportAction(*this);
    if(error != KErrNone)
        {
        // Leave with the error code if one of the observers failed.
        User::Leave(error);
        }
    }

EXPORT_C TInt TAlfActionCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C TInt TAlfActionCommand::Id() const
    {
    return iId;
    }

EXPORT_C void TAlfActionCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// OBJECT COMMAND


EXPORT_C TAlfObjectCommand::TAlfObjectCommand(TAlfCommandType aType, TAny* aDataOwnerObject, TAlfOp aOperation)
        : TAlfCommand(aType), iObject(aDataOwnerObject), iOperation(aOperation)
    {
    }

EXPORT_C TInt TAlfObjectCommand::Size() const
    {
    return sizeof(*this);
    }
    
EXPORT_C void TAlfObjectCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    __ALFLOGSTRING1( "TAlfObjectCommand::ExecuteL Unknown operation %d", Operation() )
    }

EXPORT_C TAny* TAlfObjectCommand::Object() const
    {
    return iObject;
    }


EXPORT_C TAlfOp TAlfObjectCommand::Operation() const
    {
    return iOperation;
    }


EXPORT_C const TAlfObjectCommand* TAlfObjectCommand::ObjectCommand() const
    {
    // All object commands can be casted to TAlfObjectCommand.
    return this;
    }
    
EXPORT_C void TAlfObjectCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


// TEXT COMMAND

EXPORT_C TAlfTextCommand::TAlfTextCommand(CAlfTextVisual& aTextVisual, const TDesC& aText,
                                          TAlfOp aOperation)
        : TAlfObjectCommand(EAlfCommandTypeText, &aTextVisual, aOperation),
          iText(aText)
    {
    }

EXPORT_C void TAlfTextCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EAlfOpSetText:
            TextVisual().SetTextL(iText);
            break;

        default:
            __ALFLOGSTRING1( "TAlfTextCommand::ExecuteL Unknown operation %d", Operation() )
            break;
        }
    }


EXPORT_C TInt TAlfTextCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CAlfTextVisual& TAlfTextCommand::TextVisual() const
    {
    return *(CAlfTextVisual*) Object();
    }


EXPORT_C const TDesC& TAlfTextCommand::Text() const
    {
    return iText;
    }

EXPORT_C void TAlfTextCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// VALUE COMMAND

EXPORT_C TAlfValueCommand::TAlfValueCommand(TAny* aObject, TAlfValueCommandTarget aObjectType,
                                            TReal32 aTarget, TInt aTransitionTime) __SOFTFP
        : TAlfObjectCommand(EAlfCommandTypeValue, aObject, EAlfOpSet),
          iTimedValue(aObjectType), iTarget(aTarget), iTransitionTime(aTransitionTime),
          iSpeed(0.0)
    {
    }


EXPORT_C TAlfValueCommand::TAlfValueCommand(TAny* aObject, TAlfValueCommandTarget aObjectType,
                                            TAlfOp aOperation,
                                            TReal32 aTarget, TReal32 aTiming) __SOFTFP
        : TAlfObjectCommand(EAlfCommandTypeValue, aObject, aOperation),
          iTimedValue(aObjectType), iTarget(aTarget), iTransitionTime(0),
          iSpeed(0.0)
    {
    if(aOperation == EAlfOpSetWithSpeed)
        {
        iSpeed = aTiming;
        }
    else
        {
        iTransitionTime = TInt(aTiming);
        }
    }

EXPORT_C void TAlfValueCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    TAlfTimedValue value;
    
    TInt op = Operation();
    if(op >= EAlfOpSetStyle && op < EAlfOpSetStyle + EAlfTimedValueStyleLast)
        {
        value.SetStyle(TAlfInterpolationStyle(op - EAlfOpSetStyle));
        }
    
    switch(Operation())
        {
        case EAlfOpSet:
            {
            value.SetTarget( iTarget, iTransitionTime );
            break;
            }
        case EAlfOpSetWithSpeed:
            {
            value.SetTargetWithSpeed(iTarget, iSpeed);
            break;
            }
        default:
            __ALFLOGSTRING1( "TAlfValueCommand::ExecuteL Unknown operation %d", Operation() )
            break;
        }
        
    switch( TimedValue() )
        {
        case EAlfValueCommandVisualSetOpacity:
            {
            CAlfVisual*visual = (CAlfVisual*) Object();
            visual->SetOpacity( value );
            break;
            }
        default:
            __ALFLOGSTRING1( "TAlfValueCommand::ExecuteL Unknown target %d", TimedValue() )
            break;
        }
    }


EXPORT_C TInt TAlfValueCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C TAlfValueCommandTarget TAlfValueCommand::TimedValue() const
    {
    return iTimedValue;
    }


EXPORT_C TReal32 TAlfValueCommand::Target() const __SOFTFP
    {
    return iTarget;
    }


EXPORT_C TInt TAlfValueCommand::TransitionTime() const
    {
    return iTransitionTime;
    }


EXPORT_C TReal32 TAlfValueCommand::Speed() const __SOFTFP
    {
    return iSpeed;
    }
    
EXPORT_C void TAlfValueCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }
    
// POINT COMMAND

EXPORT_C TAlfPointCommand::TAlfPointCommand(TAny* aObject, TAlfPointCommandTarget aObjectType,
                                            TAlfOp aOperation)
        : TAlfObjectCommand(EAlfCommandTypePoint, aObject, aOperation),
          iTimedPoint(aObjectType), iTransitionTime(0), iSpeed(0.0)
    {
    }


EXPORT_C TAlfPointCommand::TAlfPointCommand(TAny* aObject, TAlfPointCommandTarget aObjectType,
                                            TAlfRealPoint aTarget, TInt aTransitionTime,
                                            TParameterTarget aParameterTarget)
        : TAlfObjectCommand(EAlfCommandTypePoint, aObject, EAlfOpSet),
          iTimedPoint(aObjectType), iTarget(aTarget),
          iTransitionTime(aTransitionTime), iSpeed(0.0), iParameterTarget(aParameterTarget)
    {
    }


EXPORT_C TAlfPointCommand::TAlfPointCommand(TAny* aObject, TAlfPointCommandTarget aObjectType,
                                            TAlfOp aOperation,
                                            TAlfRealPoint aTarget, TReal32 aTiming,
                                            TParameterTarget aParameterTarget) __SOFTFP
        : TAlfObjectCommand(EAlfCommandTypePoint, aObject, aOperation),
          iTimedPoint(aObjectType), iTarget(aTarget),
          iTransitionTime(0), iSpeed(0.0), iParameterTarget(aParameterTarget)
    {
    if(aOperation == EAlfOpSetWithSpeed)
        {
        iSpeed = aTiming;
        }
    else
        {
        iTransitionTime = TInt(aTiming);
        }
    }


EXPORT_C void TAlfPointCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {        
    TAlfTimedPoint point;
    
    // Check for interpolation style change ops.
    TInt op = Operation();
    if(op >= EAlfOpSetStyle && op < EAlfOpSetStyle + EAlfTimedValueStyleLast)
        {
        point.SetStyle(TAlfInterpolationStyle(op - EAlfOpSetStyle));
        }
        
    switch( Operation() )
        {
        case EAlfOpSet:
            point.SetTarget(iTarget, iTransitionTime);
            break;

        case EAlfOpSetWithSpeed:
            point.SetTargetWithSpeed(iTarget, iSpeed);
            break;

        default:
            __ALFLOGSTRING1( "TAlfPointCommand::ExecuteL() - Unknown op %d.", Operation())
            break;
        }
    
    if ( iParameterTarget == EBoth || iParameterTarget == EXOnly )
        {
        point.iX.SetTarget( iTarget.iX, iTransitionTime );
        }
                 
    if ( iParameterTarget == EBoth || iParameterTarget == EYOnly )
        {
        point.iY.SetTarget( iTarget.iY, iTransitionTime );
        }
        
    switch( TimedPoint() )
        {
        case EAlfPointCommandVisualSetPos:
            {
            CAlfVisual* visual = (CAlfVisual*) Object();
            visual->SetPos( point );
            break;
            }
        case EAlfPointCommandVisualSetSize:
            {
            CAlfVisual*visual = (CAlfVisual*) Object();
            visual->SetSize( point );
            break;
            }
        
        default:
            __ALFLOGSTRING1( "TAlfPointCommand::ExecuteL Unknown target %d", TimedPoint() )
            break;
        }    
    }


EXPORT_C TInt TAlfPointCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C TAlfPointCommandTarget TAlfPointCommand::TimedPoint() const
    {
    return iTimedPoint;
    }


EXPORT_C TAlfRealPoint TAlfPointCommand::Target() const
    {
    return iTarget;
    }


EXPORT_C TInt TAlfPointCommand::TransitionTime() const
    {
    return iTransitionTime;
    }


EXPORT_C TReal32 TAlfPointCommand::Speed() const __SOFTFP
    {
    return iSpeed;
    }
    
EXPORT_C void TAlfPointCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// TRANSFORMATION COMMAND

EXPORT_C TAlfTransformationCommand::TAlfTransformationCommand(
    TAny* aOwner, CAlfTransformation& aTransformation, TAlfOp aOperation, TInt aStep)
        : TAlfObjectCommand(EAlfCommandTypeTransformation, aOwner, aOperation),
          iTransformation(aTransformation), iStep(aStep)
    {
    // Set default values for transformation
    switch(aOperation)
        {
        case EAlfOpRotate:
            iX = TAlfTimedValue(0.0f);
            iY = TAlfTimedValue(0.0f);
            iZ = TAlfTimedValue(1.0f);
        break;
        case EAlfOpScale:
            iX = TAlfTimedValue(1.0f);
            iY = TAlfTimedValue(1.0f);
            iZ = TAlfTimedValue(1.0f);
        break;        
        default:
        break;
        }
    }


EXPORT_C void TAlfTransformationCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EAlfOpLoadIdentity:
            iTransformation.LoadIdentity();
            break;

        case EAlfOpTranslate:
            if(iStep != EStepNone)
                {
                CAlfTransformation::TTransform xs = iTransformation.Step(iStep);
                xs.iType = CAlfTransformation::ETypeTranslate;
                xs.iParams[EAlfTransformParamTranslateX] = iX;
                xs.iParams[EAlfTransformParamTranslateY] = iY;
                xs.iParams[EAlfTransformParamTranslateZ] = iZ;
                iTransformation.ReplaceStep(iStep,xs);
                }
            else
                {
                iTransformation.Translate(iX, iY);
                }
            break;

        case EAlfOpScale:
            if(iStep != EStepNone)
                {
                CAlfTransformation::TTransform xs = iTransformation.Step(iStep);
                xs.iType = CAlfTransformation::ETypeScale;
                xs.iParams[EAlfTransformParamScaleX] = iX;
                xs.iParams[EAlfTransformParamScaleY] = iY;
                xs.iParams[EAlfTransformParamScaleZ] = iZ;
                iTransformation.ReplaceStep(iStep,xs);
                }
            else
                {
                iTransformation.Scale(iX, iY, iZ);
                }
            break;

        case EAlfOpRotate:
            if(iStep != EStepNone)
                {
                CAlfTransformation::TTransform xs = iTransformation.Step(iStep);
                xs.iType = CAlfTransformation::ETypeRotate;
                xs.iParams[EAlfTransformParamRotateAngle] = iAngle;
                xs.iParams[EAlfTransformParamRotateAxisX] = iX;
                xs.iParams[EAlfTransformParamRotateAxisY] = iY;
                xs.iParams[EAlfTransformParamRotateAxisZ] = iZ;
                iTransformation.ReplaceStep(iStep,xs);
                }
            else
                {
                TAlfTimedValue unsuspended;
                unsuspended = iX;
                TReal32 x = unsuspended.ValueNow();
                unsuspended = iY;
                TReal32 y = unsuspended.ValueNow();
                unsuspended = iZ;
                TReal32 z = unsuspended.ValueNow();
                iTransformation.Rotate(iAngle, x, y, z);
                }
            break;
            
        case EAlfOpReplaceStep:
            {
            if(iStep != EStepNone)
                {
                CAlfTransformation::TTransform xs = iTransformation.Step(iStep);
                xs.iParams[EAlfTransformParamRotateAngle] = iAngle;
                xs.iParams[EAlfTransformParamRotateAxisX] = iX;
                xs.iParams[EAlfTransformParamRotateAxisY] = iY;
                xs.iParams[EAlfTransformParamRotateAxisZ] = iZ;
                iTransformation.ReplaceStep(iStep,xs);
                }
            break;
            }

        default:
            __ALFLOGSTRING1( "TAlfTransformationCommand::ExecuteL Unknown operation %d", Operation() )
            break;
        }
    }


EXPORT_C TInt TAlfTransformationCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CAlfTransformation& TAlfTransformationCommand::Transformation() const
    {
    return iTransformation;
    }


EXPORT_C TInt TAlfTransformationCommand::Step() const
    {
    return iStep;
    }


EXPORT_C void TAlfTransformationCommand::SetX(const TAlfTimedValue& aValue)
    {
    iX = aValue;
    //iX.Suspend();
    }


EXPORT_C void TAlfTransformationCommand::SetY(const TAlfTimedValue& aValue)
    {
    iY = aValue;
    //iY.Suspend();
    }


EXPORT_C void TAlfTransformationCommand::SetZ(const TAlfTimedValue& aValue)
    {
    iZ = aValue;
    //iZ.Suspend();
    }


EXPORT_C void TAlfTransformationCommand::SetAngle(const TAlfTimedValue& aValue)
    {
    iAngle = aValue;
    //iAngle.Suspend();
    }
    
EXPORT_C void TAlfTransformationCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// MARKER COMMAND

EXPORT_C TAlfMarkerCommand::TAlfMarkerCommand(TInt aId)
        : TAlfCommand(TAlfCommandType(EAlfCommandTypeMarkerFirst + aId))
    {
    }


EXPORT_C void TAlfMarkerCommand::ExecuteL(CAlfEnv& /*aEnv*/) const
    {
    __ALFLOGSTRING1( "TAlfMarkerCommand::ExecuteL - Marker %d reached.", Id() )
    }


EXPORT_C TInt TAlfMarkerCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C TInt TAlfMarkerCommand::Id() const
    {
    return Type() - EAlfCommandTypeMarkerFirst;
    }

EXPORT_C void TAlfMarkerCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

// CUSTOM EVENT

EXPORT_C TAlfCustomEventCommand::TAlfCustomEventCommand(
    TInt aEventParam, MAlfEventHandler* aRecipient, TInt aEventData)
        : TAlfObjectCommand(EAlfCommandTypeCustomEvent, aRecipient, EAlfOpNone),
          iEventParam(aEventParam),
          iEventData(aEventData)
    {
    }
 

EXPORT_C void TAlfCustomEventCommand::ExecuteL(CAlfEnv& aEnv) const
    {
    TAlfEvent customEvent(iEventParam, iEventData);

    if(Recipient())
        {
        Recipient()->OfferEventL(customEvent);
        }
    else
        {
        aEnv.BroadcastEventL(customEvent);
        }
    }


EXPORT_C TInt TAlfCustomEventCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C MAlfEventHandler* TAlfCustomEventCommand::Recipient() const
    {
    return (MAlfEventHandler*) Object();
    }

EXPORT_C TInt TAlfCustomEventCommand::Param() const
    {
    return iEventParam;
    }
    
EXPORT_C TInt TAlfCustomEventCommand::EventData() const
    {
    return iEventData;
    }

EXPORT_C void TAlfCustomEventCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    TAlfObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

