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
* Description: 
*
*/

#ifndef __UPNPTMSERVERDEVICEINFO_INL__
#define __UPNPTMSERVERDEVICEINFO_INL__

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::SetStartConn
// Method to set the boolean value which indicates whether the server device 
// is able to initiate bluetooth connection to client or not.
// @param aStartConn Boolean value ( True or false )
// ---------------------------------------------------------------------------------
//
inline void CUpnpTmServerDeviceInfo::SetStartConn( TBool aStartConn )
    {
    iStartConnValue = aStartConn;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::IapId
// Method is used to retrieve IAP ID.
// ---------------------------------------------------------------------------------
//
inline TUint32 CUpnpTmServerDeviceInfo::IapId()const
    {
    return iIapId;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::DeviceIconList
// Method is used to retrieve an array of device icons.
// ---------------------------------------------------------------------------------
//
inline const RPointerArray<CUpnpTerminalModeIcon>& CUpnpTmServerDeviceInfo::DeviceIconList()const
    {
    return iDeviceIconList;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::DeviceInfo
// Method is used to retrieve additional information related to the device.
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpTmServerDeviceInfo::DeviceInfo()const
    {
    if ( iDeviceInfo )
        {
        return *iDeviceInfo;
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::BtAddress
// Method is used to retrieve the BT MAC address of the TM server device.
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpTmServerDeviceInfo::BtAddress()const
    {
    if ( iBtAddr )
        {
        return *iBtAddr;
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::StartConn
// Method is used to fetch the the boolean value for BT start connection preferences.
// ---------------------------------------------------------------------------------
//
inline TBool CUpnpTmServerDeviceInfo::StartConn()const
    {
    return iStartConnValue;
    }

#endif  //__UPNPTMSERVERDEVICEINFO_INL__
