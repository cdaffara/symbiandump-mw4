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
 *   EAP-FAST PAC store configuration data
 *
 */

/*
 * %version: 3 %
 */
#include <QDebug>
#include <eapqtpacstoreconfig.h>

EapQtPacStoreConfig::EapQtPacStoreConfig()
{
}

EapQtPacStoreConfig::~EapQtPacStoreConfig()
{
    // scoped pointer deleted automatically
}

QVariant EapQtPacStoreConfig::value(const PacStoreSettings id) const
{
    // check for valid range, otherwise memory is consumed for no reason
    if(id >= PacStoreLast) {
        qDebug("ERROR: EapQtPacStoreConfig::value - invalid id!");
        return QVariant::Invalid;
    }
    return mPacStoreSettings[id];
}

void EapQtPacStoreConfig::setValue(
    const PacStoreSettings id, 
    const QVariant & newValue)
{
    // check for valid range, otherwise memory is consumed for no reason
    if(id < PacStoreLast) {
        mPacStoreSettings[id] = newValue;
    } else {
        qDebug("ERROR: EapQtPacStoreConfig::setValue - invalid id!");
    }
}

void EapQtPacStoreConfig::clear()
{
    mPacStoreSettings.clear();
}

bool EapQtPacStoreConfig::operator==(const EapQtPacStoreConfig & rhs ) const
{
    bool ret = true;
    if (mPacStoreSettings.size() != rhs.mPacStoreSettings.size()) {
        qWarning("EapQtPacStoreConfig::operator==(): size: expect %d, actual %d ",
            mPacStoreSettings.size(),
            rhs.mPacStoreSettings.size() );
        ret = false;
    }
    QHashIterator<int, QVariant> i(mPacStoreSettings);
    
    while (i.hasNext()) {
        i.next();
        if (!rhs.mPacStoreSettings.contains(i.key())){
            qWarning("EapQtPacStoreConfig::operator==(): key not found: %d", i.key());
            ret = false;
            
        }
        if (i.value() != rhs.mPacStoreSettings[i.key()]){
            qWarning("EapQtPacStoreConfig::operator==(): values not match");
            qDebug() << "Expect: " << i.value();
            qDebug() << "Actual: " << rhs.mPacStoreSettings[i.key()];
            ret = false;
        }
    }
    return ret;
}


bool EapQtPacStoreConfig::operator=(const EapQtPacStoreConfig & rhs )
{
    mPacStoreSettings = rhs.mPacStoreSettings;
}
