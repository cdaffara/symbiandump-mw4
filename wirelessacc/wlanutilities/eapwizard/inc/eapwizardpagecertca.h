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
 *   EAP Wizard Page: Certificate Authority Selection.
 *
 */

#ifndef EAPWIZARDPAGECERTCA_H
#define EAPWIZARDPAGECERTCA_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbRadioButtonList;
class HbDocumentLoader;
class HbLabel;
class EapWizardPrivate;
class EapQtCertificateInfo;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPageCertCa : public EapWizardPage
{
    Q_OBJECT
public:
    explicit EapWizardPageCertCa(EapWizardPrivate* parent);
    ~EapWizardPageCertCa();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId() const;
   
signals:
        
public slots:

protected:

protected slots:

private:
    static const int IndexForAutomatic = 0;
    static const int AmountOfNonCertItems = 1;

private:
    Q_DISABLE_COPY(EapWizardPageCertCa)

private slots:
    void loadDocmlSection(Qt::Orientation orientation);
    
private: // data
    // OWNED
    
    //! docml document loader 
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! System's CA certificates
    QList<EapQtCertificateInfo> mCerts;
    
    // NOT OWNED
    
    //! Visualization of the page
    HbWidget *mWidget;
    //! Title text for "Select authority certificate"
    HbLabel *mTitle;
    //! visualization of CA certificate list 
    HbRadioButtonList *mCertList;

};

/*! @} */

#endif /* EAPWIZARDPAGECERTCA_H */
