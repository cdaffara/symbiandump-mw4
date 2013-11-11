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
* Description:   Implementation of THuiCommand and derived command classes.
*
*/



#include "uiacceltk/HuiCommand.h"  // Class definition
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiScheduler.h"
#include "uiacceltk/huieventhandler.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiEvent.h"
#include "uiacceltk/HuiControlGroup.h"
#include "uiacceltk/HuiVisuals.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiUtil.h"


THuiCommand::THuiCommand(THuiCommandType aType)
        : iType(aType), iSenderType(EHuiCommandObjectTypeNone), iSender(NULL)
    {
    }


EXPORT_C THuiCommand::~THuiCommand()
    {
    iSender = NULL;
    }


EXPORT_C void THuiCommand::SetSender(THuiCommandObjectType aType, TAny* aSender)
    {
    iSenderType = aType;
    iSender = aSender;
    }


EXPORT_C THuiCommandType THuiCommand::Type() const
    {
    return iType;
    }


EXPORT_C const THuiObjectCommand* THuiCommand::ObjectCommand() const
    {
    return NULL;
    }


EXPORT_C THuiCommandObjectType THuiCommand::SenderType() const
    {
    return iSenderType;
    }


EXPORT_C TAny* THuiCommand::Sender() const
    {
    return iSender;
    }


EXPORT_C CHuiVisual* THuiCommand::SenderVisual() const
    {
    if(iSenderType == EHuiCommandObjectTypeVisual)
        {
        return reinterpret_cast<CHuiVisual*>(iSender);
        }
    return NULL;
    }


EXPORT_C CHuiControl* THuiCommand::SenderControl() const
    {
    if(iSenderType == EHuiCommandObjectTypeControl)
        {
        return reinterpret_cast<CHuiControl*>(iSender);
        }
    return NULL;
    }


EXPORT_C CHuiControlGroup* THuiCommand::SenderControlGroup() const
    {
    if(iSenderType == EHuiCommandObjectTypeControlGroup)
        {
        return reinterpret_cast<CHuiControlGroup*>(iSender);
        }
    return NULL;
    }

EXPORT_C void THuiCommand::CommandExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    }

EXPORT_C THuiActionCommand::THuiActionCommand(TInt aId)
        : THuiCommand(EHuiCommandTypeAction), iId(aId)
    {
    }


EXPORT_C void THuiActionCommand::ExecuteL(CHuiEnv& aEnv) const
    {
    TInt error = aEnv.ReportAction(*this);
    if(error != KErrNone)
        {
        // Leave with the error code if one of the observers failed.
        User::Leave(error);
        }
    }


EXPORT_C TInt THuiActionCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C TInt THuiActionCommand::Id() const
    {
    return iId;
    }
    
EXPORT_C void THuiActionCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


THuiObjectCommand::THuiObjectCommand(THuiCommandType aType, TAny* aDataOwnerObject, THuiOp aOperation)
        : THuiCommand(aType), iObject(aDataOwnerObject), iOperation(aOperation)
    {
    }


EXPORT_C TAny* THuiObjectCommand::Object() const
    {
    return iObject;
    }


EXPORT_C THuiOp THuiObjectCommand::Operation() const
    {
    return iOperation;
    }
    
EXPORT_C TInt THuiObjectCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C const THuiObjectCommand* THuiObjectCommand::ObjectCommand() const
    {
    // All object commands can be casted to THuiObjectCommand.
    return this;
    }
    
EXPORT_C void THuiObjectCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    HUI_DEBUG1(_L("THuiObjectCommand::ExecuteL() - Unknown op %i."), Operation());
    }  
    
EXPORT_C void THuiObjectCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiGroupCommand::THuiGroupCommand(CHuiControlGroup& aGroup, THuiOp aOperation,
                                            CHuiDisplay* aDisplay)
        : THuiObjectCommand(EHuiCommandTypeGroup, &aGroup, aOperation),
          iDisplay(aDisplay)
    {
    }


EXPORT_C void THuiGroupCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EHuiOpShow:
            if(iDisplay)
                {
                iDisplay->Roster().ShowL(ControlGroup());
                }
            break;

        case EHuiOpHide:
            if(iDisplay)
                {
                iDisplay->Roster().Hide(ControlGroup());
                }
            break;

        case EHuiOpEnableInput:
            ControlGroup().SetAcceptInput(ETrue);
            break;

        case EHuiOpDisableInput:
            ControlGroup().SetAcceptInput(EFalse);
            break;

        default:
            HUI_DEBUG1(_L("THuiGroupCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiGroupCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CHuiControlGroup& THuiGroupCommand::ControlGroup() const
    {
    return *static_cast<CHuiControlGroup*>(Object());
    }


EXPORT_C CHuiDisplay* THuiGroupCommand::Display() const
    {
    return iDisplay;
    }
    
EXPORT_C void THuiGroupCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiControlCommand::THuiControlCommand(CHuiControl& aControl, THuiOp aOperation)
        : THuiObjectCommand(EHuiCommandTypeControl, &aControl, aOperation)
    {
    }


EXPORT_C void THuiControlCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    HUI_DEBUG1(_L("THuiControlCommand::ExecuteL() - Unknown op %i."), Operation());
    }


EXPORT_C TInt THuiControlCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CHuiControl& THuiControlCommand::Control() const
    {
    return *static_cast<CHuiControl*>(Object());
    }
    
EXPORT_C void THuiControlCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiVisualCommand::THuiVisualCommand(CHuiVisual& aVisual, THuiOp aOperation,
                                              TInt aSetFlags, TInt aClearFlags)
        : THuiObjectCommand(EHuiCommandTypeVisual, &aVisual, aOperation),
          iSetFlags(aSetFlags), iClearFlags(aClearFlags)
    {
    }


EXPORT_C void THuiVisualCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EHuiOpRemoveAndDestroyAll:
            Visual().RemoveAndDestroyAllD();
            // The object is no longer valid after this.
            break;
        case EHuiOpEnableTransformation:
            HUI_DEBUG(_L("THuiVisualCommand::ExecuteL() -- Enable transformation."));
            Visual().EnableTransformationL(ETrue);
            break;

        case EHuiOpDisableTransformation:
            HUI_DEBUG(_L("THuiVisualCommand::ExecuteL() -- Disable transformation."));
            Visual().EnableTransformationL(EFalse);
            break;

        case EHuiOpModifyFlags:
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
            HUI_DEBUG1(_L("THuiVisualCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiVisualCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CHuiVisual& THuiVisualCommand::Visual() const
    {
    return *static_cast<CHuiVisual*>(Object());
    }


EXPORT_C TInt THuiVisualCommand::FlagsToSet() const
    {
    return iSetFlags;
    }


EXPORT_C TInt THuiVisualCommand::FlagsToClear() const
    {
    return iClearFlags;
    }
    
EXPORT_C void THuiVisualCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiImageCommand::THuiImageCommand(CHuiImageVisual& aImageVisual,
                                            const THuiImage& aImage,
                                            THuiOp aOperation)
        : THuiObjectCommand(EHuiCommandTypeImage, &aImageVisual, aOperation),
          iImage(aImage)
    {
    }


EXPORT_C void THuiImageCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EHuiOpSetPrimaryImage:
            ImageVisual().SetImage(iImage);
            break;

        case EHuiOpSetSecondaryImage:
            ImageVisual().SetSecondaryImage(iImage);
            break;

        default:
            HUI_DEBUG1(_L("THuiImageCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiImageCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CHuiImageVisual& THuiImageCommand::ImageVisual() const
    {
    return *static_cast<CHuiImageVisual*>( Object() );
    }


EXPORT_C const THuiImage& THuiImageCommand::Image() const
    {
    return iImage;
    }
    
EXPORT_C void THuiImageCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiTextCommand::THuiTextCommand(CHuiTextVisual& aTextVisual, const TDesC& aText,
                                          THuiOp aOperation)
        : THuiObjectCommand(EHuiCommandTypeText, &aTextVisual, aOperation),
          iText(aText)
    {
    }


EXPORT_C void THuiTextCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EHuiOpSetText:
            TextVisual().SetTextL(iText);
            break;

        default:
            HUI_DEBUG1(_L("THuiTextCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiTextCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CHuiTextVisual& THuiTextCommand::TextVisual() const
    {
    return *static_cast<CHuiTextVisual*>( Object() );
    }


EXPORT_C const TDesC& THuiTextCommand::Text() const
    {
    return iText;
    }
    
EXPORT_C void THuiTextCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiValueCommand::THuiValueCommand(TAny* aOwner, THuiTimedValue& aValue,
                                            TReal32 aTarget, TInt aTransitionTime) __SOFTFP
        : THuiObjectCommand(EHuiCommandTypeValue, aOwner, EHuiOpSet),
          iTimedValue(aValue), iTarget(aTarget), iTransitionTime(aTransitionTime),
          iSpeed(0.0)
    {
    }


EXPORT_C THuiValueCommand::THuiValueCommand(TAny* aOwner, THuiTimedValue& aValue,
                                            THuiOp aOperation,
                                            TReal32 aTarget, TReal32 aTiming) __SOFTFP
        : THuiObjectCommand(EHuiCommandTypeValue, aOwner, aOperation),
          iTimedValue(aValue), iTarget(aTarget), iTransitionTime(0),
          iSpeed(0.0)
    {
    if(aOperation == EHuiOpSetWithSpeed)
        {
        iSpeed = aTiming;
        }
    else
        {
        iTransitionTime = TInt(aTiming);
        }
    }


EXPORT_C void THuiValueCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    // Check for interpolation style change ops.
    TInt op = Operation();
    if(op >= EHuiOpSetStyle && op < EHuiOpSetStyle + EHuiTimedValueStyleLast)
        {
        iTimedValue.SetStyle(THuiInterpolationStyle(op - EHuiOpSetStyle));
        }

    switch(Operation())
        {
        case EHuiOpSet:
            iTimedValue.Set(iTarget, iTransitionTime);
            break;

        case EHuiOpSetWithSpeed:
            iTimedValue.SetWithSpeed(iTarget, iSpeed);
            break;

        default:
            HUI_DEBUG1(_L("THuiValueCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiValueCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C THuiTimedValue& THuiValueCommand::TimedValue() const
    {
    return iTimedValue;
    }


EXPORT_C TReal32 THuiValueCommand::Target() const __SOFTFP
    {
    return iTarget;
    }


EXPORT_C TInt THuiValueCommand::TransitionTime() const
    {
    return iTransitionTime;
    }


EXPORT_C TReal32 THuiValueCommand::Speed() const __SOFTFP
    {
    return iSpeed;
    }
    
EXPORT_C void THuiValueCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiPointCommand::THuiPointCommand(TAny* aOwner, THuiTimedPoint& aPoint,
                                            THuiOp aOperation)
        : THuiObjectCommand(EHuiCommandTypePoint, aOwner, aOperation),
          iTimedPoint(aPoint), iTransitionTime(0), iSpeed(0.0)
    {
    }


EXPORT_C THuiPointCommand::THuiPointCommand(TAny* aOwner, THuiTimedPoint& aPoint,
                                            THuiRealPoint aTarget, TInt aTransitionTime) __SOFTFP
        : THuiObjectCommand(EHuiCommandTypePoint, aOwner, EHuiOpSet),
          iTimedPoint(aPoint), iTarget(aTarget),
          iTransitionTime(aTransitionTime), iSpeed(0.0)
    {
    }


EXPORT_C THuiPointCommand::THuiPointCommand(TAny* aOwner, THuiTimedPoint& aPoint,
                                            THuiOp aOperation,
                                            THuiRealPoint aTarget, TReal32 aTiming) __SOFTFP
        : THuiObjectCommand(EHuiCommandTypePoint, aOwner, aOperation),
          iTimedPoint(aPoint), iTarget(aTarget),
          iTransitionTime(0), iSpeed(0.0)
    {
    if(aOperation == EHuiOpSetWithSpeed)
        {
        iSpeed = aTiming;
        }
    else
        {
        iTransitionTime = TInt(aTiming);
        }
    }


EXPORT_C void THuiPointCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    // Check for interpolation style change ops.
    TInt op = Operation();
    if(op >= EHuiOpSetStyle && op < EHuiOpSetStyle + EHuiTimedValueStyleLast)
        {
        iTimedPoint.SetStyle(THuiInterpolationStyle(op - EHuiOpSetStyle));
        }

    switch(Operation())
        {
        case EHuiOpSet:
            iTimedPoint.Set(iTarget, iTransitionTime);
            break;

        case EHuiOpSetWithSpeed:
            iTimedPoint.SetWithSpeed(iTarget, iSpeed);
            break;

        default:
            HUI_DEBUG1(_L("THuiPointCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiPointCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C THuiTimedPoint& THuiPointCommand::TimedPoint() const
    {
    return iTimedPoint;
    }


EXPORT_C THuiRealPoint THuiPointCommand::Target() const __SOFTFP
    {
    return iTarget;
    }


EXPORT_C TInt THuiPointCommand::TransitionTime() const
    {
    return iTransitionTime;
    }


EXPORT_C TReal32 THuiPointCommand::Speed() const __SOFTFP
    {
    return iSpeed;
    }

EXPORT_C void THuiPointCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiTransformationCommand::THuiTransformationCommand(
    TAny* aOwner, CHuiTransformation& aTransformation, THuiOp aOperation, TInt aStep)
        : THuiObjectCommand(EHuiCommandTypeTransformation, aOwner, aOperation),
          iTransformation(aTransformation), iStep(aStep)
    {
    }


EXPORT_C void THuiTransformationCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    switch(Operation())
        {
        case EHuiOpLoadIdentity:
            iTransformation.LoadIdentity();
            break;

        case EHuiOpTranslate:
            if(iStep != EStepNone)
                {
                CHuiTransformation::TTransform& xs = iTransformation.Step(iStep);
                xs.iType = CHuiTransformation::ETypeTranslate;
                xs.iParams[EHuiTransformParamTranslateX] = iX;
                xs.iParams[EHuiTransformParamTranslateY] = iY;
                xs.iParams[EHuiTransformParamTranslateZ] = iZ;
                }
            else
                {
                iTransformation.Translate(iX, iY);
                }
            break;

        case EHuiOpScale:
            if(iStep != EStepNone)
                {
                CHuiTransformation::TTransform& xs = iTransformation.Step(iStep);
                xs.iType = CHuiTransformation::ETypeScale;
                xs.iParams[EHuiTransformParamScaleX] = iX;
                xs.iParams[EHuiTransformParamScaleY] = iY;
                xs.iParams[EHuiTransformParamScaleZ] = iZ;
                }
            else
                {
                iTransformation.Scale(iX, iY, iZ);
                }
            break;

        case EHuiOpRotate:
            if(iStep != EStepNone)
                {
                CHuiTransformation::TTransform& xs = iTransformation.Step(iStep);
                xs.iType = CHuiTransformation::ETypeRotate;
                xs.iParams[EHuiTransformParamRotateAngle] = iAngle;
                xs.iParams[EHuiTransformParamRotateAxisX] = iX;
                xs.iParams[EHuiTransformParamRotateAxisY] = iY;
                xs.iParams[EHuiTransformParamRotateAxisZ] = iZ;
                }
            else
                {
                THuiTimedValue unsuspended;
                unsuspended = iX;
                TReal32 x = unsuspended.Now();
                unsuspended = iY;
                TReal32 y = unsuspended.Now();
                unsuspended = iZ;
                TReal32 z = unsuspended.Now();
                iTransformation.Rotate(iAngle, x, y, z);
                }
            break;

        default:
            HUI_DEBUG1(_L("THuiTransformationCommand::ExecuteL() - Unknown op %i."), Operation());
            break;
        }
    }


EXPORT_C TInt THuiTransformationCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C CHuiTransformation& THuiTransformationCommand::Transformation() const
    {
    return iTransformation;
    }


EXPORT_C TInt THuiTransformationCommand::Step() const
    {
    return iStep;
    }


EXPORT_C void THuiTransformationCommand::SetX(const THuiTimedValue& aValue)
    {
    iX = aValue;
    iX.Suspend();
    }


EXPORT_C void THuiTransformationCommand::SetY(const THuiTimedValue& aValue)
    {
    iY = aValue;
    iY.Suspend();
    }


EXPORT_C void THuiTransformationCommand::SetZ(const THuiTimedValue& aValue)
    {
    iZ = aValue;
    iZ.Suspend();
    }


EXPORT_C void THuiTransformationCommand::SetAngle(const THuiTimedValue& aValue)
    {
    iAngle = aValue;
    iAngle.Suspend();
    }
    
EXPORT_C void THuiTransformationCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiMarkerCommand::THuiMarkerCommand(TInt aId)
        : THuiCommand(THuiCommandType(EHuiCommandTypeMarkerFirst + aId))
    {
    }


EXPORT_C void THuiMarkerCommand::ExecuteL(CHuiEnv& /*aEnv*/) const
    {
    HUI_DEBUG1(_L("THuiMarkerCommand::ExecuteL() - Marker %i reached."), Id());
    }


EXPORT_C TInt THuiMarkerCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C TInt THuiMarkerCommand::Id() const
    {
    return Type() - EHuiCommandTypeMarkerFirst;
    }

EXPORT_C void THuiMarkerCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }


EXPORT_C THuiCustomEventCommand::THuiCustomEventCommand(TInt aEventParam, MHuiEventHandler* aRecipient)
        : THuiObjectCommand(EHuiCommandTypeCustomEvent, aRecipient, EHuiOpNone),
          iEventParam(aEventParam)
    {
    }


EXPORT_C void THuiCustomEventCommand::ExecuteL(CHuiEnv& aEnv) const
    {
    THuiEvent customEvent(iEventParam);

    if(Recipient())
        {
        Recipient()->OfferEventL(customEvent);
        }
    else
        {
        aEnv.BroadcastEventL(customEvent);
        }
    }


EXPORT_C TInt THuiCustomEventCommand::Size() const
    {
    return sizeof(*this);
    }


EXPORT_C MHuiEventHandler* THuiCustomEventCommand::Recipient() const
    {
    return static_cast<MHuiEventHandler*>( Object() );
    }


EXPORT_C TInt THuiCustomEventCommand::Param() const
    {
    return iEventParam;
    }
    
EXPORT_C void THuiCustomEventCommand::CommandExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    THuiObjectCommand::CommandExtension( aExtensionUid, aExtensionParams );
    }

