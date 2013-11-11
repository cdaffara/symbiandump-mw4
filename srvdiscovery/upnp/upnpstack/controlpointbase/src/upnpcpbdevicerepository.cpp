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
* Description:  Declares storage for devices known by control point.
*
*/


// INCLUDE FILES

#include <e32base.h>

#include <upnpdevice.h>

#include "upnpcpbdevicerepository.h"
#include "upnpcpbdevicelistutils.h"

#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

static const TInt KDTargetDeviceTypesGranularity = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCpbDeviceRepository* CUpnpCpbDeviceRepository::NewL(const CDesC8Array& aTargetDeviceTypes)
    {
    LOG_FUNC_NAME;
    CUpnpCpbDeviceRepository* self = new (ELeave) CUpnpCpbDeviceRepository();

    CleanupStack::PushL(self);
    self->ConstructL(aTargetDeviceTypes);
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::~CUpnpCpbDeviceRepository
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDeviceRepository::~CUpnpCpbDeviceRepository()
    {
    LOG_FUNC_NAME;

    TInt i;
    // only root devices should be deleted
    // subdevices are removed from all lists
    for(i=(iDiscoveredDevices.Count());i;i--)
        {
        if ( iDiscoveredDevices[i-1]->IsEmbeddDevice() )
            {
            iDiscoveredDevices.Remove(i-1);
            }
        }

    for(i=(iUninterestingDevices.Count());i;i--)
        {
        if (iUninterestingDevices[i-1]->IsEmbeddDevice())
            {
            iUninterestingDevices.Remove(i-1);
            }
        }

    for(i=(iUnneededDevices.Count());i;i--)
        {
        if (iUnneededDevices[i-1]->IsEmbeddDevice())
            {
            iUnneededDevices.Remove(i-1);
           }
        }

    iDiscoveredDevices.ResetAndDestroy();
    iDiscoveredDevices.Close();

    iUninterestingDevices.ResetAndDestroy();
    iUninterestingDevices.Close();

    iUnneededDevices.ResetAndDestroy();
    iUnneededDevices.Close();

    iIncomliteRootDevices.ResetAndDestroy();
    iIncomliteRootDevices.Close();

    iIncomingDevices.ResetAndDestroy();
    iIncomingDevices.Close();

    // Delete target devices list
    if ( iTargetDeviceTypes )
        {
        iTargetDeviceTypes->Reset();
        delete iTargetDeviceTypes;
        }

    LOGS("CUpnpCpbDeviceRepository::CUpnpCpbDeviceRepository - END");
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::MatchTargetDevice
// Check if device match target types
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::MatchTargetDevice(const TDesC8& aDeviceType )
    {
    TBool result = FALSE;
    if(iTargetDeviceTypes)
        {
        if((*iTargetDeviceTypes)[0].Find(_L8("*")) != KErrNotFound) 
            {
            result = TRUE;
            return result;
            }
        for(TInt i(0); i < iTargetDeviceTypes->Count(); i++)
            {
            if(aDeviceType.Find((*iTargetDeviceTypes)[i]) != KErrNotFound)
                {
                result = TRUE;
                break;
                }
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::IsKnownDevice
// Check if device is in repository
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::IsKnownDevice(const TDesC8& aDeviceUuid)
    {
    return (IsDiscoveredDevice(aDeviceUuid) || IsUninterestingDevice(aDeviceUuid)
                    || IsUnneededDevice(aDeviceUuid));
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::IsDiscoveredDevice
// Check if device is discovered already
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::IsDiscoveredDevice(const TDesC8& aDeviceUuid)
    {
    return UpnpCpbDeviceListUtils::ExistOnList(aDeviceUuid, iDiscoveredDevices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::IsUninterestingDevice
// Check if device is discovered but type is not matching
// needed devices types
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::IsUninterestingDevice(const TDesC8& aDeviceUuid)
    {
    return UpnpCpbDeviceListUtils::ExistOnList(aDeviceUuid, iUninterestingDevices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::IsUnneededDevice
// Check if device is unneded (type is different then needed).
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::IsUnneededDevice(const TDesC8& aDeviceUuid)
    {
    return UpnpCpbDeviceListUtils::ExistOnList(aDeviceUuid, iUnneededDevices);
    }


// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddDiscoveredDeviceL
// Add device to discovered device list
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::AddDiscoveredDeviceL(CUpnpDevice* aDevice)
    {
    return UpnpCpbDeviceListUtils::AddDeviceL(aDevice, iDiscoveredDevices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddUnnededDeviceL
// Add device to discovered device list
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::AddUnneddedDeviceL(CUpnpDevice* aDevice)
    {
    return UpnpCpbDeviceListUtils::AddDeviceL(aDevice, iUnneededDevices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddIncompliteDeviceL
// Add device to discovered device list
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::AddIncompliteDeviceL(CUpnpDevice* aDevice)
    {
    return UpnpCpbDeviceListUtils::AddDeviceL(aDevice, iIncomliteRootDevices);
    }


// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddIncomingDeviceL
// -----------------------------------------------------------------------------
//
TBool CUpnpCpbDeviceRepository::AddIncomingDeviceL( CUpnpDevice* aDevice )
    {
    CUpnpDevice* device = RemoveIncomingDevice( aDevice->Uuid() );
    if ( device )
        {
        delete device;
        }
    return UpnpCpbDeviceListUtils::AddDeviceL( aDevice, iIncomingDevices );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::RemoveIncomingDeviceL
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbDeviceRepository::RemoveIncomingDevice( const TDesC8& aUuid )
    {
    CUpnpDevice* res = NULL;
    for ( TInt i = 0; i < iIncomingDevices.Count(); i++ )
        {
        if ( iIncomingDevices[i]->Uuid().Compare( aUuid )  ==  0 )
            {
            res = iIncomingDevices[i];
            iIncomingDevices.Remove(i);
            break;
            }
        }
    return res;
    }



// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddUnnededDeviceL
// Add device to discovered device list
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceRepository::AddDiscoveredDevicesL(RPointerArray<CUpnpDevice>& aDevices)
    {
    UpnpCpbDeviceListUtils::AddDevicesL(aDevices, iDiscoveredDevices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddUnnededDeviceL
// Add device to uninteresting device list
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceRepository::AddUninterestingDevicesL(RPointerArray<CUpnpDevice>& aDevices)
    {
    UpnpCpbDeviceListUtils::AddDevicesL(aDevices, iUninterestingDevices);
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddUnnededDeviceL
// Add device to unneded device list
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceRepository::AddUnneddedDevicesL(RPointerArray<CUpnpDevice>& aDevices)
    {
    UpnpCpbDeviceListUtils::AddDevicesL(aDevices, iUnneededDevices);
    }

// -----------------------------------------------------------------------------
// Return discovered device list
// -----------------------------------------------------------------------------
//
const RPointerArray<CUpnpDevice>& CUpnpCpbDeviceRepository::DiscoveredDeviceList() const
    {
    return iDiscoveredDevices;
    }

// -----------------------------------------------------------------------------
// Return discovered device list
// -----------------------------------------------------------------------------
//
const RPointerArray<CUpnpDevice>& CUpnpCpbDeviceRepository::UnneddedDeviceList() const
    {
    return iUnneededDevices;
    }

// -----------------------------------------------------------------------------
// Return discovered device list
// -----------------------------------------------------------------------------
//
const RPointerArray<CUpnpDevice>& CUpnpCpbDeviceRepository::UninterestingDeviceList() const
    {
    return iUninterestingDevices;
    }

// -----------------------------------------------------------------------------
// Return discovered device list
// -----------------------------------------------------------------------------
//
const RPointerArray<CUpnpDevice>& CUpnpCpbDeviceRepository::IncompliteDeviceList() const
    {
    return iIncomliteRootDevices;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::FindIncompliteDevice
// Find incomplite device in repository
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbDeviceRepository::FindIncompliteDevice(const TDesC8& aDeviceUuid)
    {
    CUpnpDevice* res = NULL;
    res = UpnpCpbDeviceListUtils::GetFromList(aDeviceUuid, iIncomliteRootDevices);
    return res;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::FindDevice
// Find device in repository
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbDeviceRepository::FindDevice(const TDesC8& aDeviceUuid)
    {
    CUpnpDevice* res = NULL;
    res = UpnpCpbDeviceListUtils::GetFromList(aDeviceUuid, iDiscoveredDevices);
    if(res)
        {
        return res;
        }
    res = UpnpCpbDeviceListUtils::GetFromList(aDeviceUuid, iUnneededDevices);
    if(res)
        {
        return res;
        }
    res = UpnpCpbDeviceListUtils::GetFromList(aDeviceUuid, iUninterestingDevices);
    if(res)
        {
        return res;
        }
    res = UpnpCpbDeviceListUtils::GetFromList(aDeviceUuid, iIncomliteRootDevices);
    if(res)
        {
        return res;
        }
    return res;
    }

// -----------------------------------------------------------------------------
// Finds root of given device among dicovered and being discovered devices
// -----------------------------------------------------------------------------
//
CUpnpDevice* CUpnpCpbDeviceRepository::FindRoot( const TDesC8& aDeviceUuid )
    {
    CUpnpDevice* res = NULL;
    res = UpnpCpbDeviceListUtils::GetRootFromList( aDeviceUuid, iDiscoveredDevices );
    if ( res )
        {
        return res;
        }
    res = UpnpCpbDeviceListUtils::GetRootFromList( aDeviceUuid, iUnneededDevices );
    if ( res )
        {
        return res;
        }
    res = UpnpCpbDeviceListUtils::GetRootFromList( aDeviceUuid, iUninterestingDevices );
    if ( res )
        {
        return res;
        }
    res = UpnpCpbDeviceListUtils::GetRootFromList( aDeviceUuid, iIncomliteRootDevices );
    if ( res )
        {
        return res;
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::RemoveDevice
// Remove device from all list
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceRepository::RemoveDevice(CUpnpDevice* aDevice)
    {
    TInt i = iDiscoveredDevices.Find(aDevice);
    if (i !=KErrNotFound)
        {
        iDiscoveredDevices.Remove( i );
        }
    else
        {
        // check unneded device list
        i = iUnneededDevices.Find(aDevice);
        if ( i != KErrNotFound)
            {
            iUnneededDevices.Remove( i );
            }
        else
            {
            // check unninteresting device list
            i = iUninterestingDevices.Find(aDevice);
            if ( i != KErrNotFound)
                {
                iUninterestingDevices.Remove( i );
                }
            else
                {
                // check root devices with errors
                i = iIncomliteRootDevices.Find(aDevice);
                if ( i != KErrNotFound)
                    {
                    iIncomliteRootDevices.Remove( i );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::CUpnpCpbDeviceRepository
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDeviceRepository::CUpnpCpbDeviceRepository()
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDeviceRepository::ConstructL(const CDesC8Array& aTargetDeviceTypes)
    {
    LOG_FUNC_NAME;
    // coping target device
    iTargetDeviceTypes = new( ELeave ) CDesC8ArrayFlat( KDTargetDeviceTypesGranularity );
    for ( TInt i( 0 ); i < aTargetDeviceTypes.Count(); i++ )
        {
        iTargetDeviceTypes->AppendL( aTargetDeviceTypes[i]);
        }
    }

//  End of File
