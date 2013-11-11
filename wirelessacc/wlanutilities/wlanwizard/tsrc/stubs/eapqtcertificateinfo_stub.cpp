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
 *   Certificate information data structure for EAP QT configuration interface
 *
 */

/*
 * %version: 3 %
 */

#include "eapqtcertificateinfo.h"
#include <QDebug>
class EapQtCertificateInfoPrivate
{
    friend class EapQtCertificateInfo; 
public:

    EapQtCertificateInfoPrivate() {};
    ~EapQtCertificateInfoPrivate() {};
    
    // copy constructor
    EapQtCertificateInfoPrivate(const EapQtCertificateInfoPrivate &certInfo)
    {
        mCerts = certInfo.mCerts;
    }

    QVariant value(int id);
    void setValue(int id, QVariant newValue);

private:
    // disable assignment
    EapQtCertificateInfoPrivate &operator=(const EapQtCertificateInfoPrivate&);
    QHash<int, QVariant> mCerts;

};


//----------------------------------------------------------------------------
//              EapQtCertificateInfo                
//----------------------------------------------------------------------------


EapQtCertificateInfo::EapQtCertificateInfo() :
    d_ptr(new EapQtCertificateInfoPrivate)
{
}

EapQtCertificateInfo::~EapQtCertificateInfo()
{
    // scoped pointer deleted automatically
}

EapQtCertificateInfo::EapQtCertificateInfo(const EapQtCertificateInfo &certInfo) :
    d_ptr(new EapQtCertificateInfoPrivate(*(certInfo.d_ptr)))
{
}

EapQtCertificateInfo& EapQtCertificateInfo::operator=(const EapQtCertificateInfo &certInfo)
{
    // check if assigning to myself
    if (this != &certInfo) {
        d_ptr.reset(new EapQtCertificateInfoPrivate(*(certInfo.d_ptr)));
    }
    return *this;
}

QVariant EapQtCertificateInfo::value(const ItemId id) const
{
    // check for valid range, otherwise memory is consumed for no reason
    if(id >= ItemIdLast) {
        qDebug("ERROR: EapQtCertificateInfo::value - invalid id!");
        return QVariant::Invalid;
    }
    return d_ptr->mCerts[id];
}

void EapQtCertificateInfo::setValue(const ItemId id, const QVariant &newValue)
{
    // check for valid range, otherwise memory is consumed for no reason
    if(id < ItemIdLast) {
        d_ptr->mCerts[id] = newValue;
    } else {
        qDebug("ERROR: EapQtCertificateInfo::setValue - invalid id!");
    }
    return;
}

bool EapQtCertificateInfo::operator==(const EapQtCertificateInfo & rhs ) const
{
    bool ret = true;
    if (d_ptr->mCerts.size() != rhs.d_ptr->mCerts.size()) {
        qWarning("EapQtCertificateInfo::operator==(): size: expect %d, actual %d ",
            d_ptr->mCerts.size(),
            rhs.d_ptr->mCerts.size() );
        ret = false;
    }
    QHashIterator<int, QVariant> i(d_ptr->mCerts);
    
    while (i.hasNext()) {
        i.next();
        if (!rhs.d_ptr->mCerts.contains(i.key())){
            qWarning("EapQtCertificateInfo::operator==(): key not found: %d", i.key());
            ret = false;
            
        }
        if (i.value() != rhs.d_ptr->mCerts[i.key()]){
            qWarning("EapQtCertificateInfo::operator==(): values not match");
            qDebug() << "Expect: " << i.value();
            qDebug() << "Actual: " << i.value();
            ret = false;
        }
    }
    return ret;
}
