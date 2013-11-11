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
* Description:  Declares utils function for devices list class.
*
*/
 

// INCLUDE FILES

#include <e32base.h>

#include <upnpdevice.h>

#include "upnpcpbdevicelistutils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// UPnPCpbDeviceDistUtils::GetFromList
// Get device from list
// -----------------------------------------------------------------------------
//
CUpnpDevice* UpnpCpbDeviceListUtils::GetFromList(const TDesC8& aUuid, RPointerArray<CUpnpDevice>& aList)
    {
    CUpnpDevice* res = NULL;
    for ( TInt l = 0; l < aList.Count(); l++ )
        {
        if ( aList[l]->Uuid().Compare( aUuid )  ==  0 )
            {
            res = aList[l];
            break;
            }	
        }
    return res;	
    }
    
// -----------------------------------------------------------------------------
// UPnPCpbDeviceDistUtils::GetRootFromList
// Get device from list
// -----------------------------------------------------------------------------
//
CUpnpDevice* UpnpCpbDeviceListUtils::GetRootFromList( const TDesC8& aDeviceUuid, 
                                       RPointerArray<CUpnpDevice>& aDeviceList )
    {
    CUpnpDevice* res = NULL;
    for ( TInt l = 0; l < aDeviceList.Count(); l++ )
        {
        if ( !aDeviceList[l]->IsEmbeddDevice() )
            {            
            if ( aDeviceList[l]->Uuid().Compare( aDeviceUuid )  ==  0 )
                {
                res = aDeviceList[l];
                break;
                }
                
            RPointerArray<CUpnpDevice> devicesAll;
            aDeviceList[l]->GetAllDevices( devicesAll );                      
            for ( TInt k = 0; k < devicesAll.Count(); k++ )
                {            
                if ( aDeviceUuid.Compare( devicesAll[k]->Uuid() ) == 0 )
                    {
                    devicesAll.Close();         
                    res = aDeviceList[l];
                    break;
                    }
                }
            devicesAll.Close();
            }
        }
    return res;	
    }
 
// -----------------------------------------------------------------------------
// UPnPCPBDeviceDistUtils::ExistOnList
// Chech if device is on list
// -----------------------------------------------------------------------------
//    
TBool UpnpCpbDeviceListUtils::ExistOnList(const TDesC8& aUuid, RPointerArray<CUpnpDevice>& aList)
    {
    TBool res(EFalse);
    if(UpnpCpbDeviceListUtils::GetFromList(aUuid,aList))
        {
        res = ETrue;
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddDeviceL
// Add device to device list
// -----------------------------------------------------------------------------
// 
TBool UpnpCpbDeviceListUtils::AddDeviceL(CUpnpDevice* aDevice, RPointerArray<CUpnpDevice>& aList)
    {
    TInt error(KErrNone);
    TInt result = AddDevice(aDevice, aList, error);
    User::LeaveIfError(error);
    return result;
    };

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddDevicesL
// Add devices to device list
// -----------------------------------------------------------------------------
// 
void UpnpCpbDeviceListUtils::AddDevicesL(RPointerArray<CUpnpDevice>& aSource, RPointerArray<CUpnpDevice>& aTarget)
    {
    TInt error(KErrNone);
    for(TInt i(0); i<aSource.Count(); i++)
        {
        AddDevice(aSource[i], aTarget, error);
        if(error < KErrNone)
            {
            for(; i>0; i--)
                {
                aTarget.Remove(aTarget.Count() - 1);
                }
            break;
            }
        }
    
    User::LeaveIfError(error);
    };

// -----------------------------------------------------------------------------
// CUpnpCpbDeviceRepository::AddDevice
// Add device to device list
// -----------------------------------------------------------------------------
// 
TBool UpnpCpbDeviceListUtils::AddDevice(CUpnpDevice* aDevice, RPointerArray<CUpnpDevice>& aList, TInt& aError)
    {
    TBool result = EFalse;
    if(!UpnpCpbDeviceListUtils::ExistOnList(aDevice->Uuid(), aList))
        {
        aError = aList.Append(aDevice);
        result = ETrue;
        }
    return result;
    };

//  End of File
