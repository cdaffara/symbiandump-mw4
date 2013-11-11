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
// @file
// @internalComponent
// 
//

#include <comms-infras/ss_msgintercept.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_logext.h>
#include <inetprottextutils.h>
#include <rmemchunk.h>

#include "upnpservicescpr.h"
#include "upnpservicescpractivities.h"
#include "upnpsubconnevents.h"
#include "upnpstatemachine.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnpcpr.h"

__FLOG_STMT(_LIT8(KComponent,"UPnPSPScpr");)


using namespace ESock;


CUPnPServiceSubConnectionProvider* CUPnPServiceSubConnectionProvider::NewL ( ESock::CSubConnectionProviderFactoryBase& aFactory )
	{
	CUPnPServiceSubConnectionProvider* provider = new ( ELeave ) CUPnPServiceSubConnectionProvider ( aFactory, UPnPServiceSCprActivities::activityMap::Self ( ) );
	CleanupStack::PushL ( provider );
	provider->ConstructL( );
	CleanupStack::Pop ( provider );
	return provider;
	}

CUPnPServiceSubConnectionProvider::~CUPnPServiceSubConnectionProvider ( )
	{
	iDescUrl.Close ( );
	iControlUrl.Close ( );
	iEventUrl.Close ( );	
	iResponseBundle.Close ();
		
	delete iSubscribeInfoCont;		
	delete iPublishInfo;				
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("Destroyed CUPnPServiceSubConnectionProvider")));	
	LOG_NODE_DESTROY ( KSubsysSPScpr, CUPnPServiceSubConnectionProvider );
	}

CUPnPServiceSubConnectionProvider::CUPnPServiceSubConnectionProvider ( ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap )
	: CUPnPSubConnectionProvider ( aFactory, aActivityMap ), iDevice ( EFalse ), iClosing ( EFalse )
	{
	LOG_NODE_CREATE ( KSubsysSPScpr, CUPnPServiceSubConnectionProvider );
	}

void CUPnPServiceSubConnectionProvider::ConstructL ( )
	{
	CCoreSubConnectionProvider::ConstructL ( );	
	iResponseBundle.CreateL ();	
	iResponseBundle.Open ();
	
	iRequestBundle.CreateL ();	

	iSubscribeInfoCont = CUPnPSubscribeInfoContainer::NewL ( );
	// set intial event data
	iSubscribeInfoCont->SetEventDataL ( KNullDesC8 ( ) );
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("Created CUPnPServiceSubConnectionProvider")));
	}

void CUPnPServiceSubConnectionProvider::SetPublishInfo ( CUPnPPublishInfoElement* aPublishInfo )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SetPublishInfo")));
	iPublishInfo = aPublishInfo;	
	}

CUPnPPublishInfoElement* CUPnPServiceSubConnectionProvider::PublishInfo ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::PublishInfo")));
	return iPublishInfo;
	}

void CUPnPServiceSubConnectionProvider::SetKeyL ( const TDesC8& aKey )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SetKeyL")));
	iDescUrl.Close ( );
	CreatePropertyUrlL ( aKey, iDescUrl );
	}
	
const TDesC8& CUPnPServiceSubConnectionProvider::Key ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::Key")));
	return iDescUrl;
	}

void CUPnPServiceSubConnectionProvider::SetEventUrlL ( const TDesC8& aEventUrl )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SetEventUrlL")));
	iEventUrl.Close ( );
	CreatePropertyUrlL ( aEventUrl, iEventUrl );
	}
	
const TDesC8& CUPnPServiceSubConnectionProvider::EventUrl ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::EventUrl")));
	return iEventUrl;
	}

void CUPnPServiceSubConnectionProvider::SetControlUrlL ( const TDesC8& aControlUrl )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SetControlUrlL")));
	iControlUrl.Close ( );
	CreatePropertyUrlL ( aControlUrl, iControlUrl );
	}

void CUPnPServiceSubConnectionProvider::CreatePropertyUrlL ( const TDesC8& aRelativeUrl, RBuf8& aTargetUrl )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::CreatePropertyUrlL")));
	const TDesC8& rootLoc = ConnectionProvider ().RootDeviceLocation ();
	
	aTargetUrl.CreateMaxL ( rootLoc.Length ( ) + aRelativeUrl.Length ( ) );
	aTargetUrl.Copy ( rootLoc );
	aTargetUrl.Append ( aRelativeUrl );	
	}
const TDesC8& CUPnPServiceSubConnectionProvider::ControlUrl ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::ControlUrl")));
	return iControlUrl;
	}

void CUPnPServiceSubConnectionProvider::SetDevice ( const TBool aIsDevice )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SetDevice")));
	iDevice = aIsDevice;
	}

TBool CUPnPServiceSubConnectionProvider::IsDevice ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::IsDevice")));
	return iDevice;
	}

const TDesC8& CUPnPServiceSubConnectionProvider::SCPDData ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SCPDData")));
	return iPublishInfo->ServiceDescription( );	
	}
	
CUPnPSubscribeInfoContainer* CUPnPServiceSubConnectionProvider::GetSubscribeInfoContainer ( )
	{
	return iSubscribeInfoCont;
	}

CUPnPServiceConnectionProvider& CUPnPServiceSubConnectionProvider::ConnectionProvider ( )
	{
	CUPnPConnectionProvider& cprBase = BaseConnectionProvider();
	return static_cast<CUPnPServiceConnectionProvider&>(cprBase);
	}	

void CUPnPServiceSubConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	ESOCK_DEBUG_MESSAGE_INTERCEPT ( aSender, aMessage, aRecipient );	
	TNodeContext<CUPnPServiceSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);	
    CCoreSubConnectionProvider::ReceivedL ( aSender, aRecipient, aMessage );
    User::LeaveIfError ( ctx.iReturn );
	}

void CUPnPServiceSubConnectionProvider::InitiateActivitiesL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateActivitiesL")));
	
	RParameterFamily paramFamily = iRequestBundle.GetFamilyAtIndex (0);
	switch ( paramFamily.Id( ) )
		{
		case EUPnPClearParamSet:
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateActivitiesL - TUPnPParamSetFamily::EUPnPClearParamSet")));
			ClearResponseParamL ( );
			break;
			
		case EUPnPServiceNotificationParamSet:
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateActivitiesL - TUPnPParamSetFamily::EUPnPServiceNotificationParamSet")));
			SendNotifyToRemoteClientsL ( paramFamily );
			break;
		
		case EUPnPServiceRegisterParamSet:
		case EUPnPServiceRefreshParamSet:
		case EUPnPDeviceRegisterParamSetExt:
			InitiateRequestL ( paramFamily );
			break;
			
		default:
			break;
		}
	//finally delete UPnP Clear Family
	iRequestBundle.DeleteFamilyAtIndex (0);		
	}

	
void CUPnPServiceSubConnectionProvider::InitiateRequestL ( RParameterFamily& aReqFamily )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateRequestL")));
	switch ( aReqFamily.Id ( ) )
		{		
		case EUPnPServiceRegisterParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPServiceRegisterParamSet")));
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPServiceRegisterParamSet - Posting message TUpnpMessage::TServiceRegistration")));
			CSubConExtensionParameterSet* paramSet = static_cast<CSubConExtensionParameterSet*> ( aReqFamily.GetParameterSetAtIndex ( 0, RParameterFamily::ERequested ) );
			TUpnpMessage::TServiceRegistration msg ( paramSet );
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( TUpnpMessage::EActivityServiceRegistration, NodeId () ) , msg );			
			aReqFamily.ClearParameterSetPointer( 0, RParameterFamily::ERequested );			
			}
		break;
		
		case EUPnPDeviceRegisterParamSetExt:
			{		
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPDeviceRegisterParamSet")));
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPDeviceRegisterParamSet - Posting message TUpnpMessage::TDeviceRegistration")));
			CUPnPDeviceRegisterParamSetExt* paramSet = static_cast<CUPnPDeviceRegisterParamSetExt*> ( aReqFamily.GetParameterSetAtIndex ( 0, RParameterFamily::ERequested ) );
			
			CUPnPDeviceRegisterParamSet* deviceParams = CopyWithIconInfoL ( paramSet );
			
			TUpnpMessage::TDeviceRegistration msg ( deviceParams );
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( TUpnpMessage::EActivityDeviceRegistration, NodeId () ) , msg );
			}
		break;
		
		case EUPnPServiceRefreshParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPServiceRefreshParamSet")));
			if ( IsDevice ( ) )
				{
				// Publish once for uuid:device-UUID
				DoPublishL ( iPublishInfo->Uuid( ), iPublishInfo->Uuid( ), iPublishInfo->CacheControl( ) );
				}
				
			// Publish once for uuid:device-UUID::urn:domain-name-device:deviceType:v
			// (or) Publish once for uuid:device-UUID::urn:domain-name-service:serviceType:v
			DoPublishL ( iPublishInfo->SearchTarget( ), iPublishInfo->Usn( ), iPublishInfo->CacheControl( ) );					
			}
		break;
		
		default:
		break;
		}
	}
	
void CUPnPServiceSubConnectionProvider::DoPublishL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::DoPublishL")));
	RMemoryAllocator allocator = ConnectionProvider().MemoryAllocator();
	
	RMemChunk locationBuf;
	locationBuf.CreateL ( ConnectionProvider ().RootDeviceLocation (), allocator );
	
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
	CleanupStack::PushL ( item );

	RMemChunk usnBuf;
	usnBuf.CreateL ( aUsn, allocator );
	
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::DoPublishL - Posting message TUpnpMessage::TUPnPPublishAliveRequest")));
	TSsdpInfo ssdpInfo ( aMaxAge, locationBuf, usnBuf, stBuf);
	RClientInterface::OpenPostMessageClose ( NodeId (), UdpClientFlow (), TUpnpMessage::TUPnPPublishAliveRequest ( ssdpInfo ).CRef () );
	CleanupStack::Pop ( ); // item
	}


// this will send notification to the clients
void  CUPnPServiceSubConnectionProvider::SendNotifyToRemoteClientsL ( RParameterFamily& aReqFamily ) 
	{	
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SendNotifyToRemoteClientsL")));
	CUPnPServiceNotificationParamSet* paramSet = static_cast<CUPnPServiceNotificationParamSet* > (aReqFamily.GetParameterSetAtIndex ( 0, RParameterFamily::ERequested ) );
	__ASSERT_DEBUG ( paramSet != NULL, User::Invariant ( ) );
	
	// replace the existing data
	iSubscribeInfoCont->SetEventDataL( paramSet->Notification( ) );
	// now send it to all the subscribers
	for ( TInt i =0; i < iSubscribeInfoCont->CountOfSubscribeInfoElementArray( ); i++ )
		{
		CUPnPSubscribeInfoElement* element = iSubscribeInfoCont->AtSubscribeInfoElementArray( i );
		TInt eventKey = element->EventKey( );
		eventKey++;
		if(eventKey == 0)
			{
			eventKey++; //wrap around
			}
		element->SetEventKey(eventKey);
		LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::SendNotifyToRemoteClientsL - Posting message TUpnpMessage::TNotifyRequest")));
		// send the message to start the activty
		RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( TUpnpMessage::EActivityNotifyRequest, NodeId () ), TUpnpMessage::TNotifyRequest ( element ).CRef () );
		}	
	}

void CUPnPServiceSubConnectionProvider::NotifySubConnectionL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::NotifySubConnectionL")));
	CUPnPSubConnEvent* subConnEvent = CUPnPSubConnEvent::NewL ( );
	
	TCleanupItem item ( &CUPnPSubConnEvent::CleanupSubConnEvent, subConnEvent );
	CleanupStack::PushL ( item );
	
	subConnEvent->SetFamily ( EUPnPPublishResponseParamSet );
	NotifyClientsL ( *subConnEvent );
	CleanupStack::Pop ( ); // item
	}

void CUPnPServiceSubConnectionProvider::ClearResponseParamL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::ClearResponseParamL")));		
	iResponseBundle.Close ();	// DEF126893 fix pending
	}

void CUPnPServiceSubConnectionProvider::AddResponseParamSetL ( TUPnPEvent aEvent )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::AddResponseParamSetL")));
	if ( !IsClosing ( ) )
		{
		RParameterFamily responseFamily = iResponseBundle.CreateFamilyL ( EUPnPPublishResponseParamSet );
		RParameterFamily family;
		CUPnPPublishResponseParamSet* responseSet = CUPnPPublishResponseParamSet::NewL ( family );
		responseFamily.AddParameterSetL ( responseSet, RParameterFamily::ERequested );
		
		if ( iPublishInfo )
			responseSet->SetUriL( iPublishInfo->SearchTarget( ) );
		responseSet->SetUPnPEvent( aEvent );
		
		NotifySubConnectionL ( );
		}
	}
	

void CUPnPServiceSubConnectionProvider::DeprecateExpiredSubscriptions ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScpr, KComponent, _L8("CUPnPServiceSubConnectionProvider::DeprecateExpiredSubscriptions")));
	TInt duration;			
	TUint count = iSubscribeInfoCont->CountOfSubscribeInfoElementArray( );
	CUPnPSubscribeInfoElement* element = NULL;
	
	for ( TInt i = 0; i < count; i++ )
		{
		element = iSubscribeInfoCont->AtSubscribeInfoElementArray( i );
		duration = element->SubscriptionDuration( );
		if ( --duration == 0 )
			{
			// timer has expired so delete this entry
			iSubscribeInfoCont->DeleteSubscribeInfoElement( element );
			}
		else
			{
			element->SetSubscriptionDuration( duration );
			}
		}		
	}

CUPnPDeviceRegisterParamSet* CUPnPServiceSubConnectionProvider::CopyWithIconInfoL ( CUPnPDeviceRegisterParamSetExt* aDeviceParam )
	{
	RParameterFamily family;
	CUPnPDeviceRegisterParamSet* newParamSet = CUPnPDeviceRegisterParamSet::NewL ( family );
	newParamSet->SetUriL ( aDeviceParam->Uri () );
	newParamSet->SetCacheControlData ( aDeviceParam->CacheControlData () );
	newParamSet->SetParentDeviceUidL ( aDeviceParam->ParentDeviceUid () );
	newParamSet->SetFriendlyNameL ( aDeviceParam->FriendlyName () );
	newParamSet->SetManufacturerL ( aDeviceParam->Manufacturer () );
	newParamSet->SetManufactureUrlL ( aDeviceParam->ManufacturerUrl () );
 	newParamSet->SetModelDescL ( aDeviceParam->ModelDesc () );
	newParamSet->SetModelNameL ( aDeviceParam->ModelName () );
 	newParamSet->SetModelNumberL ( aDeviceParam->ModelNumber () );
 	newParamSet->SetModelUriL ( aDeviceParam->ModelUri () );
 	newParamSet->SetPresentationL ( aDeviceParam->PresentationUri () );
 	newParamSet->SetSerialNumberL ( aDeviceParam->SerialNumber () );
 	newParamSet->SetUDNL ( aDeviceParam->UDN () );
	newParamSet->SetUPCL ( aDeviceParam->UPC () );
		
	LoadIconListL ( aDeviceParam->IconData (), newParamSet );	
	
	return newParamSet;
	}
	
void CUPnPServiceSubConnectionProvider::LoadIconListL ( const TDesC8& aIconData, CUPnPDeviceRegisterParamSet* aDeviceParamSet )
	{
	TPtrC8 buffer ( aIconData );	
	TInt count = 0;	
	ReadTInt ( buffer, count );
	
	TInt size = 0;
	for ( TInt i = 0; i < count; i++ )
		{
		CUPnPIconParams* icon = CUPnPIconParams::NewL ();
		CleanupStack::PushL ( icon );
		
		ReadTInt ( buffer, size );		
		icon->SetMimeTypeL ( buffer.Left ( size ) );
		buffer.Set ( buffer.Ptr () + size, buffer.Length () - size ); //update pointer
		
		ReadTInt ( buffer, size );		
		icon->SetUrlL ( buffer.Left ( size ) );
		buffer.Set ( buffer.Ptr () + size, buffer.Length () - size ); //update pointer
				
		ReadTInt ( buffer, size );		
		icon->SetWidth ( size );
		
		ReadTInt ( buffer, size );		
		icon->SetHeight ( size );
		
		ReadTInt ( buffer, size );		
		icon->SetDepth ( size );
		
		aDeviceParamSet->SetIconInfoL ( icon );
		CleanupStack::Pop (); // icon
		}
	}
	
void CUPnPServiceSubConnectionProvider::ReadTInt ( TPtrC8& aBuffer, TInt& aValue )
	{	
	TUint32 len = sizeof ( TUint32 );
	TPtrC8 ptr ( aBuffer.Ptr (), len );
	InetProtTextUtils::ConvertDescriptorToInt ( ptr, aValue );
	aBuffer.Set ( aBuffer.Ptr () + len, aBuffer.Length () - len ); //update pointer
	}
	
