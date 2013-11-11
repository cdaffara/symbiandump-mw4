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
#include <ss_glob.h>
#include <ss_roles.h>
#include "upnpmcpr.h"
#include "upnpmcpractivities.h"
#include "upnp_cf_msgs.h"
#include "upnpserverconstants.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPMCprTag KESockMetaConnectionTag
#endif


using namespace ESock;
using namespace Messages;


typedef TBuf<0x100> THostName; // Note ! cannot include es_sock.h
_LIT8 ( KHttpDefaultPort, "80" );
//Add Blocks of 64
static const TInt KBlockSize1 = 64;
static const TInt KMinGrowth1 = 30;
static const TInt KThresholdGrowth1 = 30;
static const TInt KInitialAllocation1 = 128;
//Add Blocks of 512
static const TInt KBlockSize2 = 512;
static const TInt KMinGrowth2 = 16;
static const TInt KThresholdGrowth2 = 16;
static const TInt KInitialAllocation2 = 64;

static const TInt KHeapSize	= 1024 * 1024;	//1MB

CUPnPMetaConnectionProvider* CUPnPMetaConnectionProvider::NewL(
		CMetaConnectionProviderFactoryBase& aFactory, 
		const TProviderInfo& aProviderInfo)
	{
	CUPnPMetaConnectionProvider* provider = new (ELeave) CUPnPMetaConnectionProvider(
		aFactory, 
		aProviderInfo, 
		UPnPMCprActivities::activityMap::Self());
	CleanupStack::PushL(provider);
	provider->ConstructL();
	CleanupStack::Pop(provider);
	return provider;
	}
	
CUPnPMetaConnectionProvider::~CUPnPMetaConnectionProvider()
	{
	LOG_NODE_DESTROY(KUPnPMCprTag, CUPnPMetaConnectionProvider);
	LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "Deleting ~CUPnPMetaConnectionProvider" ) ) );				
	
	TUpnpMessage::DeRegister ( );
	
	delete iSocketOpener;
	iSocket.Close ();
	iDeviceLocation.Close ();
	delete iChunkManager;
	LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "Deleted ~CUPnPMetaConnectionProvider" ) ) );	
	}
	
CUPnPMetaConnectionProvider::CUPnPMetaConnectionProvider(
		CMetaConnectionProviderFactoryBase& aFactory, 
		const TProviderInfo& aProviderInfo, 
		const MeshMachine::TNodeActivityMap& aActivityMap)
	: CCoreMetaConnectionProvider(aFactory, aProviderInfo, aActivityMap)
	{
	LOG_NODE_CREATE(KUPnPMCprTag, CUPnPMetaConnectionProvider);    
	}
	
void CUPnPMetaConnectionProvider::ConstructL ()
	{
	CCoreMetaConnectionProvider::ConstructL ();
	
	iSocketOpener = CSocketOpener::NewL ( *this );	
	
	// Register with comms transport to be able to receive our
	// custom Upnp messages
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	
	// Register UPnPMesage with dataplane thread
	TNodeId factoryContainer = SockManGlobals::Get ()->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );	
	CommsFW::TWorkerId worker = factoryContainer.Thread ();
	Den::CCommonWorkerThread* workerThread = SockManGlobals::Get ()->SelfPlayer ()->PitBoss ().WorkerDataGlobals ().WorkerThread ( worker );
	TUpnpMessage::RegisterL ( workerThread );
	
	//Create the Chunk Manager
	CChunkManager* chkmgr = CChunkManager::NewL ( KHeapSize );
	if ( chkmgr )
		{
		chkmgr->AddPoolL ( KBlockSize1, KInitialAllocation1, KMinGrowth1, KThresholdGrowth1 );
		chkmgr->AddPoolL ( KBlockSize2, KInitialAllocation2, KMinGrowth2, KThresholdGrowth2 );
		}
	
	iChunkManager = chkmgr;
	}
	
void CUPnPMetaConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
    {
	ESOCK_DEBUG_MESSAGE_INTERCEPT ( aSender, aMessage, aRecipient );
	
	MeshMachine::TNodeContext<CUPnPMetaConnectionProvider> ctx ( *this, aMessage, aSender, aRecipient );
    if ( ctx.iMessage.IsMessage<TUpnpMessage::TRequestLocalHost> () )	 
		{		
		FindLocalHost ();
		}

	CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}
	
const TDesC8& CUPnPMetaConnectionProvider::DeviceLocationUrl ()
	{
	return iDeviceLocation;
	}

CChunkManager* CUPnPMetaConnectionProvider::ChunkManager()
	{
	return iChunkManager;
	}

void CUPnPMetaConnectionProvider::FindLocalHost ()
	{
	iSocketOpener->MakeSocket( KAfInet, KSockStream, KProtocolInetTcp );
	}
	
void CUPnPMetaConnectionProvider::OpenComplete ( RInternalSocket& aSocket )
	{
	iSocket = aSocket;
	
	// We're going to ask for information about the interface used to reach
	// a certain destination address. The constant address KInetAddrAny should
	// guarantee that we get the interface which is the preferred default route.	
	TInetAddr localHost;
	localHost.SetAddress ( KInetAddrAny );
	TPckgBuf<TSoInetIfQuery> query;
	query ().iDstAddr = localHost;
	
	TInt err = iSocket.GetOpt ( KSoInetIfQueryByDstAddr, KSolInetIfQuery, query );
	
	if ( err == KErrNone )
		{
		// its local IP address
		localHost = query ().iSrcAddr;
		err = GenerateDeviceLocation ( localHost );
		
		LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "- CUPnPMetaConnectionProvider::OpenComplete -"  ) ) );
		//LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "- interface link name %s" ), query ().iName ) ) ;
		//LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "- interface link local host ip %s" ), query ().iSrcAddr ) );
		LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "- interface link Index %d" ), query ().iIndex ) );
		LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "- interface link up or not %d" ), query ().iIsUp ) );
		}
	else if ( err == KErrNotFound ) // KErrNotFound would indicate that there is no
									// 'up' default route, maybe because none is
									// configured or else we're not connected to the Internet
		{
		//UPNP Stack cannot proceed with this. Check the interface configuration
		__ASSERT_DEBUG ( ETrue, User::Invariant () );
		}
		
	RNodeInterface* dc = GetFirstClient<TDefaultClientMatchPolicy> ( TCFClientType::EData );
	if ( err == KErrNone )
		{
		LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "posting local Host to CPR" ) ) );
		RClientInterface::OpenPostMessageClose ( NodeId (), dc->RecipientId (), TUpnpMessage::TLocalHost ().CRef () );
		}
	else
		{
		LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "posting error to CPR" ) ) );				
		TEBase::TError msg ( TUpnpMessage::TRequestLocalHost::Id(), err );
		RClientInterface::OpenPostMessageClose ( NodeId (), dc->RecipientId (), msg );
		}
	}
	
TInt CUPnPMetaConnectionProvider::GenerateDeviceLocation ( const TInetAddr& aAddr )
	{
	THostName hostName;	
	aAddr.Output ( hostName );
	
	iDeviceLocation.Close ( );
	//Allocate memory for having ---  http://'ip':'port'/
	TInt err = iDeviceLocation.Create ( KHttpScheme ().Length () + hostName.Length () + 1 + KHttpDefaultPort ().Length () + 1 );
	iDeviceLocation.Copy ( KHttpScheme () );
	iDeviceLocation.Append ( hostName );
	iDeviceLocation.Append ( TChar ( ':' ) );
	iDeviceLocation.Append ( KHttpDefaultPort () );
	iDeviceLocation.Append ( TChar ( '/' ) );
	LOG ( ESockLogExternal::Printf ( KUPnPMCprTag, KUPnPMCprTag, _L8 ( "- upnp device location %s" ), &iDeviceLocation ) );
	return err;
	}

	
void CUPnPMetaConnectionProvider::Error ( TOperation /*aOperation*/, TInt aError )
	{
	RNodeInterface* dc = GetFirstClient<TDefaultClientMatchPolicy> ( TCFClientType::EData );	
	TEBase::TError msg ( TUpnpMessage::TRequestLocalHost::Id(), aError );
	RClientInterface::OpenPostMessageClose ( NodeId (), dc->RecipientId (), msg );
	}
