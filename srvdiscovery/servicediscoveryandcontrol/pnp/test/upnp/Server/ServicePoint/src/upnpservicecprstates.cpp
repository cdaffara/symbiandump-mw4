// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

//Local Includes
#include "upnpservicecprstates.h"
#include "upnpstatemachine.h"

using namespace UPnPServiceCprStates;

//-=========================================================
//
//State Forks
//
//-=========================================================
DEFINE_SMELEMENT ( TServiceOrDeviceTag, NetStateMachine::MStateFork, TContext )
TInt TServiceOrDeviceTag::TransitionTag ( )
	{
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TRegisterService> ( )	
		|| iContext.iMessage.IsMessage<TUpnpMessage::TUnregisterService> ( ) )
		{
		return UPnPStateMachine::KServiceTag;
		}
	else if  ( iContext.iMessage.IsMessage<TUpnpMessage::TRegisterDevice> ( )
		|| iContext.iMessage.IsMessage<TUpnpMessage::TUnregisterDevice> ( ) )
		{
		return UPnPStateMachine::KDeviceTag;
		}
	return MeshMachine::KErrorTag;	
	}


DEFINE_SMELEMENT ( TRootRegisteredOrNoTag, NetStateMachine::MStateFork, TContext )
TInt TRootRegisteredOrNoTag::TransitionTag ( )
	{
	return iContext.Node ( ).IsRootDeviceRegistered ( ) ? UPnPStateMachine::KRootRegisteredTag : MeshMachine::KNoTag;
	}
	
//-=========================================================
//
//Transitions
//
//-=========================================================
DEFINE_SMELEMENT ( TPerformServiceOrDeviceValidationAndSendResponse, NetStateMachine::MStateTransition, TContext )
void TPerformServiceOrDeviceValidationAndSendResponse::DoL ( )
	{
	TInt err = KErrNone;
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TServiceJoinRequest> ( ) )
		{
		TUpnpMessage::TServiceJoinRequest& joinMsg = message_cast<TUpnpMessage::TServiceJoinRequest>( iContext.iMessage );
		
		CUPnPServiceRegisterParamSet* paramSet = static_cast<CUPnPServiceRegisterParamSet*> ( joinMsg.iParamSet );
		CUPnPRootDeviceInfoContainer& container = static_cast <CUPnPRootDeviceInfoContainer&> ( iContext.Node ( ).GetRootDeviceInfoContainer ( ) );
		if(!paramSet->DeviceUid( ).Size())
			{
			paramSet->SetDeviceUidL(container.GetRootdeviceUid());
			}
		
		err = container.PerformValidation ( paramSet->DeviceUid( ), paramSet->Uri( ) );
		}
	else if ( iContext.iMessage.IsMessage<TUpnpMessage::TDeviceJoinRequest> ( ) )
		{
		TUpnpMessage::TDeviceJoinRequest& joinMsg = message_cast<TUpnpMessage::TDeviceJoinRequest>( iContext.iMessage );
		
		CUPnPDeviceRegisterParamSet* paramSet = static_cast<CUPnPDeviceRegisterParamSet*> ( joinMsg.iParamSet );
		CUPnPRootDeviceInfoContainer& container = static_cast <CUPnPRootDeviceInfoContainer&> ( iContext.Node ( ).GetRootDeviceInfoContainer ( ) );
		if(!paramSet->ParentDeviceUid( ).Size())
			{
			paramSet->SetParentDeviceUidL(container.GetRootdeviceUid());
			}
		if ( container.GetDeviceByUuid ( paramSet->ParentDeviceUid( ) )
			&& !( container.GetDeviceByUuid ( paramSet->UDN( ) ) ) )
			{
			err = KErrNone;
			}
		else
			{
			err = KErrAlreadyExists;
			}
		}
	
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), address_cast<TNodeCtxId> ( iContext.iSender ), TUpnpMessage::TServiceValidated ( err ).CRef () );
	}


DEFINE_SMELEMENT ( TAppendServiceInfo, NetStateMachine::MStateTransition, TContext )
void TAppendServiceInfo::DoL ( )
	{
	TUpnpMessage::TRegisterService& msg = message_cast<TUpnpMessage::TRegisterService>( iContext.iMessage );
	
	CUPnPServiceRegisterParamSet* paramSet = static_cast<CUPnPServiceRegisterParamSet*> ( msg.iParamSet );
	CUPnPServiceInfo* serviceInfo = static_cast<CUPnPServiceInfo*> ( msg.iPtr );
	
	iContext.Node ( ).GetRootDeviceInfoContainer ( ).AppendServiceInfoL ( paramSet->DeviceUid(), serviceInfo );
	}


DEFINE_SMELEMENT ( TAppendDeviceInfo, NetStateMachine::MStateTransition, TContext )
void TAppendDeviceInfo::DoL ( )
	{
	TUpnpMessage::TRegisterDevice& msg = message_cast<TUpnpMessage::TRegisterDevice>( iContext.iMessage );
	
	CUPnPDeviceRegisterParamSet* paramSet = static_cast<CUPnPDeviceRegisterParamSet*> ( msg.iParamSet );
	CUPnPDevice* device = static_cast<CUPnPDevice*> ( msg.iPtr );
	
	iContext.Node ( ).GetRootDeviceInfoContainer ( ).AppendEmbeddedDeviceL ( paramSet->ParentDeviceUid( ), device );
	}
	
	
DEFINE_SMELEMENT ( TDeleteServiceInfo, NetStateMachine::MStateTransition, TContext )
void TDeleteServiceInfo::DoL ( )
	{
	TUpnpMessage::TUnregisterService& msg = message_cast<TUpnpMessage::TUnregisterService>( iContext.iMessage );
	TPtr8 scpdUrl ( const_cast<TUint8*> ( msg.iPtr ), msg.iSize, msg.iSize );
	
	iContext.Node ( ).GetRootDeviceInfoContainer ( ).DeleteServiceInfoL ( scpdUrl );
	}

	
DEFINE_SMELEMENT ( TDeleteDeviceInfo, NetStateMachine::MStateTransition, TContext )
void TDeleteDeviceInfo::DoL ( )
	{
	TUpnpMessage::TUnregisterDevice& msg = message_cast<TUpnpMessage::TUnregisterDevice>( iContext.iMessage );
	TPtr8 deviceUuid ( const_cast<TUint8*> ( msg.iPtr ), msg.iSize, msg.iSize );
	
	iContext.Node ( ).GetRootDeviceInfoContainer ( ).DeleteDeviceInfoL ( deviceUuid );	
	}
