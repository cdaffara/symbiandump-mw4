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
* Description:   Implementation of tfxsrvplugin
*
*/



#include <ecom/implementationproxy.h>
#include <TfxSrvEcomIds.h>
#include <alflogger.h>
#include "alftfxsrvplugin.h"
#include "alftransitionserver.h"
#include "alftransitionservercontroller.h"
      
      
// Methods called by ECOM
MTransitionServer* TfxSrvPlugin::CreateTransitionServerL()
	{
	__ALFFXLOGSTRING("TfxSrvPlugin::CreateTransitionServerL >>");
	return CSrvImpl::NewL();
	}

MTransitionServerController* TfxSrvPlugin::CreateTransitionServerControllerL(
TAny* aInitParams )
	{
	    
	RWsSession* wsSession = (RWsSession*) aInitParams;
	return CSrvCtrlImpl::NewL( *wsSession );
	}  
   
// ======== ECOM INITIALIZATION ========

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KTfxSrvEcomImpl, TfxSrvPlugin::CreateTransitionServerL ),
    IMPLEMENTATION_PROXY_ENTRY( KTfxSrvCtrlEcomImpl, TfxSrvPlugin::CreateTransitionServerControllerL ),
    };

// ---------------------------------------------------------
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// CSrvImpl implementation 

CSrvImpl* CSrvImpl::NewL()
    {
    __ALFFXLOGSTRING("CSrvImpl::NewL >>");
    CSrvImpl* self = new (ELeave)CSrvImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self;
    __ALFFXLOGSTRING("CSrvImpl::NewL <<");
    return self;
    }
    
TInt CSrvImpl::Connect()
    {
    return iServer->Connect();
    }

void CSrvImpl::Disconnect()
    {
    iServer->Disconnect();
    }

TInt CSrvImpl::RegisterFullscreenKml(TUint aActionID, const TUid& aUid, const TDesC& aResourceDir, 
								const TDesC& aFilename)
    {
    return iServer->RegisterFullscreenKml(aActionID, aUid, aResourceDir, aFilename);
    }
								
TInt CSrvImpl::UnregisterFullscreenKml(TUint aActionID, const TUid& aUid)
    {
    return iServer->UnregisterFullscreenKml(aActionID, aUid);
    }
    
TInt CSrvImpl::UnregisterAllKml()
	{
	return iServer->UnregisterAllKml();
	}

TInt CSrvImpl::BlockFullScreenUid(const TUid& aUid, TBool aBlockFrom, TBool aBlockTo)
    {
    return iServer->BlockFullScreenUid(aUid, aBlockFrom, aBlockTo);
    }

TInt CSrvImpl::RegisterControlKml(const TUid &aUid, const TDesC& aResourceDir, const TDesC& aFilename)
    {
    return iServer->RegisterControlKml(aUid, aResourceDir, aFilename);
    }
    
TInt CSrvImpl::RegisterControlKml(const TUid &aUid, const TDesC& aResourceDir, const TDesC& aFilename, 
								  TInt aWantedTime, TInt aMinTime)
    {
    return iServer->RegisterControlKml(aUid, aResourceDir, aFilename, aWantedTime, aMinTime);
    }

TInt CSrvImpl::UnregisterControlKml(const TUid &aUid)
    {
    return iServer->UnregisterControlKml(aUid);   
    }

TInt CSrvImpl::RegisterControlAction(const TUid& aUid, TUint aActionID, const TDesC& aActionString)
    {
    return iServer->RegisterControlAction(aUid, aActionID, aActionString);
    }

TInt CSrvImpl::UnregisterControlAction(const TUid& aUid, TUint aActionID)
    {
    return iServer->UnregisterControlAction(aUid, aActionID);
    }

TInt CSrvImpl::SetWantedTime(TInt aTime)
    {
    return iServer->SetWantedTime(aTime);
    }

TInt CSrvImpl::SetMinTime(TInt aTime)
    {
    return iServer->SetMinTime(aTime);
    }

TInt CSrvImpl::RegisterFullscreenKml(TUint aActionID, const TUid& aUid, const TDesC& aResourceDir, 
								const TDesC& aFilename, TInt aCachePriority, TInt aWantedTime,
								TInt aMinTime)
    {
    return iServer->RegisterFullscreenKml(aActionID, aUid, aResourceDir, 
								aFilename, aCachePriority, aWantedTime,
								aMinTime);
    }

TInt CSrvImpl::RegisterListBoxKml(const TUid& aUid, const /*TListBoxType*/ TInt aListBoxType, 
                             const TDesC& aResourceDir, 
							 const TDesC& aBackgroundFilename, 
							 const TDesC& aListItemFilename)
    {
    return iServer->RegisterListBoxKml( aUid, 
                                    (CAlfTransitionServerClient::TListBoxType)aListBoxType, 
                                    aResourceDir, 
                                    aBackgroundFilename,
                                    aListItemFilename );
                                                                   
    }

TInt CSrvImpl::UnregisterListBoxKml(const TUid& aUid, const /*TListBoxType*/ TInt aListBoxType)
    {
    return iServer->UnregisterListBoxKml( aUid, (CAlfTransitionServerClient::TListBoxType)aListBoxType );
    }

TInt CSrvImpl::SetListBoxFrameTime( const TInt aWantedTime ) 
	{
	return iServer->SetListBoxFrameTime(aWantedTime);
	}

TInt CSrvImpl::SetListBoxMinFrameTime( const TInt aMinFrameTime ) 
	{
	return iServer->SetListBoxMinFrameTime(aMinFrameTime);
	}

TInt CSrvImpl::SetControlFrameTime( const TInt aWantedTime ) 
	{
	return iServer->SetControlFrameTime(aWantedTime);
	}

TInt CSrvImpl::SetControlMinFrameTime( const TInt aMinFrameTime ) 
	{
	return iServer->SetControlMinFrameTime(aMinFrameTime);
	}
	
TBool CSrvImpl::IsConnected()
    {
    return iServer->IsConnected();
    }

CSrvImpl::~CSrvImpl()
    {
    if ( iServer )
        {
        iServer->Disconnect();
        }
    delete iServer;    
    }

void CSrvImpl::ConstructL()
    {
    iServer = new( ELeave ) CAlfTransitionServerClient();
    }


// CSrvCtrlImpl implementation

CSrvCtrlImpl* CSrvCtrlImpl::NewL( RWsSession& aWsSession )
    {
    CSrvCtrlImpl* self = new (ELeave)CSrvCtrlImpl();
    CleanupStack::PushL(self);
    self->ConstructL( aWsSession );
    CleanupStack::Pop(); //self;
    return self;
    }

TInt CSrvCtrlImpl::StartTransitionServer()
    {
    return iController->StartTransitionServer();
    }

TInt CSrvCtrlImpl::StopTransitionServer()
    {
    return iController->StopTransitionServer();
    }

TBool CSrvCtrlImpl::ServerRunning()
    {
    return iController->ServerRunning();
    }

TInt CSrvCtrlImpl::StartTransitionServer(TInt32 aMaxBytesOngoing, TInt32 aMaxBytesBetween)
    {
    return iController->StartTransitionServer(aMaxBytesOngoing, aMaxBytesBetween);
    }

CSrvCtrlImpl::~CSrvCtrlImpl()
    {
    delete iController;
    }

void CSrvCtrlImpl::ConstructL( RWsSession& aWsSession )
    {
    iController = CAlfTransitionServerController::NewL( aWsSession );
    }

