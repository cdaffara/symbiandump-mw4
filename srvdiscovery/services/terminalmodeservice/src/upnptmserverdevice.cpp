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
* Description: CUpnpTmServerDevice class implementation.
*
*/

// INCLUDE FILES
#include "upnptmserverdevice.h"
#include <upnpconnectionmanagernetworkeventprovider.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmserverdeviceTraces.h"
#endif



// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDevice* CUpnpTmServerDevice::NewL( CUpnpTmServerDeviceInfo& aDeviceInfo,
                                                    CUpnpTmServerImpl& aTmServerImpl )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_NEWL_ENTRY );
    CUpnpTmServerDevice* self = new( ELeave ) CUpnpTmServerDevice();
    CleanupStack::PushL( self );
    self->ConstructL( aDeviceInfo, aTmServerImpl );
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_NEWL_EXIT );
    return self;
    }

// -------------------------------------------------------------------------------------
// CUpnpTmServerDevice::CUpnpTmServerDevice
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDevice::CUpnpTmServerDevice():iIsStarted(EFalse) 
    {
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDevice::ConstructL( CUpnpTmServerDeviceInfo& aDeviceInfo,
                                              CUpnpTmServerImpl& aTmServerImpl )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_CONSTRUCTL_ENTRY );
    iXmlParser = CUpnpTmServerDeviceXmlParser::NewL( aDeviceInfo );
    iXmlParser->StartL();
    iDescriptionProvider = CUpnpTmServerDescriptionProvider::NewL();
    
    iDescriptionStore = CUpnpDeviceDescriptionStore::NewL( iXmlParser->DevicePath() );
    // creates device
    iDevice = CUpnpDeviceImplementation::NewL( iXmlParser->DescriptionUri(), 
                                               *iDescriptionStore, 
                                               *iDescriptionProvider );
    // Creates two services
    iTmAppServerService = CUpnpTmAppServerService::NewL( *iDevice, iXmlParser->AppServerSrvPath(), aTmServerImpl );
    iTmClientProfileService = CUpnpTmClientProfileService::NewL( *iDevice, iXmlParser->ClientProfileSrvPath(),
                                                                                               aTmServerImpl );
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::~CUpnpTmServerDevice
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDevice::~CUpnpTmServerDevice()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_CUPNPTMSERVERDEVICE_ENTRY );
    delete iTmAppServerService;
    delete iTmClientProfileService;
    delete iDevice;
    delete iDescriptionStore;
    delete iDescriptionProvider;
    delete iXmlParser;
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_CUPNPTMSERVERDEVICE_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::StartTmServerDeviceL
// Starts the Terminal Mode Server Device if not started.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDevice::StartTmServerDeviceL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_STARTTMSERVERDEVICEL_ENTRY );
    if ( iIsStarted )
        {  
        // Device is already up and running
        OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_STARTTMSERVERDEVICEL_EXIT );
        return;
        }
    
   if ( !iDevice )
        {
        // Leaves if the device object is not initialised
        User::Leave(KErrNotReady);
        }
    // Starts TM Server Device
    iDevice->StartL();
    iIsStarted = ETrue;  
    OstTraceFunctionExit0( DUP1_CUPNPTMSERVERDEVICE_STARTTMSERVERDEVICEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::StopTmServerDeviceL
// Stops the Terminal Mode Server Device.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDevice::StopTmServerDeviceL()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_STOPTMSERVERDEVICEL_ENTRY );
    if ( iDevice )
        {
        iDevice->StopL();
        }
    iIsStarted = EFalse;  
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_STOPTMSERVERDEVICEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::AppStatusUpdateL
// Notifies the client about the application status updates
// @param aStatusEventBuffer buffer holding the update app status information
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDevice::AppStatusUpdateL( const TDesC8& aStatusEventBuffer )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_APPSTATUSUPDATEL_ENTRY );
    iTmAppServerService->AppStatusUpdateEventL(aStatusEventBuffer);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_APPSTATUSUPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::AppListUpdateL
// Notifies the client about the application list updates
// @param aListEventBuffer buffer holding the update app list information
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDevice::AppListUpdateL( const TDesC8& aListEventBuffer )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_APPLISTUPDATEL_ENTRY );
    iTmAppServerService->AppListUpdateEventL(aListEventBuffer);   
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_APPLISTUPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDevice::aUnusedProfileIdBuffer
// Notifies the client about the application list updates
// @param aUnusedProfileIdBuffer buffer holding the update app list information
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDevice::UnUsedProfileIdUpdateL( const TDesC8& aUnusedProfileIdBuffer )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICE_UNUSEDPROFILEIDUPDATEL_ENTRY );
    iTmClientProfileService->UnUsedProfileIdEventL(aUnusedProfileIdBuffer);   
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICE_UNUSEDPROFILEIDUPDATEL_EXIT );
    }

// End of File
