/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN AP (Access Point, unknown network) class.
*/

#ifndef WLANQTUTILSAP_H
#define WLANQTUTILSAP_H

// System includes

#include <QObject>
#include <QMetaType>
#include <QVariant>
#include <QScopedPointer>

// User includes

// Forward declarations

class WlanQtUtilsApPrivate;

// External data types

// Constants

//! Library interface export macro
#ifdef BUILD_WLANQTUTILITIES_DLL
#define WLANQTUTILSAP_EXPORT Q_DECL_EXPORT
#else
#define WLANQTUTILSAP_EXPORT Q_DECL_IMPORT
#endif

// Class declaration

class WLANQTUTILSAP_EXPORT WlanQtUtilsAp
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

    /*!
        Function pointer type for string comparison operation customization.
        Used for AP comparison in \a compare.
        
        @param [in] string1 String #1 to compare.
        @param [in] string2 String #2 to compare.

        @return Zero (0), if strings are considered to be same,
                Positive (>0) if string1 is considered to be "greater than" string2. 
                Negative (<0) if string1 is considered to be "less than" string2. 
    */
    typedef int (*StringComparator)( 
        const QString &string1,
        const QString &string2);
    
    WlanQtUtilsAp();

    WlanQtUtilsAp(const WlanQtUtilsAp &ref);
    
    virtual ~WlanQtUtilsAp();
    
    QVariant value(int identifier) const;
    
    void setValue(int identifier, QVariant value);
    
    static int compare(
        const WlanQtUtilsAp *ap1,
        const WlanQtUtilsAp *ap2,
        StringComparator comparator = 0);
    
signals:

public slots:

protected:

protected slots:

private:

    // Disabling implicit assignment operator
    WlanQtUtilsAp &operator=(const WlanQtUtilsAp &);
    
private slots:

private: // data

    QScopedPointer<WlanQtUtilsApPrivate> d_ptr; //!< Private implementation

    // Friend classes
};

// Metatype support is needed since we use this class with QVariant
Q_DECLARE_METATYPE(WlanQtUtilsAp)

#endif // WLANQTUTILSAP_H
