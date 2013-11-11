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

#include "upnpcpbsimpledevicedescription.h"
#include "upnpcpbhttpmessagecontroller.h"
#include "upnpcpbdevicerepository.h"

// LOGGER SETTINGS
#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbSimpleDeviceDescription* CUpnpCpbSimpleDeviceDescription::NewL(
        MUpnpCpbHttpMessageController& aMessanger,
        CUpnpCpbDeviceRepository& aRepository)
    {
    LOG_FUNC_NAME;
    CUpnpCpbSimpleDeviceDescription* self =
        new (ELeave) CUpnpCpbSimpleDeviceDescription(aMessanger, aRepository);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::~CUpnpCpbSimpleDeviceDescription
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbSimpleDeviceDescription::~CUpnpCpbSimpleDeviceDescription()
    {
    LOG_FUNC_NAME;
    if(!iIsNull)
        {
        delete iRootDevice;
        }
    iRootDevice = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::CUpnpCpbSimpleDeviceDescription
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbSimpleDeviceDescription::CUpnpCpbSimpleDeviceDescription(
        MUpnpCpbHttpMessageController& aMessanger,
        CUpnpCpbDeviceRepository& aRepository)
    : CUpnpCpbDeviceDescription(aMessanger, aRepository)
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::DiscoverDeviceL
// Start processing device.
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbSimpleDeviceDescription::DiscoverDeviceL(CUpnpDevice* aDevice)
    {
    LOG_FUNC_NAME;
    iIsNull = EFalse;
    iRootDevice = aDevice;
    // check if device match the type
    if(!iRepository.MatchTargetDevice(aDevice->DeviceType()))
        {
        iResult = KDisscoveryFinished;
        return iResult;
        }

    iResult = KDisscoveryInProgress;
    TPtrC8 buffer(GetNextServiceType(iRootDevice));

    if (buffer.Length())
        {
        LOGS("CUpnpCpbSimpleDeviceDescription::DiscoverDeviceL - Discovering services");

        //ask for 1st service description
        iResult = KDisscoveryInProgress;
        TInt sessionId = iMessanger.GetServiceDescriptionL (aDevice, buffer);
        // put SessionID to memory per pending Service Description request
        aDevice->WaitServiceDescriptionL( sessionId );
        }
    else
        {
        LOGS("CUpnpCpbSimpleDeviceDescription::DiscoverDeviceL - "
             "No service info for the device");
        LOGS("CUpnpCpbSimpleDeviceDescription::DiscoverDeviceL - "
             "All device info brought - device officially discovered");
        iResult = KDisscoveryFinished;
        }
    return iResult;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::ServiceDescription
// Function processing service description
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbSimpleDeviceDescription::ServiceDescriptionL(CUpnpHttpMessage* aMsg)
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
        for( TInt servCount(0);
                servCount < iRootDevice->WaitServiceDescriptionCount(); servCount++)
            {
            if ( iRootDevice->WaitServiceDescriptionSessionId(servCount )
                     == aMsg->SessionId())
                {
                waiting = ETrue;
                break;
                }
            }
        // Waiting for service description
        if(waiting)
            {
            // Processing message
            if ( !aMsg->Is2xx())
                {
                LOGS("CUpnpCpbSimpleDeviceDescription::ServiceDescriptionL - "
                     "Wrong response recived.");
                iResult = KDisscoveryIncorrect;
                }
            else
                {
                iResult = ProcessServiceDescriptionL(aMsg);
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
// CUpnpCpbSimpleDeviceDescription::ServiceDescription
// Parse service description
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbSimpleDeviceDescription::ProcessServiceDescriptionL(CUpnpHttpMessage* aMsg)
    {
    LOG_FUNC_NAME;
    CUpnpDevice::TServiceAdd result;

    // Parsing service desription
    CUpnpService* service = NULL;
    service = iSaxController->ParseServiceL(aMsg->Body(), iRootDevice);

    CleanupStack::PushL(service);
    result = iRootDevice->AddServiceL(aMsg->SessionId(), service);
    CleanupStack::Pop(service);

    if ( result == CUpnpDevice::EAllServicesAdded )
        { // device has received all service descriptions
        LOGS("CUpnpCpbSimpleDeviceDescription::ProcessServiceDescriptionL  - "
             "All service info added to the parent device");
        iResult = 	KDisscoveryFinished;
        }
    else if ( result == CUpnpDevice::EServiceAdded )
        {
        LOGS("CUpnpCpbSimpleDeviceDescription::ProcessServiceDescriptionL - "
             "Service added");
        // Waiting for next desciptions.
        iResult = 	KDisscoveryInProgress;

        TPtrC8 buffer(GetNextServiceType(iRootDevice));
        TInt sessionId = iMessanger.GetServiceDescriptionL (iRootDevice,
                                  buffer);
        // put SessionID to memory per pending Service Description request
        iRootDevice->WaitServiceDescriptionL( sessionId );
        }
    else
        {
        delete service;
        iResult = KDisscoveryInProgress;
        }

    LOGS1("CUpnpCpbSimpleDeviceDescription::ProcessServiceDescriptionL -res=%d", iResult);
    return iResult;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::GetIncompliteRootDevice
//  Append discovered devices to array
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbSimpleDeviceDescription::GetIncompliteRootDevice()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::NullTargetDevice
// Constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbSimpleDeviceDescription::NullTargetDevice()
    {
    if (iResult == KDisscoveryFinished)
        {
        iIsNull = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::NullTargetDevice
// Constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbSimpleDeviceDescription::DeleteTargetDevice()
    {
    if (!iIsNull || iResult == KDisscoveryIncorrect)
        {
        delete iRootDevice;
        }
    iRootDevice = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::GetDiscoveredDeviceL
//  Append discovered devices to array
// -----------------------------------------------------------------------------
//
void CUpnpCpbSimpleDeviceDescription::GetDiscoveredDeviceL(
    RPointerArray<CUpnpDevice>& aArray, TBool /*aRemove*/ )
    {
    if (iRootDevice && iRepository.MatchTargetDevice(iRootDevice->DeviceType()))
        {
        if(iResult == KDisscoveryFinished)
            {
            aArray.AppendL(iRootDevice);
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::GetUninterestingDeviceL
//  Append not discovered (does not match the type) devices to array
// -----------------------------------------------------------------------------
//
void CUpnpCpbSimpleDeviceDescription::GetUninterestingDeviceL(
    RPointerArray<CUpnpDevice>& aArray, TBool /*aRemove*/ )
    {
    if (iRootDevice && !iRepository.MatchTargetDevice(iRootDevice->DeviceType()))
        {
        if (iResult == KDisscoveryFinished)
            {
            aArray.AppendL(iRootDevice);
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::GetUnnededDeviceL
//  Append discovered but not matching target types devices to array
// -----------------------------------------------------------------------------
//
void CUpnpCpbSimpleDeviceDescription::GetUnnededDeviceL(
    RPointerArray<CUpnpDevice>& /*aArray*/, TBool /*aRemove*/ )
    {
    // nothing to do, in case of simple device discovered are only matching type devices
    }

// -----------------------------------------------------------------------------
// CUpnpCpbSimpleDeviceDescription::FailedUuids
//  Returns incorrect uuids
// -----------------------------------------------------------------------------
//
const TPtrC8 CUpnpCpbSimpleDeviceDescription::FailedUuidsL()
    {
    if (iRootDevice)
        {
        return iRootDevice->Uuid();
        }
    else
        {
        return KNullDesC8();
        }
    }

//  End of File
