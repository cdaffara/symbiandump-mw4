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

#include "upnpcpbdevicedescription.h"
#include "upnpcpbhttpmessagecontroller.h"

// LOGGER SETTINGS
#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

static const TInt KDefaultLenght = 200;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::~CUpnpCpbDeviceDescription
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDeviceDescription::~CUpnpCpbDeviceDescription()
    {
    LOG_FUNC_NAME;
    delete iSaxController;
    delete iRootDevice;
    iUuids.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::NullTargetDevice
// Constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceDescription::NullTargetDevice()
    {
    iRootDevice = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::NullTargetDevice
// Constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceDescription::DeleteTargetDevice()
    {
    delete iRootDevice;
    iRootDevice = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::CUpnpCpbDeviceDescription
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDeviceDescription::CUpnpCpbDeviceDescription(
    MUpnpCpbHttpMessageController& aMessanger,
    CUpnpCpbDeviceRepository& aRepository)
    : iMessanger(aMessanger), iRepository(aRepository)
    {
    LOG_FUNC_NAME;
    iResult = KDisscoveryFinished;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::FailedUuidsL
// Returns incorrect uuids
// -----------------------------------------------------------------------------
//
const TPtrC8 CUpnpCpbDeviceDescription::FailedUuidsL()
    {
    if ( !iUuids.Length() )
        {
        RPointerArray<CUpnpDevice> devices;
        iRootDevice->GetAllDevices(devices);
        CleanupClosePushL(devices);
        for (TInt i(0); i<devices.Count(); i++)
            {
            AddToUuidsL(devices[i]->Uuid());
            }
        AddToUuidsL(iRootDevice->Uuid());
        CleanupStack::PopAndDestroy(&devices);
        }
    return iUuids;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceDescription::ConstructL()
    {
    LOG_FUNC_NAME;
    // Sax paresr for service description
    iSaxController = CUpnpContentHandlersController::NewL();
    iUuids.CreateL( KDefaultLenght );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::GetTargetDevice
// Get currently processed device.
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbDeviceDescription::GetTargetDevice()
    {
    LOG_FUNC_NAME;
    return iRootDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceDescription::AddToUiidsL
//  Add next uuid to uuid list
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceDescription::AddToUuidsL(const TDesC8& aUuid)
    {
    if ( (iUuids.Length() + aUuid.Length()
            + UpnpString::KSemiColon().Length()) < iUuids.MaxLength() )
        {
        if (iUuids.Length())
            {
            iUuids.Append(UpnpString::KSemiColon());
            }
        iUuids.Append(aUuid);
        }
    else
        {
        iUuids.ReAllocL(iUuids.MaxLength() + aUuid.Length()
                + UpnpString::KSemiColon().Length());
        if (iUuids.Length())
            {
            iUuids.Append(UpnpString::KSemiColon());
            }
        iUuids.Append(aUuid);
        }
    }

// -----------------------------------------------------------------------------
// Finds next service description url to be processed.
// -----------------------------------------------------------------------------
//
const TPtrC8 CUpnpCpbDeviceDescription::GetNextServiceType(CUpnpDevice* aDevice)
    {
    // Because all service descriptions are not retrieved, retrieve next one.
    TInt serviceDescriptionsReceived = aDevice->ServiceList().Count();
    TInt j;
    for ( j=0; j < serviceDescriptionsReceived; j++)
        {
        if (aDevice->ServiceList()[j]->IsAdded() == EFalse)
            {
            return aDevice->ServiceList()[j]->ServiceDescriptionUrl();
            }
        }
    return KNullDesC8();
    }

//  End of File
