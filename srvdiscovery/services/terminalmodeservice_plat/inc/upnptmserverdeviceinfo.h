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
* Description: CUpnpTmServerDeviceInfo class declaration
*
*/

#ifndef __UPNPTMSERVERDEVICEINFO_H__
#define __UPNPTMSERVERDEVICEINFO_H__


// INCLUDES
#include <e32base.h>
#include <upnpterminalmodeicon.h>

/**
* This class represents device-specific parameters associated with 
* Terminal Mode Server Device
*/

class CUpnpTmServerDeviceInfo: public CBase
    {
public: 
	/**
    * Standard Symbian Two-phased constructor. 
    * @param aIapId IAP ID. 32 bit unsigned integer. IAP stands  for 
    *              internet access point.It is mandatory to set IAP ID
    *               
    */  
    IMPORT_C static CUpnpTmServerDeviceInfo* NewL( TUint32 aIapId );
    /**
     * Method to set the icon information associated with the Terminal Mode
     * Server Device. It provides device icon information to the TM Sevice.
     * @param aDeviceIcon Pointer to TM Icon object.
     *                    Ownership of the object is passed.
     * @see CUpnpTerminalModeIcon
     */
    IMPORT_C void AddDeviceIconL( CUpnpTerminalModeIcon* aDeviceIcon );
    /**
     * Method to set the Nokia specific additional information
     * about the terminal mode server device.
     * The entire deviceInfo element should as an xml formatted buffer should
     * be provided as an input.
     * The device info value is opaque to the TM Service.
     * @param aDeviceInfo XML formatted buffer containing deviceInfo element.
     */
    IMPORT_C void SetDeviceInfoL( const TDesC8& aDeviceInfo );
	/**
	* Method to set Bluetooth  MAC address of the TM server device. 
	* If this value is populated then it means that the device 
    *                         supports Bluetooth communication
	* @param aBtAddress Bt MAC address
	*/
    IMPORT_C void SetBtAddressL( const TDesC8& aBtAddress );
    /**
    * A setter to indicate that server device is able to initiate
    * Bluetooth connection to client or not.
    * @param aStartConn Boolean value
    */
    inline void SetStartConn( TBool aStartConn );
	/**
	* inlined "getter" methods for Terminal Mode Service's use.
	*/
    inline TUint32 IapId()const;
    inline const RPointerArray<CUpnpTerminalModeIcon>& DeviceIconList()const;
    inline const TDesC8& DeviceInfo( )const;
    inline const TDesC8& BtAddress( )const;
    inline TBool StartConn()const;       
    ~CUpnpTmServerDeviceInfo();
    
private: 
    CUpnpTmServerDeviceInfo( );
    void ConstructL( TUint32 aIapId );

private:
    TUint32                                 iIapId;
    HBufC8*                                 iDeviceInfo;
    HBufC8*                                 iBtAddr;
    TBool                                   iStartConnValue;
    RPointerArray<CUpnpTerminalModeIcon>    iDeviceIconList;
    };

#include <upnptmserverdeviceinfo.inl>

#endif // __UPNPTMSERVERDEVICEINFO_H__


// End of File
