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

// System includes

#include <QString>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <cmmanagerdefines_shim.h>

// User includes

#include "wlanqtutilsap.h"

/*!
    \class WlanQtUtilsApPrivate
    \brief Private implementation of WlanQtUtilsAp.
*/

class WlanQtUtilsApPrivate
{
private:
    //! Access Point configuration data
    QHash<int, QVariant> mConfigurations;

    friend class WlanQtUtilsAp;
};

/*!
    \class WlanQtUtilsAp
    \brief WLAN Access Point class.

    Contains the information related to unknown WLAN access points.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

WlanQtUtilsAp::WlanQtUtilsAp() :
    d_ptr(new WlanQtUtilsApPrivate())
{
}

/*!
    Copy constructor.

    @param [in] ref AP to create a copy of.
*/

WlanQtUtilsAp::WlanQtUtilsAp(const WlanQtUtilsAp &ref) :
    d_ptr(new WlanQtUtilsApPrivate())
{
    d_ptr->mConfigurations = ref.d_ptr->mConfigurations;
}

/*!
    Destructor.
*/

WlanQtUtilsAp::~WlanQtUtilsAp()
{
}

/*!
    Getter for AP data.

    @param [in] identifier Identifier of value to get.
    
    @return Value.
*/

QVariant WlanQtUtilsAp::value(int identifier) const
{
    // The configuration must exist
    Q_ASSERT(d_ptr->mConfigurations.contains(identifier));
    Q_ASSERT(d_ptr->mConfigurations[identifier].isValid());
    return d_ptr->mConfigurations[identifier];
}

/*!
    Setter for AP data.

    @param [in] identifier Identifier of value to set.
    @param [in] value Value to set.
*/

void WlanQtUtilsAp::setValue(int identifier, QVariant value)
{
    d_ptr->mConfigurations[identifier] = value;
}

/*!
    AP comparison function. Does AP comparison based on following configs:
    -SSID.
    -Security mode.
    -WPA PSK usage.
    -Connection mode.

    @param [in] ap1 First AP to compare.
    @param [in] ap2 Second AP to compare.
    @param [in] comparator String comparator for SSID.
                Default comparator is QString::compare() direct memory
                comparison which does not take the localization into account.

    @return Zero (0), if APs are considered to be same,
            Negative (>0) if ap1 is considered to be "greater than" ap2. 
            Negative (<0) if ap1 is considered to be "less than" ap2. 
*/

int WlanQtUtilsAp::compare(
    const WlanQtUtilsAp *ap1,
    const WlanQtUtilsAp *ap2,
    StringComparator comparator)
{
    int result = 0;

    // Compare SSID
    QString ssid1 = ap1->value(WlanQtUtilsAp::ConfIdSsid).toString();
    QString ssid2 = ap2->value(WlanQtUtilsAp::ConfIdSsid).toString();
    if (comparator != NULL) {
        result = comparator(ssid1, ssid2);
    } else {
        result = QString::compare(ssid1, ssid2);
    }

    // Compare security mode
    if (result == 0) {
        result = ap1->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt();
        result -= ap2->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt();
    }

    // Compare WPA PSK usage
    int secMode = ap1->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(); 
    if (result == 0 &&
        (secMode == CMManagerShim::WlanSecModeWpa ||
         secMode == CMManagerShim::WlanSecModeWpa2)) {
        // WPA PSK value is boolean, but it can be converted to integer
        result = ap1->value(WlanQtUtilsAp::ConfIdWpaPskUse).toInt();
        result -= ap2->value(WlanQtUtilsAp::ConfIdWpaPskUse).toInt();
    }

    // Compare connection mode
    if (result == 0) {
        result = ap1->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt();
        result -= ap2->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt();
    }

    return result;
}
