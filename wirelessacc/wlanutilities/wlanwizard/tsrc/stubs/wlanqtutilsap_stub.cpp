/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * This is the source file for WlanQtUtilsAp class.
 */

#include <QString>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include "wlanqtutilscommon.h"
#include "wlanqtutilsap.h"

class WlanQtUtilsApPrivate
{
    friend class WlanQtUtilsAp;

private:
    QHash<int, QVariant> mConfigurations;
};

WlanQtUtilsAp::WlanQtUtilsAp() :
    d_ptr(new WlanQtUtilsApPrivate())
{
}

WlanQtUtilsAp::WlanQtUtilsAp(const WlanQtUtilsAp &ref) :
    d_ptr(new WlanQtUtilsApPrivate())
{
    d_ptr->mConfigurations = ref.d_ptr->mConfigurations;
}

WlanQtUtilsAp::~WlanQtUtilsAp()
{
    d_ptr->mConfigurations.clear();
    delete d_ptr;
}

QVariant WlanQtUtilsAp::value(int identifier) const
{
    return d_ptr->mConfigurations[identifier];
}

void WlanQtUtilsAp::setValue(int identifier, QVariant value)
{
    d_ptr->mConfigurations[identifier] = value;
}

bool WlanQtUtilsAp::operator==(const WlanQtUtilsAp & rhs ) const
{
    bool ret = true;
    if (d_ptr->mConfigurations.size() != rhs.d_ptr->mConfigurations.size()) {
        qWarning("WlanQtUtilsAp::operator==(): size: expect %d, actual %d ",
            d_ptr->mConfigurations.size(),
            rhs.d_ptr->mConfigurations.size() );
        ret = false;
    }
    QHashIterator<int, QVariant> i(d_ptr->mConfigurations);
    
    while (i.hasNext()) {
        i.next();
        if (!rhs.d_ptr->mConfigurations.contains(i.key())){
            qWarning("WlanQtUtilsAp::operator==(): key not found: %d", i.key());
            ret = false;
            
        }
        if (i.value() != rhs.d_ptr->mConfigurations[i.key()]){
            qWarning("WlanQtUtilsAp::operator==(): values not match %d", i.key());
            qDebug() << "Expect: " << i.value();
            qDebug() << "Actual: " << rhs.d_ptr->mConfigurations[i.key()];
            ret = false;
        }
    }
    return ret;
}

/*!
    AP comparison function.

    @param [in] ap1 First AP to compare.
    @param [in] ap2 Second AP to compare.

    @return Zero(0), if APs are considered to be same.
*/

int WlanQtUtilsAp::compare(
    const WlanQtUtilsAp *ap1,
    const WlanQtUtilsAp *ap2)
{
    int equal = 1; // Not equal
    
    // SSID (case sensitive) and security mode (with or without PSK)
    // are the values, which identify a unique access point.
    if (ap1->value(WlanQtUtilsAp::ConfIdSsid) ==
        ap2->value(WlanQtUtilsAp::ConfIdSsid)
        && ap1->value(WlanQtUtilsAp::ConfIdSecurityMode) == 
           ap2->value(WlanQtUtilsAp::ConfIdSecurityMode)
           && ap1->value(WlanQtUtilsAp::ConfIdWpaPskUse) ==
              ap2->value(WlanQtUtilsAp::ConfIdWpaPskUse)) {
        equal = 0; // Equal
    }
    
    return equal;
}


