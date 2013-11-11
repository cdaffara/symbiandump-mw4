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
* Description:  Declares simple device discovery process.
*
*/


// INCLUDE FILES

#include <e32base.h>

#include <upnpdevice.h>
#include "upnpcontenthandlerscontroller.h"

#include "upnpcpbembeddeddevicedescription.h"
#include "upnpcpbdevicerepository.h"
#include "upnpcpbdevicelistutils.h"
#include "upnpcpbhttpmessagecontroller.h"

// LOGGER SETTINGS
#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbEmbeddedDeviceDescription* CUpnpCpbEmbeddedDeviceDescription::NewL(
            MUpnpCpbHttpMessageController& aMessanger,
            CUpnpCpbDeviceRepository& aRepository )
    {
    LOG_FUNC_NAME;
    CUpnpCpbEmbeddedDeviceDescription* self =
        new (ELeave) CUpnpCpbEmbeddedDeviceDescription(aMessanger, aRepository);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::ConstructL()
    {
    LOG_FUNC_NAME;
    CUpnpCpbDeviceDescription::ConstructL();

    iDiscoveredDevices.Reset();
    iDiscoveredDevices.Close();

    iAllDevicesInRoot.Reset();
    iAllDevicesInRoot.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::~CUpnpCpbEmbeddedDeviceDescription
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbEmbeddedDeviceDescription::~CUpnpCpbEmbeddedDeviceDescription()
    {
    LOG_FUNC_NAME;
    iNotNeededDevices.Reset();
    iNotNeededDevices.Close();
    iAllDevicesInRoot.Reset();
    iAllDevicesInRoot.Close();
    iDiscoveredDevices.Reset();
    iDiscoveredDevices.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::CUpnpCpbEmbeddedDeviceDescription
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbEmbeddedDeviceDescription::CUpnpCpbEmbeddedDeviceDescription(
    MUpnpCpbHttpMessageController& aMessanger,
    CUpnpCpbDeviceRepository& aRepository)
    : CUpnpCpbDeviceDescription(aMessanger, aRepository)
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::DiscoverDeviceL
// Start processing device.
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbEmbeddedDeviceDescription::DiscoverDeviceL(CUpnpDevice* aDevice)
    {
    LOG_FUNC_NAME;
    if(!aDevice)
        {
        return KErrNotFound;
        }

    iAllDevicesInRoot.Reset();
    iDiscoveredDevices.Reset();
    iNotNeededDevices.Reset();

    iRootDevice = aDevice;
    iResult = KDisscoveryFinished;
    iUuids.Zero();
    BuildDevicesListL(iRootDevice);

    // Start discovery if some device has services start to download it
    // And change state to inprogress
    for(TInt i = iAllDevicesInRoot.Count() - 1;  i > -1; i--)
        {
        if(iAllDevicesInRoot.Find(iAllDevicesInRoot[i]) != KErrNotFound)
            {
            GetServiceDescriptionL(iAllDevicesInRoot[i]);
            }
        }

    return iResult;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::GetIncompliteRootDevice
//  Append discovered devices to array
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbEmbeddedDeviceDescription::GetIncompliteRootDevice()
    {
    return iRootDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::BuildDevicesListL
// Build list with devices that should be processed
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::BuildDevicesListL(CUpnpDevice* aDevice)
    {
    if (iRepository.MatchTargetDevice(aDevice->DeviceType()))
        {
        // If device is already known (partial discovery of embedded)
        // add it to temporary discovered list
        if (iRepository.IsDiscoveredDevice(aDevice->Uuid()))
            {
            iDiscoveredDevices.AppendL(aDevice);
            }
        else
            {
            iAllDevicesInRoot.AppendL( aDevice );
            }

        // processing embedded devices
        // GetAllDevices method returns list of all devices of embedded devices tree
        RPointerArray<CUpnpDevice> devices;
        aDevice->GetAllDevices(devices);
        CleanupClosePushL(devices);
        for (TInt i(0); i < devices.Count(); i++)
            {
            if (!iRepository.FindDevice(devices[i]->Uuid()))
                {
                iAllDevicesInRoot.AppendL( devices[i] );
                }
            else
                {
                // device is known
                iDiscoveredDevices.AppendL(devices[i]);
                // if was not reported as discovered
                if (iRepository.IsUninterestingDevice(devices[i]->Uuid())
                        && iRepository.MatchTargetDevice(devices[i]->DeviceType()))
                    {
                    iRepository.RemoveDevice(devices[i]);
                    }
                }
            }
        CleanupStack::PopAndDestroy(&devices);
        }
    else
        {
        iNotNeededDevices.AppendL(aDevice);
        // Device is not interesting, process subdevices
        RPointerArray<CUpnpDevice> deviceList  = aDevice->DeviceList();
        for (TInt i(0); i < deviceList.Count(); i++)
            {
            BuildDevicesListL( deviceList[i] );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::GetServiceDescriptionL
// Start getting services desriptions
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::GetServiceDescriptionL(
    CUpnpDevice* aDevice )
    {

    // Because all service descriptions are not retrieved, retrieve next one.
    TPtrC8 buffer(GetNextServiceType(aDevice));

    if (buffer.Length())
        {
        //ask for 1st service description
        iResult = KDisscoveryInProgress;
        TInt sessionId = iMessanger.GetServiceDescriptionL(aDevice, buffer);
        // put SessionID to memory per pending Service Description request
        aDevice->WaitServiceDescriptionL( sessionId );
        }
        //no service, device is discovered
    else
        {
        iDiscoveredDevices.AppendL( aDevice );
        iAllDevicesInRoot.Remove(iAllDevicesInRoot.Find(aDevice));
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::ServiceDescription
// Function processing service description
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbEmbeddedDeviceDescription::ServiceDescriptionL(
    CUpnpHttpMessage* aMsg )
    {
    LOG_FUNC_NAME;

    // Check if device is waiting for this service
    TBool waiting(EFalse);
    if(!iRootDevice)
        {
        iResult = KErrNotFound;
        }
    else
        {
        TInt servCount(0);
        CUpnpDevice* device = NULL;
        for (TInt i(0); i<iAllDevicesInRoot.Count(); i++)
            {
            for (servCount = 0;
                 servCount < iAllDevicesInRoot[i]->WaitServiceDescriptionCount();
                 servCount++ )
                {
                if ( iAllDevicesInRoot[i]->WaitServiceDescriptionSessionId( servCount )
                         == aMsg->SessionId() )
                    {
                    device = iAllDevicesInRoot[i];
                    waiting = ETrue;
                    break;
                    }
                }
                if (device)
                    {
                    break;
                    }
            }
        // Waiting for service description
        if (waiting)
            {
            // Processing message
            if ( !aMsg->Is2xx())
                {
                LOGS("CUpnpCpbEmbeddedDeviceDescription::ServiceDescriptionL "
                     "- Wrong response recived.");
                iAllDevicesInRoot.Remove(iAllDevicesInRoot.Find(device));
                device->WaitServiceDescriptionRemoveSessionId(servCount);
                if (!iAllDevicesInRoot.Count())
                    {
                    // Function will set iResult
                    BuildResultListsL(iRootDevice);
                    CreateUuidsL();
                    }
                else
                    {
                    iResult = KDisscoveryInProgress;
                    }
                }
            else
                {
                if(device != NULL)
                    {
					iResult = ProcessServiceDescriptionL(device, aMsg);
                    }
				else
				    {
					iResult = KErrNotFound;
				    }
                }
            }
        else
            {
            iResult = KErrNotFound;
            }
        }
    return iResult;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::ServiceDescription
// Parse service description
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbEmbeddedDeviceDescription::ProcessServiceDescriptionL(
    CUpnpDevice* aDevice, CUpnpHttpMessage* aMsg)
    {
    LOG_FUNC_NAME;
    CUpnpDevice::TServiceAdd result;

    // Parsing service desription
    CUpnpService* service = NULL;

    TRAPD( error, service = iSaxController->ParseServiceL( aMsg->Body(), aDevice ) );
    if( !service || error )
        {
        // error handling section
        iAllDevicesInRoot.Remove(iAllDevicesInRoot.Find(aDevice));
        if(!iAllDevicesInRoot.Count())
            {
            // Function will set iResult
            BuildResultListsL(iRootDevice);
            CreateUuidsL();
            }
        else
            {
            iResult = KDisscoveryInProgress;
            }

            return iResult;
        }

    CleanupStack::PushL(service);
    result = aDevice->AddServiceL(aMsg->SessionId(), service);
    CleanupStack::Pop(service);

    if ( result == CUpnpDevice::EAllServicesAdded )
        { // device has received all service descriptions
        LOGS("CUpnpCpbEmbeddedDeviceDescription::ProcessServiceDescriptionL "
             "- All service info added to the parent device");
        // Process devicess
        iDiscoveredDevices.AppendL( aDevice );
        iAllDevicesInRoot.Remove(iAllDevicesInRoot.Find(aDevice));

        if(!iAllDevicesInRoot.Count())
            {
            // Function will set iResult
            BuildResultListsL(iRootDevice);
            CreateUuidsL();
            }
        else
            {
            iResult = KDisscoveryInProgress;
            }
        }
    else if ( result == CUpnpDevice::EServiceAdded )
        {
        LOGS("CUpnpCpbEmbeddedDeviceDescription::ProcessServiceDescriptionL "
             "- Service added");
        // Waiting for next desciptions.
        iResult = KDisscoveryInProgress;
        TPtrC8 buffer(GetNextServiceType(aDevice));
        TInt sessionId = iMessanger.GetServiceDescriptionL(aDevice, buffer);
         // put SessionID to memory per pending Service Description request
        aDevice->WaitServiceDescriptionL( sessionId );
        }
    else
        {
        delete service;
        iResult = KDisscoveryInProgress;
        }

    LOGS1("CUpnpCpbEmbeddedDeviceDescription::ProcessServiceDescriptionL -res=%d",
        iResult);
    return iResult;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::BuildResultListL
//  Create result list
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbEmbeddedDeviceDescription::BuildResultListsL(CUpnpDevice* aDevice)
    {
    //Devices discovered correctly will be moved to list iAllDevicesInRoot
    TBool result = ETrue;
    // Process all subdevices
    RPointerArray<CUpnpDevice> deviceList  = aDevice->DeviceList();
    for (TInt i(0); i < deviceList.Count(); i++)
        {
        result =  BuildResultListsL(deviceList[i]) && result;
        }
    // Process current device
    if (result)
        {
        if (UpnpCpbDeviceListUtils::ExistOnList(aDevice->Uuid(), iDiscoveredDevices))
            {
            if (iRepository.MatchTargetDevice(aDevice->DeviceType()))
                {
                // correctly discovered devices are now on iAllDevicesInRoot list
                iAllDevicesInRoot.AppendL(aDevice);
                iDiscoveredDevices.Remove(iDiscoveredDevices.Find(aDevice));
                }
            }
        else
            {
            result = EFalse;
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::CreateUuidsL
// Count result and create Uuids
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::CreateUuidsL()
    {
    if(!iRootDevice)
        {
        return;
        }

    RPointerArray<CUpnpDevice> devices;
    iRootDevice->GetAllDevices(devices);

    // All devices
    TInt allDevicesNumber = devices.Count() + 1;
    // All devices with processing finished correctly
    TInt correctlyProcessedDevices = iNotNeededDevices.Count() +
                iDiscoveredDevices.Count() + iAllDevicesInRoot.Count();
    CleanupClosePushL(devices);
    // all devices found
    if(allDevicesNumber == correctlyProcessedDevices)
        {
        iResult = KDisscoveryFinished;
        }
    else // Some devices are not finished correctly
        {
        iResult = KDisscoveryIncorrect;

        for (TInt i(0); i < devices.Count(); i++)
            {
            if ( !UpnpCpbDeviceListUtils::ExistOnList(devices[i]->Uuid(),
                    iNotNeededDevices )
                    && !UpnpCpbDeviceListUtils::ExistOnList(devices[i]->Uuid(),
                        iAllDevicesInRoot )
                    && !UpnpCpbDeviceListUtils::ExistOnList(devices[i]->Uuid(),
                        iDiscoveredDevices ) )
                {
                AddToUuidsL(devices[i]->Uuid());
                }
            }
        AddToUuidsL(iRootDevice->Uuid());
        // root might be only unneded, or not discovered correctly in case of wrong process
        TInt idx = iNotNeededDevices.Find(iRootDevice);
        if(idx != KErrNotFound)
            {
            iNotNeededDevices.Remove(idx);
            }
        else
            {
            idx = iDiscoveredDevices.Find(iRootDevice);
            if(idx != KErrNotFound)
                {
                iDiscoveredDevices.Remove(idx);
                }
            }
        }
    CleanupStack::PopAndDestroy(&devices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::GetDiscoveredDeviceL
//  Append discovered devices to array
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::GetDiscoveredDeviceL(
    RPointerArray<CUpnpDevice>& aArray, TBool aRemove )
    {
    if(iResult != KDisscoveryInProgress)
        {
        for(TInt i(0); i < iAllDevicesInRoot.Count(); i++)
            {
            if( iRepository.FindDevice( iAllDevicesInRoot[i]->Uuid() )
                && aRemove )
                {
                iAllDevicesInRoot.Remove( i-- );
                }
            else
                {
                aArray.AppendL(iAllDevicesInRoot[i]);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::GetUninterestingDeviceL
//  Append not discovered (does not match the type) devices to array
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::GetUninterestingDeviceL(
    RPointerArray<CUpnpDevice>& aArray , TBool aRemove )
    {
    if(iResult != KDisscoveryInProgress)
        {
        for(TInt i(0); i < iDiscoveredDevices.Count(); i++)
            {
            if( iRepository.FindDevice( iDiscoveredDevices[i]->Uuid() )
                && aRemove )
                {
                iDiscoveredDevices.Remove( i-- );
                }
            else
                {
                aArray.AppendL(iDiscoveredDevices[i]);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbEmbeddedDeviceDescription::GetUnnededDeviceL
//  Append discovered but not metching target types devices to array
// -----------------------------------------------------------------------------
//
void CUpnpCpbEmbeddedDeviceDescription::GetUnnededDeviceL(
    RPointerArray<CUpnpDevice>& aArray , TBool aRemove )
    {
    if(iResult != KDisscoveryInProgress)
        {
        for(TInt i(0); i < iNotNeededDevices.Count(); i++)
            if( iRepository.FindDevice( iNotNeededDevices[i]->Uuid() )
                && aRemove )
                {
                iNotNeededDevices.Remove( i-- );
                }
            else
                {
                aArray.AppendL(iNotNeededDevices[i]);
                }
        }
    }

//  End of File
