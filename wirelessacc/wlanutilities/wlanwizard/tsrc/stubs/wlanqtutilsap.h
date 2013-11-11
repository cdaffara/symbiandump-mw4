/*
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
* This is the header file for WlanQtUtilsWlanAp class.
*/

#ifndef STUB_WLANQTUTILSWLANAP_H
#define STUB_WLANQTUTILSWLANAP_H

#include <QObject>
#include <QMetaType>
#include <QVariant>
#include "cmmanagerdefines_shim.h"

class QString;
class WlanQtUtilsApPrivate;

/**
 * This class represents WLAN access point (AP).
 */
class WlanQtUtilsAp : public QObject
{
public:
    
    // Data types
    
    /*!
        Configuration IDs that can be read and set in this class.

        ConfID's available for scan results:
        - ConfIdName
        - ConfIdSsid
        - ConfIdBssid
        - ConfIdSignalStrength
        - ConfIdConnectionMode
        - ConfIdSecurityMode
        - ConfIdWpaPskUse
        - ConfIdWpsSupported

        ConfID's for IAP creation and updates, these are
        mandatory for all security modes: 
        - ConfIdName
        - ConfIdSsid
        - ConfIdConnectionMode
        - ConfIdSecurityMode
        - ConfIdWepKey1     (Empty if not used) 
        - ConfIdWepKey2     (Empty if not used)
        - ConfIdWepKey3     (Empty if not used)
        - ConfIdWepKey4     (Empty if not used)
        - ConfIdWepDefaultIndex
        - ConfIdWpaPskUse
        - ConfIdWpaPsk      (Empty if not used)
        - ConfIdHidden
        - ConfIdWlanScanSSID
        
        Values available for IAPs when reading:
        - ConfIdIapId
        - ConfIdName
        - ConfIdSsid
        - ConfIdNetworkId
        - ConfIdConnectionMode
        - ConfIdSecurityMode
        - ConfIdWpaPskUse
    */
    enum ConfId {
        //! QString: Name
        ConfIdName = 1,
        //! QByteArray: WLAN Network Name (SSID)
        ConfIdSsid = 2,
        //! QByteArray: Basic Service Set Identifier
        ConfIdBssid = 3,
        //! int: signal strength in RSSI (dBm)
        ConfIdSignalStrength = 4,
        //! int: (CMManagerShim::WlanNetMode)
        ConfIdConnectionMode = 5,
        //! int: IAP ID
        ConfIdIapId = 6,
        //! int: Network ID
        ConfIdNetworkId = 7,
        //! int: (CMManagerShim::WlanSecMode)
        ConfIdSecurityMode = 8,
        //! bool: true - WPA PSK, false - WPA EAP
        ConfIdWpaPskUse = 9,
        //! QString: Pre-Shared Key for WPA
        //! Length: 8-63 - Ascii key
        //! Length: 64 - Hex key
        ConfIdWpaPsk = 10,
        //! QString: WEP Key for index 1:
        //! HEX: 
        //! - 64 bit: allowed key length = 10
        //! - 128 bit: allowed key length = 26
        //! 
        //! ASCII:
        //! - 64 bit: allowed key length = 5   
        //! - 128 bit: allowed key length = 13
        ConfIdWepKey1 = 11,
        //! QString: WEP Key for index 2
        ConfIdWepKey2 = 12,
        //! QString: WEP Key for index 3
        ConfIdWepKey3 = 13,
        //! QString: WEP Key for index 4
        ConfIdWepKey4 = 14,
        //! int: (CMManagerShim::WlanWepKeyIndex): Default WEP Key index
        ConfIdWepDefaultIndex = 15,
        //! bool: Is WLAN hidden (CMManagerShim::CmHidden).
        ConfIdHidden = 16,
        //! bool: Does WLAN AP need explicit scanning (CMManagerShim::WlanScanSSID)
        ConfIdWlanScanSSID = 17,
        //! bool: true - Wifi Protected setup is supported, false - not
        ConfIdWpsSupported = 18,
    };
    
public:
    WlanQtUtilsAp();
    WlanQtUtilsAp(const WlanQtUtilsAp &ref);
    ~WlanQtUtilsAp();
    QVariant value(int identifier) const;
    void setValue(int identifier, QVariant value );
    bool operator==(const WlanQtUtilsAp & rhs ) const;
    
    static int compare(
        const WlanQtUtilsAp *ap1,
        const WlanQtUtilsAp *ap2);

private:
    WlanQtUtilsApPrivate *d_ptr;

};

Q_DECLARE_METATYPE(WlanQtUtilsAp)

#endif // STUB_WLANQTUTILSWLANAP_H
