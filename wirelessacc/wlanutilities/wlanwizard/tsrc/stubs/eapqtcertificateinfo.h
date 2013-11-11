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
* Stub file for eapwizard!!
*/

#ifndef EAPQTCERTIFICATEINFO_H
#define EAPQTCERTIFICATEINFO_H

// System includes
#include <QMetaType>
#include <QVariant>

// User includes

// Forward declarations
class EapQtCertificateInfoPrivate;

// External data types

// Constants

// Class declaration
class EapQtCertificateInfo
{
public:

    // Data types

    enum ItemId
    {
        /*! QString */
        SubjectName = 0,
        /*! QString */
        IssuerName,
        /*! QString */
        SerialNumber,
        /*! QByteArray */
        SubjectKeyId,
        /*! QString */
        ThumbPrint,
        /*! QString */
        CertificateLabel,
        /* marker for the last item */
        ItemIdLast
    };

public:

    EapQtCertificateInfo();
    ~EapQtCertificateInfo();

    EapQtCertificateInfo(const EapQtCertificateInfo &certInfo);
    EapQtCertificateInfo &operator=(const EapQtCertificateInfo &certInfo);

    QVariant value(const ItemId id) const;
    void setValue(const ItemId id, const QVariant &newValue);

public: // FOR TESTING PURPOSE
    bool operator==(const EapQtCertificateInfo & rhs) const;
    
private:
    QScopedPointer<EapQtCertificateInfoPrivate> d_ptr;
};

// Make the class known to QMetaType to support using QVariant
Q_DECLARE_METATYPE(EapQtCertificateInfo)

#endif // EAPQTCERTIFICATEINFO_H 
