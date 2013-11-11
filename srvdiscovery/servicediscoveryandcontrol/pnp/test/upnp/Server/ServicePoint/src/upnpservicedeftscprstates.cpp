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

//System Includes
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_logext.h>

#include <http/thttptable.h>
#include <inetprottextutils.h>
#include <upnpdescriptionschema.h>
#include <rmemchunk.h>
#include <rmemcell.h>
//Local Includes
#include "upnpservicedeftscprstates.h"
#include "upnp_cf_msgs.h"
#include "upnpstatemachine.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
using namespace UPnPServiceDefaultStates;
using namespace ESock;


//-=========================================================
//
//States
//
//-=========================================================

//-=========================================================
//
//Transitions
//
//-=========================================================
DEFINE_SMELEMENT ( TStoreUdpServer, NetStateMachine::MStateTransition, TContext )
void TStoreUdpServer::DoL ( )
	{
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated> ( iContext.iMessage );
	iContext.Node ( ).SetUdpServerFlow ( msg.iNodeId );
	}


DEFINE_SMELEMENT ( TStoreUdpClient, NetStateMachine::MStateTransition, TContext )
void TStoreUdpClient::DoL ( )
	{
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated> ( iContext.iMessage );
	iContext.Node ( ).SetUdpClientFlow ( msg.iNodeId );
	}

DEFINE_SMELEMENT ( TBuildPublishAndRootDeviceInfo, NetStateMachine::MStateTransition, TContext )
void TBuildPublishAndRootDeviceInfo::DoL ( )
	{
	if ( iContext.Node ( ).PublishInfo ( ) )
		{
		return;
		}
	
	TUpnpMessage::TRegisterRootDevice& msg = message_cast< TUpnpMessage::TRegisterRootDevice > ( iContext.iMessage );
	CUPnPDevice* device = static_cast <CUPnPDevice*> ( msg.iPtr );
	
	CStringPoolManager& stringPoolMgr = iContext.Node ().ConnectionProvider ().StringPoolManager ();
	RStringPool& sp = stringPoolMgr.StringPool ( );
	const TStringTable& upnpTable = stringPoolMgr.GetUPnPTable();
	CUPnPPublishInfoElement* publishInfo = CUPnPPublishInfoElement::NewL ( );
    CleanupStack::PushL ( publishInfo );
    
    publishInfo->SetKeyL( iContext.Node ().ConnectionProvider ().RootDeviceLocation () );
    publishInfo->SetSearchTargetL( KRootDevice ( ) ); //upnp:rootdevice
	publishInfo->SetCacheControlL( KCacheControl );
	
	const TDesC8& udnPtr = device->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUdn, upnpTable ) );
	const TDesC8& deviceTypePtr = device->Property( sp.String(UPNPDESCRIPTIONXMLTAGS::EDeviceType,upnpTable ) );
	
	publishInfo->SetUuidL ( udnPtr );
	
	// Set Usn to udn + devicetype
	TInt len = udnPtr.Length ( ) + KSeperator.iTypeLength + deviceTypePtr.Length ( );	
	RBuf8 tempBuf;
	tempBuf.CreateMaxL ( len );
	CleanupClosePushL ( tempBuf );
	
	tempBuf.Copy ( udnPtr );
	tempBuf.Append ( KSeperator ( ) );
	tempBuf.Append ( deviceTypePtr );
	publishInfo->SetUsnL( tempBuf );
		
	iContext.Node ( ).SetPublishInfoL ( publishInfo );
	// store deviceType in SCPr node for reference in M-Search Response Activity
	iContext.Node ( ).SetRootDeviceUrnL ( deviceTypePtr );
	
	CleanupStack::PopAndDestroy ( &tempBuf );
	CleanupStack::Pop ( publishInfo );	
	}


DEFINE_SMELEMENT ( TSendPublishRequest, NetStateMachine::MStateTransition, TContext )
void TSendPublishRequest::DoL ( )
	{
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	// Publish once for upnp:rootdevice::uuid:device-UUID
	DoPublishL ( publishInfo->SearchTarget ( ), iContext.Node ( ).RootDeviceUsn ( ), publishInfo->CacheControl ( ) );
		
	// Publish once for uuid:device-UUID
	DoPublishL ( publishInfo->Uuid ( ), publishInfo->Uuid ( ), publishInfo->CacheControl ( ) );
		
	// Publish once for uuid:device-UUID::urn:domain-name-device:deviceType:v	
	DoPublishL ( iContext.Node ( ).RootDeviceUrn ( ), publishInfo->Usn ( ), publishInfo->CacheControl ( ) );
		
	iContext.Node ( ).SetRootDevicePublished ( ETrue );
	}
	
void TSendPublishRequest::DoPublishL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge )
	{
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	RMemChunk locationBuf;
	locationBuf.CreateL ( iContext.Node ().ConnectionProvider ().RootDeviceLocation (), allocator );
	
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );	
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
	CleanupStack::PushL ( item );
	
	RMemChunk usnBuf;
	usnBuf.CreateL ( aUsn, allocator );
	
	TSsdpInfo ssdpInfo ( aMaxAge, locationBuf, usnBuf, stBuf );
	
	TUpnpMessage::TUPnPPublishAliveRequest publishMsg ( ssdpInfo );			
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().UdpClientFlow (), publishMsg );
	
	CleanupStack::Pop ( ); // item
	}


DEFINE_SMELEMENT ( TRegisterRootDeviceWithMuServer, NetStateMachine::MStateTransition, TContext )
void TRegisterRootDeviceWithMuServer::DoL ( )
	{
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	// Register once for upnp:rootdevice
	RegisterForMSearchL ( publishInfo->SearchTarget ( ) );
	
	// Register once for uuid:device-UUID
	RegisterForMSearchL ( publishInfo->Uuid ( ) );
	
	// Register once for uuid:device-UUID::urn:domain-name-device:deviceType:v
	RegisterForMSearchL ( iContext.Node ( ).RootDeviceUrn ( ) );
	}
	
void TRegisterRootDeviceWithMuServer::RegisterForMSearchL ( const TDesC8& aSearchTarget )
	{
	RMemoryAllocator allocator(iContext.Node ( ).ConnectionProvider().MemoryManager());
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );
	
	TSsdpInfo ssdpInfo ( stBuf );
	
	TUpnpMessage::TUPnPPublishRegistration msg ( ssdpInfo );
	RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, iContext.NodeId () ), iContext.Node ().UdpServerFlow (), msg );
	}
	

DEFINE_SMELEMENT ( TStartMuClient, NetStateMachine::MStateTransition, TContext )
void TStartMuClient::DoL ( )
	{
	iContext.iNodeActivity->PostRequestTo ( iContext.Node ().UdpClientFlow (), TCFDataClient::TStart ().CRef () );
	}

DEFINE_SMELEMENT ( TStartMuServer, NetStateMachine::MStateTransition, TContext )
void TStartMuServer::DoL ( )
	{	
	iContext.iNodeActivity->PostRequestTo ( iContext.Node ().UdpServerFlow (), TCFDataClient::TStart ().CRef () );
	}
	
DEFINE_SMELEMENT ( TSendRootDeviceRegistered, NetStateMachine::MStateTransition, TContext )
void TSendRootDeviceRegistered::DoL ( )
	{
	TUpnpMessage::TRootDeviceRegistered msg ( iContext.iNodeActivity->Error( ) );
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (), msg );
	}


DEFINE_SMELEMENT ( TSendSearchResponse, NetStateMachine::MStateTransition, TContext )
void TSendSearchResponse::DoL ( )
	{	
	if ( !iContext.Node ( ).RootDevicePublished ( ) )
		{
		return;		
		}
	
	TUpnpMessage::TUPnPSearchRequest& reqMsg = message_cast< TUpnpMessage::TUPnPSearchRequest > ( iContext.iMessage );
		
	RMemCell* mBuf = reqMsg.iSsdpInfo.iSearchTarget.First ( );
	TPtr8 stPtr ( mBuf->Ptr( ), mBuf->Length( ), mBuf->Length( ) );
		
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
		
	if ( stPtr.CompareF ( publishInfo->SearchTarget ( ) ) == 0 )
		{
		// Search Response for upnp:rootdevice::uuid:device-UUID
		SendSearchResponseL ( publishInfo->SearchTarget ( ), iContext.Node ( ).RootDeviceUsn ( ), publishInfo->CacheControl ( ) );
		}
	else if ( stPtr.CompareF ( publishInfo->Uuid ( ) ) == 0 )
		{
		// Search Response for uuid:device-UUID
		SendSearchResponseL ( publishInfo->Uuid ( ), publishInfo->Uuid ( ), publishInfo->CacheControl ( ) );
		}
	else
		{
		// Search Response for uuid:device-UUID::urn:domain-name-device:deviceType:v		
		SendSearchResponseL ( stPtr, publishInfo->Usn ( ), publishInfo->CacheControl ( ) );
		}
	reqMsg.iSsdpInfo.iSearchTarget.Free ( );
	}

void TSendSearchResponse::SendSearchResponseL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge )
	{		
	TUpnpMessage::TUPnPSearchRequest& reqMsg = message_cast< TUpnpMessage::TUPnPSearchRequest > ( iContext.iMessage );	
	TAppProtAddr remoteUnicastAddr = reqMsg.iSsdpInfo.iAddr;
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	RMemChunk locationBuf;
	locationBuf.CreateL ( iContext.Node ().ConnectionProvider ().RootDeviceLocation (), allocator );
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &locationBuf );
	CleanupStack::PushL ( item );

	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );
	TCleanupItem item1 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
	CleanupStack::PushL ( item1 );

	RMemChunk usnBuf;
	usnBuf.CreateL ( aUsn, allocator );
	
	TSsdpInfo ssdpInfo ( aMaxAge,
						locationBuf,
						usnBuf,
						stBuf,
						remoteUnicastAddr );
	
	TUpnpMessage::TUPnPSearchResponse msg ( ssdpInfo );
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().UdpClientFlow (), msg );
	
	CleanupStack::Pop ( ); // item
	CleanupStack::Pop ( ); // item1
	}


DEFINE_SMELEMENT ( TProcessDataClientStop, NetStateMachine::MStateTransition, TContext )
void TProcessDataClientStop::DoL ( )
	{
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	if ( publishInfo && iContext.Node ( ).RootDevicePublished ( ) )
		{
		// for upnp:rootdevice::uuid:device-UUID
		SendByeRequestL ( publishInfo->SearchTarget ( ), iContext.Node ( ).RootDeviceUsn ( ) );
			
		// for uuid:device-UUID
		SendByeRequestL ( publishInfo->Uuid ( ), publishInfo->Uuid ( ) );
		
		// for uuid:device-UUID::urn:domain-name-device:deviceType:v
		SendByeRequestL ( iContext.Node ( ).RootDeviceUrn ( ), publishInfo->Usn ( ) );
		
		iContext.Node ( ).SetRootDevicePublished ( EFalse );		
		}

	// StopDataClients
	iContext.Node().PostToClients<TDefaultClientMatchPolicy> ( TNodeCtxId ( iContext.ActivityId (), iContext.NodeId () ),
									TCFDataClient::TStop ( KErrNone ).CRef (), TClientType ( TCFClientType::EData, TCFClientType::EStarted ) );
	iContext.iNodeActivity->SetPostedTo( TNodeId::NullId ( ) );
	
	return;
	}

void TProcessDataClientStop::SendByeRequestL ( const TDesC8& aSearchTarget, const TDesC8& aUsn )
	{
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
	CleanupStack::PushL ( item );

	RMemChunk usnBuf;
	usnBuf.CreateL ( aUsn, allocator );
	
	TSsdpInfo ssdpInfo ( usnBuf, stBuf );
	TUpnpMessage::TUPnPPublishByeRequest msg ( ssdpInfo );
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().UdpClientFlow (), msg );
	
	CleanupStack::Pop ( ); // item
	}
	
DEFINE_SMELEMENT ( TLeaveRequest, NetStateMachine::MStateTransition, TContext )
void TLeaveRequest::DoL ( )
	{
	TUint ccCount = iContext.Node ().CountClients<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::ECtrl ) );
		
	// -- If control client goes zero and the node is not having any pending activities
	// then process the data client stop,  
	// mark the default SCPR node for deletion and close it.
	if ( ccCount == 0 )
		{
		iContext.Node ( ).SetClosing ( );		
		}
	}
	
DEFINE_SMELEMENT ( TNoTagOrDataClientIdle, NetStateMachine::MStateFork, TContext )
TInt TNoTagOrDataClientIdle::TransitionTag ( )
	{
	return iContext.Node ( ).IsClosing ( ) ? CoreNetStates::KDataClientIdle : MeshMachine::KNoTag;
	}	


DEFINE_SMELEMENT ( TRequestOrIgnoreTag, NetStateMachine::MStateFork, TContext )
TInt TRequestOrIgnoreTag::TransitionTag ( )
	{
	if ( iContext.Node ( ).IsClosing ( ) )
		{
		return CoreStates::KIgnore;
		}
	
	TUpnpMessage::TUPnPRequestInfo& requestInfoMsg = message_cast<TUpnpMessage::TUPnPRequestInfo> ( iContext.iMessage );	
	CTransaction* transaction = static_cast<CTransaction*> ( requestInfoMsg.iPtr );	
	const RStringF& header =  transaction->Request( )->Handle( ).Method ( );

	__ASSERT_DEBUG ( header == transaction->Request( )->StringPool ( ).StringF ( HTTP::EGET, THTTPTable::Table() ), User::Invariant () );
	
	const TDesC8& uriPath =  transaction->Request()->URI()->Uri().Extract ( EUriPath);
	_LIT8 ( KRootPath, "/" );
	if ( uriPath.Compare ( KRootPath ) == 0 )
		{
		return UPnPStateMachine::KDescribeRequest;
		}
	else
		{
		return UPnPStateMachine::KIconRequest;
		}
	}


DEFINE_SMELEMENT ( TNoTagOrIgnoreTag, NetStateMachine::MStateFork, TContext )
TInt TNoTagOrIgnoreTag::TransitionTag ( )
	{
	return iContext.Node ( ).IsClosing ( ) ? CoreStates::KIgnore : MeshMachine::KNoTag;
	}


