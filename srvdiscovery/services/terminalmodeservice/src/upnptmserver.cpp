/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmServer class implementation.
*
*/

//  Include Files  
#include "upnptmserver.h"   
#include "upnptmserverimpl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmserverTraces.h"
#endif

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmServer::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
EXPORT_C CUpnpTmServer* CUpnpTmServer::NewL( CUpnpTmServerDeviceInfo& aDeviceInfo,
                                            MUpnpTmServerObserver& aUpnpTmServerObserver)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_NEWL_ENTRY );
    CUpnpTmServer* self = new (ELeave) CUpnpTmServer();
    CleanupStack::PushL(self);
    self->ConstructL( aDeviceInfo, aUpnpTmServerObserver);
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMSERVER_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServer::CUpnpTmServer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmServer::CUpnpTmServer()
    {
    
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServer::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServer::ConstructL( CUpnpTmServerDeviceInfo& aDeviceInfo, MUpnpTmServerObserver& aUpnpTmServerObserver)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_CONSTRUCTL_ENTRY );
    iTmServerImpl = CUpnpTmServerImpl::NewL( aDeviceInfo, aUpnpTmServerObserver);
    OstTraceFunctionExit0( CUPNPTMSERVER_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Default Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmServer::~CUpnpTmServer()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_CUPNPTMSERVER_ENTRY );
    delete iTmServerImpl;
    OstTraceFunctionExit0( CUPNPTMSERVER_CUPNPTMSERVER_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method is used by Automotive Server to register its applications one by one.
// @param aRemotableApp Pointer to Remotable app object. Ownership is transferred.
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServer::RegisterAppL( CUpnpRemotableApp* aRemotableApp )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_REGISTERAPPL_ENTRY );
    iTmServerImpl->RegisterAppL(aRemotableApp);
    OstTraceFunctionExit0( CUPNPTMSERVER_REGISTERAPPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method is used by Automotive Server to register all the desired applications 
// in one go.
// @param aRemotableAppList Pointer Array of remotable app objects.
//                          Ownership of all the objects is passed.
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServer::RegisterAppsL(const RPointerArray<CUpnpRemotableApp>& aRemotableAppList)
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_REGISTERAPPSL_ENTRY );
    iTmServerImpl->RegisterAppsL(aRemotableAppList);
    OstTraceFunctionExit0( CUPNPTMSERVER_REGISTERAPPSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method is used by Automotive Server to un-register its applications which have 
// already been registeredone by one
// @param aAppId App ID of the application
// ---------------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpTmServer::UnRegisterApp( TUint aAppId )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_UNREGISTERAPP_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVER_UNREGISTERAPP, "CUpnpTmServer::UnRegisterApp;aAppId=%u", aAppId ); 
    return iTmServerImpl->UnRegisterApp( aAppId );
    }

// ---------------------------------------------------------------------------------
// Method is used to un-register multiple applications in one go.
// @param aAppIdArray Array of app IDs
// ---------------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpTmServer::UnRegisterApps( const RArray<TUint>& aAppIdArray )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_UNREGISTERAPPS_ENTRY );
    return iTmServerImpl->UnRegisterApps( aAppIdArray );
    }

// ---------------------------------------------------------------------------------
// Method to set the XML signature of the registered apps.
// @param aSignature XML formatted signature buffer
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServer::SetXmlSignatureL( const TDesC8& aSignature )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_SETXMLSIGNATUREL_ENTRY );
    iTmServerImpl->SetXmlSignatureL( aSignature );   
    OstTraceFunctionExit0( CUPNPTMSERVER_SETXMLSIGNATUREL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method is used by Automotive Server to start publishing the remote ui server 
// device and its service
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServer::StartL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_STARTL_ENTRY );
    iTmServerImpl->StartDeviceL();
    OstTraceFunctionExit0( CUPNPTMSERVER_STARTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method is used by Automotive Server to stop publishing the remote ui server 
// device and its service
// -------------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServer::StopL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_STOPL_ENTRY );
    iTmServerImpl->StopDeviceL();
    OstTraceFunctionExit0( CUPNPTMSERVER_STOPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method to fetch the Remotable App object by passing the App ID of the same.
// Method is invoked by the Automotive Server when it wishes to modify any of the
// existing remotable app.
// @param aAppId APP ID of the application for which caller wants the access.
// @param aErr[out] Error code. KErrNone if app is found otherwise KErrNotFound
// Returns reference to the remotable app object.
// --------------------------------------------------------------------------------- 
//
EXPORT_C CUpnpRemotableApp& CUpnpTmServer::GetRemotableApp( TUint aAppId, TInt& aErr )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_REMOTABLEAPP_ENTRY );
    OstTrace1( TRACE_NORMAL, DUP1_CUPNPTMSERVER_GETREMOTABLEAPP, "CUpnpTmServer::GetRemotableApp;aAppId =%d", aAppId  );
    return iTmServerImpl->GetRemotableApp( aAppId, aErr );  
    }

// ---------------------------------------------------------------------------------
// Method through which the notification of applications whose status 
// has changed is sent across to the Car Kit through the Service 
// @param aUpdatedAppIdList Array of app IDs which have undergone some 
//                          kind of change[ added, delete or modified ]
// ---------------------------------------------------------------------------------   
//
EXPORT_C void CUpnpTmServer::UpdateAppStatusL( const RArray<TUint>& aUpdatedAppIdList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_UPDATEAPPSTATUSL_ENTRY );
    iTmServerImpl->UpdateAppStatusL( aUpdatedAppIdList );   
    OstTraceFunctionExit0( CUPNPTMSERVER_UPDATEAPPSTATUSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method through which the notification of applications whose entries 
// in the application list have  changed is sent across to the Car Kit 
// through the Service 
// @param aUpdatedAppIdList Array of app IDs which have undergone some 
//                          kind of change[ added, delete or modified ]
// ---------------------------------------------------------------------------------   
//
EXPORT_C void CUpnpTmServer::UpdateAppListL( const RArray<TUint>& aUpdatedAppIdList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_UPDATEAPPLISTL_ENTRY );
    iTmServerImpl->UpdateAppListL( aUpdatedAppIdList );  
    OstTraceFunctionExit0( CUPNPTMSERVER_UPDATEAPPLISTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// Method through which the notification of profileIDs which are not used
// used by any Terminal Mode service hosted on the Terminal Mode device
// is sent across to the Car Kit through the Service 
// @param aUnusedProfileIdList Array of profile IDs which have undergone some 
//                             kind of change
// ---------------------------------------------------------------------------------   
//
EXPORT_C void CUpnpTmServer::UpdateUnusedProfileIdsL( const RArray<TUint>& aUnusedProfileIdList )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVER_UPDATEUNUSEDPROFILEIDSL_ENTRY );
    iTmServerImpl->UpdateUnusedProfileIdsL( aUnusedProfileIdList );   
    OstTraceFunctionExit0( CUPNPTMSERVER_UPDATEUNUSEDPROFILEIDSL_EXIT );
    }
