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
#include "upnp_cf_msgs.h"
#include "upnpsubconnevents.h"
#include "upnpctrlscpractivities.h"
#include "upnpuuid.h"
#include "upnplog.h"
#include "upnpcpr.h"

__FLOG_STMT(_LIT8(KComponent,"UPnPCtrlScpr");)

using namespace ESock;

CUPnPControlSubConnectionProvider* CUPnPControlSubConnectionProvider::NewL ( CSubConnectionProviderFactoryBase& aFactory )
	{
	CUPnPControlSubConnectionProvider* provider = new (ELeave) CUPnPControlSubConnectionProvider ( aFactory, UPnPControlSCprActivities::activityMap::Self() );
	CleanupStack::PushL ( provider );
	provider->ConstructL ( );
	CleanupStack::Pop ( provider );
	return provider;
	}

CUPnPControlSubConnectionProvider::~CUPnPControlSubConnectionProvider ( )
	{
	LOG_NODE_DESTROY(KSubsysCtrlScpr, CUPnPControlSubConnectionProvider);	
	iResponseBundle.Close( );	
	iFlowInfo.Reset ( );
	iSubscriptions.ResetAndDestroy();
	iCallbackUrl.Close();
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("Destroyed CUPnPControlSubConnectionProvider")));
	}


CUPnPControlSubConnectionProvider::CUPnPControlSubConnectionProvider ( CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap )
	: CUPnPSubConnectionProvider ( aFactory, aActivityMap ),
	iTcpSrvFlowCreateRequested ( EFalse ), iResponseBlocked ( EFalse ), iUdpClientFlow ( TNodeId::NullId ( ) ), iUdpServerFlow ( TNodeId::NullId ( ) ), iTcpServerFlow ( TNodeId::NullId ( ) )
	{
	LOG_NODE_CREATE(KSubsysCtrlScpr, CUPnPControlSubConnectionProvider);    
	}

void CUPnPControlSubConnectionProvider::ConstructL ( )
	{
	CCoreSubConnectionProvider::ConstructL();	
	// make sure bundle constructs are initialised.
	iResponseBundle.CreateL ();	
	iResponseBundle.Open ();
	
	iRequestBundle.CreateL ();
	iQueueBundle.CreateL ();
		
	RParameterFamily discover = iQueueBundle.CreateFamilyL ( EUPnPDiscoverResponseParamSet );
	RParameterFamily presenceNotify = iQueueBundle.CreateFamilyL ( EUPnPPresenceAnnouncementParamSet );
	RParameterFamily absenceNotify =  iQueueBundle.CreateFamilyL ( EUPnPAbsenceAnnouncementParamSet );
	RParameterFamily describe =  iQueueBundle.CreateFamilyL ( EUPnPDescribeResponseParamSet );
	RParameterFamily subscribe =  iQueueBundle.CreateFamilyL ( EUPnPSubscribeResponseParamSet );
	RParameterFamily eventNotify =  iQueueBundle.CreateFamilyL ( EUPnPNotifyEventParamSet );
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("Created CUPnPControlSubConnectionProvider")));
	}

void CUPnPControlSubConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	ESOCK_DEBUG_MESSAGE_INTERCEPT ( aSender, aMessage, aRecipient );
	TNodeContext<CUPnPControlSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);	
    CCoreSubConnectionProvider::ReceivedL ( aSender, aRecipient, aMessage );   
    User::LeaveIfError (  ctx.iReturn );
	}
	
void CUPnPControlSubConnectionProvider::HandlePassThroughMessage ( MeshMachine::TNodeContextBase& aContext )
  	{
  	AMMNodeBase::HandlePassThroughMessage (aContext);
  	
  	if ( aContext.iMessage.MessageId ().MessageId () != Messages::KNullMessageId )
  	// message is not handled by any of the upnp activity, free the data passed by dataplane
  		{
  		if ( aContext.iMessage.MessageId ().MessageId () == TUpnpMessage::TCancelParamSet::EId )
			{
			TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( aContext.iMessage );
			CUPnPRequestParamSetBase* cancelParamSet = static_cast<CUPnPRequestParamSetBase*> ( msg.iParamSet );
			
			STypeId metaType = cancelParamSet->GetTypeId( );
    		
    		if ( metaType.iType == EUPnPCancelDescribeParamSet )
    			{
    			CleanupEventQueuedResponses ( cancelParamSet->Uri (), EUPnPDescribeResponseParamSet );	    			
				delete static_cast<CUPnPCancelDiscoverParamSet*> ( cancelParamSet );
    			}
			}
  		}
  		
  	// clear message id now
  	aContext.iMessage.ClearMessageId ( );
  	}
	
		
void CUPnPControlSubConnectionProvider::InitiateActivitiesL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateActivitiesL")));
	RParameterFamily paramFamily = iRequestBundle.GetFamilyAtIndex (0);
	switch ( paramFamily.Id() )
		{
		case EUPnPClearParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateActivitiesL - TUPnPParamSetFamily::EUPnPClearParamSet")));
			ClearResponseParamL ( );			
			}
			break;
		case EUPnPDiscoverRequestParamSet:
		case EUPnPRegisterAnnouncementParamSet:
		case EUPnPDescribeRequestParamSet:
		case EUPnPSubscribeRequestParamSet:		
		case EUPnPCancelDiscoverParamSet:
		case EUPnPCancelDescribeParamSet:
		case EUPnPCancelSubscribeParamSet:
		case EUPnPCancelRegisterNotifyParamSet:
			{	
			InitiateRequestL ( paramFamily ) ;
			}
			break;
		default:
			break;
		}		
		iRequestBundle.DeleteFamilyAtIndex (0);
	}

void CUPnPControlSubConnectionProvider::ClearResponseParamL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::ClearResponseParamL")));	
	
	iResponseBlocked = EFalse;	
	NotifySubConnectionIfResultsAvailable ();
	}
	
void CUPnPControlSubConnectionProvider::NotifySubConnectionIfResultsAvailable ( )	
	{
	if ( IsClosing ( ) )
		{		
		return;
		}
		
	TUint32 familyType;
	if ( CheckForResultsAvailability ( familyType ) )
	 	{
		TRAP_IGNORE ( NotifySubConnectionL ( familyType ) );
	 	}
	}
	
void CUPnPControlSubConnectionProvider::NotifySubConnectionL ( const TUint32 aFamilyId )
	{	
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::NotifySubConnectionL")));
	RParameterFamily respFamily = iResponseBundle.CreateFamilyL ( aFamilyId );	
	RParameterFamily queueFamily = iQueueBundle.FindFamily ( aFamilyId );
	
	TInt count = queueFamily.CountParameterSets ( RParameterFamily::ERequested );
	TInt pos;
	for ( pos = 0; pos < count; pos++ )
		{
		XParameterSetBase* paramSet = queueFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested );
		respFamily.AddParameterSetL ( paramSet,	RParameterFamily::ERequested );
		queueFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested);
		}
	
	// Cant help it out......tried to convince ESock team
	pos = count - 1;
	while ( pos >= 0 )
		{
		reinterpret_cast<RUPnPParameterFamily*>(&queueFamily)->DeleteParamSetL ( pos );
		--pos;
		}
	
	CUPnPSubConnEvent* subConnEvent = CUPnPSubConnEvent::NewL ( );
	
	TCleanupItem item ( &CUPnPSubConnEvent::CleanupSubConnEvent, subConnEvent );
	CleanupStack::PushL ( item );
	subConnEvent->SetFamily ( aFamilyId );
	NotifyClientsL ( *subConnEvent );
	CleanupStack::Pop ( ); // item
	
	iResponseBlocked = ETrue;		
	}

TBool CUPnPControlSubConnectionProvider::CheckForResultsAvailability ( TUint32& aFamilyId )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::CheckForResultsAvailability")));
	TInt familyCount = iQueueBundle.CountParameterFamilies ();
	for( TInt index = 0; index < familyCount; index++ )
		{
		RParameterFamily paramFamily = iQueueBundle.GetFamilyAtIndex ( index );
		if( paramFamily.CountParameterSets ( RParameterFamily::ERequested ) > 0)
			{
			//There are pending responses available. return the family id.
			aFamilyId = paramFamily.Id();
			return ETrue;
			}
		}
	return EFalse;
	}


CUPnPControlConnectionProvider& CUPnPControlSubConnectionProvider::ConnectionProvider ( )
	{
	CUPnPConnectionProvider& cprBase = BaseConnectionProvider();
	return static_cast<CUPnPControlConnectionProvider&>(cprBase);
	}	

	
void CUPnPControlSubConnectionProvider::InitiateRequestL ( RParameterFamily& aReqFamily )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL")));
	TInt familyId = aReqFamily.Id();
	TUint count = aReqFamily.CountParameterSets ( RParameterFamily::ERequested );
	
	switch(familyId)
		{
		case EUPnPDiscoverRequestParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPDiscoverRequestParamSet")));
			// while ( count != 0 )
			for ( TInt pos = 0; pos < count; pos++ )
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPDiscoverRequestParamSet - Posting message TUpnpMessage::TSearchRequest")));
				//post the message to start the activity and takes ownership of parameter set								
				CUPnPDiscoverRequestParamSet* paramSet = static_cast<CUPnPDiscoverRequestParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TSearchRequest ( paramSet ).CRef () );
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );
				}
				break;
			}
		case EUPnPCancelDiscoverParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelDiscoverParamSet")));
			for ( TInt pos = 0; pos < count; pos++ )
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelDiscoverParamSet - Posting message TUpnpMessage::TDataClientStop")));
				//post the message to start the activity and takes ownership of parameter set
				CUPnPCancelDiscoverParamSet* paramSet = static_cast<CUPnPCancelDiscoverParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TCancelParamSet ( paramSet ).CRef () );
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );				
				}
				break;
			}
		case EUPnPRegisterAnnouncementParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPRegisterAnnouncementParamSet")));
			for ( TInt pos = 0; pos < count; pos++ )			
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPRegisterAnnouncementParamSet - Posting message TUpnpMessage::TRegisterNotifyRequest")));
				//post the message to start the activity and takes ownership of parameter set				
				CUPnPRegisterAnnouncementParamSet* paramSet = static_cast<CUPnPRegisterAnnouncementParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TRegisterNotifyRequest ( paramSet ).CRef () );				
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );				
				}
			break;
			}
		case EUPnPCancelRegisterNotifyParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelRegisterNotifyParamSet")));
			for ( TInt pos = 0; pos < count; pos++ )			
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelRegisterNotifyParamSet - Posting message TUpnpMessage::TDataClientStop")));
				CUPnPCancelRegisterNotifyParamSet* paramSet = static_cast<CUPnPCancelRegisterNotifyParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				//post the message to start the activity and takes ownership of parameter set				
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TCancelParamSet ( paramSet ).CRef () );
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );				
				}
			break;
			}
		case EUPnPDescribeRequestParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPDescribeRequestParamSet")));
			for ( TInt pos = 0; pos < count; pos++ )			
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPDescribeRequestParamSet - Posting message TUpnpMessage::TDescribeRequest")));
				//post the message to start the activity and takes ownership of parameter set
				CUPnPDescribeRequestParamSet* paramSet = static_cast<CUPnPDescribeRequestParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TDescribeRequest ( paramSet ).CRef () );				
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );				
				}
			break;
			}
		case EUPnPCancelDescribeParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelDescribeParamSet")));
			for ( TInt pos = 0; pos < count; pos++ )			
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelDescribeParamSet - Posting message TUpnpMessage::TDataClientStop")));
				//post the message to start the activity and takes ownership of parameter set
				CUPnPCancelDescribeParamSet* paramSet = static_cast<CUPnPCancelDescribeParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TCancelParamSet ( paramSet ).CRef () );				
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );			
				}
			break;
			}
		case EUPnPSubscribeRequestParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPSubscribeRequestParamSet")));
			
			GenerateCallbackUrlL ();			
			for ( TInt pos = 0; pos < count; pos++ )			
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPSubscribeRequestParamSet - Posting message TUpnpMessage::TSubscribeRequest")));
				//post the message to start the activity and takes ownership of parameter set				
				CUPnPSubscribeRequestParamSet* paramSet = static_cast<CUPnPSubscribeRequestParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TSubscribeRequest ( paramSet ).CRef () );				
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );				
				}
			break;
			}
		case EUPnPCancelSubscribeParamSet:
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelSubscribeRequestParamSet")));		
			for ( TInt pos = 0; pos < count; pos++ )			
				{
				LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::InitiateRequestL - TUPnPParamSetFamily::EUPnPCancelSubscribeRequestParamSet - Posting message TUpnpMessage::TSubscribeRequest")));
				//post the message to start the activity and takes ownership of parameter set
				CUPnPCancelSubscribeParamSet* paramSet = static_cast<CUPnPCancelSubscribeParamSet*> (aReqFamily.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), NodeId (), TUpnpMessage::TCancelParamSet ( paramSet ).CRef () );								
				aReqFamily.ClearParameterSetPointer( pos, RParameterFamily::ERequested );				
				}
			break;
			}
		default:
			break;
		}		
	}

void CUPnPControlSubConnectionProvider::AddResponseParamSetL ( XParameterSetBase* aResponseSet, const TUint32 aFamilyId )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::AddResponseParamSetL")));
	__ASSERT_DEBUG( aResponseSet != NULL, User::Invariant() );
	if ( IsClosing ( ) )
		{
		delete aResponseSet;
		return;
		}	
	
	RParameterFamily queueFamily = iQueueBundle.FindFamily ( aFamilyId );
	queueFamily.AddParameterSetL ( aResponseSet, RParameterFamily::ERequested );
	
	if(!iResponseBlocked)
		{
		NotifySubConnectionL(aFamilyId);
		}
	}

void CUPnPControlSubConnectionProvider::GenerateCallbackUrlL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GenerateCallbackUrlL")));
	if ( iCallbackUrl.Length ( ) != 0 )
		{
		LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GenerateCallbackUrlL - Callback url already present")));
		return;
		}
	RBuf8 uuid;
	UpnpUuid::GenerateUuidL( uuid );
	CleanupClosePushL( uuid );
	
#if !defined(__GCCXML__)
	CConnectionProviderBase& cprBase = mnode_cast<CConnectionProviderBase>(ControlProvider()->RecipientId().Node());	
#else
	CConnectionProviderBase& cprBase =
		reinterpret_cast<CConnectionProviderBase&>(ControlProvider()->RecipientId().Node());		
#endif

	CUPnPControlConnectionProvider& upnpCpr = static_cast<CUPnPControlConnectionProvider&>(cprBase);
	const TDesC8& rootLoc = upnpCpr.RootDeviceLocation ();
	
	iCallbackUrl.CreateL ( rootLoc.Length ( ) + uuid.Length ( ) );
	iCallbackUrl.Copy ( rootLoc );
	iCallbackUrl.Append ( uuid );
	CleanupStack::PopAndDestroy( &uuid );
	}

// UPnp Subscription Information Elements management API's
CSubscriptionInfo* CUPnPControlSubConnectionProvider::GetSubscriptionInfoByUrl( const TDesC8& aPublishUrl )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GetSubscriptionInfoByUrl")));
	TUint count = iSubscriptions.Count ( );
	for ( TInt index = 0; index < count; index++ )
		{		
		if ( iSubscriptions[index]->iPublishUrl.CompareF ( aPublishUrl ) == 0 )
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GetSubscriptionInfoByUrl - Found subscription info")));
			return iSubscriptions[index];
			}
		}
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GetSubscriptionInfoByUrl - Subscription info not found")));
	return NULL;
	}

CSubscriptionInfo* CUPnPControlSubConnectionProvider::GetSubscriptionInfoBySid ( const TDesC8& aSid )
	{	
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GetSubscriptionInfoBySid")));
	TUint count = iSubscriptions.Count ( );
	for ( TInt index = 0; index < count; index++ )
		{		
		if ( iSubscriptions[index]->iSid.CompareF ( aSid ) == 0 )
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GetSubscriptionInfoBySid - Found subscription info")));
			return iSubscriptions[index];
			}
		}
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::GetSubscriptionInfoBySid - Subscription info not found")));
	return NULL;
	}

void CUPnPControlSubConnectionProvider::AddSubscriptionInfoL(CSubscriptionInfo* aSubscriptionInfo)
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::AddSubscriptionInfoL")));
	iSubscriptions.Append ( aSubscriptionInfo );
	}

void CUPnPControlSubConnectionProvider::RemoveSubscriptionInfo ( const TDesC8& aSid )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CUPnPControlSubConnectionProvider::RemoveSubscriptionInfo by Sid ")));	
	CSubscriptionInfo* info = GetSubscriptionInfoBySid ( aSid );
	__ASSERT_DEBUG ( info != NULL, User::Invariant ( ) );
	
	CleanupEventQueuedResponses ( info->iPublishUrl, EUPnPNotifyEventParamSet );
	
	TInt pos = iSubscriptions.Find ( info );
	iSubscriptions.Remove ( pos );
	delete info;
	}

void CUPnPControlSubConnectionProvider::CleanupEventQueuedResponses ( const TDesC8& aUrl, TUint aFamilyId )
	{	
	RParameterFamily family = GetUPnPFamilyById ( aFamilyId );
		
	// check if response is already available and delete it
	TInt pos = family.CountParameterSets ( RParameterFamily::ERequested ) - 1;
	while ( pos >= 0 )
		{
		CUPnPRequestParamSetBase* paramSet = static_cast<CUPnPRequestParamSetBase*> ( family.GetParameterSetAtIndex ( pos, RParameterFamily::ERequested ) );
		if ( paramSet->Uri ().CompareF ( aUrl ) == 0 )
			{
			LOG(ESockLogExternal::Printf(KSubsysCtrlScpr, KComponent, _L8("CleanupEventQueuedResponses - Queued notification is deleted %d "), pos));
			TRAP_IGNORE ( reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL( pos ) );
			}
		--pos;
		}
		
	}

RParameterFamily CUPnPControlSubConnectionProvider::GetUPnPFamilyById ( TUint32 aFamilyId )
	{
	RParameterFamily family;
	
	TUint fC = iQueueBundle.CountParameterFamilies ();
	for ( TInt pos = 0; pos < fC; pos++ )
		{
		family = iQueueBundle.GetFamilyAtIndex ( pos );
		if ( aFamilyId== family.Id() )
			{
			break;
			}
		}
	return family;
	}

