/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares ControlPoint Discovery class.
*
*/


// INCLUDE FILES
#include <e32base.h>

#include <upnpdevice.h>
#include <upnpdevicedescription.h>

#include <upnphttpmessagefactory.h>

#include "upnpcontrolpoint.h"
#include "upnpcpbdescriptionagent.h"
#include "upnpcpbdevicerepository.h"
#include "upnpcpbsimpledevicedescription.h"
#include "upnpcpbembeddeddevicedescription.h"
#include "upnpcpbhttpmessagecontroller.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpcommonupnplits.h"
#include "upnpcpstackrequestor.h"

#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

// Time window reserved for single device discovery
static const TInt KDeviceDiscoveryTimeout = 30000000;

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDescriptionAgent* CUpnpCpbDescriptionAgent::NewL(
        CUpnpControlPoint& aBaseCP,
        MUpnpCpbHttpMessageController& aMessanger,
        CUpnpCpbDeviceRepository& aRepository)
    {
    LOG_FUNC_NAME;
    CUpnpCpbDescriptionAgent* self =
        new (ELeave) CUpnpCpbDescriptionAgent(
            aBaseCP, aMessanger, aRepository );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::~CUpnpCpbDescriptionAgent
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDescriptionAgent::~CUpnpCpbDescriptionAgent()
    {
    LOG_FUNC_NAME;

    iIncomingDevices.ResetAndDestroy();
    iIncomingDevices.Close();

    if (iTimer)
        {
        iTimer->Cancel();
        }
    delete iTimer;

    iUuid.Close();
    delete iSaxController;
    delete iSimpleDiscoveryEngine;
    delete iEmbeddedDiscoveryEngine;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::CUpnpCpbDescriptionAgent
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDescriptionAgent::CUpnpCpbDescriptionAgent(
        CUpnpControlPoint& aBaseCP,
        MUpnpCpbHttpMessageController& aMessanger,
        CUpnpCpbDeviceRepository& aRepository )
:    iBaseCP( aBaseCP ), iMessanger( aMessanger ), iDeviceRepository( aRepository),
     iPendingDiscovery( EFalse ), iDescriptionSession( KErrNotFound )
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::ConstructL()
    {
    LOG_FUNC_NAME;
    iSimpleDiscoveryEngine =
        CUpnpCpbSimpleDeviceDescription::NewL(iMessanger, iDeviceRepository);
    iEmbeddedDiscoveryEngine =
        CUpnpCpbEmbeddedDeviceDescription::NewL(iMessanger, iDeviceRepository);
    // Sax paresr for device description
    iSaxController = CUpnpContentHandlersController::NewL();

    iTimer = CUpnpNotifyTimer::NewL(this);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::DeviceAliveNotificationL
// New device found
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::DeviceAliveNotificationL( CUpnpDevice* aDevice )
    {
    LOG_FUNC_NAME;

    TPtrC8 deviceType;
    deviceType.Set( aDevice->DeviceType() );

    if ( (iDeviceRepository.MatchTargetDevice(deviceType)
            || (UpnpSSDP::KUPnPRootDevice().Find( deviceType ) != KErrNotFound))
            && !iDeviceRepository.IsKnownDevice( aDevice->Uuid() ) )
        {
        CUpnpDevice* tmp = aDevice->CloneL();
        CleanupStack::PushL( tmp );

        if ( iPendingDiscovery )
            {
            iIncomingDevices.AppendL(tmp);
            }
        else
            {
            GetDeviceDescriptionL( tmp );
            }

        CleanupStack::Pop(tmp);
        }
    else
        {
        CUpnpDevice* device = iDeviceRepository.FindDevice( aDevice->Uuid() );
        if ( device )
            {
            device->SetExpired( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::ProcessDeviceMessageL
// Process device message
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::ProcessDeviceMessageL(
    CUpnpHttpMessage* aHttpMessage )
    {
    if( iDescriptionSession != aHttpMessage->SessionId() )
        {
        return;
        }

    iDescriptionSession = KErrNotFound;
    // No response
    if ( !aHttpMessage->Is2xx() )
        {
        LOGS("CUpnpCpbDescriptionAgent::ProcessDeviceMessageL - "
             "Request timed out. Max Retries reached. Ignoring device.");
        if( iUuid.Length() )
            {
            iTimer->Cancel();
            iPendingDiscovery = EFalse;
            StopIgnoringUuidL(iUuid);
            }
        DiscoverNextDeviceL();
        }
    // Process description
    else
        {
        ConstructAndValidateDeviceTreeL( aHttpMessage->Body( ),
            aHttpMessage->Sender( ), aHttpMessage->SenderPath( ) );
        }
    }


// -----------------------------------------------------------------------------
//  CUpnpCpbDescriptionAgent::ConstructAndValidateDeviceTreeL
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::ConstructAndValidateDeviceTreeL(
    const TDesC8& aDeviceDescXml, const TInetAddr& aAddr,
    const TDesC8& aDeviceDescPath )
    {
    CUpnpDevice* device = NULL;
    TInt error;
    if(iDeviceRepository.MatchTargetDevice(_L8("*")))
    	{
    	CUpnpDeviceDescription* deviceDescription =new CUpnpDeviceDescription();
		TRAP( error, iSaxController->ParseDeviceL( aDeviceDescXml,deviceDescription ) );
		if(!error)
		    {
		    CleanupStack::PushL( deviceDescription );
            RBuf8 description;
            if(deviceDescription->UrlBase().Length()!=0)
                {
                description.CreateL(aDeviceDescPath.Length()+deviceDescription->UrlBase().Length());
                description.Copy(deviceDescription->UrlBase());
                TInt firstSlash(aDeviceDescPath.Locate( KSlash8()[0] ) );		
                if(firstSlash==0)
                    description.Append( aDeviceDescPath.Mid( firstSlash+1,aDeviceDescPath.Length()-1 ) );
                else
                    description.Append(aDeviceDescPath);
                deviceDescription->SetDescriptionUrlL(description);
                description.Close();
                }
            else
                {
                TBuf<25> ipBuf;
                description.CreateL(KUrlMaxLength);
                description.Copy(KHttp());
                
                aAddr.Output ( ipBuf);
                description.Append(ipBuf);
                description.Append(_L8(":"));
                ipBuf.Num(aAddr.Port());
                description.Append(ipBuf);
                description.Append(aDeviceDescPath);
                deviceDescription->SetDescriptionUrlL(description);
                description.Close();
                }
            CleanupStack::Pop(deviceDescription);
            device=deviceDescription;  	
		    }
    	}
	else
	    {
	    TRAP( error, device = iSaxController->ParseDeviceL( aDeviceDescXml ) );
	    }    	

   if ( error || ( !device ) )      // Description is wrong
        {
        if( iUuid.Length() )
            {
            iTimer->Cancel();
            iPendingDiscovery = EFalse;
            StopIgnoringUuidL(iUuid);
            }
        DiscoverNextDeviceL();
        return;
        }

    if ( aDeviceDescPath.Length() != 0 )
        {
        SetUrlBaseL( device, aDeviceDescPath );
        }

    // Device is invalid
    if( !IsDeviceValid( device ) )
        {
        delete device;
        if( iUuid.Length() )
            {
            iTimer->Cancel();
            iPendingDiscovery = EFalse;
            StopIgnoringUuidL(iUuid);
            }
        DiscoverNextDeviceL();
        return;
        }

    // Check if device is know now
    if( iDeviceRepository.IsKnownDevice( device->Uuid() ))
        {
        delete device;
        iTimer->Cancel();
        iPendingDiscovery = EFalse;
        DiscoverNextDeviceL();
        return;
        }
    // Is device partialy discovered
    CUpnpDevice* oldDevice =
        iDeviceRepository.FindIncompliteDevice( device->Uuid() );
    if( oldDevice )
        {
        iDeviceRepository.RemoveDevice( oldDevice );
        delete device;
        device = oldDevice;
        }

    RPointerArray<CUpnpDevice> devices;
    device->GetAllDevices( devices );
    devices.AppendL( device );

    CUpnpDevice::TUpnpDeviceNetwork network = FindNetworkForDeviceTree( devices );

    for (TInt i = 0; i < devices.Count(); i++ )
        {
        devices[i]->SetAddress( aAddr );
        devices[i]->SetDeviceNetwork( network );
        }
    devices.Reset();
    devices.Close();

    ChooseDesriptionProcesor(device);
    HandleDiscoveryResultL(iDiscoveryEngine->DiscoverDeviceL(device));
    }

// -----------------------------------------------------------------------------
// This function sets UrlBase value based on the relative path of the description URL
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::SetUrlBaseL(
    CUpnpDevice* aDevice, const TDesC8& aPath )
    {
    if ( aDevice->UrlBase().Length( ) == 0 )
        {
        TInt firstSlash(aPath.Locate( KSlash8()[0] ) );
        TInt lastSlash(aPath.LocateReverse( KSlash8()[0] ) );
        if ( firstSlash == lastSlash )
            {
            aDevice->SetUrlBaseL( KSlash8( ) );
            }
        else
            {
            aDevice->SetUrlBaseL( aPath.Mid( firstSlash, lastSlash ) );
            }
        }
    TPtrC8 baseURL(aDevice->UrlBase( ) );

    RPointerArray<CUpnpDevice> devices;
    aDevice->GetAllDevices( devices );
    CleanupClosePushL( devices );

    for ( TInt i(0); i< devices.Count( ); i++ )
        {
        if ( devices[i]->UrlBase().Length( ) == 0 )
            {
            devices[i]->SetUrlBaseL( baseURL );
            }
        }
    CleanupStack::PopAndDestroy( &devices );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::ChooseDesriptionProcesor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::ChooseDesriptionProcesor(CUpnpDevice* aDevice)
    {
    if( aDevice->DeviceList().Count() )
        {
        iDiscoveryEngine = iEmbeddedDiscoveryEngine;
        }
    else
        {
        iDiscoveryEngine = iSimpleDiscoveryEngine;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::FindNetworkForDeviceTree
// -----------------------------------------------------------------------------
//
CUpnpDevice::TUpnpDeviceNetwork CUpnpCpbDescriptionAgent::FindNetworkForDeviceTree(
    RPointerArray<CUpnpDevice>& aDevices )
    {
    CUpnpDevice::TUpnpDeviceNetwork network = CUpnpDevice::EUnknown;
    for (TInt i = 0; i < aDevices.Count(); i++ )
        {
        CUpnpDevice* ssdpDevice =
            iDeviceRepository.RemoveIncomingDevice( aDevices[i]->Uuid() );
        if ( ssdpDevice )
            {
            aDevices[i]->SetExpired( EFalse );
            network = ssdpDevice->DeviceNetwork();
            delete ssdpDevice;
            if ( network != CUpnpDevice::EUnknown )
                {
                break;
                }
            }
        }
    return network;
    }


// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::ProcessServiceMessageL
//  Process service message
// -----------------------------------------------------------------------------
//
 void CUpnpCpbDescriptionAgent::ProcessServiceMessageL(
    CUpnpHttpMessage* aHttpMessage)
    {
    if(iDiscoveryEngine)
        {
        HandleDiscoveryResultL(
            iDiscoveryEngine->ServiceDescriptionL(aHttpMessage) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::DiscoverNextDeviceL
// Start next device discovery
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::DiscoverNextDeviceL()
    {
    // search for new target device
    while( iIncomingDevices.Count() > 0 )
        {
        CUpnpDevice* tempDevice = iIncomingDevices[0];
        iIncomingDevices.Remove(0);
        CleanupStack::PushL( tempDevice );

        CUpnpDevice* tempDevice2 =
            iDeviceRepository.FindDevice( tempDevice->Uuid() );

        if( !tempDevice2 )
            {
            GetDeviceDescriptionL( tempDevice );
            CleanupStack::Pop( tempDevice );
            break;
            }
        else
            {
            tempDevice2->SetExpired( EFalse );
            CleanupStack::PopAndDestroy( tempDevice );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::GetDeviceDescriptionL
// Send get message
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::GetDeviceDescriptionL( CUpnpDevice* aDevice )
    {
    iUuid.Close();
    iUuid.CreateL( aDevice->Uuid());
    if( iDiscoveryEngine )
        {
        iDiscoveryEngine->DeleteTargetDevice();
        iDiscoveryEngine = NULL;
        }

    iDeviceRepository.AddIncomingDeviceL( aDevice );

    iTimer->Start( KDeviceDiscoveryTimeout );
    iPendingDiscovery = ETrue;

    iDescriptionSession = iMessanger.GetDeviceDescriptionL( aDevice );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::HandleDiscoveryResultL
// Function processing disscovery result
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::HandleDiscoveryResultL(TInt aResult)
    {
    if(aResult == KErrNotFound || aResult == KDisscoveryInProgress)
        {
        return;
        }
    iTimer->Cancel();
    iPendingDiscovery = EFalse;

    TBool result;
    if(aResult == KDisscoveryFinished)
        {
        result = CopyResult();
        if(!result)
            {
            StopIgnoringUuidL(iDiscoveryEngine->FailedUuidsL());
            }
        else
            {
            iDiscoveryEngine->NullTargetDevice();
            ReportDiscoveredDevicesL();
            }
        DiscoverNextDeviceL();
        }
    else if(aResult == KDisscoveryIncorrect)
        {
        result = CopyResult();
        if(!result)
            {
            StopIgnoringUuidL(iDiscoveryEngine->GetTargetDevice()->Uuid());
            }
        else
            {
            StopIgnoringUuidL(iDiscoveryEngine->FailedUuidsL());
            CUpnpDevice* dev = iDiscoveryEngine->GetIncompliteRootDevice();
            if(dev)
                {
                iDeviceRepository.AddIncompliteDeviceL(dev);
                }
            iDiscoveryEngine->NullTargetDevice();
            ReportDiscoveredDevicesL();
            }
        DiscoverNextDeviceL();
        }
    };

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::CopyResultL
// Copy devices to repository
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDescriptionAgent::CopyResult()
    {
    TRAPD(error, CopyResultL());
    if(error)
        {
        RPointerArray<CUpnpDevice> devices;
        iDiscoveryEngine->GetTargetDevice()->GetAllDevices(devices);
        for (TInt i(0); i < devices.Count(); i++)
            {
            iDeviceRepository.RemoveDevice(devices[i]);
            }
        iDeviceRepository.RemoveDevice(iDiscoveryEngine->GetTargetDevice());
        devices.Close();
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::CopyResultL
// Copy devices to repository
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::CopyResultL()
    {
    RPointerArray<CUpnpDevice> devices;
    CleanupClosePushL(devices);
    iDiscoveryEngine->GetUninterestingDeviceL(devices, ETrue);
    iDeviceRepository.AddUninterestingDevicesL(devices);
    devices.Reset();
    iDiscoveryEngine->GetUnnededDeviceL(devices, ETrue);
    iDeviceRepository.AddUnneddedDevicesL(devices);
    devices.Reset();
    iDiscoveryEngine->GetDiscoveredDeviceL(devices, ETrue);
    iDeviceRepository.AddDiscoveredDevicesL(devices);
    CleanupStack::PopAndDestroy(&devices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::ReportDiscoveredDevicesL
// Report discovered devices to the client
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::ReportDiscoveredDevicesL()
    {
    RPointerArray<CUpnpDevice> devices;
    CleanupClosePushL(devices);
    iDiscoveryEngine->GetDiscoveredDeviceL(devices);
    for (TInt i(0); i < devices.Count(); i++)
        {
        iBaseCP.DeviceDiscovered(devices[i]);
        }
    CleanupStack::PopAndDestroy(&devices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::StopIgnoringUidL
// Send stop filtering message to MH
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::StopIgnoringUuidL(const TDesC8& aUuids)
    {
    iMessanger.StopIgnoringUuidL(aUuids);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::IsDeviceValid
// Check if device is valid
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDescriptionAgent::IsDeviceValid( CUpnpDevice* aDevice )
    {
    const TDesC8& targetUuid = aDevice->Uuid();
       return !( aDevice->DeviceType() == KNullDesC8()
               || targetUuid == KNullDesC8() );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDescriptionAgent::TimerEventL
// Callback function
// -----------------------------------------------------------------------------
//
void CUpnpCpbDescriptionAgent::TimerEventL( CUpnpNotifyTimer* aTimer )
    {
    if( !(aTimer->iStatus == KErrNone) )
        {
        return;
        }

    if ( iPendingDiscovery )
        {
        iPendingDiscovery = FALSE;
        if(iDiscoveryEngine)
            {
            StopIgnoringUuidL(iDiscoveryEngine->FailedUuidsL());
            DiscoverNextDeviceL();
            }
        else
            {
            if( iUuid.Length() )
                {
                StopIgnoringUuidL(iUuid);
                }
            }
        }
    }

//  End of File
