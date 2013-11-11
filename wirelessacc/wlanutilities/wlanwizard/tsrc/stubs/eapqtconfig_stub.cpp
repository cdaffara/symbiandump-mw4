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
 *   STUB: EAPWIZARD: EAP method QT configuration
 *
 */

/*
 * %version: 4 %
 */

#include "eapqtconfig.h"
#include <eapqtpluginhandle.h>
#include "eapqtcertificateinfo.h"
#include <QDebug>

//----------------------------------------------------------------------------
//              EapQtConfig                
//----------------------------------------------------------------------------

EapQtConfig::EapQtConfig()
{
}

EapQtConfig::EapQtConfig(const EapQtConfig &other)
{
    mSettings = other.mSettings;
}

EapQtConfig::~EapQtConfig()
{
    // scoped pointer deleted automatically
}

QVariant EapQtConfig::value(const SettingsId id) const
{
    // check for valid range, otherwise memory is consumed for no reason
    if(id >= SettingsIdLast) {
        qDebug("ERROR: EapQtConfig::value - invalid id!");
        return QVariant::Invalid;
    }
    return mSettings[id];
}

void EapQtConfig::setValue(const SettingsId id, const QVariant &newValue)
{
    // check for valid range, otherwise memory is consumed for no reason
    if(id < SettingsIdLast) {
        mSettings[id] = newValue;
    } else {
        qDebug("ERROR: EapQtConfig::setValue - invalid id!");
    }
    return;
}

void EapQtConfig::clear() {
    mSettings.clear();
    return;
}

bool EapQtConfig::operator==(const EapQtConfig & rhs ) const
{
    bool ret = true;
    if (mSettings.size() != rhs.mSettings.size()) {
        qWarning("EapQtConfig::operator==(): size: expect %d, actual %d ",
            mSettings.size(),
            rhs.mSettings.size() );
        ret = false;
    }
    QHashIterator<int, QVariant> i(mSettings);
    
    while (i.hasNext()) {
        i.next();
        if (!rhs.mSettings.contains(i.key())){
            qWarning("EapQtConfig::operator==(): key not found: %d", i.key());
            ret = false;
            
        }
        if (i.key() == EapQtConfig::InnerType) {
            QList<QVariant> list1(mSettings[i.key()].toList());
            QList<QVariant> list2(rhs.mSettings[i.key()].toList());
            
            if (list1.length() != 1 || list1.length() != list2.length()){
                qWarning("Invalid length in EapQtConfig::InnerType");
                ret = false;
            } else {
                EapQtPluginHandle handle1 = list1.at(0).value<EapQtPluginHandle> ();
                EapQtPluginHandle handle2 = list2.at(0).value<EapQtPluginHandle> ();
                if (handle1.pluginId() != handle2.pluginId()){
                    qWarning("Outer: %d", handle1.pluginId());
                    qWarning("Outer: %d", handle2.pluginId());
                    ret = false;
                }
            }
        }
        else if (i.key() == EapQtConfig::OuterType) {
            EapQtPluginHandle handle = mSettings[i.key()].value<EapQtPluginHandle> ();
            EapQtPluginHandle handle2 = rhs.mSettings[i.key()].value<EapQtPluginHandle> ();
            if (handle.pluginId() != handle2.pluginId()){
                qWarning("Outer: %d", handle.pluginId());
                qWarning("Outer: %d", handle2.pluginId());
                ret = false;
            }
        }
        else if (i.key() == EapQtConfig::AuthorityCertificate ||
                 i.key() == EapQtConfig::UserCertificate) {
            QList<QVariant> list1(mSettings[i.key()].toList());
            QList<QVariant> list2(rhs.mSettings[i.key()].toList());
            
            if (list1.length() != 1 || list1.length() != list2.length()) {
                qWarning("Invalid length Certs %d", i.key());
                ret = false;
            } else {
                EapQtCertificateInfo cert1 = list1.at(0).value<EapQtCertificateInfo>();
                EapQtCertificateInfo cert2 = list2.at(0).value<EapQtCertificateInfo>();
                
                if (!(cert1 == cert2)) {
                    ret = false;
                    qWarning("Invalid certs %d", i.key());
                }
            }
        }
        else {
            if (i.value() != rhs.mSettings[i.key()]){
                qWarning("EapQtConfig::operator==(): values does not match %d", i.key());
                qDebug() << "Expect: " << i.value();
                qDebug() << "Actual: " << rhs.mSettings[i.key()];
                ret = false;
            }
        }
    }
    return ret;
}


